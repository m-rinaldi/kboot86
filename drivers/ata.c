#include <ata.h>

#include <stdint.h>
#include <io.h>
#include <stdbool.h>
#include <string.h>

//XXX
#include <kstdio.h>

// TODO IDENTIFY
// TODO read_lba

// Ports for routing the registers
#define DATA        0x1f0 // [r/w]  data register
#define ERR         0x1f1 // [r]    error register
#define FEATURES    0x1f1 // [w]    features register
#define SEC_CNT     0x1f2 // [r/w]  how many sectors to read/write
#define SEC_NUM     0x1f3 // [r/w]  sector number   / LBAlo
#define CYL_LO      0x1f4 // [r/w]  cylinder low    / LBAmid
#define CYL_HI      0x1f5 // [r/w]  cylinder high   / LBAhi
#define DRV_HD      0x1f6 // [r/w]  drive/head
#define CMD         0x1f7 // [w]    command port
#define STATUS      0x1f7 // [r]    status register
#define CONTROL     0x3f6 // [w]    control register
#define ASTATUS     0x3f6 // [r]    alternate status register

typedef struct {
    // 0-3  head select bits
    unsigned int head_sel   : 4;

    // 4    drive select
    //          0 -> drive select 0
    //          1 -> drive select 1
    unsigned int drive_sel  : 1;
   
    // 5    set to one
    unsigned int one0       : 1;
   
    // 6    set to zero
    unsigned int zero       : 1;
   
    // 7     set to one
    unsigned int one1       : 1;
} __attribute__((packed)) ata_drv_hd_t;


typedef struct {
    // 0    ERR: previous command ended in an error
    unsigned int  err   : 1;
    
    //  1 IDX
    unsigned int idx    : 1;

    // 2    CORR
    unsigned int corr   : 1; 

    // 3    DRQ: set when the drive has PIO data to transfer
    //           or is ready to accept PIO data
    unsigned int drq    : 1; 

    // 4    SRV: overlapped mode service request
    unsigned int srv    : 1;

    // 5    DF: Drive Fault Error (does not set ERR)
    unsigned int df     : 1;

    // 6    RDY: drive is ready
    unsigned int rdy    : 1;
 
    // 7    BSY: controller executing a command 
    //           wait for it to clear, in case of hang => sw reset
    unsigned int bsy    : 1; 
} __attribute__((packed)) ata_status_t;


typedef struct {
    unsigned int        : 1;
    // 1    nIEN enable bit for the drive interrupt to the host
    //              0 -> INTRQ enabled
    //              1 -> INTRQ disabled
    unsigned int n_ien   : 1;

    // 2    host software reset bit
    //      when set to 1 the drive is reset
    unsigned int srst   : 1;
    unsigned int        : 4;
    unsigned int hob    : 1;
} __attribute__((packed)) ata_control_t;



// commands:
#define CMD_FT      0x50    // format track
#define CMD_RSWR    0x20    // read sectors with retry
#define CMD_RSWOR   0x21    // read sectors without retry
#define CMD_RLWR    0x22    // read long with retry
#define CMD_RLWOR   0x23    // read long without retry
#define CMD_WSWR    0x30    // write sectors with retry
#define CMD_WSWOR   0x31    // write sectors without retry
#define CMD_WLWR    0x32    // write long with retry
#define CMD_WLWOR   0x32    // write long without retry

static void _set_control(ata_control_t control)
{
    uint8_t reg;

    memcpy(&reg, &control, sizeof(control));
    outb(CONTROL, reg);
}

static ata_status_t _read_status(void)
{
    ata_status_t status;
    uint8_t reg;

    reg = inb(STATUS);
    memcpy(&status, &reg, sizeof(reg));

    return status;   
}

static bool _is_busy(void)
{
    ata_status_t status;

    status = _read_status();
    return status.bsy;
}

// check BSY & DRQ before trying to send a command

static void _reset(void)
{
    ata_control_t control;

    memset(&control, 0, sizeof(control)); 

    control.srst = 1;
    _set_control(control);

    // TODO introduce a delay??

    // the reset bit has to be cleared manually
    control.srst = 0;
    _set_control(control);

    
    while (_is_busy())
        ;
}

int ata_init(void)
{
    _reset();
    return 0;
}

static ata_drv_hd_t _read_drv_hd(void)
{
    ata_drv_hd_t drv_hd;
    uint8_t reg;

    reg = inb(DRV_HD);
    memcpy(&drv_hd, &reg, sizeof(reg));

    return drv_hd;
}

static void _write_drv_hd(ata_drv_hd_t drv_hd)
{
    uint8_t reg;

    memcpy(&reg, &drv_hd, sizeof(reg));
    outb(DRV_HD, reg); 
}

static uint16_t _read_data(void)
{
    ata_status_t status;

    // TODO timeout
    do { 
        status = _read_status();
    } while (!status.drq);

    return inw(DATA);
}


static int _send_cmd(unsigned int cmd)
{
    ata_status_t status;

    // check DRQ bit before issuing a command
    status = _read_status();
    if (status.drq)
        return 1;

    // TODO timeout
    while (_is_busy())
        ;

    outb(CMD, cmd);

    return 0;
}

static int _select_head(unsigned int head_num)
{
    ata_drv_hd_t drv_hd;
    
    if (head_num > 15)
        return 1;

    drv_hd = _read_drv_hd();
    drv_hd.head_sel = head_num;
    _write_drv_hd(drv_hd);

    return 0;
}

// TODO disable optimizations for this function
int ata_select_drive(unsigned int drive_num)
{
    ata_drv_hd_t drv_hd;

    if (drive_num > 1)
        return 1;

    drv_hd = _read_drv_hd();
    drv_hd.drive_sel = drive_num;
    _write_drv_hd(drv_hd);

    // read 4 times the status register in order to achieve the 400ns delay 
    // before reading the actual value of the status register
    {
        int i;
        for (i = 0; i < 4; i++)
            (void) _read_status(); // TODO optimized away???
    }

    // TODO ensure drive is RDY before returning from here
    // ??
    {
        ata_status_t status;

        // TODO timeout
        do { 
            status = _read_status();
        } while (!status.rdy);
    
        if (status.err)
            return 1;
    }

    return 0;
}

// TODO define ata_sector_t type
// TODO write an ata_read_lba_sector() function instead
int ata_read_chs_sector(unsigned int cyl_num, unsigned int head_num,
                        unsigned int sec_num, void *buf)
{
    if (cyl_num > 1023)
        return 1;

    if (head_num > 15)
        return 1;

    if (sec_num < 1 || sec_num > 256)
        return 1;

    if (_select_head(head_num))
        return 1;

    // one sector to read
    outb(SEC_CNT, 1);
    outb(SEC_NUM, sec_num);
    outb(CYL_LO, cyl_num & 0xff);
    // cylinder high (2 bits)
    outb(CYL_HI, (cyl_num & 0x300) >> 8); 

    if (_send_cmd(CMD_RSWOR))
        return 1;

    // wait until the sector buffer requires service
    {
        ata_status_t status;

        do {
            status = _read_status();
        } while (!status.drq || status.bsy);
    
        if (status.err)
            return 1;
    }
   
    // read sector buffer data 
    {
        unsigned int i;
        uint16_t *data;

        // TODO replace magic number with sizeof() operator for the new type 
        for (i = 0, data = buf; i < 512 / sizeof(*data); i++, data++)
            *data = _read_data(); 
    }

    return 0;
}


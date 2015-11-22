AS = nasm
TARGET = floppy.img

.PHONY: all clean

all: $(TARGET)

$(TARGET) : boot0.bin boot1.bin padding.bin
	@echo -n generating floppy image...
	@cat boot0.bin boot1.bin padding.bin > floppy.img
	@echo done

padding.bin :
	@echo -n "generating the padding for the floppy image..."
	@dd if=/dev/zero of=./padding.bin bs=512 count=2871 2>/dev/null
	@echo done

boot1.bin : boot1.asm
	@echo -n "compiling the 2nd stage bootloader: $<..."
	@$(AS) $< -f bin -o $@
	@echo done

boot0.bin : boot0.asm
	@echo -n "compiling the 1st stage bootloader: $<..."
	@$(AS) $< -f bin -o $@
	@echo done

clean:
	@echo -n cleaning...
	@rm -f *.bin *.img *.log
	@echo done

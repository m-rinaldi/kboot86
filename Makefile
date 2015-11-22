AS = nasm
TARGET = floppy.img

.PHONY: all clean

all: $(TARGET)

$(TARGET) : boot0.bin boot1.bin floppy.pad track0.pad
	@echo -n generating floppy image...
	@cat boot0.bin boot1.bin track0.pad floppy.pad > floppy.img
	@echo done

floppy.pad track0.pad :
	@echo -n "generating the padding for 1st track of the floppy..."
	@dd if=/dev/zero of=./track0.pad bs=512 count=9 2>/dev/null
	@echo done
	@echo -n "generating the padding for the floppy image..."
	@dd if=/dev/zero of=./floppy.pad bs=512 count=2862 2>/dev/null
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
	@rm -f *.bin *.pad *.img *.log
	@echo done

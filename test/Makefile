INCLUDE = ../include/
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
OBJ += $(patsubst %.S, %.o, $(wildcard *.S))

TARGET = test.o

.PHONY: all clean

all: $(TARGET)

$(TARGET) : $(OBJ)
	@echo -n "  generating $@..."
	@$(LD) --relocatable $(OBJ) -o $@
	@echo done

%.o : %.c
	@echo -n "  compiling $<..."
	@$(CC) $(CFLAGS) -I $(INCLUDE) $< -o $@
	@echo done

%.o : %.S
	@echo -n "  assembling $<..."
	@$(AS) $< -o $@
	@echo done
	

clean:
	@echo -n "  cleaning..."
	@rm -f *.o
	@echo done

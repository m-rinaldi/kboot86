CPPFLAGS = -I ../include/

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

TARGET = $(shell basename $$(pwd)).o

.PHONY: all clean

all: $(TARGET)

include ../common.mk

$(TARGET): $(OBJ)
	@echo -n "  generating $@..."
	@$(LD) --relocatable $(OBJ) -o $@
	@echo done

clean:
	@echo -n "  cleaning..."
	@rm -f *.o
	@echo done

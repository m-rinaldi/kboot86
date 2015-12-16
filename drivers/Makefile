INCLUDE = ../include/
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

.PHONY: all clean

all: drivers.o

drivers.o : $(OBJ)
	@echo -n "  generating $@..."
	@$(LD) --relocatable $(OBJ) -o $@
	@echo done

%.o : %.c
	@echo -n "  compiling $<..."
	@$(CC) $(CFLAGS) -I $(INCLUDE) $< -o $@
	@echo done 

clean:
	@echo -n "  cleaning..."
	@rm -f *.o
	@echo done

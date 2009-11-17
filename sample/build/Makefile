##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Sep 20 00:28:27 2009 texane
## Last update Tue Nov 17 20:02:28 2009 texane
##


# http://www.nongnu.org/avr-libc/user-manual/group__demo__project.html


AVR_ROOT:=	/opt/avr

CC	:=	$(AVR_ROOT)/bin/avr-gcc
CFLAGS	:=	-g -Os -mmcu=atmega128
AS	:=	$(AVR_ROOT)/bin/avr-as
ASFLAGS	:=
LD	:=	$(AVR_ROOT)/bin/avr-gcc
LDFLAGS	:=
OBJCOPY	:=	$(AVR_ROOT)/bin/avr-objcopy -j .text -j .data -O ihex


HEX	:=	main.hex
ELF	:=	main.elf

CSRCS	:=	../src/main.c

OBJS	:=	$(CSRCS:.c=.o)

# rules

all	:	$(HEX)

$(ELF)	:	$(OBJS)
		$(LD) $(LDFLAGS) -o $(ELF) $(OBJS) $(LIBS)

%.o	:	%.c
		$(CC) $(CFLAGS) -c -o $(@) $(<)

%.o	:	%.asm
		$(AS) $(ASFLAGS) -c -o $(@) $(<)

%.hex	:	%.elf
		$(OBJCOPY) $(<) $(@)

PHONY	:	clean fclean re write erase test

clean	:
		$(RM) $(OBJS)

fclean	:	clean
		$(RM) $(HEX)
		$(RM) $(ELF)

re	:	fclean all

write	:	$(HEX)
		avrdude -p m128 -c stk500v1 -b 9600 -P /dev/ttyUSB0 -v  -U flash:w:$(HEX)

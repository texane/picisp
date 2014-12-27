##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Sep 20 00:28:27 2009 texane
## Last update Tue Nov 17 20:30:27 2009 texane
##


PATH	:=	$(PATH):/usr/share/pk2


CC	?=	/home/texane/proj/present/pic/18f4550/tools/sdcc/bin/sdcc
CFLAGS	:=	-mpic16						\
		--use-non-free					\
		-p18f4550 -Dpic18f4550				\
		-DSDCC_PIC18					\
		-DCONFIG_ENABLE_SERIAL				\
		-I../src					\
		 -Wa -DSDCC

AS	:=	gpasm
ASFLAGS	:=	-q -p18f4550 -Dpic18f4550 			\
		-I /usr/share/gputils/header 			\
		-DSDCC

LD	:=	gplink
LDFLAGS	:=	-w -r --map -s ../src/18f4550.lkr		\
		-I/usr/local/share/sdcc				\
		-I/usr/local/share/sdcc/lib/pic16		\
		-I/usr/local/share/sdcc/non-free/lib/pic16	\
		-I/usr/local/share/sdcc/lib/large
LIBS	:=	libdev18f4550.lib libsdcc.lib libc18f.lib

LIB	:=	gplib -c

NAME	:=	picisp.hex

CSRCS	:=	../src/main.c			\
		../src/osc.c			\
		../src/int.c			\
		../src/serial.c			\
		../src/spi.c			\
		../src/stk500.c			\
		../src/gptrput2.c		\
		../src/eeprom8_gptrput2.c	\
		../src/eeprom8_write.c		\
		../src/gptrput1.c		\
		../src/eeprom8_gptrput1.c	\
		../src/gptrget1.c		\
		../src/eeprom8_gptrget1.c	\
		../src/gptrget2.c		\
		../src/eeprom8_gptrget2.c	\
		../src/mullong.c		\
		../src/divslong.c		\
		../src/divulong.c

ASRCS	:=

OBJS	:=	$(CSRCS:.c=.o)			\
		$(ASRCS:.S=.o)

GARBAGE	:=	$(NAME:.hex=.lst)		\
		$(NAME:.hex=.cod)		\
		$(CSRCS:.c=.lst)		\
		$(CSRCS:.c=.cod)		\
		$(CSRCS:.c=.asm)

# rules

all	:	$(NAME)

$(NAME)	:	$(OBJS)
		$(LD) $(LDFLAGS) -o $(NAME) $(OBJS) $(LIBS)

%.o	:	%.c
		$(CC) $(CFLAGS) -c -o $(@) $(<)

%.o	:	%.asm
		$(AS) $(ASFLAGS) -c -o $(@) $(<)

PHONY	:	clean fclean re write erase test

clean	:
		$(RM) $(OBJS)
		$(RM) $(GARBAGE)

fclean	:	clean
		$(RM) $(NAME)
		$(RM) picisp.map

re	:	fclean all

write	:	$(NAME)
		pk2cmd -PPIC18f4550 -M -F$(NAME)
erase	:
		pk2cmd -PPIC18F4550 -E
verify	:
		pk2cmd -PPIC18f4550 -Y -F$(NAME)

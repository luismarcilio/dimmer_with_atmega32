TARGET = bluetooth.a


SRCS = \
btusb.c \
hci.c \
debug.c \
l2cap.c \
hidbt.c


OBJS = $(SRCS:.c=.o)

CC = gcc -c -DDEBUG

LD = ld -o 

CFLAGS = -g 

AR = ar r

RANLIB=ranlib


.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

all: $(TARGET)

# The variable $@ has the value of the target. In this case $@ = psort
$(TARGET): $(OBJS)
	$(AR) $@  $(OBJS) 
	$(RANLIB) $@

clean:
	rm -f $(TARGET) *.o 





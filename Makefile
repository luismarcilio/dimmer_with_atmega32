TARGET = main


SRCS = main.c

ARCHIVES =  bluetooth/bluetooth.a


OBJS = $(SRCS:.c=.o) 

CC = gcc -c -DDEBUG

LD = gcc -lusb-1.0 -lpthread -o 


CFLAGS = -g -Ibluetooth


LIBS =  


.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

all: $(TARGET)

$(ARCHIVES):
	make -C bluetooth

# The variable $@ has the value of the target. In this case $@ = psort
$(TARGET): $(OBJS) $(ARCHIVES)
	$(LD) $@  $(OBJS) $(ARCHIVES) 

clean:
	make -C bluetooth clean
	rm -f $(TARGET) *.o 







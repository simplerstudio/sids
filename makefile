CC = gcc
TARGET = sids_client
OBJS = client.o tcp_config.o hex_info.o rtu_config.o

CFLAGS = -Wall
LDFLAGS = -lpthread

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) && make clean

.c.o:
	$(CC) -c -o $@ $<


clean :
	rm -f $(OBJS)

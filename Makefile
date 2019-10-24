CFLAGS=

all:
	gcc $(CFLAGS) -c util.c
	gcc $(CFLAGS) -c udev-builtin-net_id.c
	gcc $(CFLAGS) -o net-id-8 udev-builtin-net_id.o util.o -ludev
clean:
	rm -f *.o

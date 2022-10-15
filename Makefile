CFLAGS=-O0 -g -march=native
LDFLAGS=-lgmp

all: birthday
birthday: birthday.c
	$(CC) -o birthday $(CFLAGS) birthday.c $(LDFLAGS)
clean:
	rm -f birthday

.PHONY: all clean

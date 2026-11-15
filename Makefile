CC = gcc
CFLAGS = -Wall -Wextra -g

banker: banker.c
	$(CC) $(CFLAGS) -o banker banker.c

clean:
	rm -f banker

.PHONY: clean
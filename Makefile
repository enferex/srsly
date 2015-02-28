APP = srsly
CC = gcc
CFLAGS = -g3 -Wall -pedantic -std=c99 -D_DEFAULT_SOURCE $(EXTRA_CFLAGS)

$(APP): main.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	$(RM) $(APP)

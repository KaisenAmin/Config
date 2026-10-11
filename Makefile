CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: main

main: main.c Config.c Config.h
	$(CC) $(CFLAGS) -o $@ main.c Config.c

tests/test_config: tests/test_config.c Config.c Config.h
	$(CC) $(CFLAGS) -o $@ tests/test_config.c Config.c

test: tests/test_config
	./tests/test_config

clean:
	rm -f main main.exe tests/test_config tests/test_config.exe config.ini config.ini.bak

.PHONY: all test clean

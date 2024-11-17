CC = gcc

CFLAGS = -Wall -Wextra -O2

SRC = src/main.c src/aux.c src/args.c

OUT = relocdoctor

all:
	$(CC) $(SRC) -o $(OUT) $(FLAGS)

debug:
	$(CC) $(SRC) -o $(OUT) $(FLAGS) -g

clean:
	rm $(out)

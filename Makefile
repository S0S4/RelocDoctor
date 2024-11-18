CC = gcc

CFLAGS = -Wall -Wextra -O2

SRC = source/main.c source/aux.c source/args.c

OUT = relocdoctor

all:
	$(CC) $(SRC) -o $(OUT) $(FLAGS)

debug:
	$(CC) $(SRC) -o $(OUT) $(FLAGS) -g

clean:
	rm $(out)

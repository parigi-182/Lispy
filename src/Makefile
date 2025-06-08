CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g

SRC = lispy.c core.c lenv_lib.c lval_lib.c mpc.c

TARGET = lispy
LIBS = -ledit

.PHONY: all clean

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

cleanrun: clean run
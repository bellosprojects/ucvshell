CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -Iinclude
TARGET = bin/test_path
SRCS = src/main.c src/path_utils.c
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c include/%.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/main.o: src/main.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj bin

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
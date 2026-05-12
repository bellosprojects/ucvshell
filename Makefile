CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -Iinclude
TARGET = bin/ucvsh

# Archivos fuente
SRCS = src/main.c src/shell_loop.c src/path_utils.c
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

# Regla principal
$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

# Compilar .c a .o
obj/%.o: src/%.c include/%.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Reglas especiales para archivos sin header propio
obj/main.o: src/main.c include/shell_loop.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/shell_loop.o: src/shell_loop.c include/shell_loop.h include/path_utils.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/path_utils.o: src/path_utils.c include/path_utils.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar
clean:
	rm -rf obj bin

# Ejecutar
run: $(TARGET)
	./$(TARGET)

# Útil para depurar
valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

.PHONY: clean run valgrind
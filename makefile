
CC = gcc

OBJ_DIR := obj
INC_DIR := include
BIN_DIR := bin
SRC_DIR := src

CFLAGS := -g -I$(INC_DIR)
DEPFLAGS := -MMD -MP

SRCS := $(wildcard $(SRC_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET := $(BIN_DIR)/ucvsh

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(OBJ_DIR)/*.d

clean:
	@rm -rf $(OBJ_DIR)

run: $(TARGET)
	./bin/ucvsh
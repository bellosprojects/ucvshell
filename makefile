OBJ_DIR = obj
INC_DIR = include
BIN_DIR = bin
SRC_DIR = ./src
CFLAGS = -g -Wall -I$(INC_DIR)

#forma IA SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS = main.c historial.c dequeue.c path_utils.c shell_loop.c trim.c command_utils.c ast.c

#Forma IA: $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)               
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/historial.o $(OBJ_DIR)/dequeue.o  $(OBJ_DIR)/path_utils.o $(OBJ_DIR)/shell_loop.o $(OBJ_DIR)/trim.o $(OBJ_DIR)/command_utils.o $(OBJ_DIR)/ast.o

TARGET = $(BIN_DIR)/ucvsh

all : $(TARGET)

$(TARGET) : $(OBJS)
	mkdir -p $(BIN_DIR)
	gcc $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc $(CFLAGS) -c -MD $< -o $@ 

-include $(OBJ_DIR)/*.d

clean :
#borramos carpeta objs (.o y .d) y el ejecutable
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

TARGET = $(BIN_DIR)/p1

OBJ_DIR = obj
BIN_DIR = bin
INCDIR = inc
SRC_DIR = src
LAK_NAME = lakemem

OBJS = $(OBJ_DIR)/p1.o \
	   $(OBJ_DIR)/List.o
CFLAGS = -g -Wall -pedantic -Werror -I$(INCDIR)

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c -MD $(CFLAGS) $< -o $@

-include $(OBJ_DIR)/*.d

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	gcc $(CFLAGS) -o $(TARGET) $(OBJS)

.PHONY: clean
clean:
	@rm -r $(OBJ_DIR) $(BIN_DIR) $(LAK_NAME)

memcheck:
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file=$(LAK_NAME) ./$(TARGET)

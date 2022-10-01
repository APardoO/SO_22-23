TARGET = $(BIN_DIR)/p1

OBJ_DIR = obj
BIN_DIR = bin
INCDIR = inc
SRC_DIR = src

OBJS = $(OBJ_DIR)/p1.o \
	   $(OBJ_DIR)/List.o
CFLAGS = -std=c99 -g -Wall -pedantic -Werror -I$(INCDIR)

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
	@rm -r $(OBJ_DIR) $(BIN_DIR)

TARGET = $(BIN_DIR)/shell

OBJ_DIR = obj
BIN_DIR = bin
INCDIR = inc
SRC_DIR = src
TEST_DIR = test

DELIVERY_DIR = delivery

LAK_NAME = $(TEST_DIR)/lakemem

OBJS = $(OBJ_DIR)/main.o \
	   $(OBJ_DIR)/Sys_module.o \
	   $(OBJ_DIR)/p0.o \
	   $(OBJ_DIR)/p1.o \
	   $(OBJ_DIR)/p2.o \
	   $(OBJ_DIR)/p3.o \
	   $(OBJ_DIR)/List.o

#CFLAGS = -g -Wall -Wextra -Werror -I$(INCDIR)
CFLAGS = -g -Wall -Werror -I$(INCDIR)

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc -c -MD $(CFLAGS) $< -o $@

-include $(OBJ_DIR)/*.d

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR) $(TEST_DIR)
	touch ./$(TEST_INPUT_FILE) 2>/dev/null
	gcc $(CFLAGS) -o $(TARGET) $(OBJS)

.PHONY: clean

clean:
	@rm -r $(OBJ_DIR) $(BIN_DIR) $(LAK_NAME) $(DELIVERY_DIR) 2>/dev/null

# Comprobación de memoria -> salida por pantalla
memcheck_ctn:
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Comprobación de memoria -> salida al archivo test/lakemem
memcheck_out:
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file=./$(LAK_NAME) ./$(TARGET)

# Directorio con la entrega
delivery: $(TARGET)
	mkdir -p $(DELIVERY_DIR)
	cp -r $(SRC_DIR)/* $(DELIVERY_DIR)
	cp -r $(INCDIR)/* $(DELIVERY_DIR)
	cp -r $(TARGET) $(DELIVERY_DIR)

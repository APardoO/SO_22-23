TARGET = $(BIN_DIR)/p2

OBJ_DIR = obj
BIN_DIR = bin
INCDIR = inc
SRC_DIR = src
TEST_DIR = test

LAK_NAME = $(TEST_DIR)/lakemem
TEST_INPUT_FILE = $(TEST_DIR)/test_in

REFER_OUTPUT_FILE = $(TEST_DIR)/refer_out.tmp
USER_OUTPUT_FILE = $(TEST_DIR)/user_out.tmp
DIFFER_OUT = $(TEST_DIR)/differences

REFER_TARGET = $(TEST_DIR)/shell

OBJS = $(OBJ_DIR)/p2.o \
	   $(OBJ_DIR)/List.o

CFLAGS = -g -Wall -pedantic -Werror -I$(INCDIR)


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
	@rm -r $(OBJ_DIR) $(BIN_DIR) $(LAK_NAME) $(USER_OUTPUT_FILE) $(REFER_OUTPUT_FILE) $(DIFFER_OUT) 2>/dev/null

memcheck:
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file=./$(LAK_NAME) ./$(TARGET) <./$(TEST_INPUT_FILE)

check:
	@./$(REFER_TARGET) 2>&1 1>$(REFER_OUTPUT_FILE) <./$(TEST_INPUT_FILE)
	@./$(TARGET) 2>&1 1>$(USER_OUTPUT_FILE) <./$(TEST_INPUT_FILE)
	@diff ./$(USER_OUTPUT_FILE) ./$(REFER_OUTPUT_FILE) 2>&1 1>./$(DIFFER_OUT)
	@rm -r ./$(USER_OUTPUT_FILE) ./$(REFER_OUTPUT_FILE) 2>/dev/null
	@cat ./$(DIFFER_OUT)
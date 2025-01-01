CC = gcc # Flag for implicit rules
CFLAGS = -g -Wall -Wextra -Iinclude # Flag for implicit rules. Turn on debug info

# Source and target
BIN_DIR = bin
SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
OBJ_DIR = $(BIN_DIR)/obj
TARGET = $(BIN_DIR)/jsh

# Collect all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)

# Collect all object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c,$(OBJ_DIR)/lib_%.o,$(LIB_SRCS))

# Default rule
all: $(TARGET)

# Linking the target
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $(OBJS) $(LIB_OBJS) -o $(TARGET)

# Compiling source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compiling library source files to object files
$(OBJ_DIR)/lib_%.o: $(LIB_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up object files and the binary
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean


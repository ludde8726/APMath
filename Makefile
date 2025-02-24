CC := clang
LDFLAGS := -shared
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test
LIB_NAME := libapmath.so

CFLAGS := -fPIC -Wall -Wextra -Werror -g -I$(SRC_DIR)

# Find all .c files in src and test directories
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))
TEST_FILES := $(wildcard $(TEST_DIR)/*.c)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the shared library
$(BUILD_DIR)/$(LIB_NAME): $(OBJ_FILES) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(BUILD_DIR)/$(LIB_NAME)

# Run tests
test: all
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test_runner $(TEST_FILES) -L$(BUILD_DIR) -lapmath
	LD_LIBRARY_PATH=$(BUILD_DIR) ./$(BUILD_DIR)/test_runner

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean

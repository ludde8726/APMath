# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -O2 -g -MMD -MP
LDFLAGS = 

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj

# Output binary
TARGET = $(BIN_DIR)/apmath

# Find all C source files in the src/ directory
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Dependency files
DEPS := $(OBJECTS:.o=.d)

# Default target
all: $(TARGET)

# Create the build directory structure
$(BUILD_DIR):
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Build the executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Compile source files to object files and generate dependencies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files if they exist
-include $(DEPS)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(TARGET)

.PHONY: all clean

CC := gcc
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin

SRC_DIR := src
TEST_DIR := test
LIB_NAME := libapm

CFLAGS := -fPIC -Wall -Wextra -Werror -std=c11 -pedantic -I$(SRC_DIR)

APCTX := $(SRC_DIR)/APCtx.c $(SRC_DIR)/APCtx.h
APERR := $(SRC_DIR)/APError.c $(SRC_DIR)/APError.h
APNUM := $(SRC_DIR)/APNumber.c $(SRC_DIR)/APNumber.h
APINT := $(SRC_DIR)/APIntOps.c $(SRC_DIR)/APIntOps.h
APFLO := $(SRC_DIR)/APFloatOps.c $(SRC_DIR)/APFloatOps.h
APHEL := $(SRC_DIR)/APHelpers.c $(SRC_DIR)/APHelpers.h

.PHONY: all clean test

static: $(LIB_DIR)/$(LIB_NAME).a
dynamic: $(LIB_DIR)/$(LIB_NAME).so

all: static dynamic test

clean:
	rm -rf $(BUILD_DIR)

$(LIB_DIR): 
	mkdir -p $(LIB_DIR)
$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(LIB_DIR)/$(LIB_NAME).a: $(OBJ_DIR)/APCtx.o $(OBJ_DIR)/APError.o $(OBJ_DIR)/APFloatOps.o $(OBJ_DIR)/APHelpers.o $(OBJ_DIR)/APIntOps.o $(OBJ_DIR)/APNumber.o | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_DIR)/$(LIB_NAME).so: $(OBJ_DIR)/APCtx.o $(OBJ_DIR)/APError.o $(OBJ_DIR)/APFloatOps.o $(OBJ_DIR)/APHelpers.o $(OBJ_DIR)/APIntOps.o $(OBJ_DIR)/APNumber.o | $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(OBJ_DIR)/APCtx.o: $(APCTX) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/APCtx.c -o $@

$(OBJ_DIR)/APError.o: $(APERR) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/APError.c -o $@

$(OBJ_DIR)/APNumber.o: $(APNUM) $(APCTX) $(APERR) $(APINT) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/APNumber.c -o $@

$(OBJ_DIR)/APIntOps.o: $(APNUM) $(APCTX) $(APERR) $(APINT) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/APIntOps.c -o $@

$(OBJ_DIR)/APFloatOps.o: $(APNUM) $(APCTX) $(APERR) $(APINT) $(APFLO) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/ApFloatOps.c -o $@

$(OBJ_DIR)/APHelpers.o: $(APNUM) $(APERR) $(APHEL) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/APHelpers.c -o $@


TEST_FILES := $(TEST_DIR)/tests.c $(TEST_DIR)/test_apint_ops.c $(TEST_DIR)/test_apfloat_ops.c
TEST_HEADERS := $(TEST_DIR)/test_apint_ops.h $(TEST_DIR)/test_apfloat_ops.h

$(BIN_DIR)/test_runner: $(LIB_DIR)/$(LIB_NAME).a $(TEST_FILES) $(TEST_HEADERS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_runner $(TEST_FILES) $(LIB_DIR)/$(LIB_NAME).a

test: $(BIN_DIR)/test_runner
	./$(BIN_DIR)/test_runner

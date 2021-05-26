# Compiler
CC=gcc
# Compilers flags
CFLAGS=-c -Wall
# Linker flags
LDFLAGS=
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=math-server
# Directory with source files
SRC_DIR=./src
# Directory for build
BUILD_DIR=./build_math-server

all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(BUILD_DIR)/$(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(BUILD_DIR)/$(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

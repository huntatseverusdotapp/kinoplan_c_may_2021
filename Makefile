# Compiler
CC=gcc
# Compilers flags
CFLAGS=-c -Wall
# Linker flags
LDFLAGS=
SOURCES=main.c utilities.c passivesock.c operations.c
OBJECTS=$(SOURCES:%.c=%.o)
LIBS=-lpthread
EXECUTABLE=math-server
# Directory with source files
SRC_DIR=./src
# Directory for build
BUILD_DIR=./build_math-server

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(OBJECTS:%.o=$(BUILD_DIR)/%.o) $(LIBS) -o $(BUILD_DIR)/$@

%.o:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/$(@:%.o=%.c) -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

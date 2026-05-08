#!/usr/bin/make
#
# Created 2026-04-17

GXX := g++

CFLAGS := -std=c++20
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow
CFLAGS += -ggdb
CFLAGS += -fsanitize=leak

SRC := $(wildcard *.cc)
OBJ := $(patsubst %.cc,%.o,$(SRC))
LIB := -lSDL2 -lsfml-audio

TARGET := app

all: build

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(CFLAGS) $(LIB)

$(OBJ): %.o : %.cc
	$(CXX) -c -o $@ $< $(CFLAGS)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)

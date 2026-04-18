#!/usr/bin/make
#
# Created 2026-04-17

GXX := g++

CFLAGS := -std=c++20
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow

SRC := $(wildcard *.cc)
OBJ := $(patsubst %.cc,%.o,$(SRC))
LIB := -lSDL2

TARGET := app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(CFLAGS) $(LIB)

$(OBJ): %.o : %.cc
	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)

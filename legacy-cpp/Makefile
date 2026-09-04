# ==============================================================================
# Local Betting Company (c) 2002
# System: Legacy Totalisator Subsystem (C++98 Standard)
# File: Makefile
# ==============================================================================

CXX = g++
CXXFLAGS = -O2 -Wall -Iinclude -std=c++98
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/tote_engine

SRCS = $(SRC_DIR)/tote_engine.cpp \
       $(SRC_DIR)/ticket_parser.cpp \
       $(SRC_DIR)/race_manager.cpp \
       $(SRC_DIR)/odds_display.cpp \
       $(SRC_DIR)/main.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

test: $(TARGET)
	bash baseline_test.sh

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

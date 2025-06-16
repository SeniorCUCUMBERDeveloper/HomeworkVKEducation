CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2

BIN := test_bin

.PHONY: all test clean

all: test
test: $(BIN)
	@./$(BIN)
$(BIN): lib.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
clean:
	$(RM) $(BIN)
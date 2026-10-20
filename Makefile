CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

caixeiro: main.cpp cenarios.cpp cenarios.hpp
	$(CXX) $(CXXFLAGS) main.cpp cenarios.cpp -o caixeiro

.PHONY: all
all: caixeiro

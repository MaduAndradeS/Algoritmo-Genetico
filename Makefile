CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

caixeiro: main.cpp cenarios.cpp cenarios.hpp
	$(CXX) $(CXXFLAGS) main.cpp cenarios.cpp -o caixeiro

.PHONY: all verificar experimentos
all: caixeiro

build/verificar_cenarios: verificar_cenarios.cpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_cenarios.cpp cenarios.cpp -o $@

verificar: build/verificar_cenarios
	./build/verificar_cenarios

build/preparar_experimentos: preparar_experimentos.cpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) preparar_experimentos.cpp cenarios.cpp -o $@

experimentos: build/preparar_experimentos
	./build/preparar_experimentos

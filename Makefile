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

build/verificar_ciclo: verificar_ciclo.cpp ciclo.cpp ciclo.hpp genetico.cpp genetico.hpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_ciclo.cpp ciclo.cpp genetico.cpp cenarios.cpp -o $@

.PHONY: verificar_ciclo
verificar_ciclo: build/verificar_ciclo
	./build/verificar_ciclo

build/verificar_genetico: verificar_genetico.cpp genetico.cpp genetico.hpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_genetico.cpp genetico.cpp -o $@

.PHONY: verificar_genetico comparativos
verificar_genetico: build/verificar_genetico
	./build/verificar_genetico

build/executar_experimentos: executar_experimentos.cpp ciclo.cpp ciclo.hpp genetico.cpp genetico.hpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) executar_experimentos.cpp ciclo.cpp genetico.cpp cenarios.cpp -o $@

# Escolha outra SAIDA para preservar rodadas anteriores.
SAIDA ?= experimentos/comparativos
comparativos: build/executar_experimentos
	./build/executar_experimentos "$(SAIDA)"
	python3 analisar_experimentos.py "$(SAIDA)"

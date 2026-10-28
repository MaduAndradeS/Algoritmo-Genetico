CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

.PHONY: all testes verificar verificar_genetico verificar_ciclo comparativos bonus
all: caixeiro

# Programa da primeira etapa (apenas uma rota aleatoria de referencia).
caixeiro: main.cpp cenarios.cpp cenarios.hpp
	$(CXX) $(CXXFLAGS) main.cpp cenarios.cpp -o caixeiro

# --- Testes (sem Qt) ---
testes: verificar verificar_genetico verificar_ciclo

build/verificar_cenarios: verificar_cenarios.cpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_cenarios.cpp cenarios.cpp -o $@

verificar: build/verificar_cenarios
	./build/verificar_cenarios

build/verificar_genetico: verificar_genetico.cpp genetico.cpp genetico.hpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_genetico.cpp genetico.cpp -o $@

verificar_genetico: build/verificar_genetico
	./build/verificar_genetico

build/verificar_ciclo: verificar_ciclo.cpp ciclo.cpp ciclo.hpp genetico.cpp genetico.hpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) verificar_ciclo.cpp ciclo.cpp genetico.cpp cenarios.cpp -o $@

verificar_ciclo: build/verificar_ciclo
	./build/verificar_ciclo

# --- Experimentos comparativos ---
build/executar_experimentos: executar_experimentos.cpp ciclo.cpp ciclo.hpp genetico.cpp genetico.hpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) executar_experimentos.cpp ciclo.cpp genetico.cpp cenarios.cpp -o $@

# Escolha outra SAIDA para preservar rodadas anteriores (a pasta nao pode existir).
SAIDA ?= experimentos/comparativos
comparativos: build/executar_experimentos
	./build/executar_experimentos "$(SAIDA)"
	python3 analisar_experimentos.py "$(SAIDA)"

# --- Bonus: circulo com muitos pontos ---
build/executar_bonus: executar_bonus.cpp ciclo.cpp ciclo.hpp genetico.cpp genetico.hpp cenarios.cpp cenarios.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) executar_bonus.cpp ciclo.cpp genetico.cpp cenarios.cpp -o $@

# A pasta nao pode existir.
SAIDA_BONUS ?= experimentos/bonus
bonus: build/executar_bonus
	./build/executar_bonus "$(SAIDA_BONUS)"

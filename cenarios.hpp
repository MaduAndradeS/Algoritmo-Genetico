#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Ponto {
    double x;
    double y;
};
using Rota = std::vector<int>;

std::vector<Ponto> gerar_pontos(const std::string& cenario, int quantidade = 10,
                               std::uint32_t semente = 42);
Rota criar_rota_aleatoria(int quantidade, std::uint32_t semente = 42);
double distancia_total(const Rota& rota, const std::vector<Ponto>& pontos);
double distancia_ideal_circulo(int quantidade, double raio = 40.0);

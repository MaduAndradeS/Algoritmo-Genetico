#include "cenarios.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>

std::vector<Ponto> gerar_pontos(const std::string& cenario, int quantidade,
                               std::uint32_t semente) {
    if (quantidade < 8) throw std::invalid_argument("Use pelo menos 8 pontos.");
    if (cenario != "aleatorio" && cenario != "circular")
        throw std::invalid_argument("Cenario deve ser aleatorio ou circular.");
    std::mt19937 rng(semente);
    std::uniform_real_distribution<double> uniforme(0.0, 100.0);
    const double pi = std::acos(-1.0);
    std::vector<Ponto> pontos;
    pontos.reserve(quantidade);
    for (int i = 0; i < quantidade; ++i) {
        if (cenario == "aleatorio") pontos.push_back({uniforme(rng), uniforme(rng)});
        else {
            const double angulo = 2 * pi * i / quantidade;
            pontos.push_back({50 + 40 * std::cos(angulo), 50 + 40 * std::sin(angulo)});
        }
    }
    return pontos;
}

Rota criar_rota_aleatoria(int quantidade, std::uint32_t semente) {
    if (quantidade < 1) throw std::invalid_argument("Quantidade deve ser positiva.");
    Rota rota(quantidade);
    std::iota(rota.begin(), rota.end(), 0);
    std::mt19937 rng(semente);
    std::shuffle(rota.begin(), rota.end(), rng);
    return rota;
}

double distancia_total(const Rota& rota, const std::vector<Ponto>& pontos) {
    if (pontos.empty() || rota.size() != pontos.size())
        throw std::invalid_argument("A rota deve conter cada indice exatamente uma vez.");
    std::vector<bool> visitados(pontos.size(), false);
    for (int indice : rota) {
        if (indice < 0 || static_cast<std::size_t>(indice) >= pontos.size() || visitados[indice])
            throw std::invalid_argument("Indice repetido ou fora dos limites.");
        visitados[indice] = true;
    }
    double distancia = 0;
    for (std::size_t i = 0; i < rota.size(); ++i) {
        const auto& a = pontos[rota[i]];
        const auto& b = pontos[rota[(i + 1) % rota.size()]];
        distancia += std::hypot(a.x - b.x, a.y - b.y);
    }
    return distancia;
}

double distancia_ideal_circulo(int quantidade, double raio) {
    if (quantidade < 3 || !std::isfinite(raio) || raio <= 0)
        throw std::invalid_argument("Quantidade ou raio invalidos.");
    // Perímetro do polígono regular, não comprimento da circunferência.
    return quantidade * 2 * raio * std::sin(std::acos(-1.0) / quantidade);
}

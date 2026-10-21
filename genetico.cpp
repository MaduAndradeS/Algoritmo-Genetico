#include "genetico.hpp"

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>

std::vector<Rota> criar_populacao(int quantidade_pontos, int tamanho, std::mt19937& rng) {
    if (quantidade_pontos < 1) throw std::invalid_argument("Quantidade de pontos deve ser positiva.");
    if (tamanho < 1) throw std::invalid_argument("Tamanho da populacao deve ser positivo.");
    std::vector<Rota> populacao;
    populacao.reserve(tamanho);
    Rota base(quantidade_pontos);
    std::iota(base.begin(), base.end(), 0);
    for (int i = 0; i < tamanho; ++i) {
        Rota rota = base;
        std::shuffle(rota.begin(), rota.end(), rng);
        populacao.push_back(std::move(rota));
    }
    return populacao;
}

Rota selecionar_torneio(const std::vector<Rota>& populacao,
                        const std::vector<double>& distancias,
                        std::mt19937& rng, int k) {
    if (populacao.empty()) throw std::invalid_argument("Populacao vazia.");
    if (populacao.size() != distancias.size())
        throw std::invalid_argument("Deve haver uma distancia por individuo.");
    if (k < 1) throw std::invalid_argument("O torneio precisa de ao menos 1 participante.");
    std::uniform_int_distribution<std::size_t> sorteio(0, populacao.size() - 1);
    std::size_t melhor = sorteio(rng);
    for (int i = 1; i < k; ++i) {
        const std::size_t candidato = sorteio(rng);
        if (distancias[candidato] < distancias[melhor]) melhor = candidato;
    }
    return populacao[melhor];  // copia do vencedor
}

Rota cruzar_ox(const Rota& pai1, const Rota& pai2, std::mt19937& rng) {
    if (pai1.empty() || pai1.size() != pai2.size())
        throw std::invalid_argument("Os pais devem ter o mesmo tamanho, maior que zero.");
    const std::size_t n = pai1.size();
    std::uniform_int_distribution<std::size_t> sorteio(0, n - 1);
    std::size_t a = sorteio(rng);
    std::size_t b = sorteio(rng);
    if (a > b) std::swap(a, b);

    Rota filho(n, -1);
    std::vector<bool> usado(n, false);
    // 1) O trecho [a, b] vem de pai1, nas mesmas posicoes.
    for (std::size_t i = a; i <= b; ++i) {
        filho[i] = pai1[i];
        usado[pai1[i]] = true;
    }
    // 2) O resto vem de pai2, na ordem dele, comecando logo apos b e dando a volta.
    std::size_t destino = (b + 1) % n;
    for (std::size_t k = 0; k < n; ++k) {
        const int cidade = pai2[(b + 1 + k) % n];
        if (!usado[cidade]) {
            filho[destino] = cidade;
            destino = (destino + 1) % n;
        }
    }
    return filho;
}

Rota mutar_inversao(const Rota& rota, std::mt19937& rng) {
    Rota mutante = rota;  // o original nao e alterado
    if (mutante.size() < 2) return mutante;
    std::uniform_int_distribution<std::size_t> sorteio(0, mutante.size() - 1);
    std::size_t i = sorteio(rng);
    std::size_t j = sorteio(rng);
    if (i > j) std::swap(i, j);
    std::reverse(mutante.begin() + static_cast<std::ptrdiff_t>(i),
                 mutante.begin() + static_cast<std::ptrdiff_t>(j) + 1);
    return mutante;
}
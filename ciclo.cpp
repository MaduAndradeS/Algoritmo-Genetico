#include "ciclo.hpp"

#include "genetico.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>

namespace {
using Relogio = std::chrono::steady_clock;

void validar_parametros(const std::vector<Ponto>& pontos, const ParametrosAG& p) {
    if (pontos.size() < 8) throw std::invalid_argument("Use ao menos 8 pontos.");
    for (const auto& ponto : pontos)
        if (!std::isfinite(ponto.x) || !std::isfinite(ponto.y))
            throw std::invalid_argument("As coordenadas precisam ser finitas.");
    if (p.tamanho_populacao < 1 || p.elitismo < 1 || p.elitismo > p.tamanho_populacao ||
        p.tamanho_torneio < 1 || p.max_geracoes < 0 || p.max_sem_melhora < 1 ||
        !std::isfinite(p.taxa_cruzamento) || p.taxa_cruzamento < 0 || p.taxa_cruzamento > 1 ||
        !std::isfinite(p.taxa_mutacao) || p.taxa_mutacao < 0 || p.taxa_mutacao > 1)
        throw std::invalid_argument("Parametros do AG invalidos.");
}

std::vector<std::vector<double>> calcular_matriz(const std::vector<Ponto>& pontos) {
    const std::size_t n = pontos.size();
    std::vector<std::vector<double>> matriz(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = i + 1; j < n; ++j) {
            const double d = std::hypot(pontos[i].x - pontos[j].x, pontos[i].y - pontos[j].y);
            if (!std::isfinite(d)) throw std::invalid_argument("Distancia nao finita.");
            matriz[i][j] = matriz[j][i] = d;
        }
    return matriz;
}

// Rota ja garantida como permutacao valida pelos operadores geneticos.
double distancia_rapida(const Rota& rota, const std::vector<std::vector<double>>& matriz) {
    double total = 0.0;
    for (std::size_t i = 0; i < rota.size(); ++i)
        total += matriz[rota[i]][rota[(i + 1) % rota.size()]];
    return total;
}
} // namespace

ResultadoAG executar_ag(const std::vector<Ponto>& pontos, const ParametrosAG& p,
                        CallbackGeracao callback) {
    validar_parametros(pontos, p);
    const auto matriz = calcular_matriz(pontos); // feita antes do relogio; parte da preparacao
    const auto inicio = Relogio::now();
    std::mt19937 rng(p.semente); // separado do RNG que gera os pontos
    std::bernoulli_distribution faz_cruzamento(p.taxa_cruzamento);
    std::bernoulli_distribution faz_mutacao(p.taxa_mutacao);
    auto populacao = criar_populacao(static_cast<int>(pontos.size()), p.tamanho_populacao, rng);
    ResultadoAG resultado;
    resultado.historico.reserve(static_cast<std::size_t>(p.max_geracoes) + 1);
    int sem_melhora = 0;
    double melhor_global = std::numeric_limits<double>::infinity();

    // Avalia e registra cada populacao, inclusive a inicial (geracao 0).
    for (int geracao = 0; ; ++geracao) {
        std::vector<double> distancias;
        distancias.reserve(populacao.size());
        double soma = 0.0;
        std::size_t indice_melhor = 0;
        for (std::size_t i = 0; i < populacao.size(); ++i) {
            const double d = distancia_rapida(populacao[i], matriz);
            distancias.push_back(d);
            soma += d;
            if (d < distancias[indice_melhor]) indice_melhor = i;
        }
        const double melhor_epoca = distancias[indice_melhor];
        if (melhor_epoca < melhor_global) {
            melhor_global = melhor_epoca;
            resultado.melhor_rota = populacao[indice_melhor];
            sem_melhora = 0;
        } else if (geracao > 0) {
            ++sem_melhora;
        }
        resultado.melhor_distancia = melhor_global;
        resultado.geracoes_executadas = geracao;
        const double tempo_atual = std::chrono::duration<double>(Relogio::now() - inicio).count();
        RegistroGeracao registro{geracao, populacao[indice_melhor], melhor_epoca,
                                soma / populacao.size(), tempo_atual};
        resultado.historico.push_back(registro);
        if (callback) callback(resultado.historico.back());

        if (geracao >= p.max_geracoes || sem_melhora >= p.max_sem_melhora) break;

        // Preserva os melhores individuos sem aplicar operadores sobre eles.
        std::vector<std::size_t> ranking(populacao.size());
        std::iota(ranking.begin(), ranking.end(), 0);
        std::partial_sort(ranking.begin(), ranking.begin() + p.elitismo, ranking.end(),
                          [&](std::size_t a, std::size_t b) { return distancias[a] < distancias[b]; });
        std::vector<Rota> proxima;
        proxima.reserve(populacao.size());
        for (int i = 0; i < p.elitismo; ++i) proxima.push_back(populacao[ranking[i]]);
        while (proxima.size() < populacao.size()) {
            const Rota pai1 = selecionar_torneio(populacao, distancias, rng, p.tamanho_torneio);
            const Rota pai2 = selecionar_torneio(populacao, distancias, rng, p.tamanho_torneio);
            Rota filho = faz_cruzamento(rng) ? cruzar_ox(pai1, pai2, rng) : pai1;
            if (faz_mutacao(rng)) filho = mutar_inversao(filho, rng);
            proxima.push_back(std::move(filho));
        }
        populacao = std::move(proxima);
    }
    resultado.tempo = std::chrono::duration<double>(Relogio::now() - inicio).count();
    return resultado;
}

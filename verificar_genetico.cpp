#include "genetico.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>

void verificar(bool condicao, const char* mensagem) {
    if (!condicao) throw std::runtime_error(mensagem);
}

template <typename Funcao>
void rejeita(Funcao funcao) {
    bool rejeitou = false;
    try { funcao(); }
    catch (const std::invalid_argument&) { rejeitou = true; }
    verificar(rejeitou, "Entrada invalida deveria ser rejeitada.");
}

// Contem todos os indices 0..n-1 exatamente uma vez?
bool e_permutacao(const Rota& rota, int n) {
    if (static_cast<int>(rota.size()) != n) return false;
    Rota ordenada = rota;
    std::sort(ordenada.begin(), ordenada.end());
    Rota esperada(n);
    std::iota(esperada.begin(), esperada.end(), 0);
    return ordenada == esperada;
}

int main() {
    try {
        std::mt19937 rng(123);

        for (int n : {8, 10, 50}) {
            // --- criar_populacao ---
            const auto populacao = criar_populacao(n, 100, rng);
            verificar(populacao.size() == 100, "Tamanho da populacao incorreto.");
            for (const auto& rota : populacao)
                verificar(e_permutacao(rota, n), "Rota da populacao nao e permutacao.");
            bool todas_iguais = true;
            for (const auto& rota : populacao) todas_iguais = todas_iguais && rota == populacao[0];
            verificar(!todas_iguais, "A populacao nao parece aleatoria.");

            // --- cruzar_ox e mutar_inversao: validade e pais intactos ---
            int mutacoes_que_mudaram = 0;
            for (int tentativa = 0; tentativa < 2000; ++tentativa) {
                const Rota pai1 = populacao[tentativa % 100];
                const Rota pai2 = populacao[(tentativa * 7 + 3) % 100];
                const Rota copia1 = pai1, copia2 = pai2;

                const Rota filho = cruzar_ox(pai1, pai2, rng);
                verificar(e_permutacao(filho, n), "Filho do OX nao e permutacao.");
                verificar(pai1 == copia1 && pai2 == copia2, "OX alterou um dos pais.");

                const Rota mutante = mutar_inversao(pai1, rng);
                verificar(e_permutacao(mutante, n), "Mutante nao e permutacao.");
                verificar(pai1 == copia1, "A mutacao alterou a rota original.");

                // So um trecho contiguo pode ter mudado, e ele deve estar invertido.
                std::size_t l = 0, r = mutante.size();
                while (l < mutante.size() && mutante[l] == pai1[l]) ++l;
                if (l < mutante.size()) {
                    ++mutacoes_que_mudaram;
                    r = mutante.size() - 1;
                    while (mutante[r] == pai1[r]) --r;
                    for (std::size_t i = l; i <= r; ++i)
                        verificar(mutante[i] == pai1[l + r - i], "O trecho alterado nao e uma inversao.");
                }
            }
            verificar(mutacoes_que_mudaram > 0, "A mutacao nunca alterou a rota.");

            // Pais iguais geram um filho igual a eles.
            const Rota& p = populacao[0];
            for (int i = 0; i < 200; ++i)
                verificar(cruzar_ox(p, p, rng) == p, "OX de pais iguais deve devolver o mesmo pai.");

            // --- selecionar_torneio ---
            std::vector<double> distancias(populacao.size());
            std::iota(distancias.begin(), distancias.end(), 1.0);  // a rota 0 e a melhor
            const auto populacao_copia = populacao;
            double soma = 0;
            for (int i = 0; i < 2000; ++i) {
                const Rota vencedor = selecionar_torneio(populacao, distancias, rng, 3);
                const auto it = std::find(populacao.begin(), populacao.end(), vencedor);
                verificar(it != populacao.end(), "O vencedor nao pertence a populacao.");
                soma += distancias[it - populacao.begin()];
            }
            verificar(populacao == populacao_copia, "O torneio alterou a populacao.");
            // Sem selecao a media seria 50,5; com k=3 deve ficar bem abaixo (esperado ~25).
            verificar(soma / 2000 < 40, "O torneio nao favorece as melhores rotas.");
            // Com k muito grande, o melhor quase certamente participa.
            verificar(selecionar_torneio(populacao, distancias, rng, 1000) == populacao[0],
                      "Torneio gigante deveria escolher o melhor.");
        }

        // --- reproducibilidade: mesma semente, mesmos resultados ---
        std::mt19937 a(7), b(7);
        const auto pa = criar_populacao(10, 20, a);
        const auto pb = criar_populacao(10, 20, b);
        verificar(pa == pb, "Populacao nao reproduzivel.");
        verificar(cruzar_ox(pa[0], pa[1], a) == cruzar_ox(pb[0], pb[1], b), "OX nao reproduzivel.");
        verificar(mutar_inversao(pa[0], a) == mutar_inversao(pb[0], b), "Mutacao nao reproduzivel.");

        // --- casos pequenos e entradas invalidas ---
        std::mt19937 rng2(1);
        verificar(cruzar_ox({0}, {0}, rng2) == Rota{0}, "OX com uma cidade.");
        verificar(mutar_inversao({0}, rng2) == Rota{0}, "Mutacao com uma cidade.");
        rejeita([&] { criar_populacao(0, 10, rng2); });
        rejeita([&] { criar_populacao(10, 0, rng2); });
        rejeita([&] { cruzar_ox({0, 1, 2}, {0, 1}, rng2); });
        rejeita([&] { cruzar_ox({}, {}, rng2); });
        rejeita([&] { selecionar_torneio({}, {}, rng2, 3); });
        rejeita([&] { selecionar_torneio({{0, 1}}, {1.0, 2.0}, rng2, 3); });
        rejeita([&] { selecionar_torneio({{0, 1}}, {1.0}, rng2, 0); });

        std::cout << "Operadores geneticos: OK\n";
    } catch (const std::exception& erro) {
        std::cerr << "Falha: " << erro.what() << '\n';
        return 1;
    }
}
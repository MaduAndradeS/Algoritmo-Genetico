#include "cenarios.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>

void verificar(bool condicao, const char* mensagem) {
    if (!condicao) throw std::runtime_error(mensagem);
}

bool perto(double a, double b) {
    return std::abs(a - b) <= 1e-10 * std::max(1.0, std::abs(b));
}

template <typename Funcao>
void rejeita(Funcao funcao) {
    bool rejeitou = false;
    try { funcao(); }
    catch (const std::invalid_argument&) { rejeitou = true; }
    verificar(rejeitou, "Entrada invalida deveria ser rejeitada.");
}

int main() {
    try {
        // Quatro lados de comprimento 1: sem o retorno daria apenas 3.
        const std::vector<Ponto> quadrado{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
        verificar(perto(distancia_total({0, 1, 2, 3}, quadrado), 4),
                  "O percurso deve incluir a volta ao inicio.");
        verificar(perto(distancia_total({0, 2, 1, 3}, quadrado), 2 + 2 * std::sqrt(2.0)),
                  "As diagonais devem usar distancia euclidiana.");
        verificar(perto(distancia_total({3, 2, 1, 0}, quadrado), 4),
                  "Inverter a rota deve preservar a distancia.");
        verificar(perto(distancia_total({2, 3, 0, 1}, quadrado), 4),
                  "Mudar o inicio deve preservar a distancia.");
        rejeita([&] { distancia_total({0, 1, 1, 3}, quadrado); });
        rejeita([&] { distancia_total({0, 1, 2, 4}, quadrado); });
        rejeita([&] { distancia_total({0, 1, 2, -1}, quadrado); });
        rejeita([&] { distancia_total({0, 1, 2}, quadrado); });
        rejeita([] { distancia_total({}, {}); });
        rejeita([] { gerar_pontos("aleatorio", 7); });
        rejeita([] { gerar_pontos("inexistente", 10); });

        for (int n : {8, 10, 20}) {
            const auto circulo = gerar_pontos("circular", n, 42);
            Rota ordenada(n);
            std::iota(ordenada.begin(), ordenada.end(), 0);
            verificar(perto(distancia_total(ordenada, circulo), distancia_ideal_circulo(n)),
                      "A referencia circular deve ser o perimetro do poligono.");
            for (const auto& p : circulo)
                verificar(perto(std::hypot(p.x - 50, p.y - 50), 40),
                          "Ponto circular fora do raio 40.");
            const auto pontos = gerar_pontos("aleatorio", n, 42);
            const auto repetidos = gerar_pontos("aleatorio", n, 42);
            for (int i = 0; i < n; ++i) {
                verificar(pontos[i].x >= 0 && pontos[i].x <= 100 &&
                          pontos[i].y >= 0 && pontos[i].y <= 100,
                          "Ponto aleatorio fora do quadrado.");
                verificar(pontos[i].x == repetidos[i].x && pontos[i].y == repetidos[i].y,
                          "A mesma semente deve repetir os pontos.");
            }
            auto rota = criar_rota_aleatoria(n, 42);
            verificar(rota == criar_rota_aleatoria(n, 42), "Rota nao reproduzivel.");
            std::sort(rota.begin(), rota.end());
            verificar(rota == ordenada, "Rota aleatoria deve ser uma permutacao.");
        }
        std::cout << "Cenarios, distancias, validacao de rotas e sementes: OK\n";
    } catch (const std::exception& erro) {
        std::cerr << "Falha: " << erro.what() << '\n';
        return 1;
    }
}

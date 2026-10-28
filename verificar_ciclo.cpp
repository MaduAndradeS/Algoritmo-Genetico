#include "ciclo.hpp"
#include "cenarios.hpp"

#include <algorithm>
  // Os testes dependem de assert: mantem-nos ativos mesmo com -DNDEBUG.
  #ifdef NDEBUG
  #undef NDEBUG
  #endif
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>

bool permutacao(const Rota& rota, int n) {
    if (static_cast<int>(rota.size()) != n) return false;
    Rota ordenada = rota;
    std::sort(ordenada.begin(), ordenada.end());
    Rota esperado(n);
    std::iota(esperado.begin(), esperado.end(), 0);
    return ordenada == esperado;
}

int main() {
    for (const auto& cenario : {"aleatorio", "circular"}) {
        const auto pontos = gerar_pontos(cenario, 10, 42);
        ParametrosAG p;
        p.max_geracoes = 1000;
        int chamadas = 0;
        const ResultadoAG resultado = executar_ag(pontos, p, [&](const RegistroGeracao& r) {
            assert(r.geracao == chamadas++);
        });
        assert(chamadas == resultado.geracoes_executadas + 1);
        assert(resultado.historico.size() == static_cast<std::size_t>(chamadas));
        assert(resultado.geracoes_executadas <= 1000);
        assert(permutacao(resultado.melhor_rota, 10));
        assert(std::abs(distancia_total(resultado.melhor_rota, pontos) - resultado.melhor_distancia) < 1e-8);
        assert(resultado.tempo >= resultado.historico.back().tempo);
        for (std::size_t i = 0; i < resultado.historico.size(); ++i) {
            const auto& g = resultado.historico[i];
            assert(g.geracao == static_cast<int>(i));
            assert(permutacao(g.melhor_rota, 10));
            assert(std::abs(distancia_total(g.melhor_rota, pontos) - g.melhor_distancia) < 1e-8);
            assert(g.distancia_media + 1e-8 >= g.melhor_distancia);
            if (i) {
                assert(g.melhor_distancia <= resultado.historico[i - 1].melhor_distancia + 1e-8);
                assert(g.tempo >= resultado.historico[i - 1].tempo);
            }
        }
        const ResultadoAG repeticao = executar_ag(pontos, p);
        assert(repeticao.melhor_rota == resultado.melhor_rota);
        assert(repeticao.geracoes_executadas == resultado.geracoes_executadas);
        std::cout << cenario << ": " << resultado.geracoes_executadas
                  << " geracoes, distancia " << resultado.melhor_distancia << '\n';
    }
    ParametrosAG p;
    p.max_geracoes = 0;
    const auto pontos = gerar_pontos("circular", 8, 42);
    const auto r = executar_ag(pontos, p);
    assert(r.geracoes_executadas == 0 && r.historico.size() == 1);
    p.taxa_mutacao = 1.2;
    bool erro = false;
    try { executar_ag(pontos, p); } catch (const std::invalid_argument&) { erro = true; }
    assert(erro);
    std::cout << "Testes do ciclo OK\n";
}

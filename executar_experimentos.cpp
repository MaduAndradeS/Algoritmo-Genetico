#include "cenarios.hpp"
#include "ciclo.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace fs = std::filesystem;
std::ofstream abrir(const fs::path& p) {
    std::ofstream f;
    f.exceptions(std::ios::failbit | std::ios::badbit);
    f.open(p);
    f << std::setprecision(17);
    return f;
}
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::invalid_argument("Uso: ./build/executar_experimentos pasta_nova");
        fs::path pasta(argv[1]);
        if (fs::exists(pasta)) throw std::invalid_argument("Use uma pasta nova; resultados existentes sao preservados.");
        fs::create_directories(pasta);
        auto resultados = abrir(pasta / "resultados.csv");
        auto historico = abrir(pasta / "historico.csv");
        auto pontos_csv = abrir(pasta / "pontos.csv");
        auto rotas = abrir(pasta / "rotas.csv");
        auto ambiente = abrir(pasta / "compilador.txt");
        ambiente << "Compilador: " << __VERSION__ << "\nC++: " << __cplusplus << '\n';
        resultados << "id,cenario,pontos,semente_pontos,semente_ag,populacao,taxa_cruzamento,taxa_mutacao,elitismo,torneio,max_geracoes,limite_sem_melhoria,status,distancia_referencia,distancia_inicial,melhor_distancia,tempo_segundos,geracoes_executadas,distancia_ideal,melhoria_inicial_percentual,melhoria_referencia_percentual,excesso_ideal_percentual\n";
        historico << "id,geracao,melhor_distancia,distancia_media,tempo_segundos\n";
        pontos_csv << "cenario,pontos,semente_pontos,indice,x,y\n";
        rotas << "id,tipo,ordem,indice\n";
        const int populacoes[] = {50,100,200,100,100};
        const double mutacoes[] = {.10,.10,.10,.01,.05};
        int id = 0;
        // Rodada principal de 10 pontos e extensao de 30 para avaliar dificuldade maior.
        for (int n : {10,30}) for (const std::string cenario : {"aleatorio","circular"}) {
            const auto pontos = gerar_pontos(cenario, n, 42);
            for (int i = 0; i < n; ++i)
                pontos_csv << cenario << ',' << n << ",42," << i << ',' << pontos[i].x << ',' << pontos[i].y << '\n';
            const double ideal = cenario == "circular" ? distancia_ideal_circulo(n) : 0;
            for (int config = 0; config < 5; ++config) for (unsigned seed = 1; seed <= 10; ++seed) {
                ++id;
                ParametrosAG p;
                p.tamanho_populacao = populacoes[config];
                p.taxa_mutacao = mutacoes[config];
                p.semente = seed;
                auto r = executar_ag(pontos,p);
                // Verificacao independente da avaliacao rapida usada pelo ciclo.
                if (r.historico.size() != static_cast<std::size_t>(r.geracoes_executadas + 1))
                    throw std::runtime_error("Historico incompleto.");
                double anterior = r.historico.front().melhor_distancia;
                for (const auto& g : r.historico) {
                    const double d = distancia_total(g.melhor_rota,pontos);
                    if (std::abs(d - g.melhor_distancia) > 1e-8 || g.melhor_distancia > anterior + 1e-8)
                        throw std::runtime_error("Distancia inconsistente ou perda do elitismo.");
                    anterior = g.melhor_distancia;
                }
                if (std::abs(distancia_total(r.melhor_rota,pontos) - r.melhor_distancia) > 1e-8 ||
                    (ideal > 0 && r.melhor_distancia < ideal - 1e-8))
                    throw std::runtime_error("Resultado final invalido.");
                const double inicial = r.historico.front().melhor_distancia;
                const double referencia = distancia_total(criar_rota_aleatoria(n,seed),pontos);
                resultados << id << ',' << cenario << ',' << n << ",42," << seed << ',' << p.tamanho_populacao
                    << ',' << p.taxa_cruzamento << ',' << p.taxa_mutacao << ',' << p.elitismo << ',' << p.tamanho_torneio
                    << ',' << p.max_geracoes << ',' << p.max_sem_melhora << ",concluido," << referencia << ',' << inicial
                    << ',' << r.melhor_distancia << ',' << r.tempo << ',' << r.geracoes_executadas << ',';
                if (ideal > 0) resultados << ideal;
                resultados << ',' << 100*(inicial-r.melhor_distancia)/inicial << ',' << 100*(referencia-r.melhor_distancia)/referencia << ',';
                if (ideal > 0) resultados << 100*(r.melhor_distancia/ideal-1);
                resultados << '\n';
                for (const auto& g : r.historico)
                    historico << id << ',' << g.geracao << ',' << g.melhor_distancia << ',' << g.distancia_media << ',' << g.tempo << '\n';
                for (const std::string tipo : {"inicial","final"}) {
                    const auto& rota = tipo == "inicial" ? r.historico.front().melhor_rota : r.melhor_rota;
                    for (int i = 0; i < n; ++i) rotas << id << ',' << tipo << ',' << i << ',' << rota[i] << '\n';
                }
            }
            std::cout << n << " pontos / " << cenario << ": 50 execucoes concluidas\n";
        }
        resultados.close(); historico.close(); pontos_csv.close(); rotas.close(); ambiente.close();
        std::cout << id << " execucoes concluidas em " << pasta << '\n';
    } catch (const std::exception& e) { std::cerr << "Erro: " << e.what() << '\n'; return 1; }
}

#include "cenarios.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

std::ofstream abrir(const fs::path& caminho) {
    std::ofstream arquivo;
    arquivo.exceptions(std::ios::failbit | std::ios::badbit);
    arquivo.open(caminho);
    arquivo << std::setprecision(17);
    return arquivo;
}

int main(int argc, char* argv[]) {
    try {
        if (argc > 2) throw std::invalid_argument("Uso: ./preparar_experimentos [pasta_saida]");
        const fs::path pasta = argc == 2 ? argv[1] : "experimentos/dados";
        for (const auto* nome : {"pontos.csv", "plano.csv", "resultados.csv"})
            if (fs::exists(pasta / nome))
                throw std::invalid_argument("A pasta ja contem arquivos do experimento. Use outra pasta para preservar os resultados.");
        fs::create_directories(pasta);
        auto pontos_csv = abrir(pasta / "pontos.csv");
        auto plano = abrir(pasta / "plano.csv");
        auto resultados = abrir(pasta / "resultados.csv");
        pontos_csv << "cenario,semente_pontos,indice,x,y\n";
        plano << "id,cenario,pontos,semente_pontos,semente_ag,populacao,taxa_cruzamento,taxa_mutacao,elitismo,max_geracoes,limite_sem_melhoria,distancia_rota_aleatoria,distancia_ideal\n";
        resultados << "id,status,melhor_distancia,tempo_segundos,geracoes_executadas\n";
        // A configuracao 100/10% e compartilhada pelas duas comparacoes.
        const int populacoes[] = {50, 100, 200, 100, 100};
        const double mutacoes[] = {0.10, 0.10, 0.10, 0.01, 0.05};
        int id = 0;
        for (const std::string cenario : {"aleatorio", "circular"}) {
            const auto pontos = gerar_pontos(cenario, 10, 42);
            for (std::size_t i = 0; i < pontos.size(); ++i)
                pontos_csv << cenario << ",42," << i << ',' << pontos[i].x << ',' << pontos[i].y << '\n';
            for (int configuracao = 0; configuracao < 5; ++configuracao) {
                for (std::uint32_t semente = 1; semente <= 10; ++semente) {
                    ++id;
                    plano << id << ',' << cenario << ",10,42," << semente << ','
                          << populacoes[configuracao] << ",0.9," << mutacoes[configuracao]
                          << ",1,1000,200,"
                          << distancia_total(criar_rota_aleatoria(10, semente), pontos) << ',';
                    if (cenario == "circular") plano << distancia_ideal_circulo(10);
                    plano << '\n';
                    resultados << id << ",aguardando_algoritmo,,,\n";
                }
            }
        }
        pontos_csv.close();
        plano.close();
        resultados.close();
        std::cout << id << " execucoes planejadas em " << fs::absolute(pasta)
                  << "\nNenhuma otimizacao foi executada. Resultados ainda vazios.\n";
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}

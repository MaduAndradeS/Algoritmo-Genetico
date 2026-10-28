#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "cenarios.hpp"

struct ParametrosAG {
    int tamanho_populacao = 100;
    double taxa_cruzamento = 0.90;
    double taxa_mutacao = 0.10;
    int elitismo = 1;
    int max_geracoes = 1000;
    int max_sem_melhora = 200;
    int tamanho_torneio = 3;
    std::uint32_t semente = 1;
};

struct RegistroGeracao {
    int geracao = 0;
    Rota melhor_rota;
    double melhor_distancia = 0.0;
    double distancia_media = 0.0;
    double tempo = 0.0; // segundos desde o inicio do algoritmo
};

struct ResultadoAG {
    Rota melhor_rota;
    double melhor_distancia = 0.0;
    int geracoes_executadas = 0;
    double tempo = 0.0;
    std::vector<RegistroGeracao> historico;
};

// O callback recebe o registro da geracao 0 e de cada geracao evoluida.
// A interface Qt usa esse callback para receber as atualizacoes ao vivo.
using CallbackGeracao = std::function<void(const RegistroGeracao&)>;

ResultadoAG executar_ag(const std::vector<Ponto>& pontos,
                        const ParametrosAG& parametros = {},
                        CallbackGeracao callback = {});

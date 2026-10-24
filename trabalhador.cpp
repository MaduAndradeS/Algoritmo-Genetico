#include "trabalhador.hpp"

#include <chrono>
#include <exception>
#include <utility>

namespace {
struct Interrompido {};
}

Trabalhador::Trabalhador(std::vector<Ponto> pontos, ParametrosAG parametros, QObject* pai)
    : QObject(pai), pontos_(std::move(pontos)), parametros_(parametros) {}

void Trabalhador::executar() {
    using Relogio = std::chrono::steady_clock;
    const auto intervalo = std::chrono::milliseconds(33);  // ~30 atualizacoes por segundo
    Progresso pendente;
    auto ultimoEnvio = Relogio::now();
    bool parando = false;
    try {
        auto resultado = std::make_shared<ResultadoAG>(
            executar_ag(pontos_, parametros_, [&](const RegistroGeracao& r) {
                if (pendente.melhores.empty()) pendente.primeira = r.geracao;
                pendente.melhores.push_back(r.melhor_distancia);
                pendente.medias.push_back(r.distancia_media);
                pendente.geracao = r.geracao;
                pendente.tempo = r.tempo;

                const bool parar = parar_.load();
                if (parar) {
                    // executar_ag le parametros_.max_geracoes depois de cada callback;
                    // igualar ao numero da geracao atual encerra o ciclo aqui mesmo,
                    // devolvendo o historico ate este ponto.
                    // Se a biblioteca deixar de ler esse campo por referencia, o
                    // proximo callback abandona a execucao.
                    if (parando) throw Interrompido{};
                    parando = true;
                    parametros_.max_geracoes = r.geracao;
                }
                const auto agora = Relogio::now();
                if (parar || r.geracao == 0 || agora - ultimoEnvio >= intervalo) {
                    pendente.rota = r.melhor_rota;
                    emit progresso(pendente);
                    pendente.melhores.clear();
                    pendente.medias.clear();
                    ultimoEnvio = agora;
                }
            }));
        emit terminou(std::move(resultado), parando);
    } catch (const Interrompido&) {
        emit terminou(nullptr, true);
    } catch (const std::exception& erro) {
        emit falhou(QString::fromUtf8(erro.what()));
    }
    emit finalizado();
}
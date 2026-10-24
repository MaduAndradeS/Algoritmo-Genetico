#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <memory>
#include <vector>

#include "ciclo.hpp"

// Pacote enviado a janela durante a execucao: so as geracoes novas desde o
// ultimo envio (para nao copiar o historico inteiro a cada atualizacao) e a
// melhor rota da ultima geracao.
struct Progresso {
    int primeira = 0;               // geracao do primeiro valor de melhores/medias
    int geracao = 0;                // ultima geracao incluida
    std::vector<double> melhores;
    std::vector<double> medias;
    Rota rota;                      // melhor rota da ultima geracao
    double tempo = 0.0;             // segundos desde o inicio do algoritmo
};

// Executa o algoritmo genetico fora da thread da interface.
// Uso: mover para um QThread, ligar QThread::started a executar() e receber
// os sinais. Nenhum widget e tocado aqui.
class Trabalhador : public QObject {
    Q_OBJECT
public:
    Trabalhador(std::vector<Ponto> pontos, ParametrosAG parametros, QObject* pai = nullptr);

    // Pode ser chamado de qualquer thread: a execucao termina na proxima geracao
    // e o resultado parcial (historico ate la) e entregue normalmente.
    void pedirParada() { parar_ = true; }

public slots:
    void executar();

signals:
    void progresso(const Progresso& pacote);
    // resultado nulo: execucao abandonada sem resultado aproveitavel.
    void terminou(std::shared_ptr<const ResultadoAG> resultado, bool interrompido);
    void falhou(const QString& mensagem);
    void finalizado();  // sempre o ultimo sinal

private:
    std::vector<Ponto> pontos_;
    ParametrosAG parametros_;
    std::atomic<bool> parar_{false};
};
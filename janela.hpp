#pragma once

#include <QMainWindow>
#include <memory>
#include <vector>

#include "cenarios.hpp"
#include "ciclo.hpp"
#include "trabalhador.hpp"

class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSlider;
class QSpinBox;
class QThread;
class QTimer;
class QWidget;
class WidgetGrafico;
class WidgetRota;

class Janela : public QMainWindow {
    Q_OBJECT
public:
    explicit Janela(QWidget* pai = nullptr);
    ~Janela() override;

private slots:
    void gerarPontos();
    void executarOuParar();
    void aoProgresso(const Progresso& pacote);
    void aoTerminar(std::shared_ptr<const ResultadoAG> resultado, bool interrompido);
    void aoFalhar(const QString& mensagem);
    void aoThreadParar();
    void mostrarEpoca(int geracao);
    void alternarReproducao();
    void avancarReproducao();
    void salvarCsv();
    void salvarPng();

private:
    ParametrosAG lerParametros() const;
    void definirExecutando(bool executando);
    void pararReproducao();
    void atualizarBotoes();
    bool escreverCsv(const QString& caminho) const;
    bool escreverPng(const QString& caminho) const;

    // Controles do problema
    QComboBox* cenario_;
    QSpinBox* qtdPontos_;
    QSpinBox* sementePontos_;
    // Parametros do algoritmo
    QSpinBox* populacao_;
    QDoubleSpinBox* cruzamento_;
    QDoubleSpinBox* mutacao_;
    QSpinBox* elitismo_;
    QSpinBox* torneio_;
    QSpinBox* maxGeracoes_;
    QSpinBox* maxSemMelhora_;
    QSpinBox* sementeAG_;
    QGroupBox* grupoProblema_;
    QGroupBox* grupoAG_;
    QPushButton* botaoExecutar_;
    QPushButton* botaoCsv_;
    QPushButton* botaoPng_;
    // Resultados
    QLabel* lblMelhor_;
    QLabel* lblMedia_;
    QLabel* lblMelhoria_;
    QLabel* lblExcesso_;
    QLabel* lblTempo_;
    QLabel* lblConvergencia_;
    // Visualizacao
    QWidget* conteudo_;  // rota + grafico (e o que o botao de imagem salva)
    WidgetRota* desenho_;
    WidgetGrafico* grafico_;
    QPushButton* botaoPlay_;
    QComboBox* velocidade_;
    QSlider* slider_;
    QLabel* lblEpoca_;
    QTimer* timer_;

    // Estado
    std::vector<Ponto> pontosAtuais_;
    double ideal_ = 0.0;  // > 0 apenas no cenario circular
    ParametrosAG parametrosUsados_;
    std::shared_ptr<const ResultadoAG> resultado_;
    QThread* thread_ = nullptr;
    Trabalhador* trabalhador_ = nullptr;
    bool executando_ = false;
};
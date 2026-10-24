#pragma once

#include <QWidget>
#include <vector>

// Curva de convergencia: melhor distancia e distancia media por geracao,
// linha do valor ideal (quando conhecido) e marcador da geracao escolhida.
// Nao conhece o algoritmo genetico: recebe apenas numeros.
class WidgetGrafico : public QWidget {
    Q_OBJECT
public:
    explicit WidgetGrafico(QWidget* pai = nullptr);

    void limpar();
    void definirIdeal(double ideal);  // valor <= 0: sem linha de referencia
    void definirDados(std::vector<double> melhores, std::vector<double> medias);
    // Acrescenta geracoes (usado durante a execucao). `primeira` e o numero da
    // geracao do primeiro valor recebido.
    void acrescentar(int primeira, const std::vector<double>& melhores,
                     const std::vector<double>& medias);
    void definirEpoca(int epoca);     // -1: sem marcador
    void definirInterativo(bool interativo);  // permite escolher a geracao com o mouse

signals:
    void epocaEscolhida(int epoca);

protected:
    void paintEvent(QPaintEvent* evento) override;
    void mousePressEvent(QMouseEvent* evento) override;
    void mouseMoveEvent(QMouseEvent* evento) override;

private:
    QRectF area() const;
    void escolherPorMouse(double x);

    std::vector<double> melhores_;
    std::vector<double> medias_;
    double ideal_ = 0.0;
    int epoca_ = -1;
    bool interativo_ = false;
};
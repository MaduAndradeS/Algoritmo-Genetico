#pragma once

#include <QWidget>
#include <vector>

#include "cenarios.hpp"

// Desenha os pontos e a rota fechada. Nao conhece o algoritmo genetico:
// quem usa so chama definir() com os pontos e a rota que quer mostrar.
class WidgetRota : public QWidget {
    Q_OBJECT
public:
    explicit WidgetRota(QWidget* pai = nullptr);

    void definir(const std::vector<Ponto>& pontos, const Rota& rota);

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    std::vector<Ponto> pontos_;
    Rota rota_;
};
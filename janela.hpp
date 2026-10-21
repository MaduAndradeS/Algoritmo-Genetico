#pragma once

#include <QMainWindow>
#include <vector>

#include "cenarios.hpp"

class QComboBox;
class QLabel;
class QSpinBox;
class WidgetRota;

class Janela : public QMainWindow {
    Q_OBJECT
public:
    explicit Janela(QWidget* pai = nullptr);

private slots:
    void gerar();

private:
    QComboBox* cenario_;
    QSpinBox* pontos_;
    QSpinBox* semente_;
    QLabel* distancia_;
    QLabel* ideal_;
    WidgetRota* desenho_;
};
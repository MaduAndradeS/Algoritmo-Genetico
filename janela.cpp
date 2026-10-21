#include "janela.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <exception>

#include "widgetrota.hpp"

Janela::Janela(QWidget* pai) : QMainWindow(pai) {
    setWindowTitle("Caixeiro viajante - algoritmo genetico");

    cenario_ = new QComboBox;
    cenario_->addItem("Aleatorio", "aleatorio");
    cenario_->addItem("Circular", "circular");

    pontos_ = new QSpinBox;
    pontos_->setRange(8, 5000);
    pontos_->setValue(10);

    semente_ = new QSpinBox;
    semente_->setRange(0, 2147483647);
    semente_->setValue(42);

    auto* botaoGerar = new QPushButton("Gerar");
    distancia_ = new QLabel("-");
    ideal_ = new QLabel("-");

    auto* formulario = new QFormLayout;
    formulario->addRow("Cenario:", cenario_);
    formulario->addRow("Pontos:", pontos_);
    formulario->addRow("Semente:", semente_);
    formulario->addRow(botaoGerar);
    formulario->addRow("Distancia:", distancia_);
    formulario->addRow("Ideal (circulo):", ideal_);

    auto* painel = new QWidget;
    auto* colunaEsquerda = new QVBoxLayout(painel);
    colunaEsquerda->addLayout(formulario);
    colunaEsquerda->addStretch(1);
    painel->setFixedWidth(260);

    desenho_ = new WidgetRota;

    auto* central = new QWidget;
    auto* linha = new QHBoxLayout(central);
    linha->addWidget(painel);
    linha->addWidget(desenho_, 1);
    setCentralWidget(central);

    connect(botaoGerar, &QPushButton::clicked, this, &Janela::gerar);
    gerar();  // mostra algo ja na abertura
}

void Janela::gerar() {
    try {
        const std::string nome = cenario_->currentData().toString().toStdString();
        const int n = pontos_->value();
        const auto semente = static_cast<std::uint32_t>(semente_->value());

        const auto pontos = gerar_pontos(nome, n, semente);
        const auto rota = criar_rota_aleatoria(n, semente);
        const double distancia = distancia_total(rota, pontos);

        desenho_->definir(pontos, rota);
        distancia_->setText(QString::number(distancia, 'f', 4));

        if (nome == "circular") {
            const double ideal = distancia_ideal_circulo(n);
            const double excesso = 100.0 * (distancia / ideal - 1.0);
            ideal_->setText(QString("%1 (+%2%)")
                                .arg(ideal, 0, 'f', 4)
                                .arg(excesso, 0, 'f', 1));
        } else {
            ideal_->setText("-");
        }
    } catch (const std::exception& erro) {
        QMessageBox::warning(this, "Erro", QString::fromUtf8(erro.what()));
    }
}

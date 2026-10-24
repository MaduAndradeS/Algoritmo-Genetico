#include "widgetrota.hpp"

#include <QPainter>
#include <QPointF>
#include <algorithm>

WidgetRota::WidgetRota(QWidget* pai) : QWidget(pai) {
    setMinimumSize(340, 300);
}

void WidgetRota::definir(const std::vector<Ponto>& pontos, const Rota& rota) {
    pontos_ = pontos;
    rota_ = rota;
    update();  // pede um novo paintEvent
}

void WidgetRota::definirTitulo(const QString& titulo) {
    titulo_ = titulo;
    update();
}

void WidgetRota::paintEvent(QPaintEvent*) {
    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.fillRect(rect(), Qt::white);

    pintor.setPen(QColor("#0f172a"));
    pintor.drawText(QPointF(8, 18), titulo_);

    // Area quadrada com margem; o plano [0,100] x [0,100] ocupa o quadrado inteiro,
    // com a mesma escala nos dois eixos e o eixo y para cima.
    const int margem = 30;
    const int lado = std::min(width(), height()) - 2 * margem;
    if (lado <= 0) return;
    const double escala = lado / 100.0;
    const double x0 = (width() - lado) / 2.0;
    const double y0 = (height() + lado) / 2.0;

    pintor.setPen(QPen(QColor("#cbd5e1"), 1));
    pintor.setBrush(QColor("#f8fafc"));
    pintor.drawRect(QRectF(x0, y0 - lado, lado, lado));

    if (pontos_.empty()) return;

    auto paraTela = [&](const Ponto& p) {
        return QPointF(x0 + escala * p.x, y0 - escala * p.y);
    };

    const bool muitos = pontos_.size() > 150;

    // Rota fechada: o ultimo ponto volta ao primeiro.
    if (rota_.size() == pontos_.size()) {
        pintor.setPen(QPen(QColor("#2563eb"), muitos ? 1.2 : 2));
        for (std::size_t i = 0; i < rota_.size(); ++i) {
            const Ponto& a = pontos_[rota_[i]];
            const Ponto& b = pontos_[rota_[(i + 1) % rota_.size()]];
            pintor.drawLine(paraTela(a), paraTela(b));
        }
    }

    // Pontos; o primeiro da rota (inicio e fim) fica em vermelho.
    const int inicio = rota_.empty() ? -1 : rota_.front();
    const bool mostrarRotulos = pontos_.size() <= 60;
    const double raio = muitos ? 2.0 : 4.0;
    for (std::size_t i = 0; i < pontos_.size(); ++i) {
        const QPointF centro = paraTela(pontos_[i]);
        const bool eInicio = static_cast<int>(i) == inicio;
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(eInicio ? QColor("#dc2626") : QColor("#0f172a"));
        pintor.drawEllipse(centro, eInicio ? raio + 1 : raio, eInicio ? raio + 1 : raio);
        if (mostrarRotulos) {
            pintor.setPen(QColor("#0f172a"));
            pintor.drawText(centro + QPointF(7, -7), QString::number(i));
        }
    }
}
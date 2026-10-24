#include "widgetgrafico.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPolygonF>
#include <algorithm>
#include <cmath>

namespace {
// Passo "redondo" (1, 2, 5 x 10^k) proximo de x.
double passoBonito(double x) {
    if (!(x > 0)) return 1.0;
    const double potencia = std::pow(10.0, std::floor(std::log10(x)));
    const double f = x / potencia;
    const double n = f < 1.5 ? 1 : f < 3 ? 2 : f < 7 ? 5 : 10;
    return n * potencia;
}

QString formatar(double valor, double passo) {
    const int casas = passo >= 1.0 ? 0 : static_cast<int>(std::ceil(-std::log10(passo)));
    return QString::number(valor, 'f', casas);
}

const QColor kMelhor("#2563eb");
const QColor kMedia("#f59e0b");
const QColor kIdeal("#16a34a");
const QColor kTexto("#0f172a");
const QColor kEixo("#64748b");
const QColor kGrade("#e2e8f0");
}  // namespace

WidgetGrafico::WidgetGrafico(QWidget* pai) : QWidget(pai) {
    setMinimumSize(360, 200);
}

void WidgetGrafico::limpar() {
    melhores_.clear();
    medias_.clear();
    epoca_ = -1;
    update();
}

void WidgetGrafico::definirIdeal(double ideal) {
    ideal_ = ideal;
    update();
}

void WidgetGrafico::definirDados(std::vector<double> melhores, std::vector<double> medias) {
    melhores_ = std::move(melhores);
    medias_ = std::move(medias);
    update();
}

void WidgetGrafico::acrescentar(int primeira, const std::vector<double>& melhores,
                                const std::vector<double>& medias) {
    const auto inicio = static_cast<std::size_t>(std::max(primeira, 0));
    if (inicio < melhores_.size()) {
        melhores_.resize(inicio);
        medias_.resize(inicio);
    }
    melhores_.insert(melhores_.end(), melhores.begin(), melhores.end());
    medias_.insert(medias_.end(), medias.begin(), medias.end());
    update();
}

void WidgetGrafico::definirEpoca(int epoca) {
    epoca_ = epoca;
    update();
}

void WidgetGrafico::definirInterativo(bool interativo) {
    interativo_ = interativo;
    setCursor(interativo ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

QRectF WidgetGrafico::area() const {
    return QRectF(64, 30, width() - 64 - 14, height() - 30 - 38);
}

void WidgetGrafico::escolherPorMouse(double x) {
    if (!interativo_ || melhores_.size() < 2) return;
    const QRectF a = area();
    const double fracao = std::clamp((x - a.left()) / a.width(), 0.0, 1.0);
    const int epoca = static_cast<int>(std::lround(fracao * static_cast<double>(melhores_.size() - 1)));
    emit epocaEscolhida(epoca);
}

void WidgetGrafico::mousePressEvent(QMouseEvent* evento) {
    escolherPorMouse(evento->position().x());
}

void WidgetGrafico::mouseMoveEvent(QMouseEvent* evento) {
    if (evento->buttons() & Qt::LeftButton) escolherPorMouse(evento->position().x());
}

void WidgetGrafico::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::white);

    const QRectF a = area();
    p.setPen(kTexto);
    p.drawText(QPointF(8, 18), "Distância por geração");

    if (a.width() < 20 || a.height() < 20) return;
    if (melhores_.empty()) {
        p.setPen(QPen(kGrade, 1));
        p.drawRect(a);
        p.setPen(kEixo);
        p.drawText(a, Qt::AlignCenter, "Execute o algoritmo para ver a curva de convergência.");
        return;
    }

    const std::size_t n = melhores_.size();
    double ymin = *std::min_element(melhores_.begin(), melhores_.end());
    double ymax = std::max(*std::max_element(medias_.begin(), medias_.end()),
                           *std::max_element(melhores_.begin(), melhores_.end()));
    if (ideal_ > 0) ymin = std::min(ymin, ideal_);
    if (ymax - ymin < 1e-9) ymax = ymin + 1.0;
    const double folga = 0.05 * (ymax - ymin);
    ymin = std::max(0.0, ymin - folga);
    ymax += folga;
    const double xmax = std::max<double>(1.0, static_cast<double>(n - 1));

    auto X = [&](double g) { return a.left() + g / xmax * a.width(); };
    auto Y = [&](double v) { return a.bottom() - (v - ymin) / (ymax - ymin) * a.height(); };

    // Grade e rotulos dos eixos.
    QFont fonte = p.font();
    fonte.setPointSizeF(std::max(7.0, fonte.pointSizeF() - 1.0));
    p.setFont(fonte);
    const double passoY = passoBonito((ymax - ymin) / 5.0);
    for (double v = std::ceil(ymin / passoY) * passoY; v <= ymax + 1e-9; v += passoY) {
        p.setPen(QPen(kGrade, 1));
        p.drawLine(QPointF(a.left(), Y(v)), QPointF(a.right(), Y(v)));
        p.setPen(kEixo);
        p.drawText(QRectF(0, Y(v) - 9, a.left() - 6, 18), Qt::AlignRight | Qt::AlignVCenter,
                   formatar(v, passoY));
    }
    const double passoX = std::max(1.0, passoBonito(xmax / 6.0));
    for (double g = 0; g <= xmax + 1e-9; g += passoX) {
        p.setPen(QPen(kGrade, 1));
        p.drawLine(QPointF(X(g), a.top()), QPointF(X(g), a.bottom()));
        p.setPen(kEixo);
        p.drawText(QRectF(X(g) - 30, a.bottom() + 3, 60, 16), Qt::AlignHCenter | Qt::AlignTop,
                   QString::number(static_cast<long long>(g)));
    }
    p.setPen(kEixo);
    p.drawText(QRectF(a.left(), a.bottom() + 20, a.width(), 16), Qt::AlignCenter, "Geração");
    p.setPen(QPen(kEixo, 1));
    p.drawRect(a);

    // Series (com amostragem quando ha muito mais geracoes que pixels).
    p.save();
    p.setClipRect(a.adjusted(-1, -1, 1, 1));
    auto desenhar = [&](const std::vector<double>& v, const QPen& caneta) {
        p.setPen(caneta);
        if (v.size() == 1) {
            p.setBrush(caneta.color());
            p.drawEllipse(QPointF(X(0), Y(v[0])), 3, 3);
            return;
        }
        const std::size_t passo = std::max<std::size_t>(1, v.size() / static_cast<std::size_t>(2 * a.width() + 1));
        QPolygonF linha;
        for (std::size_t i = 0; i < v.size(); i += passo)
            linha << QPointF(X(static_cast<double>(i)), Y(v[i]));
        if ((v.size() - 1) % passo != 0)
            linha << QPointF(X(static_cast<double>(v.size() - 1)), Y(v.back()));
        p.drawPolyline(linha);
    };
    desenhar(medias_, QPen(kMedia, 1.3));
    if (ideal_ > 0) {
        p.setPen(QPen(kIdeal, 1.6, Qt::DashLine));
        p.drawLine(QPointF(a.left(), Y(ideal_)), QPointF(a.right(), Y(ideal_)));
    }
    desenhar(melhores_, QPen(kMelhor, 2.2));
    p.restore();

    // Marcador da geracao escolhida.
    if (epoca_ >= 0 && static_cast<std::size_t>(epoca_) < n) {
        p.setPen(QPen(kTexto, 1, Qt::DotLine));
        p.drawLine(QPointF(X(epoca_), a.top()), QPointF(X(epoca_), a.bottom()));
        p.setPen(QPen(Qt::white, 1.5));
        p.setBrush(kMelhor);
        p.drawEllipse(QPointF(X(epoca_), Y(melhores_[static_cast<std::size_t>(epoca_)])), 5, 5);
    }

    // Legenda, da direita para a esquerda, acima da area do grafico.
    struct Item { QString texto; QColor cor; Qt::PenStyle estilo; };
    std::vector<Item> itens{{"Melhor", kMelhor, Qt::SolidLine}, {"Média", kMedia, Qt::SolidLine}};
    if (ideal_ > 0) itens.push_back({"Ideal", kIdeal, Qt::DashLine});
    p.setBrush(Qt::NoBrush);
    double xl = a.right();
    for (auto it = itens.rbegin(); it != itens.rend(); ++it) {
        const double largura = p.fontMetrics().horizontalAdvance(it->texto);
        xl -= largura;
        p.setPen(kTexto);
        p.drawText(QPointF(xl, 20), it->texto);
        xl -= 6 + 22;
        p.setPen(QPen(it->cor, 2.2, it->estilo));
        p.drawLine(QPointF(xl, 16), QPointF(xl + 22, 16));
        xl -= 14;
    }
}
#include "janela.hpp"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <exception>

#include "widgetgrafico.hpp"
#include "widgetrota.hpp"

namespace {
QString dist(double v) { return QString::number(v, 'f', 4); }

QSpinBox* criarSpin(int minimo, int maximo, int valor, const QString& dica) {
    auto* spin = new QSpinBox;
    spin->setRange(minimo, maximo);
    spin->setValue(valor);
    spin->setToolTip(dica);
    return spin;
}

QDoubleSpinBox* criarPercentual(double valor, const QString& dica) {
    auto* spin = new QDoubleSpinBox;
    spin->setRange(0.0, 100.0);
    spin->setDecimals(1);
    spin->setSingleStep(1.0);
    spin->setSuffix(" %");
    spin->setValue(valor);
    spin->setToolTip(dica);
    return spin;
}
}  // namespace

Janela::Janela(QWidget* pai) : QMainWindow(pai) {
    setWindowTitle("Caixeiro viajante - algoritmo genético");
    qRegisterMetaType<Progresso>("Progresso");
    qRegisterMetaType<std::shared_ptr<const ResultadoAG>>("std::shared_ptr<const ResultadoAG>");

    const ParametrosAG padrao;  // valores iniciais vem do proprio ciclo

    // --- Problema ---
    cenario_ = new QComboBox;
    cenario_->addItem("Aleatório (uniforme)", "aleatorio");
    cenario_->addItem("Círculo (benchmark)", "circular");
    qtdPontos_ = criarSpin(8, 2000, 10, "Quantidade de pontos (mínimo 8).");
    sementePontos_ = criarSpin(0, 2147483647, 42, "Semente que gera os pontos do cenário aleatório.");
    grupoProblema_ = new QGroupBox("Problema");
    auto* formProblema = new QFormLayout(grupoProblema_);
    formProblema->addRow("Cenário:", cenario_);
    formProblema->addRow("Pontos:", qtdPontos_);
    formProblema->addRow("Semente dos pontos:", sementePontos_);

    // --- Algoritmo ---
    populacao_ = criarSpin(1, 5000, padrao.tamanho_populacao, "Quantas rotas competem em cada geração.");
    cruzamento_ = criarPercentual(100 * padrao.taxa_cruzamento,
                                  "Chance de um filho vir do cruzamento OX (senão copia um pai).");
    mutacao_ = criarPercentual(100 * padrao.taxa_mutacao,
                               "Chance de cada filho sofrer mutação por inversão.");
    elitismo_ = criarSpin(1, 1000, padrao.elitismo, "Melhores rotas copiadas intactas para a próxima geração.");
    torneio_ = criarSpin(1, 50, padrao.tamanho_torneio, "Quantas rotas disputam cada torneio de seleção.");
    maxGeracoes_ = criarSpin(0, 1000000, padrao.max_geracoes, "Limite de gerações evoluídas.");
    maxSemMelhora_ = criarSpin(1, 1000000, padrao.max_sem_melhora,
                               "Para após tantas gerações seguidas sem melhorar a melhor distância.");
    sementeAG_ = criarSpin(0, 2147483647, static_cast<int>(padrao.semente),
                           "Semente do algoritmo (a mesma semente repete a execução).");
    grupoAG_ = new QGroupBox("Algoritmo genético");
    auto* formAG = new QFormLayout(grupoAG_);
    formAG->addRow("População:", populacao_);
    formAG->addRow("Cruzamento:", cruzamento_);
    formAG->addRow("Mutação por filho:", mutacao_);
    formAG->addRow("Elitismo:", elitismo_);
    formAG->addRow("Torneio (k):", torneio_);
    formAG->addRow("Máx. gerações:", maxGeracoes_);
    formAG->addRow("Máx. sem melhora:", maxSemMelhora_);
    formAG->addRow("Semente do AG:", sementeAG_);

    botaoExecutar_ = new QPushButton("Executar algoritmo");
    botaoExecutar_->setMinimumHeight(34);

    // --- Resultados ---
    lblMelhor_ = new QLabel("-");
    lblMedia_ = new QLabel("-");
    lblMelhoria_ = new QLabel("-");
    lblExcesso_ = new QLabel("-");
    lblTempo_ = new QLabel("-");
    lblConvergencia_ = new QLabel("Convergência: -");
    lblConvergencia_->setWordWrap(true);
    auto* grupoResultados = new QGroupBox("Geração mostrada");
    auto* formResultados = new QFormLayout(grupoResultados);
    formResultados->addRow("Melhor distância:", lblMelhor_);
    formResultados->addRow("Distância média:", lblMedia_);
    formResultados->addRow("Melhoria (geração 0):", lblMelhoria_);
    formResultados->addRow("Excesso sobre o ideal:", lblExcesso_);
    formResultados->addRow("Tempo acumulado:", lblTempo_);
    formResultados->addRow(lblConvergencia_);  // ocupa a linha inteira (texto longo)

    botaoCsv_ = new QPushButton("Salvar histórico (CSV)");
    botaoPng_ = new QPushButton("Salvar imagem (PNG)");

    auto* painel = new QWidget;
    auto* colunaEsquerda = new QVBoxLayout(painel);
    colunaEsquerda->addWidget(grupoProblema_);
    colunaEsquerda->addWidget(grupoAG_);
    colunaEsquerda->addWidget(botaoExecutar_);
    colunaEsquerda->addWidget(grupoResultados);
    colunaEsquerda->addWidget(botaoCsv_);
    colunaEsquerda->addWidget(botaoPng_);
    colunaEsquerda->addStretch(1);
    auto* rolagem = new QScrollArea;
    rolagem->setWidget(painel);
    rolagem->setWidgetResizable(true);
    rolagem->setFrameShape(QFrame::NoFrame);
    rolagem->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rolagem->setFixedWidth(330);

    // --- Visualizacao ---
    desenho_ = new WidgetRota;
    grafico_ = new WidgetGrafico;
    conteudo_ = new QWidget;
    auto* colunaConteudo = new QVBoxLayout(conteudo_);
    colunaConteudo->setContentsMargins(0, 0, 0, 0);
    colunaConteudo->setSpacing(2);
    colunaConteudo->addWidget(desenho_, 3);
    colunaConteudo->addWidget(grafico_, 2);

    botaoPlay_ = new QPushButton("▶ Reproduzir");
    velocidade_ = new QComboBox;
    velocidade_->addItem("Lenta", 300);   // milissegundos por passo
    velocidade_->addItem("Média", 120);
    velocidade_->addItem("Rápida", 30);
    velocidade_->setCurrentIndex(1);
    velocidade_->setToolTip("Velocidade da reprodução. Com o slider selecionado, "
                            "as setas esquerda e direita avançam uma geração por vez.");
    slider_ = new QSlider(Qt::Horizontal);
    slider_->setRange(0, 0);
    lblEpoca_ = new QLabel("Geração -");
    lblEpoca_->setMinimumWidth(130);
    auto* linhaSlider = new QHBoxLayout;
    linhaSlider->addWidget(botaoPlay_);
    linhaSlider->addWidget(velocidade_);
    linhaSlider->addWidget(slider_, 1);
    linhaSlider->addWidget(lblEpoca_);

    auto* direita = new QWidget;
    auto* colunaDireita = new QVBoxLayout(direita);
    colunaDireita->setContentsMargins(0, 0, 0, 0);
    colunaDireita->addWidget(conteudo_, 1);
    colunaDireita->addLayout(linhaSlider);

    auto* central = new QWidget;
    auto* linha = new QHBoxLayout(central);
    linha->addWidget(rolagem);
    linha->addWidget(direita, 1);
    setCentralWidget(central);
    resize(1180, 820);

    timer_ = new QTimer(this);
    timer_->setInterval(velocidade_->currentData().toInt());

    // --- Ligacoes ---
    connect(cenario_, &QComboBox::currentIndexChanged, this, &Janela::gerarPontos);
    connect(qtdPontos_, &QSpinBox::valueChanged, this, &Janela::gerarPontos);
    connect(sementePontos_, &QSpinBox::valueChanged, this, &Janela::gerarPontos);
    connect(botaoExecutar_, &QPushButton::clicked, this, &Janela::executarOuParar);
    connect(slider_, &QSlider::valueChanged, this, &Janela::mostrarEpoca);
    connect(grafico_, &WidgetGrafico::epocaEscolhida, slider_, &QSlider::setValue);
    connect(botaoPlay_, &QPushButton::clicked, this, &Janela::alternarReproducao);
    connect(timer_, &QTimer::timeout, this, &Janela::avancarReproducao);
    connect(velocidade_, &QComboBox::currentIndexChanged, this,
            [this] { timer_->setInterval(velocidade_->currentData().toInt()); });
    connect(botaoCsv_, &QPushButton::clicked, this, &Janela::salvarCsv);
    connect(botaoPng_, &QPushButton::clicked, this, &Janela::salvarPng);

    gerarPontos();  // mostra algo ja na abertura
}

Janela::~Janela() {
    if (thread_) {
        // Pede a parada e espera a thread terminar antes de destruir a janela.
        // O Trabalhador se apaga sozinho quando a thread termina
        // (finished -> deleteLater), entao nao deve ser apagado aqui.
        trabalhador_->pedirParada();
        thread_->wait();
    }
}

ParametrosAG Janela::lerParametros() const {
    ParametrosAG p;
    p.tamanho_populacao = populacao_->value();
    p.taxa_cruzamento = cruzamento_->value() / 100.0;
    p.taxa_mutacao = mutacao_->value() / 100.0;
    p.elitismo = elitismo_->value();
    p.tamanho_torneio = torneio_->value();
    p.max_geracoes = maxGeracoes_->value();
    p.max_sem_melhora = maxSemMelhora_->value();
    p.semente = static_cast<std::uint32_t>(sementeAG_->value());
    return p;
}

void Janela::gerarPontos() {
    if (executando_) return;
    try {
        pararReproducao();
        const std::string nome = cenario_->currentData().toString().toStdString();
        const int n = qtdPontos_->value();
        pontosAtuais_ = gerar_pontos(nome, n, static_cast<std::uint32_t>(sementePontos_->value()));
        ideal_ = nome == "circular" ? distancia_ideal_circulo(n) : 0.0;
        resultado_.reset();

        const auto rota = criar_rota_aleatoria(n, static_cast<std::uint32_t>(sementeAG_->value()));
        const double distancia = distancia_total(rota, pontosAtuais_);
        desenho_->definir(pontosAtuais_, rota);
        desenho_->definirTitulo("Rota aleatória de referência");
        grafico_->limpar();
        grafico_->definirIdeal(ideal_);
        grafico_->definirInterativo(false);

        slider_->blockSignals(true);
        slider_->setRange(0, 0);
        slider_->setValue(0);
        slider_->blockSignals(false);
        lblEpoca_->setText("Geração -");
        lblMelhor_->setText(dist(distancia));
        lblMedia_->setText("-");
        lblMelhoria_->setText("-");
        lblExcesso_->setText(ideal_ > 0 ? QString("%1 (ideal %2)")
                                              .arg(100.0 * (distancia / ideal_ - 1.0), 0, 'f', 1)
                                              .arg(dist(ideal_))
                                        : QString("-"));
        lblTempo_->setText("-");
        lblConvergencia_->setText("Convergência: -");
        statusBar()->showMessage("Pontos gerados. Ajuste os parâmetros e clique em Executar.");
        atualizarBotoes();
    } catch (const std::exception& erro) {
        QMessageBox::warning(this, "Erro", QString::fromUtf8(erro.what()));
    }
}

void Janela::executarOuParar() {
    if (executando_) {
        if (trabalhador_) {
            trabalhador_->pedirParada();
            botaoExecutar_->setEnabled(false);
            statusBar()->showMessage("Parando...");
        }
        return;
    }
    pararReproducao();
    parametrosUsados_ = lerParametros();
    resultado_.reset();
    grafico_->limpar();
    grafico_->definirIdeal(ideal_);
    slider_->blockSignals(true);
    slider_->setRange(0, 0);
    slider_->blockSignals(false);
    lblEpoca_->setText("Executando...");
    lblConvergencia_->setText("Convergência: -");

    thread_ = new QThread(this);
    trabalhador_ = new Trabalhador(pontosAtuais_, parametrosUsados_);
    trabalhador_->moveToThread(thread_);
    connect(thread_, &QThread::started, trabalhador_, &Trabalhador::executar);
    connect(trabalhador_, &Trabalhador::progresso, this, &Janela::aoProgresso);
    connect(trabalhador_, &Trabalhador::terminou, this, &Janela::aoTerminar);
    connect(trabalhador_, &Trabalhador::falhou, this, &Janela::aoFalhar);
    // Direta: roda na thread do algoritmo, sem depender do laco de eventos da janela.
    connect(trabalhador_, &Trabalhador::finalizado, thread_, &QThread::quit, Qt::DirectConnection);
    connect(thread_, &QThread::finished, trabalhador_, &QObject::deleteLater);
    connect(thread_, &QThread::finished, this, &Janela::aoThreadParar);
    definirExecutando(true);
    statusBar()->showMessage("Executando...");
    thread_->start();
}

void Janela::aoProgresso(const Progresso& pacote) {
    grafico_->acrescentar(pacote.primeira, pacote.melhores, pacote.medias);
    grafico_->definirEpoca(-1);
    desenho_->definir(pontosAtuais_, pacote.rota);
    desenho_->definirTitulo(QString("Executando... geração %1").arg(pacote.geracao));
    lblEpoca_->setText(QString("Geração %1").arg(pacote.geracao));
    const double melhor = pacote.melhores.back();
    lblMelhor_->setText(dist(melhor));
    lblMedia_->setText(dist(pacote.medias.back()));
    lblTempo_->setText(QString("%1 s").arg(pacote.tempo, 0, 'f', 3));
    if (ideal_ > 0) {
        const double excesso = std::max(0.0, 100.0 * (melhor / ideal_ - 1.0));
        lblExcesso_->setText(QString("%1 %").arg(excesso, 0, 'f', 2));
    }
}

void Janela::aoTerminar(std::shared_ptr<const ResultadoAG> resultado, bool interrompido) {
    if (!resultado) {
        statusBar()->showMessage("Execução interrompida sem resultado.");
        return;
    }
    resultado_ = std::move(resultado);
    const auto& historico = resultado_->historico;
    std::vector<double> melhores, medias;
    melhores.reserve(historico.size());
    medias.reserve(historico.size());
    for (const auto& r : historico) {
        melhores.push_back(r.melhor_distancia);
        medias.push_back(r.distancia_media);
    }
    grafico_->definirDados(std::move(melhores), std::move(medias));

    const int ultima = static_cast<int>(historico.size()) - 1;
    slider_->blockSignals(true);
    slider_->setRange(0, ultima);
    slider_->blockSignals(false);

    // Primeira geracao em que a melhor distancia final foi alcancada.
    int convergiu = ultima;
    for (const auto& r : historico)
        if (r.melhor_distancia <= resultado_->melhor_distancia * (1 + 1e-12)) { convergiu = r.geracao; break; }
    lblConvergencia_->setText(QString("Convergência: melhor valor na geração %1 (%2 s) de %3")
                                  .arg(convergiu)
                                  .arg(historico[static_cast<std::size_t>(convergiu)].tempo, 0, 'f', 3)
                                  .arg(ultima));
    mostrarEpoca(ultima);

    QString motivo;
    if (interrompido) motivo = "interrompido pelo usuário";
    else if (ultima >= parametrosUsados_.max_geracoes) motivo = "limite de gerações";
    else motivo = QString("%1 gerações sem melhora").arg(parametrosUsados_.max_sem_melhora);
    statusBar()->showMessage(QString("Concluído: %1 gerações em %2 s, melhor distância %3 (%4).")
                                 .arg(ultima)
                                 .arg(resultado_->tempo, 0, 'f', 3)
                                 .arg(dist(resultado_->melhor_distancia))
                                 .arg(motivo));
}

void Janela::aoFalhar(const QString& mensagem) {
    statusBar()->showMessage("Erro: " + mensagem);
    QMessageBox::warning(this, "Erro", mensagem);
}

void Janela::aoThreadParar() {
    thread_->deleteLater();  // o Trabalhador ja se apaga sozinho (finished -> deleteLater)
    thread_ = nullptr;
    trabalhador_ = nullptr;
    definirExecutando(false);
    if (!resultado_) {
        // Falha ou parada sem resultado: volta a mostrar os pontos.
        desenho_->definirTitulo("Sem resultado");
        lblEpoca_->setText("Geração -");
    }
}

void Janela::mostrarEpoca(int geracao) {
    if (!resultado_ || geracao < 0 || static_cast<std::size_t>(geracao) >= resultado_->historico.size()) return;
    const auto& r = resultado_->historico[static_cast<std::size_t>(geracao)];
    const auto& r0 = resultado_->historico.front();
    const int ultima = static_cast<int>(resultado_->historico.size()) - 1;

    slider_->blockSignals(true);
    slider_->setValue(geracao);
    slider_->blockSignals(false);
    grafico_->definirEpoca(geracao);
    desenho_->definir(pontosAtuais_, r.melhor_rota);
    desenho_->definirTitulo(QString("Melhor rota da geração %1 de %2").arg(geracao).arg(ultima));
    lblEpoca_->setText(QString("Geração %1 / %2").arg(geracao).arg(ultima));
    lblMelhor_->setText(dist(r.melhor_distancia));
    lblMedia_->setText(dist(r.distancia_media));
    lblMelhoria_->setText(QString("%1 %").arg(100.0 * (r0.melhor_distancia - r.melhor_distancia) / r0.melhor_distancia, 0, 'f', 1));
    if (ideal_ > 0) {
        // Arredondamento pode dar -0,0000...; nao mostrar excesso negativo.
        const double excesso = std::max(0.0, 100.0 * (r.melhor_distancia / ideal_ - 1.0));
        lblExcesso_->setText(QString("%1 % (ideal %2)").arg(excesso, 0, 'f', 2).arg(dist(ideal_)));
    } else {
        lblExcesso_->setText("- (ótimo desconhecido)");
    }
    lblTempo_->setText(QString("%1 s (total %2 s)").arg(r.tempo, 0, 'f', 3).arg(resultado_->tempo, 0, 'f', 3));
}

void Janela::alternarReproducao() {
    if (timer_->isActive()) {
        pararReproducao();
        return;
    }
    if (!resultado_ || resultado_->historico.size() < 2) return;
    if (slider_->value() >= slider_->maximum()) mostrarEpoca(0);  // recomeca do inicio
    botaoPlay_->setText("⏸ Pausar");
    timer_->start();
}

void Janela::avancarReproducao() {
    // Percorre o historico em cerca de 200 passos, qualquer que seja o tamanho dele.
    const int passo = std::max(1, slider_->maximum() / 200);
    const int proxima = std::min(slider_->value() + passo, slider_->maximum());
    mostrarEpoca(proxima);
    if (proxima >= slider_->maximum()) pararReproducao();
}

void Janela::pararReproducao() {
    timer_->stop();
    botaoPlay_->setText("▶ Reproduzir");
}

void Janela::definirExecutando(bool executando) {
    executando_ = executando;
    grupoProblema_->setEnabled(!executando);
    grupoAG_->setEnabled(!executando);
    botaoExecutar_->setText(executando ? "Parar" : "Executar algoritmo");
    botaoExecutar_->setEnabled(true);
    atualizarBotoes();
}

void Janela::atualizarBotoes() {
    const bool temResultado = resultado_ && !executando_;
    slider_->setEnabled(temResultado && resultado_->historico.size() > 1);
    botaoPlay_->setEnabled(temResultado && resultado_->historico.size() > 1);
    botaoCsv_->setEnabled(temResultado);
    botaoPng_->setEnabled(temResultado || !pontosAtuais_.empty());
    grafico_->definirInterativo(temResultado);
}

bool Janela::escreverCsv(const QString& caminho) const {
    if (!resultado_) return false;
    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream saida(&arquivo);
    saida << "geracao,melhor_distancia,distancia_media,tempo_segundos\n";
    for (const auto& r : resultado_->historico)
        saida << r.geracao << ',' << QString::number(r.melhor_distancia, 'g', 17) << ','
              << QString::number(r.distancia_media, 'g', 17) << ','
              << QString::number(r.tempo, 'g', 9) << '\n';
    saida.flush();
    return saida.status() == QTextStream::Ok;
}

bool Janela::escreverPng(const QString& caminho) const {
    return conteudo_->grab().save(caminho, "PNG");
}

void Janela::salvarCsv() {
    const QString padrao = QString("historico_%1_%2pontos.csv")
                               .arg(cenario_->currentData().toString()).arg(qtdPontos_->value());
    const QString caminho = QFileDialog::getSaveFileName(this, "Salvar histórico", padrao, "CSV (*.csv)");
    if (caminho.isEmpty()) return;
    if (escreverCsv(caminho)) statusBar()->showMessage("Histórico salvo em " + caminho);
    else QMessageBox::warning(this, "Erro", "Não foi possível salvar o arquivo.");
}

void Janela::salvarPng() {
    const int geracao = resultado_ ? slider_->value() : 0;
    const QString padrao = QString("%1_%2pontos_geracao%3.png")
                               .arg(cenario_->currentData().toString()).arg(qtdPontos_->value()).arg(geracao);
    const QString caminho = QFileDialog::getSaveFileName(this, "Salvar imagem", padrao, "PNG (*.png)");
    if (caminho.isEmpty()) return;
    if (escreverPng(caminho)) statusBar()->showMessage("Imagem salva em " + caminho);
    else QMessageBox::warning(this, "Erro", "Não foi possível salvar a imagem.");
}
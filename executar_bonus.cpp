// Bonus: circulo com muitos pontos.
// Mede o tempo ate o algoritmo genetico atingir a distancia ideal e mostra a
// melhor rota em geracoes intermediarias (SVG), alem da curva de convergencia.
#include "cenarios.hpp"
#include "ciclo.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr double TOLERANCIA = 1e-8;
const int TAMANHOS[] = {100, 200};
constexpr int SEMENTES = 5;

std::ofstream abrir(const fs::path& caminho) {
    std::ofstream f;
    f.exceptions(std::ios::failbit | std::ios::badbit);
    f.open(caminho);
    f << std::setprecision(17);
    return f;
}

// Primeira geracao cuja melhor distancia atinge `alvo`; -1 se nenhuma atingir.
int primeira_geracao_ate(const ResultadoAG& r, double alvo) {
    for (const auto& g : r.historico)
        if (g.melhor_distancia <= alvo + TOLERANCIA) return g.geracao;
    return -1;
}

// Geracoes mostradas: inicial, 5%, 10%, 25%, 50% e 100% da geracao de referencia.
std::vector<int> escolher_geracoes(int referencia) {
    std::set<int> g = {0, referencia / 20, referencia / 10, referencia / 4, referencia / 2, referencia};
    return {g.begin(), g.end()};
}

void painel_rota(std::ostream& svg, const std::vector<Ponto>& pontos, const RegistroGeracao& g,
                 double ideal, double x0, double y0) {
    const double lado = 290, margem = 12;
    const double escala = (lado - 2 * margem) / 100.0;
    auto px = [&](double x) { return x0 + margem + escala * x; };
    auto py = [&](double y) { return y0 + margem + escala * (100 - y); };
    svg << "<rect x=\"" << x0 << "\" y=\"" << y0 << "\" width=\"" << lado << "\" height=\"" << lado
        << "\" fill=\"#fff\" stroke=\"#bbb\"/>\n<polygon fill=\"none\" stroke=\"#1f5fae\" stroke-width=\"1\" points=\"";
    for (int i : g.melhor_rota) svg << px(pontos[i].x) << ',' << py(pontos[i].y) << ' ';
    svg << "\"/>\n";
    for (std::size_t k = 0; k < g.melhor_rota.size(); ++k) {
        const Ponto& p = pontos[g.melhor_rota[k]];
        svg << "<circle cx=\"" << px(p.x) << "\" cy=\"" << py(p.y) << "\" r=\"" << (k == 0 ? 3.2 : 1.8)
            << "\" fill=\"" << (k == 0 ? "#c0392b" : "#222") << "\"/>\n";
    }
    svg << std::fixed << "<text x=\"" << x0 + 6 << "\" y=\"" << y0 + lado + 17 << "\" font-size=\"13\">Geração "
        << std::setprecision(0) << g.geracao << " · tempo " << std::setprecision(3) << g.tempo << " s</text>\n"
        << "<text x=\"" << x0 + 6 << "\" y=\"" << y0 + lado + 34 << "\" font-size=\"12\" fill=\"#555\">distância "
        << std::setprecision(2) << g.melhor_distancia << " (" << std::setprecision(1)
        << 100 * (g.melhor_distancia / ideal - 1) << "% acima do ideal)</text>\n" << std::setprecision(4);
}

void exportar_rotas(const fs::path& arquivo, int n, const std::vector<Ponto>& pontos, const ResultadoAG& r,
                    const std::vector<int>& geracoes, double ideal) {
    auto svg = abrir(arquivo);
    svg << std::fixed << std::setprecision(4);
    const int colunas = 3;
    const int linhas = static_cast<int>((geracoes.size() + colunas - 1) / colunas);
    const double largura = 20 + colunas * 305, altura = 60 + linhas * 355;
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << largura << "\" height=\"" << altura
        << "\" viewBox=\"0 0 " << largura << ' ' << altura << "\" font-family=\"sans-serif\">\n"
        << "<rect width=\"100%\" height=\"100%\" fill=\"#fafafa\"/>\n"
        << "<text x=\"15\" y=\"28\" font-size=\"18\" font-weight=\"bold\">Círculo com " << n
        << " pontos: melhor rota em gerações intermediárias (distância ideal " << ideal << ")</text>\n";
    for (std::size_t i = 0; i < geracoes.size(); ++i) {
        const auto& g = r.historico[geracoes[i]];
        if (std::abs(distancia_total(g.melhor_rota, pontos) - g.melhor_distancia) > 1e-8)
            throw std::runtime_error("Distancia do historico inconsistente.");
        painel_rota(svg, pontos, g, ideal, 10 + (i % colunas) * 305, 45 + (i / colunas) * 355);
    }
    svg << "</svg>\n";
}

void exportar_convergencia(const fs::path& arquivo, int n, const ResultadoAG& r, double ideal, int g_ideal) {
    auto svg = abrir(arquivo);
    svg << std::fixed << std::setprecision(2);
    const double L = 760, A = 440, esq = 70, dir = 20, topo = 50, base = 60;
    const double ymin = std::log10(ideal * 0.9), ymax = std::log10(r.historico.front().melhor_distancia * 1.1);
    const double gmax = std::max(1, r.geracoes_executadas);
    auto X = [&](double g) { return esq + (L - esq - dir) * g / gmax; };
    auto Y = [&](double d) { return topo + (A - topo - base) * (ymax - std::log10(d)) / (ymax - ymin); };
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << L << "\" height=\"" << A << "\" viewBox=\"0 0 "
        << L << ' ' << A << "\" font-family=\"sans-serif\">\n<rect width=\"100%\" height=\"100%\" fill=\"#fff\"/>\n"
        << "<text x=\"" << esq << "\" y=\"28\" font-size=\"17\" font-weight=\"bold\">Convergência: círculo com " << n
        << " pontos (eixo vertical logarítmico)</text>\n";
    for (double v : {250.0, 500.0, 1000.0, 2000.0, 5000.0, 10000.0, 20000.0}) {
        if (v < ideal * 0.9 || v > r.historico.front().melhor_distancia * 1.1) continue;
        svg << "<line x1=\"" << esq << "\" x2=\"" << L - dir << "\" y1=\"" << Y(v) << "\" y2=\"" << Y(v)
            << "\" stroke=\"#e3e3e3\"/><text x=\"" << esq - 8 << "\" y=\"" << Y(v) + 4
            << "\" font-size=\"12\" text-anchor=\"end\">" << std::setprecision(0) << v << std::setprecision(2) << "</text>\n";
    }
    {
        const double bruto = gmax / 5;
        const double dec = std::pow(10.0, std::floor(std::log10(bruto)));
        const double passo = (bruto / dec < 1.5 ? 1 : bruto / dec < 3.5 ? 2 : bruto / dec < 7.5 ? 5 : 10) * dec;
        for (double g = 0; g <= gmax; g += passo)
            svg << "<text x=\"" << X(g) << "\" y=\"" << A - base + 20 << "\" font-size=\"12\" text-anchor=\"middle\">"
                << std::setprecision(0) << g << std::setprecision(2) << "</text>\n";
    }
    svg << "<text x=\"" << (esq + L - dir) / 2 << "\" y=\"" << A - 14
        << "\" font-size=\"13\" text-anchor=\"middle\">Geração</text>\n"
        << "<text x=\"16\" y=\"" << (topo + A - base) / 2 << "\" font-size=\"13\" text-anchor=\"middle\" transform=\"rotate(-90 16 "
        << (topo + A - base) / 2 << ")\">Distância (menor é melhor)</text>\n"
        << "<line x1=\"" << esq << "\" x2=\"" << L - dir << "\" y1=\"" << Y(ideal) << "\" y2=\"" << Y(ideal)
        << "\" stroke=\"#2e8b57\" stroke-dasharray=\"6 4\"/>\n<text x=\"" << L - dir - 4 << "\" y=\"" << Y(ideal) - 6
        << "\" font-size=\"12\" fill=\"#2e8b57\" text-anchor=\"end\">ideal " << ideal << "</text>\n";
    auto linha = [&](auto campo, const char* cor) {
        svg << "<polyline fill=\"none\" stroke=\"" << cor << "\" stroke-width=\"1.6\" points=\"";
        const std::size_t passo = std::max<std::size_t>(1, r.historico.size() / 800);
        for (std::size_t i = 0; i < r.historico.size(); i += passo)
            svg << X(r.historico[i].geracao) << ',' << Y(campo(r.historico[i])) << ' ';
        svg << X(r.historico.back().geracao) << ',' << Y(campo(r.historico.back())) << "\"/>\n";
    };
    linha([](const RegistroGeracao& g) { return g.distancia_media; }, "#e69f00");
    linha([](const RegistroGeracao& g) { return g.melhor_distancia; }, "#1f5fae");
    if (g_ideal >= 0)
        svg << "<line x1=\"" << X(g_ideal) << "\" x2=\"" << X(g_ideal) << "\" y1=\"" << topo << "\" y2=\"" << A - base
            << "\" stroke=\"#c0392b\" stroke-dasharray=\"3 3\"/><text x=\"" << X(g_ideal) + 5 << "\" y=\"" << topo + 14
            << "\" font-size=\"12\" fill=\"#c0392b\">ideal na geração " << g_ideal << "</text>\n";
    svg << "<rect x=\"" << esq + 12 << "\" y=\"" << topo + 28 << "\" width=\"14\" height=\"3\" fill=\"#1f5fae\"/><text x=\""
        << esq + 32 << "\" y=\"" << topo + 34 << "\" font-size=\"12\">melhor da geração</text>\n<rect x=\"" << esq + 12
        << "\" y=\"" << topo + 46 << "\" width=\"14\" height=\"3\" fill=\"#e69f00\"/><text x=\"" << esq + 32 << "\" y=\""
        << topo + 52 << "\" font-size=\"12\">média da população</text>\n</svg>\n";
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::invalid_argument("Uso: ./build/executar_bonus pasta_nova");
        const fs::path pasta(argv[1]);
        if (fs::exists(pasta)) throw std::invalid_argument("Use uma pasta nova; resultados existentes sao preservados.");
        fs::create_directories(pasta);

        auto resumo = abrir(pasta / "resumo_bonus.csv");
        resumo << "pontos,semente_ag,populacao,taxa_mutacao,max_geracoes,limite_sem_melhoria,geracoes_executadas,"
                  "atingiu_ideal,geracao_ideal,tempo_ate_ideal_s,tempo_total_s,melhor_distancia,distancia_ideal,"
                  "excesso_ideal_percentual\n";

        for (int n : TAMANHOS) {
            const auto pontos = gerar_pontos("circular", n, 42);
            const double ideal = distancia_ideal_circulo(n);
            for (int semente = 1; semente <= SEMENTES; ++semente) {
                ParametrosAG p;
                p.tamanho_populacao = 200;
                p.taxa_mutacao = 0.10;
                p.max_geracoes = 30000;
                p.max_sem_melhora = 2000;
                p.semente = semente;
                const auto r = executar_ag(pontos, p);

                if (r.historico.size() != static_cast<std::size_t>(r.geracoes_executadas + 1))
                    throw std::runtime_error("Historico incompleto.");
                if (std::abs(distancia_total(r.melhor_rota, pontos) - r.melhor_distancia) > 1e-8 ||
                    r.melhor_distancia < ideal - 1e-8)
                    throw std::runtime_error("Resultado final invalido.");

                const int g_ideal = primeira_geracao_ate(r, ideal);
                resumo << n << ',' << semente << ',' << p.tamanho_populacao << ',' << p.taxa_mutacao << ','
                       << p.max_geracoes << ',' << p.max_sem_melhora << ',' << r.geracoes_executadas << ','
                       << (g_ideal >= 0 ? "sim" : "nao") << ',';
                if (g_ideal >= 0) resumo << g_ideal << ',' << r.historico[g_ideal].tempo; else resumo << ',';
                resumo << ',' << r.tempo << ',' << r.melhor_distancia << ',' << ideal << ','
                       << 100 * (r.melhor_distancia / ideal - 1) << '\n';
                std::cout << n << " pontos, semente " << semente << ": " << r.geracoes_executadas << " geracoes, ";
                if (g_ideal >= 0)
                    std::cout << "ideal na geracao " << g_ideal << " (" << r.historico[g_ideal].tempo << " s)\n";
                else
                    std::cout << "ideal nao atingido (" << r.melhor_distancia << " contra " << ideal << ")\n";

                if (semente == 1) { // semente ilustrativa, fixa (nao escolhida pelo resultado)
                    const int referencia = g_ideal >= 0 ? g_ideal : r.geracoes_executadas;
                    exportar_rotas(pasta / ("rotas_intermediarias_" + std::to_string(n) + ".svg"), n, pontos, r,
                                   escolher_geracoes(referencia), ideal);
                    exportar_convergencia(pasta / ("convergencia_" + std::to_string(n) + ".svg"), n, r, ideal, g_ideal);
                    auto h = abrir(pasta / ("historico_" + std::to_string(n) + ".csv"));
                    h << "geracao,melhor_distancia,distancia_media,tempo_segundos\n";
                    for (const auto& g : r.historico)
                        h << g.geracao << ',' << g.melhor_distancia << ',' << g.distancia_media << ',' << g.tempo << '\n';
                }
            }
        }
        std::cout << "Bonus concluido em " << pasta << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
}

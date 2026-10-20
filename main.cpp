#include "cenarios.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

std::ofstream abrir_saida(const fs::path& destino) {
    std::ofstream arquivo;
    arquivo.exceptions(std::ios::failbit | std::ios::badbit);
    arquivo.open(destino);
    return arquivo;
}

void salvar_grafico(const std::vector<Ponto>& pontos, const Rota& rota,
                    double distancia, const fs::path& destino) {
    auto svg = abrir_saida(destino);
    svg << std::fixed << std::setprecision(4)
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"620\" height=\"640\" viewBox=\"0 0 620 640\">\n"
        << "<rect width=\"620\" height=\"640\" fill=\"white\"/>\n"
        << "<g font-family=\"sans-serif\"><text x=\"30\" y=\"30\" font-size=\"20\">Rota inicial aleatória</text>\n"
        << "<rect x=\"60\" y=\"60\" width=\"500\" height=\"500\" fill=\"#f8fafc\" stroke=\"#cbd5e1\"/>\n"
        << "<polyline fill=\"none\" stroke=\"#2563eb\" stroke-width=\"2\" points=\"";
    for (std::size_t i = 0; i <= rota.size(); ++i) {
        const auto& p = pontos[rota[i % rota.size()]];
        svg << 60 + 5 * p.x << ',' << 560 - 5 * p.y << ' ';
    }
    svg << "\"/>\n";
    for (std::size_t i = 0; i < pontos.size(); ++i) {
        const double x = 60 + 5 * pontos[i].x, y = 560 - 5 * pontos[i].y;
        svg << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"5\" fill=\""
            << (static_cast<int>(i) == rota.front() ? "#dc2626" : "#0f172a") << "\"/>\n"
            << "<text x=\"" << x + 8 << "\" y=\"" << y - 8 << "\" font-size=\"13\">" << i << "</text>\n";
    }
    svg << "<text x=\"30\" y=\"600\" font-size=\"16\">Distância total: " << distancia
        << " | " << pontos.size() << " pontos</text>\n"
        << "<text x=\"30\" y=\"625\" font-size=\"13\">Vermelho: início e fim. Eixos de 0 a 100 com a mesma escala.</text></g></svg>\n";
    svg.close();
}

void salvar_dados(const std::vector<Ponto>& pontos, const Rota& rota, double distancia,
                  const std::string& cenario, std::uint32_t semente, const fs::path& destino) {
    auto json = abrir_saida(destino);
    json << std::setprecision(17) << "{\n  \"cenario\": \"" << cenario
         << "\",\n  \"semente\": " << semente << ",\n  \"pontos\": [";
    for (std::size_t i = 0; i < pontos.size(); ++i)
        json << (i ? "," : "") << "\n    [" << pontos[i].x << ", " << pontos[i].y << ']';
    json << "\n  ],\n  \"rota\": [";
    for (std::size_t i = 0; i < rota.size(); ++i) json << (i ? ", " : "") << rota[i];
    json << "],\n  \"distancia\": " << distancia;
    if (cenario == "circular") {
        const double ideal = distancia_ideal_circulo(static_cast<int>(pontos.size()));
        json << ",\n  \"distancia_ideal\": " << ideal
             << ",\n  \"excesso_percentual\": " << 100 * (distancia / ideal - 1);
    }
    json << "\n}\n";
    json.close();
}

unsigned long long inteiro(const std::string& texto) {
    if (texto.empty() || texto.find_first_not_of("0123456789") != std::string::npos)
        throw std::invalid_argument("Numero inteiro invalido: " + texto);
    return std::stoull(texto);
}

int main(int argc, char* argv[]) {
    try {
        std::string cenario = "aleatorio";
        int quantidade = 10;
        std::uint32_t semente = 42;
        fs::path saida = "resultados";
        for (int i = 1; i < argc; ++i) {
            const std::string opcao = argv[i];
            if (opcao == "--help" || opcao == "-h") {
                std::cout << "Uso: ./caixeiro [--cenario aleatorio|circular] [--pontos 10] [--semente 42] [--saida resultados]\n";
                return 0;
            }
            if (i + 1 >= argc) throw std::invalid_argument("Falta valor para " + opcao);
            const std::string valor = argv[++i];
            if (opcao == "--cenario") cenario = valor;
            else if (opcao == "--saida") saida = valor;
            else if (opcao == "--pontos") {
                const auto n = inteiro(valor);
                if (n < 8 || n > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
                    throw std::invalid_argument("Use entre 8 e INT_MAX pontos.");
                quantidade = static_cast<int>(n);
            } else if (opcao == "--semente") {
                const auto n = inteiro(valor);
                if (n > std::numeric_limits<std::uint32_t>::max())
                    throw std::invalid_argument("Semente deve caber em 32 bits sem sinal.");
                semente = static_cast<std::uint32_t>(n);
            } else throw std::invalid_argument("Opcao desconhecida: " + opcao);
        }
        const auto pontos = gerar_pontos(cenario, quantidade, semente);
        const auto rota = criar_rota_aleatoria(quantidade, semente);
        const double distancia = distancia_total(rota, pontos);
        fs::create_directories(saida);
        const auto nome = cenario + "_" + std::to_string(quantidade) + "_semente_" + std::to_string(semente);
        const auto grafico = saida / (nome + ".svg");
        salvar_grafico(pontos, rota, distancia, grafico);
        salvar_dados(pontos, rota, distancia, cenario, semente, saida / (nome + ".json"));
        std::cout << std::fixed << std::setprecision(4) << "Rota fechada: ";
        for (int indice : rota) std::cout << indice << " -> ";
        std::cout << rota.front() << "\nDistancia total: " << distancia << '\n';
        if (cenario == "circular") std::cout << "Distancia ideal: " << distancia_ideal_circulo(quantidade) << '\n';
        std::cout << "Grafico: " << fs::absolute(grafico) << '\n';
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}

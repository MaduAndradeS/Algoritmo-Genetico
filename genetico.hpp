#pragma once

#include <random>
#include <vector>

#include "cenarios.hpp"

// Operadores geneticos para o caixeiro viajante.
// Uma Rota e uma permutacao de 0..n-1 (sem repetir o inicio no final).
// Todos os operadores recebem o gerador por referencia, nao alteram os
// argumentos e devolvem rotas novas.

// Cria `tamanho` rotas aleatorias com os indices 0..quantidade_pontos-1.
std::vector<Rota> criar_populacao(int quantidade_pontos, int tamanho, std::mt19937& rng);

// Sorteia k individuos (com reposicao) e devolve uma copia do de menor
// distancia. distancias[i] e a distancia de populacao[i].
Rota selecionar_torneio(const std::vector<Rota>& populacao,
                        const std::vector<double>& distancias,
                        std::mt19937& rng, int k = 3);

// Cruzamento OX (Order Crossover): o filho herda um trecho de pai1 nas mesmas
// posicoes e recebe as demais cidades na ordem em que aparecem em pai2.
Rota cruzar_ox(const Rota& pai1, const Rota& pai2, std::mt19937& rng);

// Mutacao por inversao: copia a rota e inverte um trecho sorteado.
Rota mutar_inversao(const Rota& rota, std::mt19937& rng);
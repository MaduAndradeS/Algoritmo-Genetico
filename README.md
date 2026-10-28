# Caixeiro viajante com algoritmo genético

Projeto em C++17 com seleção por torneio, cruzamento OX, mutação por inversão,
elitismo e interface gráfica Qt 6. Há experimentos comparativos já executados
e uma explicação para apoiar o relatório e a apresentação.

## Abrir a interface gráfica

Requisitos: compilador C++17, CMake e Qt 6 Widgets. No terminal:

```bash
cd "/home/guimasca/Documents/Machine Learning/Algoritimo_Genético/Algoritmo-Gen-tico"
cmake -S . -B build/qt
cmake --build build/qt -j 2
./build/qt/caixeiro_qt
```

Escolha o cenário e clique em **Executar algoritmo**. A interface apresenta
a rota, melhor distância e média da população, curva de evolução e navegação
pelas gerações. Use **Reproduzir**, **Salvar histórico (CSV)** e
**Salvar imagem (PNG)** para explorar e exportar a execução.

## Ver os experimentos concluídos e a explicação

Abra `experimentos/comparativos/relatorio.html` no navegador pelo gerenciador
de arquivos. Essa pasta contém resultados reais de **200 execuções**,
tabelas e gráficos de comparação, convergência e rotas iniciais/finais.

- [Explicação do algoritmo e roteiro de apresentação](experimentos/EXPLICACAO.md).
- [Resultados, conclusões e limitações](experimentos/comparativos/RESULTADOS.md).
- [Protocolo dos experimentos](experimentos/PROTOCOLO.md).

São cinco configurações, dois cenários (aleatório e circular), dois tamanhos
(10 e 30 pontos) e dez sementes por configuração. Os CSVs contêm resultados
individuais, resumo estatístico, histórico, pontos e rotas. A pasta concluída
é independente das antigas pastas `dados` e `dados_rodada_2`, que continham
somente o planejamento.

## Executar novas comparações

Requisitos: compilador C++17, Make e Python 3; não precisa de Qt nem de
pacotes Python externos. Na raiz do projeto:

```bash
make comparativos SAIDA=experimentos/nova_comparacao
```

Esse comando **executa o algoritmo 200 vezes** e depois gera tabelas,
relatório HTML e gráficos SVG. Use um nome de pasta que ainda não exista:
o executor recusa sobrescrever rodadas anteriores. Os tempos variam entre
execuções e computadores, mesmo quando as rotas se repetem.

Para refazer somente a análise dos resultados já existentes:

```bash
python3 analisar_experimentos.py experimentos/comparativos
```

O comando antigo `make experimentos` apenas prepara tabelas vazias em
`experimentos/dados`; não executa a busca e não gera os gráficos comparativos.
Ele foi mantido para preservar a primeira etapa do trabalho.

## Verificar o código

```bash
make verificar verificar_genetico verificar_ciclo
```

Os testes verificam cenários, distâncias, rotas inválidas, sementes,
operadores, preservação das permutações e funcionamento do ciclo.
O executor comparativo também recalcula as distâncias das melhores rotas,
confere o histórico, o elitismo e o limite ideal circular antes de exportar.
A análise confere os 200 IDs, grupos, sementes e históricos.

## Programa inicial pelo terminal

```bash
make
./caixeiro --cenario aleatorio --pontos 10 --semente 42
./caixeiro --cenario circular --pontos 10 --semente 42
```

Esse executável da primeira etapa ainda mostra somente uma rota aleatória
como referência. Seus SVGs e JSONs ficam em `resultados`. Para acompanhar o
algoritmo genético, use a interface Qt ou o executor comparativo.
Execuções iguais do programa inicial sobrescrevem suas saídas; use
`--saida outra_pasta` para preservar resultados separados.

## Organização do código

| Arquivos | Responsabilidade |
| --- | --- |
| `cenarios.hpp/.cpp` | Pontos, rotas aleatórias, distância total e referência circular. |
| `genetico.hpp/.cpp` | População, seleção por torneio, cruzamento OX e inversão. |
| `ciclo.hpp/.cpp` | Avaliação, elitismo, evolução, parada e histórico. |
| `main_qt.cpp`, `janela.*`, `widget*`, `trabalhador.*` | Interface, desenhos e execução em thread de trabalho. |
| `main.cpp` | Programa inicial de referência, sem evolução. |
| `preparar_experimentos.cpp` | Planejamento da primeira etapa, sem executar o AG. |
| `executar_experimentos.cpp` | Execução sequencial das comparações e exportação dos dados. |
| `analisar_experimentos.py` | Validação dos CSVs, estatísticas, gráficos e relatório. |

O contrato usa `Ponto { double x, y; }` e `Rota = std::vector<int>`: uma
permutação dos índices sem repetir a cidade inicial no fim. A função
`distancia_total` inclui o retorno. Menor distância é melhor.

`executar_ag(pontos, parametros, callback)` devolve `ResultadoAG` com melhor
rota, distância, tempo, gerações e histórico. O registro zero representa a
população inicial. O callback opcional permite atualizações por geração;
a interface encaminha esses dados por sinais Qt.

Parâmetros iniciais: população 100, cruzamento 90%, mutação 10% por filho,
elitismo 1, torneio 3, limite de 1.000 gerações ou 200 sem melhoria estrita.
Os pontos aleatórios são uniformes no quadrado [0,100] × [0,100]. O círculo
tem centro (50,50), raio 40 e pontos igualmente espaçados.

A mesma semente repete a busca no mesmo ambiente C++; bibliotecas padrão
diferentes podem gerar sequências diferentes. O ambiente e os hashes das
fontes da rodada entregue estão junto aos resultados.

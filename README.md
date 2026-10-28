# Caixeiro viajante com algoritmo genético

Projeto em C++ que resolve o problema do caixeiro viajante em duas dimensões
com um algoritmo genético: seleção por torneio, cruzamento OX, mutação por
inversão e elitismo. Tem interface gráfica em Qt 6, experimentos comparativos já
executados (200 execuções), um teste bônus com círculos de 100 e 200 pontos e uma
explicação do algoritmo e dos resultados.

Dois cenários: **pontos aleatórios** uniformes e **círculo de pontos**
(benchmark com solução ideal conhecida).

## Estrutura do repositório

| Arquivos | Responsabilidade |
| --- | --- |
| `cenarios.hpp/.cpp` | Pontos, rotas aleatórias, distância total e referência circular. |
| `genetico.hpp/.cpp` | População, seleção por torneio, cruzamento OX e mutação por inversão. |
| `ciclo.hpp/.cpp` | Avaliação, elitismo, evolução, critério de parada e histórico por geração. |
| `main_qt.cpp`, `janela.*`, `widgetrota.*`, `widgetgrafico.*`, `trabalhador.*` | Interface Qt: janela, desenho da rota, curva de convergência e execução em outra thread. |
| `CMakeLists.txt` | Compilação da interface Qt. |
| `executar_experimentos.cpp` | Executa as comparações e exporta os dados em CSV. |
| `executar_bonus.cpp` | Bônus: círculo com 100 e 200 pontos, tempo de convergência e rotas em gerações intermediárias. |
| `analisar_experimentos.py` | Valida os CSVs e gera estatísticas, gráficos e relatório HTML. |
| `verificar_cenarios.cpp`, `verificar_genetico.cpp`, `verificar_ciclo.cpp` | Testes automáticos, sem Qt. |
| `main.cpp` | Programa da primeira etapa: mostra apenas uma rota aleatória de referência. |
| `Makefile` | Atalhos para testes, experimentos e o programa da primeira etapa. |
| `experimentos/` | Explicação (`EXPLICACAO.md`), protocolo (`PROTOCOLO.md`), bônus (`BONUS.md`, `bonus/`) e resultados (`comparativos/`). |

## Abrir a interface gráfica

Requisitos: compilador C++, CMake e Qt 6 (módulo Widgets).

**Linux ou macOS:**

```bash
cmake -S . -B build/qt
cmake --build build/qt -j 2
./build/qt/caixeiro_qt
```

**Windows (PowerShell com Qt, MinGW, CMake e Ninja instalados pelo instalador do
Qt).** Ajuste a versão e as pastas para a sua instalação:

```powershell
$env:PATH = "C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.2\mingw_64\bin;" + $env:PATH
cmake -S . -B build/qt -G Ninja -DCMAKE_PREFIX_PATH="C:\Qt\6.11.2\mingw_64"
cmake --build build/qt
.\build\qt\caixeiro_qt.exe
```

Se o CMake não encontrar o Qt, informe a pasta da instalação em
`-DCMAKE_PREFIX_PATH`. Outra opção é abrir o `CMakeLists.txt` no Qt Creator.

Na janela, escolha o cenário, a quantidade de pontos e os parâmetros do algoritmo
e clique em **Executar algoritmo**. A evolução aparece ao vivo (rota, melhor
distância, média e curva de convergência) e dá para **Parar** a qualquer momento.
Depois da execução, o slider, a curva (clique ou arraste) e o botão
**Reproduzir** (velocidade Lenta, Média ou Rápida) mostram a melhor rota de cada
geração. **Salvar histórico (CSV)** e **Salvar imagem (PNG)** exportam a
execução. Os valores iniciais dos parâmetros são os do protocolo.

## Experimentos e resultados

Abra `experimentos/comparativos/relatorio.html` no navegador. A pasta contém os
resultados reais de **200 execuções**: cinco configurações, dois cenários, dois
tamanhos (10 e 30 pontos) e dez sementes por configuração. Os CSVs trazem os
resultados individuais, o resumo estatístico, o histórico, os pontos e as rotas.

- [Explicação do algoritmo e roteiro de apresentação](experimentos/EXPLICACAO.md)
- [Resultados, conclusões e limitações](experimentos/comparativos/RESULTADOS.md)
- [Protocolo dos experimentos](experimentos/PROTOCOLO.md)
- [Bônus: círculo com 100 e 200 pontos](experimentos/BONUS.md)

### Executar novas comparações

Requisitos: compilador C++, Make e Python 3 (não precisa de Qt nem de pacotes
Python externos). Na raiz do projeto:

```bash
make comparativos SAIDA=experimentos/nova_comparacao
```

O comando **executa o algoritmo 200 vezes** e depois gera tabelas, relatório
HTML e gráficos SVG. Use um nome de pasta que ainda não exista: o executor
recusa sobrescrever rodadas anteriores. Os tempos variam entre execuções e
computadores, mesmo quando as rotas se repetem. Para refazer só a análise de uma
pasta já existente:

```bash
python3 analisar_experimentos.py experimentos/comparativos
```

### Bônus: círculo com muitos pontos

O teste com 100 e 200 pontos em círculo (cinco sementes por tamanho) atingiu a
distância ideal nas 10 execuções da rodada entregue (em outro ambiente, 9 de 10; detalhes no `BONUS.md`). Os tempos de convergência, as rotas em gerações
intermediárias e as curvas estão em [experimentos/BONUS.md](experimentos/BONUS.md)
e na pasta `experimentos/bonus/`. Para repetir (a pasta de saída deve ser nova):

```bash
make bonus SAIDA_BONUS=experimentos/bonus_novo
```

Sem `make`, no Windows:

```powershell
g++ -std=c++17 -O2 executar_bonus.cpp ciclo.cpp genetico.cpp cenarios.cpp -o build\executar_bonus.exe
.\build\executar_bonus.exe experimentos\bonus_novo
```

## Testes

```bash
make testes
```

Os testes verificam cenários, distâncias, rotas inválidas, sementes, operadores,
preservação das permutações e o funcionamento do ciclo. O executor comparativo
também recalcula as distâncias das melhores rotas, confere o histórico, o
elitismo e o limite ideal circular antes de exportar, e a análise confere os 200
IDs, grupos, sementes e históricos.

O `Makefile` usa comandos Unix. No Windows, sem `make`, compile cada teste
diretamente (com o `g++` do MinGW no PATH) e execute:

```powershell
g++ -std=c++17 -O2 verificar_cenarios.cpp cenarios.cpp -o verificar_cenarios.exe
g++ -std=c++17 -O2 verificar_genetico.cpp genetico.cpp -o verificar_genetico.exe
g++ -std=c++17 -O2 verificar_ciclo.cpp ciclo.cpp genetico.cpp cenarios.cpp -o verificar_ciclo.exe
.\verificar_cenarios.exe; .\verificar_genetico.exe; .\verificar_ciclo.exe
```

## Programa da primeira etapa (terminal)

```bash
make
./caixeiro --cenario aleatorio --pontos 10 --semente 42
./caixeiro --cenario circular --pontos 10 --semente 42
```

Esse executável só mostra uma rota aleatória como referência, sem evolução. Seus
SVGs e JSONs ficam em `resultados`; use `--saida outra_pasta` para preservar
execuções anteriores. Para acompanhar o algoritmo genético, use a interface Qt ou
o executor comparativo.

## Roteiro completo no Windows (PowerShell)

Sequência para compilar e executar todas as partes do projeto sem `make`, na
raiz do repositório. Ajuste a versão e as pastas do Qt para a sua instalação.

```powershell
# 1. Preparação (uma vez por janela do PowerShell)
$env:PATH = "C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.2\mingw_64\bin;" + $env:PATH
mkdir build -Force

# 2. Testes automáticos
g++ -std=c++17 -O2 -Wall -Wextra verificar_cenarios.cpp cenarios.cpp -o build\verificar_cenarios.exe
g++ -std=c++17 -O2 -Wall -Wextra verificar_genetico.cpp genetico.cpp -o build\verificar_genetico.exe
g++ -std=c++17 -O2 -Wall -Wextra verificar_ciclo.cpp ciclo.cpp genetico.cpp cenarios.cpp -o build\verificar_ciclo.exe
.\build\verificar_cenarios.exe; .\build\verificar_genetico.exe; .\build\verificar_ciclo.exe

# 3. Programa da primeira etapa (gera um SVG em resultados\)
g++ -std=c++17 -O2 main.cpp cenarios.cpp -o build\caixeiro.exe
.\build\caixeiro.exe --cenario circular --pontos 10 --semente 42

# 4. Interface gráfica
cmake -S . -B build\qt -G Ninja -DCMAKE_PREFIX_PATH="C:\Qt\6.11.2\mingw_64"
cmake --build build\qt
.\build\qt\caixeiro_qt.exe

# 5. Experimentos comparativos (200 execuções; a pasta de saída deve ser nova)
g++ -std=c++17 -O2 executar_experimentos.cpp ciclo.cpp genetico.cpp cenarios.cpp -o build\executar_experimentos.exe
.\build\executar_experimentos.exe experimentos\nova_comparacao
python analisar_experimentos.py experimentos\nova_comparacao
start experimentos\nova_comparacao\relatorio.html

# 6. Bônus (a pasta de saída deve ser nova)
g++ -std=c++17 -O2 executar_bonus.cpp ciclo.cpp genetico.cpp cenarios.cpp -o build\executar_bonus.exe
.\build\executar_bonus.exe experimentos\novo_bonus
start experimentos\novo_bonus\rotas_intermediarias_200.svg
```

Resultados esperados: os testes terminam com "Operadores geneticos: OK" e
"Testes do ciclo OK"; no círculo de 10 pontos a distância final é 247,2136; a
análise dos experimentos termina com "200 execuções validadas"; o bônus lista as
execuções de 100 e 200 pontos com o tempo até o ideal. As pastas `build/` e
`resultados/` são ignoradas pelo Git; as pastas de saída dos experimentos e do
bônus devem ser removidas antes de qualquer commit, para não substituir os
resultados entregues.

## Detalhes técnicos

O contrato usa `Ponto { double x, y; }` e `Rota = std::vector<int>`: uma
permutação dos índices, sem repetir a cidade inicial no fim. A função
`distancia_total` inclui o retorno. Menor distância é melhor.

`executar_ag(pontos, parametros, callback)` devolve `ResultadoAG` com a melhor
rota, a distância, o tempo, as gerações e o histórico. O registro zero é a
população inicial, então o histórico tem `geracoes_executadas + 1` linhas. O
callback opcional é chamado a cada geração; a interface roda o ciclo em outra
thread e recebe os dados por sinais do Qt.

Parâmetros iniciais: população 100, cruzamento 90%, mutação 10% por filho,
elitismo 1, torneio 3, limite de 1.000 gerações ou 200 sem melhoria estrita.
Os pontos aleatórios são uniformes no quadrado [0,100] × [0,100]. O círculo tem
centro (50,50), raio 40 e pontos igualmente espaçados; a distância ideal é o
perímetro do polígono regular, `n * 2 * raio * sen(pi/n)`.

A mesma semente repete a busca no mesmo ambiente C++; bibliotecas padrão
diferentes podem gerar sequências diferentes. O ambiente e os hashes das fontes
da rodada entregue estão em `experimentos/comparativos/ambiente.txt`.
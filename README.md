# Caixeiro viajante com algoritmo genético

## Primeira etapa

C++17 ou superior, sem bibliotecas externas. Na pasta do projeto, compile e execute:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic main.cpp cenarios.cpp -o caixeiro
./caixeiro --cenario aleatorio --pontos 10 --semente 42
./caixeiro --cenario circular --pontos 10 --semente 42
```

Também é possível compilar com `make`. Use `./caixeiro --help` para ver as opções.

Abra os SVGs de `resultados` no navegador. Os JSONs contêm pontos, rota e
distância. Execuções com os mesmos parâmetros sobrescrevem os arquivos;
use `--saida outra_pasta` para preservar resultados separados.

Ainda não existe otimização: a rota aleatória serve de referência inicial.
Pontos aleatórios são uniformes no quadrado [0,100] × [0,100]. Pontos circulares
são igualmente espaçados, com centro (50,50) e raio 40. A referência ideal é
o perímetro do polígono regular, `n * 2 * raio * sen(pi/n)`.

## Contrato de integração

- `pontos`: `std::vector<Ponto>`, com campos `double x` e `double y`.
- `rota`: `Rota` (apelido para `std::vector<int>`), permutação de todos os índices, sem repetir o início no final.
- `distancia_total(rota, pontos)`: distância euclidiana incluindo o retorno.
- Menor distância é melhor. Sementes permitem repetir resultados.
- `cenarios.hpp` declara o contrato; `cenarios.cpp` contém dados e avaliação;
  `main.cpp`, execução e gráfico.

## Próximos passos dos três integrantes

A parte de dados já tem verificações automatizadas (`make verificar`) e um
preparador do plano experimental (`make experimentos`). O protocolo e o
combinado de integração estão em [experimentos/PROTOCOLO.md](experimentos/PROTOCOLO.md).
O preparador gera pontos fixos e 100 execuções planejadas; os resultados da
otimização permanecem vazios até a integração do ciclo evolutivo.

### Integrante 1 — dados e experimentos

A base já está implementada. Revisar as distâncias e os dois cenários.
Preparar comparações entre populações 50/100/200 e taxas de mutação
1%/5%/10%, alterando uma variável por vez e mantendo os pontos fixos.
Após integrar o algoritmo, repetir com diferentes sementes e registrar
melhor distância, tempo e gerações. Explicar a aptidão e a referência circular.

### Integrante 2 — operadores genéticos

Criar `genetico.hpp` e `genetico.cpp`. Combinar estas funções, recebendo
um `std::mt19937& rng` para controlar a aleatoriedade:

- `criar_populacao(quantidade_pontos, tamanho, rng)` → `std::vector<Rota>`.
- `selecionar_torneio(populacao, distancias, rng, k=3)` → `Rota`, cópia do vencedor.
- `cruzar_ox(pai1, pai2, rng)` → `Rota`, um filho válido.
- `mutar_inversao(rota, rng)` → `Rota`, cópia com um trecho invertido.

Verificar que os pais não mudam e que cada filho contém todos os índices
exatamente uma vez. Explicar por que OX e inversão preservam permutações.

### Integrante 3 — ciclo evolutivo e visualização

Integrar avaliação, seleção, cruzamento, mutação e nova população.
Começar com 100 indivíduos, cruzamento de 90%, mutação de 10% por filho e
elitismo de uma rota. Parar em 1.000 gerações ou 200 sem melhoria.
São hipóteses iniciais para os experimentos, não escolhas definitivas.

Registrar a geração inicial e cada geração: número, melhor rota (copiada),
melhor distância, distância média e tempo decorrido. Depois criar uma curva
de convergência e navegação pelas rotas. Tornar os parâmetros configuráveis.

## Próxima entrega conjunta

Rodar os dois cenários com 10 pontos e mostrar a distância melhorando ao
longo das gerações. Todos documentam e apresentam suas partes.
A interface completa vem depois que o ciclo evolutivo funcionar.

## Reprodutibilidade

A mesma semente reproduz a execução no mesmo ambiente C++. As sequências
não precisam coincidir com a versão anterior em Python, nem entre diferentes
implementações da biblioteca padrão C++. Registrem compilador e ambiente.

## Etapa 2 — ciclo evolutivo

O ciclo completo foi implementado em `ciclo.hpp` e `ciclo.cpp`, aproveitando
os operadores ja existentes de `genetico.cpp`. Por padrao, usa populacao 100,
crossover OX de 90%, mutacao por inversao de 10% por filho, 1 elite,
torneio de 3, limite de 1000 geracoes e parada apos 200 geracoes sem melhora.

- `executar_ag(pontos, parametros, callback)` retorna `ResultadoAG`, com a
  melhor rota, distancia final, tempo, numero de geracoes evoluidas e historico.
- `historico[0]` guarda a populacao inicial; `historico[g]` registra a melhor
  rota daquela geracao, a melhor distancia, a media e o tempo acumulado.
- O `callback` opcional e chamado a cada geracao. Para uso com Qt, execute o
  algoritmo fora da thread da interface e encaminhe atualizacoes por sinais.
  Nao modifique widgets Qt diretamente dentro do callback de uma worker thread.
- As distancias entre pontos sao pre-calculadas em uma matriz antes do cronometro;
  o cronometro mede criacao da populacao e ciclo evolutivo, sem desenhar pontos.
- `geracoes_executadas` nao inclui a geracao inicial, logo o historico tem
  `geracoes_executadas + 1` registros.

Testar esta etapa sem Qt:

```bash
make verificar_ciclo
```

Exemplo de uso em outra unidade de codigo:

```cpp
#include "ciclo.hpp"
#include "cenarios.hpp"

auto pontos = gerar_pontos("circular", 10, 42);
ParametrosAG parametros;
parametros.semente = 1;
auto resultado = executar_ag(pontos, parametros);
// resultado.historico[i].melhor_rota e .melhor_distancia
```

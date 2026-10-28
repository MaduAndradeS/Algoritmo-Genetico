# Bônus: círculo com muitos pontos

Este teste verifica se o algoritmo genético escala para círculos maiores que os
dos experimentos principais (10 e 30 pontos), mede o tempo até a convergência e
mostra a evolução da melhor rota em gerações intermediárias.

## Configuração

- Cenário circular (centro (50, 50), raio 40), com **100** e **200** pontos. A
  distância ideal é o perímetro do polígono regular: 251,2861 para 100 pontos e
  251,3171 para 200.
- População 200, cruzamento OX 90%, mutação 10%, elitismo 1, torneio 3.
- Parada em 30.000 gerações ou 2.000 consecutivas sem melhoria estrita. Esses
  limites são maiores que os do protocolo principal (1.000 e 200), porque
  com 200 pontos 1.000 gerações não bastam: em uma medição preliminar com
  população 100 e limite de 5.000 gerações, a execução de 200 pontos terminou
  com distância 505,85, o dobro do ideal.
- Cinco sementes do algoritmo (1 a 5) por tamanho; mesmo mapa (semente dos
  pontos 42).
- **Tempo de convergência:** tempo decorrido, medido pelo cronômetro interno do
  ciclo, até a primeira geração cuja melhor distância atinge o ideal (tolerância
  absoluta de 1e-8). O algoritmo só termina 2.000 gerações depois, por isso o
  tempo total é maior.

## Resultados

Na rodada entregue (arquivos de `bonus/`), todas as 10 execuções atingiram a distância ideal. Veja a seção "Repetição em outro ambiente" para o resultado em outro compilador.

| Pontos | Gerações até o ideal (média ± desvio) | Intervalo | Tempo até o ideal (média ± desvio) | Intervalo | Tempo total (média) |
| --- | --- | --- | --- | --- | --- |
| 100 | 1.340,8 ± 113,4 | 1.223 a 1.464 | 0,307 ± 0,026 s | 0,268 a 0,340 s | 0,824 s |
| 200 | 6.374,2 ± 773,3 | 5.787 a 7.684 | 2,657 ± 0,324 s | 2,366 a 3,204 s | 3,476 s |

Dobrar o número de pontos multiplicou por cerca de 4,8 o número de gerações e
por cerca de 8,7 o tempo de convergência: o problema fica mais difícil e cada
geração custa mais, pois cada rota tem o dobro de cidades.

| Pontos | Semente | Geração do ideal | Tempo até o ideal (s) | Gerações executadas | Tempo total (s) |
| --- | --- | --- | --- | --- | --- |
| 100 | 1 | 1.461 | 0,340 | 4.152 | 0,968 |
| 100 | 2 | 1.223 | 0,268 | 3.233 | 0,715 |
| 100 | 3 | 1.464 | 0,313 | 3.905 | 0,835 |
| 100 | 4 | 1.281 | 0,315 | 3.281 | 0,805 |
| 100 | 5 | 1.275 | 0,298 | 3.379 | 0,797 |
| 200 | 1 | 5.872 | 2,479 | 7.872 | 3,288 |
| 200 | 2 | 5.787 | 2,366 | 7.787 | 3,125 |
| 200 | 3 | 6.430 | 2,587 | 8.430 | 3,404 |
| 200 | 4 | 7.684 | 3,204 | 9.690 | 4,047 |
| 200 | 5 | 6.098 | 2,651 | 8.108 | 3,514 |

## Soluções em gerações intermediárias

Para a semente 1 (fixa, sem escolher o melhor caso), os arquivos
`bonus/rotas_intermediarias_100.svg` e `bonus/rotas_intermediarias_200.svg`
mostram a melhor rota da geração inicial e das gerações correspondentes a 5%,
10%, 25%, 50% e 100% da geração em que o ideal foi atingido, com a distância e o
tempo de cada uma. Para 100 pontos, as gerações mostradas são 0, 73, 146, 365,
730 e 1.461; para 200 pontos, 0, 293, 587, 1.468, 2.936 e 5.872.

A evolução segue o mesmo padrão nos dois tamanhos: a rota inicial é um emaranhado
de arestas cruzadas; nas gerações seguintes, as cidades vizinhas no círculo vão
sendo ligadas e restam poucas arestas longas, que desaparecem por último, até a
rota coincidir com o polígono. As curvas de convergência estão em
`bonus/convergencia_100.svg` e `bonus/convergencia_200.svg` (eixo vertical
logarítmico) e os históricos completos em `bonus/historico_100.csv` e
`bonus/historico_200.csv`. Em 200 pontos, a curva tem degraus: a melhor distância
permanece estável por centenas de gerações e cai de uma vez em alguns
pontos.

## Repetição em outro ambiente

O mesmo programa foi executado no Windows com MinGW (g++ 13.1), com os mesmos
parâmetros e sementes. Os resultados diferem dos da rodada entregue, que usou
GCC 16 no Linux:

| Pontos | Execuções que atingiram o ideal | Gerações até o ideal (média) | Tempo até o ideal (média) |
| --- | --- | --- | --- |
| 100 | 5 de 5 | 1.661,4 | 0,248 s |
| 200 | 4 de 5 | 6.884,3 (nas 4 que atingiram) | 1,788 s (nas 4 que atingiram) |

Na semente 5 com 200 pontos, a busca parou por 2.000 gerações sem melhoria com
distância 253,83, cerca de 1,0% acima do ideal (251,32). As demais gerações e tempos
também mudaram. Isso é coerente com a dependência de ambiente descrita nos
experimentos principais: pequenas diferenças numéricas da biblioteca
matemática mudam os desempates do círculo e, em seguida, o caminho da busca. O
algoritmo é estocástico e, com 200 pontos, nem toda execução converge dentro do
limite de 2.000 gerações sem melhoria; um limite maior aumentaria a chance de
convergir ao custo de mais tempo. As conclusões gerais se mantêm: 100 pontos
convergem de forma consistente e 200 pontos convergem na maioria das execuções,
em poucos segundos.

## Limites

Há um único mapa por tamanho e cinco sementes. Os tempos foram medidos em uma
máquina (Intel Xeon de 2,1 GHz, execução sequencial) e variam entre execuções e
computadores; as gerações até o ideal dependem do compilador e da biblioteca
padrão, como nos demais experimentos. Os parâmetros foram escolhidos em medições
preliminares com a semente 1, não otimizados. Não foram testados tamanhos acima de 200.

## Como reproduzir

```bash
make bonus SAIDA_BONUS=experimentos/bonus_novo
```

A pasta de saída deve ser nova. Na interface Qt, o mesmo cenário pode ser
visualizado escolhendo "circular" e 100 ou 200 pontos, população 200,
máximo de gerações 30.000 e máximo sem melhora 2.000.
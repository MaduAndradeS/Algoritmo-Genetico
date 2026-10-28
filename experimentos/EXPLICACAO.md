# Algoritmo genético para o problema do caixeiro viajante

Este documento descreve o algoritmo implementado e justifica as escolhas de
projeto. Os números citados vêm dos experimentos em
[comparativos/RESULTADOS.md](comparativos/RESULTADOS.md); o desenho dos
experimentos está em [PROTOCOLO.md](PROTOCOLO.md).

## 1. Problema

Dado um conjunto de cidades no plano, buscamos a ordem de visita que passa por
todas uma única vez e retorna à cidade inicial com a menor distância total. O
custo entre duas cidades é a distância euclidiana. O número de ordens possíveis
cresce fatorialmente com a quantidade de cidades, o que inviabiliza testar todas
em instâncias maiores. O algoritmo genético é uma heurística: costuma encontrar
boas soluções, mas não garante o ótimo em toda execução.

Dois cenários são usados:

- **Aleatório:** pontos uniformes no quadrado [0, 100] × [0, 100]. O ótimo não é
  conhecido.
- **Circular:** pontos igualmente espaçados em um círculo de raio 40 e centro
  (50, 50). O percurso ótimo segue o polígono inscrito, e sua distância é
  `n × 2 × 40 × sen(π/n)`: 247,2136 para 10 cidades e 250,8683 para 30. Esse
  valor é o perímetro do polígono, não o comprimento da circunferência, e serve
  de referência para verificar os resultados.

Há duas sementes independentes: a dos pontos, que define o mapa aleatório (fixa
em 42), e a do algoritmo, que define os sorteios da busca (1 a 10 nos
experimentos).

## 2. Representação e cruzamento

**Representação.** Cada indivíduo (cromossomo) é um vetor com os índices das
cidades, sem repetição: `[0, 2, 1, 3]` representa 0 → 2 → 1 → 3 → 0. Cada
posição é um gene e seu valor é o índice de uma cidade. O retorno à cidade
inicial é considerado no cálculo e no desenho, mas o índice inicial não é
repetido no vetor. A população inicial é formada por permutações aleatórias.

A permutação foi escolhida porque expressa diretamente a restrição do problema:
toda cidade aparece exatamente uma vez. Representações mais simples, como uma
sequência binária ou um vetor de inteiros livres, permitiriam rotas inválidas,
com cidades repetidas ou ausentes, e exigiriam correção após cada operação.

**Cruzamento OX (Order Crossover).** O filho recebe de `pai1` o trecho entre dois
índices sorteados, nas mesmas posições. As demais posições são preenchidas com as
cidades de `pai2`, na ordem em que aparecem a partir do fim do trecho (dando a
volta no vetor) e pulando as já usadas. O filho é sempre uma permutação válida,
sem necessidade de reparo, e herda de `pai2` a ordem relativa das cidades, que é
o que importa em um percurso. Um cruzamento posicional simples, como o de um
ponto de corte, geraria cidades repetidas.

Cada filho passa pelo OX com probabilidade de 90%; nos demais casos, copia o
primeiro pai selecionado.

## 3. Aptidão

A qualidade de uma rota é a soma das distâncias entre cidades consecutivas,
incluindo a volta do último ponto ao primeiro. **Menor distância é melhor.**

Não convertemos a distância em uma aptidão crescente (por exemplo, `1/distância`)
porque a seleção por torneio só precisa comparar candidatos: vence o de menor
distância. Assim, a escala dos valores não influencia a seleção. Para evitar
recalcular raízes quadradas, o ciclo usa uma matriz com a distância entre todos
os pares de cidades, calculada uma vez por execução.

## 4. Ciclo evolutivo

Em cada geração:

1. Avaliamos todas as rotas e registramos a melhor e a distância média.
2. A melhor rota é copiada sem alteração para a próxima população (elitismo
   de 1).
3. Os pais são escolhidos por torneio: sorteiam-se três candidatos, com
   reposição, e vence o de menor distância.
4. Cada filho é gerado por OX (90%) ou por cópia de um pai.
5. Cada filho sofre mutação por inversão com a probabilidade configurada.
6. Repete-se até completar a população.

O elitismo garante que a melhor distância nunca piora entre gerações. A
distância média da população pode subir.

**Mutação por inversão.** Sorteiam-se dois índices e inverte-se a ordem do trecho
entre eles; por exemplo, `[0, 1, 2, 3, 4]` pode virar `[0, 3, 2, 1, 4]`. A
operação preserva as cidades e troca apenas conexões entre elas, o que costuma
desfazer cruzamentos de arestas, um defeito típico de rotas ruins no plano. A
taxa é por filho, não por gene.

## 5. Taxa de mutação

Adotamos 10% por filho. Foram comparadas as taxas de 1%, 5% e 10%, com população
100:

- Com 10 cidades, as três chegaram à mesma distância final nas dez sementes
  (309,9428 no mapa aleatório e 247,2136 no circular); o problema é pequeno
  demais para diferenciá-las.
- Com 30 cidades circulares, a distância ideal foi atingida em 3 sementes com
  1%, em 9 com 5% e em 10 com 10%.
- Com 30 cidades aleatórias, a distância média foi 559,5689 (1%), 555,2924 (5%)
  e 548,5062 (10%).

Em geral, taxas baixas favorecem a perda de diversidade e a estagnação; nos
casos testados, a taxa de 10% foi a mais consistente. Taxas acima de 10% não foram
avaliadas, portanto 10% é a melhor entre as testadas, não uma taxa ótima em
geral.

## 6. Tamanho da população e critério de parada

**População.** Adotamos 100 indivíduos. Foram comparadas populações de 50, 100 e
200, com mutação 10%:

- Com 10 cidades, todas chegaram ao mesmo resultado; o tempo médio cresceu com a
  população (de 4,0 ms para 12,4 ms no mapa aleatório, de 50 para 200).
- Com 30 cidades circulares, as populações de 100 e de 200 atingiram o ideal nas
  dez sementes; a de 50 atingiu em oito.
- Com 30 cidades aleatórias, a distância média foi 566,5358 (50), 548,5062 (100)
  e 537,7991 (200), com tempo médio de 15,6 ms, 22,2 ms e 40,8 ms.

A população de 100 equilibra diversidade e custo: foi suficiente para atingir o
ideal circular em todas as sementes com 30 cidades, gastando menos tempo que a de
200. A de 200 obteve a menor média no mapa aleatório de 30 cidades, ao custo de
quase o dobro do tempo.

**Parada.** O algoritmo termina ao atingir 1.000 gerações ou 200 gerações
consecutivas sem melhoria estrita da melhor distância. O limite de 1.000 impõe um
teto ao trabalho de uma execução; o de 200 encerra buscas estagnadas e ainda
dá tempo para a mutação e o cruzamento produzirem melhorias tardias. São valores
heurísticos: os experimentos não compararam outras regras de parada. A geração
zero é a população inicial; se foram evoluídas 215 gerações, o histórico tem 216
registros. A comparação por melhoria usa `<` em ponto flutuante, de modo que uma
diferença minúscula por arredondamento pode contar como melhoria e adiar a
parada. Parar por estagnação não prova que o ótimo foi atingido.

## 7. Resultados e limites

Foram feitas 200 execuções: dois cenários (aleatório e circular), dois tamanhos (10 e 30 cidades), cinco configurações e dez sementes por
configuração. Em resumo:

- Com 10 cidades, todas as configurações empataram na distância final e, no
  circular, todas atingiram o ideal.
- Com 30 cidades, as diferenças aparecem: população e mutação maiores
  melhoraram a qualidade média, com custo maior de tempo no caso da população.

As conclusões valem para os casos medidos: há um único mapa por cenário e
tamanho, dez sementes, e não foi feito teste de significância. Os números
completos, os tempos e os desvios estão em
[comparativos/RESULTADOS.md](comparativos/RESULTADOS.md).

## 8. Gráficos e interface

O [relatório visual](comparativos/relatorio.html) reúne três gráficos:

- **Comparações:** média e desvio-padrão amostral (não intervalo de confiança) da
  distância e do tempo nas dez sementes. Valores menores são melhores.
- **Convergência:** melhor distância por geração para a semente 1 de cada
  configuração. É uma ilustração, não a média das dez execuções.
- **Rotas:** melhor rota da população inicial e rota final, com população 100,
  mutação 10% e semente 1.

A interface Qt permite escolher o cenário, a quantidade de pontos e todos os
parâmetros do algoritmo, executar com acompanhamento ao vivo (rota, melhor
distância, distância média e curva de convergência) e interromper a qualquer
momento. Após a execução, é possível percorrer as gerações pelo slider, pela
curva ou pelo botão de reprodução, e exportar o histórico em CSV e a imagem em
PNG. O ciclo evolutivo roda em uma thread própria, e a janela recebe as
atualizações por sinais do Qt, sem travar.
# Dados e experimentos — integrante 1

## O que esta pronto

Os cenarios e a avaliacao estao implementados em `cenarios.cpp`. A verificacao
automatizada cobre distancias conhecidas, retorno ao inicio, invariancia por
inversao e mudanca de inicio, rotas invalidas, geometria circular, limites dos
pontos aleatorios e repetibilidade das sementes. Execute na raiz:

```bash
make verificar
make experimentos
```

O segundo comando **prepara** 100 execucoes; nao executa o algoritmo genetico.
Gera tres arquivos em `experimentos/dados/`:

| Arquivo | Conteudo |
| --- | --- |
| `pontos.csv` | Os dois conjuntos fixos de 10 pontos, com indices e coordenadas. |
| `plano.csv` | Parametros de cada execucao, referencia aleatoria e ideal circular. |
| `resultados.csv` | Uma linha por execucao, com status pendente e metricas vazias. |

Os arquivos gerados ficam fora do Git. O preparador recusa sobrescrever arquivos
existentes. Para uma nova rodada, use uma pasta nova:

```bash
./build/preparar_experimentos experimentos/dados_rodada_2
```

## Comparacao controlada

Usaremos os cenarios aleatorio e circular, ambos com 10 pontos. A semente dos
pontos sera sempre 42; as sementes do algoritmo serao 1 a 10 para cada
configuracao. Assim, variar a aleatoriedade da busca nao muda o problema.

| Comparacao | Populacoes | Mutacao por filho |
| --- | --- | --- |
| Tamanho da populacao | 50, 100 e 200 | 10% fixa |
| Taxa de mutacao | 100 fixa | 1%, 5% e 10% |

A configuracao populacao 100 / mutacao 10% e compartilhada: sao cinco
configuracoes distintas, dois cenarios e dez sementes = 100 execucoes.
Fixar cruzamento em 90%, elitismo em uma rota, torneio com k=3 e parada em
1.000 geracoes ou 200 geracoes consecutivas sem melhoria. Uma melhoria e uma
distancia estritamente menor que a melhor anterior. Cada filho recebe uma
decisao de mutacao; 10% nao significa inverter 10% dos indices.

## Combinado para a integracao

- Gerar os pontos com `gerar_pontos(cenario, 10, semente_pontos)` ou carregar
  as coordenadas de `pontos.csv` na ordem dos indices.
- Inicializar um `std::mt19937 rng(semente_ag)` por execucao e passa-lo aos
  operadores. Nao reutilizar esse gerador para criar os pontos.
- A referencia `distancia_rota_aleatoria` usa
  `criar_rota_aleatoria(10, semente_ag)` sobre os pontos fixos. E uma rota
  independente de referencia, nao a melhor rota da populacao inicial.
- Guardar tambem a melhor distancia da geracao zero no historico do ciclo,
  para medir a melhoria dentro de cada execucao.
- Medir o tempo com `std::chrono::steady_clock`, incluindo criacao da
  populacao e evolucao, excluindo geracao de pontos, exportacao e desenhos.
- `geracoes_executadas` conta apenas as geracoes evoluidas; a populacao
  inicial e a geracao zero. O historico deve ter esse total mais uma linha.
- Preencher `resultados.csv` pelo `id` do plano, com status `concluido`,
  melhor distancia final, tempo em segundos e geracoes executadas. Falhas
  recebem status `erro`, metricas vazias e uma explicacao em arquivo separado.
- Registrar compilador (`g++ --version`), sistema (`uname -a`), flags de
  compilacao e revisao do codigo (`git rev-parse HEAD`). Medir no mesmo
  computador, sem execucoes concorrentes.

A parte dos operadores permanece em `genetico.hpp/.cpp`. A parte do ciclo
consome esses operadores; este preparador nao define uma interface nova para
eles. A conexao entre plano e executavel sera feita quando o ciclo existir.

## Como explicar a avaliacao

Cada individuo e uma permutacao dos indices das cidades. A funcao objetivo e
a soma das distancias euclidianas entre cidades consecutivas, incluindo o
retorno da ultima para a primeira. Menor distancia significa melhor individuo.
O torneio pode comparar as distancias diretamente; nao precisa converter para
`1 / distancia`, que ainda exigiria tratar distancia zero.

No circulo, os indices ja seguem a ordem ao redor do centro. Percorre-los em
ordem fornece a solucao ideal: o perimetro do poligono regular inscrito,
`n * 2 * raio * sen(pi/n)`. Para 10 pontos e raio 40, sao aproximadamente
247,2136 unidades. Nao usar o comprimento da circunferencia como ideal.
Para o cenario aleatorio, nao estamos afirmando conhecer o otimo.

## Analise depois da integracao

Para cada cenario e configuracao, apresentar media e desvio-padrao amostral
da melhor distancia final e do tempo nas dez sementes, alem do numero de
execucoes concluidas. Nao escolher apenas a melhor semente nem tratar celulas
vazias como zero. Registrar tambem as geracoes e comparar as configuracoes
pelas mesmas sementes.

Calcular a melhoria sobre a referencia aleatoria por execucao:
`100 * (distancia_referencia - melhor_distancia) / distancia_referencia`.
Valores negativos indicam um resultado pior que essa referencia.
No circulo, calcular o excesso sobre o ideal:
`100 * (melhor_distancia / distancia_ideal - 1)`.
Uma distancia abaixo do ideal alem de arredondamento indica um problema.

A primeira demonstracao deve mostrar a rota inicial, a final e a curva da
melhor distancia por geracao nos dois cenarios. Com elitismo, essa curva nao
deve subir, mas pode ficar constante; uma melhoria nao e garantida a cada
geracao. Caso todos atinjam o ideal com 10 pontos, combinar uma rodada maior
para distinguir as configuracoes antes de tirar conclusoes.

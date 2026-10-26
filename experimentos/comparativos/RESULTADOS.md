# Resultados dos experimentos comparativos

Foram concluídas **200 execuções reais**: 100 da rodada principal (10 cidades) e 100 da extensão (30 cidades). Nenhuma execução falhou. Cada configuração foi repetida nas sementes 1 a 10.

## Método

Pontos fixos por cenário e tamanho, semente dos pontos 42; cruzamento OX 90%, elitismo 1, torneio k=3, máximo de 1.000 gerações ou 200 consecutivas sem melhoria estrita. Populações 50/100/200 com mutação 10%; mutações 1%/5%/10% com população 100. A configuração 100/10% é compartilhada. A extensão de 30 cidades foi definida antes de observar os resultados.

As tabelas mostram média ± desvio-padrão amostral nas dez sementes. Menor distância é melhor; menor tempo indica menor custo nesta máquina. Os tempos vêm do cronômetro interno do ciclo: incluem população inicial, evolução e histórico; excluem matriz de distâncias, exportação e gráficos. As execuções foram sequenciais. O histórico e as rotas foram validados após cada execução, fora desse cronômetro.

## Tabelas e observações

### 10 cidades — aleatorio

| População | Mutação | Distância final | Tempo (ms) | Gerações (média) | Melhoria sobre geração 0 |
| --- | --- | --- | --- | --- | --- |
| 50 | 10% | 309.9428 ± 0.0000 | 4.027 ± 0.372 | 213.9 | 26.19% |
| 100 | 10% | 309.9428 ± 0.0000 | 6.415 ± 0.287 | 210.3 | 23.37% |
| 200 | 10% | 309.9428 ± 0.0000 | 12.410 ± 0.290 | 208.1 | 18.75% |
| 100 | 1% | 309.9428 ± 0.0000 | 6.122 ± 0.081 | 210.9 | 23.37% |
| 100 | 5% | 309.9428 ± 0.0000 | 6.232 ± 0.114 | 211.4 | 23.37% |

- **População:** empate na distância média, dentro da tolerância numérica de 1e-8.
- **Mutação:** empate na distância média, dentro da tolerância numérica de 1e-8.

- O ótimo deste mapa aleatório não foi calculado; a melhor distância encontrada não é uma prova de otimalidade.

### 10 cidades — circular

| População | Mutação | Distância final | Tempo (ms) | Gerações (média) | Melhoria sobre geração 0 |
| --- | --- | --- | --- | --- | --- |
| 50 | 10% | 247.2136 ± 0.0000 | 3.198 ± 0.112 | 213.9 | 41.34% |
| 100 | 10% | 247.2136 ± 0.0000 | 6.444 ± 0.248 | 214.7 | 38.18% |
| 200 | 10% | 247.2136 ± 0.0000 | 12.591 ± 0.231 | 210.0 | 35.75% |
| 100 | 1% | 247.2136 ± 0.0000 | 6.342 ± 0.381 | 217.1 | 38.18% |
| 100 | 5% | 247.2136 ± 0.0000 | 6.535 ± 0.531 | 220.0 | 38.18% |

- **População:** empate na distância média, dentro da tolerância numérica de 1e-8.
- **Mutação:** empate na distância média, dentro da tolerância numérica de 1e-8.

- Ideal conhecido: **247.2136**. Execuções que o atingiram (tolerância absoluta 1e-8), na ordem da tabela: **10/10, 10/10, 10/10, 10/10, 10/10**.

### 30 cidades — aleatorio

| População | Mutação | Distância final | Tempo (ms) | Gerações (média) | Melhoria sobre geração 0 |
| --- | --- | --- | --- | --- | --- |
| 50 | 10% | 566.5358 ± 21.1413 | 15.585 ± 2.900 | 567.7 | 59.58% |
| 100 | 10% | 548.5062 ± 15.4020 | 22.176 ± 4.318 | 391.7 | 59.68% |
| 200 | 10% | 537.7991 ± 8.6710 | 40.752 ± 8.350 | 342.2 | 59.66% |
| 100 | 1% | 559.5689 ± 14.5105 | 32.197 ± 12.483 | 576.6 | 58.90% |
| 100 | 5% | 555.2924 ± 22.3342 | 26.860 ± 7.180 | 466.7 | 59.20% |

- **População:** menor distância média observada com população 200 / mutação 10%. Isso é uma descrição desta amostra, sem teste de significância.
- **Mutação:** menor distância média observada com população 100 / mutação 10%. Isso é uma descrição desta amostra, sem teste de significância.

- O ótimo deste mapa aleatório não foi calculado; a melhor distância encontrada não é uma prova de otimalidade.

### 30 cidades — circular

| População | Mutação | Distância final | Tempo (ms) | Gerações (média) | Melhoria sobre geração 0 |
| --- | --- | --- | --- | --- | --- |
| 50 | 10% | 266.0669 ± 42.5687 | 12.294 ± 2.289 | 444.6 | 79.56% |
| 100 | 10% | 250.8683 ± 0.0000 | 21.535 ± 2.913 | 381.0 | 80.27% |
| 200 | 10% | 250.8683 ± 0.0000 | 35.436 ± 4.979 | 302.9 | 79.37% |
| 100 | 1% | 299.4238 ± 51.3898 | 25.666 ± 7.626 | 471.2 | 76.41% |
| 100 | 5% | 252.5224 ± 5.2308 | 24.624 ± 5.755 | 448.5 | 80.14% |

- **População:** menor distância média observada com população 100 / mutação 10% e população 200 / mutação 10%. Isso é uma descrição desta amostra, sem teste de significância.
- **Mutação:** menor distância média observada com população 100 / mutação 10%. Isso é uma descrição desta amostra, sem teste de significância.

- Ideal conhecido: **250.8683**. Execuções que o atingiram (tolerância absoluta 1e-8), na ordem da tabela: **8/10, 10/10, 10/10, 3/10, 9/10**.


## Gráficos

- [Comparações de distância e tempo](comparacoes.svg): médias e desvios nas dez sementes.
- [Convergência](convergencia.svg): semente 1 de cada configuração; não representa a média nem foi escolhida por ser a melhor.
- [Rotas iniciais e finais](rotas.svg): população 100, mutação 10%, semente 1, nos quatro problemas.

## Limites das conclusões

As dez sementes variam a busca, mas existe apenas um mapa por cenário e tamanho. O desenho muda um parâmetro por vez e não mede todas as interações entre população e mutação. Não foi feito teste de significância. Tempos pequenos dependem da máquina e da carga do sistema. Populações diferentes têm custos de avaliação diferentes, portanto o limite igual de gerações não é um orçamento igual de trabalho. A parada usa comparações estritas de ponto flutuante, como no código existente; diferenças minúsculas entre percursos equivalentes podem reiniciar o contador sem representar ganho prático. Não alteramos essa regra durante as medições.

Uma população maior ou uma taxa de mutação maior não garante um resultado melhor em toda execução. Para generalizar, repetir em vários mapas, tamanhos e máquinas e, se necessário, comparar também sob orçamento de tempo ou avaliações igual.

## Arquivos e reprodução

Os dados brutos estão em `resultados.csv`, `historico.csv`, `pontos.csv` e `rotas.csv`; o agregado está em `resumo.csv`. Ambiente e versão estão em `ambiente.txt` e `compilador.txt`. Para repetir da raiz do projeto: `make comparativos SAIDA=experimentos/nova_comparacao` (pasta ainda inexistente). Para refazer somente a análise: `python3 analisar_experimentos.py experimentos/comparativos`.

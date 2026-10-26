# Protocolo dos experimentos comparativos

## Situação

A integração foi concluída e as comparações foram executadas. Veja
[comparativos/RESULTADOS.md](comparativos/RESULTADOS.md) e
[EXPLICACAO.md](EXPLICACAO.md). As antigas pastas `dados` e `dados_rodada_2`
continham planejamento, não resultados do algoritmo.

## Desenho experimental

- Rodada principal: 10 cidades; extensão: 30 cidades.
- Cenários aleatório e circular. Um mapa fixo por cenário e tamanho, semente 42.
- Populações 50, 100 e 200, mantendo mutação 10%.
- Mutações 1%, 5% e 10%, mantendo população 100.
- A configuração população 100 / mutação 10% é compartilhada.
- Dez sementes do AG, 1 a 10, por configuração. São 200 execuções ao todo.
- Cruzamento OX 90%, elitismo 1, torneio 3 e parada em 1.000 gerações ou
  200 consecutivas sem melhoria estrita.

A extensão de 30 cidades foi estabelecida antes da leitura dos resultados,
para observar um problema maior. Os geradores dos pontos e da busca usam
sementes separadas. Cada comparação altera uma variável; as interações
entre todas as combinações de população e mutação não foram estudadas.

## Execução e arquivos

Da raiz do projeto, com C++17, Make e Python 3:

```bash
make verificar verificar_genetico verificar_ciclo
make comparativos SAIDA=experimentos/nova_comparacao
```

A pasta deve ser nova. O executor não sobrescreve saídas existentes.
O algoritmo roda sequencialmente, sem callback da interface. As validações
independentes e a exportação ocorrem após cada busca, fora de seu cronômetro.

| Arquivo | Conteúdo |
| --- | --- |
| `resultados.csv` | Parâmetros, sementes, métricas e status por execução. |
| `historico.csv` | Melhor distância, média da população e tempo por geração. |
| `pontos.csv` | Coordenadas e índices dos quatro mapas. |
| `rotas.csv` | Melhor rota inicial e final de cada execução. |
| `resumo.csv` | Médias e desvios-padrão amostrais por configuração. |
| `relatorio.html` | Tabelas e gráficos para abrir no navegador. |
| `RESULTADOS.md` | Método, observações, resultados e limitações. |
| `comparacoes.svg` | Médias e desvios de distância e tempo. |
| `convergencia.svg` | Curvas ilustrativas da semente 1 em cada configuração. |
| `rotas.svg` | Rotas ilustrativas com população 100, mutação 10%, semente 1. |
| `ambiente.txt`, `compilador.txt` | Ambiente, versão e informações de reprodução. |

O analisador captura o ambiente e os hashes das fontes presentes na análise
quando `ambiente.txt` ainda não existe. Na rodada entregue, o arquivo foi
registrado junto à execução. Se compilar com flags diferentes das padrões,
registre-as manualmente. Não altere as fontes entre a busca e a análise.

## Definições das métricas

- **Distância inicial:** melhor indivíduo da população na geração zero.
- **Melhor distância:** melhor solução devolvida pelo ciclo.
- **Referência aleatória:** rota isolada de `criar_rota_aleatoria(n, semente_ag)`
  sobre os mesmos pontos. Não é a melhor rota da população inicial.
- **Melhoria inicial:** `100 × (inicial − final) / inicial`.
- **Melhoria sobre referência:** `100 × (referência − final) / referência`.
- **Excesso circular:** `100 × (final / ideal − 1)`. Vazio no mapa aleatório.
- **Tempo:** cronômetro interno do ciclo, incluindo população, evolução e
  histórico, excluindo geração dos pontos, matriz de distâncias e exportações.
- **Gerações:** apenas as evoluídas. O histórico inclui também a geração zero.

O ideal circular é o perímetro do polígono regular, `n × 2 × raio × sen(pi/n)`.
O ótimo do mapa aleatório não foi determinado. Os resultados não usam células
vazias como zero. O executor falha com código diferente de zero ao encontrar
inconsistência; uma pasta parcial não deve ser apresentada como rodada concluída.

## Leitura e limites

Apresentamos média e desvio-padrão amostral de dez buscas por configuração.
O gráfico de convergência usa sempre a semente 1, sem selecionar o melhor
caso. As curvas terminam na parada real, sem prolongamento artificial.
Os tempos são da mesma máquina e as buscas são sequenciais.

Não foi feito teste de significância. Um mapa por cenário e tamanho não
representa todos os problemas possíveis. Os mesmos limites de gerações
não representam o mesmo número de avaliações para populações diferentes.
A parada por melhoria estrita pode ser sensível a arredondamentos; nenhuma
regra do ciclo foi alterada entre as configurações.

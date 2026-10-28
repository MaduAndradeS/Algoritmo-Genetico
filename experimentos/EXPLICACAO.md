# Explicação do projeto: caixeiro viajante com algoritmo genético

## 1. Qual problema estamos resolvendo?

Queremos visitar todas as cidades uma única vez e voltar à cidade inicial,
percorrendo a menor distância possível. É o problema do caixeiro viajante.
Aqui, cada cidade é um ponto no plano e o custo de viajar entre duas cidades
é a distância em linha reta. Não estamos simulando ruas nem trânsito.

O número de ordens possíveis cresce muito com a quantidade de cidades.
O algoritmo genético busca boas soluções sem precisar experimentar todas
as ordens. É uma heurística: pode encontrar soluções muito boas, mas não
promete o melhor caminho possível em toda execução.

## 2. Como representamos uma solução?

Cada indivíduo é uma rota: um vetor com os índices de todas as cidades,
sem repetição. Por exemplo, para quatro cidades, `[0, 2, 1, 3]` significa
visitar 0 → 2 → 1 → 3 → 0. O retorno existe no cálculo e no desenho, mas não
repetimos o índice inicial dentro do vetor. Os experimentos usam 10 e 30 cidades.

Uma população é um conjunto de rotas candidatas. População 100 significa
100 caminhos competindo e produzindo novos caminhos em cada geração.
A população inicial é formada por permutações aleatórias.

Na terminologia genética, cada posição é um gene cujo valor é o índice de
uma cidade; a rota completa é o cromossomo. Escolhemos uma permutação porque
ela representa diretamente a restrição de visitar cada cidade uma vez.
Usamos OX porque ele combina a ordem dos pais preservando essa restrição,
sem precisar reparar filhos com cidades repetidas ou ausentes.

## 3. Como avaliamos as rotas?

Para dois pontos A e B, calculamos `sqrt((Ax - Bx)² + (Ay - By)²)`.
Somamos essa distância para cada par consecutivo, incluindo a volta do
último ponto ao primeiro. **Menor distância representa maior qualidade.**

Essa avaliação é o papel da aptidão no projeto. Não precisamos transformar
as distâncias em probabilidades: a seleção por torneio pode escolher
simplesmente a menor distância entre os candidatos sorteados.

O código do ciclo usa uma matriz com as distâncias entre todos os pares
para evitar recalcular as mesmas raízes quadradas a cada avaliação.

## 4. O que acontece em uma geração?

1. Avaliamos todas as rotas e registramos a melhor e a distância média.
2. Copiamos a melhor rota intacta para a próxima população: elitismo de 1.
3. Selecionamos pais por torneios de três candidatos sorteados com reposição.
   Vence a menor distância. Sorteios podem repetir um candidato.
4. Para cada novo filho, há 90% de chance de fazer cruzamento OX. Caso
   contrário, copiamos o primeiro pai selecionado.
5. Cada filho tem uma chance de sofrer mutação por inversão.
6. Repetimos até completar a população e avaliamos a nova geração.

O **cruzamento OX** sorteia dois índices e copia o trecho entre eles do
primeiro pai. Depois preenche os lugares restantes com as cidades do
segundo pai, seguindo sua ordem circular e pulando as já usadas. Assim,
não perde cidades nem cria repetições. O trecho pode abranger toda a rota.

A **mutação por inversão** sorteia dois índices e inverte a ordem do trecho.
Exemplo: `[0, 1, 2, 3, 4]` pode virar `[0, 3, 2, 1, 4]`. Ela preserva as
cidades e modifica sua ordem. Se os índices sorteados forem iguais, nada
muda. Mutação de 10% é uma chance por filho, não 10% das cidades.

O **elitismo** impede que percamos a melhor distância já obtida. Por isso,
a curva da melhor distância pode cair ou ficar constante, mas não deve subir.
A distância média da população pode subir mesmo com elitismo.

## 5. Quando o algoritmo para?

Paramos ao atingir 1.000 gerações evoluídas ou 200 gerações consecutivas
sem melhoria estrita. A geração zero é a população inicial. Se evoluímos
215 gerações, o histórico tem 216 registros.

A regra atual usa `<` para detectar melhoria. Arredondamentos em ponto
flutuante podem contar uma diferença minúscula como melhoria e adiar a
parada. Os testes não modificaram essa regra; o relatório documenta isso.
Parar por estagnação não prova que o ótimo foi atingido.

Escolhemos dois limites complementares: 1.000 gerações limitam o trabalho
máximo de uma execução, enquanto 200 sem melhoria encerram buscas que
ficaram estagnadas, permitindo novas tentativas antes de desistir. São
valores heurísticos iniciais, não limites teoricamente ótimos. Os experimentos
compararam população e mutação, mas não compararam outras regras de parada;
portanto, não afirmamos que 1.000/200 sejam os melhores valores possíveis.

## 6. Por que existem dois cenários e duas sementes?

No cenário **aleatório**, os pontos ficam no quadrado de coordenadas 0 a 100.
É um mapa irregular. Não calculamos seu ótimo exato neste trabalho.

No **circular**, os pontos são igualmente espaçados em um círculo de raio
40 e centro (50, 50). O caminho ideal segue a borda do polígono, em qualquer
sentido. A referência é `n × 2 × 40 × sen(pi/n)`: aproximadamente 247,2136
para 10 cidades e 250,8683 para 30. É o perímetro do polígono inscrito,
não o comprimento da circunferência. Esse cenário permite conferir o
resultado contra uma solução de distância conhecida.

A **semente dos pontos** define o mapa aleatório. A **semente do AG** define
os sorteios da busca. Mantivemos a primeira em 42 e variamos a segunda de
1 a 10. Assim, repetimos a busca sem trocar de problema. A reprodução exata
depende também do ambiente e da biblioteca padrão C++.

## 7. O que comparamos e o que encontramos?

Executamos cinco configurações por cenário e tamanho: populações 50, 100 e
200 com mutação 10%, e mutações 1%, 5% e 10% com população 100. A configuração
100/10% é compartilhada, sem executá-la duas vezes na mesma comparação.
São 100 execuções com 10 cidades e mais 100 com 30 cidades.

Com **10 cidades**, as cinco configurações chegaram à mesma distância final
nos dez sorteios: 309,9428 no mapa aleatório e 247,2136 no círculo. Neste
último, todas atingiram o ideal conhecido. O mapa pequeno não distinguiu
as configurações pela qualidade final; populações maiores custaram mais tempo.
O empate no cenário aleatório não prova que encontramos seu ótimo.

Com **30 cidades aleatórias**, as médias foram 566,5358, 548,5062 e 537,7991
para populações 50, 100 e 200, respectivamente, mantendo mutação 10%.
A população 200 produziu a menor distância média, mas consumiu mais tempo.
Com população 100, mutação 10% teve média melhor que 1% e 5% nesta amostra.

Com **30 cidades circulares**, populações 100 e 200 com mutação 10% atingiram
o ideal nas dez sementes. População 50 atingiu em oito. Fixando população
100, as taxas de 1%, 5% e 10% atingiram o ideal em três, nove e dez sementes,
respectivamente. A taxa de 10% foi a mais consistente nesse conjunto.

Isso sugere que população 100 e mutação 10% são uma escolha inicial razoável
para estes casos, enquanto população 200 pode melhorar a qualidade do mapa
aleatório maior com mais custo. Não é uma recomendação universal: usamos
um único mapa por cenário e tamanho, dez sementes e nenhuma análise de
significância. Os números completos, tempos e desvios estão em
[comparativos/RESULTADOS.md](comparativos/RESULTADOS.md).

A justificativa da população 100 é equilibrar variedade de rotas e custo:
ela foi suficiente para atingir o ideal circular nas dez sementes com
30 cidades, consumindo menos tempo médio que população 200. A população
200 melhorou a média do mapa aleatório maior, mostrando essa troca entre
qualidade e tempo. A escolha da mutação 10% é apoiada pela comparação com
1% e 5%: mantendo população 100, foi a mais consistente no círculo maior
e apresentou a menor distância média no mapa aleatório maior. Não testamos
taxas acima de 10%, portanto essa escolha é a melhor observada entre as
taxas avaliadas nesses casos, não uma taxa ótima universal.

## 8. Como ler os gráficos e apresentar a interface?

O [relatório visual](comparativos/relatorio.html) reúne os três gráficos:

- **Comparações:** barras com a média das distâncias e dos tempos em dez
  sementes. Os traços representam um desvio-padrão amostral, não um intervalo
  de confiança. Distâncias menores são melhores; tempos menores são mais rápidos.
- **Convergência:** melhor distância por geração, com a semente 1 em todas
  as configurações. Serve de ilustração, não é a média das dez execuções.
- **Rotas:** melhor caminho da população inicial e caminho final com
  população 100, mutação 10% e semente 1. A rota inicial mostrada aqui já
  foi escolhida como a melhor da população inicial, não é uma rota isolada.

Na interface Qt, escolha cenário e quantidade de pontos, mantenha os
parâmetros iniciais e clique em **Executar algoritmo**. Mostre a evolução,
use a barra de gerações ou **Reproduzir**, e salve o histórico CSV e a imagem
PNG. O ciclo roda em uma thread de trabalho e envia atualizações para a
janela, permitindo acompanhar e solicitar a parada sem executar o ciclo
na thread principal da interface.

## 9. Roteiro curto para a apresentação

> Nosso projeto usa um algoritmo genético para buscar rotas curtas no
> problema do caixeiro viajante. Cada indivíduo é uma ordem de visita às
> cidades. Avaliamos a distância total, selecionamos pais por torneio,
> cruzamos com OX e aplicamos mutação por inversão, preservando a melhor
> rota por elitismo. Comparamos populações e taxas de mutação em dois
> cenários, com dez sementes para cada configuração. Com dez cidades,
> os resultados finais empataram. Com trinta, surgiram diferenças de
> qualidade e custo. O cenário circular permitiu conferir se a distância
> ideal foi atingida. As conclusões valem para os casos medidos; não há
> garantia de ótimo para qualquer mapa.

Cada integrante pode explicar sua parte: dados e experimentos; seleção,
cruzamento e mutação; ciclo evolutivo, histórico e interface.

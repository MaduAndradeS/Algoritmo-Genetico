#!/usr/bin/env python3
"""Resume os CSVs reais do executor; somente biblioteca padrao do Python 3."""
import csv
import datetime
import hashlib
import platform
import subprocess
import html
import math
from collections import defaultdict
from pathlib import Path
import statistics as st
import sys

CORES = ['#2563eb', '#16a34a', '#ea580c', '#9333ea', '#dc2626']
CONFIGS = [(50, .10), (100, .10), (200, .10), (100, .01), (100, .05)]

def ler(pasta, nome):
    with (pasta / nome).open(encoding='utf-8', newline='') as f:
        return list(csv.DictReader(f))

def texto(x, y, valor, tamanho=14, cor='#172033'):
    return f'<text x="{x}" y="{y}" font-size="{tamanho}" fill="{cor}">{html.escape(str(valor))}</text>'

def svg(pasta, nome, partes, largura=1120, altura=850):
    (pasta / nome).write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="{largura}" height="{altura}" viewBox="0 0 {largura} {altura}"><rect width="100%" height="100%" fill="white"/><g font-family="sans-serif">' + ''.join(partes) + '</g></svg>', encoding='utf-8')

def media(rs, campo):
    return st.mean(float(r[campo]) for r in rs)

def dp(rs, campo):
    return st.stdev(float(r[campo]) for r in rs)

def main():
    if len(sys.argv) != 2:
        raise SystemExit('Uso: python3 analisar_experimentos.py pasta_dos_resultados')
    pasta = Path(sys.argv[1])
    if not (pasta / 'ambiente.txt').exists():
        fontes = ['executar_experimentos.cpp', 'ciclo.cpp', 'ciclo.hpp', 'genetico.cpp', 'genetico.hpp', 'cenarios.cpp', 'cenarios.hpp']
        ambiente = [f'Data da analise UTC: {datetime.datetime.now(datetime.timezone.utc).isoformat()}', f'Sistema: {platform.platform()}', f'Python: {platform.python_version()}']
        revisao = subprocess.run(['git', 'rev-parse', 'HEAD'], capture_output=True, text=True)
        ambiente.append('Revisao base: ' + revisao.stdout.strip())
        ambiente.append('Flags padrao do Makefile: -std=c++17 -O2 -Wall -Wextra -Wpedantic; registrar eventuais sobreposicoes manualmente.')
        ambiente.append('SHA256 das fontes presentes na analise:')
        ambiente += [hashlib.sha256(Path(f).read_bytes()).hexdigest() + '  ' + f for f in fontes]
        (pasta / 'ambiente.txt').write_text('\n'.join(ambiente) + '\n', encoding='utf-8')
    dados = ler(pasta, 'resultados.csv')
    grupos = defaultdict(list)
    por_id = {}
    for r in dados:
        assert r['status'] == 'concluido', 'Ha execucoes incompletas.'
        chave = (int(r['pontos']), r['cenario'], int(r['populacao']), float(r['taxa_mutacao']))
        grupos[chave].append(r)
        assert r['id'] not in por_id, 'ID duplicado.'
        por_id[r['id']] = r
        for campo in ['melhor_distancia', 'distancia_inicial', 'tempo_segundos']:
            assert math.isfinite(float(r[campo]))
    assert len(grupos) == 20 and len(dados) == 200
    for rs in grupos.values():
        assert len(rs) == 10 and {int(r['semente_ag']) for r in rs} == set(range(1, 11))
    hist = defaultdict(list)
    for g in ler(pasta, 'historico.csv'):
        hist[g['id']].append(g)
    assert set(hist) == set(por_id)
    for id_, gs in hist.items():
        assert [int(g['geracao']) for g in gs] == list(range(len(gs)))
        assert len(gs) == int(por_id[id_]['geracoes_executadas']) + 1
        assert abs(float(gs[-1]['melhor_distancia']) - float(por_id[id_]['melhor_distancia'])) < 1e-8
        assert all(float(b['melhor_distancia']) <= float(a['melhor_distancia']) + 1e-8 for a, b in zip(gs, gs[1:]))
    contextos = [(n,c) for n in (10,30) for c in ('aleatorio','circular')]
    resumo = []
    md = ['# Resultados dos experimentos comparativos', '',
          'Foram concluídas **200 execuções reais**: 100 da rodada principal (10 cidades) e 100 da extensão (30 cidades). Nenhuma execução falhou. Cada configuração foi repetida nas sementes 1 a 10.', '',
          '## Método', '',
          'Pontos fixos por cenário e tamanho, semente dos pontos 42; cruzamento OX 90%, elitismo 1, torneio k=3, máximo de 1.000 gerações ou 200 consecutivas sem melhoria estrita. Populações 50/100/200 com mutação 10%; mutações 1%/5%/10% com população 100. A configuração 100/10% é compartilhada. A extensão de 30 cidades foi definida antes de observar os resultados.', '',
          'As tabelas mostram média ± desvio-padrão amostral nas dez sementes. Menor distância é melhor; menor tempo indica menor custo nesta máquina. Os tempos vêm do cronômetro interno do ciclo: incluem população inicial, evolução e histórico; excluem matriz de distâncias, exportação e gráficos. As execuções foram sequenciais. O histórico e as rotas foram validados após cada execução, fora desse cronômetro.', '',
          '## Tabelas e observações', '']
    partes = [texto(25,30,'Distância final e tempo: média ± desvio-padrão (10 sementes)',20)]
    for painel,(n,c) in enumerate(contextos):
        rs_contexto = [grupos[n,c,p,m] for p,m in CONFIGS]
        md += [f'### {n} cidades — {c}', '', '| População | Mutação | Distância final | Tempo (ms) | Gerações (média) | Melhoria sobre geração 0 |', '| --- | --- | --- | --- | --- | --- |']
        for (p,m),rs in zip(CONFIGS,rs_contexto):
            d,t,g = media(rs,'melhor_distancia'),media(rs,'tempo_segundos')*1000,media(rs,'geracoes_executadas')
            sd,stempo = dp(rs,'melhor_distancia'),dp(rs,'tempo_segundos')*1000
            ganho = media(rs,'melhoria_inicial_percentual')
            md += [f'| {p} | {m:.0%} | {d:.4f} ± {sd:.4f} | {t:.3f} ± {stempo:.3f} | {g:.1f} | {ganho:.2f}% |']
            resumo.append(dict(pontos=n,cenario=c,populacao=p,mutacao=m,execucoes=10,distancia_media=d,distancia_dp=sd,tempo_ms_medio=t,tempo_ms_dp=stempo,geracoes_media=g,melhoria_inicial_media=ganho))
        md.append('')
        for nome,indices in [('População',[0,1,2]),('Mutação',[3,4,1])]:
            indice = min(indices,key=lambda i:media(rs_contexto[i],'melhor_distancia'))
            p,m = CONFIGS[indice]
            valores = [media(rs_contexto[i],'melhor_distancia') for i in indices]
            if max(valores)-min(valores) < 1e-8:
                md += [f'- **{nome}:** empate na distância média, dentro da tolerância numérica de 1e-8.']
            else:
                empatados = [f'população {CONFIGS[i][0]} / mutação {CONFIGS[i][1]:.0%}' for i in indices if abs(media(rs_contexto[i], 'melhor_distancia') - min(valores)) < 1e-8]
                md += [f'- **{nome}:** menor distância média observada com ' + ' e '.join(empatados) + '. Isso é uma descrição desta amostra, sem teste de significância.']
        md.append('')
        if c == 'circular':
            ideal = float(rs_contexto[0][0]['distancia_ideal'])
            acertos = [sum(abs(float(r['melhor_distancia'])-ideal) <= 1e-8 for r in rs) for rs in rs_contexto]
            md += [f'- Ideal conhecido: **{ideal:.4f}**. Execuções que o atingiram (tolerância absoluta 1e-8), na ordem da tabela: **' + ', '.join(f'{a}/10' for a in acertos) + '**.']
        else:
            md += ['- O ótimo deste mapa aleatório não foi calculado; a melhor distância encontrada não é uma prova de otimalidade.']
        md.append('')
        # Dois graficos de barras por contexto, com eixo zero e barras de erro.
        y0 = 65 + painel*220
        partes.append(texto(25,y0,f'{n} cidades — {c}',17))
        for coluna,campo,fator,titulo in [(0,'melhor_distancia',1,'Distância final (unidades)'),(1,'tempo_segundos',1000,'Tempo (ms)')]:
            x0 = 185 + coluna*540
            maximo = max((media(rs,campo)+dp(rs,campo))*fator for rs in rs_contexto)*1.08
            partes.append(texto(x0+75,y0,titulo,13))
            for i,((p,m),rs) in enumerate(zip(CONFIGS,rs_contexto)):
                y = y0+24+i*32
                val,erro = media(rs,campo)*fator, dp(rs,campo)*fator
                largura = val/maximo*290
                ex = erro/maximo*290
                partes += [texto(x0-155,y+5,f'P={p} / M={m:.0%}',12),f'<rect x="{x0}" y="{y-8}" width="{largura}" height="16" fill="{CORES[i]}"/>',f'<path d="M{x0+largura-ex},{y} H{x0+largura+ex} M{x0+largura-ex},{y-5} V{y+5} M{x0+largura+ex},{y-5} V{y+5}" stroke="#111" fill="none"/>',texto(x0+300,y+5,f'{val:.2f}',11)]
            partes += [texto(x0,y0+193,'0',11),texto(x0+235,y0+193,f'{maximo:.1f}',11)]
    svg(pasta,'comparacoes.svg',partes,1120,960)
    # Historicos por configuracao: mesma semente ilustrativa, sem selecao do melhor caso.
    partes = [texto(25,28,'Convergência por configuração — semente do AG 1 (ilustração)',19)]
    for j,(n,c) in enumerate(contextos):
        x,y = 65+(j%2)*555,85+(j//2)*350
        partes.append(texto(x,y-20,f'{n} cidades — {c}',17))
        curvas = [hist[next(r['id'] for r in grupos[n,c,p,m] if r['semente_ag']=='1')] for p,m in CONFIGS]
        xmax = max(int(gs[-1]['geracao']) for gs in curvas)
        ymin = min(float(g['melhor_distancia']) for gs in curvas for g in gs)*.95
        ymax = max(float(g['melhor_distancia']) for gs in curvas for g in gs)*1.03
        partes += [f'<path d="M{x},{y} V{y+215} H{x+440}" stroke="#999" fill="none"/>',texto(x,y+235,'0',11),texto(x+380,y+235,f'{xmax} gerações',11),texto(x-55,y+10,f'{ymax:.0f}',11),texto(x-55,y+215,f'{ymin:.0f}',11)]
        for i,gs in enumerate(curvas):
            coords = ' '.join(f"{x+int(g['geracao'])/xmax*440:.2f},{y+215-(float(g['melhor_distancia'])-ymin)/(ymax-ymin)*215:.2f}" for g in gs)
            partes.append(f'<polyline points="{coords}" stroke="{CORES[i]}" stroke-width="2" fill="none"/>')
        partes.append(texto(x,y+263,'Distância no eixo vertical; curvas terminam na parada real.',11))
    for i,(p,m) in enumerate(CONFIGS):
        partes.append(texto(30+i*215,800,f'P={p} / M={m:.0%}',13,CORES[i]))
    svg(pasta,'convergencia.svg',partes,1120,830)
    pontos = defaultdict(list)
    for r in ler(pasta,'pontos.csv'):
        pontos[int(r['pontos']),r['cenario']].append((float(r['x']),float(r['y'])))
    rotas = defaultdict(list)
    for r in ler(pasta,'rotas.csv'):
        rotas[r['id'],r['tipo']].append(int(r['indice']))
    partes = [texto(25,28,'Melhor rota da geração 0 e rota final — população 100, mutação 10%, semente 1',17)]
    for j,(n,c) in enumerate(contextos):
        r = next(r for r in grupos[n,c,100,.10] if r['semente_ag']=='1')
        pts = pontos[n,c]
        for k,tipo in enumerate(('inicial','final')):
            x,y = 65+k*540,80+j*290
            d = float(r['distancia_inicial' if tipo=='inicial' else 'melhor_distancia'])
            partes.append(texto(x,y-12,f'{n} cidades / {c} / {tipo}: {d:.2f}',15))
            rota = rotas[r['id'],tipo]
            assert sorted(rota) == list(range(n))
            coords = ' '.join(f'{x+pts[i][0]*2.1:.2f},{y+220-pts[i][1]*2.1:.2f}' for i in rota+[rota[0]])
            partes.append(f'<polyline points="{coords}" fill="none" stroke="{CORES[k]}" stroke-width="1.6"/>')
            for i,(px,py) in enumerate(pts):
                cx,cy=x+px*2.1,y+220-py*2.1
                partes += [f'<circle cx="{cx}" cy="{cy}" r="3" fill="#172033"/>',texto(cx+4,cy-4,i,9)]
    svg(pasta,'rotas.svg',partes,1120,1250)
    with (pasta/'resumo.csv').open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(resumo[0]));w.writeheader();w.writerows(resumo)
    md += ['', '## Gráficos', '', '- [Comparações de distância e tempo](comparacoes.svg): médias e desvios nas dez sementes.', '- [Convergência](convergencia.svg): semente 1 de cada configuração; não representa a média nem foi escolhida por ser a melhor.', '- [Rotas iniciais e finais](rotas.svg): população 100, mutação 10%, semente 1, nos quatro problemas.', '', '## Limites das conclusões', '', 'As dez sementes variam a busca, mas existe apenas um mapa por cenário e tamanho. O desenho muda um parâmetro por vez e não mede todas as interações entre população e mutação. Não foi feito teste de significância. Tempos pequenos dependem da máquina e da carga do sistema. Populações diferentes têm custos de avaliação diferentes, portanto o limite igual de gerações não é um orçamento igual de trabalho. A parada usa comparações estritas de ponto flutuante, como no código existente; diferenças minúsculas entre percursos equivalentes podem reiniciar o contador sem representar ganho prático. Não alteramos essa regra durante as medições.', '', 'Uma população maior ou uma taxa de mutação maior não garante um resultado melhor em toda execução. Para generalizar, repetir em vários mapas, tamanhos e máquinas e, se necessário, comparar também sob orçamento de tempo ou avaliações igual.', '', '## Arquivos e reprodução', '', 'Os dados brutos estão em `resultados.csv`, `historico.csv`, `pontos.csv` e `rotas.csv`; o agregado está em `resumo.csv`. Ambiente e versão estão em `ambiente.txt` e `compilador.txt`. Para repetir da raiz do projeto: `make comparativos SAIDA=experimentos/nova_comparacao` (pasta ainda inexistente). Para refazer somente a análise: `python3 analisar_experimentos.py experimentos/comparativos`.', '']
    (pasta/'RESULTADOS.md').write_text('\n'.join(md),encoding='utf-8')
    tabelas=[]
    for n,c in contextos:
        linhas=''.join(f"<tr><td>{r['populacao']}</td><td>{r['mutacao']:.0%}</td><td>{r['distancia_media']:.4f} ± {r['distancia_dp']:.4f}</td><td>{r['tempo_ms_medio']:.3f} ± {r['tempo_ms_dp']:.3f}</td><td>{r['geracoes_media']:.1f}</td></tr>" for r in resumo if r['pontos']==n and r['cenario']==c)
        tabelas.append(f'<h2>{n} cidades — {c}</h2><table><tr><th>População</th><th>Mutação</th><th>Distância</th><th>Tempo (ms)</th><th>Gerações</th></tr>{linhas}</table>')
    (pasta/'relatorio.html').write_text('''<!doctype html><html lang="pt-BR"><meta charset="utf-8"><title>Experimentos comparativos</title><style>body{font:17px/1.6 sans-serif;max-width:1150px;margin:30px auto;padding:0 20px;color:#172033}img{width:100%;height:auto}table{border-collapse:collapse;width:100%}td,th{padding:8px;border:1px solid #ddd;text-align:right}th{background:#eef2ff}a{color:#2563eb}</style><h1>Experimentos comparativos</h1><p>200 execuções reais; cinco configurações, dois cenários, dois tamanhos e dez sementes. Tabelas: média ± desvio-padrão amostral. Menor distância é melhor.</p><p>Pontos fixos (semente 42), cruzamento 90%, elitismo 1, torneio 3; parada em 1.000 gerações ou 200 sem melhoria. Cada comparação altera uma variável.</p><p><a href="RESULTADOS.md">Conclusões e limitações detalhadas</a> · <a href="../EXPLICACAO.md">Explicação do projeto</a> · <a href="resumo.csv">Resumo CSV</a></p>''' + ''.join(tabelas) + '<h2>Comparações</h2><img src="comparacoes.svg" alt="Comparações de distância e tempo com barras de desvio-padrão"><h2>Evolução</h2><p>Semente 1 para todas as configurações. As curvas terminam na geração real de parada.</p><img src="convergencia.svg" alt="Curvas de convergência"><h2>Rotas</h2><p>Melhor rota da população inicial e final: população 100, mutação 10%, semente 1. A escala é a mesma para todos os mapas.</p><img src="rotas.svg" alt="Rotas iniciais e finais"><h2>Interpretação</h2><p>Resultados restritos aos mapas e sementes avaliados; não provam uma configuração universalmente melhor. O ótimo só é conhecido no cenário circular. Tempos dependem do ambiente. Veja RESULTADOS.md para as observações de cada tabela.</p></html>',encoding='utf-8')
    print('Análise concluída: 200 execuções validadas, 20 grupos, resumo, relatório e três gráficos SVG.')

if __name__ == '__main__':
    main()

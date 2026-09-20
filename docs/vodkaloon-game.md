# Vodkaloon — estado do jogo

Fonte de verdade para continuar o demo. Stack/build: `README.md` e `docs/project-requirements.md`. Não alterar compilador, MSX2, PAL, `ROM_32K` ou MSXgl sem pedido explícito.

## Premissa

Satira MSX2: **Hector Balloonski** (elmo viking) solta balões no céu da **Vila Zelina** (Praça República Lituana) sem esvaziar a garrafa de vodka. Produto: **Zelina Games**. Crédito na abertura: `2026 BY JUNIOR STRAVINO`.

## Fluxo

1. Abertura (~8 s, SPACE pula): logo + `PRESENTS` + crédito.
2. Título: `VODKALOON` / starring Hector / SCORE / HI / `PUSH SPACE KEY`. ~18 s sem SPACE → attract.
3. Briefing (typewriter) → SPACE.
4. `Game_Play`: ROUND card → táxi (só visual) → 3 rounds.
5. ROUND CLEAR avança; FESTA NA VILA ZELINA no round 3. GAME OVER se a vodka acaba sem balões no ar.
6. `ESPACO DE NOVO` / `ESC TITULO`. STOP/F1 pausa.

Attract: um round sem input; SCORE do título é preservado.

## Regras

| | |
|---|---|
| Vodka | 8 por round; cada balão gasta 1 |
| Festa | +1 se o balão chega em `y <= 20` (céu) |
| Vitória do round | festa ≥ 5 |
| Derrota | vodka = 0 e nenhum balão vivo |
| Score | +100 por balão no céu; bônus fim de round = vodka restante × 100 |
| Balões vivos | máx. 4 |
| Tiro | SPACE/trig1, edge; cooldown 12 frames (round 3: 18) |
| Nuvem | estoura o balão (HMMV, cor 3 = céu) |

## Rounds

| Round | Nuvens | Movimento | Vento |
|---|---|---|---|
| 1 | 2 | a cada 8 frames, spd 1 | não |
| 2 | 3, um pouco maiores | a cada 4 frames, spd 2 | ciclo a cada 96 frames |
| 3 | 3, largas, y 36/62/90 | todo frame, spd 2 | ciclo a cada 40 frames; aplica a cada 2 frames |

Táxi só no round 1 (e no retry a partir do 1).

## Visual / VDP

- Screen 5, sprites 16×16 modo 2. SPT `0x17000`, SAT `0x17A00`.
- Hector: 3 frames de caminhada + elmo (sprite 0 na frente, preto). Táxi: sedan 32×16 (6 sprites). Balão + outline preto.
- Cenário `DrawTown`: casas (janela pal 7 pisca), luminária, monumento, bandeira LTU, Igreja São José (janela pal 10 fixa), toldo VITO, lua. Calçada y≥180.
- **Nuvens apagam com HMMV na cor 3.** Cenário tem de ficar **abaixo de ~y 120**; senão o céu fura telhado/igreja.
- Máx. 8 sprites por linha. `PRINT_SKIP_SPACE` está TRUE: strings com espaço saem concatenadas — desenhar palavra a palavra.

## Áudio (PSG)

- Abertura: 3/4 Em, estilo Knightmare, **melodia original** (não transcrever a faixa Konami). Gerar com `tools/make_opening_music.py` → `generated/zelina_music.h` (não editar o `.h` à mão).
- In-game: ostinato 32 passos. Win/lose curtos. SFX: pop (tiro), score, cloud (estouro), typewriter. **Sem som de táxi.**

## Arquivos

| Path | Papel |
|---|---|
| `src/main.c` | opening, título, briefing |
| `src/game.c` | play / attract / HUD / cidade / rounds |
| `src/music.c` | player PSG + SFX |
| `include/game.h` | API (há decls duplicadas; inofensivo) |
| `generated/logo_zelina.h` | logo bitmap |

## Continuar daqui

O demo de 3 rounds está jogável. Próximos passos só com pedido: mais conteúdo, mapper maior, ou polish pontual. Cuidados: eject da ROM no openMSX antes do rebuild se o `.rom` estiver locked; C-BIOS precisa `romInsert` + `reset`.

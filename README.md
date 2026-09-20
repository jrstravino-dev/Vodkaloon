# Vodkaloon

**Vodkaloon** é um exemplo / demo de um jogo de 8 bits para **MSX2**, produzido com ferramentas de IA.

Feito com [MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT) (Cursor, MSXgl, SDCC, openMSX).

## Stack


| Layer           | Choice                                                                                                            |
| --------------- | ----------------------------------------------------------------------------------------------------------------- |
| Editor / AI IDE | [Cursor](https://cursor.com) + [project rules](https://github.com/hrubix/MSX-AI-Dev-INIT/tree/main/.cursor/rules) |
| Framework       | [MSXgl](https://github.com/aoineko-fr/MSXgl)                                                                      |
| Compiler        | [SDCC](https://www.msx.org/wiki/SDCC)                                                                             |
| Emulator        | [openMSX 21.0](https://openmsx.org/)                                                                              |
| OpenMSX MCP     | [@nataliapc/mcp-openmsx](https://github.com/nataliapc/mcp-openmsx)                                                |


**Strengths:**

- Strong edit, compile, test and debug workflow.
- MSXgl provides an extensive API for MSX game development.
- Most logic can be written in C.
- Performance-sensitive routines can still be implemented in assembly!
- openMSX MCP allows the AI agent to launch and control the emulator and automate testing.

> **(Note:** If you are considering an assembly-only stack for projects that require maximum control over the MSX hardware, a possible workflow would be:
>
> IDE → LLM → [SjASMPlus](https://github.com/sjasmplus/sjasmplus) or [Glass](https://www.grauw.nl/projects/glass/) → mcp-openMSX → openMSX).
>
> Check the [Example page](examples.md) for more AI stack setups



## Plan mode & OpenSpec

**Tip:** Start with your IDE’s Plan Mode to create a clear, reviewable checklist before any code is written. For example: “Create a plan for a simple MSX1-style Snake game. Ask me questions to clarify the requirements and improve the plan before implementation.”

For larger projects with complex or evolving requirements, consider using [OpenSpec](https://openspec.dev/). OpenSpec is a structured workflow for defining and maintaining specifications, including requirements, design decisions, implementation tasks, and proposed changes.

## What you get

- MSX2 project layout (`ROM_32K`)
- [Cursor rules](https://github.com/hrubix/MSX-AI-Dev-INIT/tree/main/.cursor/rules) + MCP config template for openMSX automation
- `Vodkaloon` ROM source (`src/main.c`)
- Windows helper to launch openMSX from mcp-openmsx (`tools/openmsx-mcp-launcher.*`)



## Quick start

Point any AI coding agent at this repo. Example prompts:

**Install all project requirements:**

> Set up the MSX stack for [https://github.com/hrubix/MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT) on my machine. Detect my OS first and use its native package manager where possible. Install Git, Node.js LTS, openMSX 21 + C-BIOS ROMs, and mcp-openmsx (npm i -g @nataliapc/mcp-openmsx). Clone MSXgl into the repo root, fix the SDCC cc1 binary if missing, copy the MCP config template (.cursor/mcp.json.example) to .cursor/mcp.json and set local paths. Verify with the project build. Show commands first and ask me about anything unclear instead of guessing.

**Bootstrap a new project:**

> Bootstrap a new MSX project from [https://github.com/hrubix/MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT): read its README and .cursor/rules/* and follow them, ask me for project name/machine/timing/ROM format, then create a buildable project from the template. Ask me about anything unclear instead of guessing.



### About Vodkaloon

Demo jogável de um título MSX2 feito com IA, usando [MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT).

- Play in browser (MSX2 PAL): [WebMSX](https://webmsx.org/?MACHINE=MSX2E&ROM=https://github.com/jrstravino-dev/Vodkaloon/raw/main/samples/Vodkaloon.rom)
- Local: `samples/Vodkaloon.rom` or `emul/rom/Vodkaloon.rom` in openMSX (C-BIOS MSX2, PAL)
- Opening: Zelina Games `PRESENTS` (~8 s, SPACE skips); footer `2026 BY JUNIOR STRAVINO`
- Then title (`SCORE`/`HI`, `PUSH SPACE KEY`) → briefing → ROUND → taxi → 3 rounds on the Praça República Lituana skyline
- HUD: score, balloons, festa, vodka bottle; GAME OVER banner; STOP pause, ESC title
- Setas movem Hector, SPACE solta balões
- PSG: opening homage to Knightmare (3/4, original melody), in-game ostinato, win/lose; SFX pop/score/cloud
- Opening tables: `tools/make_opening_music.py` → `generated/zelina_music.h`



## Examples

A few MSX titles built with AI — demos and small games you can load in openMSX. Check the [Example page](examples.md) for more AI stack setups and how to try them.

## Docs

- [docs/project-requirements.md](docs/project-requirements.md) — tools and dependencies overview
- [examples.md](examples.md) — sample ROMs built with AI
- [msx-dev-ai.md](msx-dev-ai.md) — AI/dev workflow
- [tests/smoke-test.md](tests/smoke-test.md) — emulator smoke procedure



## License notes

- [MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT) is public domain ([The Unlicense](https://unlicense.org)).
- **MSXgl**, **openMSX**, **C-BIOS**, and **mcp-openmsx** keep their own licenses — clone/install them separately.


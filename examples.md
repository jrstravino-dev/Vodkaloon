# Examples

Sample ROMs from MSX software **built with AI**. They are included here as inspiration — not every title used the same toolchain as [this template](README.md).

Load any `.rom` from `samples/` into openMSX (or another MSX emulator) to try them.

| ROM | WebMSX | Description | AI stack |
| --- | --- | --- | --- |
| [`helloworld.rom`](samples/helloworld.rom) | — | Simple template demo: prints a greeting, waits for SPACE, then exits. Source in this repo: `src/main.c`. | This repo stack |
| [![snAIk title screen](docs/assets/examples/snAIk-title-thumb.png)](samples/snAIk.rom)<br>[`snAIk.rom`](samples/snAIk.rom) | [snAIk](https://download.file-hunter.com/assets/webmsx.html?url=https%3A%2F%2Fdownload.file-hunter.com%2FGames%2FMSX1%2FROM%2FsnAIk%2520-%2520Rubikon%2520Lab%2520(2026).zip) | MSX1-style Snake game by [Rubikon Lab](https://rubikonlab.com). | This repo stack |
| [![Airplane title screen](docs/assets/examples/airplane-title-thumb.png)](samples/airplane.rom)<br>[`airplane.rom`](samples/airplane.rom) | [Airplane](http://webmsx.org/?ROM=https://www.jeroenderwort.nl/wp-content/uploads/airplane.rom) | Airplane demo / AI game by [Jeroen Derwort](https://www.jeroenderwort.nl/airplane-mijn-eerste-msx-game-opnieuw-gebouwd-in-assembly/) (Dutch article). | Antigravity IDE → Claude + Gemini LLM → Z80 ASM + SjASMPlus → openMSX (no MCP connector) |

More examples welcome — share your AI-built MSX projects and we can add them here.

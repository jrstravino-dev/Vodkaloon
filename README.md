# MSX-AI-Dev-INIT

Starter project for building **MSX** software with **Cursor** (AI-assisted), **MSXgl**, **SDCC**, **openMSX**, and **mcp-openmsx**.

Repo: [hrubix/MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT)

## Stack


| Layer           | Choice                                                             |
| --------------- | ------------------------------------------------------------------ |
| Editor / AI IDE | Cursor + project rules                                             |
| Framework       | MSXgl                                                              |
| Compiler        | SDCC                                                               |
| Emulator        | openMSX 21                                                         |
| OpenMSX MCP     | [@nataliapc/mcp-openmsx](https://github.com/nataliapc/mcp-openmsx) |


**Strengths:**

- Strong edit, compile, test and debug workflow.
- MSXgl provides an extensive API for MSX game development.
- Most logic can be written in C.
- Performance-sensitive routines can still be implemented in assembly!
- openMSX MCP allows the AI agent to launch and control the emulator and automate testing.

## What you get

- MSX2 project layout (provisional `ROM_32K`)
- [Cursor rules](https://github.com/hrubix/MSX-AI-Dev-INIT/tree/main/.cursor/rules) + MCP config template for openMSX automation
- `helloworld` demo ROM source (`src/main.c`)
- Windows helper to launch openMSX from mcp-openmsx (`tools/openmsx-mcp-launcher.*`)

## Quick start (Windows 11)

1. **Clone** this repo (path without spaces recommended).
2. **Clone MSXgl** into the project root:
  ```bat
   git clone https://github.com/aoineko-fr/MSXgl.git MSXgl
  ```
3. If SDCC fails with a strange `cc1` error, copy:
  `MSXgl\tools\sdcc\bin\cc1` → `MSXgl\tools\sdcc\bin\cc1.exe`
4. **Install** [openMSX](https://openmsx.org/), [Node.js LTS](https://nodejs.org/), and C-BIOS ROMs into your openMSX `systemroms` pool (see `docs/project-requirements.md`).
5. Copy MCP config and edit paths:
  ```bat
   copy .cursor\mcp.json.example .cursor\mcp.json
  ```
   Set `OPENMSX_SHARE_DIR`, screenshot/replay dirs, and keep `OPENMSX_EXECUTABLE` pointed at `tools\openmsx-mcp-launcher.exe`.
6. Enable **mcp-openmsx** in Cursor: `Ctrl+Shift+J` → **Tools & MCP**.
7. **Build**:
  ```bat
   build.bat
  ```
   Output: `emul\rom\helloworld.rom`
8. Run the smoke flow in Agent chat (or follow `tests/smoke-test.md`).

## Alternative: initialize via AI prompt

Instead of the manual steps, point any AI coding agent at this repo, for example with a prompt like:

> Initialize a new MSX project based on
> [https://github.com/hrubix/MSX-AI-Dev-INIT](https://github.com/hrubix/MSX-AI-Dev-INIT) — replicate its structure, rules,
> config templates, and MSXgl/SDCC/openMSX build setup. Ask me for project name,
> target machine, ROM/mapper format, and PAL/NTSC timing before the first build.

### About simple helloworld demo

- Prints `Hi, I'm your MSX AI Development stack`
- Then `Press SPACE to exit`
- SPACE → `Bye!` and CPU halt (a cart ROM cannot quit the openMSX process; close the window on the host)

Sample binary (optional): `samples/helloworld.rom`

## Docs

- [docs/project-requirements.md](docs/project-requirements.md) — install checklist
- [msx-dev-ai.md](msx-dev-ai.md) — AI/dev workflow
- [tests/smoke-test.md](tests/smoke-test.md) — emulator smoke procedure

## License notes

- This template is released into the public domain under [The Unlicense](https://unlicense.org) — use it for anything, commercial or private, no restrictions and no attribution required. Attribution to [rubikonlab.com](https://rubikonlab.com) is appreciated but not obligatory.
- **MSXgl**, **openMSX**, **C-BIOS**, and **mcp-openmsx** keep their own licenses — clone/install them separately.


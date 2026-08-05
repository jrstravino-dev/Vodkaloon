# Project requirements

Overview of tools, libraries, frameworks, and dependencies used in this MSX development stack. Aligns with [msx-dev-ai.md](../msx-dev-ai.md).

Setup is handled via your AI IDE — see the install prompt in [README.md](../README.md).

---

## Core stack

| Component | Role | Link |
| --- | --- | --- |
| **AI IDE** (e.g. Cursor) | Editor, agent terminal, MCP host, project rules | [cursor.com](https://cursor.com/) |
| **MSXgl** | MSX game framework and build tooling | [github.com/aoineko-fr/MSXgl](https://github.com/aoineko-fr/MSXgl) |
| **SDCC** | C compiler (bundled with MSXgl on Windows) | [sdcc.sourceforge.net](https://sdcc.sourceforge.net/) |
| **openMSX** | Primary emulator and debugger | [openmsx.org](https://openmsx.org/) |
| **mcp-openmsx** | MCP bridge for AI control of openMSX | [github.com/nataliapc/mcp-openmsx](https://github.com/nataliapc/mcp-openmsx) |
| **Git** | Version control; cloning MSXgl | [git-scm.com](https://git-scm.com/) |
| **Node.js** (LTS, ≥ 18.12.1) | Runtime for mcp-openmsx | [nodejs.org](https://nodejs.org/) |
| **C-BIOS** | System ROMs for openMSX (required for C-BIOS machines) | [cbios.sourceforge.net](https://cbios.sourceforge.net/) |

---

## Project defaults

| Topic | Default |
| --- | --- |
| Project name | **TestGame** (template demo) |
| Minimum MSX generation | **MSX2** |
| Timing | **PAL** |
| MSXgl location | Cloned into `MSXgl/` |
| Delivery format / mapper | Provisional `ROM_32K` (TBD) |
| Video | TBD — starter uses Screen 5 |
| Audio | TBD |
| Hardware validation | openMSX only |

Do not change compiler, framework, machine, or provisional `ROM_32K` unless the task explicitly requires it.

---

## Optional / alternative

| Component | When used | Link |
| --- | --- | --- |
| **SjASMPlus** | Assembly-heavy projects; alternative to MSXgl's default assembler | [github.com/sjasmplus/sjasmplus](https://github.com/sjasmplus/sjasmplus) |
| **Glass** | Z80 cross-assembler (Java); alternative assembly toolchain | [grauw.nl/projects/glass](https://www.grauw.nl/projects/glass/) |
| **Make** | Optional build entry; `build.bat` is sufficient on Windows | — |
| **openMSX Debugger UI** | Desktop debugger alongside MCP | [openmsx.org](https://openmsx.org/) |
| **OpenSpec** | Structured specs for larger or evolving projects | [openspec.dev](https://openspec.dev/) |
| **Image / map editors** | Authoring `assets/` | — |
| **Music trackers** | After audio pipeline is chosen | — |
| **Real MSX / FPGA** | Hardware validation after emulator phase | — |

---

## Third-party licenses

**MSXgl**, **openMSX**, **C-BIOS**, and **mcp-openmsx** keep their own licenses — install them separately. This template is released under [The Unlicense](../LICENSE).

---

## References

- Stack workflow: [msx-dev-ai.md](../msx-dev-ai.md)
- MSXgl docs: [aoineko.org/msxgl](https://aoineko.org/msxgl/)
- openMSX manual: [openmsx.org/manual](https://openmsx.org/manual/)
- MSX wiki: [Programming](https://www.msx.org/wiki/Category:Programming), [Graphics](https://www.msx.org/wiki/Category:Graphics), [Music](https://www.msx.org/wiki/Category:Music)

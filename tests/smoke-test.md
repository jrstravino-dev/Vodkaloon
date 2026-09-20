# Smoke test

Run after a successful ROM build and with mcp-openmsx connected.

## Preconditions

- `build.bat` produces `emul\rom\Vodkaloon.rom`
- mcp-openmsx enabled in Cursor (**Tools & MCP**)
- `.cursor/mcp.json` uses `tools\openmsx-mcp-launcher.exe` on Windows
- Machine: **C-BIOS_MSX2_EU** (PAL / 50 Hz)

## Steps

1. Build ROM (`build.bat`) if needed.
2. `emu_control` launch `C-BIOS_MSX2_EU`.
3. `emu_media` romInsert `E:/DEV/PROJETO-MSX/emul/rom/Vodkaloon.rom` (use forward slashes).
4. `emu_control` reset.
5. `emu_control` wait **4** seconds (logo still on screen).
6. `screen_shot` → expect Zelina Games, `PRESENTS`, `2026 BY JUNIOR STRAVINO`.
7. `emu_control` wait **8** more seconds → title (`VODKALOON`, `PUSH SPACE KEY`).
8. `emu_vdp` screenGetMode → expect **5**.
9. SPACE → briefing → demo; ESC returns to title.
10. `emu_control` close.

## Pass criteria

| Check | Expected |
| --- | --- |
| Emulator stays running | No unexpected halt |
| Opening (~8 s) | Zelina Games + PRESENTS + 2026 BY JUNIOR STRAVINO |
| Title | VODKALOON starring Hector, SCORE/HI, PUSH SPACE KEY |
| Screen mode | **5** |

## Last run

- **Result:** PASS (2026-09-20)
- Machine: `C-BIOS_MSX2_EU`
- Screen mode: `5`
- Evidence: openMSX screenshot of Zelina Games credit screen

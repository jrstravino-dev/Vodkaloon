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
5. `emu_control` wait **8** seconds (logo, PRESENTS, then title).
6. `screen_shot` to_file → copy/rename to `screenshots/smoke-latest.png`.
7. `emu_vdp` screenGetMode → expect **5**.
8. Confirm `Zelina Games`, `PRESENTS`, `VODKALOON`, then briefing; SPACE starts the demo.
9. `emu_control` close.

## Pass criteria

| Check | Expected |
| --- | --- |
| Emulator stays running | No unexpected halt |
| Screenshot | Zelina Games + PRESENTS + VODKALOON starring Hector |
| Screen mode | **5** |

## Last run

- **Result:** PASS (2026-07-16)
- Machine: `C-BIOS_MSX2_EU`
- Screen mode: `5`
- Evidence: `screenshots/smoke-latest.png` / `screenshots/mcp_*_0001.png`

"""Generate Zelina Games opening MIDI + compact PSG note tables.

Homage to 1980s Konami MSX adventure (Knightmare / Maze of Galious) —
original theme, not a transcription of copyrighted music.
"""
from __future__ import annotations

import math
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "assets"
GENERATED = ROOT / "generated"

PPQ = 480
BPM = 150
TEMPO = 400000  # 150 BPM, 3/4 eighths
STEP_TICKS = PPQ // 2  # eighth note
MSX_STEP_FRAMES = 8  # ~188 BPM-eighth feel at 50 Hz PAL
PSG_CLOCK = 111860.8

NOTE = {
    "R": 0,
    "E1": 28, "F1": 29, "G1": 31, "A1": 33, "Bb1": 34, "B1": 35,
    "C2": 36, "D2": 38, "E2": 40, "F2": 41, "F#2": 42, "G2": 43,
    "A2": 45, "Bb2": 46, "B2": 47, "C3": 48, "D3": 50, "E3": 52,
    "F3": 53, "F#3": 54, "G3": 55, "A3": 57, "Bb3": 58, "B3": 59,
    "C4": 60, "D4": 62, "E4": 64, "F4": 65, "F#4": 66, "G4": 67,
    "A4": 69, "Bb4": 70, "B4": 71, "C5": 72, "D5": 74,
}


def parse_line(text: str) -> list[int]:
    return [NOTE[tok] for tok in text.replace("|", " ").split()]


# 8 bars x 6 eighths = 48 steps in 3/4 (Knightmare-like waltz, original notes)
FANFARE_A = parse_line(
    """
    E4 R G4 B4 R R
    E4 G4 B4 C5 B4 A4
    G4 B4 D5 C5 B4 A4
    B4 R B4 C5 D5 R
    E4 G4 B4 A4 G4 F#4
    E4 R E4 G4 B4 R
    C5 B4 A4 G4 F#4 E4
    E4 E4 B3 B3 E4 R
    """
)
FANFARE_B = parse_line(
    """
    R E3 R B3 R R
    R G3 R E3 R G3
    R G3 R D3 R G3
    R B3 R G3 R B3
    R E3 R B3 R E3
    R E3 R G3 R B3
    R A3 R F#3 R A3
    R E3 R B3 R E3
    """
)
FANFARE_C = parse_line(
    """
    E2 E2 E2 B1 B1 B1
    E2 E2 E2 B1 B1 B1
    G2 G2 G2 D2 D2 D2
    B1 B1 B1 E2 E2 E2
    E2 E2 E2 B1 B1 B1
    E2 E2 E2 G1 G1 G1
    A1 A1 A1 E1 E1 E1
    E2 E2 E2 E2 E2 R
    """
)

LOOP_A = parse_line(
    """
    G4 A4 B4 C5 B4 A4
    G4 F#4 E4 F#4 G4 A4
    B4 C5 D5 C5 B4 A4
    G4 A4 F#4 E4 R R
    B4 R D5 C5 B4 A4
    G4 A4 B4 A4 G4 F#4
    E4 G4 A4 B4 C5 B4
    A4 G4 F#4 E4 E4 R
    """
)
LOOP_B = parse_line(
    """
    R E3 R G3 R B3
    R E3 R F#3 R A3
    R G3 R B3 R D4
    R E3 R F#3 R R
    R G3 R B3 R D4
    R E3 R G3 R A3
    R E3 R G3 R A3
    R E3 R F#3 R E3
    """
)
LOOP_C = parse_line(
    """
    E2 E2 E2 B1 B1 B1
    E2 E2 E2 B1 B1 B1
    G2 G2 G2 D2 D2 D2
    E2 E2 E2 B1 B1 B1
    G2 G2 G2 D2 D2 D2
    E2 E2 E2 B1 B1 B1
    A1 A1 A1 E2 E2 E2
    E2 E2 E2 B1 B1 E2
    """
)


def assert_len(name: str, seq: list[int], n: int) -> None:
    if len(seq) != n:
        raise SystemExit(f"{name} has {len(seq)} steps, expected {n}")


for name, seq in (
    ("FANFARE_A", FANFARE_A),
    ("FANFARE_B", FANFARE_B),
    ("FANFARE_C", FANFARE_C),
    ("LOOP_A", LOOP_A),
    ("LOOP_B", LOOP_B),
    ("LOOP_C", LOOP_C),
):
    assert_len(name, seq, 48)


def vlq(value: int) -> bytes:
    buf = [value & 0x7F]
    value >>= 7
    while value:
        buf.append(0x80 | (value & 0x7F))
        value >>= 7
    return bytes(reversed(buf))


def midi_track(events: list[tuple[int, bytes]]) -> bytes:
    events = sorted(events, key=lambda e: e[0])
    body = bytearray()
    last = 0
    for tick, payload in events:
        body += vlq(tick - last)
        body += payload
        last = tick
    body += vlq(0) + bytes([0xFF, 0x2F, 0x00])
    return b"MTrk" + struct.pack(">I", len(body)) + bytes(body)


def notes_to_events(notes: list[int], channel: int, program: int, velocity: int) -> list[tuple[int, bytes]]:
    events = [(0, bytes([0xC0 | channel, program]))]
    t = 0
    i = 0
    while i < len(notes):
        n = notes[i]
        if n == 0:
            t += STEP_TICKS
            i += 1
            continue
        length = 1
        while i + length < len(notes) and notes[i + length] == n:
            length += 1
        dur = length * STEP_TICKS
        events.append((t, bytes([0x90 | channel, n, velocity])))
        events.append((t + dur, bytes([0x80 | channel, n, 0])))
        t += dur
        i += length
    return events


def write_midi(path: Path) -> None:
    header = b"MThd" + struct.pack(">IHHH", 6, 1, 4, PPQ)
    meta = midi_track(
        [
            (0, bytes([0xFF, 0x51, 0x03]) + struct.pack(">I", TEMPO)[1:]),
            (0, bytes([0xFF, 0x03, 13]) + b"Zelina Games"),
        ]
    )
    # Repeat fanfare once then loop twice in the MIDI preview
    a = FANFARE_A + LOOP_A + LOOP_A
    b = FANFARE_B + LOOP_B + LOOP_B
    c = FANFARE_C + LOOP_C + LOOP_C
    melody = midi_track(notes_to_events(a, 0, 80, 96))   # Square Lead
    harmony = midi_track(notes_to_events(b, 1, 6, 72))    # Harpsichord
    bass = midi_track(notes_to_events(c, 2, 32, 88))      # Acoustic Bass
    path.write_bytes(header + meta + melody + harmony + bass)


def period(midi_note: int) -> int:
    freq = 440.0 * (2.0 ** ((midi_note - 69) / 12.0))
    return max(1, min(4095, int(round(PSG_CLOCK / freq))))


def c_rows(a: list[int], b: list[int], c: list[int]) -> str:
    lines = []
    for i, (na, nb, nc) in enumerate(zip(a, b, c)):
        comma = "," if i < len(a) - 1 else ""
        lines.append(f"\t{{ {na}, {nb}, {nc} }}{comma}")
    return "\n".join(lines)


def write_header(path: Path) -> None:
    lo, hi = 28, 74
    periods = ", ".join(str(period(n)) for n in range(lo, hi + 1))
    text = f"""#pragma once
/* Generated by tools/make_opening_music.py — do not hand-edit. */

#define MUSIC_NOTE_MIN      {lo}
#define MUSIC_NOTE_MAX      {hi}
#define MUSIC_STEP_FRAMES   {MSX_STEP_FRAMES}
#define MUSIC_FANFARE_LEN   {len(FANFARE_A)}
#define MUSIC_LOOP_LEN      {len(LOOP_A)}

static const u16 g_MusicPeriod[] = {{ {periods} }};

static const u8 g_MusicFanfare[][3] =
{{
{c_rows(FANFARE_A, FANFARE_B, FANFARE_C)}
}};

static const u8 g_MusicLoop[][3] =
{{
{c_rows(LOOP_A, LOOP_B, LOOP_C)}
}};
"""
    path.write_text(text, encoding="utf-8")


def main() -> None:
    ASSETS.mkdir(exist_ok=True)
    GENERATED.mkdir(exist_ok=True)
    mid = ASSETS / "zelina-opening.mid"
    hdr = GENERATED / "zelina_music.h"
    write_midi(mid)
    write_header(hdr)
    print(f"Wrote {mid} ({mid.stat().st_size} bytes)")
    print(f"Wrote {hdr} ({hdr.stat().st_size} bytes)")


if __name__ == "__main__":
    main()

/*
 * Vodkaloon — MSX2 (PAL) ROM_32K
 * Opening: Zelina Games presents Vodkaloon starring Hector
 */

#include "msxgl.h"
#include "psg.h"
#include "font/font_mgl_sample6.h"
#include "generated/logo_zelina.h"
#include "include/music.h"
#include "include/game.h"

#define LOGO_W			240
#define LOGO_H			48
#define LOGO_X			8
#define LOGO_Y			72

static bool g_SkipOpen;

static void WaitFrames(u8 frames)
{
	u8 i;
	for (i = 0; i < frames; ++i)
	{
		Music_Tick();
		Halt();
		if (Keyboard_IsKeyPressed(KEY_SPACE))
			g_SkipOpen = TRUE;
		if (g_SkipOpen)
			return;
	}
}

static void WaitKeyState(u8 key, bool wantPressed, u8 frames)
{
	u8 stable = 0;
	while (stable < frames)
	{
		Music_Tick();
		Halt();
		if (wantPressed)
		{
			if (Keyboard_IsKeyPressed(key))
				stable++;
			else
				stable = 0;
		}
		else
		{
			if (!Keyboard_IsKeyPressed(key))
				stable++;
			else
				stable = 0;
		}
	}
}

/* Silver ramp matching the converted logo (palette indices 1..8). */
static void SetLogoPalette(void)
{
	VDP_SetPaletteEntry(1, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(2, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(3, RGB16(1, 1, 1));
	VDP_SetPaletteEntry(4, RGB16(3, 3, 3));
	VDP_SetPaletteEntry(5, RGB16(4, 4, 4));
	VDP_SetPaletteEntry(6, RGB16(5, 5, 5));
	VDP_SetPaletteEntry(7, RGB16(6, 6, 6));
	VDP_SetPaletteEntry(8, RGB16(7, 7, 7));
}

static void ShineLogo(void)
{
	u8 i;

	for (i = 0; i < 12; ++i)
	{
		if (g_SkipOpen)
			break;
		VDP_SetPaletteEntry(8, RGB16(7, 7, 7));
		VDP_SetPaletteEntry(7, RGB16(6, 6, 6));
		VDP_SetPaletteEntry(6, RGB16(4, 4, 4));
		WaitFrames(3);
		VDP_SetPaletteEntry(8, RGB16(5, 5, 5));
		VDP_SetPaletteEntry(7, RGB16(7, 7, 7));
		VDP_SetPaletteEntry(6, RGB16(6, 6, 6));
		WaitFrames(3);
		VDP_SetPaletteEntry(8, RGB16(6, 6, 6));
		VDP_SetPaletteEntry(7, RGB16(5, 5, 5));
		VDP_SetPaletteEntry(6, RGB16(7, 7, 7));
		WaitFrames(3);
	}
	SetLogoPalette();
}

static void PlayOpening(void)
{
	g_SkipOpen = FALSE;
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetColor(COLOR_BLACK);
	VDP_EnableVBlank(TRUE);
	VDP_ClearVRAM();
	SetLogoPalette();

	WaitFrames(40);

	Music_Init();
	VDP_CommandHMMC(g_LogoZelina, LOGO_X, LOGO_Y, LOGO_W, LOGO_H);
	WaitFrames(4);

	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(COLOR_WHITE, COLOR_BLACK);
	Print_DrawTextAt(104, 132, "PRESENTS");

	ShineLogo();
}

static void DrawNum6(u8 x, u8 y, u16 n)
{
	u8 i;
	u16 v = n;
	u8 d[6];

	for (i = 6; i > 0; --i)
	{
		d[i - 1] = (u8)(v % 10);
		v /= 10;
	}
	Print_SetPosition(x, y);
	for (i = 0; i < 6; ++i)
		Print_DrawChar((c8)('0' + d[i]));
}

/* 0 = start game, 1 = attract. */
static u8 PlayTitle(void)
{
	u16 t;

	VDP_EnableSprite(FALSE);
	VDP_CommandHMMV(0, 0, 256, 212, 0);
	VDP_CommandWait();
	SetLogoPalette();
	Music_Init();

	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(COLOR_WHITE, COLOR_BLACK);
	Print_DrawTextAt(92, 48, "VODKALOON");
	Print_DrawTextAt(80, 64, "starring Hector");
	Print_DrawTextAt(40, 104, "SCORE");
	DrawNum6(80, 104, Game_GetScore());
	Print_DrawTextAt(40, 120, "HI");
	DrawNum6(80, 120, Game_GetHiScore());
	Print_DrawTextAt(80, 176, "PRESSIONE ESPACO");

	WaitKeyState(KEY_SPACE, FALSE, 8);
	for (t = 0; t < 900; ++t)
	{
		Music_Tick();
		Halt();
		if (Keyboard_IsKeyPressed(KEY_SPACE))
		{
			WaitKeyState(KEY_SPACE, FALSE, 8);
			return 0;
		}
	}
	return 1;
}

static bool g_SkipType;

static void TypeChar(c8 chr)
{
	Print_DrawChar((u8)chr);
	if (g_SkipType)
		return;

	if (chr == ' ')
	{
		WaitFrames(2);
		return;
	}

	Sfx_TypeClick((chr == '.') ? 12 : 10);
	WaitFrames(2);
	PSG_SetVolume(PSG_CHANNEL_A, 0);
	PSG_Apply();
	WaitFrames((chr == '.') ? 12 : 3);

	if (Keyboard_IsKeyPressed(KEY_SPACE))
		g_SkipType = TRUE;
}

static void TypeLine(UX x, UY y, const c8* text)
{
	Print_SetPosition(x, y);
	while (*text != 0)
	{
		TypeChar(*text);
		text++;
	}
}

static void PlayBriefing(void)
{
	Music_Pause();
	VDP_CommandHMMV(0, 0, 256, 212, 0);
	WaitFrames(12);

	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(COLOR_WHITE, COLOR_BLACK);

	g_SkipType = FALSE;
	TypeLine(16, 40, "AJUDE O HECTOR BALLOONSKI A");
	TypeLine(16, 56, "SOLTAR BALOES PELO CEU DA");
	TypeLine(16, 72, "VILA ZELINA SEM DERRAMAR A VODKA.");
	WaitFrames(g_SkipType ? 2 : 10);
	TypeLine(16, 104, "QUANTO MAIS BALOES SUBIREM,");
	TypeLine(16, 120, "MAIOR A FESTA. SE A GARRAFA");
	TypeLine(16, 136, "ACABAR, O SONHO DESINFLA.");
	WaitFrames(g_SkipType ? 2 : 16);
	TypeLine(80, 176, "PRESSIONE ESPACO");

	Music_Resume();
}

void main(void)
{
	PlayOpening();

	for (;;)
	{
		if (PlayTitle() != 0)
		{
			Game_Attract();
			continue;
		}

		PlayBriefing();

		WaitKeyState(KEY_SPACE, FALSE, 15);
		WaitKeyState(KEY_SPACE, TRUE, 8);
		WaitKeyState(KEY_SPACE, FALSE, 8);

		Game_Play();
	}
}

/*
 * Vodkaloon demo — viking na Vila Zelina, chega de taxi.
 */

#include "msxgl.h"
#include "psg.h"
#include "include/music.h"
#include "include/game.h"

extern const unsigned char g_Font_MGL_Sample6[];

#define SPR_HELM		0
#define SPR_HECTOR		1
#define SPR_TAXI_LD		2
#define SPR_TAXI_L		3
#define SPR_TAXI_RD		4
#define SPR_TAXI_R		5
#define SPR_TAXI_LW		6
#define SPR_TAXI_RW		7
#define SPR_BALLOON		8
#define BALLOON_MAX		4
#define PAT_HECTOR		0
#define PAT_HELM		4
#define PAT_TAXI_L		8
#define PAT_TAXI_R		12
#define PAT_TAXI_LD		16
#define PAT_TAXI_RD		20
#define PAT_TAXI_LW		24
#define PAT_TAXI_RW		28
#define PAT_BALLOON		32
#define PAT_HECTOR2		36
#define HECTOR_Y		156
#define TAXI_Y			158
#define TAXI_STOP		88
#define HECTOR_MIN		8
#define HECTOR_MAX		232
#define SKY_TOP			20
#define VODKA_MAX		8
#define VODKA_BAR		8
#define FESTA_WIN		5
#define ROUND_MAX		3
#define SCORE_BALLOON	100
#define SCORE_VODKA		100
#define FIRE_WAIT		12
#define WIND_PERIOD		80
#define CLOUD_MAX		3
#define CLOUD_H			16
#define BALLOON_X_MIN	2
#define BALLOON_X_MAX	240

typedef struct
{
	bool alive;
	u8 x;
	u8 y;
	u8 color;
} Balloon;

static Balloon g_Balloon[BALLOON_MAX];
static u8 g_HectorX;
static u8 g_Vodka;
static u8 g_Festa;
static u8 g_FireWait;
static bool g_FireHeld;
static bool g_Ended;
static bool g_Won;
static bool g_RetryReady;
static bool g_Walking;
static bool g_Attract;
static u8 g_Frame;
static u8 g_Round;
static u16 g_Score;
static u16 g_HiScore;
static i8 g_Wind;
static u8 g_WindTime;
static u8 g_WindIdx;

typedef struct
{
	u8 x;
	u8 y;
	u8 w;
	i8 dir;
} Cloud;

static Cloud g_Cloud[CLOUD_MAX];
static const i8 g_WindCycle[4] = { -1, 0, 1, 0 };

/* Knightmare-like human (Popolon): head, visor, torso, two legs. */
static const u8 g_PatViking[32] =
{
	0x0F, 0x1F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x1F,
	0x3F, 0x7F, 0x67, 0x3F, 0x06, 0x06, 0x06, 0x0E,
	0xF0, 0xF8, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xF8,
	0xFC, 0xFE, 0xE6, 0xFC, 0x60, 0x60, 0x60, 0x70
};

/* Walk frame 1: opposite legs. */
static const u8 g_PatViking2[32] =
{
	0x0F, 0x1F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x1F,
	0x3F, 0x7F, 0x67, 0x3F, 0x0C, 0x0C, 0x06, 0x0E,
	0xF0, 0xF8, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xF8,
	0xFC, 0xFE, 0xE6, 0xFC, 0x30, 0x30, 0x60, 0x70
};

/* Black details in front: helmet, visor slots, mouth, belt, boots. */
static const u8 g_PatHelm[32] =
{
	0x0F, 0x1F, 0x33, 0x00, 0x06, 0x00, 0x00, 0x00,
	0x00, 0x1E, 0x00, 0x0E, 0x00, 0x00, 0x06, 0x0E,
	0xF0, 0xF8, 0xCC, 0x00, 0x60, 0x00, 0x00, 0x00,
	0x00, 0x78, 0x00, 0x70, 0x00, 0x00, 0x60, 0x70
};

/* 32x16 taxi like the reference: box body, lamp, 2 windows, checker, 2 wheels. Facing right. */
static const u8 g_PatTaxiL[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x03, 0x07, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x3F, 0xFF, 0xC1, 0xC1, 0xC1, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF8, 0xF8, 0xF0
};

static const u8 g_PatTaxiR[32] =
{
	0x30, 0x30, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x1F, 0x1F, 0x0F,
	0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF8, 0xF8, 0xF8, 0xFC, 0x00, 0x80, 0x80, 0x00
};

static const u8 g_PatTaxiLD[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3E, 0x00,
	0x55, 0x00, 0x00, 0x00, 0xF0, 0x98, 0x98, 0xF0
};

static const u8 g_PatTaxiRD[32] =
{
	0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x7F, 0x00,
	0x55, 0x00, 0x00, 0x00, 0x0F, 0x19, 0x19, 0x0F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x50, 0x0C, 0x0C, 0x00, 0x00, 0x80, 0x80, 0x00
};

static const u8 g_PatTaxiLW[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x3E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 g_PatTaxiRW[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 g_PatBalloon[32] =
{
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF,
	0x7F, 0x3F, 0x1F, 0x0F, 0x06, 0x02, 0x02, 0x01,
	0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF,
	0xFE, 0xFC, 0xF8, 0xF0, 0x60, 0x40, 0x40, 0x80
};

static const u8 g_BalloonColor[4] = { 8, 9, 14, 8 };

static void Tick(void)
{
	volatile u16* jiffy = (volatile u16*)(u16)0xFC9E;
	u16 t = *jiffy;
	Music_Tick();
	while (*jiffy == t)
		Halt();
}

static void PlaceHector(u8 x, u8 y)
{
	u8 pat = (g_Walking && ((g_Frame & 8) != 0)) ? PAT_HECTOR2 : PAT_HECTOR;
	VDP_SetSpriteExUniColor(SPR_HELM, x, y, PAT_HELM, 1);
	VDP_SetSpriteExUniColor(SPR_HECTOR, x, y, pat, 15);
}

static void PlaceTaxi(u8 x)
{
	u8 xr = (u8)(x + 16);
	VDP_SetSpritePosition(SPR_TAXI_L, x, TAXI_Y);
	VDP_SetSpritePosition(SPR_TAXI_LD, x, TAXI_Y);
	VDP_SetSpritePosition(SPR_TAXI_LW, x, TAXI_Y);
	VDP_SetSpritePosition(SPR_TAXI_R, xr, TAXI_Y);
	VDP_SetSpritePosition(SPR_TAXI_RD, xr, TAXI_Y);
	VDP_SetSpritePosition(SPR_TAXI_RW, xr, TAXI_Y);
}

static void HideTaxi(void)
{
	VDP_HideSprite(SPR_TAXI_L);
	VDP_HideSprite(SPR_TAXI_LD);
	VDP_HideSprite(SPR_TAXI_LW);
	VDP_HideSprite(SPR_TAXI_R);
	VDP_HideSprite(SPR_TAXI_RD);
	VDP_HideSprite(SPR_TAXI_RW);
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

static void DrawScore(void)
{
	Print_SetColor(14, 1);
	Print_DrawTextAt(8, 2, "SCORE");
	DrawNum6(44, 2, g_Score);
	Print_DrawTextAt(92, 2, "HI");
	DrawNum6(108, 2, g_HiScore);
}

static void AddScore(u16 n)
{
	u16 s;

	if (g_Attract)
		return;
	s = (u16)(g_Score + n);
	if (s < g_Score)
		s = 65535;
	g_Score = s;
	if (g_Score > g_HiScore)
		g_HiScore = g_Score;
	DrawScore();
}

static void DrawByte(u8 x, u8 y, u8 value)
{
	Print_SetPosition(x, y);
	Print_DrawChar((c8)('0' + (value / 10)));
	Print_DrawChar((c8)('0' + (value % 10)));
}

static void DrawVodka(void)
{
	u8 w = (u8)(g_Vodka * VODKA_BAR);
	VDP_CommandHMMV(160, 10, 64, 6, COLOR_MERGE2(12));
	if (w > 0)
		VDP_CommandHMMV(160, 10, w, 6, COLOR_MERGE2(11));
}

static void DrawFesta(void)
{
	Print_SetColor(14, 1);
	Print_DrawTextAt(8, 10, "BALAO");
	DrawByte(44, 10, g_Festa);
	Print_DrawTextAt(56, 10, "/");
	DrawByte(62, 10, FESTA_WIN);
}

static void DrawHud(void)
{
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(14, 1);
	DrawScore();
	DrawFesta();
	Print_DrawTextAt(84, 10, "VODKA");
	DrawVodka();
	Print_DrawTextAt(8, 200, "STOP:PAUSA  ESC:TITULO");
}

static void SetPlayPalette(void)
{
	VDP_SetPaletteEntry(0, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(1, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(2, RGB16(0, 0, 2));
	VDP_SetPaletteEntry(3, RGB16(1, 2, 5));
	VDP_SetPaletteEntry(4, RGB16(0, 2, 1));
	VDP_SetPaletteEntry(5, RGB16(1, 4, 2));
	VDP_SetPaletteEntry(6, RGB16(2, 1, 1));
	VDP_SetPaletteEntry(7, RGB16(5, 4, 1));
	VDP_SetPaletteEntry(8, RGB16(6, 1, 1));
	VDP_SetPaletteEntry(9, RGB16(1, 5, 6));
	VDP_SetPaletteEntry(10, RGB16(7, 6, 1));
	VDP_SetPaletteEntry(11, RGB16(7, 6, 2));
	VDP_SetPaletteEntry(12, RGB16(2, 2, 2));
	VDP_SetPaletteEntry(13, RGB16(6, 5, 3));
	VDP_SetPaletteEntry(14, RGB16(7, 7, 7));
	VDP_SetPaletteEntry(15, RGB16(7, 7, 7));
}

static void DrawTown(void)
{
	VDP_CommandHMMV(0, 0, 256, 212, COLOR_MERGE2(3));
	VDP_CommandHMMV(0, 0, 256, 16, COLOR_MERGE2(1));
	VDP_CommandHMMV(0, 148, 256, 24, COLOR_MERGE2(6));
	VDP_CommandHMMV(16, 132, 40, 40, COLOR_MERGE2(6));
	VDP_CommandHMMV(72, 124, 36, 48, COLOR_MERGE2(12));
	VDP_CommandHMMV(128, 136, 48, 36, COLOR_MERGE2(6));
	VDP_CommandHMMV(196, 128, 44, 44, COLOR_MERGE2(12));
	VDP_CommandHMMV(24, 140, 8, 10, COLOR_MERGE2(7));
	VDP_CommandHMMV(80, 136, 8, 10, COLOR_MERGE2(7));
	VDP_CommandHMMV(140, 144, 8, 10, COLOR_MERGE2(7));
	VDP_CommandHMMV(208, 140, 8, 10, COLOR_MERGE2(7));
	VDP_CommandHMMV(0, 172, 256, 40, COLOR_MERGE2(5));
	VDP_CommandHMMV(0, 180, 256, 32, COLOR_MERGE2(4));
	VDP_CommandWait();
}

static void DrawCloud(u8 i, u8 color)
{
	u8 x = g_Cloud[i].x;
	u8 y = g_Cloud[i].y;
	u8 w = g_Cloud[i].w;

	VDP_CommandHMMV((u8)(x + 8), y, 16, 6, COLOR_MERGE2(color));
	VDP_CommandHMMV(x, (u8)(y + 4), w, 8, COLOR_MERGE2(color));
	VDP_CommandHMMV((u8)(x + 4), (u8)(y + 10), (u8)(w - 8), 6, COLOR_MERGE2(color));
}

static u8 CloudCount(void)
{
	return (g_Round <= 1) ? 2 : 3;
}

static void InitClouds(void)
{
	u8 n;
	u8 i;

	if (g_Round <= 1)
	{
		g_Cloud[0].x = 32;
		g_Cloud[0].y = 44;
		g_Cloud[0].w = 28;
		g_Cloud[0].dir = 1;
		g_Cloud[1].x = 160;
		g_Cloud[1].y = 96;
		g_Cloud[1].w = 28;
		g_Cloud[1].dir = -1;
	}
	else
	{
		g_Cloud[0].x = 24;
		g_Cloud[0].y = 40;
		g_Cloud[0].w = 36;
		g_Cloud[0].dir = 1;
		g_Cloud[1].x = 120;
		g_Cloud[1].y = 72;
		g_Cloud[1].w = 32;
		g_Cloud[1].dir = -1;
		g_Cloud[2].x = 176;
		g_Cloud[2].y = 104;
		g_Cloud[2].w = 32;
		g_Cloud[2].dir = 1;
	}

	n = CloudCount();
	for (i = 0; i < n; ++i)
		DrawCloud(i, 15);
	VDP_CommandWait();
}

static void UpdateClouds(void)
{
	u8 i;
	u8 n;
	u8 maxx;
	u8 spd;

	if ((g_Round == 1) && ((g_Frame & 7) != 0))
		return;
	if ((g_Round == 2) && ((g_Frame & 3) != 0))
		return;
	if ((g_Round >= 3) && ((g_Frame & 1) != 0))
		return;

	spd = (g_Round <= 1) ? 1 : 2;
	n = CloudCount();
	for (i = 0; i < n; ++i)
	{
		DrawCloud(i, 3);
		maxx = (u8)(240 - g_Cloud[i].w);
		if (g_Cloud[i].dir > 0)
		{
			if (g_Cloud[i].x >= maxx)
				g_Cloud[i].x = 0;
			else
				g_Cloud[i].x += spd;
		}
		else
		{
			if (g_Cloud[i].x < 4)
				g_Cloud[i].x = maxx;
			else
				g_Cloud[i].x -= spd;
		}
		DrawCloud(i, 15);
	}
	VDP_CommandWait();
}

static bool HitCloud(u8 x, u8 y)
{
	u8 bx0 = (u8)(x + 4);
	u8 bx1 = (u8)(x + 11);
	u8 by0 = (u8)(y + 3);
	u8 by1 = (u8)(y + 8);
	u8 i;
	u8 n = CloudCount();

	for (i = 0; i < n; ++i)
	{
		u8 cx0 = (u8)(g_Cloud[i].x + 2);
		u8 cx1 = (u8)(g_Cloud[i].x + g_Cloud[i].w - 3);
		u8 cy0 = (u8)(g_Cloud[i].y + 2);
		u8 cy1 = (u8)(g_Cloud[i].y + CLOUD_H - 3);

		if ((bx1 < cx0) || (bx0 > cx1) || (by1 < cy0) || (by0 > cy1))
			continue;
		return TRUE;
	}
	return FALSE;
}

static void ResetBalloons(void)
{
	u8 i;
	for (i = 0; i < BALLOON_MAX; ++i)
	{
		g_Balloon[i].alive = FALSE;
		VDP_HideSprite((u8)(SPR_BALLOON + i));
	}
}

static void PlayTaxiIntro(void)
{
	u8 x;
	u8 wait;

	VDP_HideSprite(SPR_HECTOR);
	VDP_HideSprite(SPR_HELM);
	VDP_SetSpriteExUniColor(SPR_TAXI_LD, 0, TAXI_Y, PAT_TAXI_LD, 1);
	VDP_SetSpriteExUniColor(SPR_TAXI_L, 0, TAXI_Y, PAT_TAXI_L, 10);
	VDP_SetSpriteExUniColor(SPR_TAXI_LW, 0, TAXI_Y, PAT_TAXI_LW, 9);
	VDP_SetSpriteExUniColor(SPR_TAXI_RD, 16, TAXI_Y, PAT_TAXI_RD, 1);
	VDP_SetSpriteExUniColor(SPR_TAXI_R, 16, TAXI_Y, PAT_TAXI_R, 10);
	VDP_SetSpriteExUniColor(SPR_TAXI_RW, 16, TAXI_Y, PAT_TAXI_RW, 9);

	for (x = 0; x < TAXI_STOP; ++x)
	{
		PlaceTaxi(x);
		if ((x & 7) == 0)
			Sfx_Taxi();
		Tick();
	}

	PlaceTaxi(TAXI_STOP);
	for (wait = 0; wait < 18; ++wait)
		Tick();

	g_HectorX = (u8)(TAXI_STOP + 10);
	VDP_SetSpriteExUniColor(SPR_HELM, g_HectorX, HECTOR_Y, PAT_HELM, 1);
	VDP_SetSpriteExUniColor(SPR_HECTOR, g_HectorX, HECTOR_Y, PAT_HECTOR, 15);

	for (wait = 0; wait < 16; ++wait)
		Tick();

	for (x = TAXI_STOP; x < 224; x += 3)
	{
		PlaceTaxi(x);
		if ((x & 7) == 0)
			Sfx_Taxi();
		Tick();
	}
	HideTaxi();
}

static void ShowRoundCard(void)
{
	u8 i;

	Print_SetColor(14, 3);
	Print_DrawTextAt(92, 88, "ROUND");
	Print_SetPosition(128, 88);
	Print_DrawChar((c8)('0' + g_Round));
	for (i = 0; i < 50; ++i)
		Tick();
	VDP_CommandHMMV(80, 80, 80, 20, COLOR_MERGE2(3));
	VDP_CommandWait();
}

static void InitRound(bool taxi)
{
	u8 i;

	g_HectorX = TAXI_STOP + 10;
	g_Vodka = VODKA_MAX;
	g_Festa = 0;
	g_FireWait = 0;
	g_FireHeld = TRUE;
	g_Ended = FALSE;
	g_Won = FALSE;
	g_RetryReady = FALSE;
	g_Walking = FALSE;
	g_Frame = 0;
	g_Wind = 0;
	g_WindTime = 0;
	g_WindIdx = 0;
	Music_PlayGame();

	DrawTown();
	InitClouds();
	DrawHud();

	VDP_EnableSprite(TRUE);
	VDP_SetSpritePatternTable(0x17000);
	VDP_SetSpriteAttributeTable(0x17A00);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	VDP_LoadSpritePattern(g_PatViking, PAT_HECTOR, 4);
	VDP_LoadSpritePattern(g_PatViking2, PAT_HECTOR2, 4);
	VDP_LoadSpritePattern(g_PatHelm, PAT_HELM, 4);
	VDP_LoadSpritePattern(g_PatTaxiL, PAT_TAXI_L, 4);
	VDP_LoadSpritePattern(g_PatTaxiR, PAT_TAXI_R, 4);
	VDP_LoadSpritePattern(g_PatTaxiLD, PAT_TAXI_LD, 4);
	VDP_LoadSpritePattern(g_PatTaxiRD, PAT_TAXI_RD, 4);
	VDP_LoadSpritePattern(g_PatTaxiLW, PAT_TAXI_LW, 4);
	VDP_LoadSpritePattern(g_PatTaxiRW, PAT_TAXI_RW, 4);
	VDP_LoadSpritePattern(g_PatBalloon, PAT_BALLOON, 4);

	for (i = 0; i < BALLOON_MAX; ++i)
		VDP_SetSpriteExUniColor((u8)(SPR_BALLOON + i), 0, VDP_SPRITE_HIDE, PAT_BALLOON, g_BalloonColor[i]);
	ResetBalloons();
	VDP_DisableSpritesFrom(SPR_BALLOON + BALLOON_MAX);

	if (taxi)
		PlayTaxiIntro();
	else
	{
		HideTaxi();
		PlaceHector(g_HectorX, HECTOR_Y);
	}
	if (!g_Attract)
		ShowRoundCard();
}

static bool AnyBalloon(void)
{
	u8 i;
	for (i = 0; i < BALLOON_MAX; ++i)
	{
		if (g_Balloon[i].alive)
			return TRUE;
	}
	return FALSE;
}

static void LaunchBalloon(void)
{
	u8 i;

	if ((g_Vodka == 0) || (g_FireWait != 0))
		return;

	for (i = 0; i < BALLOON_MAX; ++i)
	{
		if (!g_Balloon[i].alive)
		{
			g_Balloon[i].alive = TRUE;
			g_Balloon[i].x = g_HectorX;
			g_Balloon[i].y = (u8)(HECTOR_Y - 12);
			g_Balloon[i].color = g_BalloonColor[i];
			g_Vodka--;
			g_FireWait = FIRE_WAIT;
			VDP_SetSpriteExUniColor((u8)(SPR_BALLOON + i), g_Balloon[i].x, g_Balloon[i].y, PAT_BALLOON, g_Balloon[i].color);
			DrawVodka();
			Sfx_Pop();
			return;
		}
	}
}

static void EndRound(bool won)
{
	g_Ended = TRUE;
	g_Won = won;
	g_RetryReady = FALSE;
	Print_SetColor(14, 3);
	if (won)
	{
		AddScore((u16)g_Vodka * SCORE_VODKA);
		Music_PlayWin();
		if (g_Round >= ROUND_MAX)
			Print_DrawTextAt(52, 48, "FESTA NA VILA ZELINA!");
		else
			Print_DrawTextAt(80, 48, "ROUND CLEAR");
		Print_DrawTextAt(68, 64, "BONUS");
		DrawNum6(104, 64, (u16)g_Vodka * SCORE_VODKA);
	}
	else
	{
		Music_PlayLose();
		Print_DrawTextAt(80, 48, "GAME OVER");
		Print_DrawTextAt(64, 64, "A GARRAFA ACABOU");
	}
}

static void WaitKeyUp(void)
{
	while (Keyboard_IsKeyPressed(KEY_SPACE) || Keyboard_IsKeyPressed(KEY_ESC)
		|| Keyboard_IsKeyPressed(KEY_SELECT) || Keyboard_IsKeyPressed(KEY_STOP)
		|| Keyboard_IsKeyPressed(KEY_F1) || JOY_GET_TRIG1(Joystick_Read(JOY_PORT_1)))
		Tick();
}

static void DoPause(void)
{
	Music_Pause();
	Print_SetColor(14, 3);
	Print_DrawTextAt(108, 88, "PAUSE");
	WaitKeyUp();
	while (!Keyboard_IsKeyPressed(KEY_STOP) && !Keyboard_IsKeyPressed(KEY_F1)
		&& !Keyboard_IsKeyPressed(KEY_SPACE))
		Tick();
	WaitKeyUp();
	Music_Resume();
	VDP_CommandHMMV(80, 80, 96, 20, COLOR_MERGE2(3));
	VDP_CommandWait();
}

static void CheckEnd(void)
{
	if (g_Ended)
		return;
	if (g_Festa >= FESTA_WIN)
		EndRound(TRUE);
	else if ((g_Vodka == 0) && !AnyBalloon())
		EndRound(FALSE);
}

static void BurstBalloon(u8 i)
{
	g_Balloon[i].alive = FALSE;
	VDP_HideSprite((u8)(SPR_BALLOON + i));
	Sfx_Cloud();
}

static void TickWind(void)
{
	u8 per;

	if (g_Round <= 1)
	{
		g_Wind = 0;
		return;
	}

	per = (g_Round == 2) ? 96 : 64;
	if (++g_WindTime < per)
		return;
	g_WindTime = 0;
	g_WindIdx = (u8)((g_WindIdx + 1) & 3);
	g_Wind = g_WindCycle[g_WindIdx];
}

static void UpdateBalloons(void)
{
	u8 i;

	TickWind();

	for (i = 0; i < BALLOON_MAX; ++i)
	{
		if (!g_Balloon[i].alive)
			continue;

		if ((g_Frame & 3) == 0)
		{
			i8 w = g_Wind;
			while (w < 0)
			{
				if (g_Balloon[i].x <= BALLOON_X_MIN)
				{
					g_Balloon[i].x = BALLOON_X_MIN;
					break;
				}
				g_Balloon[i].x--;
				w++;
			}
			while (w > 0)
			{
				if (g_Balloon[i].x >= BALLOON_X_MAX)
				{
					g_Balloon[i].x = BALLOON_X_MAX;
					break;
				}
				g_Balloon[i].x++;
				w--;
			}
		}

		g_Balloon[i].y--;

		if (HitCloud(g_Balloon[i].x, g_Balloon[i].y))
		{
			BurstBalloon(i);
			continue;
		}

		if (g_Balloon[i].y <= SKY_TOP)
		{
			g_Balloon[i].alive = FALSE;
			VDP_HideSprite((u8)(SPR_BALLOON + i));
			if (g_Festa < 99)
				g_Festa++;
			DrawFesta();
			AddScore(SCORE_BALLOON);
			Sfx_Score();
			continue;
		}

		VDP_SetSpritePosition((u8)(SPR_BALLOON + i), g_Balloon[i].x, g_Balloon[i].y);
	}
}

static void ReadInput(void)
{
	u8 joy = Joystick_Read(JOY_PORT_1);
	bool left = Keyboard_IsKeyPressed(KEY_LEFT) || IS_JOY_PRESSED(joy, JOY_INPUT_DIR_LEFT);
	bool right = Keyboard_IsKeyPressed(KEY_RIGHT) || IS_JOY_PRESSED(joy, JOY_INPUT_DIR_RIGHT);
	bool fire = Keyboard_IsKeyPressed(KEY_SPACE) || JOY_GET_TRIG1(joy);

	if (left && (g_HectorX > HECTOR_MIN))
		g_HectorX -= 2;
	if (right && (g_HectorX < HECTOR_MAX))
		g_HectorX += 2;
	g_Walking = (left || right) ? TRUE : FALSE;

	if (fire && !g_FireHeld)
		LaunchBalloon();
	g_FireHeld = fire;

	PlaceHector(g_HectorX, HECTOR_Y);
}

void Game_Play(void)
{
	BIOS_SetKeyClick(FALSE);
	SetPlayPalette();
	g_Attract = FALSE;
	g_Round = 1;
	g_Score = 0;
	while (Keyboard_IsKeyPressed(KEY_SPACE) || JOY_GET_TRIG1(Joystick_Read(JOY_PORT_1)))
		Tick();
	InitRound(TRUE);

	while (1)
	{
		Tick();
		g_Frame++;

		if (g_Ended)
		{
			if (!g_RetryReady)
			{
				if (!Music_IsPlaying())
				{
					if (g_Won && (g_Round < ROUND_MAX))
					{
						g_Round++;
						InitRound(FALSE);
						continue;
					}
					g_RetryReady = TRUE;
					Print_SetColor(14, 3);
					Print_DrawTextAt(56, 88, "ESPACO: DE NOVO");
					Print_DrawTextAt(68, 104, "ESC: TITULO");
					WaitKeyUp();
				}
				continue;
			}
			if (Keyboard_IsKeyPressed(KEY_SPACE) || JOY_GET_TRIG1(Joystick_Read(JOY_PORT_1)))
			{
				WaitKeyUp();
				g_Round = 1;
				g_Score = 0;
				InitRound(TRUE);
				continue;
			}
			if (Keyboard_IsKeyPressed(KEY_ESC) || Keyboard_IsKeyPressed(KEY_SELECT))
			{
				WaitKeyUp();
				VDP_EnableSprite(FALSE);
				Music_Stop();
				return;
			}
			continue;
		}

		if (Keyboard_IsKeyPressed(KEY_STOP) || Keyboard_IsKeyPressed(KEY_F1))
			DoPause();
		if (Keyboard_IsKeyPressed(KEY_ESC) || Keyboard_IsKeyPressed(KEY_SELECT))
		{
			WaitKeyUp();
			VDP_EnableSprite(FALSE);
			Music_Stop();
			return;
		}

		if (g_FireWait != 0)
			g_FireWait--;

		ReadInput();
		UpdateClouds();
		UpdateBalloons();
		CheckEnd();
	}
}

void Game_Attract(void)
{
	u16 t;
	u16 keepScore = g_Score;
	u16 keepHi = g_HiScore;

	BIOS_SetKeyClick(FALSE);
	SetPlayPalette();
	g_Attract = TRUE;
	g_Round = 1;
	InitRound(TRUE);
	for (t = 0; t < 400; ++t)
	{
		Tick();
		g_Frame++;
		if (Keyboard_IsKeyPressed(KEY_SPACE) || Keyboard_IsKeyPressed(KEY_ESC))
			break;
		UpdateClouds();
		UpdateBalloons();
		if (t == 40)
			LaunchBalloon();
	}
	g_Score = keepScore;
	g_HiScore = keepHi;
	g_Attract = FALSE;
	VDP_EnableSprite(FALSE);
	Music_Stop();
	WaitKeyUp();
}

u16 Game_GetScore(void)
{
	return g_Score;
}

u16 Game_GetHiScore(void)
{
	return g_HiScore;
}

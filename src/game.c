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
#define SPR_BALOON_OL	12
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
#define PAT_HECTOR3		40
#define PAT_BALLOON2	44
#define PAT_BALLOON_OL	48
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

/* Walk frame 2: recover stride. */
static const u8 g_PatViking3[32] =
{
	0x0F, 0x1F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x1F,
	0x3F, 0x7F, 0x67, 0x3F, 0x0C, 0x06, 0x06, 0x0E,
	0xF0, 0xF8, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xF8,
	0xFC, 0xFE, 0xE6, 0xFC, 0x30, 0x60, 0x60, 0x70
};

/* Black details in front: helmet, visor slots, mouth, belt, boots. */
static const u8 g_PatHelm[32] =
{
	0x0F, 0x1F, 0x33, 0x00, 0x06, 0x00, 0x00, 0x00,
	0x00, 0x1E, 0x00, 0x0E, 0x00, 0x00, 0x06, 0x0E,
	0xF0, 0xF8, 0xCC, 0x00, 0x60, 0x00, 0x00, 0x00,
	0x00, 0x78, 0x00, 0x70, 0x00, 0x00, 0x60, 0x70
};

/* 32x16 sedan: roof, 2 windows, checker, 2 wheel wells. Facing right. */
static const u8 g_PatTaxiL[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x03, 0x07, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0x3F, 0x7F, 0x61, 0x61, 0x61, 0x7F,
	0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x00
};

static const u8 g_PatTaxiR[32] =
{
	0x18, 0xFF, 0xFF, 0xFF, 0x83, 0x83, 0x83, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0x00,
	0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
	0xF0, 0xF0, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0x00
};

static const u8 g_PatTaxiLD[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x55, 0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x3C
};

static const u8 g_PatTaxiRD[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x55, 0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00
};

static const u8 g_PatTaxiLW[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 g_PatTaxiRW[32] =
{
	0x00, 0x00, 0x00, 0x00, 0x7C, 0x7C, 0x7C, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u8 g_PatBalloon[32] =
{
	0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
	0x3F, 0x1F, 0x0F, 0x07, 0x02, 0x02, 0x02, 0x01,
	0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE,
	0xFC, 0xF8, 0xF0, 0xE0, 0x40, 0x40, 0x40, 0x80
};

/* Bob frame: diamond 1px lower. */
static const u8 g_PatBalloon2[32] =
{
	0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F,
	0x7F, 0x3F, 0x1F, 0x0F, 0x02, 0x02, 0x02, 0x01,
	0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC,
	0xFE, 0xFC, 0xF8, 0xF0, 0x40, 0x40, 0x40, 0x80
};

/* Black outline behind color balloon. */
static const u8 g_PatBalloonOL[32] =
{
	0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF,
	0x7F, 0x3F, 0x1F, 0x0F, 0x06, 0x06, 0x07, 0x03,
	0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF,
	0xFE, 0xFC, 0xF8, 0xF0, 0x60, 0x60, 0xE0, 0xC0
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
	u8 pat = PAT_HECTOR;
	if (g_Walking)
	{
		u8 f = (u8)((g_Frame >> 3) & 3);
		if (f == 1)
			pat = PAT_HECTOR2;
		else if (f == 3)
			pat = PAT_HECTOR3;
	}
	VDP_SetSpriteExUniColor(SPR_HELM, x, y, PAT_HELM, 1);
	VDP_SetSpriteExUniColor(SPR_HECTOR, x, y, pat, 15);
}

static void PlaceBalloon(u8 i)
{
	u8 pat = ((g_Balloon[i].y + g_Frame) & 8) ? PAT_BALLOON2 : PAT_BALLOON;
	VDP_SetSpriteExUniColor((u8)(SPR_BALOON_OL + i), g_Balloon[i].x, g_Balloon[i].y, PAT_BALLOON_OL, 1);
	VDP_SetSpriteExUniColor((u8)(SPR_BALLOON + i), g_Balloon[i].x, g_Balloon[i].y, pat, g_Balloon[i].color);
}

static void HideBalloon(u8 i)
{
	VDP_HideSprite((u8)(SPR_BALLOON + i));
	VDP_HideSprite((u8)(SPR_BALOON_OL + i));
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
	Print_DrawTextAt(4, 4, "SCORE");
	DrawNum6(36, 4, g_Score);
	Print_DrawTextAt(80, 4, "HI");
	DrawNum6(96, 4, g_HiScore);
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

	VDP_CommandHMMV(171, 1, 2, 2, COLOR_MERGE2(6));
	VDP_CommandHMMV(171, 3, 2, 3, COLOR_MERGE2(9));
	VDP_CommandHMMV(170, 6, 4, 2, COLOR_MERGE2(9));
	VDP_CommandHMMV(169, 8, 6, 7, COLOR_MERGE2(11));
	VDP_CommandHMMV(170, 10, 4, 2, COLOR_MERGE2(14));
	VDP_CommandHMMV(178, 4, 74, 8, COLOR_MERGE2(14));
	VDP_CommandHMMV(179, 5, 72, 6, COLOR_MERGE2(1));
	VDP_CommandHMMV(180, 6, 70, 4, COLOR_MERGE2(12));
	if (w > 0)
	{
		if (w > 70)
			w = 70;
		VDP_CommandHMMV(180, 6, w, 4, COLOR_MERGE2(11));
	}
}

static void DrawFesta(void)
{
	Print_SetColor(14, 1);
	DrawByte(148, 4, g_Festa);
}

static void DrawHud(void)
{
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(14, 1);
	DrawScore();
	VDP_CommandHMMV(138, 4, 2, 2, COLOR_MERGE2(8));
	VDP_CommandHMMV(136, 6, 6, 4, COLOR_MERGE2(8));
	VDP_CommandHMMV(138, 10, 2, 3, COLOR_MERGE2(14));
	DrawFesta();
	DrawVodka();
}

static void SetPlayPalette(void)
{
	VDP_SetPaletteEntry(0, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(1, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(2, RGB16(0, 0, 2));
	VDP_SetPaletteEntry(3, RGB16(1, 2, 5));
	VDP_SetPaletteEntry(4, RGB16(0, 2, 1));
	VDP_SetPaletteEntry(5, RGB16(1, 4, 2));
	VDP_SetPaletteEntry(6, RGB16(3, 1, 1));
	VDP_SetPaletteEntry(7, RGB16(5, 4, 1));
	VDP_SetPaletteEntry(8, RGB16(6, 1, 1));
	VDP_SetPaletteEntry(9, RGB16(1, 5, 6));
	VDP_SetPaletteEntry(10, RGB16(7, 6, 1));
	VDP_SetPaletteEntry(11, RGB16(7, 6, 2));
	VDP_SetPaletteEntry(12, RGB16(3, 2, 2));
	VDP_SetPaletteEntry(13, RGB16(6, 5, 3));
	VDP_SetPaletteEntry(14, RGB16(7, 7, 7));
	VDP_SetPaletteEntry(15, RGB16(7, 7, 7));
}

static void DrawWin(u8 x0, u8 y0, u8 cols, u8 rows, u8 ink)
{
	u8 c, r;
	for (r = 0; r < rows; ++r)
		for (c = 0; c < cols; ++c)
			VDP_CommandHMMV((u8)(x0 + (u8)(c * 6)), (u8)(y0 + (u8)(r * 6)), 2, 2, COLOR_MERGE2(ink));
}

static void DrawTown(void)
{
	VDP_CommandHMMV(0, 0, 256, 212, COLOR_MERGE2(3));
	VDP_CommandHMMV(0, 0, 256, 16, COLOR_MERGE2(1));
	VDP_CommandHMMV(220, 24, 10, 10, COLOR_MERGE2(15));
	VDP_CommandHMMV(222, 26, 6, 6, COLOR_MERGE2(13));

	/* casas baixas (esq.) — janela 7 pisca */
	VDP_CommandHMMV(2, 148, 36, 24, COLOR_MERGE2(6));
	VDP_CommandHMMV(2, 144, 36, 4, COLOR_MERGE2(8));
	DrawWin(6, 152, 4, 2, 7);
	VDP_CommandHMMV(40, 154, 28, 18, COLOR_MERGE2(6));
	VDP_CommandHMMV(40, 150, 28, 4, COLOR_MERGE2(8));
	DrawWin(44, 156, 3, 2, 7);

	/* luminária */
	VDP_CommandHMMV(72, 156, 2, 16, COLOR_MERGE2(1));
	VDP_CommandHMMV(68, 154, 10, 3, COLOR_MERGE2(10));

	/* monumento (pedestal + figura) */
	VDP_CommandHMMV(82, 148, 8, 24, COLOR_MERGE2(12));
	VDP_CommandHMMV(84, 142, 4, 6, COLOR_MERGE2(14));

	/* bandeira LTU (amarelo / verde / vermelho) */
	VDP_CommandHMMV(94, 132, 2, 20, COLOR_MERGE2(1));
	VDP_CommandHMMV(96, 132, 10, 3, COLOR_MERGE2(10));
	VDP_CommandHMMV(96, 135, 10, 3, COLOR_MERGE2(5));
	VDP_CommandHMMV(96, 138, 10, 3, COLOR_MERGE2(8));

	/* Igreja São José: torre, frontão, nave, portal; janela 10 fixa */
	VDP_CommandHMMV(120, 124, 16, 48, COLOR_MERGE2(13));
	VDP_CommandHMMV(122, 120, 12, 4, COLOR_MERGE2(13));
	VDP_CommandHMMV(126, 126, 4, 4, COLOR_MERGE2(1));
	VDP_CommandHMMV(100, 140, 56, 32, COLOR_MERGE2(13));
	VDP_CommandHMMV(110, 134, 36, 6, COLOR_MERGE2(13));
	VDP_CommandHMMV(120, 130, 16, 4, COLOR_MERGE2(13));
	VDP_CommandHMMV(122, 132, 4, 8, COLOR_MERGE2(10));
	VDP_CommandHMMV(106, 146, 4, 6, COLOR_MERGE2(10));
	VDP_CommandHMMV(146, 146, 4, 6, COLOR_MERGE2(10));
	VDP_CommandHMMV(134, 156, 10, 16, COLOR_MERGE2(1));

	/* comércio VITO + toldo */
	VDP_CommandHMMV(164, 152, 40, 20, COLOR_MERGE2(12));
	VDP_CommandHMMV(164, 148, 40, 4, COLOR_MERGE2(10));
	VDP_CommandHMMV(168, 140, 32, 8, COLOR_MERGE2(10));
	VDP_CommandHMMV(178, 160, 8, 12, COLOR_MERGE2(1));

	/* casas (dir.) */
	VDP_CommandHMMV(208, 150, 46, 22, COLOR_MERGE2(6));
	VDP_CommandHMMV(208, 146, 46, 4, COLOR_MERGE2(8));
	DrawWin(212, 154, 5, 2, 7);

	VDP_CommandHMMV(0, 172, 256, 8, COLOR_MERGE2(12));
	VDP_CommandHMMV(0, 180, 256, 32, COLOR_MERGE2(4));
	VDP_CommandWait();

	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(1, 10);
	Print_DrawTextAt(172, 141, "VITO");
}

static void EraseCloud(u8 i)
{
	VDP_CommandHMMV(g_Cloud[i].x, g_Cloud[i].y, g_Cloud[i].w, CLOUD_H, COLOR_MERGE2(3));
}

static void DrawCloud(u8 i)
{
	u8 x = g_Cloud[i].x;
	u8 y = g_Cloud[i].y;
	u8 w = g_Cloud[i].w;
	u8 hx = (u8)(x + 8 + ((g_Frame & 16) ? 2 : 0));

	VDP_CommandHMMV((u8)(x + 8), y, 16, 6, COLOR_MERGE2(14));
	VDP_CommandHMMV(x, (u8)(y + 4), w, 8, COLOR_MERGE2(14));
	VDP_CommandHMMV((u8)(x + 4), (u8)(y + 10), (u8)(w - 8), 6, COLOR_MERGE2(14));
	VDP_CommandHMMV(hx, (u8)(y + 1), 8, 2, COLOR_MERGE2(15));
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
	else if (g_Round == 2)
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
	else
	{
		g_Cloud[0].x = 16;
		g_Cloud[0].y = 36;
		g_Cloud[0].w = 40;
		g_Cloud[0].dir = 1;
		g_Cloud[1].x = 100;
		g_Cloud[1].y = 62;
		g_Cloud[1].w = 38;
		g_Cloud[1].dir = -1;
		g_Cloud[2].x = 168;
		g_Cloud[2].y = 90;
		g_Cloud[2].w = 40;
		g_Cloud[2].dir = 1;
	}

	n = CloudCount();
	for (i = 0; i < n; ++i)
		DrawCloud(i);
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

	spd = (g_Round <= 1) ? 1 : 2;
	n = CloudCount();
	for (i = 0; i < n; ++i)
	{
		EraseCloud(i);
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
		DrawCloud(i);
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
		HideBalloon(i);
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
		Tick();
	}
	HideTaxi();
}

static void ShowRoundCard(void)
{
	u8 i;

	VDP_EnableSprite(FALSE);
	VDP_CommandHMMV(0, 0, 256, 212, COLOR_MERGE2(1));
	VDP_CommandWait();
	Print_SetBitmapFont(g_Font_MGL_Sample6);
	Print_SetColor(14, 1);
	Print_DrawTextAt(92, 96, "ROUND");
	Print_SetPosition(128, 96);
	Print_DrawChar((c8)('0' + g_Round));
	for (i = 0; i < 50; ++i)
		Tick();
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

	VDP_SetSpritePatternTable(0x17000);
	VDP_SetSpriteAttributeTable(0x17A00);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	VDP_LoadSpritePattern(g_PatViking, PAT_HECTOR, 4);
	VDP_LoadSpritePattern(g_PatViking2, PAT_HECTOR2, 4);
	VDP_LoadSpritePattern(g_PatViking3, PAT_HECTOR3, 4);
	VDP_LoadSpritePattern(g_PatHelm, PAT_HELM, 4);
	VDP_LoadSpritePattern(g_PatTaxiL, PAT_TAXI_L, 4);
	VDP_LoadSpritePattern(g_PatTaxiR, PAT_TAXI_R, 4);
	VDP_LoadSpritePattern(g_PatTaxiLD, PAT_TAXI_LD, 4);
	VDP_LoadSpritePattern(g_PatTaxiRD, PAT_TAXI_RD, 4);
	VDP_LoadSpritePattern(g_PatTaxiLW, PAT_TAXI_LW, 4);
	VDP_LoadSpritePattern(g_PatTaxiRW, PAT_TAXI_RW, 4);
	VDP_LoadSpritePattern(g_PatBalloon, PAT_BALLOON, 4);
	VDP_LoadSpritePattern(g_PatBalloon2, PAT_BALLOON2, 4);
	VDP_LoadSpritePattern(g_PatBalloonOL, PAT_BALLOON_OL, 4);

	for (i = 0; i < BALLOON_MAX; ++i)
	{
		VDP_SetSpriteExUniColor((u8)(SPR_BALLOON + i), 0, VDP_SPRITE_HIDE, PAT_BALLOON, g_BalloonColor[i]);
		VDP_SetSpriteExUniColor((u8)(SPR_BALOON_OL + i), 0, VDP_SPRITE_HIDE, PAT_BALLOON_OL, 1);
	}
	ResetBalloons();
	VDP_DisableSpritesFrom(SPR_BALOON_OL + BALLOON_MAX);

	if (!g_Attract)
		ShowRoundCard();

	DrawTown();
	InitClouds();
	DrawHud();
	VDP_EnableSprite(TRUE);

	if (taxi)
		PlayTaxiIntro();
	else
	{
		HideTaxi();
		PlaceHector(g_HectorX, HECTOR_Y);
	}
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
			g_FireWait = (g_Round >= 3) ? 18 : FIRE_WAIT;
			PlaceBalloon(i);
			DrawVodka();
			Sfx_Pop();
			return;
		}
	}
}

static void FillEndBanner(void)
{
	VDP_CommandHMMV(32, 40, 192, 80, COLOR_MERGE2(1));
	VDP_CommandWait();
	Print_SetBitmapFont(g_Font_MGL_Sample6);
}

static void EndRound(bool won)
{
	g_Ended = TRUE;
	g_Won = won;
	g_RetryReady = FALSE;
	FillEndBanner();
	if (won)
	{
		AddScore((u16)g_Vodka * SCORE_VODKA);
		Music_PlayWin();
		Print_SetColor(10, 1);
		if (g_Round >= ROUND_MAX)
		{
			Print_DrawTextAt(68, 52, "FESTA");
			Print_DrawTextAt(104, 52, "NA");
			Print_DrawTextAt(122, 52, "VILA");
			Print_DrawTextAt(152, 52, "ZELINA");
		}
		else
		{
			Print_DrawTextAt(95, 52, "ROUND");
			Print_DrawTextAt(131, 52, "CLEAR");
		}
		Print_SetColor(14, 1);
		Print_DrawTextAt(92, 68, "BONUS");
		DrawNum6(128, 68, (u16)g_Vodka * SCORE_VODKA);
	}
	else
	{
		Music_PlayLose();
		Print_SetColor(10, 1);
		Print_DrawTextAt(101, 52, "GAME");
		Print_DrawTextAt(131, 52, "OVER");
		Print_SetColor(14, 1);
		Print_DrawTextAt(80, 68, "A");
		Print_DrawTextAt(92, 68, "GARRAFA");
		Print_DrawTextAt(140, 68, "ACABOU");
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
	VDP_CommandHMMV(88, 80, 80, 20, COLOR_MERGE2(1));
	VDP_CommandWait();
	Print_SetColor(10, 1);
	Print_DrawTextAt(113, 86, "PAUSE");
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
	HideBalloon(i);
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

	per = (g_Round == 2) ? 96 : 40;
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

		if ((g_Round >= 3) ? ((g_Frame & 1) == 0) : ((g_Frame & 3) == 0))
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
			HideBalloon(i);
			if (g_Festa < 99)
				g_Festa++;
			DrawFesta();
			AddScore(SCORE_BALLOON);
			Sfx_Score();
			continue;
		}

		PlaceBalloon(i);
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

static void BlinkWindows(void)
{
	if ((g_Frame & 32) == 0)
		VDP_SetPaletteEntry(7, RGB16(5, 4, 1));
	else
		VDP_SetPaletteEntry(7, RGB16(2, 1, 0));
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
		BlinkWindows();

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
					Print_SetColor(14, 1);
					Print_DrawTextAt(86, 88, "ESPACO");
					Print_DrawTextAt(128, 88, "DE");
					Print_DrawTextAt(146, 88, "NOVO");
					Print_DrawTextAt(101, 104, "ESC");
					Print_DrawTextAt(125, 104, "TITULO");
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
		BlinkWindows();
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

/*
 * Zelina Games opening music — PSG homage to 1980s Konami MSX adventure.
 * PAL: one pattern row every MUSIC_STEP_FRAMES vblanks (~125 BPM).
 */

#include "msxgl.h"
#include "psg.h"
#include "include/music.h"
#include "generated/zelina_music.h"

#define MUSIC_MODE_OPENING	0
#define MUSIC_MODE_GAME		1
#define MUSIC_MODE_WIN		2
#define MUSIC_MODE_LOSE		3
#define MUSIC_OPEN_STEP		8
#define MUSIC_GAME_STEP		8
#define MUSIC_WIN_STEP		10
#define MUSIC_LOSE_STEP		14
#define MUSIC_WIN_LEN		12
#define MUSIC_LOSE_LEN		12
#define MUSIC_STAGE_LEN		32

/* In-game: 32-step ostinato (lead + arp + plucked bass). */
static const u8 g_MusicStage[][3] =
{
	{ 62, 50, 38 },
	{ 65, 53, 38 },
	{ 69, 57, 33 },
	{  0, 53, 33 },
	{ 74, 50, 38 },
	{  0, 53, 38 },
	{ 69, 57, 33 },
	{ 65, 53, 33 },
	{ 67, 48, 36 },
	{ 69, 52, 36 },
	{ 70, 55, 31 },
	{ 69, 52, 31 },
	{ 67, 48, 36 },
	{ 65, 52, 36 },
	{ 64, 55, 31 },
	{ 62, 52, 31 },
	{ 65, 46, 34 },
	{  0, 50, 34 },
	{ 69, 53, 29 },
	{  0, 50, 29 },
	{ 72, 45, 33 },
	{  0, 48, 33 },
	{ 69, 52, 28 },
	{  0, 48, 28 },
	{ 74, 50, 38 },
	{ 69, 53, 38 },
	{ 65, 57, 33 },
	{ 62, 53, 33 },
	{ 69, 50, 38 },
	{ 65, 57, 33 },
	{ 62, 50, 38 },
	{  0,  0,  0 }
};

/* Stage clear — short rising fanfare. */
static const u8 g_MusicWin[][3] =
{
	{ 62, 50, 38 },
	{ 65, 53, 38 },
	{ 69, 57, 33 },
	{ 74, 62, 38 },
	{ 72, 60, 36 },
	{ 74, 62, 38 },
	{ 69, 57, 33 },
	{ 74, 62, 38 },
	{ 74, 69, 50 },
	{ 74, 62, 38 },
	{ 74, 62, 38 },
	{  0,  0,  0 }
};

/* Game over — descending, last notes fade. */
static const u8 g_MusicLose[][3] =
{
	{ 69, 53, 38 },
	{ 67, 52, 36 },
	{ 65, 50, 34 },
	{ 62, 48, 33 },
	{ 60, 45, 31 },
	{ 57, 41, 29 },
	{ 53, 38, 29 },
	{ 50, 33, 28 },
	{ 45, 33, 28 },
	{ 41, 29, 28 },
	{  0, 29, 28 },
	{  0,  0,  0 }
};

static u8 g_MusicFrame;
static u8 g_MusicStep;
static bool g_MusicLooping;
static bool g_MusicPlaying;
static u8 g_MusicMode;
static u8 g_LastNote[3];
static u8 g_ChanVol[3];
static u8 g_SfxLeft;
static u8 g_SfxKind;
static u8 g_SfxStep;

#define SFX_KIND_NONE	0
#define SFX_KIND_SCORE	1
#define SFX_KIND_CLOUD	2
#define SFX_KIND_POP	3
#define SFX_KIND_CLICK	4

static u16 PeriodOf(u8 note)
{
	if ((note < MUSIC_NOTE_MIN) || (note > MUSIC_NOTE_MAX))
		return 0;
	return g_MusicPeriod[note - MUSIC_NOTE_MIN];
}

static const u8* CurrentRow(void)
{
	if (g_MusicMode == MUSIC_MODE_GAME)
		return g_MusicStage[g_MusicStep];
	if (g_MusicMode == MUSIC_MODE_WIN)
		return g_MusicWin[g_MusicStep];
	if (g_MusicMode == MUSIC_MODE_LOSE)
		return g_MusicLose[g_MusicStep];
	if (g_MusicLooping)
		return g_MusicLoop[g_MusicStep];
	return g_MusicFanfare[g_MusicStep];
}

static void ApplyRow(const u8* row);

static u8 MusicStepFrames(void)
{
	if (g_MusicMode == MUSIC_MODE_GAME)
		return MUSIC_GAME_STEP;
	if (g_MusicMode == MUSIC_MODE_WIN)
		return MUSIC_WIN_STEP;
	if (g_MusicMode == MUSIC_MODE_LOSE)
		return MUSIC_LOSE_STEP;
	return MUSIC_OPEN_STEP;
}

static u8 MusicPatternLen(void)
{
	if (g_MusicMode == MUSIC_MODE_GAME)
		return MUSIC_STAGE_LEN;
	if (g_MusicMode == MUSIC_MODE_WIN)
		return MUSIC_WIN_LEN;
	if (g_MusicMode == MUSIC_MODE_LOSE)
		return MUSIC_LOSE_LEN;
	if (g_MusicLooping)
		return MUSIC_LOOP_LEN;
	return MUSIC_FANFARE_LEN;
}

static void MusicStartMode(u8 mode, bool loop)
{
	g_MusicMode = mode;
	g_MusicFrame = 0;
	g_MusicStep = 0;
	g_MusicLooping = loop;
	g_MusicPlaying = TRUE;
	g_SfxLeft = 0;
	g_SfxKind = SFX_KIND_NONE;
	g_SfxStep = 0;
	g_LastNote[0] = g_LastNote[1] = g_LastNote[2] = 0xFF;
	PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
	ApplyRow(CurrentRow());
	PSG_Apply();
}

static void ApplyRow(const u8* row)
{
	u8 ch;

	for (ch = 0; ch < 3; ++ch)
	{
		u8 note = row[ch];
		u16 period = PeriodOf(note);

		if (note == 0)
		{
			g_ChanVol[ch] = 0;
			g_LastNote[ch] = 0;
			PSG_SetVolume(ch, 0);
			continue;
		}

		if (note != g_LastNote[ch])
		{
			g_LastNote[ch] = note;
			PSG_SetTone(ch, period);
		}
		else if (g_MusicMode == MUSIC_MODE_OPENING)
			continue;

		if (g_MusicMode == MUSIC_MODE_GAME)
			g_ChanVol[ch] = (ch == 0) ? 12 : ((ch == 1) ? 7 : 11);
		else if (g_MusicMode == MUSIC_MODE_OPENING)
			g_ChanVol[ch] = (ch == 0) ? 13 : ((ch == 1) ? 7 : 11);
		else
			g_ChanVol[ch] = (ch == 0) ? 13 : ((ch == 1) ? 10 : 12);
		PSG_SetVolume(ch, g_ChanVol[ch]);
	}
}

static void SfxApplyStep(void)
{
	u8 i;

	if (g_SfxKind == SFX_KIND_SCORE)
	{
		static const u16 per[] = { 226, 190, 160 };
		i = g_SfxStep / 3;
		if (i > 2)
			i = 2;
		PSG_SetTone(PSG_CHANNEL_A, per[i]);
		PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
		PSG_SetVolume(PSG_CHANNEL_A, (u8)(13 - i));
		return;
	}

	if (g_SfxKind == SFX_KIND_CLOUD)
	{
		i = g_SfxStep;
		if (i > 6)
			i = 6;
		PSG_SetNoise((u8)(0x08 + i));
		PSG_SetMixer(PSG_TONE_B_ON | PSG_TONE_C_ON | PSG_NOISE_A_ON);
		PSG_SetVolume(PSG_CHANNEL_A, (u8)(13 - i));
		return;
	}

	if (g_SfxKind == SFX_KIND_POP)
	{
		static const u16 per[] = { 127, 160, 190, 226 };
		i = g_SfxStep / 2;
		if (i > 3)
			i = 3;
		PSG_SetTone(PSG_CHANNEL_A, per[i]);
		PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
		PSG_SetVolume(PSG_CHANNEL_A, (u8)(12 - i));
	}
}

static void SfxStart(u8 kind, u8 len)
{
	g_SfxKind = kind;
	g_SfxStep = 0;
	g_SfxLeft = len;
	SfxApplyStep();
	PSG_Apply();
}

void Music_Init(void)
{
	g_MusicFrame = 0;
	g_MusicStep = 0;
	g_MusicLooping = FALSE;
	g_MusicPlaying = TRUE;
	g_MusicMode = MUSIC_MODE_OPENING;
	g_SfxLeft = 0;
	g_SfxKind = SFX_KIND_NONE;
	g_SfxStep = 0;
	g_LastNote[0] = g_LastNote[1] = g_LastNote[2] = 0xFF;
	PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
	ApplyRow(CurrentRow());
	PSG_Apply();
}

void Music_Tick(void)
{
	u8 ch;

	if (!g_MusicPlaying)
		return;

	g_MusicFrame++;
	if (g_MusicFrame >= MusicStepFrames())
	{
		u8 len;
		g_MusicFrame = 0;
		g_MusicStep++;
		len = MusicPatternLen();

		if (g_MusicStep >= len)
		{
			if ((g_MusicMode == MUSIC_MODE_WIN) || (g_MusicMode == MUSIC_MODE_LOSE))
			{
				Music_Stop();
				return;
			}
			g_MusicStep = 0;
			if (g_MusicMode == MUSIC_MODE_OPENING)
				g_MusicLooping = TRUE;
			g_LastNote[0] = g_LastNote[1] = g_LastNote[2] = 0xFF;
		}
		if (g_SfxLeft == 0)
			ApplyRow(CurrentRow());
		else
		{
			const u8* row = CurrentRow();
			for (ch = 1; ch < 3; ++ch)
			{
				u8 note = row[ch];
				u16 period = PeriodOf(note);
				if ((note != 0) && (note != g_LastNote[ch]))
				{
					g_LastNote[ch] = note;
					PSG_SetTone(ch, period);
				}
			}
		}
	}
	else
	{
		for (ch = 0; ch < 3; ++ch)
		{
			u8 floor = (ch == 2) ? 5 : ((ch == 1) ? 4 : 7);
			if ((g_SfxLeft != 0) && (ch == 0))
				continue;
			if (g_ChanVol[ch] <= floor)
				continue;
			if ((g_MusicMode == MUSIC_MODE_GAME) || ((g_MusicFrame & 1) == 0))
			{
				g_ChanVol[ch]--;
				PSG_SetVolume(ch, g_ChanVol[ch]);
			}
		}
	}

	if (g_SfxLeft != 0)
	{
		SfxApplyStep();
		g_SfxStep++;
		g_SfxLeft--;
		if (g_SfxLeft == 0)
		{
			g_SfxKind = SFX_KIND_NONE;
			PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
			g_LastNote[0] = 0xFF;
			ApplyRow(CurrentRow());
		}
	}

	PSG_Apply();
}

void Music_PlayGame(void)
{
	MusicStartMode(MUSIC_MODE_GAME, TRUE);
}

void Music_PlayWin(void)
{
	MusicStartMode(MUSIC_MODE_WIN, FALSE);
}

void Music_PlayLose(void)
{
	MusicStartMode(MUSIC_MODE_LOSE, FALSE);
}

void Music_Pause(void)
{
	g_MusicPlaying = FALSE;
	PSG_Mute();
	PSG_Apply();
}

void Music_Resume(void)
{
	g_MusicPlaying = TRUE;
	g_LastNote[0] = g_LastNote[1] = g_LastNote[2] = 0xFF;
	PSG_SetMixer(PSG_TONE_A_ON | PSG_TONE_B_ON | PSG_TONE_C_ON);
	ApplyRow(CurrentRow());
	PSG_Apply();
}

void Sfx_TypeClick(u8 volume)
{
	if (volume == 0)
		return;
	g_SfxKind = SFX_KIND_CLICK;
	g_SfxStep = 0;
	g_SfxLeft = 3;
	PSG_SetNoise(0x10);
	PSG_SetMixer(PSG_NOISE_A_ON);
	PSG_SetVolume(PSG_CHANNEL_A, volume);
	PSG_Apply();
}

void Sfx_Pop(void)
{
	SfxStart(SFX_KIND_POP, 8);
}

void Sfx_Score(void)
{
	SfxStart(SFX_KIND_SCORE, 9);
}

void Sfx_Cloud(void)
{
	SfxStart(SFX_KIND_CLOUD, 8);
}

void Music_Stop(void)
{
	g_MusicPlaying = FALSE;
	PSG_Mute();
	PSG_Apply();
}

bool Music_IsPlaying(void)
{
	return g_MusicPlaying;
}

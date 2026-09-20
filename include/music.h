#pragma once

void Music_Init(void);
void Music_Tick(void);
void Music_Pause(void);
void Music_Resume(void);
void Music_Stop(void);
void Music_PlayGame(void);
void Music_PlayWin(void);
void Music_PlayLose(void);
bool Music_IsPlaying(void);
void Sfx_TypeClick(u8 volume);
void Sfx_Pop(void);
void Sfx_Score(void);
void Sfx_Cloud(void);
void Sfx_Taxi(void);

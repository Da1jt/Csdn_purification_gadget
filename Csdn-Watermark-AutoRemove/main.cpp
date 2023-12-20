
#include <Windows.h>
#include <shellapi.h>
#include <regex>
#include <thread>
#include <Psapi.h>
using namespace std;
NOTIFYICONDATA g_notifyIconData;
HMENU g_menu;

void ShowContextMenu(HWND hwnd);
wstring GetClipboardText() {
	wstring clipboardText;

	if (!OpenClipboard(NULL)) {
		return clipboardText;
	}

	HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
	if (hClipboardData != NULL) {
		wchar_t* pClipboardData = static_cast<wchar_t*>(GlobalLock(hClipboardData));
		if (pClipboardData != NULL) {
			clipboardText = pClipboardData;

			GlobalUnlock(hClipboardData);
		}
	}

	CloseClipboard();

	return clipboardText;
}

void SetClipboardText(const wstring& text) {
	if (!OpenClipboard(NULL)) {
		return;
	}

	EmptyClipboard();

	HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (text.length() + 1) * sizeof(wchar_t));
	if (hClipboardData == NULL) {
		CloseClipboard();
		return;
	}


	auto* pClipboardData = static_cast<wchar_t*>(GlobalLock(hClipboardData));
	if (pClipboardData == NULL) {
		GlobalFree(hClipboardData);
		CloseClipboard();
		return;
	}

	wcscpy_s(pClipboardData, text.length() + 1, text.c_str());

	GlobalUnlock(hClipboardData);

	SetClipboardData(CF_UNICODETEXT, hClipboardData);

	CloseClipboard();
}

void clipb() {
	while (true) {
		
		wstring clipboardText = GetClipboardText();
		size_t textLength = clipboardText.length();
		try
		{
			wstring target = L"版权声明：本文为CSDN博主";

			for (int i = 0; i < textLength - 14; i++) {
				wstring slp = clipboardText.substr(i, 14);
				if (slp == target) {
					SetClipboardText(clipboardText.substr(0, i - 18));
					break;
				}
			}
		}
		catch (exception ex)
		{
		}
		Sleep(500);
	}
}
void HandleMenuCommand(HWND hwnd, WPARAM wParam)
{
		Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
		PostQuitMessage(0);
		return;
	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
		g_notifyIconData.hWnd = hwnd;
		g_notifyIconData.uID = 1;
		g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		g_notifyIconData.uCallbackMessage = WM_USER + 1;
		g_notifyIconData.hIcon = LoadIcon(NULL, IDI_INFORMATION);
		wcscpy_s(g_notifyIconData.szTip, L"CSDN-Purify");
		Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

		g_menu = CreatePopupMenu();
		AppendMenu(g_menu, MF_STRING, 1, L"EXIT");
		break;

	case WM_USER + 1:
		switch (lParam)
		{
		case WM_RBUTTONUP:
			ShowContextMenu(hwnd);
			break;
		}
		break;

	case WM_COMMAND:
		HandleMenuCommand(hwnd, wParam);
		break;

	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ShowContextMenu(HWND hwnd)
{
	POINT pt;
	GetCursorPos(&pt);

	SetForegroundWindow(hwnd);
	TrackPopupMenu(g_menu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//HWND self = GetForegroundWindow();
	//ShowWindow(self, 0);
	const wchar_t className[] = L"mainc";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	RegisterClass(&wc);
	CreateWindowEx(0, className, L"Csdn_SharePlate", 0, 0, 0, 0, 0,NULL, NULL, hInstance, NULL);
	MSG msg;
	thread thread1(clipb);
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/* Keymapper v1.1
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

// Make a really small executable
#pragma comment(linker,"/ENTRY:main") // Set entry point

// Merge all default sections into the .text (code) section.
#pragma comment(linker,"/MERGE:.rdata=.data")
#pragma comment(linker,"/MERGE:.text=.data")

#pragma comment(lib, "msvcrt.lib")

#if (_MSC_VER < 1300)
	#pragma comment(linker,"/IGNORE:4078")
	#pragma comment(linker,"/OPT:NOWIN98")
#endif

#pragma comment(linker, "/FILEALIGN:0x200")

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType); // for graceful exit
DWORD dwMainThread = 0; // because apparently the console event handler thread for Ctrl+C is different from the main thread

int main(int args, char* argv[])
{
	const char message[] =
		"Caps Lock Remapper\n"
		"Remaps Caps Lock to Backspace on the fly without rebooting.\n"
		"Written by Petrus Theron http://freshcode.co/\n"
		"Fork this on GitHub: http://github.com/theronic/keymapper\n"
		"\nPress Ctrl+C or close window to exit.\n";

	DWORD count = 0;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(hStdOut, message, sizeof(message)-2, &count, NULL);

	if (!SetConsoleCtrlHandler(ConsoleEventHandler, TRUE))
		return -1;

	dwMainThread = GetCurrentThreadId();

	// Retrieve the applications instance
	HINSTANCE appInstance = GetModuleHandle(NULL);
 
	// Attach global keyboard hook to capture keystrokes
	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, appInstance, 0);
	if (!hHook)
		return -2;

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Clean up
	UnhookWindowsHookEx(hHook);
	SetConsoleCtrlHandler(ConsoleEventHandler, FALSE);

	return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

	switch( pKeyBoard->vkCode )
	{
	case VK_CAPITAL:
		{
			switch (wParam)
			{
			case WM_KEYDOWN:
				keybd_event(VK_BACK, 0x8e, 0, 0);
				return 1;
			case WM_KEYUP:
				keybd_event(VK_BACK, 0x8e, KEYEVENTF_KEYUP, 0);
				return 1;
			}
		}
		
		default:
			return CallNextHookEx( NULL, nCode, wParam, lParam );
	}

	return 0;
}


BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType)
{
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
			PostThreadMessage(dwMainThread, WM_QUIT, NULL, NULL);
			return TRUE;

		default:
			return FALSE;
	}
}

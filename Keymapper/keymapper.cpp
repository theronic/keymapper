/*
	PROJECT:
		Keymapper v1.1

	DESCRIPTION:
		Keymapper is a portable tool to remap Caps Lock to Backspace without rebooting.
		Intended for Colemak enthusiasts roaming on others' QWERTY keyboard layouts.
	
		Caps Lock keypresses are intercepted with a global keyboard hook (SetWindowsHookEx)
		and a backspace keypress is simulated in its place to improve touch typing (keybd_event).

		Certain anti-virus heuristics may classify the executable as a potential threat
		due to the keylogging potential of a global keyboard hook. The right way to remap
		your keyboard is using a registry tool like SharpKeys.

	LICENCE:
		Copyright 2010 FreshCode (www.freshcode.co.za).

		Redistribution and use in source and binary forms, with or without modification, are
		permitted provided that the following conditions are met:

			1. Redistributions of source code must retain the above copyright notice, this
			list of conditions and the following disclaimer.

			2. Redistributions in binary form must reproduce the above copyright notice, this
			list of conditions and the following disclaimer in the documentation and/or other
			materials provided with the distribution.

		This software is provided by FreshCode ``as is'' without any express or implied warranty
		blah blah blah if you break stuff, you're an idiot.
*/

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
		"Copyright 2010 FreshCode (www.freshcode.co.za)\n"
		"\nPress Ctrl+C or close window to exit.\n";

	if (args > 2)
	{
	}

	DWORD count = 0;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(hStdOut, message, sizeof(message)-2, &count, NULL);

	if (!SetConsoleCtrlHandler(ConsoleEventHandler, TRUE))
	{
		return -1;
	} else
	{
		dwMainThread = GetCurrentThreadId();
	}

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


#include <windows.h>
#include <map>
#include <iostream>

HHOOK g_hKeyboardHook = NULL;

std::map<DWORD, DWORD> g_keyMap;

const ULONG_PTR MAPPED_KEY_FLAG = 0xABCDEF01;

void InitializeKeyMap() {
    g_keyMap['Q'] = 'A';
    g_keyMap['W'] = 'B';
    g_keyMap['E'] = 'C';
    g_keyMap['R'] = 'D';
    g_keyMap['T'] = 'E';
    g_keyMap['Y'] = 'F';
    g_keyMap['U'] = 'G';
    g_keyMap['I'] = 'H';
    g_keyMap['O'] = 'I';
    g_keyMap['P'] = 'J';

    g_keyMap['A'] = 'K';
    g_keyMap['S'] = 'L';
    g_keyMap['D'] = 'M';
    g_keyMap['F'] = 'N';
    g_keyMap['G'] = 'O';
    g_keyMap['H'] = 'P';
    g_keyMap['J'] = 'Q';
    g_keyMap['K'] = 'R';
    g_keyMap['L'] = 'S';

    g_keyMap['Z'] = 'T';
    g_keyMap['X'] = 'U';
    g_keyMap['C'] = 'V';
    g_keyMap['V'] = 'W';
    g_keyMap['B'] = 'X';
    g_keyMap['N'] = 'Y';
    g_keyMap['M'] = 'Z';

}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;

        if (pkbhs->dwExtraInfo == MAPPED_KEY_FLAG) {
            return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
        }

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
            bool lCtrlPressed = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
            bool lAltPressed = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;

            if (pkbhs->vkCode == 'Q' && lCtrlPressed && lAltPressed) {
                PostMessage(NULL, WM_QUIT, 0, 0);
            }
        }
        auto it = g_keyMap.find(pkbhs->vkCode);
        if (it != g_keyMap.end()) {
            DWORD newVkCode = it->second;

            INPUT input = { 0 };
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = (WORD)newVkCode;
            input.ki.wScan = (WORD)MapVirtualKey(newVkCode, MAPVK_VK_TO_VSC);
            input.ki.dwFlags = 0;

            if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                input.ki.dwFlags |= KEYEVENTF_KEYUP;
            }
            input.ki.dwExtraInfo = MAPPED_KEY_FLAG;

            SendInput(1, &input, sizeof(INPUT));

            return 1;
        }
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    InitializeKeyMap();

    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    if (g_hKeyboardHook == NULL) {
        MessageBox(NULL, L"安装键盘钩子失败！\n请确保应用程序拥有必要权限或未被安全软件阻止。", L"错误", MB_ICONERROR | MB_OK);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
    }

    return (int)msg.wParam;
}

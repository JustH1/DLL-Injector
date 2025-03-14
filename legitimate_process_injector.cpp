#include <windows.h>
#include <string>
#include <iostream>
#include <tlhelp32.h>

using namespace std;

class LegitimateProcessInjector {
public:
    enum class LegitimateProcessInjectorError{
        FAILED_CREATE_REMOTE_THREAD = -6,
        PROCESS_NOT_FOUND = -5,
        FAILED_OPEN_PROCESS = -4,
        FAILED_ALLOCATE_MEMORY_IN_TARGET_PROCESS = -3,
        FAILED_WRITE_TARGET_PROCESS_MEMORY = -2,
        FAILED_GET_ADDRESS_LoadLibraryA = -1,
        SUCCESS = 1
    };

    LegitimateProcessInjector(std::string dllPath)
        : _dllPath(dllPath) {}

    LegitimateProcessInjectorError InjectIntoProcess(const std::string& processName) {
        DWORD processId = GetProcessIdByName(processName);
        if (processId == -1) {
            return LegitimateProcessInjectorError::PROCESS_NOT_FOUND;
        }

        HANDLE processH = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (processH == NULL) {
            return LegitimateProcessInjectorError::FAILED_OPEN_PROCESS;
        }

        LegitimateProcessInjectorError success = InjectDll(processH);
        CloseHandle(processH);
        return success;
    }

private:
    const std::string _dllPath;

    DWORD GetProcessIdByName(const std::string& processName) {
        DWORD processId = -1;
        HANDLE snapshotDescriptor = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshotDescriptor == INVALID_HANDLE_VALUE) {
            return -1;
        }

        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshotDescriptor, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshotDescriptor, &processEntry));
        }

        CloseHandle(snapshotDescriptor);
        return processId;
    }

    LegitimateProcessInjectorError InjectDll(HANDLE processH) {
        LPVOID pRemoteMemory = VirtualAllocEx(processH, NULL, _dllPath.size() + 1, MEM_COMMIT, PAGE_READWRITE);
        if (pRemoteMemory == NULL) {
            return LegitimateProcessInjectorError::FAILED_ALLOCATE_MEMORY_IN_TARGET_PROCESS;
        }

        if (!WriteProcessMemory(processH, pRemoteMemory, _dllPath.c_str(), _dllPath.size() + 1, NULL)) {
            VirtualFreeEx(processH, pRemoteMemory, 0, MEM_RELEASE);
            return LegitimateProcessInjectorError::FAILED_WRITE_TARGET_PROCESS_MEMORY;
        }

        LPVOID pLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
        if (pLoadLibrary == NULL) {
            VirtualFreeEx(processH, pRemoteMemory, 0, MEM_RELEASE);
            return LegitimateProcessInjectorError::FAILED_GET_ADDRESS_LoadLibraryA;
        }

        HANDLE hRemoteThread = CreateRemoteThread(processH, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemoteMemory, 0, NULL);
        if (hRemoteThread == NULL) {
            VirtualFreeEx(processH, pRemoteMemory, 0, MEM_RELEASE);
            return LegitimateProcessInjectorError::FAILED_CREATE_REMOTE_THREAD;
        }

        WaitForSingleObject(hRemoteThread, INFINITE);

        VirtualFreeEx(processH, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hRemoteThread);
        return LegitimateProcessInjectorError::SUCCESS;
    }
};
#define UNICODE
#define _UNICODE

#include "karin.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN64

BYTE LOAD_THREAD_START_ROUTINE[] = {
  0x48, 0x89, 0x4C, 0x24, 0x08    // mov [rsp+0x8],rcx
, 0x48, 0x89, 0xC8                // mov rax,rcx
, 0x48, 0x83, 0xC1, 0x40          // add rcx,byte +0x40
, 0x48, 0x83, 0xC0, 0x08          // add rax,byte +0x8
, 0x48, 0x83, 0xEC, 0x20          // sub rsp,byte +0x20
, 0xFF, 0x10                      // call qword [rax]
, 0x48, 0x83, 0xC4, 0x20          // add rsp,byte +0x20
, 0x48, 0x8B, 0x4C, 0x24, 0x08    // mov rcx,[rsp+0x8]
, 0x48, 0x89, 0x41, 0x10          // mov [rcx+0x10],rax
, 0xC3                            // ret
};

BYTE FREE_THREAD_START_ROUTINE[] = {
  0x48, 0x89, 0xC8         // mov rax,rcx
, 0x48, 0x8B, 0x49, 0x10   // mov rcx,[rcx+0x10]
, 0x48, 0x83, 0xC0, 0x18   // add rax,byte +0x18
, 0x48, 0x83, 0xEC, 0x20   // sub rsp,byte +0x20
, 0xFF, 0x10               // call qword [rax]
, 0x48, 0x83, 0xC4, 0x20   // add rsp,byte +0x20
, 0xC3                     // ret
};

#elif defined(_WIN32)
BYTE LOAD_THREAD_START_ROUTINE[] = {
  0x55                // push ebp
, 0x89, 0xE5          // mov ebp,esp
, 0x51                // push ecx
, 0x8B, 0x45, 0x08    // mov eax,[ebp+0x8]
, 0x89, 0xC1          // mov ecx,eax
, 0x83, 0xC0, 0x24    // add eax,byte +0x24
, 0x50                // push eax
, 0x83, 0xC1, 0x08    // add ecx,byte +0x8
, 0xFF, 0x11          // call dword [ecx]
, 0x8B, 0x4D, 0x08    // mov ecx,[ebp+0x8]
, 0x89, 0x41, 0x0C    // mov [ecx+0xc],eax
, 0x59                // pop ecx
, 0x5D                // pop ebp
, 0xC2, 0x04, 0x00    // ret 0x4
};

BYTE FREE_THREAD_START_ROUTINE[] = {
  0x55                // push ebp
, 0x89, 0xE5          // mov ebp,esp
, 0x8B, 0x45, 0x08    // mov eax,[ebp+0x8]
, 0x83, 0xC0, 0x0C    // add eax,byte +0xc
, 0xFF, 0x30          // push dword [eax]
, 0x83, 0xC0, 0x04    // add eax,byte +0x4
, 0xFF, 0x10          // call dword [eax]
, 0x5D                // pop ebp
, 0xC2, 0x04, 0x00    // ret 0x4
};
#else
    #error Only win32 or win64 is supported.
#endif

void loadlibray(wchar_t* argv[])
{
    DWORD pid = (DWORD)_wtoi64(argv[2]);
    HANDLE hProcess = OpenProcess(GENERIC_ALL, FALSE, pid);
    BYTE* pv = (BYTE*)VirtualAllocEx(hProcess, NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    SIZE_T sz;    
    WriteProcessMemory(hProcess, pv + c_offval(signature), KARIN_SIGNATURE, KARIN_SIGNATURE_SIZE, &sz);
    HMODULE hM = GetModuleHandle(L"Kernel32.dll");
    void* pLoadLibrary = GetProcAddress(hM, "LoadLibraryW");
    WriteProcessMemory(hProcess, pv + c_offval(lpLoadLibraryW), &pLoadLibrary, sizeof(void*), &sz); // 0
    void* pFreeLibrary = GetProcAddress(hM, "FreeLibrary");
    WriteProcessMemory(hProcess, pv+c_offval(lpFreeLibrary), &pFreeLibrary, sizeof(void*), &sz); // 8
    WriteProcessMemory(hProcess, pv+c_offval(lpszKarinDllPath), argv[3], wcslen(argv[3])*2+2, &sz); // 12    
    WriteProcessMemory(hProcess, pv+offset_loadlib, LOAD_THREAD_START_ROUTINE, sizeof(LOAD_THREAD_START_ROUTINE), &sz);
    WriteProcessMemory(hProcess, pv+offset_freelib, FREE_THREAD_START_ROUTINE, sizeof(FREE_THREAD_START_ROUTINE), &sz);
    VirtualProtectEx(hProcess, pv, 4096, PAGE_EXECUTE_READ, NULL);
    FlushInstructionCache(hProcess, pv, 4096);
    wprintf(L"alloc space at %p [%u]\n", pv, pv);
    wprintf(L"thread start at %p\n", (PTHREAD_START_ROUTINE)(pv+offset_loadlib));
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)(pv+offset_loadlib), pv, 0, NULL);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

void reloadlibray(wchar_t* argv[])
{
    DWORD pid = (DWORD)_wtoi64(argv[2]);
    BYTE* pv = (BYTE*)(DWORD)_wtoi64(argv[3]);
    HANDLE hProcess = OpenProcess(GENERIC_ALL, FALSE, pid);
    wprintf(L"parsed adress at %p [%u]\n", pv, pv);
    wprintf(L"thread start at %p\n", (PTHREAD_START_ROUTINE)(pv+offset_loadlib));
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)(pv+offset_loadlib), pv, 0, NULL);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

void freelibray(wchar_t* argv[])
{
    DWORD pid = (DWORD)_wtoi64(argv[2]);
    BYTE* pv = (BYTE*)(DWORD)_wtoi64(argv[3]);
    HANDLE hProcess = OpenProcess(GENERIC_ALL, FALSE, pid);
    wprintf(L"parsed adress at %p [%u]\n", pv, pv);
    wprintf(L"thread start at %p\n", (PTHREAD_START_ROUTINE)(pv+offset_freelib));
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)(pv+offset_freelib), pv, 0, NULL);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

void wmain(int argc, wchar_t* argv[])
{   
    /*printf("%u, %u, %u, %u, %u, %u\n"
, c_offval(signature)
, c_offval(lpLoadLibraryW)
, c_offval(hModuleKarin)
, c_offval(lpFreeLibrary)
, c_offval(lpszKarinDllPath)
, c_offval(Code)
    );
    return;*/

    if(argc != 4) {
        wprintf(L"miyamoto /l <pid> <karin path>");
        wprintf(L"miyamoto /r <pid> <alloc space>");
        wprintf(L"miyamoto /f <pid> <alloc space>");
        return;
    }
    //DebugBreak();

    if(wcsicmp(argv[1], L"/l") == 0)
    {
        loadlibray(argv);
    }
    else if(wcsicmp(argv[1], L"/r") == 0)
    {
        reloadlibray(argv);
    }
    else if(wcsicmp(argv[1], L"/f") == 0)
    {
        freelibray(argv);
    }
}


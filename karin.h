
#include <Windows.h>

#define KARINHEADER_SIZE     1024
#define KARIN_SIGNATURE_SIZE 8

#include <pshpack1.h>
struct KARINHEADER
{
    char signature[KARIN_SIGNATURE_SIZE];      // see KARIN_SIGNATURE macro
    void* lpLoadLibraryW;                      // the address of LoadLibraryW
    HMODULE hModuleKarin;                      // the HMODULE returned from LoadLibraryW
    void* lpFreeLibrary;                       // the address of FreeLibrary
    void* reserved[4];                         // reserved for future use
    wchar_t lpszKarinDllPath[(KARINHEADER_SIZE - 8 - sizeof(void*) * 7) / 2];  // the path of karin.dll
    BYTE Code[0];
};
#include <poppack.h>

typedef KARINHEADER *PKARINHEADER;

C_ASSERT(sizeof(KARINHEADER) <= KARINHEADER_SIZE);
 
#define offset_loadlib 1024
#define offset_freelib 1064

#ifdef _WIN64
    #define KARIN_SIGNATURE "KARIN64"
#elif defined(_WIN32)
    #define KARIN_SIGNATURE "KARIN32"
#else
    #error Only win32 or win64 is supported.
#endif

C_ASSERT(sizeof(KARIN_SIGNATURE) == KARIN_SIGNATURE_SIZE);

// dont't worry, the compiler will compile it to a const int value
#define OFFSET_MEMBER(T, M) \
    (int)&((T*)0)->M

#define PTR_OFFSET_MEMBER(base, T, M) \
    (((BYTE*)base) + OFFSET_MEMBER(T, M))

#define c_offset(m) \
    const int c_offset_##m = OFFSET_MEMBER(KARINHEADER, m);

#define c_offval(m) c_offset_##m

c_offset(signature)
c_offset(lpLoadLibraryW)
c_offset(hModuleKarin)
c_offset(lpFreeLibrary)
c_offset(lpszKarinDllPath)
c_offset(Code)



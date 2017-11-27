#include "karin.h"

typedef unsigned __int64 QWORD;

#define EXPORT_SYMBOL(sym) \
    /*__declspec(dllexport)*/ sym * __stdcall __export##sym ( sym& val ) { return NULL; }

struct KARIN
{
    BYTE  b[8];
    WORD  w[4];
    DWORD dw[2];
    QWORD qw;
};

EXPORT_SYMBOL(KARIN)
EXPORT_SYMBOL(RECT)
EXPORT_SYMBOL(KARINHEADER)
EXPORT_SYMBOL(ACTION_HEADER)
all: karin_bin miyamoto karin_dll do_clear

!if DEFINED(_WIN32)

karin_bin : karin32.asm
    "A:\mip\nasm\nasm.exe" -f bin karin32.asm

miyamoto : miyamoto.cpp
    cl /Zi miyamoto.cpp /link /DEBUG /OUT:miyamoto32.exe

karin_dll : karin.cpp
    cl /Zi karin.cpp /link /SUBSYSTEM:WINDOWS /DLL /DEBUG /OUT:karin32.dll

!else if DEFINED(_WIN64)

karin_bin : karin64.asm
    "A:\mip\nasm\nasm.exe" -f bin karin64.asm

miyamoto : miyamoto.cpp
    cl /Zi miyamoto.cpp /link /DEBUG /OUT:miyamoto64.exe

karin_dll : karin.cpp
    cl /Zi karin.cpp /link /SUBSYSTEM:WINDOWS /DLL /DEBUG /OUT:karin64.dll

!endif

do_clear : 
    del *.ilk
    
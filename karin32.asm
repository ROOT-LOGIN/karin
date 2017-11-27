;!nasm

%include "karin.inc.asm"

BITS 32

SECTION .text

__start:
signature     DB   "KARIN32", 0
LoadLibrary   DD   0
libraryHandle DD   0
FreeLibrary   DD   0
reserved      RESD 4
lpszKarin     DB   "karin32.dll", 0

TIMES offset_loadlib - $ + __start DB 0xCC

; offset offset_loadlib
loadlibrary__:
    push ebp
    mov ebp, esp
    push ecx
    mov eax, [ebp+8]    
    mov ecx, eax
    add eax, lpszKarin - __start
    push eax
    add ecx, LoadLibrary - __start
    call [ecx]
    mov ecx, [ebp+8]
    mov [ecx + libraryHandle - __start], eax
    pop ecx
    pop ebp
    ret 4

TIMES offset_freelib - $ + __start DB 0xCC

; offset offset_freelib
freelibrary__:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    add eax, libraryHandle - __start
    push dword [eax]
    add eax, FreeLibrary - libraryHandle
    call [eax]
    pop ebp
    ret 4

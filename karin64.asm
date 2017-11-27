;!nasm

%include "karin.inc.asm"

BITS 64

SECTION .text

__start:
signature     DB   "KARIN64", 0
LoadLibrary   DQ   0
libraryHandle DQ   0
FreeLibrary   DQ   0
reserved      RESQ 4
lpszKarin     DB   "karin64.dll", 0

TIMES offset_loadlib - $ + __start DB 0xCC

; offset offset_loadlib
loadlibrary__:    
    mov [rsp+8h], rcx
    mov rax, rcx
    add rcx, lpszKarin - __start
    add rax, LoadLibrary - __start
    sub rsp, 20h
    call [rax]    
    add rsp, 20h
    mov rcx, [rsp+8h]
    mov [rcx + libraryHandle - __start], rax
    ret

TIMES offset_freelib - $ + __start DB 0xCC

; offset offset_freelib
freelibrary__:
    mov rax, rcx
    mov rcx, [rcx + libraryHandle - __start]
    add rax, FreeLibrary - __start
    sub rsp, 20h
    call [rax]
    add rsp, 20h
    ret

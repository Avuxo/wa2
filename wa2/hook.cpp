#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

#include "hook.h"

// MS detours was triggering AV for annoying reasons, and just instituting trampolines
// doesn't seem to so... volatility for the win!

// NOTE:
// hook functions (dest) MUST MUST return void - otherwise when the ret instruction runs
// it'll break down the trampoline.

// inject jump instruction to our custom code
static bool _hook(char* src, char* dest, unsigned int len) {
    DWORD originalProtection;

    // make the page writable so that we can inject our instructions
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &originalProtection);
    memset(src, NOP, len);

    // calculate the relative address of the function so we can jump to it
    volatile char* relativeAddress = (char*)(dest - src - 5);

    // insert in jump 
    *src = JMP;
    *(char**)(src + 1) = (char*)relativeAddress;

    // restore the original memory protection for the page
    VirtualProtect(src, len, originalProtection, &originalProtection);

    return true;
}

// allocate region and insert jump instruction to custom code
char* installHook(char* src, char* dest, unsigned int len) {
    char* newRegion = (char*)VirtualAlloc(
        NULL,
        len + 5,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );

    if (!newRegion) return nullptr;

    // insert bytes we have to move to make room for trampoline
    memcpy(newRegion, src, len);
    char* jumpAddress = (char*)(src - newRegion - 5);

    // insert return jump
    *(newRegion + len) = JMP;
    *(char**)(newRegion + len + 1) = jumpAddress;

    if (!_hook(src, dest, len)) {
        return nullptr;
    }
    return newRegion;
}

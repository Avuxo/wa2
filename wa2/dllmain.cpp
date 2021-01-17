#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>

#include "GameHooks.h"
#include "Utility.h"

// injection entrypoint
DWORD WINAPI entry(HMODULE hInst) {
    if (Utility::doesTodokanaiDirectoryExist()) {
        initHooks();
        for (;;) { Sleep(1); }
    } else {
        MessageBoxA(0, "Todokanai Resources missing. Does the 'todokanai' directory exist in the White Album 2 Root?", "Todokanai Error", 0);
    }

    FreeLibraryAndExitThread(hInst, 0);
}

// ideally this state would be stored inside of the main hook context, but there's a slim chance
// of a double-attachment error and then two sub contexts vying for control of hooks
bool hasAttached = false;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (!hasAttached && reason == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)entry, hInst, 0, 0));
        hasAttached = true;
    }
    return TRUE;
}

#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>

#include <d3d9.h>

#include "hook.h"
#include "d3d9.h"
#include "SubContext.h"
#include "GameContext.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// maybe creating function pointer types for every hook isn't very nice.
// if the hooks exceed more than just a couple I'll think about using something a little more...
// void pointer-y
typedef HRESULT(APIENTRY* endscene_t)(LPDIRECT3DDEVICE9 device);
typedef int(__cdecl* audio_function_t)(int*, int, int, int, int);
typedef int(__cdecl* voice_audio_t)(int a1, int a2, int arglist, int a4, int a5, int a6, int a7);


// original callbacks
endscene_t originalEndSceneCB = nullptr;
audio_function_t originalAudioCb = nullptr;
voice_audio_t originalAudio2Cb = nullptr;


// d3d9 vtable and device (captured in hooks)
void* d3dDevice[119];
LPDIRECT3DDEVICE9 device;

// main context for subtitling work
SubContext subContext;


// called every frame (entry into the draw loop)
void __stdcall endSceneHook(LPDIRECT3DDEVICE9 originalDevice) {
    if (!device) {
        device = originalDevice;
    }

    if (!subContext.device) {
        subContext = SubContext(device);
    }

    subContext.update();

    originalEndSceneCB(device);
}

// called every time an audio resource is loaded.
int __cdecl audioFunctionHook(int* a1, int audioId, int a3, int a4, int a5) {
    int result = originalAudioCb(a1, audioId, a3, a4, a5);

    if (subContext.device) {
        subContext.checkForTrigger(audioId, SOUND_EFFECT);
    }


    return result;
}

int __cdecl voiceAudioHook(int a1, int a2, int arglist, int a4, int a5, int a6, int a7) {
    int result = originalAudio2Cb(a1, a2, arglist, a4, a5, a6, a7);

    // The reality here is that they do a weird check of a few different
    // values to get the filename but if I read from the global context
    // version, it appears to always be right.
    int file = *GameContext::currentFile;
    
    // this offset appears to always be 0 but leaving here for posterity's sake
    int voiceIndex = (*GameContext::voiceOffset) + a4;

    if (subContext.device) {
        subContext.checkForTrigger(voiceIndex, VOICE);
    }

    return result;
}

// obviously this works with a lot of nasty global state here, so it's crucial that this is called AFTER the memory
// for the d3ddevice is establshed AND the device has already been obtained.
// I'm pretty sure the order in which hooks are installed doesn't really matter.
static void setupHooks() {
    originalEndSceneCB = (endscene_t)installHook(
        (char*)d3dDevice[42],
        (char*)endSceneHook,
        7
    );

    originalAudioCb = (audio_function_t)installHook(
        (char*)0x00405940,
        (char*)audioFunctionHook,
        8
    );

    originalAudio2Cb = (voice_audio_t)installHook(
        (char*)0x0040ECC0,
        (char*)voiceAudioHook,
        6
    );
}

/*
    Does the todokanai directory exist in the root of the project?
    This directory contains both patch config and assets.
*/
static inline bool doesTodokanaiDirectoryExist() {
    return !(GetFileAttributesA(".\\todokanai") == INVALID_FILE_ATTRIBUTES);
}


// injection entrypoint
DWORD WINAPI entry(HMODULE hInst) {
    if (doesTodokanaiDirectoryExist()) {
        // remove hooks and uninject DLL.
        if (tryToGetDevice(d3dDevice, sizeof(d3dDevice))) {
            setupHooks();
        }

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

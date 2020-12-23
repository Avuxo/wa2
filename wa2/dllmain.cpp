#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>

#include <d3d9.h>

#include "hook.h"
#include "d3d9.h"
#include "SubContext.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// maybe creating function pointer types for every hook isn't very nice.
// if the hooks exceed more than just a couple I'll think about using something a little more...
// void pointer-y
typedef HRESULT(APIENTRY* endscene_t)(LPDIRECT3DDEVICE9 device);
typedef int(__cdecl* increment_sub_cb_t)(char*, int);


// original callbacks
endscene_t originalEndSceneCB = nullptr;
increment_sub_cb_t originalIncrementCb = nullptr;


// d3d9 vtable and device (captured in hooks)
void* d3dDevice[119];
LPDIRECT3DDEVICE9 device;

// main context for subtitling work
SubContext subContext;

// called for every new line loaded (and loads / quick loads; works with auto/skip)
// line is the loaded line of text... i don't know what the int is, it's a flag of some sort.
void __cdecl incrementHook(char* line, int unknown) {
	if (subContext.device)
		subContext.updateSubs();
	
	originalIncrementCb(line, unknown);
}


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

// obviously this works with a lot of nasty global state here, so it's crucial that this is called AFTER the memory
// for the d3ddevice is establshed AND the device has already been obtained.
// I'm pretty sure the order in which hooks are installed doesn't really matter.
static void setupHooks() {
	originalEndSceneCB = (endscene_t)installHook(
		(char*)d3dDevice[42],
		(char*)endSceneHook,
		7
	);

	originalIncrementCb = (increment_sub_cb_t)installHook(
		(char*)0x004034C0, // address of original function to increment "relative" script index
		(char*)incrementHook,
		8
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
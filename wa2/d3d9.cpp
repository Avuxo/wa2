#include <Windows.h>
#include <d3d9.h>

#include "d3d9.h"

static HWND window;
int width, height;

BOOL CALLBACK enumWind(HWND handle, LPARAM lp) {
    DWORD pid;
    GetWindowThreadProcessId(handle, &pid);
    if (GetCurrentProcessId() != pid) return TRUE;

    window = handle;
    return FALSE;
}

HWND getWindow() {
    window = nullptr;

    // write the window we're currently using.
    EnumWindows(enumWind, NULL);

    RECT size;
    GetWindowRect(window, &size);
    width = size.right - size.left;
    height = size.top - size.bottom;

    return window;
}

// this function is also taken from wherever the hook code is from.
// I've been using this DX9 hook for a long time so I can't remember where it's from
// but all credits go to the original author
//
// write the vtable for directx9 into the given pointer.
// this is a memory leak
bool getDXDevice(void** vtable, size_t size) {
    if (!vtable) return false;

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d) return false;
    IDirect3DDevice9* dummyDevice = nullptr;

    // this is not dx related, but we need to get the base address and this is the most
    // convenient place because we already have a reference to the HWND which makes
    // calculating the pid sorta easier.
    HWND hDeviceWindow = getWindow();

    D3DPRESENT_PARAMETERS d3dParams = {};
    // wa2 starts in windowed by default. If it's in full screen we fix this later
    d3dParams.Windowed = true;
    d3dParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dParams.hDeviceWindow = hDeviceWindow;

    HRESULT createdDevice = d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        d3dParams.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dParams,
        &dummyDevice
    );

    if (createdDevice != S_OK) {
        // this will fail if the user is in full screen, so we try again
        // now assuming it is in fullscreen mode.
        d3dParams.Windowed = false;
        createdDevice = d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            d3dParams.hDeviceWindow,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dParams,
            &dummyDevice
        );
        if (createdDevice != S_OK) {
            d3d->Release();
            return false;
        }
    }

    // save our vtable and free the memory we allocated since we don't need it anymore
    memcpy(vtable, *(void***)(dummyDevice), size);
    dummyDevice->Release();
    d3d->Release();
    return true;
}

// Direct3DCreate9 is called in the walbum engine incredibly late (after the Leaf intro has finished)
bool tryToGetDevice(void** vtable, size_t size) {
    unsigned int tries = 0;
    bool result = false;
    while (!result && tries < MAX_TRIES) {
        Sleep(150);
        result = getDXDevice(vtable, size);
        tries++;
    }

    if (tries == MAX_TRIES) {
        MessageBoxA(0, "Maximum number of attempts to start plugin patch failed. Please restart game.", "Todokanai Error", 0);
    }

    return result;
}

typedef IDirect3D9* (__stdcall* callback_t)(UINT);
HINSTANCE hDX = nullptr;

callback_t realCreate;

// direct the exported symbol to the mangled version
// https://docs.microsoft.com/en-us/cpp/build/reference/export-exports-a-function?view=vs-2019/
#pragma comment(linker, "")

extern "C" __declspec(dllexport) IDirect3D9 * WINAPI Direct3DCreate9Hook(UINT SDKVersion) {
    if (!hDX) {
        TCHAR buffer[MAX_PATH];
        GetSystemDirectory(buffer, MAX_PATH);
        strncat_s(buffer, "\\d3d9.dll", MAX_PATH);

        hDX = LoadLibrary(buffer);

        if (!hDX) {
            MessageBoxA(0, "could not load", 0, 0);
            return nullptr;
        }
    }

    callback_t realCreate = (callback_t)GetProcAddress(hDX, "Direct3DCreate9");

    if (!realCreate) {
        MessageBoxA(0, "could not create", 0, 0);
        return nullptr;
    }

    return realCreate(SDKVersion);
}
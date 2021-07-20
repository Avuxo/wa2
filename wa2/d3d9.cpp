#include "hook.h"
#include "Direct3DWrapper.h"

static HWND window;
int width, height;

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

    IDirect3D9 *device = realCreate(SDKVersion);

    Direct3DWrapper *wrapped = new Direct3DWrapper(device);

    return wrapped;
}
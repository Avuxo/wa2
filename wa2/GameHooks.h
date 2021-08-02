#pragma once

#include <d3d9.h>

// hook callback types
typedef HRESULT (APIENTRY   *endscene_t)        (LPDIRECT3DDEVICE9 device);
typedef int     (__cdecl    *set_line_cb_t)    (signed int);
typedef int     (__cdecl    *audio_function_t) (int *, int, int, int, int);
typedef int     (__cdecl    *voice_audio_t)    (int a1, int a2, int arglist, int a4, int a5, int a6, int a7);
typedef int     (__fastcall *video_playback_t) (void *_this, LPVOID *ppv, LPCSTR lpMultiByteStr, HWND hWnd, int a4);

void endScene(LPDIRECT3DDEVICE9 d3dDevice);
void setupHooks();
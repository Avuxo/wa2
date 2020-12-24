#pragma once

#include <d3d9.h>
#include <d3dx9.h>

// renders debug info to the screen
class DebugLogger {
    LPDIRECT3DDEVICE9 device;
    ID3DXFont* font;
    DWORD fontColor;

    void update();
};
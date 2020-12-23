#pragma once

#include <d3d9.h>
#include <d3dx9.h>

struct TextRenderer {
    LPDIRECT3DDEVICE9 device;
    LPDIRECT3DTEXTURE9 texture;
    LPD3DXSPRITE sprite;
    D3DXVECTOR3 pos;
    D3DCOLOR color;

    int lastCharWidth;

    TextRenderer();
    TextRenderer(LPDIRECT3DDEVICE9);
    
    HRESULT init();

    LPCSTR getFontPath();
    void drawText(char* s, int x, int y);
    int getTopAdjustedGlyph(char ch, RECT *rect);
    void renderText(char *str, int len, int x, int y);
};
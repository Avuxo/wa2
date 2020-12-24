#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>

const char LINE_BREAK = '^';
const int SCREEN_WIDTH = 1280;
const int CHAR_WIDTH = 20;

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

    int getTopAdjustedGlyph(char ch, RECT *rect);
    void renderText(char *str, int len);
    int calculateXOffset(char* line, int len);
};
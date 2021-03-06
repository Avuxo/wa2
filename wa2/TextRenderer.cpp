#pragma once

#include "TextRenderer.h"
#include "font_atlas.h"
#include "Utility.h"

TextRenderer::TextRenderer() {
    this->device = nullptr;
    this->texture = nullptr;
    this->sprite = nullptr;
    this->lastCharWidth = 0;
}

TextRenderer::TextRenderer(LPDIRECT3DDEVICE9 device) {
    this->device = device;
    this->texture = nullptr;
    this->sprite = nullptr;

    this->color = D3DCOLOR_ARGB(255, 255, 255, 255);

    pos.x = 50;
    pos.y = 50;
    pos.z = 0; // using a v3 here is just useful for compatibility

    this->lastCharWidth = 0;
}

HRESULT TextRenderer::init() {
    HRESULT result;

    result = D3DXCreateTextureFromFileEx(
        this->device,
        Utility::getResourcePath(".\\todokanai\\font.png"),
        FONT_FILE_WIDTH,
        FONT_FILE_HEIGHT,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        NULL,
        NULL,
        &(this->texture)
    );

    if (FAILED(result)) {
        return result;
    }

    result = D3DXCreateSprite(this->device, &(this->sprite));
    return result;
}

/**
 * validate char as being in the renderer's page
 */
static inline bool isValidChar(char c) {
    return (((int)c > 0x1f) && ((int)c < 0x7f));
}

// the x-location of a subtitle is calculated using the center of the screen
// combined with the length of the line.
int TextRenderer::calculateXOffset(char* line, int len) {
    // length from pointer to start of line to next line break or end of string
    // length of current line is needed to calculate the `centering' of the string

    int lineLen;
    for (lineLen = 0; lineLen < len && line[lineLen] != LINE_BREAK; lineLen++);

    // roughly calculate the center of the screen offset the center of the string
    return (SCREEN_WIDTH - lineLen * CHAR_WIDTH) / 2;
}

void TextRenderer::renderText(char* str, int len) {
    RECT rect;

    if (str == nullptr) {
        MessageBoxA(0, "Attempted to load null string.", "Todokanai Error", 0);
        return;
    }

    if (sprite && texture) {
        sprite->Begin(D3DXSPRITE_ALPHABLEND);
        this->pos.x = calculateXOffset(str, len);
        unsigned int lineOffset = 0;
        for (int i = 0; i < len; i++) {
            if (!isValidChar(str[i])) continue;

            if (str[i] == LINE_BREAK) {
                i++;
                lineOffset += 35;
                // find x offset for line following current char
                this->pos.x = calculateXOffset(&str[i], len - i);
            }

            int topAdjust = this->getTopAdjustedGlyph(str[i], &rect);
            this->pos.y = 50 + topAdjust + lineOffset;
            sprite->Draw(this->texture, &rect, NULL, &(this->pos), this->color);
            this->pos.x += this->lastCharWidth - 2;
        }

        sprite->End();
    }
}

/*
    SECURITY WARNING: This function is built such that it assumes both that the char exists in the page

    get the texture atlas location for a char so that a rect can be built.
    return value is the top adjust for the given char
*/
int TextRenderer::getTopAdjustedGlyph(char ch, RECT* rect) {
    if (ch == fontAtlas[ch - 0x20].codePoint) {
        Character atlasInfo = fontAtlas[ch - 0x20];

        rect->left = atlasInfo.x;
        rect->top = atlasInfo.y;
        rect->right = atlasInfo.x + atlasInfo.width;
        rect->bottom = atlasInfo.y + atlasInfo.height;

        this->lastCharWidth = atlasInfo.width;

        return atlasInfo.topAdjust;
    }
    return 0;
}

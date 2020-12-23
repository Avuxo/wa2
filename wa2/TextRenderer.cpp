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



void TextRenderer::renderText(char *str, int len, int x, int y) {
	RECT rect;

	if (sprite && texture) {
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
		this->pos.x = 50;
		for (int i = 0; i < len; i++) {
			int topAdjust = this->getTopAdjustedGlyph(str[i], &rect);
			this->pos.y = 50 + topAdjust;
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
int TextRenderer::getTopAdjustedGlyph(char ch, RECT *rect) {
    // TODO: make this a literal access once the logic is finished
    //       by filling in the spaces in the font atlas with empty members
    //       since there's the subset of... ascii characters
    for (int i = 0; i < FONT_CHAR_COUNT; i++) {
        if (ch == fontAtlas[i].codePoint) {
            Character atlasInfo = fontAtlas[i];
			
			rect->left = atlasInfo.x;
			rect->top = atlasInfo.y;
			rect->right = atlasInfo.x + atlasInfo.width;
			rect->bottom = atlasInfo.y + atlasInfo.height;

			this->lastCharWidth = atlasInfo.width;
			
			return atlasInfo.topAdjust;
        }
    }
	return 0;
}
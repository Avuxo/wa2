#include "SubContext.h"
#include "GameContext.h"

#include <stdlib.h>

SubContext::SubContext() {
    this->device = nullptr;

    this->startTick = 0;

    this->subTrackIndex = 0;
    this->subIndex = 0;

    this->playing = false;
}

SubContext::SubContext(LPDIRECT3DDEVICE9 device) {
    this->device = device;

    this->startTick = GetTickCount64();

    this->subTrackIndex = 0;
    this->subIndex = 0;

    this->renderer = TextRenderer(this->device);

    this->playing = false;

    HRESULT result = this->renderer.init();
    if (FAILED(result)) {
        MessageBoxA(0, "Failed to load Todokanai Font file, will run without extra text functionality.", "Todokanai Error", 0);
    }

    this->tracks = parseSubs();

    this->font = nullptr;
#ifdef _DEBUG
    // create the debug menu font
    D3DXCreateFontA(
        device,
        20,
        0,
        true,
        1,
        false,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        "Andale Mono",
        &(this->font)
    );
    this->fontColor = D3DCOLOR_ARGB(255, 50, 255, 150);
#endif
}

void SubContext::checkForTrigger() {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;

    // in defense of this lame linear search, this only iterates over each
    // sub _track_, not each line and more importantly, it's only triggered
    // on each new line of dialogue, not every frame.
    for (unsigned int i = 0; i < tracks.size(); i++) {
        if (playing && tracks[i].endFile == file && tracks[i].endLine == line) {
            this->playing = false;
            // no breaking condition here to account for the fact that the end of one sub
            // can be the start of another.
        }

        if (tracks[i].startFile == file && tracks[i].startLine == line) {
            this->playing = true;
            this->startTick = GetTickCount64();

            this->subTrackIndex = i;
            this->subIndex = 0;

            return;
        }
    }
}

void SubContext::update() {
    if (!this->device) return;

#ifdef _DEBUG
    this->drawDebugMenu();
#endif

    if (playing) {
        std::vector<line_t> lines = this->tracks[this->subTrackIndex].lines;
        ULONGLONG ticks = GetTickCount64() - this->startTick;

        if (ticks > lines[this->subIndex].start && ticks < lines[this->subIndex].end) {
            this->displayCurrentSubtitle();
        }

        if (ticks > lines[this->subIndex].end) {
            if (this->subIndex < lines.size()) {
                this->subIndex++;
            }
        }
    }
}

void SubContext::displayCurrentSubtitle() {
    std::string text = this->tracks[this->subTrackIndex].lines[this->subIndex].text;
    this->renderer.renderText(
        // TODO: pull this `calculation' out so that the precomputed c-str representation
        //       is already available and it doesn't need to have this double dereference every frame
        (char*)text.c_str(),
        text.length()
    );
}

void SubContext::updateSubs() {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;

    this->checkForTrigger();
}

// this is used to draw debug text.
void SubContext::drawText(int x, int y, char* text) {
    RECT rect;
    SetRect(&rect, x, y, x, y);
    this->font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, fontColor);
}

// draw an on-screen debug screen so that i can monitor values while
// debugging
void SubContext::drawDebugMenu() {
    char* ticksString = (char*)malloc(sizeof(char) * 64);
    sprintf_s(ticksString, sizeof(char) * 64, "ticks: %llu", GetTickCount64() - this->startTick);
    char* playingString = (char*)malloc(sizeof(char) * 13);
    sprintf_s(playingString, sizeof(char) * 13, "playing: %s", this->playing ? "yes" : "no");

    const long currentLine = *((long*)0xA391E8);
    const long currentFile = *((long*)0x4BE4CC);

    char* scriptString = (char*)malloc(sizeof(char) * 64);
    sprintf_s(scriptString, sizeof(char) * 64, "Script: %d:%d", currentFile, currentLine);

    this->drawText(50, 50, ticksString);
    this->drawText(50, 65, playingString);
    this->drawText(50, 80, scriptString);

    free(ticksString);
    free(playingString);
    free(scriptString);
}
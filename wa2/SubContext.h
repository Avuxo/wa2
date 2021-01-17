#pragma once

#include <map>
#include <string>
#include <vector>

#include "TextRenderer.h"
#include "SubParser.h"

enum AudioType {
    BGM = 0, VOICE = 1, SOUND_EFFECT = 2
};

// used for debug output
static const char* audioTypeStrings[] {
    "BGM", "VOICE", "SOUND EFFECT"
};

/*
    Context object for the subtitling system.
    This is intended to be used as a singleton, initialized
    as soon as the D3D9 hooks are in place in dllmain.cpp.
*/

struct SubContext {
    TextRenderer renderer;
    LPDIRECT3DDEVICE9 device;

    ULONGLONG startTick;
    ULONGLONG waitTicks;

    ID3DXFont* font;
    DWORD fontColor;

    unsigned int subTrackIndex;
    unsigned int subIndex;
    
    int lastSoundPlayed;
    const char* lastSoundPlayedType;

    std::vector<subtitle_t> tracks;

    bool playing;

    SubContext();
    SubContext(LPDIRECT3DDEVICE9 device);
    void checkForTrigger(int audioId, AudioType type);
    void checkForCutoff();
    void displayCurrentSubtitle();
    void play(int trackIndex, AudioType type);
    void update(); // called every frame by d3d9 hook

    void drawText(int x, int y, char* text);
    void drawDebugMenu();
};
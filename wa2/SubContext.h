#pragma once

#include <map>
#include <string>
#include <vector>

#include "TextRenderer.h"
#include "SubParser.h"

#define UNLOADED_LINE -2

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

    // in the case of 1002:105 in IC, 105 is before a long animation plays
    // and then the callback is called /again/ without the line having changed
    // halfway through a voice track we need to add subtitles... I know, how 
    // god damn unlucky is that.
    // Of course, this naive solution has a pretty obvious pitfall: if a user loads
    // a save midway through a sub track back to the trigger, it's not going to play
    // unless they load exactly the line before. This means that a single trigger is going
    // to be a little bit messed up. I personally apologize to that user, but too bad.
    unsigned int currentPlayingLine;
    unsigned int currentPlayingFile;

    std::vector<subtitle_t> tracks;

    bool playing;

    SubContext();
    SubContext(LPDIRECT3DDEVICE9 device);
    void checkForTrigger();
    void displayCurrentSubtitle();
    void update(); // called every frame by d3d9 hook
    void updateSubs();

    void drawText(int x, int y, char* text);
    void drawDebugMenu();
};
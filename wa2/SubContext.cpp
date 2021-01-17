#include "SubContext.h"
#include "GameContext.h"


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

    this->lastSoundPlayed = -1;

    HRESULT result = this->renderer.init();
    if (FAILED(result)) {
        MessageBoxA(0, "Failed to load Todokanai Font file, will run without extra text functionality.", "Todokanai Error", 0);
    }

    this->tracks = parseSubs();

    this->lastSoundPlayedType = "";

    this->font = nullptr;
#ifdef _TTL_DEBUG
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

void SubContext::checkForCutoff() {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;
    unsigned int index = this->subTrackIndex;
    
    if (index < this->tracks.size()) {
        if (playing && tracks[index].endFile == file && tracks[index].endLine <= line) {
            this->playing = false;
        }
    }
}

void SubContext::checkForTrigger(int audioId, AudioType type) {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;

    this->lastSoundPlayed = audioId;
    // is this ideal? absolutely not. However, it's only being used for debug output
    // and I even put it in a conditional compile macro so that this won't cause problems
    // upon release (since this is just some bullshit)
#ifdef _TTL_DEBUG
    this->lastSoundPlayedType = audioTypeStrings[(int)type];
#endif

    this->checkForCutoff();

    // in defense of this lame linear search, this only iterates over each
    // sub _track_, not each line and more importantly, it's only triggered
    // on each new line of dialogue, not every frame.
    size_t len = tracks.size();
    for (unsigned int i = 0; i < len; i++) {
        // check for start
        if (type == SOUND_EFFECT) {
            if (!playing && audioId == tracks[i].triggerId) {
                this->play(i, type);
                return;
            }
        } else if (type == VOICE) {
            if (!playing && file == tracks[i].triggerFile && audioId == tracks[i].triggerLine) {
                this->play(i, type);
                return;
            }
        }
    }
}

void SubContext::play(int trackIndex, AudioType type) {
    this->playing = true;
    this->startTick = GetTickCount64();

    this->subTrackIndex = trackIndex;
    this->subIndex = 0;

}

void SubContext::update() {
    if (!this->device) { return; }

#ifdef _TTL_DEBUG
    this->drawDebugMenu();
#endif

    if (playing) {
        ULONGLONG ticks = GetTickCount64() - this->startTick;
        
        if (this->subTrackIndex < this->tracks.size()) {
            std::vector<line_t> lines = this->tracks[this->subTrackIndex].lines;
            
            if (ticks > lines[this->subIndex].start && ticks < lines[this->subIndex].end) {
                //this->displayCurrentSubtitle();
            }

            if (this->subIndex < lines.size() && ticks > lines[this->subIndex].end) {
                if (this->subIndex < lines.size() - 1) {
                    this->subIndex++;
                } else {
                    if (ticks > lines[this->subIndex].end) {
                        this->playing = false;
                    }
                }
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

// this is used to draw debug text.
void SubContext::drawText(int x, int y, char* text) {
    RECT rect;
    SetRect(&rect, x, y, x, y);
    this->font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, fontColor);
}

// draw an on-screen debug screen so that i can monitor values while
// debugging
void SubContext::drawDebugMenu() {
    if (this->playing) {
        // probably will never /not/ hit just because of the playing check, but
        // just to be safe...
        if (this->subTrackIndex < this->tracks.size()) {
            std::vector<line_t> lines = this->tracks[this->subTrackIndex].lines;
            char currentInfoString[64];
            sprintf_s(currentInfoString, sizeof(char) * 64, "Start tick: %llu | End tick: %llu", lines[this->subIndex].start, lines[this->subIndex].end);
            this->drawText(50, 95, currentInfoString);
        }
    }

    char audioIdString[128];
    sprintf_s(audioIdString, sizeof(char) * 128, "Last Loaded Audio ID: %s:%d", this->lastSoundPlayedType, this->lastSoundPlayed);

    char ticksString[64];
    sprintf_s(ticksString, sizeof(char) * 64, "Ticks: %llu", GetTickCount64() - this->startTick);
    
    char playingString[13];
    sprintf_s(playingString, sizeof(char) * 13, "Playing: %s", this->playing ? "yes" : "no");

    const long currentLine = *((long*)0xA391E8);
    const long currentFile = *((long*)0x4BE4CC);

    char scriptString[64];
    sprintf_s(scriptString, sizeof(char) * 64, "Script: %d:%d", currentFile, currentLine);


    this->drawText(50, 35, audioIdString);
    this->drawText(50, 50, playingString);
    this->drawText(50, 65, scriptString);
    this->drawText(50, 80, ticksString);
}
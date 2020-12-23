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
}

void SubContext::checkForTrigger() {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;

    // in defense of this lame linear search, this only iterates over each
    // sub _track_, not each line and more importantly, it's only triggered
    // on each new line of dialogue, not every frame.
    for (unsigned int i = 0; i < tracks.size(); i++) {
        if (tracks[i].startFile == file && tracks[i].startLine == line) {
            this->playing = true;
            this->startTick = GetTickCount64();
            
            this->subTrackIndex = i;
            this->subIndex = 0;
            
            return;
        }

        if (playing && tracks[i].endFile == file && tracks[i].endLine == line) {
            this->playing = false;
            return;
        }
    }
}

void SubContext::update() {
    if (!this->device) return;

    if (playing) {
        std::vector<line_t> lines = this->tracks[this->subTrackIndex].lines;
        ULONGLONG ticks = GetTickCount64() - this->startTick;

        if (ticks > lines[this->subIndex].start && ticks < lines[this->subIndex].end) {
            this->displayCurrentSubtitle();
        }

        if (ticks > lines[this->subIndex].end && this->subIndex < lines.size()) this->subIndex++;
    }
}

void SubContext::displayCurrentSubtitle() {
    this->renderer.renderText(
        // TODO: pull this `calculation' out so that the precomputed c-str representation
        //       is already available and it doesn't need to have this double dereference every frame
        (char*)this->tracks[this->subTrackIndex].lines[this->subIndex].text.c_str(),
        this->tracks[this->subTrackIndex].lines[this->subIndex].text.length(),
        50,
        50
    );
}

void SubContext::updateSubs() {
    int line = *GameContext::currentLine;
    int file = *GameContext::currentFile;

    this->checkForTrigger();
}
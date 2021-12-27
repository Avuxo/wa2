#include "hook.h"
#include "d3d9.h"
#include "SubContext.h"
#include "GameHooks.h"
#include "GameContext.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// original callbacks
set_line_cb_t originalLineWriteCb = nullptr;
audio_function_t originalAudioCb = nullptr;
voice_audio_t originalAudio2Cb = nullptr;
video_playback_t videoPlaybackCb = nullptr;

// main context for subtitling work
SubContext subContext;


// called every frame (entry into the draw loop)
void endScene(LPDIRECT3DDEVICE9 device) {
    if (!subContext.device) {
        subContext = SubContext(device);
    }

    if (subContext.device) {
        subContext.update();
    }
}

// called for every new line loaded (and loads / quick loads; works with auto/skip)
// line is the loaded line of text... i don't know what the int is, it's a flag of some sort.
// without this hook in place, monologue lines (no audio) wouldn't work as cut-offs.
int __cdecl setLineHook(signed int a1) {
    int rv = originalLineWriteCb(a1);
    if (subContext.device) {
        subContext.checkForCutoff();
    }


    return rv;
}

// called every time an audio resource is loaded.
int __cdecl audioFunctionHook(int *a1, int audioId, int a3, int a4, int a5) {
    int result = originalAudioCb(a1, audioId, a3, a4, a5);

    if (subContext.device) {
        subContext.checkForTrigger(audioId, AudioType::SOUND_EFFECT);
    }


    return result;
}

int __cdecl voiceAudioHook(int a1, int a2, int arglist, int a4, int a5, int a6, int a7) {

    int result = originalAudio2Cb(a1, a2, arglist, a4, a5, a6, a7);

    // The reality here is that they do a weird check of a few different
    // values to get the filename but if I read from the global context
    // version, it appears to always be right.
    int file = *GameContext::currentFile;
    // this offset appears to always be 0 but leaving here for posterity's sake
    int voiceIndex = (*GameContext::voiceOffset) + a4;

    if (subContext.device) {
        subContext.checkForTrigger(voiceIndex, AudioType::VOICE);
    }

    return result;
}


void setupHooks() {
    // in cases where d3d9 is double or triple loaded, we still need
    // to know if hooks have already been installed. As such, I'm using
    // the current file region because it will be overwritten when the game
    // is loaded. magic number chosen at random through a golden witch dice roll
   if (*GameContext::currentFile == 0xBEA70) { return; }
   *GameContext::currentFile = 0xBEA70;
   
    originalLineWriteCb = (set_line_cb_t)installHook(
        (char *)0x00405180,
        (char *)setLineHook,
        5
    );

    originalAudioCb = (audio_function_t)installHook(
        (char *)0x00405940,
        (char *)audioFunctionHook,
        8
    );

    originalAudio2Cb = (voice_audio_t)installHook(
        (char *)0x0040ECC0,
        (char *)voiceAudioHook,
        6
    );
}
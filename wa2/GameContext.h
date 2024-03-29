#pragma once

// random assortment of known addresses in the game process for getting context about the game.
namespace GameContext {
    volatile int* const currentLine = (int*)0xA391E8;
    volatile int* const currentFile = (int*)0x4BE4CC;
    volatile int* const inHScene    = (int*)0x4D2D20; // bool
    volatile int* const skipping    = (int*)0x4BE018; // bool
    volatile int* const voiceOffset = (int*)0xA391EC; // offset that seems to always be 0
    volatile int *const playingMov =  (int*)0xB6F214; // movie playing?
}
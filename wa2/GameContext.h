#pragma once

// random assortment of known addresses in the game process for getting context about the game.
namespace GameContext {
    volatile int* const currentLine = (int*)0xA391E8;
    volatile int* const currentFile = (int*)0x4BE4CC;
    volatile int* const inHScene = (int*)0x4D2D20; // bool
}
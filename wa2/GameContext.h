#pragma once

// random assortment of known addresses in the game process for getting context about the game.
namespace GameContext {
    int* const currentLine = (int*)0xA391E8;
    int* const currentFile = (int*)0x4BE4CC;
    int* const inHScene = (int*)0x4D2D20; // bool
}
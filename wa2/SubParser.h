#pragma once

#include <Windows.h>
#include <vector>

#define MAX_LINE_LENGTH 2048

#define UNLOADED_LINE -2

struct line_t {
    line_t() {}
    line_t(ULONGLONG start, ULONGLONG length, std::string text) {
        this->start = start;
        this->end = start + length;
        this->text = text;
    }

    ULONGLONG start;
    ULONGLONG end;
    std::string text;
};

struct subtitle_t {
    subtitle_t() {}
    subtitle_t(std::vector<line_t> lines, int fileOrZero, int trigger, unsigned int endFile, unsigned int endLine) {
        this->lines = lines;
        // either triggerLine&file OR triggerId
        // depends on if its a voice trigger or not
        this->trigger = trigger;
        this->triggerFile = fileOrZero;
        
        this->endFile = endFile;
        this->endLine = endLine;
    }

    std::vector<line_t> lines;
    int trigger, triggerFile, endFile, endLine;
};

std::vector<subtitle_t> parseSubs();

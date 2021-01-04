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
    subtitle_t(std::vector<line_t> lines, int idOrFile, int lineOrZero, unsigned int endFile, unsigned int endLine) {
        this->lines = lines;
        // either triggerLine&file OR triggerId
        // depends on if its a voice trigger or not
        if (lineOrZero != 0) {
            this->triggerLine = lineOrZero;
            this->triggerFile = idOrFile;
            this->triggerId = UNLOADED_LINE;
        } else {
            this->triggerLine = UNLOADED_LINE;
            this->triggerFile = UNLOADED_LINE;
            this->triggerId = idOrFile;
        }
        
        this->endFile = endFile;
        this->endLine = endLine;
    }

    std::vector<line_t> lines;
    unsigned int triggerId, triggerLine, triggerFile, endFile, endLine;
};

std::vector<subtitle_t> parseSubs();

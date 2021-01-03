#pragma once

#include <Windows.h>
#include <vector>
#define MAX_LINE_LENGTH 2048

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
    subtitle_t(std::vector<line_t> lines, int triggerId, unsigned int endFile, unsigned int endLine) {
        this->lines = lines;
        this->triggerId = triggerId;
        this->endFile = endFile;
        this->endLine = endLine;
    }

    std::vector<line_t> lines;
    unsigned int triggerId, endFile, endLine;
};

std::vector<subtitle_t> parseSubs();

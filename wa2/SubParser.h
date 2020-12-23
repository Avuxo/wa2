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
    subtitle_t(std::vector<line_t> lines, unsigned int startFile, unsigned startLine, unsigned endFile, unsigned endLine) {
        this->lines = lines;
        this->startFile = startFile;
        this->startLine = startLine;
        this->endFile = endFile;
        this->endLine = endLine;
    }

    std::vector<line_t> lines;
    unsigned int startFile, startLine, endFile, endLine;
};

std::vector<subtitle_t> parseSubs();

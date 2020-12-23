#include <string>
#include <fstream>
#include <sstream>

#include "SubParser.h"
#include "Utility.h"

void readFile(const char* filename, std::vector<std::string>& v) {
    std::ifstream file(filename);

    std::string l;
    while (std::getline(file, l)) {
        v.push_back(l);
    }
}

// assumes complete subtitle is given - 
// { 
// line+ 
// }
subtitle_t parseSub(std::vector<std::string>& file, int& i) {
    std::vector<line_t> lines;
    i++; // skip {

    // parse out the trigger info
    unsigned int startFile, startLine, endFile, endLine;
    std::stringstream(file[i]) >> startFile >> startLine >> endFile >> endLine;
    i++;

    for (; i < file.size() && file[i][0] != '}'; i++) {
        ULONGLONG start, length;
        std::string text;

        std::stringstream lineStream(file[i]);

        lineStream >> start >> length;

        std::getline(lineStream, text);

        lines.emplace_back(line_t(start, length, text));
    }

    subtitle_t track(lines, startFile, startLine, endFile, endLine);

    return track;
}

std::vector<subtitle_t> parseSubs() {
    const char* filename = Utility::getResourcePath(".\\todokanai\\subtitles");
    std::vector<std::string> lines;

    readFile(filename, lines);

    std::vector<subtitle_t> tracks;
    bool inSub = false;
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i][0] == '{') {
            subtitle_t track = parseSub(lines, i);
            tracks.push_back(track);
        }
    }

    return tracks;
}
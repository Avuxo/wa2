#include <string>
#include <fstream>
#include <sstream>

#include "SubParser.h"
#include "Utility.h"

void readFile(const char* filename, std::vector<std::string>& lines) {
    std::ifstream file(filename);

    // PLEASE NOTE: This log is just for the user, it doesn't actually change how the
    // program will execute. As it stands, there's no nice clean-exit for us since
    // this is technically DirectX 9 as far as the game is concerned.
    // please note the lack of a return here. This is intentional
    if (!file.is_open()) {
        MessageBoxA(0, "Todokanai Resources missing. Does the 'todokanai' directory contain a file named 'subtitles'?", "Todokanai Error", 0);
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
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
    int idOrFile, lineOrZero, endFile, endLine;
    std::stringstream(file[i]) >> idOrFile >> lineOrZero >> endFile >> endLine;

    i++;

    for (; i < file.size() && file[i][0] != '}'; i++) {
        ULONGLONG start, length;
        std::string text;

        std::stringstream lineStream(file[i]);

        lineStream >> start >> length;

        std::getline(lineStream, text);

        // remove ' ' at the start of string from parser since getline() will
        // just give the remainder of the stringstream and the last stream read
        // didn't trim the last space.
        text = text.substr(1, text.length() - 1);

        lines.emplace_back(line_t(start, length, text));
    }

    subtitle_t track(lines, idOrFile, lineOrZero, endFile, endLine);

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
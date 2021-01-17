#include "Utility.h"

LPCSTR Utility::getResourcePath(LPCSTR filename) {
    CHAR buf[MAX_PATH];

    DWORD result = GetFullPathNameA(
        filename,
        MAX_PATH,
        (LPSTR)buf, // this will end up null-terminated
        NULL
    );

    // result is 0 in the event of failure, otherwise length of written buffer
    if (!result) return "";

    return buf;
}

bool Utility::doesTodokanaiDirectoryExist() {
    return !(GetFileAttributesA(".\\todokanai") == INVALID_FILE_ATTRIBUTES);
}
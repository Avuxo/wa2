#pragma once
#define NOP 0x90
#define JMP (char)0xE9

char* installHook(char* src, char* dest, unsigned int len);
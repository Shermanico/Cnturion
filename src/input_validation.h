#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <color.h>
#include <security.h>

#define INPUT_CANCELLED -1

int getDigit(unsigned int *digit);
int getFloat(float *number);
int isExit(const char* str);

// Read a string from stdin, validate length, and sanitize for CSV safety.
// Returns 1 if input was received, 0 if empty, INPUT_CANCELLED if "exit" was typed.
int getValidatedString(char *output, int maxLen);

#endif

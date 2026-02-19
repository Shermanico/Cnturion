#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <color.h>
#include <security.h>

void getDigit(unsigned int *digit);
void getFloat(float *number);

// Read a string from stdin, validate length, and sanitize for CSV safety.
// Returns 1 if input was received, 0 if empty.
int getValidatedString(char *output, int maxLen);

#endif

#ifndef SECURITY_H
#define SECURITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SHA-256 produces 32 bytes = 64 hex characters + null terminator
#define HASH_HEX_LEN 65

// Hash a password string into a 64-char hex output
void hashPassword(const char *password, char *outputHex);

// Validate password policy: min 8 chars, 1 digit, 1 special char
// Returns 1 if valid, 0 if not
int validatePassword(const char *password);

// Sanitize a string: reject commas and newlines (CSV-unsafe)
// Returns 1 if clean, 0 if it was modified
int sanitizeString(char *input, int maxLen);

#endif

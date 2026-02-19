#ifndef SECURITY_H
#define SECURITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Argon2id encoded hash max length (includes $argon2id$v=...$m=...salt...hash)
#define ARGON2_ENCODED_LEN 256

// Legacy SHA-256 hex len (for migration detection)
#define HASH_HEX_LEN 65

// Salt: 16 raw bytes
#define SALT_RAW_LEN 16
#define SALT_LEN 33 // 32 hex chars + null (legacy compat)

// ---- Argon2id password hashing (primary) ----

// Hash a password using Argon2id. Output is a self-contained encoded string.
// Returns 1 on success, 0 on failure.
int hashPasswordArgon2(const char *password, char *encodedOut,
                       size_t encodedMax);

// Verify a password against an Argon2id encoded hash.
// Returns 1 if match, 0 if no match or error.
int verifyPasswordArgon2(const char *encoded, const char *password);

// ---- Legacy SHA-256 (kept for backward compatibility) ----

void hashPassword(const char *password, char *outputHex);
void hashPasswordWithSalt(const char *password, const char *salt,
                          char *outputHex);

// Generate a random 32-char hex salt from /dev/urandom
int generateSalt(char *saltOut);

// ---- Validation (all use POSIX regex internally) ----

// Generic regex match: returns 1 if input matches pattern, 0 otherwise
int matchesPattern(const char *input, const char *pattern);

// Validate password policy: min 8 chars, 1 uppercase, 1 digit, 1 special char
// Regex: ^(?=.*[A-Z])(?=.*[0-9])(?=.*[!-/:-@\[-`{-~]).{8,}$
int validatePassword(const char *password);

// Validate username: ^[a-zA-Z0-9_]{3,63}$
int validateUsername(const char *username);

// Validate product name: ^[a-zA-Z0-9 _\-\.]{1,127}$
int validateProductName(const char *name);

// Validate product category: ^[a-zA-Z0-9 _\-]{1,63}$
int validateProductCategory(const char *category);

// Sanitize a string: replace commas, quotes, and newlines (CSV-unsafe chars)
int sanitizeString(char *input, int maxLen);

#endif

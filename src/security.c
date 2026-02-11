/**
 * ============================================
 * security.c - Security Functions Implementation
 * ============================================
 * 
 * This file contains implementations of security-related functions
 * including password hashing and cryptographic utilities.
 * 
 * LEARNING: Implementing secure password hashing
 */

#include "security.h"
#include "utils.h"
#include <crypt.h>
#include <unistd.h>
#include <time.h>
#include <openssl/sha.h>
#include <ctype.h>

// ============================================
// PASSWORD HASHING FUNCTIONS
// ============================================

/**
 * Hash a password using SHA-512 with crypt()
 * 
 * Note: On Linux systems, we use crypt() with SHA-512 ($6$ prefix).
 * This is a strong password hashing method.
 * 
 * SECURITY: Never store plain text passwords!
 */
int hash_password(const char* password, char* hash_out, size_t hash_size) {
    if (password == NULL || hash_out == NULL || hash_size < BCRYPT_HASH_LENGTH + 1) {
        return ERR_INVALID_INPUT;
    }
    
    // Generate a random salt
    char salt[BCRYPT_SALT_LENGTH + 1];
    if (generate_salt(salt, sizeof(salt)) != ERR_NONE) {
        return ERR_UNKNOWN;
    }
    
    // Use crypt() with SHA-512 ($6$ prefix)
    // Format: $6$<salt>$<hash>
    char salt_prefix[BCRYPT_SALT_LENGTH + 10];
    snprintf(salt_prefix, sizeof(salt_prefix), "$6$%s$", salt);
    
    // Hash the password
    char* hashed = crypt(password, salt_prefix);
    if (hashed == NULL) {
        return ERR_UNKNOWN;
    }
    
    // Copy the hash to output
    size_t hash_len = strlen(hashed);
    if (hash_len >= hash_size) {
        return ERR_MEMORY;
    }
    
    strcpy(hash_out, hashed);
    
    return ERR_NONE;
}

/**
 * Verify a password against a stored hash
 * 
 * Uses constant-time comparison to prevent timing attacks.
 */
int verify_password(const char* password, const char* stored_hash) {
    if (password == NULL || stored_hash == NULL) {
        return 0;
    }
    
    // Hash the input password with the same salt
    char* hashed = crypt(password, stored_hash);
    if (hashed == NULL) {
        return 0;
    }
    
    // Use constant-time comparison
    return constant_time_compare(hashed, stored_hash);
}

/**
 * Generate a random salt
 * 
 * Uses cryptographically secure random number generator.
 */
int generate_salt(char* salt_out, size_t salt_size) {
    if (salt_out == NULL || salt_size < BCRYPT_SALT_LENGTH + 1) {
        return ERR_INVALID_INPUT;
    }
    
    // Generate random bytes
    unsigned char random_bytes[16];
    FILE* urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
        // Fallback to rand() if /dev/urandom is not available
        srand(time(NULL));
        for (int i = 0; i < 16; i++) {
            random_bytes[i] = rand() % 256;
        }
    } else {
        fread(random_bytes, 1, sizeof(random_bytes), urandom);
        fclose(urandom);
    }
    
    // Convert to base64-like characters
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";
    size_t charset_size = sizeof(charset) - 1;  // Exclude null terminator
    for (int i = 0; i < BCRYPT_SALT_LENGTH; ++i) {
        salt_out[i] = charset[random_bytes[i] % charset_size];
    }
    salt_out[BCRYPT_SALT_LENGTH] = '\0';
    
    return ERR_NONE;
}

// ============================================
// PASSWORD VALIDATION
// ============================================

int is_strong_password(const char* password) {
    if (is_null_or_empty(password)) {
        return 0;
    }
    
    size_t len = strlen(password);
    if (len < 8 || len > MAX_PASSWORD_LEN) {
        return 0;
    }
    
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("!@#$%^&*", password[i]) != NULL) has_special = 1;
    }
    
    return has_upper && has_lower && has_digit && has_special;
}

int get_password_strength(const char* password) {
    if (is_null_or_empty(password)) {
        return 0;
    }
    
    int score = 0;
    size_t len = strlen(password);
    
    // Length score (up to 40 points)
    if (len >= 8) score += 10;
    if (len >= 12) score += 10;
    if (len >= 16) score += 10;
    if (len >= 20) score += 10;
    
    // Character variety (up to 40 points)
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("!@#$%^&*", password[i]) != NULL) has_special = 1;
    }
    
    if (has_upper) score += 10;
    if (has_lower) score += 10;
    if (has_digit) score += 10;
    if (has_special) score += 10;
    
    // Bonus for variety (up to 20 points)
    int variety = has_upper + has_lower + has_digit + has_special;
    if (variety >= 4) score += 20;
    else if (variety >= 3) score += 15;
    else if (variety >= 2) score += 10;
    
    return (score > 100) ? 100 : score;
}

// ============================================
// CRYPTOGRAPHIC UTILITIES
// ============================================

int generate_secure_random(char* output, size_t length) {
    if (output == NULL || length == 0) {
        return ERR_INVALID_INPUT;
    }
    
    FILE* urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
        // Fallback to rand() if /dev/urandom is not available
        srand(time(NULL));
        for (size_t i = 0; i < length; i++) {
            output[i] = rand() % 256;
        }
        return ERR_NONE;
    }
    
    size_t bytes_read = fread(output, 1, length, urandom);
    fclose(urandom);
    
    if (bytes_read != length) {
        return ERR_UNKNOWN;
    }
    
    return ERR_NONE;
}

int generate_session_token(char* token_out, size_t token_size) {
    if (token_out == NULL || token_size < 33) {
        return ERR_INVALID_INPUT;
    }
    
    unsigned char random_bytes[16];
    if (generate_secure_random((char*)random_bytes, sizeof(random_bytes)) != ERR_NONE) {
        return ERR_UNKNOWN;
    }
    
    // Convert to hexadecimal string
    for (int i = 0; i < 16; i++) {
        snprintf(token_out + (i * 2), 3, "%02x", random_bytes[i]);
    }
    token_out[32] = '\0';
    
    return ERR_NONE;
}

// ============================================
// DATA SANITIZATION
// ============================================

int sanitize_string(const char* input, char* output, size_t output_size) {
    if (input == NULL || output == NULL || output_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        // Keep only printable ASCII characters
        if (isprint(input[i]) && input[i] < 127) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    
    return ERR_NONE;
}

int escape_sql_chars(const char* input, char* output, size_t output_size) {
    if (input == NULL || output == NULL || output_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    const char* chars_to_escape = "'\"\\;--";
    size_t j = 0;
    
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        if (strchr(chars_to_escape, input[i]) != NULL) {
            // Escape the character
            if (j + 2 >= output_size) {
                break;
            }
            output[j++] = '\\';
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
    
    return ERR_NONE;
}

// ============================================
// HASHING FOR NON-PASSWORD DATA
// ============================================

int hash_sha256(const char* data, size_t data_len, char* hash_out) {
    if (data == NULL || hash_out == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)data, data_len, hash);
    
    // Convert to hexadecimal string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_out + (i * 2), "%02x", hash[i]);
    }
    hash_out[SHA256_DIGEST_LENGTH * 2] = '\0';
    
    return ERR_NONE;
}

int constant_time_compare(const char* hash1, const char* hash2) {
    if (hash1 == NULL || hash2 == NULL) {
        return 0;
    }
    
    size_t len1 = strlen(hash1);
    size_t len2 = strlen(hash2);
    
    if (len1 != len2) {
        return 0;
    }
    
    volatile int result = 0;
    for (size_t i = 0; i < len1; i++) {
        result |= hash1[i] ^ hash2[i];
    }
    
    return result == 0;
}

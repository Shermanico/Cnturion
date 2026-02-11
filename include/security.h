/**
 * ============================================
 * security.h - Security Functions Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding password hashing
 * - Cryptographic security principles
 * - Salt generation
 * - Why we never store plain text passwords
 * 
 * KEY CONCEPTS:
 * 1. Hashing: One-way transformation of data
 * 2. Salt: Random data added to password before hashing
 * 3. bcrypt: Password hashing algorithm designed for security
 * 4. Work Factor: Makes hashing slower to prevent brute force
 * 
 * SECURITY NOTES:
 * - NEVER store plain text passwords
 * - NEVER implement your own crypto
 * - ALWAYS use proven algorithms (bcrypt, Argon2, PBKDF2)
 * - Salts prevent rainbow table attacks
 * - Slow hashing prevents brute force attacks
 * 
 * WHY BCRYPT?
 * - Designed specifically for passwords
 * - Includes salt automatically
 * - Adjustable work factor (computational cost)
 * - Resistant to GPU/ASIC attacks
 */

#ifndef SECURITY_H
#define SECURITY_H

#include "common.h"

// ============================================
// PASSWORD HASHING CONSTANTS
// ============================================

/**
 * BCRYPT work factor
 * 
 * Higher values = more secure but slower
 * 12 is a good balance (2^12 = 4096 iterations)
 * Each increment doubles the time required
 */
#define BCRYPT_WORK_FACTOR 12

/**
 * Maximum hash length for bcrypt
 * bcrypt produces 60-character hashes
 */
#define BCRYPT_HASH_LENGTH 60

/**
 * Salt length (bcrypt includes salt in hash)
 */
#define BCRYPT_SALT_LENGTH 22

// ============================================
// PASSWORD HASHING FUNCTIONS
// ============================================

/**
 * Hash a password using bcrypt
 * 
 * This function takes a plain text password and returns
 * a bcrypt hash. The hash includes the salt automatically.
 * 
 * IMPORTANT: Never store plain text passwords!
 * 
 * @param password Plain text password to hash
 * @param hash_out Buffer to store the resulting hash
 * @param hash_size Size of the hash buffer (should be at least BCRYPT_HASH_LENGTH + 1)
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: bcrypt is a one-way function - you cannot decrypt it
 * SECURITY: The hash includes the salt, so we don't need to store it separately
 * 
 * Example:
 *   char hash[BCRYPT_HASH_LENGTH + 1];
 *   hash_password("mypassword", hash, sizeof(hash));
 *   // hash now contains something like: "$2b$12$..."
 */
int hash_password(const char* password, char* hash_out, size_t hash_size);

/**
 * Verify a password against a stored hash
 * 
 * This function checks if a plain text password matches
 * a stored bcrypt hash. It uses constant-time comparison
 * to prevent timing attacks.
 * 
 * @param password Plain text password to verify
 * @param stored_hash The bcrypt hash to compare against
 * @return 1 if password matches, 0 if it doesn't, negative on error
 * 
 * LEARNING: We hash the input password and compare hashes
 * SECURITY: Constant-time comparison prevents timing attacks
 * 
 * Example:
 *   if (verify_password(user_input, stored_hash)) {
 *       // Password is correct
 *   } else {
 *       // Password is incorrect
 *   }
 */
int verify_password(const char* password, const char* stored_hash);

/**
 * Generate a random salt
 * 
 * Note: bcrypt includes the salt in the hash automatically,
 * so this function is mainly for educational purposes or
 * if using other hashing algorithms.
 * 
 * @param salt_out Buffer to store the salt
 * @param salt_size Size of the salt buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: Salts prevent rainbow table attacks
 * SECURITY: Use cryptographically secure random number generator
 */
int generate_salt(char* salt_out, size_t salt_size);

// ============================================
// PASSWORD VALIDATION
// ============================================

/**
 * Check if password meets complexity requirements
 * 
 * Password policy:
 * - Minimum 8 characters
 * - At least one uppercase letter
 * - At least one lowercase letter
 * - At least one digit
 * - At least one special character (!@#$%^&*)
 * - Maximum 128 characters
 * 
 * @param password Password to validate
 * @return 1 if password meets requirements, 0 otherwise
 * 
 * SECURITY: Strong passwords prevent brute force attacks
 */
int is_strong_password(const char* password);

/**
 * Get password strength score
 * 
 * Returns a score from 0-100 based on password strength.
 * 
 * @param password Password to evaluate
 * @return Strength score (0-100)
 */
int get_password_strength(const char* password);

// ============================================
// CRYPTOGRAPHIC UTILITIES
// ============================================

/**
 * Generate a secure random string
 * 
 * Uses cryptographically secure random number generator.
 * Useful for generating session tokens, API keys, etc.
 * 
 * @param output Buffer to store the random string
 * @param length Length of the random string to generate
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Always use cryptographically secure RNG for security-sensitive data
 */
int generate_secure_random(char* output, size_t length);

/**
 * Generate a session token
 * 
 * Creates a cryptographically secure random token
 * that can be used for session management.
 * 
 * @param token_out Buffer to store the token
 * @param token_size Size of the token buffer
 * @return ERR_NONE on success, error code on failure
 */
int generate_session_token(char* token_out, size_t token_size);

// ============================================
// DATA SANITIZATION
// ============================================

/**
 * Sanitize a string for safe storage
 * 
 * Removes or escapes potentially dangerous characters.
 * This is a defense-in-depth measure.
 * 
 * @param input String to sanitize
 * @param output Buffer to store sanitized string
 * @param output_size Size of the output buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Always sanitize user input
 * NOTE: This is additional protection, prepared statements are primary defense
 */
int sanitize_string(const char* input, char* output, size_t output_size);

/**
 * Escape SQL special characters
 * 
 * Escapes characters that could be used in SQL injection.
 * This is a backup defense - always use prepared statements!
 * 
 * @param input String to escape
 * @param output Buffer to store escaped string
 * @param output_size Size of the output buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Prepared statements are the primary defense against SQL injection
 * This function provides additional protection
 */
int escape_sql_chars(const char* input, char* output, size_t output_size);

// ============================================
// HASHING FOR NON-PASSWORD DATA
// ============================================

/**
 * Hash data using SHA-256
 * 
 * For non-password data where a fast hash is acceptable.
 * DO NOT use for password hashing!
 * 
 * @param data Data to hash
 * @param data_len Length of data
 * @param hash_out Buffer to store hash (must be at least 65 bytes for hex + null)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: SHA-256 is good for integrity checking, NOT for passwords
 * Use bcrypt for passwords instead
 */
int hash_sha256(const char* data, size_t data_len, char* hash_out);

/**
 * Compare two hashes in constant time
 * 
 * Prevents timing attacks when comparing hashes.
 * 
 * @param hash1 First hash
 * @param hash2 Second hash
 * @return 1 if equal, 0 if not equal
 * 
 * SECURITY: Constant-time comparison prevents timing attacks
 */
int constant_time_compare(const char* hash1, const char* hash2);

#endif // SECURITY_H

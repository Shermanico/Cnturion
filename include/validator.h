/**
 * ============================================
 * validator.h - Input Validation Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding input validation
 * - Preventing buffer overflows
 * - Regular expressions (regex) in C
 * - Whitelist vs blacklist validation
 * 
 * KEY CONCEPTS:
 * 1. Input Validation: Checking that user input is safe and correct
 * 2. Buffer Overflow: Writing past array boundaries (security vulnerability)
 * 3. Whitelist Validation: Only allow known good characters/values
 * 4. Blacklist Validation: Block known bad characters/values (less secure)
 * 
 * SECURITY NOTES:
 * - NEVER trust user input
 * - ALWAYS validate on the server side (not just client)
 * - Use whitelist validation (allow only known good characters)
 * - Validate ALL inputs before processing
 * - Use safe string functions (strncpy, not strcpy)
 * 
 * VALIDATION STRATEGY:
 * - Type checking (is it a string, number, etc.)
 * - Length checking (is it within allowed range)
 * - Format checking (does it match expected pattern)
 * - Range checking (is value within valid range)
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "common.h"

// ============================================
// VALIDATION RESULT
// ============================================

/**
 * Validation result structure
 * Contains detailed information about validation failures
 */
typedef struct {
    int is_valid;              // 1 = valid, 0 = invalid
    ErrorCode error_code;      // Error code if invalid
    char error_message[MAX_ERROR_MSG];  // Human-readable error
} ValidationResult;

// ============================================
// USERNAME VALIDATION
// ============================================

/**
 * Validate username format
 * 
 * Username requirements:
 * - 3 to 20 characters
 * - Only alphanumeric characters and underscore
 * - Must start with a letter
 * 
 * @param username Username to validate
 * @return ValidationResult with validation status
 * 
 * LEARNING: Whitelist validation - only allow known good characters
 * SECURITY: Prevents injection attacks through username field
 */
ValidationResult validate_username(const char* username);

/**
 * Check if username contains only allowed characters
 * 
 * @param username Username to check
 * @return 1 if valid characters, 0 otherwise
 */
int is_valid_username_chars(const char* username);

// ============================================
// EMAIL VALIDATION
// ============================================

/**
 * Validate email format
 * 
 * Email requirements:
 * - Standard email format (user@domain.tld)
 * - Maximum 100 characters
 * - Contains @ symbol
 * - Contains at least one . after @
 * 
 * @param email Email address to validate
 * @return ValidationResult with validation status
 * 
 * LEARNING: Basic email format validation
 * SECURITY: Prevents injection through email field
 */
ValidationResult validate_email(const char* email);

/**
 * Check if email format is valid
 * 
 * @param email Email to check
 * @return 1 if valid format, 0 otherwise
 */
int is_valid_email_format(const char* email);

// ============================================
// PASSWORD VALIDATION
// ============================================

/**
 * Validate password against security policy
 * 
 * Password requirements:
 * - 8 to 128 characters
 * - At least one uppercase letter
 * - At least one lowercase letter
 * - At least one digit
 * - At least one special character (!@#$%^&*)
 * 
 * @param password Password to validate
 * @return ValidationResult with validation status
 * 
 * SECURITY: Strong passwords prevent brute force attacks
 */
ValidationResult validate_password(const char* password);

/**
 * Check password length
 * 
 * @param password Password to check
 * @return 1 if valid length, 0 otherwise
 */
int is_valid_password_length(const char* password);

// ============================================
// PRODUCT VALIDATION
// ============================================

/**
 * Validate product name
 * 
 * Requirements:
 * - 1 to 100 characters
 * - Not empty
 * - No special control characters
 * 
 * @param name Product name to validate
 * @return ValidationResult with validation status
 */
ValidationResult validate_product_name(const char* name);

/**
 * Validate product description
 * 
 * Requirements:
 * - 0 to 500 characters (optional)
 * - No special control characters
 * 
 * @param description Description to validate
 * @return ValidationResult with validation status
 */
ValidationResult validate_product_description(const char* description);

/**
 * Validate SKU (Stock Keeping Unit)
 * 
 * Requirements:
 * - 1 to 50 characters
 * - Alphanumeric and hyphens/underscores only
 * - Unique per user (enforced at database level)
 * 
 * @param sku SKU to validate
 * @return ValidationResult with validation status
 */
ValidationResult validate_sku(const char* sku);

/**
 * Validate price string and convert to double
 * 
 * Requirements:
 * - Valid positive number
 * - Maximum 2 decimal places
 * - Not negative
 * 
 * @param price_str Price string to validate
 * @param price_out Output pointer for converted price
 * @return ValidationResult with validation status
 * 
 * LEARNING: String to number conversion with validation
 */
ValidationResult validate_price(const char* price_str, double* price_out);

/**
 * Validate quantity string and convert to int
 * 
 * Requirements:
 * - Valid non-negative integer
 * - Not negative
 * 
 * @param quantity_str Quantity string to validate
 * @param quantity_out Output pointer for converted quantity
 * @return ValidationResult with validation status
 */
ValidationResult validate_quantity(const char* quantity_str, int* quantity_out);

/**
 * Validate product category
 * 
 * Requirements:
 * - 0 to 50 characters (optional)
 * - Alphanumeric and spaces only
 * 
 * @param category Category to validate
 * @return ValidationResult with validation status
 */
ValidationResult validate_category(const char* category);

// ============================================
// GENERAL VALIDATION FUNCTIONS
// ============================================

/**
 * Validate string length
 * 
 * @param str String to check
 * @param min_len Minimum allowed length
 * @param max_len Maximum allowed length
 * @return 1 if valid length, 0 otherwise
 * 
 * SECURITY: Prevents buffer overflows
 */
int is_valid_length(const char* str, size_t min_len, size_t max_len);

/**
 * Check if string is empty or only whitespace
 * 
 * @param str String to check
 * @return 1 if empty/whitespace, 0 otherwise
 */
int is_empty(const char* str);

/**
 * Check if string contains only alphanumeric characters
 * 
 * @param str String to check
 * @return 1 if alphanumeric only, 0 otherwise
 */
int is_alphanumeric(const char* str);

/**
 * Check if string contains only digits
 * 
 * @param str String to check
 * @return 1 if digits only, 0 otherwise
 */
int is_numeric(const char* str);

/**
 * Check if string contains only printable ASCII characters
 * 
 * @param str String to check
 * @return 1 if printable ASCII only, 0 otherwise
 * 
 * SECURITY: Prevents control character injection
 */
int is_printable_ascii(const char* str);

/**
 * Check if string contains SQL injection patterns
 * 
 * This is a defense-in-depth measure. The primary defense
 * is using prepared statements.
 * 
 * @param str String to check
 * @return 1 if suspicious patterns found, 0 otherwise
 * 
 * SECURITY: Additional protection against SQL injection
 * NOTE: Prepared statements are the primary defense!
 */
int contains_sql_injection(const char* str);

// ============================================
// SANITIZATION FUNCTIONS
// ============================================

/**
 * Trim whitespace from string
 * 
 * Removes leading and trailing whitespace.
 * 
 * @param str String to trim (modified in place)
 */
void trim_whitespace(char* str);

/**
 * Remove dangerous characters from string
 * 
 * Removes characters that could be used in injection attacks.
 * 
 * @param str String to sanitize (modified in place)
 * 
 * SECURITY: Defense-in-depth measure
 */
void sanitize_input(char* str);

/**
 * Convert string to lowercase
 * 
 * @param str String to convert (modified in place)
 */
void to_lowercase(char* str);

// ============================================
// SAFE STRING FUNCTIONS
// ============================================

/**
 * Safe string copy
 * 
 * Copies source to destination with length checking.
 * Always null-terminates the destination.
 * 
 * @param dest Destination buffer
 * @param src Source string
 * @param dest_size Size of destination buffer
 * @return Number of characters copied (excluding null terminator)
 * 
 * LEARNING: Prevents buffer overflows
 * SECURITY: Always use safe string functions!
 */
size_t safe_strcpy(char* dest, const char* src, size_t dest_size);

/**
 * Safe string concatenation
 * 
 * Appends source to destination with length checking.
 * Always null-terminates the destination.
 * 
 * @param dest Destination buffer
 * @param src Source string to append
 * @param dest_size Size of destination buffer
 * @return Number of characters appended (excluding null terminator)
 * 
 * SECURITY: Prevents buffer overflows
 */
size_t safe_strcat(char* dest, const char* src, size_t dest_size);

#endif // VALIDATOR_H

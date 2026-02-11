/**
 * ============================================
 * utils.h - Utility Functions Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding helper functions
 * - String manipulation
 * - Memory management
 * - Error handling patterns
 * 
 * KEY CONCEPTS:
 * 1. Utility Functions: Reusable helper functions
 * 2. String Manipulation: Working with text in C
 * 3. Memory Management: Allocating and freeing memory
 * 4. Error Handling: Consistent error reporting
 * 
 * SECURITY NOTES:
 * - Always check buffer sizes
 * - Always check return values
 * - Free allocated memory
 * - Use safe string functions
 * - Validate pointers before use
 * 
 * UTILITY CATEGORIES:
 * - String utilities
 * - Memory utilities
 * - Time utilities
 * - Error handling utilities
 * - Conversion utilities
 */

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

// ============================================
// STRING UTILITIES
// ============================================

/**
 * Calculate string length safely
 * 
 * Like strlen but with a maximum length to prevent
 * reading past buffer boundaries.
 * 
 * @param str String to measure
 * @param max_len Maximum length to check
 * @return Length of string (up to max_len)
 * 
 * LEARNING: Safe string length calculation
 * SECURITY: Prevents buffer over-read
 */
size_t safe_strlen(const char* str, size_t max_len);

/**
 * Copy string with length check
 * 
 * Copies source to destination with bounds checking.
 * Always null-terminates the destination.
 * 
 * @param dest Destination buffer
 * @param src Source string
 * @param dest_size Size of destination buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Prevents buffer overflow
 */
int safe_str_copy(char* dest, const char* src, size_t dest_size);

/**
 * Concatenate strings with length check
 * 
 * Appends source to destination with bounds checking.
 * Always null-terminates the destination.
 * 
 * @param dest Destination buffer
 * @param src Source string to append
 * @param dest_size Size of destination buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Prevents buffer overflow
 */
int safe_str_cat(char* dest, const char* src, size_t dest_size);

/**
 * Compare strings safely
 * 
 * Like strcmp but with length limits.
 * 
 * @param str1 First string
 * @param str2 Second string
 * @param max_len Maximum length to compare
 * @return 0 if equal, negative if str1 < str2, positive if str1 > str2
 */
int safe_str_compare(const char* str1, const char* str2, size_t max_len);

/**
 * Check if string starts with prefix
 * 
 * @param str String to check
 * @param prefix Prefix to look for
 * @return 1 if starts with prefix, 0 otherwise
 */
int str_starts_with(const char* str, const char* prefix);

/**
 * Check if string ends with suffix
 * 
 * @param str String to check
 * @param suffix Suffix to look for
 * @return 1 if ends with suffix, 0 otherwise
 */
int str_ends_with(const char* str, const char* suffix);

/**
 * Find substring in string
 * 
 * @param str String to search
 * @param substr Substring to find
 * @return Pointer to first occurrence, or NULL if not found
 */
const char* str_find(const char* str, const char* substr);

/**
 * Replace all occurrences of substring
 * 
 * @param str String to modify
 * @param old_str Substring to replace
 * @param new_str Replacement string
 * @param max_size Maximum size of output buffer
 * @return ERR_NONE on success, error code on failure
 */
int str_replace(char* str, const char* old_str, const char* new_str, size_t max_size);

/**
 * Convert string to lowercase
 * 
 * @param str String to convert (modified in place)
 */
void str_to_lower(char* str);

/**
 * Convert string to uppercase
 * 
 * @param str String to convert (modified in place)
 */
void str_to_upper(char* str);

/**
 * Trim whitespace from both ends
 * 
 * @param str String to trim (modified in place)
 */
void str_trim(char* str);

/**
 * Trim leading whitespace
 * 
 * @param str String to trim (modified in place)
 */
void str_trim_left(char* str);

/**
 * Trim trailing whitespace
 * 
 * @param str String to trim (modified in place)
 */
void str_trim_right(char* str);

// ============================================
// MEMORY UTILITIES
// ============================================

/**
 * Safe memory allocation
 * 
 * Allocates memory and checks for failure.
 * 
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 * 
 * LEARNING: Safe memory allocation
 * SECURITY: Always check allocation result
 */
void* safe_malloc(size_t size);

/**
 * Safe memory reallocation
 * 
 * Reallocates memory and checks for failure.
 * 
 * @param ptr Pointer to existing memory
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* safe_realloc(void* ptr, size_t size);

/**
 * Safe memory deallocation
 * 
 * Frees memory and sets pointer to NULL.
 * 
 * @param ptr Pointer to pointer to memory
 * 
 * LEARNING: Prevents use-after-free bugs
 */
void safe_free(void** ptr);

/**
 * Zero out memory securely
 * 
 * Overwrites memory with zeros.
 * Useful for clearing sensitive data.
 * 
 * @param ptr Pointer to memory
 * @param size Size of memory
 * 
 * SECURITY: Clears sensitive data from memory
 */
void secure_zero(void* ptr, size_t size);

// ============================================
// TIME UTILITIES
// ============================================

/**
 * Get current timestamp as string
 * 
 * Formats current time as ISO 8601 string.
 * 
 * @param buffer Buffer to store timestamp
 * @param size Size of buffer
 * @return ERR_NONE on success, error code on failure
 */
int get_timestamp(char* buffer, size_t size);

/**
 * Get current time as time_t
 * 
 * @return Current time as time_t value
 */
time_t get_current_time(void);

/**
 * Format time as string
 * 
 * @param time Time value to format
 * @param buffer Buffer to store formatted time
 * @param size Size of buffer
 * @param format Format string (strftime format)
 * @return ERR_NONE on success, error code on failure
 */
int format_time(time_t time, char* buffer, size_t size, const char* format);

/**
 * Parse time from string
 * 
 * @param str String to parse
 * @param format Format string (strptime format)
 * @return Parsed time as time_t, or -1 on error
 */
time_t parse_time(const char* str, const char* format);

/**
 * Get time difference in seconds
 * 
 * @param start Start time
 * @param end End time
 * @return Difference in seconds
 */
long time_diff_seconds(time_t start, time_t end);

// ============================================
// CONVERSION UTILITIES
// ============================================

/**
 * Convert string to integer safely
 * 
 * @param str String to convert
 * @param value Output parameter for converted value
 * @return ERR_NONE on success, error code on failure
 */
int str_to_int(const char* str, int* value);

/**
 * Convert string to double safely
 * 
 * @param str String to convert
 * @param value Output parameter for converted value
 * @return ERR_NONE on success, error code on failure
 */
int str_to_double(const char* str, double* value);

/**
 * Convert integer to string
 * 
 * @param value Value to convert
 * @param buffer Buffer to store string
 * @param size Size of buffer
 * @return ERR_NONE on success, error code on failure
 */
int int_to_str(int value, char* buffer, size_t size);

/**
 * Convert double to string
 * 
 * @param value Value to convert
 * @param buffer Buffer to store string
 * @param size Size of buffer
 * @param decimals Number of decimal places
 * @return ERR_NONE on success, error code on failure
 */
int double_to_str(double value, char* buffer, size_t size, int decimals);

/**
 * Convert boolean to string
 * 
 * @param value Boolean value
 * @return "true" or "false"
 */
const char* bool_to_str(int value);

/**
 * Convert string to boolean
 * 
 * @param str String to convert
 * @return 1 for true, 0 for false
 */
int str_to_bool(const char* str);

// ============================================
// VALIDATION UTILITIES
// ============================================

/**
 * Check if pointer is null
 * 
 * @param ptr Pointer to check
 * @return 1 if null, 0 otherwise
 */
int is_null(const void* ptr);

/**
 * Check if string is null or empty
 * 
 * @param str String to check
 * @return 1 if null or empty, 0 otherwise
 */
int is_null_or_empty(const char* str);

/**
 * Check if value is within range
 * 
 * @param value Value to check
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return 1 if in range, 0 otherwise
 */
int is_in_range(int value, int min, int max);

/**
 * Check if double is within range
 * 
 * @param value Value to check
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return 1 if in range, 0 otherwise
 */
int is_in_range_double(double value, double min, double max);

// ============================================
// ERROR HANDLING UTILITIES
// ============================================

/**
 * Get error message for error code
 * 
 * @param code Error code
 * @return Human-readable error message
 */
const char* get_error_message(ErrorCode code);

/**
 * Print error message to stderr
 * 
 * @param code Error code
 * @param context Additional context (can be NULL)
 */
void print_error(ErrorCode code, const char* context);

/**
 * Handle error with logging
 * 
 * Logs error and optionally exits.
 * 
 * @param code Error code
 * @param context Additional context
 * @param should_exit Whether to exit program
 */
void handle_error(ErrorCode code, const char* context, int should_exit);

// ============================================
// FILE UTILITIES
// ============================================

/**
 * Check if file exists
 * 
 * @param path Path to file
 * @return 1 if exists, 0 otherwise
 */
int file_exists(const char* path);

/**
 * Get file size
 * 
 * @param path Path to file
 * @return File size in bytes, or -1 on error
 */
long get_file_size(const char* path);

/**
 * Read file into buffer
 * 
 * @param path Path to file
 * @param buffer Buffer to store file contents
 * @param buffer_size Size of buffer
 * @return Number of bytes read, or negative on error
 */
int read_file(const char* path, char* buffer, size_t buffer_size);

/**
 * Write buffer to file
 * 
 * @param path Path to file
 * @param buffer Buffer to write
 * @param size Number of bytes to write
 * @return Number of bytes written, or negative on error
 */
int write_file(const char* path, const char* buffer, size_t size);

/**
 * Delete file
 * 
 * @param path Path to file
 * @return ERR_NONE on success, error code on failure
 */
int delete_file(const char* path);

// ============================================
// DEBUG UTILITIES
// ============================================

/**
 * Print debug message
 * 
 * Only prints if DEBUG is defined.
 * 
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void debug_printf(const char* format, ...);

/**
 * Print hex dump of buffer
 * 
 * Useful for debugging binary data.
 * 
 * @param data Buffer to dump
 * @param size Size of buffer
 */
void hex_dump(const void* data, size_t size);

#endif // UTILS_H

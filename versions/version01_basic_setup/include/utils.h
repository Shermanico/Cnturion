/**
 * ============================================
 * utils.h - Utility Functions Interface
 * ============================================
 * 
 * This file declares utility functions used throughout
 * the application.
 * 
 * LEARNING: Creating reusable helper functions
 */

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

// ============================================
// STRING UTILITIES
// ============================================

/**
 * Safely get string length with maximum limit
 * 
 * @param str String to measure
 * @param max_len Maximum length to check
 * @return Length of string (capped at max_len)
 */
size_t safe_strlen(const char* str, size_t max_len);

/**
 * Safe string copy with buffer overflow protection
 * 
 * @param dest Destination buffer
 * @param src Source string
 * @param dest_size Size of destination buffer
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode safe_strcpy(char* dest, const char* src, size_t dest_size);

/**
 * Safe string concatenation with buffer overflow protection
 * 
 * @param dest Destination buffer
 * @param src String to append
 * @param dest_size Size of destination buffer
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode safe_strcat(char* dest, const char* src, size_t dest_size);

// ============================================
// MEMORY UTILITIES
// ============================================

/**
 * Safe memory allocation with error checking
 * 
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, NULL on failure
 */
void* safe_malloc(size_t size);

/**
 * Safe memory reallocation with error checking
 * 
 * @param ptr Pointer to existing memory block
 * @param size New size in bytes
 * @return Pointer to reallocated memory, NULL on failure
 */
void* safe_realloc(void* ptr, size_t size);

/**
 * Safe memory deallocation (handles NULL pointers)
 * 
 * @param ptr Pointer to memory to free
 */
void safe_free(void** ptr);

// ============================================
// ERROR HANDLING
// ============================================

/**
 * Get user-friendly error message for error code
 * 
 * @param code Error code
 * @return String describing the error
 */
const char* get_error_message(ErrorCode code);

/**
 * Print error message to stderr
 * 
 * @param code Error code
 * @param context Context information
 */
void print_error(ErrorCode code, const char* context);

#endif // UTILS_H

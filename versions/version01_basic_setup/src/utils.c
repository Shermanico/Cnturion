/**
 * ============================================
 * utils.c - Utility Functions Implementation
 * ============================================
 * 
 * This file contains implementations of utility functions
 * used throughout the application.
 * 
 * LEARNING: Implementing reusable helper functions
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// STRING UTILITIES
// ============================================

size_t safe_strlen(const char* str, size_t max_len) {
    if (str == NULL) {
        return 0;
    }
    
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}

ErrorCode safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    size_t src_len = safe_strlen(src, dest_size - 1);
    if (src_len >= dest_size) {
        return ERR_MEMORY;  // Would overflow
    }
    
    memcpy(dest, src, src_len);
    dest[src_len] = '\0';
    
    return ERR_NONE;
}

ErrorCode safe_strcat(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    size_t dest_len = safe_strlen(dest, dest_size);
    size_t src_len = safe_strlen(src, dest_size - dest_len - 1);
    
    if (dest_len + src_len >= dest_size) {
        return ERR_MEMORY;  // Would overflow
    }
    
    memcpy(dest + dest_len, src, src_len);
    dest[dest_len + src_len] = '\0';
    
    return ERR_NONE;
}

// ============================================
// MEMORY UTILITIES
// ============================================

void* safe_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    if (size == 0) {
        if (ptr != NULL) {
            free(ptr);
        }
        return NULL;
    }
    
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL && size > 0) {
        fprintf(stderr, "Memory reallocation failed\n");
        return NULL;
    }
    
    return new_ptr;
}

void safe_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

// ============================================
// ERROR HANDLING
// ============================================

const char* get_error_message(ErrorCode code) {
    switch (code) {
        case ERR_NONE:
            return "Success";
        case ERR_INVALID_INPUT:
            return "Invalid input provided";
        case ERR_AUTH_FAILED:
            return "Authentication failed";
        case ERR_NOT_AUTHORIZED:
            return "Not authorized for this action";
        case ERR_DB_ERROR:
            return "Database error occurred";
        case ERR_NOT_FOUND:
            return "Resource not found";
        case ERR_DUPLICATE:
            return "Duplicate entry exists";
        case ERR_MEMORY:
            return "Memory allocation failed";
        case ERR_FILE_IO:
            return "File input/output error";
        case ERR_CONFIG:
            return "Configuration error";
        case ERR_UNKNOWN:
        default:
            return "Unknown error occurred";
    }
}

void print_error(ErrorCode code, const char* context) {
    fprintf(stderr, "Error");
    if (context != NULL) {
        fprintf(stderr, " (%s)", context);
    }
    fprintf(stderr, ": %s\n", get_error_message(code));
}

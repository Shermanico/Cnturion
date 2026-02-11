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

#define _GNU_SOURCE
#include "utils.h"
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

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

int safe_str_copy(char* dest, const char* src, size_t dest_size) {
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

int safe_str_cat(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    size_t dest_len = safe_strlen(dest, dest_size - 1);
    size_t src_len = safe_strlen(src, dest_size - dest_len - 1);
    
    if (dest_len + src_len >= dest_size) {
        return ERR_MEMORY;  // Would overflow
    }
    
    memcpy(dest + dest_len, src, src_len);
    dest[dest_len + src_len] = '\0';
    
    return ERR_NONE;
}

int safe_str_compare(const char* str1, const char* str2, size_t max_len) {
    if (str1 == NULL && str2 == NULL) {
        return 0;
    }
    if (str1 == NULL) {
        return -1;
    }
    if (str2 == NULL) {
        return 1;
    }
    
    return strncmp(str1, str2, max_len);
}

int str_starts_with(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    
    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return 1;  // Empty prefix matches everything
    }
    
    return strncmp(str, prefix, prefix_len) == 0;
}

int str_ends_with(const char* str, const char* suffix) {
    if (str == NULL || suffix == NULL) {
        return 0;
    }
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return 0;
    }
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

const char* str_find(const char* str, const char* substr) {
    if (str == NULL || substr == NULL) {
        return NULL;
    }
    
    return strstr(str, substr);
}

int str_replace(char* str, const char* old_str, const char* new_str, size_t max_size) {
    if (str == NULL || old_str == NULL || new_str == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* pos = strstr(str, old_str);
    if (pos == NULL) {
        return ERR_NONE;  // Nothing to replace
    }
    
    size_t old_len = strlen(old_str);
    size_t new_len = strlen(new_str);
    size_t str_len = strlen(str);
    
    if (str_len - old_len + new_len >= max_size) {
        return ERR_MEMORY;  // Would overflow
    }
    
    // Move the rest of the string
    memmove(pos + new_len, pos + old_len, strlen(pos + old_len) + 1);
    
    // Copy new string
    memcpy(pos, new_str, new_len);
    
    return ERR_NONE;
}

void str_to_lower(char* str) {
    if (str == NULL) {
        return;
    }
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}

void str_to_upper(char* str) {
    if (str == NULL) {
        return;
    }
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

void str_trim(char* str) {
    if (str == NULL) {
        return;
    }
    
    str_trim_left(str);
    str_trim_right(str);
}

void str_trim_left(char* str) {
    if (str == NULL) {
        return;
    }
    
    size_t i = 0;
    while (str[i] != '\0' && isspace(str[i])) {
        i++;
    }
    
    if (i > 0) {
        memmove(str, str + i, strlen(str + i) + 1);
    }
}

void str_trim_right(char* str) {
    if (str == NULL) {
        return;
    }
    
    size_t len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        len--;
    }
    str[len] = '\0';
}

// ============================================
// MEMORY UTILITIES
// ============================================

void* safe_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);  // Initialize to zero
    }
    
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    return realloc(ptr, size);
}

void safe_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

void secure_zero(void* ptr, size_t size) {
    if (ptr != NULL && size > 0) {
        memset(ptr, 0, size);
        // Force compiler to not optimize this away
        volatile unsigned char* p = ptr;
        for (size_t i = 0; i < size; i++) {
            p[i] = 0;
        }
    }
}

// ============================================
// TIME UTILITIES
// ============================================

int get_timestamp(char* buffer, size_t size) {
    if (buffer == NULL || size < 20) {
        return ERR_INVALID_INPUT;
    }
    
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    if (tm_info == NULL) {
        return ERR_UNKNOWN;
    }
    
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
    return ERR_NONE;
}

time_t get_current_time(void) {
    return time(NULL);
}

int format_time(time_t time, char* buffer, size_t size, const char* format) {
    if (buffer == NULL || format == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    struct tm* tm_info = localtime(&time);
    if (tm_info == NULL) {
        return ERR_UNKNOWN;
    }
    
    if (strftime(buffer, size, format, tm_info) == 0) {
        return ERR_MEMORY;
    }
    
    return ERR_NONE;
}

time_t parse_time(const char* str, const char* format) {
    if (str == NULL || format == NULL) {
        return -1;
    }
    
    struct tm tm_info;
    memset(&tm_info, 0, sizeof(tm_info));
    
    char* result = strptime(str, format, &tm_info);
    if (result == NULL) {
        return -1;
    }
    
    return mktime(&tm_info);
}

long time_diff_seconds(time_t start, time_t end) {
    return (long)difftime(end, start);
}

// ============================================
// CONVERSION UTILITIES
// ============================================

int str_to_int(const char* str, int* value) {
    if (str == NULL || value == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* endptr;
    errno = 0;
    long result = strtol(str, &endptr, 10);
    
    if (errno == ERANGE || result < INT_MIN || result > INT_MAX) {
        return ERR_INVALID_INPUT;
    }
    
    if (endptr == str || *endptr != '\0') {
        return ERR_INVALID_INPUT;
    }
    
    *value = (int)result;
    return ERR_NONE;
}

int str_to_double(const char* str, double* value) {
    if (str == NULL || value == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* endptr;
    errno = 0;
    double result = strtod(str, &endptr);
    
    if (errno == ERANGE) {
        return ERR_INVALID_INPUT;
    }
    
    if (endptr == str || *endptr != '\0') {
        return ERR_INVALID_INPUT;
    }
    
    *value = result;
    return ERR_NONE;
}

int int_to_str(int value, char* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    int result = snprintf(buffer, size, "%d", value);
    
    if (result < 0 || (size_t)result >= size) {
        return ERR_MEMORY;
    }
    
    return ERR_NONE;
}

int double_to_str(double value, char* buffer, size_t size, int decimals) {
    if (buffer == NULL || size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    int result = snprintf(buffer, size, "%.*f", decimals, value);
    
    if (result < 0 || (size_t)result >= size) {
        return ERR_MEMORY;
    }
    
    return ERR_NONE;
}

const char* bool_to_str(int value) {
    return value ? "true" : "false";
}

int str_to_bool(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    char lower_str[10];
    safe_str_copy(lower_str, str, sizeof(lower_str));
    str_to_lower(lower_str);
    
    return (strcmp(lower_str, "true") == 0 ||
            strcmp(lower_str, "yes") == 0 ||
            strcmp(lower_str, "1") == 0 ||
            strcmp(lower_str, "on") == 0);
}

// ============================================
// VALIDATION UTILITIES
// ============================================

int is_null(const void* ptr) {
    return ptr == NULL;
}

int is_null_or_empty(const char* str) {
    return str == NULL || str[0] == '\0';
}

int is_in_range(int value, int min, int max) {
    return value >= min && value <= max;
}

int is_in_range_double(double value, double min, double max) {
    return value >= min && value <= max;
}

// ============================================
// ERROR HANDLING UTILITIES
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
            return "Not authorized";
        case ERR_DB_ERROR:
            return "Database error";
        case ERR_NOT_FOUND:
            return "Resource not found";
        case ERR_DUPLICATE:
            return "Duplicate entry";
        case ERR_MEMORY:
            return "Memory allocation failed";
        case ERR_FILE_IO:
            return "File input/output error";
        case ERR_CONFIG:
            return "Configuration error";
        case ERR_UNKNOWN:
        default:
            return "Unknown error";
    }
}

void print_error(ErrorCode code, const char* context) {
    fprintf(stderr, "Error: %s", get_error_message(code));
    if (context != NULL) {
        fprintf(stderr, " (%s)", context);
    }
    fprintf(stderr, "\n");
}

void handle_error(ErrorCode code, const char* context, int should_exit) {
    print_error(code, context);
    if (should_exit) {
        exit(EXIT_FAILURE);
    }
}

// ============================================
// FILE UTILITIES
// ============================================

int file_exists(const char* path) {
    if (path == NULL) {
        return 0;
    }
    
    FILE* file = fopen(path, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}

long get_file_size(const char* path) {
    if (path == NULL) {
        return -1;
    }
    
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    return size;
}

int read_file(const char* path, char* buffer, size_t buffer_size) {
    if (path == NULL || buffer == NULL || buffer_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return ERR_FILE_IO;
    }
    
    size_t bytes_read = fread(buffer, 1, buffer_size - 1, file);
    buffer[bytes_read] = '\0';
    
    fclose(file);
    
    return (int)bytes_read;
}

int write_file(const char* path, const char* buffer, size_t size) {
    if (path == NULL || buffer == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return ERR_FILE_IO;
    }
    
    size_t bytes_written = fwrite(buffer, 1, size, file);
    fclose(file);
    
    return (bytes_written == size) ? (int)bytes_written : ERR_FILE_IO;
}

int delete_file(const char* path) {
    if (path == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    if (remove(path) == 0) {
        return ERR_NONE;
    }
    return ERR_FILE_IO;
}

// ============================================
// DEBUG UTILITIES
// ============================================

void debug_printf(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
#else
    (void)format;  // Suppress unused parameter warning
#endif
}

void hex_dump(const void* data, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    
    for (size_t i = 0; i < size; i++) {
        fprintf(stderr, "%02x ", bytes[i]);
        if ((i + 1) % 16 == 0) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
}

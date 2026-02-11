/**
 * ============================================
 * validator.c - Input Validation Implementation
 * ============================================
 * 
 * This file contains implementations of input validation functions.
 * All user inputs must be validated before processing.
 * 
 * LEARNING: Implementing input validation for security
 */

#include "validator.h"
#include "utils.h"
#include <ctype.h>
#include <regex.h>
#include <errno.h>
#include <limits.h>

// ============================================
// USERNAME VALIDATION
// ============================================

ValidationResult validate_username(const char* username) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    // Check if username is NULL or empty
    if (is_null_or_empty(username)) {
        safe_str_copy(result.error_message, "Username cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check length (3-20 characters)
    size_t len = strlen(username);
    if (len < 3 || len > MAX_USERNAME_LEN) {
        safe_str_copy(result.error_message, "Username must be 3-20 characters", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check if starts with a letter
    if (!isalpha(username[0])) {
        safe_str_copy(result.error_message, "Username must start with a letter", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for valid characters (alphanumeric and underscore only)
    for (size_t i = 0; i < len; i++) {
        if (!isalnum(username[i]) && username[i] != '_') {
            safe_str_copy(result.error_message, 
                         "Username can only contain letters, numbers, and underscores", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    // Check for SQL injection patterns
    if (contains_sql_injection(username)) {
        safe_str_copy(result.error_message, "Username contains invalid characters", 
                     sizeof(result.error_message));
        return result;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

int is_valid_username_chars(const char* username) {
    ValidationResult result = validate_username(username);
    return result.is_valid;
}

// ============================================
// EMAIL VALIDATION
// ============================================

ValidationResult validate_email(const char* email) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    // Check if email is NULL or empty
    if (is_null_or_empty(email)) {
        safe_str_copy(result.error_message, "Email cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check length
    size_t len = strlen(email);
    if (len > MAX_EMAIL_LEN) {
        safe_str_copy(result.error_message, "Email is too long", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for @ symbol
    const char* at = strchr(email, '@');
    if (at == NULL) {
        safe_str_copy(result.error_message, "Email must contain @ symbol", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for at least one . after @
    const char* dot = strrchr(at, '.');
    if (dot == NULL || dot == at + 1) {
        safe_str_copy(result.error_message, "Email must have a domain", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for characters after the last dot
    if (strlen(dot) < 2) {
        safe_str_copy(result.error_message, "Email must have a valid TLD", 
                     sizeof(result.error_message));
        return result;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

int is_valid_email_format(const char* email) {
    ValidationResult result = validate_email(email);
    return result.is_valid;
}

// ============================================
// PASSWORD VALIDATION
// ============================================

ValidationResult validate_password(const char* password) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    // Check if password is NULL or empty
    if (is_null_or_empty(password)) {
        safe_str_copy(result.error_message, "Password cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check length (8-128 characters)
    size_t len = strlen(password);
    if (len < 8 || len > MAX_PASSWORD_LEN) {
        safe_str_copy(result.error_message, 
                     "Password must be 8-128 characters", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for at least one uppercase letter
    int has_upper = 0;
    int has_lower = 0;
    int has_digit = 0;
    int has_special = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("!@#$%^&*", password[i]) != NULL) has_special = 1;
    }
    
    if (!has_upper) {
        safe_str_copy(result.error_message, 
                     "Password must contain at least one uppercase letter", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (!has_lower) {
        safe_str_copy(result.error_message, 
                     "Password must contain at least one lowercase letter", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (!has_digit) {
        safe_str_copy(result.error_message, 
                     "Password must contain at least one digit", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (!has_special) {
        safe_str_copy(result.error_message, 
                     "Password must contain at least one special character (!@#$%^&*)", 
                     sizeof(result.error_message));
        return result;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

int is_valid_password_length(const char* password) {
    if (is_null_or_empty(password)) {
        return 0;
    }
    
    size_t len = strlen(password);
    return len >= 8 && len <= MAX_PASSWORD_LEN;
}

// ============================================
// PRODUCT VALIDATION
// ============================================

ValidationResult validate_product_name(const char* name) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (is_null_or_empty(name)) {
        safe_str_copy(result.error_message, "Product name cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    size_t len = strlen(name);
    if (len > MAX_PRODUCT_NAME) {
        safe_str_copy(result.error_message, "Product name is too long", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for control characters
    for (size_t i = 0; i < len; i++) {
        if (iscntrl(name[i])) {
            safe_str_copy(result.error_message, "Product name contains invalid characters", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

ValidationResult validate_product_description(const char* description) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    // Description is optional
    if (description == NULL || description[0] == '\0') {
        result.is_valid = 1;
        result.error_code = ERR_NONE;
        result.error_message[0] = '\0';
        return result;
    }
    
    size_t len = strlen(description);
    if (len > MAX_DESCRIPTION) {
        safe_str_copy(result.error_message, "Description is too long", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for control characters
    for (size_t i = 0; i < len; i++) {
        if (iscntrl(description[i])) {
            safe_str_copy(result.error_message, "Description contains invalid characters", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

ValidationResult validate_sku(const char* sku) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (is_null_or_empty(sku)) {
        safe_str_copy(result.error_message, "SKU cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    size_t len = strlen(sku);
    if (len > MAX_SKU_LEN) {
        safe_str_copy(result.error_message, "SKU is too long", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for valid characters (alphanumeric, hyphens, underscores)
    for (size_t i = 0; i < len; i++) {
        if (!isalnum(sku[i]) && sku[i] != '-' && sku[i] != '_') {
            safe_str_copy(result.error_message, 
                         "SKU can only contain letters, numbers, hyphens, and underscores", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

ValidationResult validate_price(const char* price_str, double* price_out) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (is_null_or_empty(price_str)) {
        safe_str_copy(result.error_message, "Price cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Convert to double
    char* endptr;
    errno = 0;
    double price = strtod(price_str, &endptr);
    
    if (errno == ERANGE) {
        safe_str_copy(result.error_message, "Price is out of range", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (endptr == price_str || *endptr != '\0') {
        safe_str_copy(result.error_message, "Price must be a valid number", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check if price is non-negative
    if (price < 0) {
        safe_str_copy(result.error_message, "Price cannot be negative", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check decimal places (max 2)
    char* dot = strchr(price_str, '.');
    if (dot != NULL) {
        size_t decimal_places = strlen(dot + 1);
        if (decimal_places > 2) {
            safe_str_copy(result.error_message, "Price can have at most 2 decimal places", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    if (price_out != NULL) {
        *price_out = price;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

ValidationResult validate_quantity(const char* quantity_str, int* quantity_out) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (is_null_or_empty(quantity_str)) {
        safe_str_copy(result.error_message, "Quantity cannot be empty", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Convert to integer
    char* endptr;
    errno = 0;
    long quantity = strtol(quantity_str, &endptr, 10);
    
    if (errno == ERANGE || quantity < 0 || quantity > INT_MAX) {
        safe_str_copy(result.error_message, "Quantity is out of range", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (endptr == quantity_str || *endptr != '\0') {
        safe_str_copy(result.error_message, "Quantity must be a valid integer", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (quantity < 0) {
        safe_str_copy(result.error_message, "Quantity cannot be negative", 
                     sizeof(result.error_message));
        return result;
    }
    
    if (quantity_out != NULL) {
        *quantity_out = (int)quantity;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

ValidationResult validate_category(const char* category) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    // Category is optional
    if (category == NULL || category[0] == '\0') {
        result.is_valid = 1;
        result.error_code = ERR_NONE;
        result.error_message[0] = '\0';
        return result;
    }
    
    size_t len = strlen(category);
    if (len > MAX_CATEGORY_LEN) {
        safe_str_copy(result.error_message, "Category is too long", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Check for valid characters (alphanumeric and spaces)
    for (size_t i = 0; i < len; i++) {
        if (!isalnum(category[i]) && !isspace(category[i])) {
            safe_str_copy(result.error_message, 
                         "Category can only contain letters, numbers, and spaces", 
                         sizeof(result.error_message));
            return result;
        }
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

// ============================================
// GENERAL VALIDATION FUNCTIONS
// ============================================

int is_valid_length(const char* str, size_t min_len, size_t max_len) {
    if (str == NULL) {
        return 0;
    }
    
    size_t len = strlen(str);
    return len >= min_len && len <= max_len;
}

int is_empty(const char* str) {
    return is_null_or_empty(str);
}

int is_alphanumeric(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isalnum(str[i])) {
            return 0;
        }
    }
    return 1;
}

int is_numeric(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int is_printable_ascii(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        // Check if character is printable ASCII (0-127)
        unsigned char c = (unsigned char)str[i];
        if (!isprint(c) || c > 127) {
            return 0;
        }
    }
    return 1;
}

int contains_sql_injection(const char* str) {
    if (str == NULL) {
        return 0;
    }
    
    // Convert to lowercase for case-insensitive matching
    char lower_str[MAX_ERROR_MSG];
    safe_str_copy(lower_str, str, sizeof(lower_str));
    str_to_lower(lower_str);
    
    // Check for common SQL injection patterns
    const char* patterns[] = {
        "' or '",
        "' or 1=1",
        "' or 1=1--",
        "' or 1=1#",
        "union select",
        "drop table",
        "delete from",
        "insert into",
        "update set",
        "--",
        "/*",
        "*/",
        "xp_",
        "exec(",
        "script:",
        "javascript:",
        "onerror=",
        "onload=",
        "<script",
        "</script>"
    };
    
    for (size_t i = 0; i < ARRAY_SIZE(patterns); i++) {
        char pattern_lower[50];
        safe_str_copy(pattern_lower, patterns[i], sizeof(pattern_lower));
        str_to_lower(pattern_lower);
        
        if (strstr(lower_str, pattern_lower) != NULL) {
            return 1;
        }
    }
    
    return 0;
}

// ============================================
// SANITIZATION FUNCTIONS
// ============================================

void trim_whitespace(char* str) {
    str_trim(str);
}

void sanitize_input(char* str) {
    if (str == NULL) {
        return;
    }
    
    // Trim whitespace
    trim_whitespace(str);
    
    // Remove potentially dangerous characters
    size_t j = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        // Keep only printable ASCII characters (excluding some dangerous ones)
        if (isprint(str[i]) && str[i] < 127 && 
            str[i] != '\r' && str[i] != '\n' && str[i] != '\t') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void to_lowercase(char* str) {
    str_to_lower(str);
}

// ============================================
// SAFE STRING FUNCTIONS
// ============================================

size_t safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return 0;
    }
    
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    
    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
    
    return copy_len;
}

size_t safe_strcat(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return 0;
    }
    
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    
    if (dest_len + src_len >= dest_size) {
        src_len = dest_size - dest_len - 1;
    }
    
    memcpy(dest + dest_len, src, src_len);
    dest[dest_len + src_len] = '\0';
    
    return src_len;
}

// ============================================
// COMMON FUNCTIONS (from utils)
// ============================================

const char* role_to_string(UserRole role) {
    switch (role) {
        case ROLE_USER:
            return "user";
        case ROLE_ADMIN:
            return "admin";
        default:
            return "unknown";
    }
}

UserRole string_to_role(const char* role_str) {
    if (role_str == NULL) {
        return ROLE_USER;
    }
    
    char lower_str[20];
    safe_str_copy(lower_str, role_str, sizeof(lower_str));
    str_to_lower(lower_str);
    
    if (strcmp(lower_str, "admin") == 0) {
        return ROLE_ADMIN;
    }
    return ROLE_USER;
}

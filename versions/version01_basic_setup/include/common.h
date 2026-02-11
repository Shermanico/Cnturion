/**
 * ============================================
 * common.h - Common Types and Constants
 * ============================================
 * 
 * This file contains common data types, constants,
 * and function declarations used throughout the application.
 * 
 * LEARNING: Understanding C structs, enums, and constants
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>  // For size_t type

// Forward declaration for SQLite (avoids including sqlite3.h everywhere)
typedef struct sqlite3 sqlite3;

// ============================================
// CONSTANTS
// ============================================

// Maximum lengths for various fields
#define MAX_USERNAME_LEN    50      // Maximum username length
#define MAX_EMAIL_LEN       100     // Maximum email length
#define MAX_PASSWORD_LEN    128     // Maximum password length
#define MAX_HASH_LEN        100     // Maximum password hash length
#define MAX_SALT_LEN       50      // Maximum salt length
#define MAX_ROLE_LEN       10      // Maximum role length
#define MAX_PRODUCT_NAME    100     // Maximum product name length
#define MAX_DESCRIPTION     500     // Maximum description length
#define MAX_SKU_LEN       50      // Maximum SKU length
#define MAX_CATEGORY_LEN    50      // Maximum category length
#define MAX_PATH_LEN       512     // Maximum file path length
#define MAX_LOG_MESSAGE    500     // Maximum log message length
#define MAX_ERROR_MSG      200     // Maximum error message length

// ============================================
// ERROR CODES
// ============================================
// Using error codes instead of returning strings is more secure
// because it prevents information leakage in error messages

typedef enum {
    ERR_NONE = 0,              // No error (success)
    ERR_INVALID_INPUT,           // Invalid input provided
    ERR_AUTH_FAILED,            // Authentication failed
    ERR_NOT_AUTHORIZED,         // User not authorized for action
    ERR_DB_ERROR,              // Database error occurred
    ERR_NOT_FOUND,             // Resource not found
    ERR_DUPLICATE,             // Duplicate entry exists
    ERR_MEMORY,                // Memory allocation failed
    ERR_FILE_IO,               // File input/output error
    ERR_CONFIG,                // Configuration error
    ERR_UNKNOWN                // Unknown error occurred
} ErrorCode;

// ============================================
// USER ROLE ENUM
// ============================================
// Using enum for roles provides type safety

typedef enum {
    ROLE_ADMIN = 1,    // Administrator with full access
    ROLE_USER = 2      // Regular user with limited access
} UserRole;

// ============================================
// USER STRUCTURE
// ============================================
// Represents a user in the system

typedef struct {
    int id;                         // Unique user ID
    char username[MAX_USERNAME_LEN];   // Username
    char password_hash[MAX_HASH_LEN];  // Hashed password
    char email[MAX_EMAIL_LEN];         // Email address
    UserRole role;                    // User role (admin or user)
    int is_locked;                   // Account lock status
    int failed_login_attempts;        // Failed login attempts counter
} User;

// ============================================
// PRODUCT STRUCTURE
// ============================================
// Represents a product in inventory

typedef struct {
    int id;                             // Unique product ID
    int user_id;                         // Owner user ID
    char name[MAX_PRODUCT_NAME];            // Product name
    char description[MAX_DESCRIPTION];       // Product description
    char sku[MAX_SKU_LEN];               // Stock Keeping Unit
    int quantity;                        // Quantity in stock
    double price;                         // Product price
    char category[MAX_CATEGORY_LEN];       // Product category
} Product;

// ============================================
// DATABASE STRUCTURE
// ============================================
// Represents the database connection

typedef struct {
    sqlite3* db;  // SQLite database handle
} Database;

#endif // COMMON_H

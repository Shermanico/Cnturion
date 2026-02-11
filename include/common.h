/**
 * ============================================
 * common.h - Common Definitions and Types
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding header files in C
 * - Type definitions with typedef
 * - Constants and macros
 * - Include guards
 * 
 * KEY CONCEPTS:
 * 1. Header Files (.h): Declare functions and types that can be shared
 * 2. Include Guards: Prevent multiple inclusion of the same header
 * 3. typedef: Create aliases for types (makes code more readable)
 * 4. enum: Define a set of named integer constants
 * 5. struct: Group related variables together
 * 
 * SECURITY NOTES:
 * - Define clear error codes for secure error handling
 * - Use fixed-size buffers to prevent buffer overflows
 * - Define maximum lengths for all string inputs
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================
// CONSTANTS - Maximum lengths for security
// ============================================
// These constants prevent buffer overflows by defining maximum sizes
// Always use these when allocating buffers or copying strings

#define MAX_USERNAME_LEN    50      // Maximum username length
#define MAX_EMAIL_LEN       100     // Maximum email length
#define MAX_PASSWORD_LEN    128     // Maximum password length
#define MAX_HASH_LEN        100     // Maximum password hash length
#define MAX_SALT_LEN        50      // Maximum salt length
#define MAX_ROLE_LEN        10      // Maximum role length
#define MAX_PRODUCT_NAME    100     // Maximum product name length
#define MAX_DESCRIPTION     500     // Maximum description length
#define MAX_SKU_LEN         50      // Maximum SKU length
#define MAX_CATEGORY_LEN    50      // Maximum category length
#define MAX_PATH_LEN        512     // Maximum file path length (increased for log rotation)
#define MAX_LOG_MESSAGE     500     // Maximum log message length
#define MAX_ERROR_MSG       200     // Maximum error message length

// ============================================
// ERROR CODES
// ============================================
// Using error codes instead of returning strings is more secure
// because it prevents information leakage in error messages

typedef enum {
    ERR_NONE = 0,              // No error (success)
    ERR_INVALID_INPUT,         // Input validation failed
    ERR_AUTH_FAILED,           // Authentication failed
    ERR_NOT_AUTHORIZED,        // User lacks permission
    ERR_DB_ERROR,              // Database operation failed
    ERR_NOT_FOUND,             // Resource not found
    ERR_DUPLICATE,             // Duplicate entry (e.g., username exists)
    ERR_MEMORY,                // Memory allocation failed
    ERR_FILE_IO,               // File input/output error
    ERR_CONFIG,                // Configuration error
    ERR_UNKNOWN                // Unknown error
} ErrorCode;

// ============================================
// USER ROLES
// ============================================
// Role-based access control (RBAC)
// Each role has different permissions

typedef enum {
    ROLE_USER = 0,             // Regular user - can manage inventory
    ROLE_ADMIN = 1             // Admin - can manage users and inventory
} UserRole;

// ============================================
// LOG LEVELS
// ============================================
// Different severity levels for logging

typedef enum {
    LOG_DEBUG = 0,             // Detailed debugging information
    LOG_INFO,                  // General informational messages
    LOG_WARNING,               // Warning messages
    LOG_ERROR,                 // Error messages
    LOG_SECURITY               // Security-related events
} LogLevel;

// ============================================
// DATA STRUCTURES
// ============================================

/**
 * UserSession Structure
 * 
 * Represents an authenticated user session.
 * This structure holds all information about the currently logged-in user.
 * 
 * SECURITY NOTES:
 * - Never store passwords in this structure
 * - Use is_authenticated flag to check login status
 * - Store role for authorization checks
 * 
 * LEARNING: struct allows grouping related variables
 */
typedef struct {
    int id;                    // Unique user ID from database
    char username[MAX_USERNAME_LEN];  // Username
    char email[MAX_EMAIL_LEN];         // Email address
    UserRole role;             // User role (admin or user)
    int is_authenticated;      // 1 = logged in, 0 = not logged in
    time_t login_time;         // When user logged in
} UserSession;

/**
 * Product Structure
 * 
 * Represents an inventory product.
 * Contains all product information that will be stored in the database.
 * 
 * SECURITY NOTES:
 * - All string fields have fixed maximum lengths
 * - Price is stored as double (floating point)
 * - Quantity is stored as int (integer)
 */
typedef struct {
    int id;                    // Unique product ID
    int user_id;               // Owner's user ID
    char name[MAX_PRODUCT_NAME];       // Product name
    char description[MAX_DESCRIPTION]; // Product description
    char sku[MAX_SKU_LEN];            // Stock Keeping Unit (unique code)
    double price;              // Product price
    int quantity;              // Available quantity
    char category[MAX_CATEGORY_LEN];  // Product category
    char created_at[30];       // Creation timestamp
    char updated_at[30];       // Last update timestamp
} Product;

/**
 * User Structure
 * 
 * Represents a user account.
 * Contains all user information from the database.
 * 
 * SECURITY NOTES:
 * - password_hash is the bcrypt hash, NOT plain text
 * - salt is used with bcrypt (bcrypt includes it automatically)
 * - is_active determines if account is usable
 */
typedef struct {
    int id;                    // Unique user ID
    char username[MAX_USERNAME_LEN];  // Username
    char email[MAX_EMAIL_LEN];         // Email address
    char password_hash[MAX_HASH_LEN]; // bcrypt hashed password
    char salt[MAX_SALT_LEN];          // Salt for password hashing
    UserRole role;             // User role
    char created_at[30];       // Account creation timestamp
    char last_login[30];      // Last successful login
    int is_active;             // 1 = active, 0 = inactive
} User;

/**
 * AuditLog Structure
 * 
 * Represents an audit log entry.
 * Used for security auditing and compliance.
 * 
 * SECURITY NOTES:
 * - Audit logs should be immutable (no UPDATE/DELETE)
 * - Helps detect suspicious activity
 * - Required for compliance in many industries
 */
typedef struct {
    int id;                    // Unique log entry ID
    int user_id;               // User who performed action (0 if system)
    char action[50];           // Action performed (LOGIN, CREATE, etc.)
    char resource[50];         // Resource affected (user, product, etc.)
    char details[MAX_LOG_MESSAGE];    // Additional details
    char ip_address[50];       // IP address (N/A for console app)
    char timestamp[30];        // When the action occurred
    int success;               // 1 = success, 0 = failure
} AuditLog;

// ============================================
// UTILITY MACROS
// ============================================

/**
 * Array size macro
 * Calculates the number of elements in an array
 * 
 * LEARNING: Macros are text substitutions done by preprocessor
 * This is safer than manually counting array elements
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * Min/Max macros
 * Returns the minimum or maximum of two values
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// ============================================
// FUNCTION DECLARATIONS
// ============================================

/**
 * Get error message for an error code
 * 
 * @param code The error code
 * @return Human-readable error message
 * 
 * SECURITY: Returns generic messages, not sensitive details
 */
const char* get_error_message(ErrorCode code);

/**
 * Get current timestamp as string
 * 
 * @param buffer Buffer to store timestamp
 * @param size Size of buffer
 * @return 0 on success, -1 on error
 */
int get_timestamp(char* buffer, size_t size);

/**
 * Convert role enum to string
 * 
 * @param role The role enum value
 * @return String representation of role
 */
const char* role_to_string(UserRole role);

/**
 * Convert string to role enum
 * 
 * @param role_str String representation of role
 * @return Role enum value
 */
UserRole string_to_role(const char* role_str);

#endif // COMMON_H

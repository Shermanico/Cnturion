/**
 * ============================================
 * logger.h - Logging and Auditing Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding logging systems
 * - File I/O in C
 * - Audit trails for security
 * - Log rotation concepts
 * 
 * KEY CONCEPTS:
 * 1. Logging: Recording events for debugging and monitoring
 * 2. Audit Trail: Immutable record of security-relevant events
 * 3. Log Levels: Different severity levels for filtering
 * 4. Log Rotation: Managing log file size
 * 
 * SECURITY NOTES:
 * - Log all security-relevant events (login, data changes, errors)
 * - Don't log sensitive data (passwords, credit cards, etc.)
 * - Protect log files from unauthorized access
 * - Use timestamps for forensic analysis
 * - Audit logs should be immutable (no UPDATE/DELETE)
 * 
 * AUDIT EVENTS TO LOG:
 * - User login (success and failure)
 * - User creation/deletion
 * - Password changes
 * - Product creation/update/deletion
 * - Failed authorization attempts
 * - System errors
 * - Configuration changes
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include <sqlite3.h>

// ============================================
// LOG FILE PATHS
// ============================================

/**
 * Default log file path
 * Can be overridden via environment variable
 */
#define DEFAULT_LOG_FILE "logs/app.log"

/**
 * Maximum log file size before rotation (10 MB)
 */
#define MAX_LOG_SIZE (10 * 1024 * 1024)

/**
 * Maximum number of log files to keep
 */
#define MAX_LOG_FILES 5

// ============================================
// LOG LEVEL STRINGS
// ============================================

/**
 * Convert log level to string
 * 
 * @param level Log level enum
 * @return String representation
 */
const char* log_level_to_string(LogLevel level);

// ============================================
// INITIALIZATION
// ============================================

/**
 * Initialize the logging system
 * 
 * Opens the log file and prepares for logging.
 * Creates the log directory if it doesn't exist.
 * 
 * @param log_file_path Path to the log file (NULL for default)
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: File I/O in C
 * SECURITY: Set appropriate file permissions
 */
int logger_init(const char* log_file_path);

/**
 * Close the logging system
 * 
 * Flushes any pending log entries and closes the log file.
 * 
 * @return ERR_NONE on success, error code on failure
 */
int logger_close(void);

/**
 * Check if logger is initialized
 * 
 * @return 1 if initialized, 0 otherwise
 */
int logger_is_initialized(void);

// ============================================
// GENERAL LOGGING
// ============================================

/**
 * Log a message at a specific level
 * 
 * This is the main logging function. All other logging
 * functions call this one.
 * 
 * @param level Log level (INFO, WARNING, ERROR, SECURITY)
 * @param message Message to log
 * @param session User session (can be NULL for system events)
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: Formatted output with fprintf
 * SECURITY: Don't log sensitive data
 */
int log_message(LogLevel level, const char* message, const UserSession* session);

/**
 * Log a formatted message
 * 
 * Like printf but writes to the log file.
 * 
 * @param level Log level
 * @param format Printf-style format string
 * @param ... Variable arguments
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: Variadic functions in C
 */
int log_printf(LogLevel level, const char* format, ...);

/**
 * Log an info message
 * 
 * @param message Message to log
 * @param session User session (can be NULL)
 * @return ERR_NONE on success, error code on failure
 */
int log_info(const char* message, const UserSession* session);

/**
 * Log a warning message
 * 
 * @param message Message to log
 * @param session User session (can be NULL)
 * @return ERR_NONE on success, error code on failure
 */
int log_warning(const char* message, const UserSession* session);

/**
 * Log an error message
 * 
 * @param message Message to log
 * @param session User session (can be NULL)
 * @return ERR_NONE on success, error code on failure
 */
int log_error(const char* message, const UserSession* session);

/**
 * Log a security event
 * 
 * Security events are logged at the highest priority.
 * 
 * @param message Security event message
 * @param session User session (can be NULL)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Always log security events
 */
int log_security(const char* message, const UserSession* session);

// ============================================
// AUDIT LOGGING
// ============================================

/**
 * Log an audit event to the database
 * 
 * Audit logs are stored in the database for permanent record.
 * They are used for compliance and forensic analysis.
 * 
 * @param db Database connection
 * @param action Action performed (LOGIN, CREATE, UPDATE, DELETE)
 * @param resource Resource affected (user, product, etc.)
 * @param details Additional details about the action
 * @param session User session (can be NULL for system events)
 * @param success 1 if action succeeded, 0 if it failed
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Audit logs are immutable
 */
int log_audit(sqlite3* db, const char* action, const char* resource, 
              const char* details, const UserSession* session, int success);

/**
 * Log a successful login
 * 
 * @param db Database connection
 * @param username Username that logged in
 * @return ERR_NONE on success, error code on failure
 */
int log_login_success(sqlite3* db, const char* username);

/**
 * Log a failed login attempt
 * 
 * @param db Database connection
 * @param username Username that attempted to log in
 * @param reason Reason for failure (invalid credentials, etc.)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Log all failed login attempts for threat detection
 */
int log_login_failure(sqlite3* db, const char* username, const char* reason);

/**
 * Log a user creation event
 * 
 * @param db Database connection
 * @param username Username of created user
 * @param role Role assigned to the user
 * @param admin_session Session of admin who created the user
 * @return ERR_NONE on success, error code on failure
 */
int log_user_created(sqlite3* db, const char* username, UserRole role, 
                     const UserSession* admin_session);

/**
 * Log a user deletion event
 * 
 * @param db Database connection
 * @param username Username of deleted user
 * @param admin_session Session of admin who deleted the user
 * @return ERR_NONE on success, error code on failure
 */
int log_user_deleted(sqlite3* db, const char* username, 
                     const UserSession* admin_session);

/**
 * Log a product creation event
 * 
 * @param db Database connection
 * @param product_name Name of created product
 * @param session Session of user who created the product
 * @return ERR_NONE on success, error code on failure
 */
int log_product_created(sqlite3* db, const char* product_name, 
                        const UserSession* session);

/**
 * Log a product update event
 * 
 * @param db Database connection
 * @param product_name Name of updated product
 * @param session Session of user who updated the product
 * @return ERR_NONE on success, error code on failure
 */
int log_product_updated(sqlite3* db, const char* product_name, 
                        const UserSession* session);

/**
 * Log a product deletion event
 * 
 * @param db Database connection
 * @param product_name Name of deleted product
 * @param session Session of user who deleted the product
 * @return ERR_NONE on success, error code on failure
 */
int log_product_deleted(sqlite3* db, const char* product_name, 
                         const UserSession* session);

/**
 * Log an authorization failure
 * 
 * @param db Database connection
 * @param resource Resource that was accessed without permission
 * @param session Session of user who attempted access
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Log all authorization failures for threat detection
 */
int log_authorization_failure(sqlite3* db, const char* resource, 
                              const UserSession* session);

// ============================================
// LOG MANAGEMENT
// ============================================

/**
 * Check if log rotation is needed
 * 
 * Rotates the log file if it exceeds the maximum size.
 * 
 * @return ERR_NONE on success, error code on failure
 */
int logger_rotate_if_needed(void);

/**
 * Get current log file size
 * 
 * @return File size in bytes, or negative on error
 */
long logger_get_file_size(void);

/**
 * Clear the log file
 * 
 * WARNING: This deletes all log entries!
 * Use with caution.
 * 
 * @return ERR_NONE on success, error code on failure
 */
int logger_clear(void);

// ============================================
// LOG ANALYSIS
// ============================================

/**
 * Count failed login attempts in the last N minutes
 * 
 * Useful for detecting brute force attacks.
 * 
 * @param minutes Number of minutes to look back
 * @return Number of failed login attempts, or negative on error
 * 
 * SECURITY: Detect brute force attacks
 */
int logger_count_failed_logins(int minutes);

/**
 * Get recent security events
 * 
 * @param events Output array for events
 * @param max_events Maximum number of events to retrieve
 * @return Number of events retrieved, or negative on error
 */
int logger_get_recent_security_events(char** events, int max_events);

#endif // LOGGER_H

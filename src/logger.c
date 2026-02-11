/**
 * ============================================
 * logger.c - Logging and Auditing Implementation
 * ============================================
 * 
 * This file contains implementations of logging and audit functions.
 * 
 * LEARNING: Implementing logging for security and debugging
 */

#define _GNU_SOURCE
#include "logger.h"
#include "database.h"
#include "utils.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

// ============================================
// GLOBAL STATE
// ============================================

static FILE* g_log_file = NULL;
static char g_log_path[MAX_PATH_LEN] = {0};

// ============================================
// LOG LEVEL STRINGS
// ============================================

const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_INFO:
            return "INFO";
        case LOG_WARNING:
            return "WARNING";
        case LOG_ERROR:
            return "ERROR";
        case LOG_SECURITY:
            return "SECURITY";
        default:
            return "UNKNOWN";
    }
}

// ============================================
// INITIALIZATION
// ============================================

int logger_init(const char* log_file_path) {
    // Use default path if none provided
    if (log_file_path == NULL) {
        log_file_path = DEFAULT_LOG_FILE;
    }
    
    // Store log path
    safe_str_copy(g_log_path, log_file_path, sizeof(g_log_path));
    
    // Create logs directory if it doesn't exist
    char log_dir[MAX_PATH_LEN];
    safe_str_copy(log_dir, g_log_path, sizeof(log_dir));
    char* last_slash = strrchr(log_dir, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        mkdir(log_dir, 0755);  // Create directory with rwxr-xr-x permissions
    }
    
    // Open log file in append mode
    g_log_file = fopen(g_log_path, "a");
    if (g_log_file == NULL) {
        return ERR_FILE_IO;
    }
    
    // Set line buffering
    setlinebuf(g_log_file);
    
    // Log initialization
    char timestamp[30];
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(g_log_file, "[%s] [%s] Logger initialized\n", 
            timestamp, log_level_to_string(LOG_INFO));
    fflush(g_log_file);
    
    return ERR_NONE;
}

int logger_close(void) {
    if (g_log_file != NULL) {
        char timestamp[30];
        get_timestamp(timestamp, sizeof(timestamp));
        fprintf(g_log_file, "[%s] [%s] Logger closed\n", 
                timestamp, log_level_to_string(LOG_INFO));
        fflush(g_log_file);
        fclose(g_log_file);
        g_log_file = NULL;
    }
    return ERR_NONE;
}

int logger_is_initialized(void) {
    return g_log_file != NULL;
}

// ============================================
// GENERAL LOGGING
// ============================================

int log_message(LogLevel level, const char* message, const UserSession* session) {
    if (!logger_is_initialized()) {
        return ERR_CONFIG;
    }
    
    if (message == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Get timestamp
    char timestamp[30];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Build log entry
    fprintf(g_log_file, "[%s] [%s]", timestamp, log_level_to_string(level));
    
    // Add user info if session is available
    if (session != NULL && session->is_authenticated) {
        fprintf(g_log_file, " [User:%s(%s)]", session->username, 
                role_to_string(session->role));
    }
    
    // Add message
    fprintf(g_log_file, " %s\n", message);
    fflush(g_log_file);
    
    // Rotate if needed
    logger_rotate_if_needed();
    
    return ERR_NONE;
}

int log_printf(LogLevel level, const char* format, ...) {
    if (!logger_is_initialized()) {
        return ERR_CONFIG;
    }
    
    if (format == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Get timestamp
    char timestamp[30];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Print timestamp and level
    fprintf(g_log_file, "[%s] [%s] ", timestamp, log_level_to_string(level));
    
    // Print formatted message
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);
    
    fprintf(g_log_file, "\n");
    fflush(g_log_file);
    
    // Rotate if needed
    logger_rotate_if_needed();
    
    return ERR_NONE;
}

int log_info(const char* message, const UserSession* session) {
    return log_message(LOG_INFO, message, session);
}

int log_warning(const char* message, const UserSession* session) {
    return log_message(LOG_WARNING, message, session);
}

int log_error(const char* message, const UserSession* session) {
    return log_message(LOG_ERROR, message, session);
}

int log_security(const char* message, const UserSession* session) {
    return log_message(LOG_SECURITY, message, session);
}

// ============================================
// AUDIT LOGGING
// ============================================

int log_audit(sqlite3* db, const char* action, const char* resource, 
              const char* details, const UserSession* session, int success) {
    if (db == NULL || action == NULL || resource == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    AuditLog log;
    memset(&log, 0, sizeof(log));
    
    // Set log data
    log.user_id = (session != NULL && session->is_authenticated) ? session->id : 0;
    safe_str_copy(log.action, action, sizeof(log.action));
    safe_str_copy(log.resource, resource, sizeof(log.resource));
    
    if (details != NULL) {
        safe_str_copy(log.details, details, sizeof(log.details));
    } else {
        log.details[0] = '\0';
    }
    
    safe_str_copy(log.ip_address, "N/A", sizeof(log.ip_address));
    get_timestamp(log.timestamp, sizeof(log.timestamp));
    log.success = success;
    
    // Write to database
    int result = db_create_audit_log(db, &log);
    
    // Also log to file
    char log_msg[MAX_LOG_MESSAGE];
    snprintf(log_msg, sizeof(log_msg), "AUDIT: %s %s - %s", 
             action, resource, success ? "SUCCESS" : "FAILURE");
    if (details != NULL && details[0] != '\0') {
        safe_str_cat(log_msg, " (", sizeof(log_msg));
        safe_str_cat(log_msg, details, sizeof(log_msg));
        safe_str_cat(log_msg, ")", sizeof(log_msg));
    }
    log_message(LOG_SECURITY, log_msg, session);
    
    return result;
}

int log_login_success(sqlite3* db, const char* username) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s logged in successfully", username);
    return log_audit(db, "LOGIN", "system", details, NULL, 1);
}

int log_login_failure(sqlite3* db, const char* username, const char* reason) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Failed login attempt for %s: %s", 
             username, reason ? reason : "unknown");
    return log_audit(db, "LOGIN", "system", details, NULL, 0);
}

int log_user_created(sqlite3* db, const char* username, UserRole role, 
                     const UserSession* admin_session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s created user %s with role %s",
             admin_session ? admin_session->username : "system",
             username, role_to_string(role));
    return log_audit(db, "CREATE", "user", details, admin_session, 1);
}

int log_user_deleted(sqlite3* db, const char* username, 
                     const UserSession* admin_session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s deleted user %s",
             admin_session ? admin_session->username : "system", username);
    return log_audit(db, "DELETE", "user", details, admin_session, 1);
}

int log_product_created(sqlite3* db, const char* product_name, 
                        const UserSession* session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s created product %s",
             session ? session->username : "unknown", product_name);
    return log_audit(db, "CREATE", "product", details, session, 1);
}

int log_product_updated(sqlite3* db, const char* product_name, 
                        const UserSession* session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s updated product %s",
             session ? session->username : "unknown", product_name);
    return log_audit(db, "UPDATE", "product", details, session, 1);
}

int log_product_deleted(sqlite3* db, const char* product_name, 
                         const UserSession* session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s deleted product %s",
             session ? session->username : "unknown", product_name);
    return log_audit(db, "DELETE", "product", details, session, 1);
}

int log_authorization_failure(sqlite3* db, const char* resource, 
                              const UserSession* session) {
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s attempted unauthorized access to %s",
             session ? session->username : "unknown", resource);
    return log_audit(db, "AUTH_FAILURE", resource, details, session, 0);
}

// ============================================
// LOG MANAGEMENT
// ============================================

int logger_rotate_if_needed(void) {
    long size = logger_get_file_size();
    if (size < 0 || size < MAX_LOG_SIZE) {
        return ERR_NONE;  // No rotation needed
    }
    
    // Close current log file
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    
    // Rotate existing log files
    char old_path[MAX_PATH_LEN];
    char new_path[MAX_PATH_LEN];
    
    // Delete oldest log file if it exists
    snprintf(old_path, sizeof(old_path), "%s.%d", g_log_path, MAX_LOG_FILES);
    remove(old_path);
    
    // Rotate log files
    for (int i = MAX_LOG_FILES - 1; i >= 1; i--) {
        snprintf(old_path, sizeof(old_path), "%s.%d", g_log_path, i);
        snprintf(new_path, sizeof(new_path), "%s.%d", g_log_path, i + 1);
        rename(old_path, new_path);
    }
    
    // Move current log to .1
    snprintf(new_path, sizeof(new_path), "%s.1", g_log_path);
    rename(g_log_path, new_path);
    
    // Reopen log file
    g_log_file = fopen(g_log_path, "a");
    if (g_log_file == NULL) {
        return ERR_FILE_IO;
    }
    setlinebuf(g_log_file);
    
    return ERR_NONE;
}

long logger_get_file_size(void) {
    if (g_log_path[0] == '\0') {
        return -1;
    }
    
    FILE* file = fopen(g_log_path, "rb");
    if (file == NULL) {
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    return size;
}

int logger_clear(void) {
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    
    // Truncate the log file
    FILE* file = fopen(g_log_path, "w");
    if (file != NULL) {
        fclose(file);
    }
    
    // Reopen log file
    g_log_file = fopen(g_log_path, "a");
    if (g_log_file == NULL) {
        return ERR_FILE_IO;
    }
    setlinebuf(g_log_file);
    
    return ERR_NONE;
}

// ============================================
// LOG ANALYSIS
// ============================================

int logger_count_failed_logins(int minutes) {
    if (g_log_path[0] == '\0') {
        return -1;
    }
    
    FILE* file = fopen(g_log_path, "r");
    if (file == NULL) {
        return 0;
    }
    
    time_t cutoff_time = time(NULL) - (minutes * 60);
    int count = 0;
    char line[1024];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Check if line contains failed login
        if (strstr(line, "LOGIN") != NULL && strstr(line, "FAILURE") != NULL) {
            // Extract timestamp and check if within time window
            char timestamp[30];
            if (sscanf(line, "[%29[^]]", timestamp) == 1) {
                struct tm tm_info;
                if (strptime(timestamp, "%Y-%m-%d %H:%M:%S", &tm_info) != NULL) {
                    time_t log_time = mktime(&tm_info);
                    if (difftime(log_time, cutoff_time) >= 0) {
                        count++;
                    }
                }
            }
        }
    }
    
    fclose(file);
    return count;
}

int logger_get_recent_security_events(char** events, int max_events) {
    if (g_log_path[0] == '\0' || events == NULL) {
        return -1;
    }
    
    FILE* file = fopen(g_log_path, "r");
    if (file == NULL) {
        return 0;
    }
    
    // Seek to end and read backwards
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    
    int count = 0;
    char line[1024];
    long pos = file_size;
    
    // Read lines backwards
    while (pos > 0 && count < max_events) {
        // Find previous newline
        pos--;
        while (pos > 0 && fgetc(file) != '\n') {
            pos--;
            fseek(file, pos, SEEK_SET);
        }
        
        if (fgets(line, sizeof(line), file) != NULL) {
            // Check if this is a security event
         if (strstr(line, "[SECURITY]") != NULL) {
                 events[count] = (char*)strdup(line);
                 if (events[count] != NULL) {
                     count++;
                 }
             }
        }
        
        fseek(file, pos, SEEK_SET);
    }
    
    fclose(file);
    return count;
}

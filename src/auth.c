/**
 * ============================================
 * auth.c - Authentication and Authorization Implementation
 * ============================================
 * 
 * This file contains implementations of authentication and
 * authorization functions.
 * 
 * LEARNING: Implementing secure authentication and RBAC
 */

#include "auth.h"
#include "logger.h"
#include "security.h"
#include "validator.h"
#include "database.h"
#include "utils.h"

// ============================================
// SESSION MANAGEMENT
// ============================================

UserSession* session_create(void) {
    UserSession* session = (UserSession*)safe_malloc(sizeof(UserSession));
    if (session == NULL) {
        return NULL;
    }
    
    // Initialize all fields
    memset(session, 0, sizeof(UserSession));
    session->is_authenticated = 0;
    session->role = ROLE_USER;
    session->login_time = 0;
    
    return session;
}

void session_destroy(UserSession* session) {
    if (session != NULL) {
        // Clear sensitive data
        secure_zero(session->username, sizeof(session->username));
        secure_zero(session->email, sizeof(session->email));
        
        safe_free((void**)&session);
    }
}

int session_is_authenticated(const UserSession* session) {
    if (session == NULL) {
        return 0;
    }
    return session->is_authenticated;
}

int session_is_admin(const UserSession* session) {
    if (session == NULL) {
        return 0;
    }
    return session->is_authenticated && session->role == ROLE_ADMIN;
}

int session_get_age(const UserSession* session) {
    if (session == NULL || !session->is_authenticated) {
        return -1;
    }
    
    time_t now = time(NULL);
    return (int)time_diff_seconds(session->login_time, now);
}

int session_is_expired(const UserSession* session) {
    int age = session_get_age(session);
    if (age < 0) {
        return 1;  // Invalid session
    }
    
    // Session expires after 30 minutes (1800 seconds)
    return age > 1800;
}

// ============================================
// AUTHENTICATION
// ============================================

UserSession* authenticate(sqlite3* db, const char* username, const char* password) {
    if (db == NULL || username == NULL || password == NULL) {
        return NULL;
    }
    
    // Validate username format
    ValidationResult vresult = validate_username(username);
    if (!vresult.is_valid) {
        log_login_failure(db, username, "Invalid username format");
        return NULL;
    }
    
    // Validate password length
    if (!is_valid_password_length(password)) {
        log_login_failure(db, username, "Invalid password length");
        return NULL;
    }
    
    // Get user from database
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        // Generic error message - don't reveal if user exists
        log_login_failure(db, username, "Invalid credentials");
        return NULL;
    }
    
    // Check if account is active
    if (!user.is_active) {
        log_login_failure(db, username, "Account is locked");
        return NULL;
    }
    
    // Verify password
    if (!verify_password(password, user.password_hash)) {
        log_login_failure(db, username, "Invalid credentials");
        return NULL;
    }
    
    // Create session
    UserSession* session = session_create();
    if (session == NULL) {
        log_error("Failed to create session", NULL);
        return NULL;
    }
    
    // Populate session
    session->id = user.id;
    safe_str_copy(session->username, user.username, sizeof(session->username));
    safe_str_copy(session->email, user.email, sizeof(session->email));
    session->role = user.role;
    session->is_authenticated = 1;
    session->login_time = time(NULL);
    
    // Update last login in database
    db_update_last_login(db, user.id);
    
    // Log successful login
    log_login_success(db, username);
    
    return session;
}

int logout(UserSession* session) {
    if (session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    session_destroy(session);
    return ERR_NONE;
}

int is_logged_in(const UserSession* session) {
    return session_is_authenticated(session);
}

// ============================================
// AUTHORIZATION
// ============================================

int has_permission(const UserSession* session, const char* action) {
    if (session == NULL || !session->is_authenticated) {
        return 0;
    }
    
    if (action == NULL) {
        return 0;
    }
    
    // Admin has all permissions
    if (session->role == ROLE_ADMIN) {
        return 1;
    }
    
    // Regular user permissions
    if (strcmp(action, "view_products") == 0 ||
        strcmp(action, "create_product") == 0 ||
        strcmp(action, "update_product") == 0 ||
        strcmp(action, "delete_product") == 0 ||
        strcmp(action, "search_products") == 0) {
        return 1;
    }
    
    return 0;
}

int can_manage_users(const UserSession* session) {
    return session_is_admin(session);
}

int can_manage_products(const UserSession* session) {
    return session_is_authenticated(session);
}

int can_view_audit_logs(const UserSession* session) {
    return session_is_admin(session);
}

int require_authentication(const UserSession* session) {
    if (!session_is_authenticated(session)) {
        return ERR_AUTH_FAILED;
    }
    return ERR_NONE;
}

int require_admin(const UserSession* session) {
    if (!session_is_admin(session)) {
        return ERR_NOT_AUTHORIZED;
    }
    return ERR_NONE;
}

// ============================================
// USER AUTHENTICATION HELPERS
// ============================================

int verify_credentials(sqlite3* db, const char* username, const char* password) {
    if (db == NULL || username == NULL || password == NULL) {
        return 0;
    }
    
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return 0;
    }
    
    if (!user.is_active) {
        return 0;
    }
    
    return verify_password(password, user.password_hash);
}

int change_password(sqlite3* db, const char* username, 
                   const char* current_password, const char* new_password) {
    if (db == NULL || username == NULL || current_password == NULL || new_password == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Validate new password
    ValidationResult vresult = validate_password(new_password);
    if (!vresult.is_valid) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify current password
    if (!verify_credentials(db, username, current_password)) {
        return ERR_AUTH_FAILED;
    }
    
    // Get user
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return ERR_NOT_FOUND;
    }
    
    // Hash new password
    char new_hash[BCRYPT_HASH_LENGTH + 1];
    result = hash_password(new_password, new_hash, sizeof(new_hash));
    if (result != ERR_NONE) {
        return result;
    }
    
    // Update password in database
    // Note: We need to add a function to update password in database
    // For now, we'll use a direct SQL update
    const char* sql = "UPDATE users SET password_hash = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, new_hash, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user.id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Clear password from memory
    secure_zero(new_hash, sizeof(new_hash));
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "User %s changed password", username);
    log_audit(db, "PASSWORD_CHANGE", "user", details, NULL, 1);
    
    return ERR_NONE;
}

int reset_password(sqlite3* db, const char* username, 
                   const char* new_password, const UserSession* admin_session) {
    if (db == NULL || username == NULL || new_password == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate new password
    ValidationResult vresult = validate_password(new_password);
    if (!vresult.is_valid) {
        return ERR_INVALID_INPUT;
    }
    
    // Get user
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return ERR_NOT_FOUND;
    }
    
    // Hash new password
    char new_hash[BCRYPT_HASH_LENGTH + 1];
    result = hash_password(new_password, new_hash, sizeof(new_hash));
    if (result != ERR_NONE) {
        return result;
    }
    
    // Update password in database
    const char* sql = "UPDATE users SET password_hash = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, new_hash, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user.id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Clear password from memory
    secure_zero(new_hash, sizeof(new_hash));
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s reset password for user %s", 
             admin_session->username, username);
    log_audit(db, "PASSWORD_RESET", "user", details, admin_session, 1);
    
    return ERR_NONE;
}

// ============================================
// ACCOUNT MANAGEMENT
// ============================================

int lock_account(sqlite3* db, const char* username, 
                 const UserSession* admin_session) {
    if (db == NULL || username == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get user
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return ERR_NOT_FOUND;
    }
    
    // Update account status
    const char* sql = "UPDATE users SET is_active = 0 WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, user.id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s locked account for user %s", 
             admin_session->username, username);
    log_audit(db, "ACCOUNT_LOCK", "user", details, admin_session, 1);
    
    return ERR_NONE;
}

int unlock_account(sqlite3* db, const char* username, 
                   const UserSession* admin_session) {
    if (db == NULL || username == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get user
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return ERR_NOT_FOUND;
    }
    
    // Update account status
    const char* sql = "UPDATE users SET is_active = 1 WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, user.id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s unlocked account for user %s", 
             admin_session->username, username);
    log_audit(db, "ACCOUNT_UNLOCK", "user", details, admin_session, 1);
    
    return ERR_NONE;
}

int is_account_locked(sqlite3* db, const char* username) {
    if (db == NULL || username == NULL) {
        return -1;
    }
    
    User user;
    int result = db_get_user_by_username(db, username, &user);
    if (result != ERR_NONE) {
        return -1;
    }
    
    return user.is_active == 0 ? 1 : 0;
}

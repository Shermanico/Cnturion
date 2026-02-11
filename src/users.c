/**
 * ============================================
 * users.c - User Management Implementation (Admin Only)
 * ============================================
 * 
 * This file contains implementations of user management functions.
 * All operations require admin role.
 * 
 * LEARNING: Implementing admin-only functionality with RBAC
 */

#include "users.h"
#include "logger.h"
#include "security.h"
#include "validator.h"
#include "auth.h"
#include "database.h"
#include "utils.h"

// ============================================
// USER CREATION
// ============================================

int users_create(sqlite3* db, const char* username, const char* email,
                const char* password, UserRole role,
                const UserSession* admin_session) {
    if (db == NULL || username == NULL || email == NULL || 
        password == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "create_user", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate username
    ValidationResult vresult = validate_username(username);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, admin_session);
        return ERR_INVALID_INPUT;
    }
    
    // Validate email
    vresult = validate_email(email);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, admin_session);
        return ERR_INVALID_INPUT;
    }
    
    // Validate password
    vresult = validate_password(password);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, admin_session);
        return ERR_INVALID_INPUT;
    }
    
    // Check if username already exists
    if (users_username_exists(db, username)) {
        log_error("Username already exists", admin_session);
        return ERR_DUPLICATE;
    }
    
    // Check if email already exists
    if (users_email_exists(db, email)) {
        log_error("Email already exists", admin_session);
        return ERR_DUPLICATE;
    }
    
    // Hash password
    char password_hash[BCRYPT_HASH_LENGTH + 1];
    int result = hash_password(password, password_hash, sizeof(password_hash));
    if (result != ERR_NONE) {
        log_error("Failed to hash password", admin_session);
        return result;
    }
    
    // Create user structure
    User user;
    memset(&user, 0, sizeof(user));
    safe_str_copy(user.username, username, sizeof(user.username));
    safe_str_copy(user.email, email, sizeof(user.email));
    safe_str_copy(user.password_hash, password_hash, sizeof(user.password_hash));
    safe_str_copy(user.salt, "included_in_hash", sizeof(user.salt));  // Salt is included in hash
    user.role = role;
    get_timestamp(user.created_at, sizeof(user.created_at));
    user.is_active = 1;
    
    // Create user in database
    result = db_create_user(db, &user);
    if (result != ERR_NONE) {
        log_error("Failed to create user", admin_session);
        secure_zero(password_hash, sizeof(password_hash));
        return result;
    }
    
    // Clear password from memory
    secure_zero(password_hash, sizeof(password_hash));
    
    // Log the event
    log_user_created(db, username, role, admin_session);
    
    return ERR_NONE;
}

int users_create_from_struct(sqlite3* db, const User* user,
                             const UserSession* admin_session) {
    if (db == NULL || user == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    return users_create(db, user->username, user->email, 
                     user->password_hash, user->role, admin_session);
}

// ============================================
// USER RETRIEVAL
// ============================================

int users_get_by_id(sqlite3* db, int user_id, User* user,
                    const UserSession* admin_session) {
    if (db == NULL || user == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    return db_get_user_by_id(db, user_id, user);
}

int users_get_by_username(sqlite3* db, const char* username, User* user,
                         const UserSession* admin_session) {
    if (db == NULL || username == NULL || user == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    return db_get_user_by_username(db, username, user);
}

int users_list_all(sqlite3* db, User* users, int max_users,
                   const UserSession* admin_session) {
    if (db == NULL || users == NULL || admin_session == NULL) {
        return -1;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return -1;
    }
    
    return db_list_users(db, users, max_users);
}

int users_list_by_role(sqlite3* db, UserRole role, User* users, int max_users,
                       const UserSession* admin_session) {
    if (db == NULL || users == NULL || admin_session == NULL) {
        return -1;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return -1;
    }
    
    // Get all users and filter by role
    int count = db_list_users(db, users, max_users);
    if (count < 0) {
        return -1;
    }
    
    int filtered_count = 0;
    for (int i = 0; i < count && filtered_count < max_users; i++) {
        if (users[i].role == role) {
            if (i != filtered_count) {
                users[filtered_count] = users[i];
            }
            filtered_count++;
        }
    }
    
    return filtered_count;
}

// ============================================
// USER UPDATE
// ============================================

int users_update(sqlite3* db, int user_id, const char* new_email,
                 UserRole new_role, const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "update_user", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Prevent admin from changing their own role
    if (user_id == admin_session->id && new_role != (UserRole)-1) {
        log_error("Cannot change your own role", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get existing user
    User user;
    int result = db_get_user_by_id(db, user_id, &user);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Validate new email if provided
    if (new_email != NULL && new_email[0] != '\0') {
        ValidationResult vresult = validate_email(new_email);
        if (!vresult.is_valid) {
            log_error(vresult.error_message, admin_session);
            return ERR_INVALID_INPUT;
        }
        
        // Check if email already exists (and belongs to different user)
        User existing_user;
        if (db_get_user_by_username(db, new_email, &existing_user) == ERR_NONE) {
            if (existing_user.id != user_id) {
                log_error("Email already exists", admin_session);
                return ERR_DUPLICATE;
            }
        }
        
        // Update email
        const char* sql = "UPDATE users SET email = ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (result != SQLITE_OK) {
            return ERR_DB_ERROR;
        }
        
        sqlite3_bind_text(stmt, 1, new_email, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, user_id);
        
        result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (result != SQLITE_DONE) {
            return ERR_DB_ERROR;
        }
    }
    
    // Update role if specified
    if (new_role != (UserRole)-1 && new_role != user.role) {
        // Check if this is the last admin
        if (user.role == ROLE_ADMIN && users_is_last_admin(db, user_id)) {
            log_error("Cannot change role of last admin", admin_session);
            return ERR_NOT_AUTHORIZED;
        }
        
        result = db_change_user_role(db, user_id, new_role);
        if (result != ERR_NONE) {
            return result;
        }
    }
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s updated user %d", 
             admin_session->username, user_id);
    log_audit(db, "UPDATE", "user", details, admin_session, 1);
    
    return ERR_NONE;
}

int users_change_role(sqlite3* db, int user_id, UserRole new_role,
                      const UserSession* admin_session) {
    return users_update(db, user_id, NULL, new_role, admin_session);
}

int users_reset_password(sqlite3* db, int user_id, const char* new_password,
                         const UserSession* admin_session) {
    if (db == NULL || new_password == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "reset_password", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate new password
    ValidationResult vresult = validate_password(new_password);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, admin_session);
        return ERR_INVALID_INPUT;
    }
    
    // Get user to get username
    User user;
    int result = db_get_user_by_id(db, user_id, &user);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Hash new password
    char password_hash[BCRYPT_HASH_LENGTH + 1];
    result = hash_password(new_password, password_hash, sizeof(password_hash));
    if (result != ERR_NONE) {
        log_error("Failed to hash password", admin_session);
        return result;
    }
    
    // Update password in database
    const char* sql = "UPDATE users SET password_hash = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        secure_zero(password_hash, sizeof(password_hash));
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        secure_zero(password_hash, sizeof(password_hash));
        return ERR_DB_ERROR;
    }
    
    // Clear password from memory
    secure_zero(password_hash, sizeof(password_hash));
    
    // Log the event
    char details[MAX_LOG_MESSAGE];
    snprintf(details, sizeof(details), "Admin %s reset password for user %s", 
             admin_session->username, user.username);
    log_audit(db, "PASSWORD_RESET", "user", details, admin_session, 1);
    
    return ERR_NONE;
}

// ============================================
// USER DELETION
// ============================================

int users_delete(sqlite3* db, int user_id, const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "delete_user", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Prevent admin from deleting themselves
    if (user_id == admin_session->id) {
        log_error("Cannot delete your own account", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Check if this is the last admin
    if (users_is_last_admin(db, user_id)) {
        log_error("Cannot delete last admin", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get user to log username
    User user;
    int result = db_get_user_by_id(db, user_id, &user);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Delete user
    result = db_delete_user(db, user_id);
    if (result != ERR_NONE) {
        log_error("Failed to delete user", admin_session);
        return result;
    }
    
    // Log the event
    log_user_deleted(db, user.username, admin_session);
    
    return ERR_NONE;
}

// ============================================
// ACCOUNT CONTROL
// ============================================

int users_lock_account(sqlite3* db, int user_id,
                      const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "lock_account", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Prevent admin from locking themselves
    if (user_id == admin_session->id) {
        log_error("Cannot lock your own account", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Update account status
    const char* sql = "UPDATE users SET is_active = 0 WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Log the event
    User user;
    if (db_get_user_by_id(db, user_id, &user) == ERR_NONE) {
        char details[MAX_LOG_MESSAGE];
        snprintf(details, sizeof(details), "Admin %s locked account for user %s", 
                 admin_session->username, user.username);
        log_audit(db, "ACCOUNT_LOCK", "user", details, admin_session, 1);
    }
    
    return ERR_NONE;
}

int users_unlock_account(sqlite3* db, int user_id,
                        const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        log_authorization_failure(db, "unlock_account", admin_session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Update account status
    const char* sql = "UPDATE users SET is_active = 1 WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    // Log the event
    User user;
    if (db_get_user_by_id(db, user_id, &user) == ERR_NONE) {
        char details[MAX_LOG_MESSAGE];
        snprintf(details, sizeof(details), "Admin %s unlocked account for user %s", 
                 admin_session->username, user.username);
        log_audit(db, "ACCOUNT_UNLOCK", "user", details, admin_session, 1);
    }
    
    return ERR_NONE;
}

int users_is_account_locked(sqlite3* db, int user_id) {
    if (db == NULL) {
        return -1;
    }
    
    User user;
    int result = db_get_user_by_id(db, user_id, &user);
    if (result != ERR_NONE) {
        return -1;
    }
    
    return user.is_active == 0 ? 1 : 0;
}

// ============================================
// USER STATISTICS
// ============================================

int users_count_total(sqlite3* db, const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return -1;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return -1;
    }
    
    User users[100];
    return db_list_users(db, users, 100);
}

int users_count_by_role(sqlite3* db, UserRole role,
                        const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return -1;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return -1;
    }
    
    User users[100];
    int count = db_list_users(db, users, 100);
    
    if (count < 0) {
        return -1;
    }
    
    int role_count = 0;
    for (int i = 0; i < count; i++) {
        if (users[i].role == role) {
            role_count++;
        }
    }
    
    return role_count;
}

int users_count_active(sqlite3* db, const UserSession* admin_session) {
    if (db == NULL || admin_session == NULL) {
        return -1;
    }
    
    // Verify admin is authenticated
    if (!session_is_admin(admin_session)) {
        return -1;
    }
    
    User users[100];
    int count = db_list_users(db, users, 100);
    
    if (count < 0) {
        return -1;
    }
    
    int active_count = 0;
    for (int i = 0; i < count; i++) {
        if (users[i].is_active) {
            active_count++;
        }
    }
    
    return active_count;
}

// ============================================
// VALIDATION HELPERS
// ============================================

ValidationResult users_validate_user(const User* user) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (user == NULL) {
        safe_str_copy(result.error_message, "User is NULL", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Validate username
    ValidationResult vresult = validate_username(user->username);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    // Validate email
    vresult = validate_email(user->email);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

int users_username_exists(sqlite3* db, const char* username) {
    if (db == NULL || username == NULL) {
        return -1;
    }
    
    User user;
    int result = db_get_user_by_username(db, username, &user);
    
    return (result == ERR_NONE) ? 1 : 0;
}

int users_email_exists(sqlite3* db, const char* email) {
    if (db == NULL || email == NULL) {
        return -1;
    }
    
    // For simplicity, we'll check all users
    User users[100];
    int count = db_list_users(db, users, 100);
    
    if (count < 0) {
        return -1;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].email, email) == 0) {
            return 1;
        }
    }
    
    return 0;
}

int users_is_last_admin(sqlite3* db, int user_id) {
    if (db == NULL) {
        return -1;
    }
    
    // Get user to check role
    User user;
    int result = db_get_user_by_id(db, user_id, &user);
    if (result != ERR_NONE) {
        return -1;
    }
    
    // If user is not admin, they're not the last admin
    if (user.role != ROLE_ADMIN) {
        return 0;
    }
    
    // Count total admins
    User users[100];
    int count = db_list_users(db, users, 100);
    
    if (count < 0) {
        return -1;
    }
    
    int admin_count = 0;
    for (int i = 0; i < count; i++) {
        if (users[i].role == ROLE_ADMIN && users[i].is_active) {
            admin_count++;
        }
    }
    
    return (admin_count <= 1) ? 1 : 0;
}

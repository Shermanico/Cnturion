/**
 * ============================================
 * users.h - User Management Header (Admin Only)
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding admin-only functionality
 * - User lifecycle management
 * - Role-based operations
 * - Authorization enforcement
 * 
 * KEY CONCEPTS:
 * 1. User Management: Create, read, update, delete users
 * 2. Admin Operations: Actions only admins can perform
 * 3. Role Assignment: Assigning roles to users
 * 4. Account Control: Enabling/disabling accounts
 * 
 * SECURITY NOTES:
 * - All operations require admin role
 * - Verify admin session before every operation
 * - Log all user management actions
 * - Validate all user data
 * - Use transactions for multi-step operations
 * - Admins cannot delete themselves
 * - At least one admin must always exist
 * 
 * USER MANAGEMENT OPERATIONS:
 * - Create: Add new user account
 * - Read: View user details
 * - Update: Modify user information
 * - Delete: Remove user account
 * - List: View all users
 */

#ifndef USERS_H
#define USERS_H

#include "common.h"
#include "validator.h"
#include <sqlite3.h>

// ============================================
// USER CREATION
// ============================================

/**
 * Create a new user account
 * 
 * Creates a new user with the specified role.
 * Only admins can create users.
 * 
 * @param db Database connection
 * @param username Username for new user
 * @param email Email for new user
 * @param password Password for new user (will be hashed)
 * @param role Role to assign (ROLE_USER or ROLE_ADMIN)
 * @param admin_session Session of admin creating the user
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Validates all input
 * - Hashes password before storage
 * - Logs the event
 * 
 * LEARNING: User creation with security checks
 */
int users_create(sqlite3* db, const char* username, const char* email,
                const char* password, UserRole role,
                const UserSession* admin_session);

/**
 * Create a new user from User structure
 * 
 * Alternative method that accepts a User structure.
 * 
 * @param db Database connection
 * @param user User structure with user data
 * @param admin_session Session of admin creating the user
 * @return ERR_NONE on success, error code on failure
 */
int users_create_from_struct(sqlite3* db, const User* user,
                             const UserSession* admin_session);

// ============================================
// USER RETRIEVAL
// ============================================

/**
 * Get user by ID
 * 
 * Retrieves user details by ID.
 * 
 * @param db Database connection
 * @param user_id User ID to retrieve
 * @param user Output parameter for user data
 * @param admin_session Session of admin requesting the data
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Admin only
 */
int users_get_by_id(sqlite3* db, int user_id, User* user,
                    const UserSession* admin_session);

/**
 * Get user by username
 * 
 * Retrieves user details by username.
 * 
 * @param db Database connection
 * @param username Username to retrieve
 * @param user Output parameter for user data
 * @param admin_session Session of admin requesting the data
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Admin only
 */
int users_get_by_username(sqlite3* db, const char* username, User* user,
                         const UserSession* admin_session);

/**
 * List all users
 * 
 * Retrieves all users in the system.
 * 
 * @param db Database connection
 * @param users Output array for users
 * @param max_users Maximum number of users to retrieve
 * @param admin_session Session of admin requesting the list
 * @return Number of users retrieved, or negative error code
 * 
 * SECURITY: Admin only
 */
int users_list_all(sqlite3* db, User* users, int max_users,
                   const UserSession* admin_session);

/**
 * List users by role
 * 
 * Retrieves all users with a specific role.
 * 
 * @param db Database connection
 * @param role Role to filter by
 * @param users Output array for users
 * @param max_users Maximum number of users to retrieve
 * @param admin_session Session of admin requesting the list
 * @return Number of users retrieved, or negative error code
 */
int users_list_by_role(sqlite3* db, UserRole role, User* users, int max_users,
                       const UserSession* admin_session);

// ============================================
// USER UPDATE
// ============================================

/**
 * Update user information
 * 
 * Modifies user details.
 * Only admins can update user information.
 * 
 * @param db Database connection
 * @param user_id User ID to update
 * @param new_email New email (NULL to keep current)
 * @param new_role New role (ROLE_USER or ROLE_ADMIN, -1 to keep current)
 * @param admin_session Session of admin updating the user
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Cannot change own role (prevents privilege escalation)
 * - Logs the event
 */
int users_update(sqlite3* db, int user_id, const char* new_email,
                 UserRole new_role, const UserSession* admin_session);

/**
 * Change user role
 * 
 * Changes the role of a user.
 * 
 * @param db Database connection
 * @param user_id User ID to update
 * @param new_role New role to assign
 * @param admin_session Session of admin changing the role
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Cannot change own role
 * - At least one admin must always exist
 * - Logs the event
 */
int users_change_role(sqlite3* db, int user_id, UserRole new_role,
                      const UserSession* admin_session);

/**
 * Reset user password
 * 
 * Resets a user's password to a new value.
 * The user will need to change it on next login.
 * 
 * @param db Database connection
 * @param user_id User ID to update
 * @param new_password New password to set
 * @param admin_session Session of admin resetting the password
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Validates password strength
 * - Hashes new password
 * - Logs the event
 */
int users_reset_password(sqlite3* db, int user_id, const char* new_password,
                         const UserSession* admin_session);

// ============================================
// USER DELETION
// ============================================

/**
 * Delete a user account
 * 
 * Removes a user from the system.
 * All products owned by the user will also be deleted (CASCADE).
 * 
 * @param db Database connection
 * @param user_id User ID to delete
 * @param admin_session Session of admin deleting the user
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Cannot delete own account
 * - At least one admin must always exist
 * - Uses transaction for atomicity
 * - Logs the event
 */
int users_delete(sqlite3* db, int user_id, const UserSession* admin_session);

// ============================================
// ACCOUNT CONTROL
// ============================================

/**
 * Lock a user account
 * 
 * Prevents user from logging in.
 * 
 * @param db Database connection
 * @param user_id User ID to lock
 * @param admin_session Session of admin locking the account
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Cannot lock own account
 * - Logs the event
 */
int users_lock_account(sqlite3* db, int user_id,
                      const UserSession* admin_session);

/**
 * Unlock a user account
 * 
 * Allows user to log in again.
 * 
 * @param db Database connection
 * @param user_id User ID to unlock
 * @param admin_session Session of admin unlocking the account
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Logs the event
 */
int users_unlock_account(sqlite3* db, int user_id,
                        const UserSession* admin_session);

/**
 * Check if account is locked
 * 
 * @param db Database connection
 * @param user_id User ID to check
 * @return 1 if locked, 0 if not, negative on error
 */
int users_is_account_locked(sqlite3* db, int user_id);

// ============================================
// USER STATISTICS
// ============================================

/**
 * Get total number of users
 * 
 * @param db Database connection
 * @param admin_session Session of admin requesting the count
 * @return Number of users, or negative error code
 */
int users_count_total(sqlite3* db, const UserSession* admin_session);

/**
 * Get number of users by role
 * 
 * @param db Database connection
 * @param role Role to count
 * @param admin_session Session of admin requesting the count
 * @return Number of users with specified role, or negative error code
 */
int users_count_by_role(sqlite3* db, UserRole role,
                        const UserSession* admin_session);

/**
 * Get number of active users
 * 
 * @param db Database connection
 * @param admin_session Session of admin requesting the count
 * @return Number of active users, or negative error code
 */
int users_count_active(sqlite3* db, const UserSession* admin_session);

// ============================================
// VALIDATION HELPERS
// ============================================

/**
 * Validate user data
 * 
 * Checks all user fields for validity.
 * 
 * @param user User to validate
 * @return ValidationResult with validation status
 */
ValidationResult users_validate_user(const User* user);

/**
 * Check if username exists
 * 
 * @param db Database connection
 * @param username Username to check
 * @return 1 if exists, 0 if not, negative on error
 */
int users_username_exists(sqlite3* db, const char* username);

/**
 * Check if email exists
 * 
 * @param db Database connection
 * @param email Email to check
 * @return 1 if exists, 0 if not, negative on error
 */
int users_email_exists(sqlite3* db, const char* email);

/**
 * Check if this is the last admin
 * 
 * Prevents deleting or demoting the last admin account.
 * 
 * @param db Database connection
 * @param user_id User ID to check
 * @return 1 if this is the last admin, 0 otherwise, negative on error
 */
int users_is_last_admin(sqlite3* db, int user_id);

#endif // USERS_H

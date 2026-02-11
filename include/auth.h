/**
 * ============================================
 * auth.h - Authentication and Authorization Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding authentication (who are you?)
 * - Understanding authorization (what can you do?)
 * - Session management
 * - Role-based access control (RBAC)
 * 
 * KEY CONCEPTS:
 * 1. Authentication: Verifying user identity (login)
 * 2. Authorization: Checking permissions (access control)
 * 3. Session: Maintaining authenticated state
 * 4. RBAC: Role-Based Access Control
 * 
 * SECURITY NOTES:
 * - Never store plain text passwords
 * - Use generic error messages (don't reveal if username exists)
 * - Implement rate limiting for login attempts
 * - Log all authentication events
 * - Sessions should expire after inactivity
 * - Always check authorization on every protected operation
 * 
 * AUTHENTICATION FLOW:
 * 1. User enters credentials
 * 2. Validate input
 * 3. Look up user in database
 * 4. Verify password hash
 * 5. Create session if successful
 * 6. Log the event
 * 
 * AUTHORIZATION FLOW:
 * 1. Check if user is authenticated
 * 2. Check if user has required role
 * 3. Allow or deny access
 * 4. Log authorization failures
 */

#ifndef AUTH_H
#define AUTH_H

#include "common.h"
#include <sqlite3.h>

// ============================================
// SESSION MANAGEMENT
// ============================================

/**
 * Create a new user session
 * 
 * Allocates and initializes a new UserSession structure.
 * 
 * @return Pointer to new session, NULL on error
 * 
 * LEARNING: Dynamic memory allocation with malloc
 * SECURITY: Initialize all fields to safe defaults
 */
UserSession* session_create(void);

/**
 * Destroy a user session
 * 
 * Frees memory allocated for the session.
 * 
 * @param session Session to destroy
 * 
 * LEARNING: Free allocated memory to prevent leaks
 */
void session_destroy(UserSession* session);

/**
 * Check if a session is authenticated
 * 
 * @param session Session to check
 * @return 1 if authenticated, 0 otherwise
 */
int session_is_authenticated(const UserSession* session);

/**
 * Check if a session belongs to an admin
 * 
 * @param session Session to check
 * @return 1 if admin, 0 otherwise
 */
int session_is_admin(const UserSession* session);

/**
 * Get session age in seconds
 * 
 * @param session Session to check
 * @return Age in seconds, or -1 on error
 */
int session_get_age(const UserSession* session);

/**
 * Check if session has expired
 * 
 * Sessions expire after 30 minutes of inactivity.
 * 
 * @param session Session to check
 * @return 1 if expired, 0 if still valid
 */
int session_is_expired(const UserSession* session);

// ============================================
// AUTHENTICATION
// ============================================

/**
 * Authenticate a user with username and password
 * 
 * This function performs the full authentication flow:
 * 1. Validates input
 * 2. Looks up user in database
 * 3. Verifies password hash
 * 4. Creates session if successful
 * 5. Logs the event
 * 
 * @param db Database connection
 * @param username Username to authenticate
 * @param password Password to verify
 * @return Pointer to authenticated session, NULL on failure
 * 
 * SECURITY:
 * - Generic error messages (don't reveal if username exists)
 * - Log all attempts (success and failure)
 * - Rate limiting should be implemented at UI level
 * 
 * LEARNING: Multi-step authentication process
 */
UserSession* authenticate(sqlite3* db, const char* username, const char* password);

/**
 * Log out a user
 * 
 * Destroys the session and logs the event.
 * 
 * @param session Session to log out
 * @return ERR_NONE on success, error code on failure
 */
int logout(UserSession* session);

/**
 * Check if a user is currently logged in
 * 
 * @param session Session to check
 * @return 1 if logged in, 0 otherwise
 */
int is_logged_in(const UserSession* session);

// ============================================
// AUTHORIZATION
// ============================================

/**
 * Check if user has permission to perform an action
 * 
 * This function implements role-based access control (RBAC).
 * 
 * @param session User session
 * @param action Action to check permission for
 * @return 1 if authorized, 0 if not
 * 
 * SECURITY: Always check authorization before protected operations
 * 
 * Example:
 *   if (!has_permission(session, "delete_user")) {
 *       log_authorization_failure(db, "delete_user", session);
 *       return ERR_NOT_AUTHORIZED;
 *   }
 */
int has_permission(const UserSession* session, const char* action);

/**
 * Check if user can manage users (admin only)
 * 
 * @param session User session
 * @return 1 if authorized, 0 if not
 */
int can_manage_users(const UserSession* session);

/**
 * Check if user can manage products
 * 
 * Both admins and regular users can manage their own products.
 * 
 * @param session User session
 * @return 1 if authorized, 0 if not
 */
int can_manage_products(const UserSession* session);

/**
 * Check if user can view audit logs (admin only)
 * 
 * @param session User session
 * @return 1 if authorized, 0 if not
 */
int can_view_audit_logs(const UserSession* session);

/**
 * Require authentication
 * 
 * Returns error if user is not authenticated.
 * 
 * @param session User session
 * @return ERR_NONE if authenticated, ERR_AUTH_FAILED otherwise
 */
int require_authentication(const UserSession* session);

/**
 * Require admin role
 * 
 * Returns error if user is not an admin.
 * 
 * @param session User session
 * @return ERR_NONE if admin, ERR_NOT_AUTHORIZED otherwise
 */
int require_admin(const UserSession* session);

// ============================================
// USER AUTHENTICATION HELPERS
// ============================================

/**
 * Verify user credentials
 * 
 * Checks if username and password are valid without creating a session.
 * Useful for password changes or account verification.
 * 
 * @param db Database connection
 * @param username Username to verify
 * @param password Password to verify
 * @return 1 if credentials are valid, 0 otherwise
 */
int verify_credentials(sqlite3* db, const char* username, const char* password);

/**
 * Change user password
 * 
 * Requires current password for verification.
 * 
 * @param db Database connection
 * @param username Username
 * @param current_password Current password for verification
 * @param new_password New password to set
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Verify current password first
 * - Validate new password strength
 * - Hash new password before storing
 * - Log the event
 */
int change_password(sqlite3* db, const char* username, 
                   const char* current_password, const char* new_password);

/**
 * Reset user password (admin only)
 * 
 * Allows admins to reset user passwords without knowing current password.
 * 
 * @param db Database connection
 * @param username Username
 * @param new_password New password to set
 * @param admin_session Session of admin performing the reset
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Admin only
 * - Validate new password strength
 * - Log the event
 */
int reset_password(sqlite3* db, const char* username, 
                   const char* new_password, const UserSession* admin_session);

// ============================================
// ACCOUNT MANAGEMENT
// ============================================

/**
 * Lock a user account
 * 
 * Prevents user from logging in.
 * 
 * @param db Database connection
 * @param username Username to lock
 * @param admin_session Session of admin locking the account
 * @return ERR_NONE on success, error code on failure
 */
int lock_account(sqlite3* db, const char* username, 
                 const UserSession* admin_session);

/**
 * Unlock a user account
 * 
 * Allows user to log in again.
 * 
 * @param db Database connection
 * @param username Username to unlock
 * @param admin_session Session of admin unlocking the account
 * @return ERR_NONE on success, error code on failure
 */
int unlock_account(sqlite3* db, const char* username, 
                   const UserSession* admin_session);

/**
 * Check if account is locked
 * 
 * @param db Database connection
 * @param username Username to check
 * @return 1 if locked, 0 if not, negative on error
 */
int is_account_locked(sqlite3* db, const char* username);

#endif // AUTH_H

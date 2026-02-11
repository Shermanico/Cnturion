/**
 * ============================================
 * database.h - Database Operations Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding SQLite C API
 * - Database connection management
 * - Prepared statements (SQL injection prevention)
 * - Transaction management
 * 
 * KEY CONCEPTS:
 * 1. sqlite3*: SQLite database connection handle
 * 2. Prepared Statements: Pre-compiled SQL for security
 * 3. Transactions: Group multiple operations atomically
 * 4. Foreign Keys: Enforce referential integrity
 * 
 * SECURITY NOTES:
 * - ALWAYS use prepared statements to prevent SQL injection
 * - Never concatenate user input into SQL queries
 * - Use transactions for multi-step operations
 * - Enable foreign key constraints
 * - Close connections when done
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"
#include <sqlite3.h>
#include <stdint.h>

// ============================================
// DATABASE CONNECTION
// ============================================

/**
 * Connect to SQLite database
 * 
 * Creates a connection to the SQLite database file.
 * If the file doesn't exist, it will be created.
 * 
 * @param db_path Path to the database file
 * @return sqlite3* pointer to database connection, NULL on error
 * 
 * LEARNING: sqlite3_open() creates or opens a database file
 * SECURITY: Validate db_path to prevent path traversal
 */
sqlite3* db_connect(const char* db_path);

/**
 * Disconnect from database
 * 
 * Closes the database connection and frees resources.
 * 
 * @param db Database connection pointer
 * 
 * LEARNING: Always close connections to prevent resource leaks
 * SECURITY: NULL check prevents crashes
 */
void db_disconnect(sqlite3* db);

/**
 * Initialize database schema
 * 
 * Creates all tables if they don't exist.
 * This should be called once when setting up the application.
 * 
 * @param db Database connection pointer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Enables foreign keys and creates proper indexes
 */
int db_init(sqlite3* db);

/**
 * Check if database exists and is valid
 * 
 * @param db_path Path to the database file
 * @return 1 if valid, 0 if not
 */
int db_exists(const char* db_path);

// ============================================
// USER OPERATIONS
// ============================================

/**
 * Create a new user in the database
 * 
 * SECURITY: Uses prepared statement to prevent SQL injection
 * 
 * @param db Database connection
 * @param user User structure with user data
 * @return ERR_NONE on success, error code on failure
 */
int db_create_user(sqlite3* db, const User* user);

/**
 * Get user by username
 * 
 * SECURITY: Uses prepared statement, returns password hash for verification
 * 
 * @param db Database connection
 * @param username Username to search for
 * @param user Output parameter for user data
 * @return ERR_NONE on success, ERR_NOT_FOUND if user doesn't exist
 */
int db_get_user_by_username(sqlite3* db, const char* username, User* user);

/**
 * Get user by ID
 * 
 * @param db Database connection
 * @param user_id User ID to search for
 * @param user Output parameter for user data
 * @return ERR_NONE on success, ERR_NOT_FOUND if user doesn't exist
 */
int db_get_user_by_id(sqlite3* db, int user_id, User* user);

/**
 * Update user's last login timestamp
 * 
 * @param db Database connection
 * @param user_id User ID to update
 * @return ERR_NONE on success, error code on failure
 */
int db_update_last_login(sqlite3* db, int user_id);

/**
 * List all users (admin only)
 * 
 * @param db Database connection
 * @param users Output array for users
 * @param max_users Maximum number of users to retrieve
 * @return Number of users retrieved, or negative error code
 */
int db_list_users(sqlite3* db, User* users, int max_users);

/**
 * Delete a user
 * 
 * SECURITY: Uses transaction to ensure atomicity
 * CASCADE will delete user's products
 * 
 * @param db Database connection
 * @param user_id User ID to delete
 * @return ERR_NONE on success, error code on failure
 */
int db_delete_user(sqlite3* db, int user_id);

/**
 * Change user role
 * 
 * @param db Database connection
 * @param user_id User ID to update
 * @param new_role New role for the user
 * @return ERR_NONE on success, error code on failure
 */
int db_change_user_role(sqlite3* db, int user_id, UserRole new_role);

// ============================================
// PRODUCT OPERATIONS
// ============================================

/**
 * Create a new product
 * 
 * SECURITY: Uses prepared statement, enforces user ownership
 * 
 * @param db Database connection
 * @param product Product structure with product data
 * @return ERR_NONE on success, error code on failure
 */
int db_create_product(sqlite3* db, const Product* product);

/**
 * Get product by ID
 * 
 * SECURITY: Verifies user ownership before returning product
 * 
 * @param db Database connection
 * @param product_id Product ID to search for
 * @param user_id User ID requesting the product (for ownership check)
 * @param product Output parameter for product data
 * @return ERR_NONE on success, ERR_NOT_FOUND or ERR_NOT_AUTHORIZED on failure
 */
int db_get_product_by_id(sqlite3* db, int product_id, int user_id, Product* product);

/**
 * Update a product
 * 
 * SECURITY: Uses prepared statement, verifies ownership
 * 
 * @param db Database connection
 * @param product_id Product ID to update
 * @param user_id User ID requesting the update
 * @param product Product structure with updated data
 * @return ERR_NONE on success, error code on failure
 */
int db_update_product(sqlite3* db, int product_id, int user_id, const Product* product);

/**
 * Delete a product
 * 
 * SECURITY: Verifies ownership before deletion
 * 
 * @param db Database connection
 * @param product_id Product ID to delete
 * @param user_id User ID requesting the deletion
 * @return ERR_NONE on success, error code on failure
 */
int db_delete_product(sqlite3* db, int product_id, int user_id);

/**
 * List all products for a user
 * 
 * SECURITY: Only returns products owned by the user
 * 
 * @param db Database connection
 * @param user_id User ID to get products for
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 */
int db_list_products(sqlite3* db, int user_id, Product* products, int max_products);

/**
 * Search products by name or SKU
 * 
 * SECURITY: Uses prepared statement with LIKE, filters by user
 * 
 * @param db Database connection
 * @param user_id User ID to search for
 * @param search_term Search term (can be name or SKU)
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 */
int db_search_products(sqlite3* db, int user_id, const char* search_term, 
                      Product* products, int max_products);

/**
 * Check if SKU exists for a user
 * 
 * @param db Database connection
 * @param user_id User ID to check for
 * @param sku SKU to check
 * @return 1 if SKU exists, 0 if not, negative on error
 */
int db_sku_exists(sqlite3* db, int user_id, const char* sku);

// ============================================
// AUDIT LOG OPERATIONS
// ============================================

/**
 * Create an audit log entry
 * 
 * SECURITY: Audit logs are immutable (no UPDATE/DELETE)
 * 
 * @param db Database connection
 * @param log AuditLog structure with log data
 * @return ERR_NONE on success, error code on failure
 */
int db_create_audit_log(sqlite3* db, const AuditLog* log);

/**
 * Get recent audit logs
 * 
 * SECURITY: Only admins can view full audit logs
 * 
 * @param db Database connection
 * @param logs Output array for logs
 * @param max_logs Maximum number of logs to retrieve
 * @return Number of logs retrieved, or negative error code
 */
int db_get_recent_audit_logs(sqlite3* db, AuditLog* logs, int max_logs);

/**
 * Get audit logs for a specific user
 * 
 * @param db Database connection
 * @param user_id User ID to get logs for
 * @param logs Output array for logs
 * @param max_logs Maximum number of logs to retrieve
 * @return Number of logs retrieved, or negative error code
 */
int db_get_user_audit_logs(sqlite3* db, int user_id, AuditLog* logs, int max_logs);

// ============================================
// UTILITY FUNCTIONS
// ============================================

/**
 * Begin a transaction
 * 
 * Transactions ensure atomicity - either all operations succeed,
 * or none do. This is critical for data integrity.
 * 
 * @param db Database connection
 * @return ERR_NONE on success, error code on failure
 */
int db_begin_transaction(sqlite3* db);

/**
 * Commit a transaction
 * 
 * Makes all changes in the transaction permanent.
 * 
 * @param db Database connection
 * @return ERR_NONE on success, error code on failure
 */
int db_commit_transaction(sqlite3* db);

/**
 * Rollback a transaction
 * 
 * Undoes all changes made in the transaction.
 * Call this when an error occurs during a transaction.
 * 
 * @param db Database connection
 * @return ERR_NONE on success, error code on failure
 */
int db_rollback_transaction(sqlite3* db);

/**
 * Get the last error message from SQLite
 * 
 * @param db Database connection
 * @return Error message string
 */
const char* db_get_last_error(sqlite3* db);

/**
 * Get the last insert row ID
 * 
 * Useful after INSERT operations to get the auto-generated ID.
 * 
 * @param db Database connection
 * @return Last insert row ID
 */
int64_t db_last_insert_rowid(sqlite3* db);

#endif // DATABASE_H

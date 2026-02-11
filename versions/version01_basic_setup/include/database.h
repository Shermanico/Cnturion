/**
 * ============================================
 * database.h - Database Operations Interface
 * ============================================
 * 
 * This file declares database operations for the inventory
 * management system.
 * 
 * LEARNING: Database interface design, prepared statements
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"

// ============================================
// DATABASE INITIALIZATION
// ============================================

/**
 * Initialize the database connection and create tables
 * 
 * @param db_path Path to the database file
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode database_init(const char* db_path);

/**
 * Close the database connection
 * 
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode database_close(void);

/**
 * Get the database connection
 * 
 * @return Pointer to database structure, NULL if not initialized
 */
Database* database_get_connection(void);

// ============================================
// DATABASE SCHEMA CREATION
// ============================================

/**
 * Create the users table
 * 
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode database_create_users_table(void);

/**
 * Create the products table
 * 
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode database_create_products_table(void);

/**
 * Create the audit_logs table
 * 
 * @return ERR_NONE on success, error code on failure
 */
ErrorCode database_create_audit_logs_table(void);

#endif // DATABASE_H

/**
 * ============================================
 * database.c - Database Operations Implementation
 * ============================================
 * 
 * This file contains implementations of database operations
 * using SQLite.
 * 
 * LEARNING: SQLite C API, prepared statements, SQL injection prevention
 */

#include "database.h"
#include "utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// GLOBAL STATE
// ============================================

static Database g_database = {0};

// ============================================
// DATABASE INITIALIZATION
// ============================================

ErrorCode database_init(const char* db_path) {
    if (db_path == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Open database (creates if doesn't exist)
    int rc = sqlite3_open(db_path, &g_database.db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(g_database.db));
        return ERR_DB_ERROR;
    }
    
    printf("Database opened successfully: %s\n", db_path);
    
    // Create tables
    ErrorCode err = database_create_users_table();
    if (err != ERR_NONE) {
        return err;
    }
    
    err = database_create_products_table();
    if (err != ERR_NONE) {
        return err;
    }
    
    err = database_create_audit_logs_table();
    if (err != ERR_NONE) {
        return err;
    }
    
    printf("Database tables created successfully\n");
    
    return ERR_NONE;
}

ErrorCode database_close(void) {
    if (g_database.db != NULL) {
        sqlite3_close(g_database.db);
        g_database.db = NULL;
        printf("Database closed successfully\n");
    }
    return ERR_NONE;
}

Database* database_get_connection(void) {
    return &g_database;
}

// ============================================
// DATABASE SCHEMA CREATION
// ============================================

ErrorCode database_create_users_table(void) {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "role INTEGER NOT NULL,"
        "is_locked INTEGER DEFAULT 0,"
        "failed_login_attempts INTEGER DEFAULT 0,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    char* err_msg = NULL;
    int rc = sqlite3_exec(g_database.db, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    printf("Users table created (or already exists)\n");
    return ERR_NONE;
}

ErrorCode database_create_products_table(void) {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS products ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "description TEXT,"
        "sku TEXT UNIQUE NOT NULL,"
        "quantity INTEGER DEFAULT 0,"
        "price REAL DEFAULT 0.0,"
        "category TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ");";
    
    char* err_msg = NULL;
    int rc = sqlite3_exec(g_database.db, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    printf("Products table created (or already exists)\n");
    return ERR_NONE;
}

ErrorCode database_create_audit_logs_table(void) {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS audit_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "username TEXT NOT NULL,"
        "action TEXT NOT NULL,"
        "details TEXT,"
        "ip_address TEXT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ");";
    
    char* err_msg = NULL;
    int rc = sqlite3_exec(g_database.db, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    printf("Audit logs table created (or already exists)\n");
    return ERR_NONE;
}

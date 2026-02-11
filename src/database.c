/**
 * ============================================
 * database.c - Database Operations Implementation
 * ============================================
 * 
 * This file contains implementations of all database operations.
 * 
 * SECURITY CRITICAL:
 * - ALL database operations use prepared statements
 * - This prevents SQL injection attacks
 * - Never concatenate user input into SQL queries
 * 
 * LEARNING: SQLite C API and prepared statements
 */

#include "database.h"
#include "utils.h"
#include <sys/stat.h>

// ============================================
// DATABASE CONNECTION
// ============================================

sqlite3* db_connect(const char* db_path) {
    if (db_path == NULL) {
        return NULL;
    }
    
    sqlite3* db = NULL;
    int result = sqlite3_open(db_path, &db);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    // Enable foreign key constraints
    result = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "Cannot enable foreign keys: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    
    return db;
}

void db_disconnect(sqlite3* db) {
    if (db != NULL) {
        sqlite3_close(db);
    }
}

int db_init(sqlite3* db) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // SQL to create all tables
    const char* sql = 
        "PRAGMA foreign_keys = ON;"
        
        "CREATE TABLE IF NOT EXISTS users ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   username TEXT UNIQUE NOT NULL,"
        "   email TEXT UNIQUE NOT NULL,"
        "   password_hash TEXT NOT NULL,"
        "   salt TEXT NOT NULL,"
        "   role TEXT NOT NULL CHECK(role IN ('admin', 'user')),"
        "   created_at TEXT NOT NULL,"
        "   last_login TEXT,"
        "   is_active INTEGER DEFAULT 1"
        ");"
        
        "CREATE TABLE IF NOT EXISTS products ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   user_id INTEGER NOT NULL,"
        "   name TEXT NOT NULL,"
        "   description TEXT,"
        "   sku TEXT UNIQUE NOT NULL,"
        "   price REAL NOT NULL CHECK(price >= 0),"
        "   quantity INTEGER NOT NULL CHECK(quantity >= 0),"
        "   category TEXT,"
        "   created_at TEXT NOT NULL,"
        "   updated_at TEXT NOT NULL,"
        "   FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS audit_logs ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   user_id INTEGER,"
        "   action TEXT NOT NULL,"
        "   resource TEXT NOT NULL,"
        "   details TEXT,"
        "   ip_address TEXT,"
        "   timestamp TEXT NOT NULL,"
        "   success INTEGER NOT NULL,"
        "   FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL"
        ");"
        
        "CREATE INDEX IF NOT EXISTS idx_products_user_id ON products(user_id);"
        "CREATE INDEX IF NOT EXISTS idx_audit_logs_user_id ON audit_logs(user_id);"
        "CREATE INDEX IF NOT EXISTS idx_audit_logs_timestamp ON audit_logs(timestamp);";
    
    char* err_msg = NULL;
    int result = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_exists(const char* db_path) {
    if (db_path == NULL) {
        return 0;
    }
    
    struct stat st;
    return (stat(db_path, &st) == 0 && S_ISREG(st.st_mode));
}

// ============================================
// USER OPERATIONS
// ============================================

int db_create_user(sqlite3* db, const User* user) {
    if (db == NULL || user == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "INSERT INTO users (username, email, password_hash, salt, role, created_at) "
                     "VALUES (?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    // Bind parameters (prevents SQL injection)
    sqlite3_bind_text(stmt, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user->salt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, role_to_string(user->role), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, user->created_at, -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT) {
            return ERR_DUPLICATE;
        }
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_get_user_by_username(sqlite3* db, const char* username, User* user) {
    if (db == NULL || username == NULL || user == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "SELECT id, username, email, password_hash, salt, role, "
                     "created_at, last_login, is_active "
                     "FROM users WHERE username = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        safe_str_copy(user->username, (const char*)sqlite3_column_text(stmt, 1), 
                     sizeof(user->username));
        safe_str_copy(user->email, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(user->email));
        safe_str_copy(user->password_hash, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(user->password_hash));
        safe_str_copy(user->salt, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(user->salt));
        
        const char* role_str = (const char*)sqlite3_column_text(stmt, 5);
        user->role = string_to_role(role_str);
        
        safe_str_copy(user->created_at, (const char*)sqlite3_column_text(stmt, 6), 
                     sizeof(user->created_at));
        
        const char* last_login = (const char*)sqlite3_column_text(stmt, 7);
        if (last_login != NULL) {
            safe_str_copy(user->last_login, last_login, sizeof(user->last_login));
        } else {
            user->last_login[0] = '\0';
        }
        
        user->is_active = sqlite3_column_int(stmt, 8);
        
        sqlite3_finalize(stmt);
        return ERR_NONE;
    }
    
    sqlite3_finalize(stmt);
    return ERR_NOT_FOUND;
}

int db_get_user_by_id(sqlite3* db, int user_id, User* user) {
    if (db == NULL || user == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "SELECT id, username, email, password_hash, salt, role, "
                     "created_at, last_login, is_active "
                     "FROM users WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        safe_str_copy(user->username, (const char*)sqlite3_column_text(stmt, 1), 
                     sizeof(user->username));
        safe_str_copy(user->email, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(user->email));
        safe_str_copy(user->password_hash, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(user->password_hash));
        safe_str_copy(user->salt, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(user->salt));
        
        const char* role_str = (const char*)sqlite3_column_text(stmt, 5);
        user->role = string_to_role(role_str);
        
        safe_str_copy(user->created_at, (const char*)sqlite3_column_text(stmt, 6), 
                     sizeof(user->created_at));
        
        const char* last_login = (const char*)sqlite3_column_text(stmt, 7);
        if (last_login != NULL) {
            safe_str_copy(user->last_login, last_login, sizeof(user->last_login));
        } else {
            user->last_login[0] = '\0';
        }
        
        user->is_active = sqlite3_column_int(stmt, 8);
        
        sqlite3_finalize(stmt);
        return ERR_NONE;
    }
    
    sqlite3_finalize(stmt);
    return ERR_NOT_FOUND;
}

int db_update_last_login(sqlite3* db, int user_id) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char timestamp[30];
    get_timestamp(timestamp, sizeof(timestamp));
    
    const char* sql = "UPDATE users SET last_login = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_list_users(sqlite3* db, User* users, int max_users) {
    if (db == NULL || users == NULL) {
        return -1;
    }
    
    const char* sql = "SELECT id, username, email, password_hash, salt, role, "
                     "created_at, last_login, is_active FROM users;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    int count = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW && count < max_users) {
        User* user = &users[count];
        
        user->id = sqlite3_column_int(stmt, 0);
        safe_str_copy(user->username, (const char*)sqlite3_column_text(stmt, 1), 
                     sizeof(user->username));
        safe_str_copy(user->email, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(user->email));
        safe_str_copy(user->password_hash, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(user->password_hash));
        safe_str_copy(user->salt, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(user->salt));
        
        const char* role_str = (const char*)sqlite3_column_text(stmt, 5);
        user->role = string_to_role(role_str);
        
        safe_str_copy(user->created_at, (const char*)sqlite3_column_text(stmt, 6), 
                     sizeof(user->created_at));
        
        const char* last_login = (const char*)sqlite3_column_text(stmt, 7);
        if (last_login != NULL) {
            safe_str_copy(user->last_login, last_login, sizeof(user->last_login));
        } else {
            user->last_login[0] = '\0';
        }
        
        user->is_active = sqlite3_column_int(stmt, 8);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int db_delete_user(sqlite3* db, int user_id) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "DELETE FROM users WHERE id = ?;";
    
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
    
    return ERR_NONE;
}

int db_change_user_role(sqlite3* db, int user_id, UserRole new_role) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "UPDATE users SET role = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, role_to_string(new_role), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

// ============================================
// PRODUCT OPERATIONS
// ============================================

int db_create_product(sqlite3* db, const Product* product) {
    if (db == NULL || product == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "INSERT INTO products (user_id, name, description, sku, price, "
                     "quantity, category, created_at, updated_at) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, product->user_id);
    sqlite3_bind_text(stmt, 2, product->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, product->description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, product->sku, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, product->price);
    sqlite3_bind_int(stmt, 6, product->quantity);
    sqlite3_bind_text(stmt, 7, product->category, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, product->created_at, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, product->updated_at, -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT) {
            return ERR_DUPLICATE;
        }
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_get_product_by_id(sqlite3* db, int product_id, int user_id, Product* product) {
    if (db == NULL || product == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Include user_id in WHERE clause for row-level security
    const char* sql = "SELECT id, user_id, name, description, sku, price, quantity, "
                     "category, created_at, updated_at "
                     "FROM products WHERE id = ? AND user_id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, product_id);
    sqlite3_bind_int(stmt, 2, user_id);
    
    result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        product->id = sqlite3_column_int(stmt, 0);
        product->user_id = sqlite3_column_int(stmt, 1);
        safe_str_copy(product->name, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(product->name));
        safe_str_copy(product->description, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(product->description));
        safe_str_copy(product->sku, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(product->sku));
        product->price = sqlite3_column_double(stmt, 5);
        product->quantity = sqlite3_column_int(stmt, 6);
        safe_str_copy(product->category, (const char*)sqlite3_column_text(stmt, 7), 
                     sizeof(product->category));
        safe_str_copy(product->created_at, (const char*)sqlite3_column_text(stmt, 8), 
                     sizeof(product->created_at));
        safe_str_copy(product->updated_at, (const char*)sqlite3_column_text(stmt, 9), 
                     sizeof(product->updated_at));
        
        sqlite3_finalize(stmt);
        return ERR_NONE;
    }
    
    sqlite3_finalize(stmt);
    return ERR_NOT_FOUND;
}

int db_update_product(sqlite3* db, int product_id, int user_id, const Product* product) {
    if (db == NULL || product == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char timestamp[30];
    get_timestamp(timestamp, sizeof(timestamp));
    
    const char* sql = "UPDATE products SET name = ?, description = ?, sku = ?, "
                     "price = ?, quantity = ?, category = ?, updated_at = ? "
                     "WHERE id = ? AND user_id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, product->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, product->description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, product->sku, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, product->price);
    sqlite3_bind_int(stmt, 5, product->quantity);
    sqlite3_bind_text(stmt, 6, product->category, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, product_id);
    sqlite3_bind_int(stmt, 9, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        if (sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT) {
            return ERR_DUPLICATE;
        }
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_delete_product(sqlite3* db, int product_id, int user_id) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Include user_id for row-level security
    const char* sql = "DELETE FROM products WHERE id = ? AND user_id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, product_id);
    sqlite3_bind_int(stmt, 2, user_id);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_list_products(sqlite3* db, int user_id, Product* products, int max_products) {
    if (db == NULL || products == NULL) {
        return -1;
    }
    
    // Filter by user_id for row-level security
    const char* sql = "SELECT id, user_id, name, description, sku, price, quantity, "
                     "category, created_at, updated_at "
                     "FROM products WHERE user_id = ? ORDER BY name;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    int count = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW && count < max_products) {
        Product* product = &products[count];
        
        product->id = sqlite3_column_int(stmt, 0);
        product->user_id = sqlite3_column_int(stmt, 1);
        safe_str_copy(product->name, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(product->name));
        safe_str_copy(product->description, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(product->description));
        safe_str_copy(product->sku, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(product->sku));
        product->price = sqlite3_column_double(stmt, 5);
        product->quantity = sqlite3_column_int(stmt, 6);
        safe_str_copy(product->category, (const char*)sqlite3_column_text(stmt, 7), 
                     sizeof(product->category));
        safe_str_copy(product->created_at, (const char*)sqlite3_column_text(stmt, 8), 
                     sizeof(product->created_at));
        safe_str_copy(product->updated_at, (const char*)sqlite3_column_text(stmt, 9), 
                     sizeof(product->updated_at));
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int db_search_products(sqlite3* db, int user_id, const char* search_term, 
                      Product* products, int max_products) {
    if (db == NULL || search_term == NULL || products == NULL) {
        return -1;
    }
    
    // Use LIKE for pattern matching, filter by user_id
    const char* sql = "SELECT id, user_id, name, description, sku, price, quantity, "
                     "category, created_at, updated_at "
                     "FROM products WHERE user_id = ? "
                     "AND (name LIKE ? OR sku LIKE ?) "
                     "ORDER BY name;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    char pattern[MAX_SKU_LEN + 2];
    snprintf(pattern, sizeof(pattern), "%%%s%%", search_term);
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, pattern, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pattern, -1, SQLITE_STATIC);
    
    int count = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW && count < max_products) {
        Product* product = &products[count];
        
        product->id = sqlite3_column_int(stmt, 0);
        product->user_id = sqlite3_column_int(stmt, 1);
        safe_str_copy(product->name, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(product->name));
        safe_str_copy(product->description, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(product->description));
        safe_str_copy(product->sku, (const char*)sqlite3_column_text(stmt, 4), 
                     sizeof(product->sku));
        product->price = sqlite3_column_double(stmt, 5);
        product->quantity = sqlite3_column_int(stmt, 6);
        safe_str_copy(product->category, (const char*)sqlite3_column_text(stmt, 7), 
                     sizeof(product->category));
        safe_str_copy(product->created_at, (const char*)sqlite3_column_text(stmt, 8), 
                     sizeof(product->created_at));
        safe_str_copy(product->updated_at, (const char*)sqlite3_column_text(stmt, 9), 
                     sizeof(product->updated_at));
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int db_sku_exists(sqlite3* db, int user_id, const char* sku) {
    if (db == NULL || sku == NULL) {
        return -1;
    }
    
    const char* sql = "SELECT COUNT(*) FROM products WHERE user_id = ? AND sku = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, sku, -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    int count = 0;
    if (result == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count > 0 ? 1 : 0;
}

// ============================================
// AUDIT LOG OPERATIONS
// ============================================

int db_create_audit_log(sqlite3* db, const AuditLog* log) {
    if (db == NULL || log == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    const char* sql = "INSERT INTO audit_logs (user_id, action, resource, details, "
                     "ip_address, timestamp, success) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return ERR_DB_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, log->user_id);
    sqlite3_bind_text(stmt, 2, log->action, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, log->resource, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, log->details, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, log->ip_address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, log->timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, log->success);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_get_recent_audit_logs(sqlite3* db, AuditLog* logs, int max_logs) {
    if (db == NULL || logs == NULL) {
        return -1;
    }
    
    const char* sql = "SELECT id, user_id, action, resource, details, ip_address, "
                     "timestamp, success FROM audit_logs "
                     "ORDER BY timestamp DESC LIMIT ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, max_logs);
    
    int count = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW && count < max_logs) {
        AuditLog* log = &logs[count];
        
        log->id = sqlite3_column_int(stmt, 0);
        log->user_id = sqlite3_column_int(stmt, 1);
        safe_str_copy(log->action, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(log->action));
        safe_str_copy(log->resource, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(log->resource));
        
        const char* details = (const char*)sqlite3_column_text(stmt, 4);
        if (details != NULL) {
            safe_str_copy(log->details, details, sizeof(log->details));
        } else {
            log->details[0] = '\0';
        }
        
        safe_str_copy(log->ip_address, (const char*)sqlite3_column_text(stmt, 5), 
                     sizeof(log->ip_address));
        safe_str_copy(log->timestamp, (const char*)sqlite3_column_text(stmt, 6), 
                     sizeof(log->timestamp));
        log->success = sqlite3_column_int(stmt, 7);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int db_get_user_audit_logs(sqlite3* db, int user_id, AuditLog* logs, int max_logs) {
    if (db == NULL || logs == NULL) {
        return -1;
    }
    
    const char* sql = "SELECT id, user_id, action, resource, details, ip_address, "
                     "timestamp, success FROM audit_logs "
                     "WHERE user_id = ? ORDER BY timestamp DESC LIMIT ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, max_logs);
    
    int count = 0;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW && count < max_logs) {
        AuditLog* log = &logs[count];
        
        log->id = sqlite3_column_int(stmt, 0);
        log->user_id = sqlite3_column_int(stmt, 1);
        safe_str_copy(log->action, (const char*)sqlite3_column_text(stmt, 2), 
                     sizeof(log->action));
        safe_str_copy(log->resource, (const char*)sqlite3_column_text(stmt, 3), 
                     sizeof(log->resource));
        
        const char* details = (const char*)sqlite3_column_text(stmt, 4);
        if (details != NULL) {
            safe_str_copy(log->details, details, sizeof(log->details));
        } else {
            log->details[0] = '\0';
        }
        
        safe_str_copy(log->ip_address, (const char*)sqlite3_column_text(stmt, 5), 
                     sizeof(log->ip_address));
        safe_str_copy(log->timestamp, (const char*)sqlite3_column_text(stmt, 6), 
                     sizeof(log->timestamp));
        log->success = sqlite3_column_int(stmt, 7);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

// ============================================
// UTILITY FUNCTIONS
// ============================================

int db_begin_transaction(sqlite3* db) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* err_msg = NULL;
    int result = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err_msg);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "Cannot begin transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_commit_transaction(sqlite3* db) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* err_msg = NULL;
    int result = sqlite3_exec(db, "COMMIT;", NULL, NULL, &err_msg);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "Cannot commit transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

int db_rollback_transaction(sqlite3* db) {
    if (db == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char* err_msg = NULL;
    int result = sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &err_msg);
    
    if (result != SQLITE_OK) {
        fprintf(stderr, "Cannot rollback transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        return ERR_DB_ERROR;
    }
    
    return ERR_NONE;
}

const char* db_get_last_error(sqlite3* db) {
    if (db == NULL) {
        return "Invalid database handle";
    }
    return sqlite3_errmsg(db);
}

int64_t db_last_insert_rowid(sqlite3* db) {
    if (db == NULL) {
        return -1;
    }
    return sqlite3_last_insert_rowid(db);
}

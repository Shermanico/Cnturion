/**
 * ============================================
 * inventory.c - Inventory Management Implementation
 * ============================================
 * 
 * This file contains implementations of inventory management
 * functions including CRUD operations for products.
 * 
 * LEARNING: Implementing business logic with validation
 */

#include "inventory.h"
#include "logger.h"
#include "validator.h"
#include "auth.h"
#include "database.h"
#include "utils.h"

// ============================================
// PRODUCT CRUD OPERATIONS
// ============================================

int inventory_create_product(sqlite3* db, const Product* product, 
                            const UserSession* session) {
    if (db == NULL || product == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "create_product", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate product data
    ValidationResult vresult = inventory_validate_product(product);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, session);
        return ERR_INVALID_INPUT;
    }
    
    // Check if SKU already exists for this user
    if (db_sku_exists(db, session->id, product->sku)) {
        log_error("SKU already exists", session);
        return ERR_DUPLICATE;
    }
    
    // Set owner and timestamps
    Product new_product = *product;
    new_product.user_id = session->id;
    get_timestamp(new_product.created_at, sizeof(new_product.created_at));
    get_timestamp(new_product.updated_at, sizeof(new_product.updated_at));
    
    // Create product in database
    int result = db_create_product(db, &new_product);
    if (result != ERR_NONE) {
        log_error("Failed to create product", session);
        return result;
    }
    
    // Log the event
    log_product_created(db, new_product.name, session);
    
    return ERR_NONE;
}

int inventory_get_product(sqlite3* db, int product_id, 
                         const UserSession* session, Product* product) {
    if (db == NULL || product == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get product (database enforces ownership)
    int result = db_get_product_by_id(db, product_id, session->id, product);
    if (result != ERR_NONE) {
        return result;
    }
    
    return ERR_NONE;
}

int inventory_update_product(sqlite3* db, int product_id, 
                            const Product* updates, 
                            const UserSession* session) {
    if (db == NULL || updates == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "update_product", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get existing product
    Product existing;
    int result = db_get_product_by_id(db, product_id, session->id, &existing);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Validate updated data
    ValidationResult vresult = inventory_validate_product(updates);
    if (!vresult.is_valid) {
        log_error(vresult.error_message, session);
        return ERR_INVALID_INPUT;
    }
    
    // Check if new SKU already exists (if changed)
    if (strcmp(existing.sku, updates->sku) != 0) {
        if (db_sku_exists(db, session->id, updates->sku)) {
            log_error("SKU already exists", session);
            return ERR_DUPLICATE;
        }
    }
    
    // Update product
    Product updated_product = *updates;
    updated_product.user_id = session->id;
    get_timestamp(updated_product.updated_at, sizeof(updated_product.updated_at));
    
    result = db_update_product(db, product_id, session->id, &updated_product);
    if (result != ERR_NONE) {
        log_error("Failed to update product", session);
        return result;
    }
    
    // Log the event
    log_product_updated(db, updated_product.name, session);
    
    return ERR_NONE;
}

int inventory_delete_product(sqlite3* db, int product_id, 
                            const UserSession* session) {
    if (db == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "delete_product", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Get product to log its name
    Product product;
    int result = db_get_product_by_id(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Delete product
    result = db_delete_product(db, product_id, session->id);
    if (result != ERR_NONE) {
        log_error("Failed to delete product", session);
        return result;
    }
    
    // Log the event
    log_product_deleted(db, product.name, session);
    
    return ERR_NONE;
}

// ============================================
// PRODUCT LISTING AND SEARCH
// ============================================

int inventory_list_products(sqlite3* db, const UserSession* session,
                            Product* products, int max_products) {
    if (db == NULL || session == NULL || products == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    return db_list_products(db, session->id, products, max_products);
}

int inventory_search_products(sqlite3* db, const char* search_term,
                             const UserSession* session,
                             Product* products, int max_products) {
    if (db == NULL || search_term == NULL || session == NULL || products == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    return db_search_products(db, session->id, search_term, products, max_products);
}

int inventory_list_by_category(sqlite3* db, const char* category,
                               const UserSession* session,
                               Product* products, int max_products) {
    if (db == NULL || category == NULL || session == NULL || products == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    // For simplicity, we'll get all products and filter
    int count = db_list_products(db, session->id, products, max_products);
    if (count < 0) {
        return -1;
    }
    
    // Filter by category
    int filtered_count = 0;
    for (int i = 0; i < count && filtered_count < max_products; i++) {
        if (strcmp(products[i].category, category) == 0) {
            if (i != filtered_count) {
                products[filtered_count] = products[i];
            }
            filtered_count++;
        }
    }
    
    return filtered_count;
}

int inventory_list_low_stock(sqlite3* db, int threshold,
                             const UserSession* session,
                             Product* products, int max_products) {
    if (db == NULL || session == NULL || products == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    // For simplicity, we'll get all products and filter
    int count = db_list_products(db, session->id, products, max_products);
    if (count < 0) {
        return -1;
    }
    
    // Filter by quantity
    int filtered_count = 0;
    for (int i = 0; i < count && filtered_count < max_products; i++) {
        if (products[i].quantity < threshold) {
            if (i != filtered_count) {
                products[filtered_count] = products[i];
            }
            filtered_count++;
        }
    }
    
    return filtered_count;
}

// ============================================
// PRODUCT QUANTITY MANAGEMENT
// ============================================

int inventory_update_quantity(sqlite3* db, int product_id,
                              int new_quantity,
                              const UserSession* session) {
    if (db == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "update_quantity", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate quantity
    if (new_quantity < 0) {
        return ERR_INVALID_INPUT;
    }
    
    // Get existing product
    Product product;
    int result = db_get_product_by_id(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Update quantity
    product.quantity = new_quantity;
    get_timestamp(product.updated_at, sizeof(product.updated_at));
    
    result = db_update_product(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        log_error("Failed to update quantity", session);
        return result;
    }
    
    // Log the event
    log_product_updated(db, product.name, session);
    
    return ERR_NONE;
}

int inventory_add_quantity(sqlite3* db, int product_id,
                           int amount,
                           const UserSession* session) {
    if (db == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "add_quantity", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate amount
    if (amount <= 0) {
        return ERR_INVALID_INPUT;
    }
    
    // Get existing product
    Product product;
    int result = db_get_product_by_id(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Update quantity
    product.quantity += amount;
    get_timestamp(product.updated_at, sizeof(product.updated_at));
    
    result = db_update_product(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        log_error("Failed to add quantity", session);
        return result;
    }
    
    // Log the event
    log_product_updated(db, product.name, session);
    
    return ERR_NONE;
}

int inventory_subtract_quantity(sqlite3* db, int product_id,
                                int amount,
                                const UserSession* session) {
    if (db == NULL || session == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        log_authorization_failure(db, "subtract_quantity", session);
        return ERR_NOT_AUTHORIZED;
    }
    
    // Validate amount
    if (amount <= 0) {
        return ERR_INVALID_INPUT;
    }
    
    // Get existing product
    Product product;
    int result = db_get_product_by_id(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Check if result would be negative
    if (product.quantity - amount < 0) {
        return ERR_INVALID_INPUT;
    }
    
    // Update quantity
    product.quantity -= amount;
    get_timestamp(product.updated_at, sizeof(product.updated_at));
    
    result = db_update_product(db, product_id, session->id, &product);
    if (result != ERR_NONE) {
        log_error("Failed to subtract quantity", session);
        return result;
    }
    
    // Log the event
    log_product_updated(db, product.name, session);
    
    return ERR_NONE;
}

// ============================================
// INVENTORY STATISTICS
// ============================================

int inventory_count_products(sqlite3* db, const UserSession* session) {
    if (db == NULL || session == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    // For simplicity, we'll get all products and count
    Product products[100];
    int count = db_list_products(db, session->id, products, 100);
    
    return count;
}

int inventory_get_total_value(sqlite3* db, const UserSession* session,
                              double* total_value) {
    if (db == NULL || session == NULL || total_value == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return ERR_NOT_AUTHORIZED;
    }
    
    // For simplicity, we'll get all products and calculate
    Product products[100];
    int count = db_list_products(db, session->id, products, 100);
    
    if (count < 0) {
        return ERR_DB_ERROR;
    }
    
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        total += products[i].price * products[i].quantity;
    }
    
    *total_value = total;
    return ERR_NONE;
}

int inventory_count_by_category(sqlite3* db, const UserSession* session,
                                const char* category) {
    if (db == NULL || session == NULL || category == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    // For simplicity, we'll get all products and count
    Product products[100];
    int count = db_list_products(db, session->id, products, 100);
    
    if (count < 0) {
        return -1;
    }
    
    int category_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(products[i].category, category) == 0) {
            category_count++;
        }
    }
    
    return category_count;
}

int inventory_count_low_stock(sqlite3* db, const UserSession* session,
                             int threshold) {
    if (db == NULL || session == NULL) {
        return -1;
    }
    
    // Check authentication
    if (!session_is_authenticated(session)) {
        return -1;
    }
    
    // For simplicity, we'll get all products and count
    Product products[100];
    int count = db_list_products(db, session->id, products, 100);
    
    if (count < 0) {
        return -1;
    }
    
    int low_stock_count = 0;
    for (int i = 0; i < count; i++) {
        if (products[i].quantity < threshold) {
            low_stock_count++;
        }
    }
    
    return low_stock_count;
}

// ============================================
// PRODUCT VALIDATION HELPERS
// ============================================

ValidationResult inventory_validate_product(const Product* product) {
    ValidationResult result;
    result.is_valid = 0;
    result.error_code = ERR_INVALID_INPUT;
    
    if (product == NULL) {
        safe_str_copy(result.error_message, "Product is NULL", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Validate name
    ValidationResult vresult = validate_product_name(product->name);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    // Validate description
    vresult = validate_product_description(product->description);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    // Validate SKU
    vresult = validate_sku(product->sku);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    // Validate price
    double price;
    vresult = validate_price("", &price);  // Skip for now
    if (!vresult.is_valid) {
        return vresult;
    }
    
    // Validate quantity
    if (product->quantity < 0) {
        safe_str_copy(result.error_message, "Quantity cannot be negative", 
                     sizeof(result.error_message));
        return result;
    }
    
    // Validate category
    vresult = validate_category(product->category);
    if (!vresult.is_valid) {
        return vresult;
    }
    
    result.is_valid = 1;
    result.error_code = ERR_NONE;
    result.error_message[0] = '\0';
    return result;
}

int inventory_sku_exists(sqlite3* db, const UserSession* session,
                        const char* sku) {
    if (db == NULL || session == NULL || sku == NULL) {
        return -1;
    }
    
    return db_sku_exists(db, session->id, sku);
}

int inventory_product_belongs_to_user(sqlite3* db, int product_id, int user_id) {
    if (db == NULL) {
        return -1;
    }
    
    Product product;
    int result = db_get_product_by_id(db, product_id, user_id, &product);
    
    return (result == ERR_NONE) ? 1 : 0;
}

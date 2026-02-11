/**
 * ============================================
 * inventory.h - Inventory Management Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding CRUD operations (Create, Read, Update, Delete)
 * - Business logic implementation
 * - Data validation in business layer
 * - Transaction management
 * 
 * KEY CONCEPTS:
 * 1. CRUD: Create, Read, Update, Delete operations
 * 2. Business Logic: Rules that govern how data is processed
 * 3. Validation: Ensuring data integrity
 * 4. Transactions: Grouping operations atomically
 * 
 * SECURITY NOTES:
 * - Users can only access their own products (row-level security)
 * - Validate all product data before database operations
 * - Use transactions for multi-step operations
 * - Log all data changes
 * - Verify ownership before operations
 * 
 * INVENTORY OPERATIONS:
 * - Create: Add new product to inventory
 * - Read: View products (list, search, details)
 * - Update: Modify existing product
 * - Delete: Remove product from inventory
 */

#ifndef INVENTORY_H
#define INVENTORY_H

#include "common.h"
#include "validator.h"
#include <sqlite3.h>

// ============================================
// PRODUCT CRUD OPERATIONS
// ============================================

/**
 * Create a new product
 * 
 * Validates product data and creates it in the database.
 * The product is automatically assigned to the current user.
 * 
 * @param db Database connection
 * @param product Product structure with product data
 * @param session User session (for ownership and authorization)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Validates all product data
 * - Verifies user is authenticated
 * - Assigns product to current user
 * - Logs the event
 * 
 * LEARNING: Create operation with validation
 */
int inventory_create_product(sqlite3* db, const Product* product, 
                            const UserSession* session);

/**
 * Get product details
 * 
 * Retrieves a product by ID.
 * Only returns products owned by the current user.
 * 
 * @param db Database connection
 * @param product_id Product ID to retrieve
 * @param session User session (for ownership check)
 * @param product Output parameter for product data
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Row-level security - users only see their own products
 */
int inventory_get_product(sqlite3* db, int product_id, 
                         const UserSession* session, Product* product);

/**
 * Update a product
 * 
 * Modifies an existing product.
 * Only products owned by the current user can be updated.
 * 
 * @param db Database connection
 * @param product_id Product ID to update
 * @param updates Product structure with updated fields
 * @param session User session (for ownership check)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Verifies ownership
 * - Validates updated data
 * - Logs the event
 */
int inventory_update_product(sqlite3* db, int product_id, 
                            const Product* updates, 
                            const UserSession* session);

/**
 * Delete a product
 * 
 * Removes a product from the database.
 * Only products owned by the current user can be deleted.
 * 
 * @param db Database connection
 * @param product_id Product ID to delete
 * @param session User session (for ownership check)
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY:
 * - Verifies ownership
 * - Uses transaction for atomicity
 * - Logs the event
 */
int inventory_delete_product(sqlite3* db, int product_id, 
                            const UserSession* session);

// ============================================
// PRODUCT LISTING AND SEARCH
// ============================================

/**
 * List all products for a user
 * 
 * Retrieves all products owned by the current user.
 * 
 * @param db Database connection
 * @param session User session
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 * 
 * SECURITY: Only returns user's own products
 */
int inventory_list_products(sqlite3* db, const UserSession* session,
                            Product* products, int max_products);

/**
 * Search products by name or SKU
 * 
 * Searches for products matching the search term.
 * Only searches within the user's own products.
 * 
 * @param db Database connection
 * @param search_term Search term (can be name or SKU)
 * @param session User session
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 * 
 * SECURITY: Uses parameterized query with LIKE
 */
int inventory_search_products(sqlite3* db, const char* search_term,
                             const UserSession* session,
                             Product* products, int max_products);

/**
 * List products by category
 * 
 * Retrieves all products in a specific category.
 * 
 * @param db Database connection
 * @param category Category to filter by
 * @param session User session
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 */
int inventory_list_by_category(sqlite3* db, const char* category,
                               const UserSession* session,
                               Product* products, int max_products);

/**
 * List low stock products
 * 
 * Retrieves products with quantity below threshold.
 * 
 * @param db Database connection
 * @param threshold Minimum quantity threshold
 * @param session User session
 * @param products Output array for products
 * @param max_products Maximum number of products to retrieve
 * @return Number of products retrieved, or negative error code
 */
int inventory_list_low_stock(sqlite3* db, int threshold,
                             const UserSession* session,
                             Product* products, int max_products);

// ============================================
// PRODUCT QUANTITY MANAGEMENT
// ============================================

/**
 * Update product quantity
 * 
 * Changes the quantity of a product.
 * 
 * @param db Database connection
 * @param product_id Product ID to update
 * @param new_quantity New quantity value
 * @param session User session
 * @return ERR_NONE on success, error code on failure
 */
int inventory_update_quantity(sqlite3* db, int product_id,
                              int new_quantity,
                              const UserSession* session);

/**
 * Add to product quantity
 * 
 * Increases the quantity of a product.
 * 
 * @param db Database connection
 * @param product_id Product ID to update
 * @param amount Amount to add (must be positive)
 * @param session User session
 * @return ERR_NONE on success, error code on failure
 */
int inventory_add_quantity(sqlite3* db, int product_id,
                           int amount,
                           const UserSession* session);

/**
 * Subtract from product quantity
 * 
 * Decreases the quantity of a product.
 * Prevents negative quantities.
 * 
 * @param db Database connection
 * @param product_id Product ID to update
 * @param amount Amount to subtract (must be positive)
 * @param session User session
 * @return ERR_NONE on success, error code on failure
 */
int inventory_subtract_quantity(sqlite3* db, int product_id,
                                int amount,
                                const UserSession* session);

// ============================================
// INVENTORY STATISTICS
// ============================================

/**
 * Get total number of products for a user
 * 
 * @param db Database connection
 * @param session User session
 * @return Number of products, or negative error code
 */
int inventory_count_products(sqlite3* db, const UserSession* session);

/**
 * Get total inventory value
 * 
 * Calculates the sum of (price * quantity) for all products.
 * 
 * @param db Database connection
 * @param session User session
 * @param total_value Output parameter for total value
 * @return ERR_NONE on success, error code on failure
 */
int inventory_get_total_value(sqlite3* db, const UserSession* session,
                              double* total_value);

/**
 * Get product count by category
 * 
 * @param db Database connection
 * @param session User session
 * @param category Category to count
 * @return Number of products in category, or negative error code
 */
int inventory_count_by_category(sqlite3* db, const UserSession* session,
                                const char* category);

/**
 * Get low stock count
 * 
 * Counts products with quantity below threshold.
 * 
 * @param db Database connection
 * @param session User session
 * @param threshold Minimum quantity threshold
 * @return Number of low stock products, or negative error code
 */
int inventory_count_low_stock(sqlite3* db, const UserSession* session,
                             int threshold);

// ============================================
// PRODUCT VALIDATION HELPERS
// ============================================

/**
 * Validate product data
 * 
 * Checks all product fields for validity.
 * 
 * @param product Product to validate
 * @return ValidationResult with validation status
 */
ValidationResult inventory_validate_product(const Product* product);

/**
 * Check if SKU exists for a user
 * 
 * @param db Database connection
 * @param session User session
 * @param sku SKU to check
 * @return 1 if SKU exists, 0 if not, negative on error
 */
int inventory_sku_exists(sqlite3* db, const UserSession* session,
                        const char* sku);

/**
 * Check if product belongs to user
 * 
 * @param db Database connection
 * @param product_id Product ID to check
 * @param user_id User ID to verify ownership
 * @return 1 if owned by user, 0 if not, negative on error
 */
int inventory_product_belongs_to_user(sqlite3* db, int product_id, int user_id);

#endif // INVENTORY_H

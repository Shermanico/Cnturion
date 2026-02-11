/**
 * ============================================
 * ui.h - User Interface Header
 * ============================================
 * 
 * LEARNING OBJECTIVES:
 * - Understanding console I/O in C
 * - Menu systems
 * - User input handling
 * - Display formatting
 * 
 * KEY CONCEPTS:
 * 1. Console I/O: printf for output, scanf/fgets for input
 * 2. Menu System: Hierarchical navigation
 * 3. Input Handling: Reading and validating user input
 * 4. Display Formatting: Making output readable
 * 
 * SECURITY NOTES:
 * - Use fgets instead of scanf for strings (prevents buffer overflow)
 * - Always validate user input
 * - Clear sensitive data from memory when done
 * - Display generic error messages
 * - Limit input length
 * 
 * UI STRUCTURE:
 * - Main Menu: Top-level navigation
 * - Login Screen: User authentication
 * - Product Menu: Inventory management
 * - User Menu: User management (admin only)
 * - Forms: Data entry for create/update operations
 */

#ifndef UI_H
#define UI_H

#include "common.h"
#include <sqlite3.h>

// ============================================
// INITIALIZATION
// ============================================

/**
 * Initialize the UI system
 * 
 * Sets up the console and prepares for user interaction.
 * 
 * @return ERR_NONE on success, error code on failure
 */
int ui_init(void);

/**
 * Clean up the UI system
 * 
 * Performs cleanup before exiting.
 * 
 * @return ERR_NONE on success, error code on failure
 */
int ui_cleanup(void);

/**
 * Clear the console screen
 * 
 * Clears the terminal screen for a fresh display.
 */
void ui_clear_screen(void);

// ============================================
// DISPLAY FUNCTIONS
// ============================================

/**
 * Display the application banner
 * 
 * Shows the application name and welcome message.
 */
void ui_display_banner(void);

/**
 * Display a separator line
 * 
 * Prints a horizontal line for visual separation.
 */
void ui_display_separator(void);

/**
 * Display a success message
 * 
 * @param message Success message to display
 */
void ui_display_success(const char* message);

/**
 * Display an error message
 * 
 * @param message Error message to display
 */
void ui_display_error(const char* message);

/**
 * Display a warning message
 * 
 * @param message Warning message to display
 */
void ui_display_warning(const char* message);

/**
 * Display an info message
 * 
 * @param message Info message to display
 */
void ui_display_info(const char* message);

// ============================================
// INPUT FUNCTIONS
// ============================================

/**
 * Get string input from user
 * 
 * Reads a line of text from the user.
 * 
 * @param prompt Prompt to display to user
 * @param buffer Buffer to store input
 * @param buffer_size Size of buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * LEARNING: Safe input reading with fgets
 * SECURITY: Limits input length to prevent buffer overflow
 */
int ui_get_string(const char* prompt, char* buffer, size_t buffer_size);

/**
 * Get password input from user
 * 
 * Reads a password without displaying characters.
 * 
 * @param prompt Prompt to display to user
 * @param buffer Buffer to store password
 * @param buffer_size Size of buffer
 * @return ERR_NONE on success, error code on failure
 * 
 * SECURITY: Hides password from display
 */
int ui_get_password(const char* prompt, char* buffer, size_t buffer_size);

/**
 * Get integer input from user
 * 
 * Reads an integer value from the user.
 * 
 * @param prompt Prompt to display to user
 * @param value Pointer to store the value
 * @return ERR_NONE on success, error code on failure
 */
int ui_get_int(const char* prompt, int* value);

/**
 * Get double input from user
 * 
 * Reads a floating-point value from the user.
 * 
 * @param prompt Prompt to display to user
 * @param value Pointer to store the value
 * @return ERR_NONE on success, error code on failure
 */
int ui_get_double(const char* prompt, double* value);

/**
 * Get yes/no confirmation from user
 * 
 * @param prompt Prompt to display to user
 * @return 1 for yes, 0 for no
 */
int ui_get_confirmation(const char* prompt);

/**
 * Get menu option from user
 * 
 * @param options Array of option strings
 * @param num_options Number of options
 * @param min_value Minimum valid value
 * @param max_value Maximum valid value
 * @return Selected option value
 */
int ui_get_menu_option(const char** options, int num_options, 
                       int min_value, int max_value);

/**
 * Press any key to continue
 * 
 * Waits for user to press a key.
 */
void ui_press_any_key(void);

// ============================================
// MAIN MENU
// ============================================

/**
 * Display and handle the main menu
 * 
 * Shows options based on user role and handles selection.
 * 
 * @param db Database connection
 * @param session Current user session
 * @return 0 to continue, 1 to exit
 */
int ui_main_menu(sqlite3* db, UserSession* session);

/**
 * Display the login screen
 * 
 * Prompts user for username and password.
 * 
 * @param db Database connection
 * @param session Output parameter for authenticated session
 * @return ERR_NONE on success, error code on failure
 */
int ui_login_screen(sqlite3* db, UserSession* session);

// ============================================
// PRODUCT MENU
// ============================================

/**
 * Display the product management menu
 * 
 * Shows options for managing products.
 * 
 * @param db Database connection
 * @param session Current user session
 * @return 0 to continue, 1 to go back
 */
int ui_product_menu(sqlite3* db, UserSession* session);

/**
 * Display the add product form
 * 
 * Collects product information from user.
 * 
 * @param product Output parameter for product data
 * @return ERR_NONE on success, error code on failure
 */
int ui_add_product_form(Product* product);

/**
 * Display the edit product form
 * 
 * Collects updated product information from user.
 * 
 * @param product Product to edit (will be updated)
 * @return ERR_NONE on success, error code on failure
 */
int ui_edit_product_form(Product* product);

/**
 * Display product list
 * 
 * Shows a table of products.
 * 
 * @param products Array of products
 * @param count Number of products
 */
void ui_display_product_list(const Product* products, int count);

/**
 * Display product details
 * 
 * Shows detailed information about a single product.
 * 
 * @param product Product to display
 */
void ui_display_product_details(const Product* product);

/**
 * Prompt for product ID
 * 
 * @param prompt Prompt to display
 * @param product_id Output parameter for product ID
 * @return ERR_NONE on success, error code on failure
 */
int ui_prompt_product_id(const char* prompt, int* product_id);

// ============================================
// USER MENU (ADMIN ONLY)
// ============================================

/**
 * Display the user management menu
 * 
 * Shows options for managing users.
 * 
 * @param db Database connection
 * @param session Current admin session
 * @return 0 to continue, 1 to go back
 */
int ui_user_menu(sqlite3* db, UserSession* session);

/**
 * Display the add user form
 * 
 * Collects user information from admin.
 * 
 * @param username Output parameter for username
 * @param email Output parameter for email
 * @param password Output parameter for password
 * @param role Output parameter for role
 * @return ERR_NONE on success, error code on failure
 */
int ui_add_user_form(char* username, char* email, char* password, UserRole* role);

/**
 * Display user list
 * 
 * Shows a table of users.
 * 
 * @param users Array of users
 * @param count Number of users
 */
void ui_display_user_list(const User* users, int count);

/**
 * Display user details
 * 
 * Shows detailed information about a single user.
 * 
 * @param user User to display
 */
void ui_display_user_details(const User* user);

/**
 * Prompt for user ID
 * 
 * @param prompt Prompt to display
 * @param user_id Output parameter for user ID
 * @return ERR_NONE on success, error code on failure
 */
int ui_prompt_user_id(const char* prompt, int* user_id);

/**
 * Prompt for role selection
 * 
 * @param role Output parameter for selected role
 * @return ERR_NONE on success, error code on failure
 */
int ui_prompt_role(UserRole* role);

// ============================================
// AUDIT LOGS MENU (ADMIN ONLY)
// ============================================

/**
 * Display the audit logs menu
 * 
 * Shows options for viewing audit logs.
 * 
 * @param db Database connection
 * @param session Current admin session
 * @return 0 to continue, 1 to go back
 */
int ui_audit_menu(sqlite3* db, UserSession* session);

/**
 * Display audit log entries
 * 
 * Shows a table of audit log entries.
 * 
 * @param logs Array of audit logs
 * @param count Number of logs
 */
void ui_display_audit_logs(const AuditLog* logs, int count);

// ============================================
// SEARCH AND FILTER
// ============================================

/**
 * Prompt for search term
 * 
 * @param search_term Output parameter for search term
 * @return ERR_NONE on success, error code on failure
 */
int ui_prompt_search_term(char* search_term);

/**
 * Prompt for category filter
 * 
 * @param category Output parameter for category
 * @return ERR_NONE on success, error code on failure
 */
int ui_prompt_category(char* category);

// ============================================
// STATISTICS DISPLAY
// ============================================

/**
 * Display inventory statistics
 * 
 * Shows summary statistics about the user's inventory.
 * 
 * @param total_products Total number of products
 * @param total_value Total inventory value
 * @param low_stock_count Number of low stock items
 */
void ui_display_inventory_stats(int total_products, double total_value,
                                int low_stock_count);

/**
 * Display user statistics
 * 
 * Shows summary statistics about users.
 * 
 * @param total_users Total number of users
 * @param admin_count Number of admins
 * @param user_count Number of regular users
 */
void ui_display_user_stats(int total_users, int admin_count, int user_count);

#endif // UI_H

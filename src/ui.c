/**
 * ============================================
 * ui.c - User Interface Implementation
 * ============================================
 * 
 * This file contains implementations of user interface functions
 * including menus, forms, and display functions.
 * 
 * LEARNING: Console I/O and menu systems in C
 */

#include "ui.h"
#include "auth.h"
#include "inventory.h"
#include "users.h"
#include "logger.h"
#include "database.h"
#include "utils.h"
#include "validator.h"
#include <termios.h>
#include <unistd.h>

// ============================================
// INITIALIZATION
// ============================================

int ui_init(void) {
    // Initialize logger
    return logger_init(NULL);
}

int ui_cleanup(void) {
    return logger_close();
}

void ui_clear_screen(void) {
    // ANSI escape code to clear screen
    printf("\033[2J\033[H");
}

// ============================================
// DISPLAY FUNCTIONS
// ============================================

void ui_display_banner(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                                                            ║\n");
    printf("║           Cnturion Inventory Manager                     ║\n");
    printf("║                                                            ║\n");
    printf("║           Secure Inventory Management for Micro Businesses     ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void ui_display_separator(void) {
    printf("────────────────────────────────────────────────────────────\n");
}

void ui_display_success(const char* message) {
    printf("\n✓ %s\n\n", message);
}

void ui_display_error(const char* message) {
    printf("\n✗ %s\n\n", message);
}

void ui_display_warning(const char* message) {
    printf("\n⚠ %s\n\n", message);
}

void ui_display_info(const char* message) {
    printf("\nℹ %s\n\n", message);
}

// ============================================
// INPUT FUNCTIONS
// ============================================

int ui_get_string(const char* prompt, char* buffer, size_t buffer_size) {
    if (prompt == NULL || buffer == NULL || buffer_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    printf("%s", prompt);
    fflush(stdout);
    
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    // Remove newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return ERR_NONE;
}

int ui_get_password(const char* prompt, char* buffer, size_t buffer_size) {
    if (prompt == NULL || buffer == NULL || buffer_size == 0) {
        return ERR_INVALID_INPUT;
    }
    
    printf("%s", prompt);
    fflush(stdout);
    
    // Disable echo
    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        // Restore echo
        tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
        printf("\n");
        return ERR_INVALID_INPUT;
    }
    
    // Restore echo
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    printf("\n");
    
    // Remove newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return ERR_NONE;
}

int ui_get_int(const char* prompt, int* value) {
    if (prompt == NULL || value == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char buffer[50];
    if (ui_get_string(prompt, buffer, sizeof(buffer)) != ERR_NONE) {
        return ERR_INVALID_INPUT;
    }
    
    return str_to_int(buffer, value);
}

int ui_get_double(const char* prompt, double* value) {
    if (prompt == NULL || value == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    char buffer[50];
    if (ui_get_string(prompt, buffer, sizeof(buffer)) != ERR_NONE) {
        return ERR_INVALID_INPUT;
    }
    
    return str_to_double(buffer, value);
}

int ui_get_confirmation(const char* prompt) {
    char buffer[10];
    
    while (1) {
        printf("%s (y/n): ", prompt);
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0;
        }
        
        str_to_lower(buffer);
        
        if (buffer[0] == 'y') {
            return 1;
        } else if (buffer[0] == 'n') {
            return 0;
        }
        
        printf("Please enter 'y' or 'n'.\n");
    }
}

int ui_get_menu_option(const char** options, int num_options, 
                       int min_value, int max_value) {
    if (options == NULL || num_options == 0) {
        return -1;
    }
    
    int value;
    
    while (1) {
        printf("\n");
        for (int i = 0; i < num_options; i++) {
            printf("  %d. %s\n", min_value + i, options[i]);
        }
        
        printf("\nSelect option (%d-%d): ", min_value, max_value);
        fflush(stdout);
        
        char buffer[10];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        
        if (str_to_int(buffer, &value) == ERR_NONE) {
            if (value >= min_value && value <= max_value) {
                return value;
            }
        }
        
        printf("Invalid option. Please try again.\n");
    }
}

void ui_press_any_key(void) {
    printf("\nPress Enter to continue...");
    fflush(stdout);
    getchar();
}

// ============================================
// MAIN MENU
// ============================================

int ui_main_menu(sqlite3* db, UserSession* session) {
    while (1) {
        ui_clear_screen();
        ui_display_banner();
        
        printf("Logged in as: %s (%s)\n", session->username, 
               role_to_string(session->role));
        ui_display_separator();
        
        // Common options
        const char* common_options[] = {
            "View Products",
            "Add Product",
            "Update Product",
            "Delete Product",
            "Search Products",
            "Logout"
        };
        
        int num_common = ARRAY_SIZE(common_options);
        int choice = ui_get_menu_option(common_options, num_common, 1, num_common);
        
        switch (choice) {
            case 1:
                ui_product_menu(db, session);
                break;
            case 2: {
                Product product;
                if (ui_add_product_form(&product) == ERR_NONE) {
                    if (inventory_create_product(db, &product, session) == ERR_NONE) {
                        ui_display_success("Product created successfully!");
                    } else {
                        ui_display_error("Failed to create product.");
                    }
                }
                break;
            }
            case 3: {
                int product_id;
                if (ui_prompt_product_id("Enter product ID to update: ", &product_id) == ERR_NONE) {
                    Product product;
                    if (inventory_get_product(db, product_id, session, &product) == ERR_NONE) {
                        if (ui_edit_product_form(&product) == ERR_NONE) {
                            if (inventory_update_product(db, product_id, &product, session) == ERR_NONE) {
                                ui_display_success("Product updated successfully!");
                            } else {
                                ui_display_error("Failed to update product.");
                            }
                        }
                    } else {
                        ui_display_error("Product not found.");
                    }
                }
                break;
            }
            case 4: {
                int product_id;
                if (ui_prompt_product_id("Enter product ID to delete: ", &product_id) == ERR_NONE) {
                    if (ui_get_confirmation("Are you sure you want to delete this product?")) {
                        if (inventory_delete_product(db, product_id, session) == ERR_NONE) {
                            ui_display_success("Product deleted successfully!");
                        } else {
                            ui_display_error("Failed to delete product.");
                        }
                    }
                }
                break;
            }
            case 5: {
                char search_term[100];
                if (ui_prompt_search_term(search_term) == ERR_NONE) {
                    Product products[50];
                    int count = inventory_search_products(db, search_term, session, products, 50);
                    if (count > 0) {
                        ui_display_product_list(products, count);
                    } else {
                        ui_display_info("No products found matching your search.");
                    }
                }
                break;
            }
            case 6:
                return 0;  // Logout
        }
        
        ui_press_any_key();
    }
}

int ui_login_screen(sqlite3* db, UserSession* session) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    
    ui_clear_screen();
    ui_display_banner();
    printf("Login\n");
    ui_display_separator();
    
    if (ui_get_string("Username: ", username, sizeof(username)) != ERR_NONE) {
        return ERR_INVALID_INPUT;
    }
    
    if (ui_get_password("Password: ", password, sizeof(password)) != ERR_NONE) {
        return ERR_INVALID_INPUT;
    }
    
    UserSession* new_session = authenticate(db, username, password);
    if (new_session == NULL) {
        ui_display_error("Invalid username or password.");
        return ERR_AUTH_FAILED;
    }
    
    // Copy session data
    *session = *new_session;
    safe_free((void**)&new_session);
    
    ui_display_success("Login successful!");
    return ERR_NONE;
}

// ============================================
// PRODUCT MENU
// ============================================

int ui_product_menu(sqlite3* db, UserSession* session) {
    Product products[100];
    int count = inventory_list_products(db, session, products, 100);
    
    if (count < 0) {
        ui_display_error("Failed to retrieve products.");
        return 1;
    }
    
    ui_clear_screen();
    printf("Your Products\n");
    ui_display_separator();
    
    if (count == 0) {
        printf("No products found.\n");
    } else {
        ui_display_product_list(products, count);
        
        // Show statistics
        double total_value;
        if (inventory_get_total_value(db, session, &total_value) == ERR_NONE) {
            printf("\nTotal Products: %d\n", count);
            printf("Total Value: $%.2f\n", total_value);
        }
    }
    
    return 1;
}

int ui_add_product_form(Product* product) {
    if (product == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    ui_clear_screen();
    printf("Add New Product\n");
    ui_display_separator();
    
    char buffer[500];
    
    // Product name
    while (1) {
        if (ui_get_string("Product Name: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        ValidationResult vresult = validate_product_name(buffer);
        if (vresult.is_valid) {
            safe_str_copy(product->name, buffer, sizeof(product->name));
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Description (optional)
    if (ui_get_string("Description (optional, press Enter to skip): ", buffer, sizeof(buffer)) == ERR_NONE) {
        safe_str_copy(product->description, buffer, sizeof(product->description));
    }
    
    // SKU
    while (1) {
        if (ui_get_string("SKU (Stock Keeping Unit): ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        ValidationResult vresult = validate_sku(buffer);
        if (vresult.is_valid) {
            safe_str_copy(product->sku, buffer, sizeof(product->sku));
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Price
    while (1) {
        if (ui_get_string("Price: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        double price;
        ValidationResult vresult = validate_price(buffer, &price);
        if (vresult.is_valid) {
            product->price = price;
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Quantity
    while (1) {
        if (ui_get_string("Quantity: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        int quantity;
        ValidationResult vresult = validate_quantity(buffer, &quantity);
        if (vresult.is_valid) {
            product->quantity = quantity;
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Category (optional)
    if (ui_get_string("Category (optional, press Enter to skip): ", buffer, sizeof(buffer)) == ERR_NONE) {
        safe_str_copy(product->category, buffer, sizeof(product->category));
    }
    
    return ERR_NONE;
}

int ui_edit_product_form(Product* product) {
    if (product == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    ui_clear_screen();
    printf("Edit Product\n");
    ui_display_separator();
    printf("Current values shown. Press Enter to keep current value.\n\n");
    
    char buffer[500];
    
    // Product name
    printf("Product Name [%s]: ", product->name);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        if (buffer[0] != '\0') {
            safe_str_copy(product->name, buffer, sizeof(product->name));
        }
    }
    
    // Description
    printf("Description [%s]: ", product->description);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        safe_str_copy(product->description, buffer, sizeof(product->description));
    }
    
    // SKU
    printf("SKU [%s]: ", product->sku);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        if (buffer[0] != '\0') {
            safe_str_copy(product->sku, buffer, sizeof(product->sku));
        }
    }
    
    // Price
    printf("Price [%.2f]: ", product->price);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        if (buffer[0] != '\0') {
            double price;
            if (str_to_double(buffer, &price) == ERR_NONE) {
                product->price = price;
            }
        }
    }
    
    // Quantity
    printf("Quantity [%d]: ", product->quantity);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        if (buffer[0] != '\0') {
            int quantity;
            if (str_to_int(buffer, &quantity) == ERR_NONE) {
                product->quantity = quantity;
            }
        }
    }
    
    // Category
    printf("Category [%s]: ", product->category);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL && buffer[0] != '\n') {
        str_trim(buffer);
        safe_str_copy(product->category, buffer, sizeof(product->category));
    }
    
    return ERR_NONE;
}

void ui_display_product_list(const Product* products, int count) {
    if (products == NULL || count == 0) {
        return;
    }
    
    printf("\n┌────┬─────────────────────────┬────────────┬──────────┬──────────┬─────────────┐\n");
    printf("│ ID │ Name                    │ SKU        │ Price    │ Quantity │ Category    │\n");
    printf("├────┼─────────────────────────┼────────────┼──────────┼──────────┼─────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        printf("│ %3d │ %-22s │ %-10s │ $%7.2f │ %8d │ %-11s │\n",
               products[i].id, products[i].name, products[i].sku,
               products[i].price, products[i].quantity, products[i].category);
    }
    
    printf("└────┴─────────────────────────┴────────────┴──────────┴──────────┴─────────────┘\n");
}

void ui_display_product_details(const Product* product) {
    if (product == NULL) {
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║ Product Details                                      ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ ID:           %-36d ║\n", product->id);
    printf("║ Name:         %-36s ║\n", product->name);
    printf("║ Description:  %-36s ║\n", product->description);
    printf("║ SKU:          %-36s ║\n", product->sku);
    printf("║ Price:        $%-35.2f ║\n", product->price);
    printf("║ Quantity:     %-36d ║\n", product->quantity);
    printf("║ Category:     %-36s ║\n", product->category);
    printf("║ Created:      %-36s ║\n", product->created_at);
    printf("║ Updated:      %-36s ║\n", product->updated_at);
    printf("╚════════════════════════════════════════════════════════╝\n");
}

int ui_prompt_product_id(const char* prompt, int* product_id) {
    return ui_get_int(prompt, product_id);
}

// ============================================
// USER MENU (ADMIN ONLY)
// ============================================

int ui_user_menu(sqlite3* db, UserSession* session) {
    User users[50];
    int count = users_list_all(db, users, 50, session);
    
    if (count < 0) {
        ui_display_error("Failed to retrieve users.");
        return 1;
    }
    
    ui_clear_screen();
    printf("User Management\n");
    ui_display_separator();
    
    if (count == 0) {
        printf("No users found.\n");
    } else {
        ui_display_user_list(users, count);
    }
    
    return 1;
}

int ui_add_user_form(char* username, char* email, char* password, UserRole* role) {
    if (username == NULL || email == NULL || password == NULL || role == NULL) {
        return ERR_INVALID_INPUT;
    }
    
    ui_clear_screen();
    printf("Add New User\n");
    ui_display_separator();
    
    char buffer[100];
    
    // Username
    while (1) {
        if (ui_get_string("Username: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        ValidationResult vresult = validate_username(buffer);
        if (vresult.is_valid) {
            safe_str_copy(username, buffer, MAX_USERNAME_LEN);
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Email
    while (1) {
        if (ui_get_string("Email: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        ValidationResult vresult = validate_email(buffer);
        if (vresult.is_valid) {
            safe_str_copy(email, buffer, MAX_EMAIL_LEN);
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Password
    while (1) {
        if (ui_get_password("Password: ", buffer, sizeof(buffer)) != ERR_NONE) {
            return ERR_INVALID_INPUT;
        }
        
        ValidationResult vresult = validate_password(buffer);
        if (vresult.is_valid) {
            safe_str_copy(password, buffer, MAX_PASSWORD_LEN);
            break;
        }
        ui_display_error(vresult.error_message);
    }
    
    // Role
    if (ui_prompt_role(role) != ERR_NONE) {
        return ERR_INVALID_INPUT;
    }
    
    return ERR_NONE;
}

void ui_display_user_list(const User* users, int count) {
    if (users == NULL || count == 0) {
        return;
    }
    
    printf("\n┌────┬─────────────────────────┬─────────────────────────┬────────┬────────┬─────────┐\n");
    printf("│ ID │ Username               │ Email                  │ Role   │ Active  │ Created │\n");
    printf("├────┼─────────────────────────┼─────────────────────────┼────────┼────────┼─────────┤\n");
    
    for (int i = 0; i < count; i++) {
        printf("│ %3d │ %-21s │ %-21s │ %-6s │ %-6s │ %-7s │\n",
               users[i].id, users[i].username, users[i].email,
               role_to_string(users[i].role),
               users[i].is_active ? "Yes" : "No",
               users[i].created_at);
    }
    
    printf("└────┴─────────────────────────┴─────────────────────────┴────────┴────────┴─────────┘\n");
}

void ui_display_user_details(const User* user) {
    if (user == NULL) {
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║ User Details                                         ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ ID:           %-38d ║\n", user->id);
    printf("║ Username:     %-38s ║\n", user->username);
    printf("║ Email:        %-38s ║\n", user->email);
    printf("║ Role:         %-38s ║\n", role_to_string(user->role));
    printf("║ Active:       %-38s ║\n", user->is_active ? "Yes" : "No");
    printf("║ Created:      %-38s ║\n", user->created_at);
    printf("║ Last Login:   %-38s ║\n", user->last_login);
    printf("╚════════════════════════════════════════════════════════╝\n");
}

int ui_prompt_user_id(const char* prompt, int* user_id) {
    return ui_get_int(prompt, user_id);
}

int ui_prompt_role(UserRole* role) {
    const char* options[] = {
        "Regular User",
        "Administrator"
    };
    
    int choice = ui_get_menu_option(options, 2, 1, 2);
    *role = (choice == 2) ? ROLE_ADMIN : ROLE_USER;
    
    return ERR_NONE;
}

// ============================================
// AUDIT LOGS MENU (ADMIN ONLY)
// ============================================

int ui_audit_menu(sqlite3* db, UserSession* session) {
    (void)session;  // Unused parameter
    AuditLog logs[50];
    int count = db_get_recent_audit_logs(db, logs, 50);
    
    if (count < 0) {
        ui_display_error("Failed to retrieve audit logs.");
        return 1;
    }
    
    ui_clear_screen();
    printf("Audit Logs\n");
    ui_display_separator();
    
    if (count == 0) {
        printf("No audit logs found.\n");
    } else {
        ui_display_audit_logs(logs, count);
    }
    
    return 1;
}

void ui_display_audit_logs(const AuditLog* logs, int count) {
    if (logs == NULL || count == 0) {
        return;
    }
    
    printf("\n");
    for (int i = 0; i < count; i++) {
        printf("[%s] %s - %s: %s\n", 
               logs[i].timestamp, logs[i].action, logs[i].resource,
               logs[i].success ? "SUCCESS" : "FAILURE");
        if (logs[i].details[0] != '\0') {
            printf("  Details: %s\n", logs[i].details);
        }
        printf("\n");
    }
}

// ============================================
// SEARCH AND FILTER
// ============================================

int ui_prompt_search_term(char* search_term) {
    return ui_get_string("Enter search term: ", search_term, 100);
}

int ui_prompt_category(char* category) {
    return ui_get_string("Enter category: ", category, MAX_CATEGORY_LEN);
}

// ============================================
// STATISTICS DISPLAY
// ============================================

void ui_display_inventory_stats(int total_products, double total_value,
                                int low_stock_count) {
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║ Inventory Statistics                                 ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Total Products:     %-31d ║\n", total_products);
    printf("║ Total Value:       $%-30.2f ║\n", total_value);
    printf("║ Low Stock Items:   %-31d ║\n", low_stock_count);
    printf("╚════════════════════════════════════════════════════════╝\n");
}

void ui_display_user_stats(int total_users, int admin_count, int user_count) {
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║ User Statistics                                    ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Total Users:       %-31d ║\n", total_users);
    printf("║ Administrators:    %-31d ║\n", admin_count);
    printf("║ Regular Users:    %-31d ║\n", user_count);
    printf("╚════════════════════════════════════════════════════════╝\n");
}

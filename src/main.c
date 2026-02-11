/**
 * ============================================
 * main.c - Application Entry Point
 * ============================================
 * 
 * This is the main entry point for the Cnturion Inventory Manager.
 * 
 * LEARNING: Understanding the main() function and program flow
 */

#include "common.h"
#include "database.h"
#include "auth.h"
#include "ui.h"
#include "logger.h"
#include "security.h"
#include "utils.h"

// ============================================
// CONFIGURATION
// ============================================

#define DB_PATH "data/inventory.db"
#define DEFAULT_ADMIN_USERNAME "admin"
#define DEFAULT_ADMIN_EMAIL "admin@cnturion.local"
#define DEFAULT_ADMIN_PASSWORD "Admin123!"

// ============================================
// HELPER FUNCTIONS
// ============================================

/**
 * Create default admin user
 * 
 * Creates an admin account if no users exist in the database.
 * This is useful for initial setup.
 * 
 * @param db Database connection
 * @return ERR_NONE on success, error code on failure
 */
int create_default_admin(sqlite3* db) {
    // Check if any users exist
    User users[10];
    int count = db_list_users(db, users, 10);
    
    if (count > 0) {
        // Users already exist, don't create default admin
        return ERR_NONE;
    }
    
    // Create default admin
    User admin;
    memset(&admin, 0, sizeof(admin));
    safe_str_copy(admin.username, DEFAULT_ADMIN_USERNAME, sizeof(admin.username));
    safe_str_copy(admin.email, DEFAULT_ADMIN_EMAIL, sizeof(admin.email));
    safe_str_copy(admin.created_at, "2026-01-01 00:00:00", sizeof(admin.created_at));
    admin.role = ROLE_ADMIN;
    admin.is_active = 1;
    
    // Hash default password
    int result = hash_password(DEFAULT_ADMIN_PASSWORD, admin.password_hash, 
                           sizeof(admin.password_hash));
    if (result != ERR_NONE) {
        return result;
    }
    
    safe_str_copy(admin.salt, "included_in_hash", sizeof(admin.salt));
    
    // Create admin in database
    result = db_create_user(db, &admin);
    if (result != ERR_NONE) {
        return result;
    }
    
    // Log the event
    log_info("Default admin account created", NULL);
    
    printf("\n========================================\n");
    printf("DEFAULT ADMIN ACCOUNT CREATED\n");
    printf("========================================\n");
    printf("Username: %s\n", DEFAULT_ADMIN_USERNAME);
    printf("Password: %s\n", DEFAULT_ADMIN_PASSWORD);
    printf("========================================\n");
    printf("Please change this password after first login!\n");
    printf("========================================\n\n");
    
    return ERR_NONE;
}

/**
 * Display welcome screen
 */
void display_welcome(void) {
    ui_clear_screen();
    ui_display_banner();
    printf("Welcome to Cnturion Inventory Manager!\n");
    printf("A secure inventory management system for micro businesses.\n\n");
    printf("This application implements the following security features:\n");
    printf("  • Password hashing with SHA-512\n");
    printf("  • SQL injection prevention with prepared statements\n");
    printf("  • Role-based access control (RBAC)\n");
    printf("  • Input validation and sanitization\n");
    printf("  • Comprehensive audit logging\n");
    printf("  • Secure error handling\n\n");
    ui_press_any_key();
}

/**
 * Display main menu (not logged in)
 */
int display_main_menu_not_logged_in(void) {
    const char* options[] = {
        "Login",
        "Exit"
    };
    
    return ui_get_menu_option(options, 2, 1, 2);
}

// ============================================
// MAIN FUNCTION
// ============================================

int main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    int result = 0;
    sqlite3* db = NULL;
    UserSession* session = NULL;
    
    // Initialize UI system
    if (ui_init() != ERR_NONE) {
        fprintf(stderr, "Failed to initialize UI system\n");
        return 1;
    }
    
    // Display welcome screen
    display_welcome();
    
    // Connect to database
    printf("Connecting to database...\n");
    db = db_connect(DB_PATH);
    if (db == NULL) {
        fprintf(stderr, "Failed to connect to database\n");
        log_error("Failed to connect to database", NULL);
        ui_cleanup();
        return 1;
    }
    printf("Database connected successfully!\n\n");
    
    // Initialize database schema
    printf("Initializing database schema...\n");
    result = db_init(db);
    if (result != ERR_NONE) {
        fprintf(stderr, "Failed to initialize database\n");
        log_error("Failed to initialize database", NULL);
        db_disconnect(db);
        ui_cleanup();
        return 1;
    }
    printf("Database initialized successfully!\n\n");
    
    // Create default admin if no users exist
    result = create_default_admin(db);
    if (result != ERR_NONE) {
        fprintf(stderr, "Warning: Failed to create default admin\n");
    }
    
    // Main application loop
    while (1) {
        ui_clear_screen();
        ui_display_banner();
        
        if (session == NULL || !session->is_authenticated) {
            // Not logged in
            int choice = display_main_menu_not_logged_in();
            
            switch (choice) {
                case 1:  // Login
                    session = session_create();
                    if (session == NULL) {
                        fprintf(stderr, "Failed to create session\n");
                        break;
                    }
                    
                    result = ui_login_screen(db, session);
                    if (result == ERR_NONE) {
                        ui_press_any_key();
                    } else {
                        session_destroy(session);
                        session = NULL;
                        ui_press_any_key();
                    }
                    break;
                    
                case 2:  // Exit
                    printf("\nThank you for using Cnturion Inventory Manager!\n");
                    printf("Goodbye!\n\n");
                    goto cleanup;
            }
        } else {
            // Logged in - check if session expired
            if (session_is_expired(session)) {
                ui_display_warning("Your session has expired. Please login again.");
                session_destroy(session);
                session = NULL;
                ui_press_any_key();
                continue;
            }
            
            // Show main menu
            result = ui_main_menu(db, session);
            if (result == 0) {
                // Logout
                printf("\nLogging out...\n");
                session_destroy(session);
                session = NULL;
                ui_press_any_key();
            }
        }
    }
    
cleanup:
    // Cleanup
    if (session != NULL) {
        session_destroy(session);
    }
    
    if (db != NULL) {
        db_disconnect(db);
    }
    
    ui_cleanup();
    
    return 0;
}

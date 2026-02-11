/**
 * ============================================
 * main.c - Application Entry Point (Version 1)
 * ============================================
 * 
 * This is the main entry point for Cnturion Inventory Manager.
 * Version 1: Basic Setup - initializes database and tables.
 * 
 * LEARNING: Main function structure, initialization
 */

#include "database.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================
// MAIN FUNCTION
// ============================================

int main(void) {
    printf("============================================\n");
    printf("  Cnturion Inventory Manager v1.0\n");
    printf("  Version 1: Basic Setup\n");
    printf("============================================\n\n");
    
    // Initialize database
    printf("Initializing database...\n");
    ErrorCode err = database_init("data/cnturion.db");
    if (err != ERR_NONE) {
        print_error(err, "database_init");
        return EXIT_FAILURE;
    }
    
    printf("\nDatabase setup complete!\n");
    printf("Tables created:\n");
    printf("  - users\n");
    printf("  - products\n");
    printf("  - audit_logs\n");
    
    // Close database
    printf("\nClosing database...\n");
    database_close();
    
    printf("\nVersion 1 setup complete!\n");
    printf("Next step: Implement authentication (Version 2)\n");
    
    return EXIT_SUCCESS;
}

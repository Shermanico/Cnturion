#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <auth.h>
#include <input_validation.h>
#include <logger.h>
#include <product_controller.h>

void cleanup(Product **pointer) {
  free(*pointer);
  *pointer = NULL;
}

void searchMenu(Product *prod, unsigned int *total, unsigned int *viewLimit) {
  unsigned short exit = 0;
  char input[10];

  while (!exit) {
    printf(CYN "===== Search Existing Product ===== \n" reset);
    printf("--------------------------------- \n");
    printf("| 1 | %-25s |\n", "Search by ID");
    printf("| 2 | %-25s |\n", "Search by Name");
    printf("| 3 | %-25s |\n", "Search by Category");
    printf("| 4 | %-25s | \n", "Search by Quantity");
    printf("| 5 | %-25s | \n", "Search by Price");
    printf("| 0 | %-25s | \n", "Back to Main Menu");
    printf("--------------------------------- \n");
    printf("Actions : ");
    fgets(input, 10, stdin);
    int choice = atoi(input);

    switch (choice) {
    case 0:
      exit = 1;
      ClearScreen();
      break;
    case 1:
      ClearScreen();
      printf(CYN "===== Search by ID ===== \n" reset);
      searchProduct(prod, total, viewLimit, choice);
      break;
    case 2:
      ClearScreen();
      printf(CYN "===== Search by Name ===== \n" reset);
      searchProduct(prod, total, viewLimit, choice);
      break;
    case 3:
      ClearScreen();
      printf(CYN "===== Search by Category ===== \n" reset);
      searchProduct(prod, total, viewLimit, choice);
      break;
    case 4:
      ClearScreen();
      printf(CYN "===== Search by Quantity ===== \n" reset);
      searchProduct(prod, total, viewLimit, choice);
      break;
    case 5:
      ClearScreen();
      printf(CYN "===== Search by Price ===== \n" reset);
      searchProduct(prod, total, viewLimit, choice);
      break;
    default:
      ClearScreen();
      printf(RED "Please select the options available\n" reset);
      break;
    }
  }
}

void adminMenu(Product *prod, unsigned int *total, unsigned int *uid,
               unsigned int *size, unsigned int *viewLimit, Session *session) {
  char input[10];
  unsigned short exit = 0;

  while (!exit) {
    // Check session timeout before each action
    if (!checkSessionTimeout(session)) {
      logEvent(LOG_AUDIT, session->username, "SESSION_TIMEOUT",
               "Admin session expired due to inactivity");
      exit = 1;
      break;
    }

    printf(CYN "\n===== Inventory Management [Admin: %s] ===== \n" reset,
           session->username);
    printf("----------------------------------------- \n");
    printf("| 1 | %-33s |\n", "View Existing Products");
    printf("| 2 | %-33s |\n", "Add New Product");
    printf("| 3 | %-33s |\n", "Update Existing Product");
    printf("| 4 | %-33s |\n", "Delete Existing Product");
    printf("| 5 | %-33s |\n", "Search Existing Product/s");
    printf("| 6 | %-33s |\n", "Sell Product");
    printf("| 7 | %-33s |\n", "Restock Product");
    printf("| 8 | %-33s |\n", "Manage Users");
    printf("| 0 | %-33s |\n", "Exit program");
    printf("----------------------------------------- \n");
    printf("Actions : ");
    fgets(input, 10, stdin);
    int choice = atoi(input);

    updateSessionActivity(session);

    switch (choice) {
    case 0:
      printf("Exiting. \n");
      logEvent(LOG_AUDIT, session->username, "LOGOUT", "Admin logged out");
      exit = 1;
      break;
    case 1:
      ClearScreen();
      printf(CYN "===== View Existing Products ===== \n" reset);
      listProduct(prod, total, viewLimit);
      break;
    case 2:
      ClearScreen();
      printf(CYN "===== Add New Product ===== \n" reset);
      (*size)++;
      prod = realloc(prod, ((*size) * sizeof(Product)));
      if (!prod) {
        printf(RED "Error: Memory allocation failed.\n" reset);
        logEvent(LOG_ERROR, session->username, "REALLOC_FAIL",
                 "Failed to allocate memory for new product");
        exit = 1;
        break;
      }
      addProduct(prod, total, uid, viewLimit);
      logEvent(LOG_AUDIT, session->username, "ADD_PRODUCT",
               "Product added successfully");
      break;
    case 3:
      ClearScreen();
      printf(CYN "===== Update Existing Product ===== \n\n" reset);
      updateProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "UPDATE_PRODUCT",
               "Product updated");
      break;
    case 4:
      ClearScreen();
      printf(CYN "===== Delete Existing Product ===== \n\n" reset);
      deleteProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "DELETE_PRODUCT",
               "Product deleted");
      break;
    case 5:
      ClearScreen();
      searchMenu(prod, total, viewLimit);
      break;
    case 6:
      ClearScreen();
      printf(CYN "===== Sell Product ===== \n\n" reset);
      sellProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "SELL_PRODUCT", "Product sold");
      break;
    case 7:
      ClearScreen();
      printf(CYN "===== Restock Product ===== \n\n" reset);
      restockProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "RESTOCK_PRODUCT",
               "Product restocked");
      break;
    case 8:
      ClearScreen();
      userManagementMenu(session);
      break;
    default:
      ClearScreen();
      printf(RED "Please select the options available\n" reset);
      break;
    }
  }
}

void employeeMenu(Product *prod, unsigned int *total, unsigned int *viewLimit,
                  Session *session) {
  char input[10];
  unsigned short exit = 0;

  while (!exit) {
    // Check session timeout before each action
    if (!checkSessionTimeout(session)) {
      logEvent(LOG_AUDIT, session->username, "SESSION_TIMEOUT",
               "Employee session expired due to inactivity");
      exit = 1;
      break;
    }

    printf(CYN "\n===== Inventory Management [Employee: %s] ===== \n" reset,
           session->username);
    printf("----------------------------------------- \n");
    printf("| 1 | %-33s |\n", "View Existing Products");
    printf("| 2 | %-33s |\n", "Search Existing Product/s");
    printf("| 3 | %-33s |\n", "Sell Product");
    printf("| 4 | %-33s |\n", "Restock Product");
    printf("| 0 | %-33s |\n", "Exit program");
    printf("----------------------------------------- \n");
    printf("Actions : ");
    fgets(input, 10, stdin);
    int choice = atoi(input);

    updateSessionActivity(session);

    switch (choice) {
    case 0:
      printf("Exiting. \n");
      logEvent(LOG_AUDIT, session->username, "LOGOUT", "Employee logged out");
      exit = 1;
      break;
    case 1:
      ClearScreen();
      printf(CYN "===== View Existing Products ===== \n" reset);
      listProduct(prod, total, viewLimit);
      break;
    case 2:
      ClearScreen();
      searchMenu(prod, total, viewLimit);
      break;
    case 3:
      ClearScreen();
      printf(CYN "===== Sell Product ===== \n\n" reset);
      sellProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "SELL_PRODUCT", "Product sold");
      break;
    case 4:
      ClearScreen();
      printf(CYN "===== Restock Product ===== \n\n" reset);
      restockProduct(prod, total, viewLimit);
      logEvent(LOG_AUDIT, session->username, "RESTOCK_PRODUCT",
               "Product restocked");
      break;
    default:
      ClearScreen();
      printf(RED "Please select the options available\n" reset);
      break;
    }
  }
}

int main(void) {
  // --- Initialize Logger ---
  initLogger();

  // --- Authentication ---
  Session session = {0, "", ROLE_EMPLOYEE, 0, 0, 0};

  printf(CYN ""
             "  _____       _               _             "
             "\n / ____|     | |             (_)            "
             "\n| |     _ __ | |_ _   _ _ __  _  ___  _ __  "
             "\n| |    | '_ \\| __| | | | '__|| |/ _ \\| '_ \\ "
             "\n| |____| | | | |_| |_| | |   | | (_) | | | |"
             "\n \\_____| |_|\\__|\\__,_|_|   |_|\\___/|_| |_|"
             "\n" reset);

  seedDefaultAdmin();

  if (!login(&session)) {
    logEvent(LOG_AUDIT, NULL, "LOGIN_FAILED",
             "Maximum login attempts exceeded");
    return 1;
  }

  logEvent(LOG_AUDIT, session.username, "LOGIN_SUCCESS", "User logged in");

  ClearScreen();

  // --- Inventory initialization ---
  unsigned int *size = (unsigned int *)malloc(sizeof(unsigned int));
  unsigned int *total = (unsigned int *)malloc(sizeof(unsigned int));
  unsigned int *uid = (unsigned int *)malloc(sizeof(unsigned int));
  unsigned int *viewLimit = (unsigned int *)malloc(sizeof(unsigned int));

  if (!size || !total || !uid || !viewLimit) {
    printf(RED "Error: Memory allocation failed. Cannot start.\n" reset);
    logEvent(LOG_ERROR, session.username, "MALLOC_FAIL",
             "Failed to allocate startup memory");
    free(size);
    free(total);
    free(uid);
    free(viewLimit);
    return 1;
  }

  Product *prod = initiate(NULL, total, uid, size, viewLimit);

  if (!prod) {
    printf(RED "Error: Could not initialize inventory.\n" reset);
    logEvent(LOG_ERROR, session.username, "INIT_FAIL",
             "Failed to initialize inventory data");
    free(size);
    free(total);
    free(uid);
    free(viewLimit);
    return 1;
  }

  printf(CYN ""
             "  _____       _               _             "
             "\n / ____|     | |             (_)            "
             "\n| |     _ __ | |_ _   _ _ __  _  ___  _ __  "
             "\n| |    | '_ \\| __| | | | '__|| |/ _ \\| '_ \\ "
             "\n| |____| | | | |_| |_| | |   | | (_) | | | |"
             "\n \\_____| |_|\\__|\\__,_|_|   |_|\\___/|_| |_|"
             "\n" reset);

  // --- Role-based menu ---
  if (session.role == ROLE_ADMIN) {
    adminMenu(prod, total, uid, size, viewLimit, &session);
  } else {
    employeeMenu(prod, total, viewLimit, &session);
  }

  // --- Cleanup ---
  logoutSession(&session);
  free(prod);
  free(total);
  free(uid);
  free(size);
  free(viewLimit);
  return 0;
}
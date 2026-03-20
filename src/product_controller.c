#include <math.h>
#include <product_controller.h>
#include <security.h>

void simpleView(Product *prod, unsigned int *total, unsigned int *viewLimit,
                unsigned int target, unsigned int type) {
  target++;
  unsigned int remain =
      (target % *viewLimit); // the exact number on the list of the current page
  unsigned int lastPageRemain = *total % *viewLimit;
  unsigned int page =
      (remain > 0)
          ? (target / *viewLimit + 1)
          : (target / *viewLimit); // current page of the target product

  unsigned int totalPage = *total / *viewLimit;
  totalPage = (lastPageRemain > 0) ? totalPage + 1 : totalPage;

  unsigned int start = (page - 1) * (*viewLimit);

  unsigned int end =
      (lastPageRemain == 0) ? start + *viewLimit : start + lastPageRemain;
  end = (totalPage == page)
            ? end
            : start + *viewLimit; // 'end' depends on target's page

  // printf("Target : %d\n", target);
  // printf("remain : %d\n", remain);
  // printf("page : %d\n", page);
  // printf("total : %d\n", *total);
  // printf("totalPage : %d\n", totalPage);
  // printf("lastPageRemain : %d\n", lastPageRemain);
  // printf("start : %d\n", start);
  // printf("end : %d\n", end);

  printf(CYN " \t\t\t***** INVENTORY *****\n" reset);
  printf(
      "------------------------------------------------------------------\n");
  printf("S.N.|    %-5s|  %-10s|  %-10s| %-10s|  %-10s|\n", "ID", "Name",
         "Category", "Quantity", "Price");
  printf(
      "------------------------------------------------------------------\n");

  for (int i = start; i < end; i++) {
    // (i == target-1) ?
    //     (type == 0) ?
    //     printf(GRN "%-4d|    %-5d|  %-10s|  %-10s| %-10d|  %-10.2f| ADDED\n"
    //     reset, i + 1, prod[i].id, prod[i].name, prod[i].category,
    //     prod[i].quantity, prod[i].price) : printf(GRN "%-4d|    %-5d|  %-10s|
    //     %-10s| %-10d|  %-10.2f| UPDATED\n" reset, i + 1, prod[i].id,
    //     prod[i].name, prod[i].category, prod[i].quantity, prod[i].price) :
    //     printf("%-4d|    %-5d|  %-10s|  %-10s| %-10d|  %-10.2f|\n", i + 1,
    //     prod[i].id, prod[i].name, prod[i].category, prod[i].quantity,
    //     prod[i].price);

    if (i == target - 1) {
      if (type == 0) {
        printf(GRN
               "%-4d|    %-5u|  %-10s|  %-10s| %-10u|  %-10.2f| ADDED\n" reset,
               i + 1, prod[i].id, prod[i].name, prod[i].category,
               prod[i].quantity, prod[i].price);
      } else if (type == 1) {
        printf(
            GRN
            "%-4d|    %-5u|  %-10s|  %-10s| %-10u|  %-10.2f| UPDATED\n" reset,
            i + 1, prod[i].id, prod[i].name, prod[i].category, prod[i].quantity,
            prod[i].price);
      } else {
        printf(
            RED
            "%-4d|    %-5d|  %-10s|  %-10s| %-10d|  %-10.2f| DELETED\n" reset,
            0, 0, "Product", "is Null", 0, 0.00);
        printf("%-4d|    %-5u|  %-10s|  %-10s| %-10u|  %-10.2f|\n", i + 1,
               prod[i].id, prod[i].name, prod[i].category, prod[i].quantity,
               prod[i].price);
      }
    } else {
      printf("%-4d|    %-5u|  %-10s|  %-10s| %-10u|  %-10.2f|\n", i + 1,
             prod[i].id, prod[i].name, prod[i].category, prod[i].quantity,
             prod[i].price);
    }
  }
  printf(
      "------------------------------------------------------------------\n\n");
}

void listProduct(Product *prod, unsigned int *total, unsigned int *viewLimit) {
  char input[10];

  // Default configuration for first page
  unsigned int remain = *total % *viewLimit;
  unsigned int page = 0;
  unsigned int start = 0;
  unsigned int end = (*total > *viewLimit)
                         ? *viewLimit
                         : *total; // [left] multi page : [right] single page

  unsigned int totalPage =
      (*total > *viewLimit)
          ? ((remain > 0) ? ((*total / *viewLimit) + 1) : (*total / *viewLimit))
          : 1;

  unsigned int state = 1;

  if (*total == 0) {
    printf(RED "No products to view\n" reset);
    printf("Press enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  while (state) {
    printf(CYN " \t\t\t***** INVENTORY *****\n" reset);
    printf(
        "------------------------------------------------------------------\n");
    printf("S.N.|    %-5s|  %-10s|  %-10s| %-10s|  %-10s|\n", "ID", "Name",
           "Category", "Quantity", "Price");
    printf(
        "------------------------------------------------------------------\n");

    for (unsigned int i = start; i < end; i++) {
      printf("%-4u|    %-5u|  %-10s|  %-10s| %-10u|  %-10.2f|\n", i + 1,
             prod[i].id, prod[i].name, prod[i].category, prod[i].quantity,
             prod[i].price);
    }
    printf("------------------------------------------------------------------"
           "\n\n");

    // Asking for pages to view only if
    if (*total > *viewLimit) {
      printf("Current Page : " YEL "%-34u" reset " Total Pages: " RED
             "%u\n" reset,
             page + 1, totalPage);
      printf("Which page to view (Enter nothing to skip): ");
      unsigned int ask = 1;
      while (ask) {
        fgets(input, 10, stdin);
        if (*input == '\n') {
          ask = 0;   // leave the ask loop [get input]
          state = 0; // leave the state loop [view page]
          break;
        }
        input[strcspn(input, "\n")] = 0;
        if (atoi(input) == 0 || atoi(input) > totalPage) {
          printf(RED "Invalid Input !" reset " Enter again : ");
          ask = 1;
        } else {
          page = atoi(input) - 1;
          ask = 0;
          ClearScreen();
        }
      }

      start = (page) * (*viewLimit);
      end = (remain > 0) ? (start + remain) : (start + *viewLimit);
      end = (totalPage == page + 1) ? end : (start + (*viewLimit));

    } else {
      state = 0;
      printf("Press enter to continue...");
      getchar();
      ClearScreen();
    }
  }
}

void addProduct(Product *prod, unsigned int *total, unsigned int *uid,
                unsigned int *viewLimit) {
  prod[*total].id = *uid + 1;
  printf("UID: %u\n", prod[*total].id);

  printf("Product Name: ");
  fgets(prod[*total].name, 128, stdin);
  prod[*total].name[strcspn(prod[*total].name, "\n")] = 0;
  while (!validateProductName(prod[*total].name)) {
    printf(RED
           "Invalid name! Use letters, numbers, spaces, hyphens, dots." reset
           " Enter again: ");
    fgets(prod[*total].name, 128, stdin);
    prod[*total].name[strcspn(prod[*total].name, "\n")] = 0;
  }
  sanitizeString(prod[*total].name, 127);

  printf("Product Category: ");
  fgets(prod[*total].category, 64, stdin);
  prod[*total].category[strcspn(prod[*total].category, "\n")] = 0;
  while (!validateProductCategory(prod[*total].category)) {
    printf(RED "Invalid category! Use letters, numbers, spaces, hyphens." reset
               " Enter again: ");
    fgets(prod[*total].category, 64, stdin);
    prod[*total].category[strcspn(prod[*total].category, "\n")] = 0;
  }
  sanitizeString(prod[*total].category, 63);

  printf("Product Quantity: ");
  getDigit(&prod[*total].quantity);
  while (prod[*total].quantity == 0 || prod[*total].quantity > 999999) {
    printf(RED "Quantity must be between 1 and 999999!" reset " Enter again: ");
    getDigit(&prod[*total].quantity);
  }

  printf("Product Price: ");
  getFloat(&prod[*total].price);
  while (prod[*total].price <= 0 || prod[*total].price > 999999.99f) {
    printf(RED "Price must be between 0.01 and 999999.99!" reset
               " Enter again: ");
    getFloat(&prod[*total].price);
  }

  unsigned int target = (*total);
  (*total)++;
  (*uid)++;

  ClearScreen();
  simpleView(prod, total, viewLimit, target, 0);
  printf(GRN "Product with ID %u added successfully\n\n" reset,
         prod[*total - 1].id);

  printf("Press enter to continue...");
  getchar();
  ClearScreen();
  // listProduct(prod, total, viewLimit);
  writeCSV(prod, total);
}

void deleteProduct(Product *prod, unsigned int *total,
                   unsigned int *viewLimit) {
  unsigned int id;
  unsigned int exist = 0;
  unsigned int item = 0;
  listProduct(prod, total, viewLimit);
  printf("Please select the ID of the product to delete : ");
  getDigit(&id);

  for (int i = 0; i < *total; i++) {
    if (prod[i].id == id) {
      exist = 1;
      item = i;
    }
    if (exist)
      prod[i] = prod[i + 1];
  }

  if (exist) {
    ClearScreen();
    (*total)--;
    prod = realloc(prod, (*total > 0 ? *total : 1) * sizeof(Product));
    writeCSV(prod, total);
    simpleView(prod, total, viewLimit, item, 2);
    printf(CYN "Product with ID %u deleted successfully\n" reset, id);

    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
  } else {
    printf(RED "Product with ID %u does not exist.\n" reset, id);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
  }
}

void updateProduct(Product *prod, unsigned int *total,
                   unsigned int *viewLimit) {
  unsigned int id;
  unsigned int exist = 0;
  unsigned int item;
  char input[128];

  listProduct(prod, total, viewLimit);
  printf("Please select the ID of the product to update : ");
  getDigit(&id);

  for (int i = 0; i < *total; i++) {
    if (prod[i].id == id) {
      exist = 1;
      item = i;
      break;
    }
  }
  if (exist) {
    prod[item].id = id;
    printf("- Type 'Enter' to skip and the data will remain the same.\n");
    printf("%22s" YEL "%10s" reset " -> " CYN "%-10s" reset "\n", "", "[Old]",
           "[New]");
    printf("%-20s: " YEL "%10s" reset " -> " CYN, "Product Name",
           prod[item].name);
    // fflush(stdin);
    fgets(input, 100, stdin);
    if (*input != '\n') {
      input[strcspn(input, "\n")] = 0;
      if (validateProductName(input)) {
        strncpy(prod[item].name, input, 127);
        prod[item].name[127] = '\0';
        sanitizeString(prod[item].name, 127);
      } else {
        printf(RED "Invalid name format, keeping original.\n" reset);
      }
    }

    printf(reset "%-20s: " YEL "%10s" reset " -> " CYN, "Product Category",
           prod[item].category);
    // fflush(stdin);
    fgets(input, 100, stdin);
    if (*input != '\n') {
      input[strcspn(input, "\n")] = 0;
      if (validateProductCategory(input)) {
        strncpy(prod[item].category, input, 63);
        prod[item].category[63] = '\0';
        sanitizeString(prod[item].category, 63);
      } else {
        printf(RED "Invalid category format, keeping original.\n" reset);
      }
    }

    printf(reset "%-20s: " YEL "%10u" reset " -> " CYN, "Product Quantity",
           prod[item].quantity);
    // fflush(stdin);
    fgets(input, 100, stdin);
    if (*input != '\n') {
      input[strcspn(input, "\n")] = 0;
      if (atoi(input) == 0) {
        printf(RED "Invalid input !" reset " Enter again : " CYN);
        getDigit(&prod[item].quantity);
      } else {
        prod[item].quantity = atoi(input);
      }
    }

    printf(reset "%-20s: " YEL "%10.2f" reset " -> " CYN, "Product Price",
           prod[item].price);
    // fflush(stdin);
    fgets(input, 100, stdin);
    if (*input != '\n') {
      input[strcspn(input, "\n")] = 0;
      if (atof(input) == 0) {
        printf(RED "Invalid input !" reset " Enter again : " CYN);
        getFloat(&prod[item].price);
      } else {
        prod[item].price = atof(input);
      }
    }

    printf(reset);
    ClearScreen();
    simpleView(prod, total, viewLimit, item, 1);
    printf(GRN "Product with ID %u updated successfully.\n" reset, id);

    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
  } else {
    printf(RED "Product with ID %u does not exist.\n" reset, id);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
  }
  writeCSV(prod, total);
}

void searchProduct(Product *prod, unsigned int *total, unsigned int *viewLimit,
                   unsigned int action) {
  unsigned size = *viewLimit;
  Product *result = (Product *)malloc((size) * sizeof(Product));
  if (!result) {
    printf(RED "Memory allocation error.\n" reset);
    return;
  }
  unsigned int digit;
  char string[128];
  float number = 0;
  unsigned int *k = (unsigned int *)malloc(sizeof(unsigned int));
  if (!k) {
    printf(RED "Memory allocation error.\n" reset);
    free(result);
    return;
  }
  *k = 0;

  if (action == 1) {
    printf("ID : ");
    getDigit(&digit);

    for (int i = 0; i < *total; i++) {
      if (prod[i].id == digit) {
        result[*k] = prod[i];
        (*k)++;
        if (*k >= size) {
          size++;
          Product *tmp = realloc(result, ((size) * sizeof(Product)));
          if (!tmp) {
            printf(RED "Memory error.\n" reset);
            break;
          }
          result = tmp;
        }
      }
    }
    listProduct(result, k, viewLimit);
    *k = 0;
  } else if (action == 2) {
    printf("Name : ");
    fgets(string, 128, stdin);
    string[strcspn(string, "\n")] = 0;
    sanitizeString(string, 127);

    for (int i = 0; i < *total; i++) {
      if (strcmp(prod[i].name, string) == 0) {
        result[*k] = prod[i];
        (*k)++;
        if (*k >= size) {
          size++;
          Product *tmp = realloc(result, ((size) * sizeof(Product)));
          if (!tmp) {
            printf(RED "Memory error.\n" reset);
            break;
          }
          result = tmp;
        }
      }
    }
    listProduct(result, k, viewLimit);
    *k = 0;
  } else if (action == 3) {
    printf("Category : ");
    fgets(string, 128, stdin);
    string[strcspn(string, "\n")] = 0;
    sanitizeString(string, 127);

    for (int i = 0; i < *total; i++) {
      if (strcmp(prod[i].category, string) == 0) {
        result[*k] = prod[i];
        (*k)++;
        if (*k >= size) {
          size++;
          Product *tmp = realloc(result, ((size) * sizeof(Product)));
          if (!tmp) {
            printf(RED "Memory error.\n" reset);
            break;
          }
          result = tmp;
        }
      }
    }
    listProduct(result, k, viewLimit);
    *k = 0;
  } else if (action == 4) {
    printf("Quantity : ");
    getDigit(&digit);

    for (int i = 0; i < *total; i++) {
      if (prod[i].quantity == digit) {
        result[*k] = prod[i];
        (*k)++;
        if (*k >= size) {
          size++;
          Product *tmp = realloc(result, ((size) * sizeof(Product)));
          if (!tmp) {
            printf(RED "Memory error.\n" reset);
            break;
          }
          result = tmp;
        }
      }
    }
    listProduct(result, k, viewLimit);
    *k = 0;
  } else if (action == 5) {
    printf("Price : ");
    getFloat(&number);

    for (int i = 0; i < *total; i++) {
      if (fabs(prod[i].price - number) < 0.01f) {
        result[*k] = prod[i];
        (*k)++;
        if (*k >= size) {
          size++;
          Product *tmp = realloc(result, ((size) * sizeof(Product)));
          if (!tmp) {
            printf(RED "Memory error.\n" reset);
            break;
          }
          result = tmp;
        }
      }
    }
    listProduct(result, k, viewLimit);
    *k = 0;
  }
  free(k);
  free(result);
}

void sellProduct(Product *prod, unsigned int *total, unsigned int *viewLimit) {
  unsigned int id;
  unsigned int quantity;
  unsigned int exist = 0;
  unsigned int item = 0;

  listProduct(prod, total, viewLimit);

  if (*total == 0)
    return;

  printf("Please select the ID of the product to sell: ");
  getDigit(&id);

  for (unsigned int i = 0; i < *total; i++) {
    if (prod[i].id == id) {
      exist = 1;
      item = i;
      break;
    }
  }

  if (!exist) {
    printf(RED "Product with ID %u does not exist.\n" reset, id);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  printf("Product: " CYN "%s" reset " | Available stock: " YEL "%u" reset "\n",
         prod[item].name, prod[item].quantity);
  printf("Quantity to sell: ");
  getDigit(&quantity);

  if (quantity > prod[item].quantity) {
    printf(RED "Insufficient stock! Only %u available.\n" reset,
           prod[item].quantity);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  if (quantity == 0) {
    printf(RED "Quantity must be greater than 0.\n" reset);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  float totalPrice = quantity * prod[item].price;
  prod[item].quantity -= quantity;

  ClearScreen();
  simpleView(prod, total, viewLimit, item, 1);
  printf(GRN "Sale completed!\n" reset);
  printf("  Product : %s\n", prod[item].name);
  printf("  Sold    : %u units\n", quantity);
  printf("  Price   : $%.2f each\n", prod[item].price);
  printf(BGRN "  Total   : $%.2f\n" reset, totalPrice);
  printf("  Stock   : %u remaining\n", prod[item].quantity);

  writeCSV(prod, total);

  printf("\nPress enter to continue...");
  getchar();
  ClearScreen();
}

void restockProduct(Product *prod, unsigned int *total,
                    unsigned int *viewLimit) {
  unsigned int id;
  unsigned int quantity;
  unsigned int exist = 0;
  unsigned int item = 0;

  listProduct(prod, total, viewLimit);

  if (*total == 0)
    return;

  printf("Please select the ID of the product to restock: ");
  getDigit(&id);

  for (unsigned int i = 0; i < *total; i++) {
    if (prod[i].id == id) {
      exist = 1;
      item = i;
      break;
    }
  }

  if (!exist) {
    printf(RED "Product with ID %u does not exist.\n" reset, id);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  printf("Product: " CYN "%s" reset " | Current stock: " YEL "%u" reset "\n",
         prod[item].name, prod[item].quantity);
  printf("Quantity to add: ");
  getDigit(&quantity);

  if (quantity == 0) {
    printf(RED "Quantity must be greater than 0.\n" reset);
    printf("\nPress enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  prod[item].quantity += quantity;

  ClearScreen();
  simpleView(prod, total, viewLimit, item, 1);
  printf(GRN "Restock completed!\n" reset);
  printf("  Product   : %s\n", prod[item].name);
  printf("  Added     : %u units\n", quantity);
  printf("  New stock : %u\n", prod[item].quantity);

  writeCSV(prod, total);

  printf("\nPress enter to continue...");
  getchar();
  ClearScreen();
}

Product *initiate(Product *prod, unsigned int *total, unsigned int *uid,
                  unsigned int *size, unsigned int *viewLimit) {
  readINI(viewLimit);

  *total = totalProductCSV();
  *size = *total + 1;
  prod = (Product *)malloc((*size) * sizeof(Product));
  if (!prod) {
    return NULL;
  }
  if (*total > 0) {
    readCSV(prod);
  }
  *uid = (*total == 0) ? 0 : prod[*total - 1].id;
  return prod;
}

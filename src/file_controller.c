#include <file_controller.h>
#include <logger.h>

int readINI(unsigned int *viewLimit) {
  FILE *f = fopen(INI, "r");
  if (!f) {
    printf(RED "Error: Could not load configuration file.\n" reset);
    logEvent(LOG_ERROR, NULL, "READ_CONFIG", "Failed to open config.ini");
    *viewLimit = 10; // Safe default
    return -1;
  }
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    char *value = strtok(line, " = ");
    value = strtok(NULL, " = ");
    if (value) {
      *viewLimit = atoi(value);
    }
  }
  fclose(f);
  return 0;
}

unsigned int totalProductCSV() {
  FILE *f = fopen(CSV, "r");
  if (!f) {
    logEvent(LOG_WARN, NULL, "READ_INVENTORY",
             "Inventory CSV not found, starting empty");
    return 0;
  }
  int n = -1;
  char line[256];
  while (fgets(line, sizeof(line), f))
    n++;
  fclose(f);
  n = (n == -1) ? 0 : n;
  return (unsigned int)n;
}

int writeCSV(Product *prod, unsigned int *total) {
  FILE *f = fopen(CSV, "w");
  if (!f) {
    printf(RED "Error: Could not save inventory data.\n" reset);
    logEvent(LOG_ERROR, NULL, "WRITE_INVENTORY",
             "Failed to open Inventory.csv for writing");
    return -1;
  }
  fprintf(f, "%s, %s, %s, %s, %s\n", "ID", "Name", "Category", "Quantity",
          "Price");
  for (int i = 0; i < *total; i++) {
    fprintf(f, "%u, %s, %s, %u, %.2f\n", prod[i].id, prod[i].name,
            prod[i].category, prod[i].quantity, prod[i].price);
  }
  fclose(f);
  return 0;
}

int readCSV(Product *prod) {
  FILE *f = fopen(CSV, "r");
  if (!f) {
    printf(RED "Error: Could not load inventory data.\n" reset);
    logEvent(LOG_ERROR, NULL, "READ_INVENTORY",
             "Failed to open Inventory.csv for reading");
    return -1;
  }
  char string[2048];
  int row = 0;
  int column = 0;
  while (fgets(string, 2048, f)) {
    column = 0;
    row++;
    if (row == 1)
      continue;

    char *value = strtok(string, ", ");
    int fieldCount = 0;
    while (value) {
      if (column == 0)
        prod[row - 2].id = atoi(value);
      if (column == 1)
        strncpy(prod[row - 2].name, value, 127);
      if (column == 2)
        strncpy(prod[row - 2].category, value, 63);
      if (column == 3)
        prod[row - 2].quantity = atoi(value);
      if (column == 4)
        prod[row - 2].price = atof(value);

      value = strtok(NULL, ", ");
      column++;
      fieldCount++;
    }

    // Validate row has all 5 fields
    if (fieldCount < 5) {
      char detail[128];
      snprintf(detail, sizeof(detail),
               "Malformed CSV row %d: expected 5 fields, got %d", row,
               fieldCount);
      logEvent(LOG_WARN, NULL, "READ_INVENTORY", detail);
    }
  }
  fclose(f);
  return 0;
}

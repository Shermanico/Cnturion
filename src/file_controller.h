#ifndef FILE_CONTROLLER_H
#define FILE_CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <color.h>
#include <product.h>

#define DATA "data/Inventory.txt"
#define CSV "data/Inventory.csv"
#define INI "config.ini"
#define DATA_DIR "data"

// Create data/ directory if it doesn't exist
void initDataDir(void);

// Create config.ini with defaults if it doesn't exist
void initConfig(void);

// Returns 0 on success, -1 on error
int readINI(unsigned int *viewLimit);

// Returns total count, or 0 on error
unsigned int totalProductCSV();

// Returns 0 on success, -1 on error
int writeCSV(Product *prod, unsigned int *total);

// Returns 0 on success, -1 on error
int readCSV(Product *prod);

#endif
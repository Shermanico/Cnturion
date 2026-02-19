#ifndef FILE_CONTROLLER_H
#define FILE_CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <color.h>
#include <product.h>

#define DATA "data/Inventory.txt"
#define CSV "data/Inventory.csv"
#define INI "config.ini"

// Returns 0 on success, -1 on error
int readINI(unsigned int *viewLimit);

// Returns total count, or 0 on error
unsigned int totalProductCSV();

// Returns 0 on success, -1 on error
int writeCSV(Product *prod, unsigned int *total);

// Returns 0 on success, -1 on error
int readCSV(Product *prod);

#endif
#ifndef PRODUCT_CONTROLLER_H
#define PRODUCT_CONTROLLER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clear.h>
#include <color.h>
#include <file_controller.h>
#include <input_validation.h>
#include <product.h>

void simpleView(Product *prod, unsigned int *total, unsigned int *viewLimit,
                unsigned int target, unsigned int type);
void listProduct(Product *prod, unsigned int *total, unsigned int *viewLimit);
void addProduct(Product *prod, unsigned int *total, unsigned int *uid,
                unsigned int *viewLimit);
void deleteProduct(Product *prod, unsigned int *total, unsigned int *viewLimit);
void updateProduct(Product *prod, unsigned int *total, unsigned int *viewLimit);
void searchProduct(Product *prod, unsigned int *total, unsigned int *viewLimit,
                   unsigned int action);
void sellProduct(Product *prod, unsigned int *total, unsigned int *viewLimit);
void restockProduct(Product *prod, unsigned int *total,
                    unsigned int *viewLimit);
Product *initiate(Product *prod, unsigned int *total, unsigned int *uid,
                  unsigned int *size, unsigned int *viewLimit);

#endif

#include <stdio.h>

int main(void) {
	printf("Hello and welcome to the Cnturion Inventory App\n");
	printf("Here's a list of the options that you can do:\n");
	printf("1- Add Product\n");
	printf("2- Update Product\n");
	printf("3- Remove Product\n");
	int option;
	scanf("%d", &option);

	switch (option) {
 case 1:
	printf("Here you can add products\n");
	printf("The syntax is the following:\n");
	break;
 case 2:
 printf("Here you can update your existing products\n");
 printf("The syntax is the following:\n");
 break;
 case 3:
 printf("Here you can delete existing products\n");
 printf("The syntax is the following:\n");
 break;
 default:
 printf("Operation not permitted\n"); 
 break;
 }
}


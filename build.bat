gcc main.c ^
  src/input_validation.c src/product_controller.c src/file_controller.c ^
  src/auth.c src/security.c src/logger.c ^
  -I./src/ ^
  -I./model/ ^
  -I./utilities/ ^
  ./utilities/clear.c ^
  -lm -largon2 -lbcrypt ^
  -Wall -Wextra ^
  -o Cnturion.exe
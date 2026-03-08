gcc main.c \
  src/input_validation.c src/product_controller.c src/file_controller.c \
  src/auth.c src/security.c src/logger.c \
  -I./src/ \
  -I./model/ \
  -I./utilities/ \
  ./utilities/clear.c \
  -lncurses -lm -largon2 \
  -O2 -Wall -Wextra -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
  -o Cnturion

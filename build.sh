ARGON2_DIR="./deps/phc-winner-argon2"

gcc main.c \
  src/input_validation.c src/product_controller.c src/file_controller.c \
  src/auth.c src/security.c src/logger.c \
  ./utilities/clear.c \
  "$ARGON2_DIR/src/argon2.c" \
  "$ARGON2_DIR/src/core.c" \
  "$ARGON2_DIR/src/blake2/blake2b.c" \
  "$ARGON2_DIR/src/thread.c" \
  "$ARGON2_DIR/src/encoding.c" \
  "$ARGON2_DIR/src/ref.c" \
  -I./src/ \
  -I./model/ \
  -I./utilities/ \
  -I"$ARGON2_DIR/include" \
  -lncurses -lm -lpthread \
  -O2 -Wall -Wextra -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
  -o Cnturion

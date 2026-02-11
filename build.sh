gcc main.c \
src/input_validation.c src/product_controller.c src/file_controller.c  -I./src/ \
-I./model/ \
-I./utilities/ \
./utilities/clear.c \
-lncurses \
-o output 

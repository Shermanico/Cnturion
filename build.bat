gcc main.c ^
  src\input_validation.c src\product_controller.c src\file_controller.c ^
  src\auth.c src\security.c ^
  -I.\src\ ^
  -I.\model\ ^
  .\utilities\clear.c -I.\utilities\ ^
  -o output

@REM cls

IF EXIST "output.exe" (
  output.exe
)
import sys

with open('src/input_validation.c', 'r') as f:
    content = f.read()

content = content.replace('static int isExit', 'int isExit')

with open('src/input_validation.c', 'w') as f:
    f.write(content)

with open('src/input_validation.h', 'r') as f:
    content = f.read()

content = content.replace('int getFloat(float *number);', 'int getFloat(float *number);\nint isExit(const char* str);')

with open('src/input_validation.h', 'w') as f:
    f.write(content)

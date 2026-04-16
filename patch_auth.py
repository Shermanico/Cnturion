import sys

with open('src/auth.c', 'r') as f:
    text = f.read()

# Add include
text = text.replace('#include <auth.h>', '#include <auth.h>\n#include <input_validation.h>')

# Create User modifications
text = text.replace(
    'printf("Username (3-63 chars, alphanumeric/underscore): ");',
    'printf("Username (3-63 chars, alphanumeric/underscore) (or \'exit\' to cancel): ");'
)
text = text.replace(
    'fgets(newUser.username, 64, stdin);',
    'if (!fgets(newUser.username, 64, stdin) || isExit(newUser.username)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)
text = text.replace(
    'printf("Choice: ");',
    'printf("Choice (or type \'exit\' to cancel): ");'
)
text = text.replace(
    'fgets(roleInput, 10, stdin);',
    'if (!fgets(roleInput, 10, stdin) || isExit(roleInput)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)

# readPassword replacement
text = text.replace(
    'readPassword(password, 128);',
    'readPassword(password, 128);\n    if (isExit(password)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)

with open('src/auth.c', 'w') as f:
    f.write(text)

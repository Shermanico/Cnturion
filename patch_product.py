import sys
import re

with open('src/product_controller.c', 'r') as f:
    text = f.read()

# Replace getDigit and getFloat to check for INPUT_CANCELLED
text = re.sub(
    r'getDigit\(&(prod\[.*?\]\.|)quantity\);',
    r'if (getDigit(&\1quantity) == INPUT_CANCELLED) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }',
    text
)
text = re.sub(
    r'getFloat\(&(prod\[.*?\]\.|)price\);',
    r'if (getFloat(&\1price) == INPUT_CANCELLED) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }',
    text
)
text = re.sub(
    r'getDigit\(&id\);',
    r'if (getDigit(&id) == INPUT_CANCELLED) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }',
    text
)
text = re.sub(
    r'getDigit\(&digit\);',
    r'if (getDigit(&digit) == INPUT_CANCELLED) { printf(YEL "\\nOperation cancelled.\\n" reset); free(result); free(k); return; }',
    text
)
text = re.sub(
    r'getFloat\(&number\);',
    r'if (getFloat(&number) == INPUT_CANCELLED) { printf(YEL "\\nOperation cancelled.\\n" reset); free(result); free(k); return; }',
    text
)

# String inputs
text = text.replace(
    'fgets(prod[*total].name, 128, stdin);',
    'if (!fgets(prod[*total].name, 128, stdin) || isExit(prod[*total].name)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)
text = text.replace(
    'fgets(prod[*total].category, 64, stdin);',
    'if (!fgets(prod[*total].category, 64, stdin) || isExit(prod[*total].category)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)
text = text.replace(
    'fgets(input, 100, stdin);',
    'if (!fgets(input, 100, stdin) || isExit(input)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)

# Search string inputs
text = text.replace(
    'fgets(string, 128, stdin);',
    'if (!fgets(string, 128, stdin) || isExit(string)) { printf(YEL "\\nOperation cancelled.\\n" reset); free(result); free(k); return; }'
)

# Prompts
text = text.replace('printf("Please select the ID of the product to update : ");',
                    'printf("Please select the ID of the product to update (or \'exit\' to cancel) : ");')
text = text.replace('printf("Please select the ID of the product to delete : ");',
                    'printf("Please select the ID of the product to delete (or \'exit\' to cancel) : ");')
text = text.replace('printf("Please select the ID of the product to sell: ");',
                    'printf("Please select the ID of the product to sell (or \'exit\' to cancel): ");')
text = text.replace('printf("Please select the ID of the product to restock: ");',
                    'printf("Please select the ID of the product to restock (or \'exit\' to cancel): ");')
text = text.replace('printf("Product Name: ");',
                    'printf("Product Name (or \'exit\' to cancel): ");')
text = text.replace('printf("Product Category: ");',
                    'printf("Product Category (or \'exit\' to cancel): ");')
text = text.replace('printf("Product Quantity: ");',
                    'printf("Product Quantity (or \'exit\' to cancel): ");')
text = text.replace('printf("Product Price: ");',
                    'printf("Product Price (or \'exit\' to cancel): ");')
text = text.replace('printf("Quantity to sell: ");',
                    'printf("Quantity to sell (or \'exit\' to cancel): ");')
text = text.replace('printf("Quantity to add: ");',
                    'printf("Quantity to add (or \'exit\' to cancel): ");')
text = text.replace('printf("ID : ");',
                    'printf("ID (or \'exit\' to cancel): ");')
text = text.replace('printf("Name : ");',
                    'printf("Name (or \'exit\' to cancel): ");')
text = text.replace('printf("Category : ");',
                    'printf("Category (or \'exit\' to cancel): ");')
text = text.replace('printf("Quantity : ");',
                    'printf("Quantity (or \'exit\' to cancel): ");')
text = text.replace('printf("Price : ");',
                    'printf("Price (or \'exit\' to cancel): ");')

# Update Product prompts
text = text.replace('printf("%-20s: " YEL "%10s" reset " -> " CYN, "Product Name",',
                    'printf("%-20s (or \'exit\'): " YEL "%10s" reset " -> " CYN, "Product Name",')
text = text.replace('printf(reset "%-20s: " YEL "%10s" reset " -> " CYN, "Product Category",',
                    'printf(reset "%-20s (or \'exit\'): " YEL "%10s" reset " -> " CYN, "Product Category",')
text = text.replace('printf(reset "%-20s: " YEL "%10u" reset " -> " CYN, "Product Quantity",',
                    'printf(reset "%-20s (or \'exit\'): " YEL "%10u" reset " -> " CYN, "Product Quantity",')
text = text.replace('printf(reset "%-20s: " YEL "%10.2f" reset " -> " CYN, "Product Price",',
                    'printf(reset "%-20s (or \'exit\'): " YEL "%10.2f" reset " -> " CYN, "Product Price",')

with open('src/product_controller.c', 'w') as f:
    f.write(text)


with open('src/auth.c', 'r') as f:
    text = f.read()

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
# We also have readPassword in auth.c, which reads characters one by one. If user types exit during readPassword?
# It's harder. But let's check `isExit` on `password` after readPassword returns.
text = text.replace(
    'readPassword(password, 128);',
    'readPassword(password, 128);\\n    if (isExit(password)) { printf(YEL "\\nOperation cancelled.\\n" reset); printf("\\nPress enter to continue..."); getchar(); ClearScreen(); return; }'
)

# For login we shouldn't really cancel with exit, since user just wants to login. Or maybe yes?
# Let's leave login alone as "exit" could theoretically be a password? Unlikely.

with open('src/auth.c', 'w') as f:
    # un-escape the newlines
    text = text.replace('\\n', '\n')
    f.write(text)


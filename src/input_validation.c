#include <input_validation.h>

// Clear remaining characters from stdin (replaces fflush(stdin) which is UB)
static void clearStdin(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int isExit(const char* str) {
    if ((str[0] == 'e' || str[0] == 'E') &&
        (str[1] == 'x' || str[1] == 'X') &&
        (str[2] == 'i' || str[2] == 'I') &&
        (str[3] == 't' || str[3] == 'T') &&
        (str[4] == '\n' || str[4] == '\0')) {
        return 1;
    }
    return 0;
}

int getDigit(unsigned int *digit) {
  char *p;
  char input[128];
  unsigned short ask = 1;
  while (ask) {
    if (!fgets(input, sizeof(input), stdin)) return INPUT_CANCELLED;
    if (isExit(input)) return INPUT_CANCELLED;
    
    *digit = strtol(input, &p, 10);
    if (p == input || (*p != '\n' && *p != '\0')) {
      printf(RED "Invalid input ! " reset "Enter again (or 'exit' to cancel): ");
      ask = 1;
    } else {
      ask = 0;
    }
  }
  return 1;
}

int getFloat(float *number) {
  char *p;
  char input[128];
  unsigned short ask = 1;
  while (ask) {
    if (!fgets(input, sizeof(input), stdin)) return INPUT_CANCELLED;
    if (isExit(input)) return INPUT_CANCELLED;

    *number = strtof(input, &p);
    if (p == input || (*p != '\n' && *p != '\0')) {
      printf(RED "Invalid input ! " reset "Enter again (or 'exit' to cancel): ");
      ask = 1;
    } else {
      ask = 0;
    }
  }
  return 1;
}

int getValidatedString(char *output, int maxLen) {
  char buffer[256];
  if (!fgets(buffer, sizeof(buffer), stdin)) return INPUT_CANCELLED;
  if (isExit(buffer)) return INPUT_CANCELLED;

  buffer[strcspn(buffer, "\n")] = '\0';

  int len = strlen(buffer);
  if (len == 0)
    return 0; // Empty input

  if (len > maxLen - 1) {
    buffer[maxLen - 1] = '\0';
  }

  strcpy(output, buffer);
  sanitizeString(output, maxLen - 1);
  return 1;
}
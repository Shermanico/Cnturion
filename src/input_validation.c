#include <input_validation.h>

// Clear remaining characters from stdin (replaces fflush(stdin) which is UB)
static void clearStdin(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void getDigit(unsigned int *digit) {
  char *p;
  char input[128];
  unsigned short ask = 1;
  while (ask) {
    fgets(input, sizeof(input), stdin);
    *digit = strtol(input, &p, 10);
    if (p == input || (*p != '\n' && *p != '\0')) {
      printf(RED "Invalid input ! " reset "Enter again: ");
      ask = 1;
    } else {
      ask = 0;
    }
  }
}

void getFloat(float *number) {
  char *p;
  char input[128];
  unsigned short ask = 1;
  while (ask) {
    fgets(input, sizeof(input), stdin);
    *number = strtof(input, &p);
    if (p == input || (*p != '\n' && *p != '\0')) {
      printf(RED "Invalid input ! " reset "Enter again: ");
      ask = 1;
    } else {
      ask = 0;
    }
  }
}

int getValidatedString(char *output, int maxLen) {
  char buffer[256];
  fgets(buffer, sizeof(buffer), stdin);
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
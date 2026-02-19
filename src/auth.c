#include <auth.h>
#include <termios.h>
#include <unistd.h>

/* ---------- helpers ---------- */

// Read password without echoing characters to terminal
static void readPassword(char *buf, int maxLen) {
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ECHO); // Disable echo
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  fgets(buf, maxLen, stdin);
  buf[strcspn(buf, "\n")] = '\0';

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore echo
  printf("\n"); // Newline since user's Enter wasn't echoed
}

static unsigned int totalUsersCSV() {
  FILE *f = fopen(USERS_CSV, "r");
  if (!f)
    return 0;
  int n = -1;
  char line[512];
  while (fgets(line, sizeof(line), f))
    n++;
  fclose(f);
  return (n < 0) ? 0 : (unsigned int)n;
}

static int readUsers(User *users, unsigned int max) {
  FILE *f = fopen(USERS_CSV, "r");
  if (!f)
    return 0;

  char line[512];
  int row = 0;
  unsigned int count = 0;

  while (fgets(line, sizeof(line), f) && count < max) {
    row++;
    if (row == 1)
      continue; // skip header

    char *value = strtok(line, "|");
    int col = 0;
    while (value) {
      // skip leading spaces
      while (*value == ' ')
        value++;

      switch (col) {
      case 0:
        users[count].id = (unsigned int)atoi(value);
        break;
      case 1:
        strncpy(users[count].username, value, 63);
        users[count].username[63] = '\0';
        break;
      case 2:
        strncpy(users[count].password_hash, value, ARGON2_ENCODED_LEN - 1);
        users[count].password_hash[ARGON2_ENCODED_LEN - 1] = '\0';
        break;
      case 3:
        users[count].role = (Role)atoi(value);
        break;
      }
      value = strtok(NULL, "|");
      col++;
    }
    count++;
  }
  fclose(f);
  return (int)count;
}

static void writeUserCSV(User *user) {
  FILE *f = fopen(USERS_CSV, "r");
  int exists = (f != NULL);
  if (f)
    fclose(f);

  f = fopen(USERS_CSV, "a");
  if (!f) {
    printf(RED "Error: Could not open users file for writing.\n" reset);
    return;
  }

  if (!exists) {
    fprintf(f, "ID|Username|PasswordHash|Role\n");
  }
  fprintf(f, "%d|%s|%s|%d\n", user->id, user->username, user->password_hash,
          user->role);
  fclose(f);
}

/* ---------- Public API ---------- */

void seedDefaultAdmin() {
  FILE *f = fopen(USERS_CSV, "r");
  if (f) {
    // File exists — check if it uses Argon2 format (starts with $argon2)
    char header[512];
    char firstUser[512];
    int hasHeader = (fgets(header, sizeof(header), f) != NULL);
    int hasUser = (hasHeader && fgets(firstUser, sizeof(firstUser), f) != NULL);

    if (hasUser) {
      // Check for Argon2 marker in password hash field
      if (strstr(firstUser, "$argon2") != NULL) {
        fclose(f);
        return; // Already using Argon2 format
      }
      // Old format detected — re-create
      fclose(f);
      printf(YEL "Upgrading user database to Argon2id hashing...\n" reset);
      remove(USERS_CSV);
    } else {
      fclose(f);
      // Empty/corrupt file, re-create
      remove(USERS_CSV);
    }
  }

  // Create default admin with Argon2id
  User admin;
  admin.id = 1;
  strcpy(admin.username, "admin");

  if (!hashPasswordArgon2("Admin123!", admin.password_hash,
                          ARGON2_ENCODED_LEN)) {
    printf(RED "Error: Could not hash admin password.\n" reset);
    return;
  }

  admin.role = ROLE_ADMIN;
  writeUserCSV(&admin);
  printf(YEL "Default admin account created (username: admin)\n" reset);
}

int login(Session *session) {
  char username[64];
  char password[128];

  unsigned int totalUsers = totalUsersCSV();
  if (totalUsers == 0) {
    printf(RED "No users found. System error.\n" reset);
    return 0;
  }

  User *users = (User *)malloc(totalUsers * sizeof(User));
  if (!users) {
    printf(RED "Memory allocation error.\n" reset);
    return 0;
  }
  int count = readUsers(users, totalUsers);

  for (int attempt = 0; attempt < MAX_LOGIN_ATTEMPTS; attempt++) {
    printf(CYN "\n===== Login (%d/%d) =====\n" reset, attempt + 1,
           MAX_LOGIN_ATTEMPTS);
    printf("Username: ");
    fgets(username, 64, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Password: ");
    readPassword(password, 128);

    for (int i = 0; i < count; i++) {
      if (strcmp(users[i].username, username) == 0 &&
          verifyPasswordArgon2(users[i].password_hash, password)) {

        session->user_id = users[i].id;
        strncpy(session->username, users[i].username, 63);
        session->username[63] = '\0';
        session->role = users[i].role;
        session->active = 1;
        session->login_time = time(NULL);
        session->last_activity = time(NULL);

        printf(GRN "\nLogin successful! Welcome, %s (%s)\n" reset,
               session->username,
               session->role == ROLE_ADMIN ? "Admin" : "Employee");

        memset(password, 0, sizeof(password));
        free(users);
        return 1;
      }
    }

    memset(password, 0, sizeof(password));
    printf(RED "Invalid username or password.\n" reset);
  }

  printf(RED "\nMaximum login attempts reached. Exiting.\n" reset);
  free(users);
  return 0;
}

void createUser(Session *session) {
  if (session->role != ROLE_ADMIN) {
    printf(RED "Access denied. Admin privileges required.\n" reset);
    return;
  }

  User newUser;
  char password[128];
  char confirmPassword[128];
  unsigned int roleChoice;

  unsigned int totalUsers = totalUsersCSV();
  newUser.id = totalUsers + 1;

  printf(CYN "===== Create New User =====\n" reset);

  // Username with validation
  printf("Username (3-63 chars, alphanumeric/underscore): ");
  fgets(newUser.username, 64, stdin);
  newUser.username[strcspn(newUser.username, "\n")] = '\0';

  while (!validateUsername(newUser.username)) {
    printf(RED "Invalid username! " reset
               "Must be 3-63 chars, alphanumeric/underscore only: ");
    fgets(newUser.username, 64, stdin);
    newUser.username[strcspn(newUser.username, "\n")] = '\0';
  }

  // Check if username already exists
  User *users =
      (User *)malloc((totalUsers > 0 ? totalUsers : 1) * sizeof(User));
  if (users) {
    int count = readUsers(users, totalUsers);
    for (int i = 0; i < count; i++) {
      if (strcmp(users[i].username, newUser.username) == 0) {
        printf(RED "Username '%s' already exists.\n" reset, newUser.username);
        free(users);
        printf("\nPress enter to continue...");
        getchar();
        ClearScreen();
        return;
      }
    }
    free(users);
  }

  // Password with enhanced policy
  int validPass = 0;
  while (!validPass) {
    printf("Password (min 8 chars, 1 uppercase, 1 digit, 1 special): ");
    readPassword(password, 128);

    if (!validatePassword(password)) {
      printf(RED "Password does not meet requirements!\n" reset);
      continue;
    }

    printf("Confirm password: ");
    readPassword(confirmPassword, 128);

    if (strcmp(password, confirmPassword) != 0) {
      printf(RED "Passwords do not match!\n" reset);
      continue;
    }

    validPass = 1;
  }

  // Hash with Argon2id
  if (!hashPasswordArgon2(password, newUser.password_hash,
                          ARGON2_ENCODED_LEN)) {
    printf(RED "Error: Password hashing failed.\n" reset);
    memset(password, 0, sizeof(password));
    memset(confirmPassword, 0, sizeof(confirmPassword));
    return;
  }

  memset(password, 0, sizeof(password));
  memset(confirmPassword, 0, sizeof(confirmPassword));

  // Role
  printf("Select role:\n");
  printf("  0 - Admin\n");
  printf("  1 - Employee\n");
  printf("Choice: ");
  char roleInput[10];
  fgets(roleInput, 10, stdin);
  roleChoice = (unsigned int)atoi(roleInput);

  if (roleChoice > 1) {
    printf(RED "Invalid role. Defaulting to Employee.\n" reset);
    roleChoice = 1;
  }
  newUser.role = (Role)roleChoice;

  writeUserCSV(&newUser);

  ClearScreen();
  printf(GRN "User '%s' created successfully as %s.\n\n" reset,
         newUser.username, newUser.role == ROLE_ADMIN ? "Admin" : "Employee");

  printf("Press enter to continue...");
  getchar();
  ClearScreen();
}

void listUsers(Session *session) {
  if (session->role != ROLE_ADMIN) {
    printf(RED "Access denied. Admin privileges required.\n" reset);
    return;
  }

  unsigned int totalUsers = totalUsersCSV();
  if (totalUsers == 0) {
    printf(RED "No users found.\n" reset);
    printf("Press enter to continue...");
    getchar();
    ClearScreen();
    return;
  }

  User *users = (User *)malloc(totalUsers * sizeof(User));
  if (!users) {
    printf(RED "Memory allocation error.\n" reset);
    return;
  }
  int count = readUsers(users, totalUsers);

  printf(CYN "\n\t\t***** USERS *****\n" reset);
  printf("--------------------------------------------------\n");
  printf("%-5s| %-20s| %-10s|\n", "ID", "Username", "Role");
  printf("--------------------------------------------------\n");

  for (int i = 0; i < count; i++) {
    printf("%-5d| %-20s| %-10s|\n", users[i].id, users[i].username,
           users[i].role == ROLE_ADMIN ? "Admin" : "Employee");
  }
  printf("--------------------------------------------------\n\n");

  free(users);

  printf("Press enter to continue...");
  getchar();
  ClearScreen();
}

void userManagementMenu(Session *session) {
  if (session->role != ROLE_ADMIN) {
    printf(RED "Access denied. Admin privileges required.\n" reset);
    return;
  }

  unsigned short exitMenu = 0;
  char input[10];

  while (!exitMenu) {
    printf(CYN "\n===== User Management =====\n" reset);
    printf("---------------------------------\n");
    printf("| 1 | %-25s |\n", "Create New User");
    printf("| 2 | %-25s |\n", "List All Users");
    printf("| 0 | %-25s |\n", "Back to Main Menu");
    printf("---------------------------------\n");
    printf("Actions: ");
    fgets(input, 10, stdin);
    int choice = atoi(input);

    switch (choice) {
    case 0:
      exitMenu = 1;
      ClearScreen();
      break;
    case 1:
      ClearScreen();
      createUser(session);
      break;
    case 2:
      ClearScreen();
      listUsers(session);
      break;
    default:
      ClearScreen();
      printf(RED "Please select from the available options.\n" reset);
      break;
    }
  }
}

/* ---------- Session Management ---------- */

int checkSessionTimeout(Session *session) {
  if (!session->active)
    return 0;

  time_t now = time(NULL);
  double inactivity = difftime(now, session->last_activity);
  double totalDuration = difftime(now, session->login_time);

  if (inactivity >= SESSION_TIMEOUT_SEC) {
    printf(RED "\n===== Session Expired =====\n" reset);
    printf("Your session has been inactive for more than %d minutes.\n",
           SESSION_TIMEOUT_SEC / 60);
    printf("You have been logged out for security.\n\n");
    logoutSession(session);
    return 0;
  }

  if (totalDuration >= SESSION_MAX_DURATION) {
    printf(RED "\n===== Session Expired =====\n" reset);
    printf("Your session has exceeded the maximum duration of %d hour(s).\n",
           SESSION_MAX_DURATION / 3600);
    printf("Please log in again.\n\n");
    logoutSession(session);
    return 0;
  }

  return 1;
}

void updateSessionActivity(Session *session) {
  if (session->active) {
    session->last_activity = time(NULL);
  }
}

void logoutSession(Session *session) {
  session->active = 0;
  session->user_id = 0;
  session->role = ROLE_EMPLOYEE;
  session->login_time = 0;
  session->last_activity = 0;
  memset(session->username, 0, sizeof(session->username));
}

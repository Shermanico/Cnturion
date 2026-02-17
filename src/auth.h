#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clear.h>
#include <color.h>
#include <security.h>

#define USERS_CSV "data/Users.csv"
#define MAX_LOGIN_ATTEMPTS 3

typedef enum { ROLE_ADMIN = 0, ROLE_EMPLOYEE = 1 } Role;

typedef struct {
  unsigned int id;
  char username[64];
  char password_hash[HASH_HEX_LEN];
  Role role;
} User;

typedef struct {
  unsigned int user_id;
  char username[64];
  Role role;
  int active;
} Session;

// Ensure Users.csv exists with a default admin account
void seedDefaultAdmin();

// Prompt for login. Returns 1 on success, 0 on failure (after
// MAX_LOGIN_ATTEMPTS).
int login(Session *session);

// Admin-only: create a new user
void createUser(Session *session);

// Admin-only: list all users
void listUsers(Session *session);

// User management sub-menu (admin only)
void userManagementMenu(Session *session);

#endif

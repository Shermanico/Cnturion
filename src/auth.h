#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <clear.h>
#include <color.h>
#include <security.h>

#define USERS_CSV "data/Users.csv"
#define MAX_LOGIN_ATTEMPTS 3
#define SESSION_TIMEOUT_SEC 300   // 5 minutes of inactivity
#define SESSION_MAX_DURATION 3600 // 1 hour hard limit

typedef enum { ROLE_ADMIN = 0, ROLE_EMPLOYEE = 1 } Role;

typedef struct {
  unsigned int id;
  char username[64];
  char password_hash[ARGON2_ENCODED_LEN]; // Argon2id encoded string
  Role role;
} User;

typedef struct {
  unsigned int user_id;
  char username[64];
  Role role;
  int active;
  time_t last_activity;
  time_t login_time;
} Session;

// Ensure Users.csv exists with a default admin account
void seedDefaultAdmin();

// Prompt for login. Returns 1 on success, 0 on failure.
int login(Session *session);

// Admin-only: create a new user
void createUser(Session *session);

// Admin-only: list all users
void listUsers(Session *session);

// User management sub-menu (admin only)
void userManagementMenu(Session *session);

// Check if session has timed out (inactivity OR max duration).
// Returns 1 if still active, 0 if expired.
int checkSessionTimeout(Session *session);

// Update the last_activity timestamp to current time
void updateSessionActivity(Session *session);

// Securely clear session data and mark as inactive
void logoutSession(Session *session);

#endif

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define AUDIT_LOG_FILE "logs/audit.log"
#define LOG_DIR "logs"

typedef enum { LOG_INFO, LOG_WARN, LOG_ERROR, LOG_AUDIT } LogLevel;

// Initialize the logger (create logs directory if needed)
// Returns 1 on success, 0 on failure
int initLogger(void);

// Log an event to the audit log file
// user can be NULL for system-level events
void logEvent(LogLevel level, const char *user, const char *action,
              const char *detail);

#endif

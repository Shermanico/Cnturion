#include <logger.h>

static const char *levelToString(LogLevel level) {
  switch (level) {
  case LOG_INFO:
    return "INFO";
  case LOG_WARN:
    return "WARN";
  case LOG_ERROR:
    return "ERROR";
  case LOG_AUDIT:
    return "AUDIT";
  default:
    return "UNKNOWN";
  }
}

int initLogger(void) {
  // Create logs directory if it doesn't exist
  struct stat st = {0};
  if (stat(LOG_DIR, &st) == -1) {
    if (mkdir(LOG_DIR, 0700) != 0) {
      fprintf(stderr, "Warning: Could not create logs directory.\n");
      return 0;
    }
  }

  // Test that we can write to the log file
  FILE *f = fopen(AUDIT_LOG_FILE, "a");
  if (!f) {
    fprintf(stderr, "Warning: Could not open audit log file.\n");
    return 0;
  }
  fclose(f);
  return 1;
}

void logEvent(LogLevel level, const char *user, const char *action,
              const char *detail) {
  FILE *f = fopen(AUDIT_LOG_FILE, "a");
  if (!f)
    return; // Silent fail — can't log to log file

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);

  char timestamp[26];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

  const char *username = user ? user : "SYSTEM";
  const char *det = detail ? detail : "";

  fprintf(f, "[%s] [%-5s] [%s] %s: %s\n", timestamp, levelToString(level),
          username, action, det);
  fclose(f);
}

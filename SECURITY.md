# Cnturion Inventory Manager - Security Documentation

## Table of Contents

- [Overview](#overview)
- [Security Architecture](#security-architecture)
- [Threat Model](#threat-model)
- [Security Controls](#security-controls)
- [Data Protection](#data-protection)
- [Audit and Logging](#audit-and-logging)
- [Secure Configuration](#secure-configuration)
- [Security Testing](#security-testing)
- [Known Limitations](#known-limitations)
- [Security Best Practices](#security-best-practices)

## Overview

The Cnturion Inventory Manager implements a defense-in-depth approach to security, with multiple layers of protection against common vulnerabilities. This document details the security architecture, controls, and best practices implemented in the application.

## Security Architecture

### Defense in Depth

The application uses multiple layers of security controls:

1. **Authentication Layer**: Password hashing, session management, account lockout
2. **Authorization Layer**: Role-based access control, permission checks
3. **Input Validation Layer**: Whitelist validation, sanitization
4. **Database Layer**: Prepared statements, row-level security
5. **Application Layer**: Secure error handling, audit logging
6. **Infrastructure Layer**: Secure configuration, file permissions

### Security Principles

- **Principle of Least Privilege**: Users have only the permissions they need
- **Defense in Depth**: Multiple layers of security controls
- **Fail Secure**: System fails to a secure state
- **Secure by Default**: Default settings are secure
- **Transparency**: Security decisions are documented

## Threat Model

### Identified Threats

| Threat | Likelihood | Impact | Mitigation |
|--------|------------|--------|------------|
| SQL Injection | Medium | High | Prepared statements |
| Brute Force Attack | High | Medium | Account lockout |
| Password Theft | Medium | High | SHA-512 hashing with salt |
| Unauthorized Access | Medium | High | RBAC, session management |
| Data Tampering | Low | High | Audit logging, immutable records |
| Denial of Service | Low | Medium | Input validation, resource limits |
| Information Leakage | Medium | Medium | Secure error handling |

### Attack Vectors

1. **SQL Injection**: Attempting to inject SQL through input fields
   - **Mitigation**: All queries use prepared statements with parameter binding

2. **Brute Force Login**: Repeated login attempts to guess credentials
   - **Mitigation**: Account lockout after 5 failed attempts (30 minutes)

3. **Session Hijacking**: Stealing session tokens
   - **Mitigation**: Session timeout, secure session storage

4. **Privilege Escalation**: Attempting to access admin functions
   - **Mitigation**: Role-based access control, permission checks

5. **Input Manipulation**: Submitting malformed or malicious input
   - **Mitigation**: Comprehensive input validation and sanitization

## Security Controls

### Authentication Controls

#### Password Hashing

```c
// Password hashing using SHA-512 with salt
char* hash_password(const char* password, const char* salt);
int verify_password(const char* password, const char* hash);
```

- **Algorithm**: SHA-512 via `crypt()` with `$6$` prefix
- **Salt**: Automatically generated and included in hash
- **Storage**: Hash stored in database, never plain text
- **Verification**: Hash comparison prevents timing attacks

#### Account Lockout

```c
// Automatic lockout after failed attempts
#define MAX_FAILED_LOGIN_ATTEMPTS 5
#define ACCOUNT_LOCKOUT_DURATION 30  // minutes
```

- Tracks failed login attempts per user
- Automatically locks account after threshold
- Logs all failed attempts for audit
- Admin can unlock locked accounts

### Authorization Controls

#### Role-Based Access Control (RBAC)

```c
typedef enum {
    ROLE_ADMIN = 1,
    ROLE_USER = 2
} UserRole;
```

- **Admin Role**: Full access to all features
- **User Role**: Limited to own products
- Permission checks before each operation
- Role stored in database, verified on each request

#### Permission Checks

```c
// Check if user has admin role
int auth_is_admin(const UserSession* session);

// Check if user is authenticated
int auth_is_authenticated(const UserSession* session);
```

### Input Validation Controls

#### Whitelist Validation

```c
// Validate username (alphanumeric, underscore, hyphen)
int validator_validate_username(const char* username);

// Validate email format
int validator_validate_email(const char* email);

// Validate password strength
int validator_validate_password(const char* password);
```

- Username: 3-50 characters, alphanumeric + underscore + hyphen
- Email: Valid email format, max 100 characters
- Password: 8-128 characters, uppercase, lowercase, digit, special
- Product name: 1-100 characters
- SKU: 1-50 characters, alphanumeric + underscore + hyphen

#### Buffer Overflow Prevention

```c
// Safe string functions with length checks
size_t safe_strlen(const char* str, size_t max_len);
int safe_strcpy(char* dest, const char* src, size_t dest_size);
int safe_strcat(char* dest, const char* src, size_t dest_size);
```

### Database Security Controls

#### SQL Injection Prevention

```c
// All queries use prepared statements
const char* sql = "SELECT * FROM users WHERE username = ?";
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
sqlite3_step(stmt);
sqlite3_finalize(stmt);
```

- **No string concatenation** in SQL queries
- **Parameter binding** for all user input
- **Type-safe** binding functions
- **Automatic escaping** by SQLite

#### Row-Level Security

```c
// Users can only access their own products
const char* sql = "SELECT * FROM products WHERE user_id = ?";
sqlite3_bind_int(stmt, 1, session->id);
```

- Products filtered by `user_id`
- Admins can see all products
- Prevents unauthorized data access

### Audit and Logging Controls

#### Immutable Audit Trail

```c
// Audit log stored in database (not file)
typedef struct {
    int64_t id;
    int user_id;
    char username[MAX_USERNAME_LEN];
    char action[50];
    char details[MAX_LOG_MESSAGE];
    char ip_address[50];
    char timestamp[20];
} AuditLog;
```

- **Database storage**: Cannot be modified by file system
- **Append-only**: Records cannot be deleted or modified
- **Comprehensive**: Logs all security-relevant events
- **Tamper-evident**: Any tampering would be detectable

#### Security Events Logged

- Login attempts (success and failure)
- Logout events
- Account creation and deletion
- Password changes
- Product CRUD operations
- Authorization failures
- System errors

#### Log Rotation

```c
// Automatic rotation to prevent disk overflow
#define MAX_LOG_SIZE 10485760  // 10MB
#define MAX_LOG_FILES 5        // Keep 5 rotated logs
```

- Automatic rotation when size limit reached
- Old logs renamed with `.N` suffix
- Oldest log deleted when limit exceeded
- Prevents denial of service via log file growth

### Error Handling Controls

#### Secure Error Messages

```c
// Generic messages to users
const char* get_user_friendly_error(ErrorCode code);

// Detailed messages in logs
logger_log(LOG_ERROR, "Detailed error: %s", detailed_message);
```

- **User-facing**: Generic, non-revealing messages
- **Internal logs**: Detailed error information
- **No stack traces**: Never shown to users
- **No system details**: Paths, versions hidden from users

## Data Protection

### Password Security

- **Storage**: SHA-512 hash with salt, never plain text
- **Transmission**: Not applicable (local application)
- **Generation**: Strong password requirements enforced
- **Recovery**: Not implemented (security decision)

### Sensitive Data Protection

- **Database**: SQLite file with restricted permissions (600)
- **Configuration**: Config file with restricted permissions (600)
- **Logs**: Log files with restricted permissions (600)
- **Memory**: Sensitive data zeroed after use

### Data Integrity

- **Audit Trail**: Immutable records of all changes
- **Referential Integrity**: Foreign keys in database schema
- **Validation**: All inputs validated before storage

## Audit and Logging

### Log Levels

```c
typedef enum {
    LOG_DEBUG = 0,    // Detailed debugging information
    LOG_INFO = 1,     // General informational messages
    LOG_WARNING = 2,  // Warning messages
    LOG_ERROR = 3,    // Error messages
    LOG_SECURITY = 4  // Security-related events
} LogLevel;
```

### Audit Log Schema

```sql
CREATE TABLE audit_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    username TEXT NOT NULL,
    action TEXT NOT NULL,
    details TEXT,
    ip_address TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

### Security Events Logged

1. **Authentication Events**
   - Login success
   - Login failure
   - Logout
   - Account lockout
   - Account unlock

2. **Authorization Events**
   - Permission denied
   - Unauthorized access attempt

3. **User Management Events**
   - User creation
   - User deletion
   - Password change
   - Role change

4. **Data Events**
   - Product creation
   - Product update
   - Product deletion
   - Data access

5. **System Events**
   - Application start
   - Application stop
   - Error conditions

## Secure Configuration

### File Permissions

```bash
# Configuration file (sensitive)
chmod 600 config/config.env

# Database file
chmod 600 data/cnturion.db

# Log files
chmod 600 logs/cnturion.log*

# Executable
chmod 755 bin/cnturion
```

### Configuration Settings

| Setting | Default | Security Consideration |
|---------|---------|------------------------|
| `MAX_FAILED_LOGIN_ATTEMPTS` | 5 | Prevents brute force |
| `ACCOUNT_LOCKOUT_DURATION` | 30 | Balance security and usability |
| `MIN_PASSWORD_LENGTH` | 8 | Enforces strong passwords |
| `MAX_PASSWORD_LENGTH` | 128 | Prevents DoS via long passwords |
| `SESSION_TIMEOUT` | 60 | Prevents session hijacking |
| `LOG_LEVEL` | INFO | Balance detail and performance |

### Environment Variables

The application reads configuration from `config/config.env`. This file:

- Should not be committed to version control
- Should have restricted permissions (600)
- Contains sensitive configuration
- Should be backed up securely

## Security Testing

### Recommended Tests

#### 1. SQL Injection Testing

```bash
# Test login form with SQL injection
Username: admin' OR '1'='1
Password: anything

# Expected: Login denied, logged in audit log
```

#### 2. Brute Force Testing

```bash
# Attempt multiple failed logins
# Expected: Account locked after 5 attempts
```

#### 3. Authorization Testing

```bash
# Login as regular user
# Attempt to access admin functions
# Expected: Access denied, logged in audit log
```

#### 4. Input Validation Testing

```bash
# Test with various malicious inputs
- Very long strings
- Special characters
- Null bytes
- Unicode characters
# Expected: Input rejected, logged if suspicious
```

#### 5. Session Testing

```bash
# Test session timeout
# Expected: Session expires after timeout period
```

### Security Review Checklist

- [ ] All database queries use prepared statements
- [ ] Passwords are hashed before storage
- [ ] Input validation on all user inputs
- [ ] Error messages don't reveal sensitive information
- [ ] Audit logging for all security events
- [ ] File permissions are set correctly
- [ ] Configuration file is not in version control
- [ ] Default admin password is changed
- [ ] Account lockout is working
- [ ] Session timeout is configured

## Known Limitations

1. **No HTTPS**: Application runs locally, no network transmission
2. **No Multi-Factor Authentication**: Single factor only
3. **No Password Recovery**: Security decision to prevent social engineering
4. **No Encryption at Rest**: Database stored as plain SQLite file
5. **No Rate Limiting**: Only account lockout for brute force protection
6. **No Input Sanitization**: Uses validation instead (whitelist approach)

## Security Best Practices

### For Developers

1. **Never trust user input**: Always validate and sanitize
2. **Use prepared statements**: Never concatenate SQL strings
3. **Check return values**: Always handle errors properly
4. **Free allocated memory**: Prevent memory leaks
5. **Log security events**: Maintain audit trail
6. **Use secure error handling**: Don't leak information

### For Administrators

1. **Change default password**: Immediately after first login
2. **Set file permissions**: Restrict access to sensitive files
3. **Review audit logs**: Regularly check for suspicious activity
4. **Keep software updated**: Apply security patches
5. **Backup regularly**: Protect against data loss
6. **Monitor disk space**: Ensure logs don't fill disk

### For Users

1. **Use strong passwords**: Follow password policy
2. **Don't share credentials**: Keep passwords secret
3. **Report issues**: Notify admins of suspicious activity
4. **Log out**: When finished using the application
5. **Change passwords**: Periodically

## Security References

- [OWASP Top 10](https://owasp.org/www-project-top-ten/)
- [CWE/SANS Top 25](https://cwe.mitre.org/top25/)
- [SQLite Security](https://www.sqlite.org/security.html)
- [OpenSSL Documentation](https://www.openssl.org/docs/)

## Contact

For security concerns or questions, contact the development team:
- **Nico**: Developer
- **Ximena**: Developer

DevSecOps Class Project

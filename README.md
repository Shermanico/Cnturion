# Cnturion Inventory Manager

A secure inventory management system for micro businesses, developed in C with SQLite database support. This project implements comprehensive security best practices for DevSecOps class.

## Table of Contents

- [Quick Start](#quick-start)
- [Progressive Learning Path](#progressive-learning-path)
- [Project Overview](#project-overview)
- [Features](#features)
- [Security Features](#security-features)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Development](#development)
- [Testing](#testing)
- [Authors](#authors)
- [License](#license)

## Quick Start

### For Learning (Recommended)

If you're learning C and DevSecOps, use the **progressive versions** in the `versions/` folder:

```bash
# Start with Version 1 (Basic Setup)
cd versions/version01_basic_setup
make
./bin/cnturion

# Read the learning guide
cat versions/README.md
```

### For Production Use

If you want the complete application:

```bash
# Build the complete application
make

# Run the application
./bin/cnturion
```

## Progressive Learning Path

This project is organized into **10 progressive versions** that build upon each other. Each version adds new features and security controls while teaching important concepts.

### Version Overview

| Version | Name | Status | Key Concepts |
|---------|--------|---------|--------------|
| 1 | Basic Setup | ✅ Complete | Project structure, SQLite basics |
| 2 | Authentication | 📝 To implement | Password hashing, sessions |
| 3 | Authorization | 📝 To implement | RBAC, permissions |
| 4 | Input Validation | 📝 To implement | Whitelist validation, buffer overflow prevention |
| 5 | Logging | 📝 To implement | File logging, log rotation |
| 6 | Audit Logging | 📝 To implement | Audit trail, security events |
| 7 | Inventory Management | 📝 To implement | CRUD operations, row-level security |
| 8 | User Management | 📝 To implement | Admin operations, account lifecycle |
| 9 | Security Features | 📝 To implement | Account lockout, password policies |
| 10 | Complete | 📝 To implement | Full integration, testing |

### How to Use Progressive Versions

1. **Start with Version 1:**
   ```bash
   cd versions/version01_basic_setup
   make
   ./bin/cnturion
   ```

2. **Study the code:**
   - Read through all files
   - Understand the structure
   - Learn the concepts

3. **Move to next version:**
   - Copy previous version files
   - Add new features
   - Test and debug

4. **Continue progressively:**
   - Each version builds on previous
   - Learn incrementally
   - Test at each step

**See [`versions/README.md`](versions/README.md) for detailed version information.**

## Project Overview

**Cnturion Inventory Manager** is a secure inventory management system for micro businesses, developed as a DevSecOps class project by Nico and Ximena.

### Learning Objectives

This project teaches:
- **C Programming:** Pointers, structs, memory management, file I/O
- **SQLite Database:** Database design, prepared statements, SQL injection prevention
- **Security Best Practices:** Password hashing, RBAC, input validation, audit logging
- **DevSecOps Principles:** Defense in depth, least privilege, secure by default

## Features

- **User Authentication:** Secure login system with password hashing
- **Role-Based Access Control (RBAC):** Admin and regular user roles
- **Product Management:** Create, read, update, and delete products
- **Inventory Tracking:** Track stock levels and product information
- **Audit Logging:** Immutable audit trail for security events
- **User Management:** Admin can create and manage user accounts
- **Account Lockout:** Automatic lockout after failed login attempts
- **Log Rotation:** Automatic log file rotation to prevent disk overflow

## Security Features

This project implements the following security measures:

### Authentication & Authorization
- **Password Hashing:** Uses SHA-512 with salt via `crypt()` for secure password storage
- **Role-Based Access Control:** Users can only access functions appropriate to their role
- **Session Management:** Secure session tracking with timeout support
- **Account Lockout:** Automatic lockout after 5 failed login attempts (30 minutes)

### Input Validation
- **Whitelist Validation:** All user inputs are validated against allowed patterns
- **Buffer Overflow Prevention:** Safe string functions with length checks
- **SQL Injection Prevention:** All database queries use prepared statements
- **XSS Prevention:** Output encoding for user-generated content

### Audit & Logging
- **Immutable Audit Trail:** All security events are logged to database
- **Comprehensive Logging:** Detailed logs for debugging and security monitoring
- **Log Rotation:** Automatic rotation to prevent disk overflow
- **Secure Error Handling:** Generic error messages to users, detailed logs internally

### Data Protection
- **Row-Level Security:** Users can only access their own products
- **Secure Configuration:** Configuration stored in separate file with restricted access
- **Memory Safety:** Proper memory allocation and deallocation
- **Type Safety:** Strong typing with explicit error codes

## Project Structure

```
Cnturion/
├── bin/                    # Compiled executable
│   └── cnturion           # Main application binary
├── config/                 # Configuration files
│   └── config.env         # Application configuration
├── data/                   # Database files
│   └── cnturion.db        # SQLite database (created on first run)
├── include/                # Header files
│   ├── auth.h            # Authentication and authorization
│   ├── common.h          # Common types and constants
│   ├── database.h         # Database operations
│   ├── inventory.h        # Inventory management
│   ├── logger.h           # Logging and audit
│   ├── security.h         # Security utilities
│   ├── ui.h               # User interface
│   ├── users.h            # User management
│   ├── utils.h            # Utility functions
│   └── validator.h        # Input validation
├── logs/                   # Log files
│   ├── cnturion.log       # Current log file
│   └── cnturion.log.N    # Rotated log files
├── obj/                    # Object files (compiled)
├── plans/                  # Project planning documents
│   └── inventory_manager_plan.md
├── src/                    # Source files
│   ├── auth.c             # Authentication implementation
│   ├── database.c         # Database operations
│   ├── inventory.c        # Inventory management
│   ├── logger.c           # Logging implementation
│   ├── main.c             # Application entry point
│   ├── security.c         # Security utilities
│   ├── ui.c               # User interface
│   ├── users.c            # User management
│   ├── utils.c            # Utility functions
│   └── validator.c        # Input validation
├── tests/                  # Test files
├── versions/                # Progressive learning versions
│   ├── version01_basic_setup/
│   ├── version02_authentication/
│   ├── version03_authorization/
│   ├── version04_validation/
│   ├── version05_logging/
│   ├── version06_audit/
│   ├── version07_inventory/
│   ├── version08_users/
│   ├── version09_security/
│   └── version10_complete/
├── .gitignore              # Git ignore rules
├── Makefile               # Build configuration
├── README.md              # This file
└── SECURITY.md            # Security documentation
```

## Prerequisites

### System Requirements
- Linux operating system (tested on Linux 6.18)
- GCC compiler with C99 support
- Make build system

### Required Libraries
- **SQLite3:** Database engine
  - Install: `sudo apt-get install libsqlite3-dev` (Ubuntu/Debian)
  - Install: `sudo yum install sqlite-devel` (RHEL/CentOS)
- **OpenSSL:** Cryptographic functions
  - Install: `sudo apt-get install libssl-dev` (Ubuntu/Debian)
  - Install: `sudo yum install openssl-devel` (RHEL/CentOS)
- **pthread:** Threading support (usually included with GCC)

## Installation

### 1. Clone or download the project
```bash
cd /path/to/your/projects
# Extract or copy the Cnturion directory
```

### 2. Install required dependencies
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libsqlite3-dev libssl-dev

# RHEL/CentOS
sudo yum groupinstall "Development Tools"
sudo yum install sqlite-devel openssl-devel
```

### 3. Build the project
```bash
cd Cnturion
make
```

This will:
- Create necessary directories (`bin/`, `obj/`, `data/`, `logs/`)
- Compile all source files
- Link the executable with required libraries
- Create the `bin/cnturion` executable

### 4. Set configuration file permissions
```bash
chmod 600 config/config.env
```

## Usage

### First Run

The first time you run the application, it will:
1. Initialize the SQLite database
2. Create the default admin user:
   - Username: `admin`
   - Password: `Admin123!`
   - **IMPORTANT**: Change this password immediately!

### Running the Application

```bash
# Run the application
./bin/cnturion

# Or use the Makefile
make run
```

### Default Admin Credentials

- **Username**: `admin`
- **Password**: `Admin123!`

**⚠️ SECURITY WARNING**: Change the default admin password immediately after first login!

### User Roles

#### Admin
- Can create, read, update, and delete products
- Can create, delete, lock, and unlock user accounts
- Can view all products (including those owned by other users)
- Has full access to all system features

#### Regular User
- Can create, read, update, and delete their own products
- Can only view their own products
- Cannot manage other users

## Configuration

The application configuration is stored in [`config/config.env`](config/config.env). Key settings:

### Database Configuration
```bash
DB_PATH=data/cnturion.db    # Path to SQLite database
```

### Logging Configuration
```bash
LOG_PATH=logs/cnturion.log  # Path to log file
LOG_LEVEL=INFO               # Log level (DEBUG, INFO, WARNING, ERROR, SECURITY)
MAX_LOG_SIZE=10485760        # Max log size in bytes (10MB)
MAX_LOG_FILES=5              # Number of rotated log files to keep
```

### Security Configuration
```bash
MAX_FAILED_LOGIN_ATTEMPTS=5         # Failed attempts before lockout
ACCOUNT_LOCKOUT_DURATION=30         # Lockout duration in minutes
MIN_PASSWORD_LENGTH=8                # Minimum password length
MAX_PASSWORD_LENGTH=128              # Maximum password length
REQUIRE_UPPERCASE=1                  # Require uppercase letters
REQUIRE_LOWERCASE=1                  # Require lowercase letters
REQUIRE_DIGITS=1                     # Require digits
REQUIRE_SPECIAL=1                    # Require special characters
```

### Session Configuration
```bash
SESSION_TIMEOUT=60  # Session timeout in minutes (0 = no timeout)
```

## Development

### Building the Project

```bash
# Clean build artifacts
make clean

# Build the project
make

# Rebuild everything
make clean && make
```

### Code Structure

The project follows a modular architecture with clear separation of concerns:

- **Header files (`include/`)**: Define interfaces and data structures
- **Source files (`src/`)**: Implement the functionality
- **Common types**: Defined in [`include/common.h`](include/common.h)
- **Error handling**: Uses `ErrorCode` enum for consistent error reporting

### Adding New Features

1. **Define the interface** in the appropriate header file
2. **Implement the function** in the corresponding source file
3. **Add error handling** using `ErrorCode` enum
4. **Add logging** for security events using `logger_log_audit()`
5. **Update the UI** if user-facing
6. **Test thoroughly** before committing

### Coding Standards

- Use C99 standard
- Follow the existing code style
- Add comments explaining complex logic
- Use descriptive variable names
- Always check return values
- Free allocated memory
- Use prepared statements for all SQL queries

## Testing

### Running Tests

```bash
# Run all tests
make test

# Run specific test
./bin/test_validator
```

### Test Coverage

The project includes tests for:
- Password hashing and verification
- Input validation functions
- Database operations
- Authentication and authorization
- Security utilities

### Security Testing

Before deployment, perform:
1. **SQL Injection Testing**: Attempt to inject SQL through input fields
2. **Authentication Testing**: Test login with invalid credentials
3. **Authorization Testing**: Verify users cannot access unauthorized functions
4. **Input Validation Testing**: Test with malformed and malicious input
5. **Session Management Testing**: Verify session timeout and security

## Authors

- **Nico** - Developer
- **Ximena** - Developer

DevSecOps Class Project

## License

This project is developed for educational purposes as part of a DevSecOps class assignment.

## Acknowledgments

- SQLite development team for the excellent database library
- OpenSSL project for cryptographic functions
- DevSecOps class instructors for guidance and requirements

## Support

For issues or questions, please contact the development team or refer to [`SECURITY.md`](SECURITY.md) for detailed security documentation.

**For learning purposes, see [`versions/README.md`](versions/README.md) for the progressive version guide.**

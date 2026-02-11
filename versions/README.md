# Cnturion Inventory Manager - Progressive Versions

## Overview

This directory contains progressive versions of the Cnturion Inventory Manager application. Each version builds upon the previous one, gradually adding features and security controls.

## Version Structure

```
versions/
├── version01_basic_setup/      # Basic project structure, database
├── version02_authentication/       # Login/logout functionality
├── version03_authorization/        # Roles and RBAC
├── version04_validation/          # Input validation
├── version05_logging/             # Logging system
├── version06_audit/               # Audit logging
├── version07_inventory/           # Product CRUD
├── version08_users/               # User management
├── version09_security/            # Security features
└── version10_complete/           # Complete application
```

## Version Progression

### Version 1: Basic Setup
**Status:** ✅ Complete

**Features:**
- Basic project structure
- Database initialization with SQLite
- Table schema creation (users, products, audit_logs)
- Utility functions (string, memory, error handling)
- Makefile for building

**Learning Objectives:**
- C project organization
- SQLite C API basics
- Makefile fundamentals
- Basic error handling

**Files:** 3 source files, 3 header files

---

### Version 2: Authentication
**Status:** 📝 To be implemented

**New Features:**
- User login functionality
- Password hashing (SHA-512)
- Session management
- Login/logout UI

**Learning Objectives:**
- Password security concepts
- Session management
- User authentication flow
- Cryptographic functions

**Files:** +2 source files, +2 header files

---

### Version 3: Authorization
**Status:** 📝 To be implemented

**New Features:**
- Role-based access control (RBAC)
- Admin vs user roles
- Permission checking
- Access control functions

**Learning Objectives:**
- Authorization vs authentication
- Role-based security
- Permission systems
- Access control patterns

**Files:** +1 source file, +1 header file

---

### Version 4: Input Validation
**Status:** 📝 To be implemented

**New Features:**
- Username validation
- Email validation
- Password strength validation
- Product data validation
- Whitelist validation approach

**Learning Objectives:**
- Input validation principles
- Whitelist vs blacklist
- Buffer overflow prevention
- Regular expressions (if used)

**Files:** +1 source file, +1 header file

---

### Version 5: Logging System
**Status:** 📝 To be implemented

**New Features:**
- File-based logging
- Log levels (DEBUG, INFO, WARNING, ERROR, SECURITY)
- Log formatting
- Log rotation

**Learning Objectives:**
- Logging best practices
- File I/O operations
- Log rotation strategies
- Debugging with logs

**Files:** +1 source file, +1 header file

---

### Version 6: Audit Logging
**Status:** 📝 To be implemented

**New Features:**
- Audit trail in database
- Security event logging
- Immutable audit records
- Audit query functions

**Learning Objectives:**
- Audit trail importance
- Database logging
- Security event tracking
- Compliance considerations

**Files:** +1 source file (modifies logger)

---

### Version 7: Inventory Management
**Status:** 📝 To be implemented

**New Features:**
- Product CRUD operations
- Row-level security (users see own products)
- Product search/filter
- Inventory tracking

**Learning Objectives:**
- CRUD operations
- SQL injection prevention (prepared statements)
- Row-level security
- Data relationships

**Files:** +1 source file, +1 header file

---

### Version 8: User Management
**Status:** 📝 To be implemented

**New Features:**
- User creation (admin only)
- User deletion (admin only)
- Account lock/unlock
- User listing

**Learning Objectives:**
- Admin-only operations
- User lifecycle management
- Account security features
- Admin UI functions

**Files:** +1 source file, +1 header file

---

### Version 9: Security Features
**Status:** 📝 To be implemented

**New Features:**
- Account lockout after failed attempts
- Password policy enforcement
- Secure error handling
- Configuration management

**Learning Objectives:**
- Brute force prevention
- Password policies
- Secure error messages
- Configuration security

**Files:** +1 source file (security), +1 header file

---

### Version 10: Complete Application
**Status:** 📝 To be implemented

**New Features:**
- Complete UI with all menus
- Full integration of all features
- Comprehensive testing
- Documentation

**Learning Objectives:**
- System integration
- UI/UX considerations
- Testing strategies
- Documentation skills

**Files:** +1 source file (main), complete documentation

---

## How to Use These Versions

### Working Through the Versions

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
   
3. **Move to Version 2:**
   - Copy Version 1 files to Version 2
   - Add new authentication features
   - Test and debug
   
4. **Continue progressively:**
   - Each version builds on the previous
   - Learn incrementally
   - Test at each step

### Building Any Version

```bash
cd versions/versionXX_xxx
make          # Build the project
make clean    # Clean build artifacts
make run      # Build and run
```

## Security Features by Version

| Version | Security Feature | Implementation |
|---------|------------------|----------------|
| 1 | Database schema | Tables with proper constraints |
| 2 | Password hashing | SHA-512 with salt |
| 3 | RBAC | Role-based permissions |
| 4 | Input validation | Whitelist validation |
| 5 | Logging | Security event logging |
| 6 | Audit trail | Immutable database logs |
| 7 | Row-level security | Users see own data only |
| 8 | Account management | Admin controls |
| 9 | Account lockout | Brute force prevention |
| 10 | Complete system | All security integrated |

## Learning Path

### Phase 1: Foundations (Versions 1-2)
- **Goal:** Understand C basics and SQLite
- **Time Estimate:** 2-3 weeks
- **Key Concepts:** Pointers, structs, SQLite API

### Phase 2: Security Basics (Versions 3-4)
- **Goal:** Learn authentication and validation
- **Time Estimate:** 2-3 weeks
- **Key Concepts:** RBAC, input validation

### Phase 3: Data Management (Versions 5-7)
- **Goal:** Implement logging and inventory
- **Time Estimate:** 3-4 weeks
- **Key Concepts:** File I/O, CRUD operations

### Phase 4: Advanced Security (Versions 8-9)
- **Goal:** Add advanced security features
- **Time Estimate:** 2-3 weeks
- **Key Concepts:** Account management, lockout

### Phase 5: Integration (Version 10)
- **Goal:** Complete and test the application
- **Time Estimate:** 2-3 weeks
- **Key Concepts:** System integration, testing

## Tips for Learning

1. **Don't Rush:** Take time to understand each concept
2. **Experiment:** Modify code to see what happens
3. **Debug:** Learn from errors and warnings
4. **Document:** Add your own comments to code
5. **Test:** Verify each version works before moving on
6. **Ask Questions:** Use resources when stuck

## Resources

- **C Programming:** K&R C Programming Language
- **SQLite:** https://www.sqlite.org/docs.html
- **Security:** OWASP Top 10, CWE/SANS Top 25
- **DevSecOps:** DevSecOps course materials

## Notes

- Each version is self-contained
- Versions can be studied independently
- Later versions include all previous features
- Focus on understanding, not just completing

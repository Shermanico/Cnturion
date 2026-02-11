# Version 1: Basic Setup

## Overview
This is the first version of Cnturion Inventory Manager. It establishes the basic project structure and database connection.

## Features Implemented

### 1. Project Structure
- Basic directory layout (src/, include/, bin/, data/, logs/, config/)
- Makefile for building the project
- Common header files with types and constants

### 2. Database Setup
- SQLite database initialization
- Basic table schema (users, products, audit_logs)
- Database connection management
- Prepared statement foundation for SQL injection prevention

### 3. Basic Utilities
- String manipulation functions
- Memory management helpers
- Error handling framework

## Files Included
- `include/common.h` - Common types, enums, constants
- `include/database.h` - Database interface
- `include/utils.h` - Utility functions
- `src/database.c` - Database operations
- `src/utils.c` - Utility implementations
- `src/main.c` - Basic main function
- `Makefile` - Build configuration

## How to Build and Run

```bash
cd version01_basic_setup
make
./bin/cnturion
```

## What You'll Learn

### C Programming
- Basic project structure and organization
- Header files and include guards
- Makefile basics
- SQLite C API fundamentals

### DevSecOps Concepts
- Project organization
- Database schema design
- Basic security considerations (prepared statements)

## Next Steps

After completing this version, proceed to **Version 2: Authentication** to add login/logout functionality.

## Notes

- This version creates the database file in `data/cnturion.db`
- Default admin user is NOT created yet (comes in Version 2)
- No security features implemented yet
- Basic error handling only

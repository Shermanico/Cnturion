# Cnturion — Security Requirements Document

> **Project**: Cnturion — CLI Inventory Management System  
> **Authors**: Jorge Nicolás Jiménez Moreno & Penélope Ximena Sánchez Silva  
> **Course**: DevSecOps — Universidad Politécnica de Yucatán  
> **Supervisor**: Angel Arturo Pech Che  
> **Date**: 2026-03-07

---

## 1. Overview

Cnturion is a **CLI-based Inventory Management System** written in C with cross-platform support (Linux & Windows). It manages ~1000 product records stored in CSV with role-based access control, password hashing, session management, and audit logging.

### Threat Model

| Threat | Risk | Mitigation |
|---|---|---|
| Brute-force login | Medium | Max 3 login attempts per session |
| Password cracking | High | Argon2id hashing (memory-hard, OWASP-recommended) |
| Buffer overflow | High | `fgets()` bounded input, `strncpy()`, `-fstack-protector-strong` |
| Privilege escalation | Medium | Role-based access control (Admin/Employee separation) |
| Session hijacking | Low | Single-user CLI — 5-min inactivity + 1-hour max duration |
| Data exposure (at rest) | Medium | File permissions via `deploy.sh`/`deploy.bat` (chmod 600 / icacls) |
| Injection (CSV/input) | Medium | `sanitizeString()` + POSIX regex validation on all inputs |
| Supply chain (deps) | Low | 3 dependencies only, scanned via `arch-audit` — 0 CVEs |

---

## 2. Security Requirements (Tareas.pdf — 13 Points)

### 2.1 Security Requirements Defined ✅

This document and `extra/CONTEXT.md` define the security posture:
- Threat model (above)
- Per-requirement implementation status
- Accepted risks and N/A items documented

### 2.2 Secure Architecture Design ✅

- **Role separation**: Admin (full CRUD + user management) vs Employee (view, search, sell, restock)
- **Principle of minimum privilege**: Employees cannot add/update/delete products or manage users
- **Defense in depth**: Input validation → sanitization → bounded buffers → hardened compiler flags
- **Modular design**: Security logic isolated in `security.c/h`, auth in `auth.c/h`, logging in `logger.c/h`

### 2.3 Input Validation ✅

All user inputs are validated before processing:

| Input | Validation Method | Pattern |
|---|---|---|
| Username | POSIX regex (Linux) / manual (Windows) | `^[a-zA-Z0-9_]{3,63}$` |
| Password | Policy check | Min 8 chars, 1 uppercase, 1 digit, 1 special |
| Product name | POSIX regex / manual | `^[a-zA-Z0-9 _./-]{1,127}$` |
| Product category | POSIX regex / manual | `^[a-zA-Z0-9 _-]{1,63}$` |
| Quantity | Range validation | 1–999,999 |
| Price | Range validation | 0.01–999,999.99 |
| All text inputs | `sanitizeString()` | Strips control chars, double quotes |

### 2.4 Password Hashing ✅

- **Algorithm**: Argon2id (OWASP-recommended, memory-hard)
- **Parameters**: t=3 iterations, m=64MB memory, p=1 parallelism
- **Storage**: Self-contained encoded strings in pipe-delimited CSV
- **Masking**: Password input hidden via `termios` (Linux) / `conio.h` (Windows)

### 2.5 Authentication & Authorization ✅

- Login with max 3 attempts
- Role-based menus (Admin vs Employee)
- Username validation via regex
- Confirmation required for password during user creation
- Default admin auto-seeded on first run

### 2.6 Session Management ✅

- **Inactivity timeout**: 5 minutes
- **Max session duration**: 1 hour (hard limit)
- **Secure logout**: Zeroes all session fields (`memset`)
- **Activity tracking**: `updateSessionActivity()` on each menu action

### 2.7 Error Handling ✅

- All `fopen()`, `malloc()`, `realloc()` return values checked
- Generic error messages to users (no internal details exposed)
- Errors logged to `logs/audit.log` with timestamps
- Graceful degradation (safe defaults on config read failure)

### 2.8 Logging & Auditing ✅

- `logger.c/h` with `logEvent()` function
- Events: LOGIN_SUCCESS, LOGIN_FAILED, LOGOUT, SESSION_TIMEOUT, ADD/UPDATE/DELETE/SELL/RESTOCK_PRODUCT, errors
- Format: `[timestamp] [level] [username] EVENT: description`
- Log file: `logs/audit.log` (auto-created, chmod 600 via deploy script)

### 2.9 Dependencies Scanned ✅

| Dependency | Purpose | CVEs |
|---|---|---|
| libargon2 | Password hashing | None |
| ncurses | Terminal UI (Linux) | None |
| glibc (libm) | Math functions | None |

Scanned via `arch-audit`. Full report: `extra/dep_arch_audit.txt`

### 2.10 Secrets Out of Code ✅

- No hardcoded passwords or API keys in source code
- Default admin password exists only in `seedDefaultAdmin()` — auto-generated on first run
- `data/Users.csv` excluded from git via `.gitignore`
- `logs/audit.log` excluded from git via `.gitignore`
- Configuration paths documented in `config.ini`

### 2.11 HTTPS Everywhere — N/A

Cnturion is a CLI application with **no network communication**. All data is local (CSV files). This requirement does not apply. If network features are added in the future, TLS would be required.

### 2.12 Security Testing (SAST/DAST) ✅

| Tool | Type | Result |
|---|---|---|
| cppcheck 2.19.1 | SAST | 5 errors found and fixed, post-fix: 0 errors |
| flawfinder 2.0.19 | SAST | 199 hits (mostly false positives), CWE-mapped |
| AddressSanitizer (GCC) | DAST | No runtime memory errors |
| arch-audit | Dependency | 0 CVEs for project dependencies |

Full report: `extra/SAST_DAST_Report.md`

### 2.13 Hardened Deployment ✅

**Compiler hardening** (build.sh):
- `-O2 -Wall -Wextra` — optimization + all warnings
- `-fstack-protector-strong` — stack canaries
- `-D_FORTIFY_SOURCE=2` — runtime buffer overflow detection
- `-static` (Windows) — self-contained binary

**File permissions** (deploy.sh / deploy.bat):
- `data/Users.csv` → chmod 600 / icacls current-user-only
- `data/Inventory.csv` → chmod 600
- `logs/audit.log` → chmod 600
- `data/`, `logs/` → chmod 700
- Binary → chmod 750

---

## 3. Architecture Diagram

```
┌─────────────────────────────────────────────┐
│                  main.c                      │
│         Login → Role-based Menu              │
├──────────────┬──────────────┬────────────────┤
│   auth.c/h   │ security.c/h │  logger.c/h   │
│  Login/RBAC  │  Argon2id    │  Audit Log     │
│  Session Mgmt│  Validation  │  Timestamps    │
│  User CRUD   │  Sanitize    │  Error Logging │
├──────────────┴──────────────┴────────────────┤
│          product_controller.c/h              │
│     CRUD + Sell + Restock + Search           │
├──────────────────────────────────────────────┤
│          file_controller.c/h                 │
│       CSV Read/Write + INI Config            │
├──────────────────────────────────────────────┤
│  data/Users.csv  │  data/Inventory.csv       │
│  (Argon2 hashes) │  (~1000 products)         │
└──────────────────────────────────────────────┘
```

---

## 4. Accepted Risks

| Risk | Justification |
|---|---|
| `fgets()` return values unchecked | Harmless for interactive CLI — user is always present at terminal |
| `atoi()` without overflow check | Input is bounded by `fgets()` to small buffers; values validated after conversion |
| `srand()` in Windows fallback | Only used in legacy salt generation path; primary hashing uses Argon2id with proper randomness |
| CSV storage (not encrypted) | File permissions restrict access; encryption planned for Phase 3 (SQLCipher) |

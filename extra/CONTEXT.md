# Cnturion — Project Context & Workflow Log

> **Last updated**: 2026-03-07  
> **Authors**: Jorge Nicolás Jiménez Moreno (Nico) & Penélope Ximena Sánchez Silva (Chobiol) 
> **Course**: DevSecOps — Universidad Politécnica de Yucatán  
> **Supervisor**: Angel Arturo Pech Che  
> **Repository**: `Shermanico/Cnturion`

---

## What Is Cnturion?

A **CLI-based Inventory Management System** written in **C**, with **cross-platform support** (Linux & Windows). It manages a product catalog (~1000 records) stored in CSV, with colored terminal UI via ANSI escape codes. Includes **Argon2id password hashing**, **POSIX regex input validation** (manual validation on Windows), **role-based access control**, **session timeout**, **input sanitization**, and **audit logging**.

---

## Current Credentials

| Username | Role | Notes |
|----------|------|-------|
| `admin` | Admin (full access) | Auto-seeded on first run — see `seedDefaultAdmin()` in `auth.c` |
| `employee` | Employee (restricted) | Created via admin user management |

> Passwords are hashed using **Argon2id** (OWASP-recommended, memory-hard) with self-contained encoded strings stored in `data/Users.csv` (pipe-delimited). Default credentials are defined only in source code and applied on first run.

---

## Project Structure

```
Cnturion/
├── main.c                      # Entry point: login → role-based menus
├── src/
│   ├── auth.c / auth.h         # Login, user CRUD, session management
│   ├── security.c / security.h # Argon2id hashing, validation (POSIX regex on Linux, manual on Windows), sanitization
│   ├── product_controller.c/h  # CRUD + sell + restock logic
│   ├── file_controller.c/h     # CSV/INI read/write (with error handling)
│   ├── input_validation.c/h    # Numeric/string input validation
│   └── logger.c / logger.h     # Audit logging system
├── model/
│   └── product.h               # Product struct (id, name, category, qty, price)
├── utilities/
│   ├── clear.c / clear.h       # Cross-platform screen clear (ncurses/WinAPI)
│   └── color.h                 # ANSI color macros
├── data/
│   ├── Inventory.csv           # Product data (~1003 records)
│   ├── Inventory.txt           # Legacy (unused)
│   └── Users.csv               # User accounts with Argon2id hashes (pipe-delimited)
├── logs/
│   └── audit.log               # Security audit trail (auto-generated, gitignored)
├── config.ini                  # ViewLimit + documented data paths
├── .gitignore                  # Excludes binaries, Users.csv, logs, deps
├── build.sh / build.bat        # Build scripts (Linux / Windows native)
├── cross-build.sh              # Cross-compilation script (Linux→Windows + native Linux)
├── deploy.sh / deploy.bat      # Deployment hardening (chmod/icacls permissions)
├── deps/                       # Auto-downloaded dependencies (gitignored)
└── extra/
    ├── CONTEXT.md              # This file
    ├── Security_Requirements.md # Full security requirements document
    ├── SAST_DAST_Report.md     # SAST/DAST findings and fixes
    ├── Tareas.pdf              # Assignment requirements (13 security points)
    └── DevSecOps Centurion.pdf # Original security spec (superseded by Security_Requirements.md)
```

### Build & Run
```bash
# Linux (native)
bash build.sh             # Compiles to ./Cnturion
./Cnturion                # Run the app

# Cross-compile (from Linux)
./cross-build.sh          # Build both Linux + Windows
./cross-build.sh linux    # Linux only  → ./Cnturion
./cross-build.sh windows  # Windows only → ./Cnturion.exe
```

**Linux dependencies**: `gcc`, `ncurses` (`-lncurses`), `libargon2` (`-largon2`), `libm` (`-lm`)  
**Windows cross-compile**: `mingw-w64-gcc` (Arch: `sudo pacman -S mingw-w64-gcc`). Argon2 source is auto-downloaded and embedded; no external library needed on Windows.

---

## Role Permissions

| Feature | Admin | Employee |
|---------|:-----:|:--------:|
| View Products | ✅ | ✅ |
| Search Products | ✅ | ✅ |
| Sell Product (new) | ✅ | ✅ |
| Restock Product (new) | ✅ | ✅ |
| Add Product | ✅ | ❌ |
| Update Product | ✅ | ❌ |
| Delete Product | ✅ | ❌ |
| Manage Users | ✅ | ❌ |

---

## Workflow Chronology

### 2026-02-16 — Full codebase analysis
- Analyzed all source files, architecture, data flow
- Identified 10+ code quality / security issues (fflush(stdin) UB, missing NULL checks, scanf without validation, float comparison in search, etc.)

### 2026-02-16 — Initial compliance analysis vs Tareas.pdf
- Scored ~8% compliance (2/13 points partially met)
- Most requirements assume web/mobile; Cnturion is CLI

### 2026-02-16 — Revised compliance (CLI-adapted)
- Read `DevSecOps Centurion.pdf` — partially complete security spec
- CLI architecture approved; security concepts adapted to CLI context
- Created a 4-phase roadmap (see below)
- Revised score: ~15%

### 2026-02-16 — Implemented Section 2 (Secure Architecture)
- Created `src/security.c/h` — bundled SHA-256, password policy, input sanitization
- Created `src/auth.c/h` — login (max 3 attempts), user management, session, admin seeding
- Added `sellProduct()` and `restockProduct()` to `product_controller.c`
- Rewrote `main.c` — login flow, separate Admin/Employee menus, replaced `scanf` with `fgets`
- Updated `build.sh` and `build.bat`
- **All tests passed**: admin login, employee creation, restricted menus, hashed passwords

### 2026-02-18 — Implemented Section 3 (Secure Programming Practices)
- **3.1**: Applied `sanitizeString()` to all product inputs, fixed `fflush(stdin)` UB, added `validateUsername()`, created `getValidatedString()`
- **3.2**: Added per-user salt via `/dev/urandom`, `hashPasswordWithSalt()`, uppercase in password policy, auto-migration of old Users.csv
- **3.3**: Replaced `strcpy` → `strncpy` in update, CSV field count validation, double-quote sanitization
- **3.4**: Session timeout (5 min), `checkSessionTimeout()`, `updateSessionActivity()`, `logoutSession()`
- **3.5**: Created `logger.c/h` with audit log, added NULL checks to all `fopen`/`malloc`, error codes in file_controller
- Build hardened with `-O2 -Wall -Wextra -fstack-protector-strong -D_FORTIFY_SOURCE=2`

### 2026-02-18 — Upgraded to Argon2id & Closed Remaining Gaps
- Replaced SHA-256+salt with **Argon2id** (`libargon2`, t=3, m=64MB, p=1)
- Added **password masking** via `termios` (no echo during input)
- Added **max session duration** (1 hour hard limit)
- Added **range validation** (qty 1–999999, price 0.01–999999.99)
- Sanitized **search inputs** (name/category)
- Switched Users.csv to **pipe delimiter** (Argon2 hash contains commas)
- Added `-largon2` to `build.sh`

### 2026-02-18 — Added POSIX Regex Validation
- Added `matchesPattern()` (public POSIX regex helper) to `security.c`
- Rewrote `validateUsername()` to use regex: `^[a-zA-Z0-9_]{3,63}$`
- Added `validateProductName()`: `^[a-zA-Z0-9 _./-]{1,127}$`
- Added `validateProductCategory()`: `^[a-zA-Z0-9 _-]{1,63}$`
- Applied regex validation to `addProduct()` and `updateProduct()` (reject-and-retry / reject-and-keep)

### 2026-02-19 — Cross-Platform Compilation (Linux → Windows)
- Created `cross-build.sh` — builds for Linux (native) and Windows (cross-compile via `mingw-w64`)
- Argon2 source is auto-cloned into `deps/` and **embedded** directly (no library install needed on Windows)
- Windows `.exe` uses **`-static`** linking for a fully self-contained portable binary
- **`auth.c`**: Added `#ifdef _WIN32` for password input — `conio.h` + `_getch()` on Windows, `termios.h` on Linux
- **`security.c`**: Replaced POSIX `regex.h` with manual C string validation on Windows; replaced `/dev/urandom` with `BCryptGenRandom`
- **`logger.c`**: Fixed `mkdir()` call — Windows takes 1 argument (no permissions), POSIX takes 2
- Updated `build.bat` to include `-largon2 -lbcrypt`

### 2026-02-26 — SAST/DAST Security Testing & Bug Fixes
- Ran **cppcheck** 2.19.1 (SAST) — found 5 errors, 30+ warnings
- Ran **flawfinder** 2.0.19 (SAST) — 199 hits across 1906 SLOC with CWE mappings
- Built with **AddressSanitizer** (DAST) — no runtime memory errors detected
- Ran **arch-audit** (dependency scan) — 0 CVEs for argon2, ncurses, glibc
- **Fixed**: 5 realloc memory leaks in `searchProduct()` (CWE-401) — used temp pointer pattern
- **Fixed**: 2 null pointer dereferences after unchecked `malloc()` (CWE-476)
- **Fixed**: 26 format specifier mismatches `%d` → `%u` for `unsigned int` (CWE-686) across `product_controller.c`, `auth.c`, `file_controller.c`
- Post-fix cppcheck re-scan: **zero errors, zero warnings**
- Full report saved to `extra/SAST_DAST_Report.md`

### 2026-03-07 — Pre-Presentation Compliance Hardening
- Created `.gitignore` — excludes `data/Users.csv`, `logs/`, binaries, `deps/`
- Removed sensitive files from git tracking (`git rm --cached`)
- Created `pre-presentation-backup` branch on GitHub with full snapshot
- Created `deploy.sh` (Linux: chmod 600/700) and `deploy.bat` (Windows: icacls)
- Created `extra/Security_Requirements.md` — threat model + all 13 security points documented
- Updated `config.ini` — documented data file paths
- Removed plaintext passwords from `CONTEXT.md` credential table
- Updated compliance: #1 → 80%, #10 → 85%, #13 → 70%; overall → ~90%

---

## Compliance with Tareas.pdf (13 Security Points)

| # | Requirement | Status | Notes |
|---|------------|--------|-------|
| 1 | Security requirements defined | ✅ 80% | `extra/Security_Requirements.md` — threat model, all 13 points documented |
| 2 | **Secure architecture design** | ✅ 80% | **Done**: role separation, minimum privilege, defense in depth |
| 3 | Input validation (backend) | ✅ 100% | **Done**: POSIX regex for all text inputs, sanitizeString, range checks |
| 4 | Password hashing | ✅ 100% | **Done**: Argon2id (OWASP-recommended), self-contained encoded hashes |
| 5 | Authentication & authorization | ✅ 95% | Login + roles + username validation + password policy + masking |
| 6 | Session/token management | ✅ 95% | **Done**: 5-min inactivity + 1-hour max duration, secure logout |
| 7 | Error handling | ✅ 90% | **Done**: NULL checks, error codes, generic messages, logging |
| 8 | Logging & auditing | ✅ 80% | **Done**: logger.c/h, audit.log with timestamps |
| 9 | Dependencies scanned | ✅ 90% | `arch-audit` run — 0 CVEs for argon2, ncurses, glibc |
| 10 | Secrets out of code | ✅ 85% | `.gitignore` excludes Users.csv/logs; paths in `config.ini`; no secrets in source |
| 11 | HTTPS everywhere | ⬜ N/A | CLI app, no network |
| 12 | Security testing (SAST/DAST) | ✅ 80% | cppcheck + flawfinder (SAST), ASan (DAST), all bugs fixed |
| 13 | Hardened deployment | ✅ 70% | Compiler flags + `deploy.sh`/`deploy.bat` with file permissions |

**Estimated overall compliance: ~90%** (up from ~82%)

---

## Next Steps (Phased Roadmap)

### Phase 1 — Critical Fixes ✅ COMPLETE
- [x] Add `fopen()` / `malloc()` / `realloc()` NULL checks
- [x] Apply `sanitizeString()` + regex validation to all product inputs
- [x] Implement `logger.c/h` → `logs/audit.log`
- [x] Build with hardened compiler flags (`-O2 -Wall -Wextra -fstack-protector-strong -D_FORTIFY_SOURCE=2`)
- [x] Run `cppcheck` and `flawfinder` for SAST
- [x] Run AddressSanitizer for DAST
- [x] Run `arch-audit` for dependency scanning
- [x] Fix all critical/warning-level findings

### Phase 2 — SQLite Migration
- [ ] Add SQLite3, create `db_manager.c/h`
- [ ] Migrate product storage CSV → SQLite with parameterized queries
- [ ] Migrate user storage CSV → SQLite

### Phase 3 — Security Tooling & Encryption
- [ ] Encrypt SQLite at rest (SQLCipher or file-level)
- [ ] Move config to `.env`, add to `.gitignore`
- [ ] Write manual security test plan
- [ ] Finalize `DevSecOps Centurion.pdf`

### Phase 4 — CI/CD & Deployment
- [ ] GitHub Actions CI pipeline with SAST + build
- [ ] File permissions (`chmod 600` for DB/logs)
- [ ] Deployment hardening checklist

---

## Known Issues / Tech Debt
- `fgets()` return values unchecked (`-Wunused-result` warnings with `-O2`) — harmless for interactive CLI
- Sign comparison warnings (`int` vs `unsigned int` loop counters) — cosmetic only
- `clearStdin()` defined but unused in `input_validation.c`
- flawfinder level-4 `printf` CWE-134 findings are false positives (ANSI color macros, not user input)
- flawfinder level-3 `srand()` CWE-327 applies only to Windows fallback (Linux uses `/dev/urandom`)

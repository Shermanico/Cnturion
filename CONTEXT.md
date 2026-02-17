# Cnturion — Project Context & Workflow Log

> **Last updated**: 2026-02-16  
> **Authors**: Jorge Nicolás Jiménez Moreno (Nico) & Penélope Ximena Sánchez Silva (Chobiol) 
> **Course**: DevSecOps — Universidad Politécnica de Yucatán  
> **Supervisor**: Angel Arturo Pech Che  
> **Repository**: `Shermanico/Cnturion`

---

## What Is Cnturion?

A **CLI-based Inventory Management System** written in **C**. It manages a product catalog (~1000 records) stored in CSV, with colored terminal UI via ANSI escape codes. As of 2026-02-16, it includes **authentication with role-based access control**.

---

## Current Credentials

| Username | Password | Role |
|----------|----------|------|
| `admin` | `Admin123!` | Admin (full access) |
| `employee1` | `Emp12345!` | Employee (restricted) |

> Passwords are stored as **SHA-256 hashes** in `data/Users.csv`. The default admin is auto-created on first run if `Users.csv` doesn't exist.

---

## Project Structure

```
Cnturion/
├── main.c                      # Entry point: login → role-based menus
├── src/
│   ├── auth.c / auth.h         # Login, user CRUD, session management
│   ├── security.c / security.h # SHA-256 hashing, password policy, sanitization
│   ├── product_controller.c/h  # CRUD + sell + restock logic
│   ├── file_controller.c/h     # CSV/INI read/write
│   └── input_validation.c/h    # Numeric input validation
├── model/
│   └── product.h               # Product struct (id, name, category, qty, price)
├── utilities/
│   ├── clear.c / clear.h       # Cross-platform screen clear (ncurses/WinAPI)
│   └── color.h                 # ANSI color macros
├── data/
│   ├── Inventory.csv           # Product data (~1003 records)
│   ├── Inventory.txt           # Legacy (unused)
│   └── Users.csv               # User accounts (auto-generated)
├── config.ini                  # ViewLimit = 10 (pagination)
├── build.sh / build.bat        # Build scripts (Linux / Windows)
├── Tareas.pdf                  # Assignment requirements (13 security points)
├── DevSecOps Centurion.pdf     # Security requirements document (WIP)
└── ansi_color/color.h          # Duplicate of utilities/color.h (unused)
```

### Build & Run
```bash
bash build.sh    # Compiles to ./output
./output         # Run the app (login prompt appears)
```

**Dependencies**: `gcc`, `ncurses` (`-lncurses`)

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

---

## Compliance with Tareas.pdf (13 Security Points)

| # | Requirement | Status | Notes |
|---|------------|--------|-------|
| 1 | Security requirements defined | 🟡 40% | `DevSecOps Centurion.pdf` exists but incomplete |
| 2 | **Secure architecture design** | ✅ 80% | **Done**: role separation, minimum privilege, defense in depth |
| 3 | Input validation (backend) | 🟡 30% | Numeric validation exists; string sanitization added but not yet applied to all inputs |
| 4 | Password hashing | ✅ 90% | SHA-256 implemented; upgrade to Argon2/bcrypt recommended later |
| 5 | Authentication & authorization | ✅ 85% | Login + Admin/Employee roles enforced at menu level |
| 6 | Session/token management | 🟡 50% | In-memory session struct; no inactivity timeout yet |
| 7 | Error handling | 🟡 25% | User-friendly input errors exist; `fopen`/`malloc` NULL checks still missing |
| 8 | Logging & auditing | ❌ 0% | Not implemented |
| 9 | Dependencies scanned | ❌ 0% | No SAST/scanning |
| 10 | Secrets out of code | 🟡 50% | No secrets in code; file paths hardcoded |
| 11 | HTTPS everywhere | ⬜ N/A | CLI app, no network |
| 12 | Security testing (SAST/DAST) | ❌ 0% | No testing |
| 13 | Hardened deployment | ❌ 0% | No CI/CD or hardening |

**Estimated overall compliance: ~30%** (up from ~8% at start)

---

## Next Steps (Phased Roadmap)

### Phase 1 — Critical Fixes (no architecture change)
- [ ] Add `fopen()` / `malloc()` / `realloc()` NULL checks in `file_controller.c`, `product_controller.c`, `main.c`
- [ ] Apply `sanitizeString()` to product name/category on add/update
- [ ] Implement `logger.c/h` → `logs/audit.log` (login, CRUD actions, timestamps)
- [ ] Add `Makefile` with hardened compiler flags (`-Wall -Wextra -fstack-protector-strong`)
- [ ] Run `cppcheck` or `flawfinder` for SAST

### Phase 2 — SQLite Migration
- [ ] Add SQLite3, create `db_manager.c/h`
- [ ] Migrate product storage CSV → SQLite with parameterized queries
- [ ] Migrate user storage CSV → SQLite
- [ ] Add session inactivity timeout

### Phase 3 — Security Tooling & Encryption
- [ ] Encrypt SQLite at rest (SQLCipher or file-level)
- [ ] Move config to `.env`, add to `.gitignore`
- [ ] Write manual security test plan
- [ ] Finalize `DevSecOps Centurion.pdf`

### Phase 4 — CI/CD & Deployment
- [ ] GitHub Actions CI pipeline with SAST + build
- [ ] File permissions (`chmod 600` for DB/logs)
- [ ] Compiler security flags: PIE, FORTIFY_SOURCE
- [ ] Deployment hardening checklist

---

## Known Issues / Tech Debt
- `fflush(stdin)` used in `input_validation.c` — undefined behavior on POSIX
- `ansi_color/color.h` is an unused duplicate of `utilities/color.h`
- `Inventory.txt` is legacy and unused
- `cleanup()` in `main.c` defined but never called
- Float comparison in `searchProduct()` unreliable (floating-point precision)
- Delete function shifts array but doesn't `realloc` to free memory
- Inner `while(1)` loop in `searchMenu()` breaks on first iteration (dead code pattern)

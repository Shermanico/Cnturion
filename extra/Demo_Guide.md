# Cnturion — Demo Guide (Presentation Script)

This guide walks through demonstrating **all Cnturion features** starting from a fresh clone. Follow these steps during the Wednesday presentation.

---

## Preparation (Before Presenting)

### On Linux
```bash
git clone https://github.com/Shermanico/Cnturion.git
cd Cnturion
bash build.sh          # Compile → ./Cnturion
bash deploy.sh         # Set secure file permissions
```

### On Windows
```cmd
git clone https://github.com/Shermanico/Cnturion.git
cd Cnturion
build.bat              &REM Compile → Cnturion.exe
deploy.bat             &REM Set secure NTFS permissions
```

> After building, the app is **immediately functional**. The admin account is auto-created on first run.

---

## Demo Script (Step by Step)

### 1. First Run — Auto Admin Seeding

```bash
./Cnturion   # or Cnturion.exe on Windows
```

**Expected output:**
```
Upgrading user database to Argon2id hashing...
Default admin account created (username: admin)
```

**Talking point:** _"The system auto-creates a default admin account on first run using Argon2id hashing — no plaintext passwords are stored."_

### 2. Login as Admin

```
Username: admin
Password: Admin123!
```

**Expected:** `Login successful! Welcome, admin (Admin)`

**Talking point:** _"Passwords are masked during input (no echo). Max 3 attempts before lockout."_

### 3. View Inventory (Option 1)

Shows paginated list of ~1,000 products. Navigate pages or press Enter to skip.

**Talking point:** _"Pagination is configurable via `config.ini`. Data is stored in CSV with validated inputs."_

### 4. Add a Product (Option 3)

```
Product Name: Test Product
Product Category: Electronics
Product Quantity: 100
Product Price: 29.99
```

**Talking point:** _"All inputs are validated with POSIX regex. Try entering special characters — it will reject them."_

#### Demo: Input Rejection

```
Product Name: <script>alert('xss')</script>
→ "Invalid name! Use letters, numbers, spaces, hyphens, dots."
```

**Talking point:** _"Input sanitization prevents injection and strips control characters."_

### 5. Search (Option 2)

Search by Name → enter the product you just added.

**Talking point:** _"Search works across all fields: ID, name, category, quantity, price."_

### 6. Sell a Product (Option 5)

Select a product ID → enter quantity to sell.

**Expected:** Shows total price, remaining stock.

### 7. Restock a Product (Option 6)

Select a product ID → enter quantity to add.

### 8. Update a Product (Option 4)

Select a product ID → press Enter to skip fields you don't want to change.

**Talking point:** _"Pressing Enter keeps the original value — only changed fields are updated."_

### 9. Delete a Product (Option 7)

Select the test product's ID.

**Talking point:** _"Memory is properly freed with `realloc` after deletion — verified by AddressSanitizer."_

### 10. User Management (Option 8)

#### Create a New Employee

```
Username: demo_employee
Password: DemoPass1!
Confirm:  DemoPass1!
Role: 1 (Employee)
```

#### Demo: Weak Password Rejection

```
Password: 123
→ "Password does not meet requirements!"
```

**Talking point:** _"Password policy enforces min 8 chars, 1 uppercase, 1 digit, 1 special character."_

#### List Users (Sub-option 2)

Shows all registered users with roles.

### 11. Logout (Option 0) → Login as Employee

```
Username: demo_employee
Password: DemoPass1!
```

**Expected:** Employee menu appears (no Add/Update/Delete/User Management options).

**Talking point:** _"Role-based access control — employees can only view, search, sell, and restock."_

### 12. Session Timeout Demo (Optional)

Wait 5+ minutes without interacting, then try any action.

**Expected:** `Session Expired — Your session has been inactive for more than 5 minutes.`

**Talking point:** _"5-minute inactivity timeout + 1-hour max session duration."_

### 13. Audit Log

After the demo, show the audit log:
```bash
cat logs/audit.log
```

**Expected:** Shows timestamped entries for every login, logout, CRUD operation, sell, restock.

**Talking point:** _"All security-relevant events are logged with timestamps and usernames."_

---

## Security Features Summary (Slide Points)

| Feature | Implementation | Proof |
|---|---|---|
| Password hashing | Argon2id (t=3, m=64MB) | `data/Users.csv` — no plaintext |
| Input validation | POSIX regex + sanitization | Reject invalid names, SQL/XSS attempts |
| RBAC | Admin/Employee separation | Employee menu lacks CRUD options |
| Session management | 5-min timeout + 1-hour max | `checkSessionTimeout()` in auth.c |
| Audit logging | `logs/audit.log` | Show log after demo |
| Memory safety | SAST (cppcheck/flawfinder) + DAST (ASan) | Reports in `extra/` |
| Dependency scanning | `arch-audit` — 0 CVEs | `extra/dep_arch_audit.txt` |
| Deployment hardening | `deploy.sh`/`deploy.bat` + compiler flags | chmod 600 on data files |
| Git security | `.gitignore` for secrets | No Users.csv in repo |

---

## If Something Goes Wrong

| Problem | Fix |
|---|---|
| `libargon2 not found` | `sudo pacman -S argon2` (Arch) or `sudo apt install libargon2-dev` (Debian) |
| `ncurses not found` | `sudo pacman -S ncurses` or `sudo apt install libncurses-dev` |
| No admin account | Delete `data/Users.csv` and restart — auto-seeds new admin |
| Forgot admin password | Same as above — delete Users.csv |
| Permission denied on data/ | Run `bash deploy.sh` |
| Windows: build fails | Need `mingw-w64-gcc` or compile natively with `build.bat` |

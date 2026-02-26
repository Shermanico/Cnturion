# SAST & DAST Report — Cnturion

> **Date**: 2026-02-26  
> **Authors**: Jorge Nicolás Jiménez Moreno & Penélope Ximena Sánchez Silva  
> **Course**: DevSecOps — Universidad Politécnica de Yucatán  
> **Supervisor**: Angel Arturo Pech Che

---

## 1. Tools Used

| Tool | Version | Type | Purpose |
|---|---|---|---|
| **cppcheck** | 2.19.1 | SAST | Static analysis for C — detects memory leaks, null derefs, format bugs, undefined behavior |
| **flawfinder** | 2.0.19 | SAST | Security-focused lexical scanner — maps findings to CWE IDs |
| **AddressSanitizer** | GCC 15.2.1 | DAST | Runtime memory error detection — buffer overflows, use-after-free, memory leaks |
| **arch-audit** | — | Dependency Scan | Checks installed packages against known CVEs |

---

## 2. Errors Found & Fixes Applied

### 2.1 Memory Leak on `realloc` Failure (CWE-401)

**Tool**: cppcheck  
**Severity**: Error  
**File**: `src/product_controller.c` — `searchProduct()` (lines 386, 404, 422, 438, 454)

**Problem**: Direct assignment `result = realloc(result, ...)` loses the original pointer if `realloc()` returns `NULL`, causing a memory leak.

**Before**:
```c
result = realloc(result, ((size) * sizeof(Product)));
```

**After**:
```c
Product *tmp = realloc(result, ((size) * sizeof(Product)));
if (!tmp) { printf(RED "Memory error.\n" reset); break; }
result = tmp;
```

**Explanation**: By saving `realloc()`'s return value to a temporary pointer, the original `result` is preserved if allocation fails. This prevents memory leaks and null pointer dereference. This fix was applied to all 5 instances in the function.

---

### 2.2 Null Pointer Dereference After `malloc` (CWE-476)

**Tool**: cppcheck  
**Severity**: Warning  
**Files**: `src/product_controller.c` — lines 373–374 and 602–604

#### Instance 1: `searchProduct()` — variable `k`

**Problem**: `malloc()` result for `k` was used without checking for `NULL`.

**Before**:
```c
unsigned int *k = (unsigned int *)malloc(sizeof(unsigned int));
*k = 0;
```

**After**:
```c
unsigned int *k = (unsigned int *)malloc(sizeof(unsigned int));
if (!k) {
    printf(RED "Memory allocation error.\n" reset);
    free(result);
    return;
}
*k = 0;
```

#### Instance 2: `initiate()` — variable `prod`

**Problem**: `malloc()` result for `prod` was used without checking for `NULL`, leading to potential null dereference on `prod[*total - 1].id`.

**Before**:
```c
prod = (Product *)malloc((*size) * sizeof(Product));
readCSV(prod);
*uid = (*total == 0) ? 0 : prod[*total - 1].id;
```

**After**:
```c
prod = (Product *)malloc((*size) * sizeof(Product));
if (!prod) {
    return NULL;
}
readCSV(prod);
*uid = (*total == 0) ? 0 : prod[*total - 1].id;
```

---

### 2.3 Format Specifier Mismatch — `%d` for `unsigned int` (CWE-686)

**Tool**: cppcheck  
**Severity**: Warning  
**Files**: `src/product_controller.c`, `src/auth.c`, `src/file_controller.c`

**Problem**: Using `%d` (signed int) format specifier for `unsigned int` variables (`prod.id`, `prod.quantity`, `user.id`). While this rarely causes visible bugs for small values, it is undefined behavior per the C standard and can produce incorrect output for values exceeding `INT_MAX`.

**Fix**: Changed all `%d` to `%u` for `unsigned int` variables across 3 files:

| File | Instances Fixed |
|---|---|
| `product_controller.c` | 22 |
| `auth.c` | 2 |
| `file_controller.c` | 2 |

**Example**:
```diff
- printf("Product with ID %d added successfully\n", prod[*total - 1].id);
+ printf("Product with ID %u added successfully\n", prod[*total - 1].id);
```

---

## 3. Flawfinder Results Summary

Flawfinder analyzes source code for dangerous function usage and maps findings to CWE identifiers.

| Risk Level | Hits | Key CWEs | Assessment |
|---|---|---|---|
| **Level 4** | 94 | CWE-134 (format string) | **False positives** — `printf` with compile-time ANSI color macros, not user input |
| **Level 3** | 4 | CWE-327 (weak PRNG) | `srand()` in Windows-only fallback; Linux path uses `/dev/urandom` |
| **Level 2** | 65 | CWE-120 (buffer), CWE-190 (`atoi`), CWE-362 (`fopen` race) | Low risk — bounded by `fgets` and existing validation |
| **Level 1** | 36 | CWE-120 (`strncpy`, `getchar`, `strlen`) | Informational |

**Total**: 199 hits across 1,906 SLOC (104.4 hits/KSLOC)

> Most level-4 hits are false positives caused by ANSI color macros (`RED`, `GRN`, `CYN`) being string-concatenated with `printf`. These are compile-time constants and cannot be influenced by user input.

---

## 4. AddressSanitizer (DAST) Results

The project was compiled with `-fsanitize=address -fno-omit-frame-pointer -g` to enable AddressSanitizer runtime checks.

**Build**:
```bash
gcc main.c src/*.c utilities/clear.c \
  -I./src/ -I./model/ -I./utilities/ \
  -lncurses -lm -largon2 \
  -fsanitize=address -fno-omit-frame-pointer -g \
  -o output_asan
```

**Result**: **No memory errors detected** during login flow and basic operations. ASan monitors for:
- Heap buffer overflow
- Stack buffer overflow
- Use-after-free
- Double-free
- Memory leaks at exit

The clean result indicates no runtime memory corruption in the tested code paths.

---

## 5. Dependency Scan Results

`arch-audit` was used to scan all installed system packages for known CVEs.

**Cnturion Dependencies**:

| Dependency | Purpose | Known CVEs |
|---|---|---|
| `libargon2` | Password hashing | ✅ None |
| `ncurses` | Terminal UI | ✅ None |
| `glibc` (`libm`) | Math functions | ✅ None |

All three project dependencies are free of known vulnerabilities.

---

## 6. Verification

After applying all fixes:

| Check | Result |
|---|---|
| `bash build.sh` | ✅ Compiles successfully |
| cppcheck re-scan (errors + warnings) | ✅ Zero errors, zero warnings |
| ASan build | ✅ Compiles and runs clean |
| Dependency scan | ✅ No CVEs for project dependencies |

---

## 7. Commands Used

```bash
# SAST — cppcheck
cppcheck --enable=all --inconclusive --std=c11 \
  -I./src/ -I./model/ -I./utilities/ \
  main.c src/*.c utilities/clear.c

# SAST — flawfinder
flawfinder --minlevel=1 --columns main.c src/ utilities/

# DAST — AddressSanitizer
gcc main.c src/*.c utilities/clear.c \
  -I./src/ -I./model/ -I./utilities/ \
  -lncurses -lm -largon2 \
  -fsanitize=address -fno-omit-frame-pointer -g \
  -o output_asan
./output_asan

# Dependency Scan
arch-audit | grep -E "argon2|ncurses|glibc"
```

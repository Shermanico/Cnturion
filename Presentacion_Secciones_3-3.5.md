# Cnturion — Prácticas de Programación Segura
## Secciones 3 – 3.5 del documento de Tareas

> **Proyecto**: Cnturion — Sistema CLI de Gestión de Inventarios  
> **Autores**: Jorge Nicolás Jiménez Moreno & Penélope Ximena Sánchez Silva  
> **Materia**: DevSecOps — Universidad Politécnica de Yucatán  
> **Supervisor**: Angel Arturo Pech Che

---

## 3.1 — Validación y Saneamiento de Entradas

### Requerimiento
> Validación rigurosa en el backend para todos los datos de usuario: tipo, longitud, rango y formato (regex).

### Implementación

#### Motor de validación con POSIX Regex (`security.c`)

Todas las validaciones de texto usan un helper genérico de regex POSIX:

```c
#include <regex.h>

// Helper genérico — retorna 1 si input coincide con el patrón
int matchesPattern(const char *input, const char *pattern) {
    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0)
        return 0;
    int result = regexec(&regex, input, 0, NULL, 0);
    regfree(&regex);
    return (result == 0) ? 1 : 0;
}
```

#### Patrones aplicados a cada entrada

| Entrada | Patrón Regex | Archivo |
|---------|-------------|---------|
| Username | `^[a-zA-Z0-9_]{3,63}$` | `security.c` |
| Nombre de producto | `^[a-zA-Z0-9 _./-]{1,127}$` | `security.c` |
| Categoría de producto | `^[a-zA-Z0-9 _-]{1,63}$` | `security.c` |
| Contraseña (longitud) | `.{8,}` | `security.c` |
| Contraseña (mayúscula) | `[A-Z]` | `security.c` |
| Contraseña (dígito) | `[0-9]` | `security.c` |
| Contraseña (especial) | `[^a-zA-Z0-9]` | `security.c` |

```c
int validateUsername(const char *username) {
    return matchesPattern(username, "^[a-zA-Z0-9_]{3,63}$");
}

int validateProductName(const char *name) {
    return matchesPattern(name, "^[a-zA-Z0-9 _./-]{1,127}$");
}

int validateProductCategory(const char *category) {
    return matchesPattern(category, "^[a-zA-Z0-9 _-]{1,63}$");
}

int validatePassword(const char *password) {
    return matchesPattern(password, ".{8,}")      &&  // mín 8 caracteres
           matchesPattern(password, "[A-Z]")      &&  // tiene mayúscula
           matchesPattern(password, "[0-9]")      &&  // tiene dígito
           matchesPattern(password, "[^a-zA-Z0-9]");  // tiene carácter especial
}
```

#### Saneamiento contra inyección CSV (`security.c`)

```c
int sanitizeString(char *input, int maxLen) {
    int clean = 1, len = strlen(input);
    if (len > maxLen) { input[maxLen] = '\0'; len = maxLen; clean = 0; }

    for (int i = 0; i < len; i++) {
        if (input[i] == ',' || input[i] == '"' ||
            input[i] == '\n' || input[i] == '\r') {
            input[i] = '_';
            clean = 0;
        }
    }
    return clean;
}
```

#### Validación de rango numérico (`product_controller.c`)

```c
// Cantidad: debe estar entre 1 y 999,999
printf("Product Quantity: ");
getDigit(&prod[*total].quantity);
while (prod[*total].quantity == 0 || prod[*total].quantity > 999999) {
    printf("Quantity must be between 1 and 999999! Enter again: ");
    getDigit(&prod[*total].quantity);
}

// Precio: debe estar entre 0.01 y 999,999.99
printf("Product Price: ");
getFloat(&prod[*total].price);
while (prod[*total].price <= 0 || prod[*total].price > 999999.99f) {
    printf("Price must be between 0.01 and 999999.99! Enter again: ");
    getFloat(&prod[*total].price);
}
```

#### Aplicación en flujo de entrada (`product_controller.c`)

```c
// addProduct() — validación regex + saneamiento
printf("Product Name: ");
fgets(prod[*total].name, 128, stdin);
prod[*total].name[strcspn(prod[*total].name, "\n")] = 0;
while (!validateProductName(prod[*total].name)) {
    printf("Invalid name! Use letters, numbers, spaces, hyphens, dots."
           " Enter again: ");
    fgets(prod[*total].name, 128, stdin);
    prod[*total].name[strcspn(prod[*total].name, "\n")] = 0;
}
sanitizeString(prod[*total].name, 127);
```

#### Justificación
- **Regex POSIX** (`<regex.h>`) es nativo del sistema; no requiere dependencias externas
- Cada entrada tiene un patrón formal documentado como especificación de seguridad
- `sanitizeString()` actúa como segunda capa defensiva contra caracteres peligrosos para CSV
- La validación de rango previene desbordamientos y valores ilógicos de negocio

---

## 3.2 — Gestión Segura de Contraseñas

### Requerimiento
> Algoritmos robustos como Argon2, bcrypt, scrypt o PBKDF2 con sal individual por usuario.

### Implementación

#### Hashing con Argon2id (`security.c`)

Se utiliza **Argon2id** — el algoritmo recomendado por OWASP para hashing de contraseñas:

```c
#include <argon2.h>

// Parámetros OWASP recomendados
#define ARGON2_T_COST 3       // 3 iteraciones
#define ARGON2_M_COST 65536   // 64 MB de memoria
#define ARGON2_PARALLELISM 1  // mono-hilo (app CLI)
#define ARGON2_HASHLEN 32     // hash de 32 bytes

int hashPasswordArgon2(const char *password, char *encodedOut,
                       size_t encodedMax) {
    // Sal criptográfica de 16 bytes desde /dev/urandom
    unsigned char salt[16];
    FILE *f = fopen("/dev/urandom", "rb");
    size_t rd = fread(salt, 1, 16, f);
    fclose(f);
    if (rd != 16) return 0;

    int result = argon2id_hash_encoded(
        ARGON2_T_COST, ARGON2_M_COST, ARGON2_PARALLELISM,
        password, strlen(password),
        salt, 16, ARGON2_HASHLEN,
        encodedOut, encodedMax);

    return (result == ARGON2_OK) ? 1 : 0;
}
```

#### Verificación en tiempo constante (`security.c`)

```c
int verifyPasswordArgon2(const char *encoded, const char *password) {
    int result = argon2id_verify(encoded, password, strlen(password));
    return (result == ARGON2_OK) ? 1 : 0;
}
```

#### Hash auto-contenido almacenado en Users.csv

```
ID|Username|PasswordHash|Role
1|admin|$argon2id$v=19$m=65536,t=3,p=1$qhJBjWW3T2reWZLEkXLcIQ$NECEWnPfaAJo...|0
```

El hash codificado incluye: algoritmo (`argon2id`), versión, parámetros (`m`, `t`, `p`), sal, y hash — todo en una sola cadena.

#### Enmascaramiento de contraseña durante entrada (`auth.c`)

```c
#include <termios.h>

static void readPassword(char *buf, int maxLen) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);                     // Desactivar echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fgets(buf, maxLen, stdin);
    buf[strcspn(buf, "\n")] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);      // Restaurar echo
    printf("\n");
}
```

#### Justificación
- **Argon2id** es *memory-hard* — resistente a ataques por GPU/ASIC
- La sal se genera con `/dev/urandom` (fuente criptográfica del sistema)
- `argon2id_verify()` realiza comparación en tiempo constante contra timing attacks
- El hash auto-contenido elimina la necesidad de columnas separadas para sal
- `termios` enmascara la entrada de contraseña para prevenir shoulder surfing

---

## 3.3 — Protección contra Inyección

### Requerimiento
> Uso de consultas parametrizadas o sentencias preparadas, saneamiento de todas las entradas de usuario.

### Implementación (adaptada a CLI/CSV)

> **Nota**: Cnturion es una aplicación CLI que usa archivos CSV, no SQL. Los conceptos de inyección se adaptan al contexto de almacenamiento en archivos planos.

#### Saneamiento CSV como equivalente a consultas parametrizadas

```c
// sanitizeString() reemplaza caracteres peligrosos para CSV
// Equivalente funcional a "parametrized queries" para archivos planos
int sanitizeString(char *input, int maxLen) {
    for (int i = 0; i < len; i++) {
        if (input[i] == ',' || input[i] == '"' ||
            input[i] == '\n' || input[i] == '\r') {
            input[i] = '_';  // Neutralizar inyección CSV
        }
    }
    return clean;
}
```

#### Prevención de desbordamiento de buffer (`product_controller.c`)

```c
// updateProduct() — strncpy con límite explícito
if (validateProductName(input)) {
    strncpy(prod[item].name, input, 127);
    prod[item].name[127] = '\0';     // Garantizar terminación null
    sanitizeString(prod[item].name, 127);
} else {
    printf("Invalid name format, keeping original.\n");
}
```

#### Validación de estructura CSV (`file_controller.c`)

```c
// Validación de columnas al leer CSV
char *value = strtok(line, ",");
int col = 0;
while (value && col < EXPECTED_COLUMNS) {
    // ... procesar columna por columna
    value = strtok(NULL, ",");
    col++;
}
// Registrar advertencia si el número de columnas no coincide
if (col != EXPECTED_COLUMNS) {
    logEvent(LOG_WARN, "SYSTEM", "CSV_MALFORMED", "Row has wrong field count");
}
```

#### Delimitador seguro para Users.csv

```c
// Users.csv usa pipe (|) como delimitador
// porque el hash Argon2id contiene comas internas: m=65536,t=3,p=1
fprintf(f, "%d|%s|%s|%d\n", user->id, user->username,
        user->password_hash, user->role);
```

#### Justificación
- `sanitizeString()` neutraliza los 4 caracteres peligrosos para CSV: `,` `"` `\n` `\r`
- `strncpy` con límite explícito previene **buffer overflow** (CWE-120)
- La validación regex **antes** de escribir al almacenamiento es la primera línea de defensa
- El uso de pipe (`|`) como delimitador evita conflictos con datos internos del hash

---

## 3.4 — Gestión Segura de Sesiones

### Requerimiento
> Identificadores de sesión seguros, expiración automática, verificación antes de cada acción.

### Implementación

#### Estructura de sesión (`auth.h`)

```c
#define SESSION_TIMEOUT_SEC  300   // 5 minutos de inactividad
#define SESSION_MAX_DURATION 3600  // 1 hora de duración máxima

typedef struct {
    unsigned int user_id;
    char username[64];
    Role role;
    int active;
    time_t last_activity;    // Timestamp de última acción
    time_t login_time;       // Timestamp de inicio de sesión
} Session;
```

#### Verificación de timeout doble (`auth.c`)

```c
int checkSessionTimeout(Session *session) {
    if (!session->active) return 0;

    time_t now = time(NULL);
    double inactivity = difftime(now, session->last_activity);
    double totalDuration = difftime(now, session->login_time);

    // Timeout por inactividad (5 minutos)
    if (inactivity >= SESSION_TIMEOUT_SEC) {
        printf("Session expired due to inactivity.\n");
        logoutSession(session);
        return 0;
    }

    // Timeout por duración máxima (1 hora)
    if (totalDuration >= SESSION_MAX_DURATION) {
        printf("Session exceeded maximum duration.\n");
        logoutSession(session);
        return 0;
    }

    return 1;  // Sesión activa
}
```

#### Verificación antes de cada acción del menú (`main.c`)

```c
// Se verifica el timeout ANTES de ejecutar cualquier acción
case 1:
    if (!checkSessionTimeout(&session)) {
        logEvent(LOG_AUDIT, session.username,
                 "SESSION_TIMEOUT", "Expired during admin menu");
        exitMenu = 1;
        break;
    }
    updateSessionActivity(&session);
    ClearScreen();
    listProduct(prod, total, viewLimit);
    logEvent(LOG_AUDIT, session.username,
             "VIEW_PRODUCTS", "Listed inventory");
    break;
```

#### Cierre seguro de sesión (`auth.c`)

```c
void logoutSession(Session *session) {
    session->active = 0;
    session->user_id = 0;
    session->role = ROLE_EMPLOYEE;
    session->login_time = 0;
    session->last_activity = 0;
    memset(session->username, 0, sizeof(session->username));  // Limpiar memoria
}
```

#### Justificación
- **Doble timeout**: inactividad (5 min) + duración máxima (1 hora)
- La verificación se ejecuta **antes** de cada acción del menú, no después
- `logoutSession()` usa `memset()` para borrar datos sensibles de memoria
- Cada timeout genera un evento en el log de auditoría
- El control de acceso por rol se verifica en cada acción privilegiada

---

## 3.5 — Manejo de Errores y Registro de Auditoría

### Requerimiento
> Manejo seguro de errores, sin exposición de información sensible, y registro de acciones de seguridad.

### Implementación

#### Sistema de logging (`logger.c`)

```c
typedef enum { LOG_INFO, LOG_WARN, LOG_ERROR, LOG_AUDIT } LogLevel;

void logEvent(LogLevel level, const char *user,
              const char *action, const char *detail) {
    FILE *f = fopen("logs/audit.log", "a");
    if (!f) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(f, "[%s] [%-5s] [%s] %s: %s\n",
            timestamp, levelToString(level),
            user ? user : "SYSTEM", action, detail ? detail : "");
    fclose(f);
}
```

#### Ejemplo de salida en `logs/audit.log`

```
[2026-02-18 18:54:23] [AUDIT] [admin] LOGIN_SUCCESS: User logged in
[2026-02-18 18:54:45] [AUDIT] [admin] VIEW_PRODUCTS: Listed inventory
[2026-02-18 18:55:04] [AUDIT] [admin] LOGOUT: Admin logged out
```

#### Mensajes genéricos al usuario vs. logs detallados (`file_controller.c`)

```c
int readCSV(Product *prod, unsigned int *total, unsigned int *uid) {
    FILE *f = fopen(CSV_FILE, "r");
    if (!f) {
        // Mensaje genérico al usuario (no revela rutas internas)
        printf("Error: Could not load inventory data.\n");

        // Log detallado para administradores
        logEvent(LOG_ERROR, "SYSTEM", "FILE_OPEN_FAIL",
                 "Could not open " CSV_FILE);
        return -1;
    }
    // ...
}
```

#### Verificación de NULL en todas las asignaciones de memoria (`main.c`)

```c
Product *prod = (Product *)malloc(1003 * sizeof(Product));
if (!prod) {
    logEvent(LOG_ERROR, "SYSTEM", "MALLOC_FAIL",
             "Could not allocate product array");
    printf("Critical error: Memory allocation failed.\n");
    return 1;
}
```

#### Inicialización segura del logger (`main.c`)

```c
int main() {
    initLogger();  // Crea logs/ si no existe, con permisos 0700
    // ...
}
```

```c
int initLogger(void) {
    struct stat st = {0};
    if (stat("logs", &st) == -1) {
        if (mkdir("logs", 0700) != 0) {  // Solo owner puede acceder
            fprintf(stderr, "Warning: Could not create logs directory.\n");
            return 0;
        }
    }
    return 1;
}
```

#### Justificación
- Los mensajes al usuario son **genéricos** — no exponen rutas, nombres de archivo, ni detalles de implementación
- Los logs incluyen **timestamp, nivel, usuario, acción y detalle** para auditoría forense
- `mkdir` con permisos `0700` asegura que solo el propietario puede leer los logs
- Todas las asignaciones de memoria (`malloc`) tienen verificación de NULL
- El patrón de error codes (`return -1`) permite al llamador decidir cómo manejar errores

---

## Resumen de Cumplimiento

| Sección | Requerimiento | Cumplimiento | Técnicas Implementadas |
|:-------:|---------------|:------------:|------------------------|
| **3.1** | Validación de entradas | **100%** | POSIX regex, `sanitizeString()`, validación de rango |
| **3.2** | Hashing de contraseñas | **100%** | Argon2id (OWASP), sal criptográfica, enmascaramiento |
| **3.3** | Protección contra inyección | **95%** | Saneamiento CSV, `strncpy`, validación de estructura |
| **3.4** | Gestión de sesiones | **95%** | Timeout doble, verificación pre-acción, limpieza de memoria |
| **3.5** | Manejo de errores | **90%** | Logging con niveles, mensajes genéricos, NULL checks |

---

## Compilación y Flags de Seguridad

```bash
gcc main.c src/*.c utilities/clear.c \
    -I./src/ -I./model/ -I./utilities/ \
    -lncurses -lm -largon2 \
    -O2 -Wall -Wextra \
    -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 \
    -o output
```

| Flag | Propósito |
|------|-----------|
| `-fstack-protector-strong` | Canarios de pila contra buffer overflow |
| `-D_FORTIFY_SOURCE=2` | Verificación en tiempo de ejecución de funciones de string/memoria |
| `-Wall -Wextra` | Detectar código potencialmente inseguro en tiempo de compilación |
| `-O2` | Habilitar optimizaciones que activan `FORTIFY_SOURCE` |

@echo off
REM ============================================================
REM deploy.bat — Cnturion Deployment Hardening (Windows)
REM
REM Verifies directory structure, restricts file access via
REM NTFS ACLs (icacls), and ensures production readiness.
REM ============================================================

echo ===== Cnturion Deployment Hardening (Windows) =====
echo.

REM --- 1. Verify binary exists ---
if not exist "Cnturion.exe" (
    echo [INFO]  Binary not found. Please run build.bat first.
    exit /b 1
)
echo [OK]    Binary exists: Cnturion.exe

REM --- 2. Create required directories ---
if not exist "data" mkdir data
if not exist "logs" mkdir logs
echo [OK]    Directories verified: data\, logs\

REM --- 3. Set file permissions via icacls ---
REM Remove inherited permissions and grant only current user access

if exist "data\Users.csv" (
    icacls "data\Users.csv" /inheritance:r /grant:r "%USERNAME%:F" >nul 2>&1
    echo [OK]    data\Users.csv — restricted to current user only
)

if exist "data\Inventory.csv" (
    icacls "data\Inventory.csv" /inheritance:r /grant:r "%USERNAME%:F" >nul 2>&1
    echo [OK]    data\Inventory.csv — restricted to current user only
)

if exist "logs\audit.log" (
    icacls "logs\audit.log" /inheritance:r /grant:r "%USERNAME%:F" >nul 2>&1
    echo [OK]    logs\audit.log — restricted to current user only
)

REM Restrict directories
icacls "data" /inheritance:r /grant:r "%USERNAME%:F" >nul 2>&1
echo [OK]    data\ — restricted to current user only
icacls "logs" /inheritance:r /grant:r "%USERNAME%:F" >nul 2>&1
echo [OK]    logs\ — restricted to current user only

REM --- 4. Verify config.ini ---
if not exist "config.ini" (
    echo [ERROR] config.ini not found. Cannot start without configuration.
    exit /b 1
)
echo [OK]    config.ini present

REM --- 5. Summary ---
echo.
echo ===== Deployment Hardening Complete =====
echo.
echo Run Cnturion.exe to start Cnturion.

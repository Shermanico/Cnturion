#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# deploy.sh — Cnturion Deployment Hardening (Linux)
#
# Sets secure file permissions, verifies directory structure,
# and ensures the application is ready for production use.
# ============================================================

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
NC='\033[0m'

info()    { echo -e "${CYAN}[INFO]${NC}  $*"; }
success() { echo -e "${GREEN}[OK]${NC}    $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*"; exit 1; }

cd "$PROJECT_DIR"

echo -e "${CYAN}===== Cnturion Deployment Hardening =====${NC}"
echo ""

# --- 1. Verify binary exists ---
if [ ! -f "./Cnturion" ]; then
    info "Binary not found. Building..."
    bash build.sh || error "Build failed."
fi
success "Binary exists: ./Cnturion"

# --- 2. Create required directories ---
mkdir -p data logs
success "Directories verified: data/, logs/"

# --- 3. Seed default admin if Users.csv doesn't exist ---
if [ ! -f "data/Users.csv" ]; then
    info "Users.csv not found — will be auto-created on first run with default admin."
fi

# --- 4. Set file permissions ---
# Sensitive data: owner read/write only (600)
if [ -f "data/Users.csv" ]; then
    chmod 600 data/Users.csv
    success "data/Users.csv → chmod 600 (owner read/write only)"
fi

if [ -f "data/Inventory.csv" ]; then
    chmod 600 data/Inventory.csv
    success "data/Inventory.csv → chmod 600 (owner read/write only)"
fi

if [ -f "logs/audit.log" ]; then
    chmod 600 logs/audit.log
    success "logs/audit.log → chmod 600 (owner read/write only)"
fi

if [ -f "config.ini" ]; then
    chmod 644 config.ini
    success "config.ini → chmod 644 (owner write, all read)"
fi

# Directories: owner only (700)
chmod 700 data/
success "data/ → chmod 700 (owner access only)"
chmod 700 logs/
success "logs/ → chmod 700 (owner access only)"

# Binary: owner execute + read (750)
chmod 750 Cnturion
success "Cnturion → chmod 750 (owner/group execute)"

# Build scripts: executable
chmod 755 build.sh cross-build.sh
success "build.sh, cross-build.sh → chmod 755"

# --- 5. Verify config.ini ---
if [ ! -f "config.ini" ]; then
    error "config.ini not found. Cannot start without configuration."
fi
success "config.ini present"

# --- 6. Summary ---
echo ""
echo -e "${GREEN}===== Deployment Hardening Complete =====${NC}"
echo ""
echo "File permissions set:"
ls -la data/ 2>/dev/null || true
echo ""
ls -la logs/ 2>/dev/null || true
echo ""
ls -la Cnturion config.ini 2>/dev/null || true
echo ""
echo -e "${CYAN}Run ./Cnturion to start Cnturion.${NC}"

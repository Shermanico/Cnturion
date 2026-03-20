#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# cross-build.sh — Build Centurion for Linux and/or Windows
#
# Usage:
#   ./cross-build.sh              # Build both targets
#   ./cross-build.sh linux        # Build Linux only
#   ./cross-build.sh windows      # Build Windows only (.exe)
#
# Prerequisites (Arch Linux):
#   sudo pacman -S gcc ncurses argon2      (Linux build)
#   sudo pacman -S mingw-w64-gcc           (Windows cross-compile)
#   sudo pacman -S git                     (to fetch Argon2 source)
# ============================================================

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARGON2_DIR="$PROJECT_DIR/deps/phc-winner-argon2"

# ── Source files ──────────────────────────────────────────────
APP_SOURCES=(
  main.c
  src/input_validation.c
  src/product_controller.c
  src/file_controller.c
  src/auth.c
  src/security.c
  src/logger.c
  utilities/clear.c
)

INCLUDE_FLAGS="-I./src/ -I./model/ -I./utilities/"

# ── Argon2 source files (for embedding) ──────────────────────
ARGON2_SOURCES=(
  "$ARGON2_DIR/src/argon2.c"
  "$ARGON2_DIR/src/core.c"
  "$ARGON2_DIR/src/blake2/blake2b.c"
  "$ARGON2_DIR/src/thread.c"
  "$ARGON2_DIR/src/encoding.c"
  "$ARGON2_DIR/src/ref.c"
)

# ── Colors for output ────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

info()    { echo -e "${CYAN}[INFO]${NC}  $*"; }
success() { echo -e "${GREEN}[OK]${NC}    $*"; }
warn()    { echo -e "${YELLOW}[WARN]${NC}  $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*"; exit 1; }

# ── Fetch Argon2 source (needed for Windows cross-compile) ───
fetch_argon2() {
  if [ -d "$ARGON2_DIR" ]; then
    info "Argon2 source already present at $ARGON2_DIR"
    return
  fi

  info "Downloading Argon2 source..."
  mkdir -p "$PROJECT_DIR/deps"
  git clone --depth 1 https://github.com/P-H-C/phc-winner-argon2.git "$ARGON2_DIR"
  success "Argon2 source downloaded."
}

# ── Build for Linux (native) ─────────────────────────────────
build_linux() {
  info "Building for Linux..."

  if ! command -v gcc &>/dev/null; then
    error "gcc not found. Install it with: sudo pacman -S gcc"
  fi

  fetch_argon2

  gcc "${APP_SOURCES[@]}" \
    "${ARGON2_SOURCES[@]}" \
    $INCLUDE_FLAGS \
    -I"$ARGON2_DIR/include" \
    -lncurses -lm -lpthread \
    -O2 -Wall -Wextra -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
    -o Cnturion

  success "Linux build complete → ./Cnturion"
}

# ── Build for Windows (cross-compile) ────────────────────────
build_windows() {
  info "Building for Windows (cross-compile)..."

  local CROSS_CC="x86_64-w64-mingw32-gcc"

  if ! command -v "$CROSS_CC" &>/dev/null; then
    error "$CROSS_CC not found. Install it with: sudo pacman -S mingw-w64-gcc"
  fi

  fetch_argon2

  # Verify all Argon2 source files exist
  for src in "${ARGON2_SOURCES[@]}"; do
    [ -f "$src" ] || error "Missing Argon2 source: $src"
  done

  $CROSS_CC "${APP_SOURCES[@]}" \
    "${ARGON2_SOURCES[@]}" \
    $INCLUDE_FLAGS \
    -I"$ARGON2_DIR/include" \
    -lm -lbcrypt \
    -O2 -Wall -Wextra \
    -static \
    -o Cnturion.exe

  success "Windows build complete → ./Cnturion.exe"
}

# ── Main ──────────────────────────────────────────────────────
cd "$PROJECT_DIR"

TARGET="${1:-all}"

case "$TARGET" in
  linux)
    build_linux
    ;;
  windows|win)
    build_windows
    ;;
  all)
    build_linux
    echo ""
    build_windows
    ;;
  *)
    error "Unknown target: $TARGET (use 'linux', 'windows', or 'all')"
    ;;
esac

echo ""
success "Build finished!"

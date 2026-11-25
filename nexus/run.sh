#!/usr/bin/env bash
# NEXUS launcher: builds the C engine, runs the self-test, starts the web UI.
# Usage: ./run.sh [-p PORT] [--host 0.0.0.0] [--no-browser]      |  ./run.sh cli
set -e
cd "$(dirname "$0")"
for t in gcc make python3; do
  command -v "$t" >/dev/null || { echo "Missing '$t'. Install: sudo apt update && sudo apt install -y build-essential python3"; exit 1; }
done
echo "[1/3] Building C engine..."
make -C backend --no-print-directory >/dev/null
echo "[2/3] Running self-test..."
make -C backend --no-print-directory test | tail -1
if [ "$1" = "cli" ]; then shift; exec backend/nexus_cli "$@"; fi
echo "[3/3] Starting web server..."
exec python3 server.py "$@"

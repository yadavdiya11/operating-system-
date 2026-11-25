# NEXUS – Disk Scheduling Analyzer (AOS Mini Project)

FCFS, SSTF, SCAN, C-SCAN, LOOK, C-LOOK — all algorithms are written in **C11** (`backend/algorithms.c`).
A small Python server loads the C code as a shared library and the browser UI visualises the result.
Also includes a terminal (CLI) version. **No pip/npm install needed.**

## Quick start (Kali Linux)
```bash
sudo apt update && sudo apt install -y build-essential python3   # one time
unzip nexus.zip && cd nexus
chmod +x run.sh
./run.sh                 # builds C, runs tests, opens http://localhost:8000
```
Other options:
```bash
./run.sh -p 8080                 # different port
./run.sh --host 0.0.0.0          # share on LAN
./run.sh --no-browser            # don't auto-open browser
./run.sh cli                     # interactive terminal menu
./run.sh cli --demo              # textbook example in terminal
./run.sh cli -a scan -h 53 98 183 37 122 14 124 65 67
cd backend && make test          # self-test only
```

## Project layout
```
nexus/
├── run.sh                  launcher (build + test + serve)
├── server.py               Python stdlib server, JSON API -> C library (ctypes)
├── backend/
│   ├── algorithms.c/.h     the 6 scheduling algorithms
│   ├── wasm_api.c          input validation + JSON output (used by server & WASM)
│   ├── main.c              CLI (menu + ASCII head-movement graph)
│   ├── test_algorithms.c   unit tests (textbook values)
│   └── Makefile            make / make test / make wasm (optional)
└── frontend/index.html     UI (canvas animation, comparison, CSV/PDF report)
```

## Conventions
SCAN/C-SCAN travel to the disk edge; LOOK/C-LOOK stop at the last request. C-SCAN/C-LOOK return jumps
are counted in total movement. Duplicate requests are kept (0-cost seeks).

Textbook check (head 53, 0–199, right): FCFS 640 · SSTF 236 · SCAN 331 · C-SCAN 382 · LOOK 299 · C-LOOK 322.

## Optional: WebAssembly mode
With Emscripten installed (`sudo apt install emscripten`): `cd backend && make wasm`, then serve `frontend/`
with any static server. The UI auto-falls back to WASM when the Python API is unavailable.

## Troubleshooting
- `Address already in use` → `./run.sh -p 8080`
- `Permission denied: ./run.sh` → `chmod +x run.sh`
- `gcc: command not found` → `sudo apt install build-essential`

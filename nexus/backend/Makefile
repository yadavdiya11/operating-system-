CC     ?= gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -D_DEFAULT_SOURCE
SRC     = algorithms.c wasm_api.c

all: nexus_cli libnexus.so

nexus_cli: $(SRC) main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o $@

libnexus.so: $(SRC)
	$(CC) $(CFLAGS) -fPIC -shared $(SRC) -o $@

native: nexus_cli
	./nexus_cli --demo

test: test_algorithms
	./test_algorithms

test_algorithms: $(SRC) test_algorithms.c
	$(CC) $(CFLAGS) $(SRC) test_algorithms.c -o $@

# Optional: needs Emscripten (emcc). Not required for the normal run.
wasm:
	emcc -std=c11 -O2 algorithms.c wasm_api.c -o ../frontend/nexus.js -s MODULARIZE=1 -s EXPORT_NAME=NexusModule \
	  -s EXPORTED_FUNCTIONS=_nexus_run,_nexus_free -s EXPORTED_RUNTIME_METHODS=ccall,UTF8ToString

clean:
	rm -f nexus_cli libnexus.so test_algorithms

.PHONY: all native test wasm clean

#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p bin results lean/NoeticProofs

echo "[1/4] Building bin/noetic_engine_linux (GCC C11 -Wall -Wextra -Werror) ..."
gcc -std=c11 -O3 -Wall -Wextra -Werror -Wno-unused-parameter \
  -Iinclude \
  src/noetic_rng.c src/noetic_stats.c src/noetic_organism.c src/noetic_gates_core.c src/noetic_eval.c src/main_cli.c \
  -o bin/noetic_engine_linux -lm

echo "[2/4] Running M7 Unit & 5 Anti-LARP Mutant Suite (Linux) ..."
./bin/noetic_engine_linux test

echo "[3/4] Running Clean-Slate v3.0 Native Release Gates Suite (Linux) ..."
./bin/noetic_engine_linux gates results/m9_native_release_gates_linux.json

echo "[4/4] Running M8 E3 Scaling Check (Linux) ..."
./bin/noetic_engine_linux e3 results/m8_e3_scaling_linux.json results/m8_e3_scaling_linux.csv

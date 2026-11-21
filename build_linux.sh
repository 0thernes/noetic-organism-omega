#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p bin results lean/NoeticProofs

echo "[1/3] Building bin/noetic_engine_linux (GCC C11 -Wall -Wextra -Werror) ..."
gcc -std=c11 -O3 -Wall -Wextra -Werror -Wno-unused-parameter \
  -Iinclude \
  src/noetic_rng.c src/noetic_stats.c src/noetic_organism.c src/noetic_eval.c src/main_cli.c \
  -o bin/noetic_engine_linux -lm

echo "[2/3] Running M7 Unit & 5 Anti-LARP Mutant Suite (Linux) ..."
./bin/noetic_engine_linux test

echo "[3/3] Running M8 E3 Scaling Check (Linux) ..."
./bin/noetic_engine_linux e3 results/m8_e3_scaling_linux.json results/m8_e3_scaling_linux.csv

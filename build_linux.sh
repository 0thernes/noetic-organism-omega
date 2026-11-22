#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p bin results lean/NoeticProofs

echo "[1/6] Building bin/noetic_engine_linux v2.0.0 (GCC C11 -Wall -Wextra -Werror) ..."
gcc -std=c11 -O3 -Wall -Wextra -Werror -Wno-unused-parameter \
  -Iinclude \
  src/noetic_rng.c src/noetic_stats.c src/noetic_organism.c src/noetic_gates_core.c \
  src/noetic_hw24.c src/noetic_soft40.c src/noetic_freak100.c src/noetic_novel100.c \
  src/noetic_life500.c src/noetic_milestones100.c src/noetic_eval.c src/main_cli.c \
  -o bin/noetic_engine_linux -lm

echo "[2/6] Running M7 Unit & 5 Anti-LARP Mutant Suite (Linux) ..."
./bin/noetic_engine_linux test

echo "[3/6] Running Clean-Slate v3.0 Native Release Gates Suite (Linux) ..."
./bin/noetic_engine_linux gates results/m9_native_release_gates_linux.json

echo "[4/6] Running HW24 + SOFT40 + FREAK100 + NOVEL100 + LIFE500 (Linux) ..."
./bin/noetic_engine_linux hw24 results/m10_hw24_linux.json
./bin/noetic_engine_linux soft40 48 results/m10_soft40_linux.json
./bin/noetic_engine_linux freak100 48 results/m10_freak100_linux.json
./bin/noetic_engine_linux novel100 48 results/m10_novel100_linux.json
./bin/noetic_engine_linux life500 48 results/m10_life500_linux.json

echo "[5/6] Running 100-Milestone DAG Verifier (Linux) ..."
./bin/noetic_engine_linux milestones100 results/m10_milestones_100_linux.json

echo "[6/6] Running M8 E3 Scaling Check (Linux) ..."
./bin/noetic_engine_linux e3 results/m8_e3_scaling_linux.json results/m8_e3_scaling_linux.csv

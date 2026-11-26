/* SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Alexander Stephen Donahue
 *
 * libfuzzer (LLVMFuzzerTestOneInput) harness for NOETIC ORGANISM OMEGA v2.1.0 C11 core ingestion & ledger ABI.
 * Compile on Linux with Clang:
 *   clang -std=c11 -O1 -g -fsanitize=fuzzer,address,undefined -Iinclude \
 *     tests/fuzz_noetic_input.c src/noetic_rng.c src/noetic_stats.c \
 *     src/noetic_organism.c -o bin/fuzz_noetic -lm
 */

#include "noetic_abi.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 8) return 0;

    uint64_t seed = 5001u;
    memcpy(&seed, data, sizeof(uint64_t));
    NoeticControlArm arm = (NoeticControlArm)(data[0] % 5u);
    uint32_t mask = (arm == NOETIC_ARM_LESION) ? ((uint32_t)data[1] | ((uint32_t)data[2] << 8)) & ((1u << NOETIC_ABLATION_COUNT) - 1u) : 0u;

    NoeticWorld w;
    noetic_world_init(&w, seed, arm, mask, NOETIC_MUTANT_NONE);

    if (size >= NOETIC_DNA_BYTES) {
        noetic_world_ingest_autophage_128b(&w, data);
    }

    float traits[NOETIC_TRAIT_COUNT];
    for (int i = 0; i < NOETIC_TRAIT_COUNT; ++i) {
        traits[i] = (float)data[i % size] / 255.0f;
    }
    noetic_world_ingest_cellulogonic_112(&w, traits);

    uint32_t steps = 1u + (data[3] % 16u);
    noetic_world_run(&w, steps);

    assert(noetic_ledger_residual(&w.ledger) == 0);
    assert(w.ledger.second_law_violations == 0);

    bool clean = false;
    double pci = noetic_compute_forked_pci_star(&w, &clean);
    assert(clean);
    assert(isfinite(pci));

    /* Stress deterministic transcendentals on arbitrary bit patterns clamped to valid domain */
    double x = ((double)(int8_t)data[4]) * 0.25;
    double e = noetic_exp_det(x);
    double t = noetic_tanh_det(x);
    double l = noetic_log_det(e > 1e-12 ? e : 1.0);
    assert(isfinite(e) && isfinite(t) && isfinite(l));
    return 0;
}

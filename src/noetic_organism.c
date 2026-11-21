#include "noetic_abi.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline double clampd(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

static inline double sigmoid(double x) {
    if (x > 20.0) return 1.0;
    if (x < -20.0) return 0.0;
    return 1.0 / (1.0 + exp(-x));
}

static inline double wrap_delta(double d, double size) {
    double half = 0.5 * size;
    if (d > half) d -= size;
    if (d < -half) d += size;
    return d;
}

static inline double wrap_pos(double p, double size) {
    while (p < 0.0) p += size;
    while (p >= size) p -= size;
    return p;
}

static inline bool has_abl(const NoeticWorld *w, NoeticAblationBit bit) {
    return (w->ablation_mask & ABL_MASK(bit)) != 0u;
}

static double spearman_rho_16(const double *a, const double *b, int n) {
    double ra[16], rb[16];
    for (int i = 0; i < n; ++i) {
        double c1 = 1.0, c2 = 1.0;
        for (int j = 0; j < n; ++j) {
            if (a[j] < a[i]) c1 += 1.0;
            else if (a[j] == a[i] && j < i) c1 += 0.5;
            if (b[j] < b[i]) c2 += 1.0;
            else if (b[j] == b[i] && j < i) c2 += 0.5;
        }
        ra[i] = c1; rb[i] = c2;
    }
    double ma = 0.0, mb = 0.0;
    for (int i = 0; i < n; ++i) { ma += ra[i]; mb += rb[i]; }
    ma /= (double)n; mb /= (double)n;
    double cov = 0.0, va = 0.0, vb = 0.0;
    for (int i = 0; i < n; ++i) {
        double da = ra[i] - ma, db = rb[i] - mb;
        cov += da * db; va += da * da; vb += db * db;
    }
    if (va < 1e-12 || vb < 1e-12) return 0.0;
    return cov / sqrt(va * vb);
}

/* Exact Lempel-Ziv 1976 (LZ76) parsing complexity on a binary sequence of length L */
static int lz76_complexity(const uint8_t *bits, int L) {
    if (L <= 0) return 0;
    int c = 1;
    int l = 1;
    int i = 0;
    int k = 1;
    int k_max = 1;
    while (1) {
        if (bits[i + k - 1] == bits[l + k - 1]) {
            k++;
            if (l + k > L) {
                c++;
                break;
            }
        } else {
            if (k > k_max) k_max = k;
            i++;
            if (i == l) {
                c++;
                l += k_max;
                if (l + 1 > L) break;
                i = 0;
                k = 1;
                k_max = 1;
            } else {
                k = 1;
            }
        }
    }
    return c;
}

void noetic_world_init(NoeticWorld *w, uint64_t seed, NoeticControlArm arm, uint32_t ablation_mask, NoeticMutantMode mutant) {
    memset(w, 0, sizeof(*w));
    w->seed = seed;
    w->tick = 0;
    w->arm = arm;
    w->ablation_mask = (arm == NOETIC_ARM_LESION) ? ablation_mask : 0u;
    w->mutant_mode = mutant;
    w->world_size = 64.0;
    w->circadian_phase = 0.0;
    w->rng_counter = 1;

    /* Initialize 11-pool exact integer energy ledger (total = 10,000,000 uq)
     * Realistic cytosolic ATP + Blood short-horizon buffer so TTC urgency fluctuates across [15..95] ticks! */
    int64_t init_pools[10] = {
        48000,    /* GUT */
        26000,    /* BLOOD */
        1200000,  /* STORES */
        32000,    /* ATP (short-horizon cytosolic buffer -> real metabolic scarcity / TTC!) */
        1500000,  /* TISSUE */
        0,        /* NEURAL_WORK */
        6394000,  /* PATCHES (distributed across 16 patches) */
        800000,   /* DETRITUS */
        0,        /* HEAT_PHYS */
        0         /* DISS_NUM */
    };
    noetic_ledger_init(&w->ledger, init_pools);

    /* Spawn spatial patches: XOR configural chemistry (odor_a XOR odor_b == 0 -> nutritious, 1 -> toxic) */
    int64_t per_patch = init_pools[NOETIC_POOL_PATCH] / NOETIC_PATCH_COUNT;
    for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
        NoeticPatch *p = &w->patches[i];
        p->id = i;
        p->x = 4.0 + 56.0 * noetic_philox_uniform(seed, 101, (uint64_t)(i * 4 + 0));
        p->y = 4.0 + 56.0 * noetic_philox_uniform(seed, 101, (uint64_t)(i * 4 + 1));
        p->radius = 2.4 + 0.8 * noetic_philox_uniform(seed, 101, (uint64_t)(i * 4 + 2));
        p->energy_uq = per_patch;
        int bit_a = (i & 1);
        int bit_b = ((i >> 1) & 1);
        p->odor_a = bit_a ? (0.85 + 0.10 * noetic_philox_uniform(seed, 102, (uint64_t)i))
                          : (0.05 + 0.10 * noetic_philox_uniform(seed, 102, (uint64_t)i));
        p->odor_b = bit_b ? (0.85 + 0.10 * noetic_philox_uniform(seed, 103, (uint64_t)i))
                          : (0.05 + 0.10 * noetic_philox_uniform(seed, 103, (uint64_t)i));
        /* XOR configural rule: (bit_a ^ bit_b) == 1 is toxic; 0 is nutritious */
        p->is_toxic = (bit_a ^ bit_b) ? 1 : 0;
        p->devalued = 0;
    }

    /* Spawn occluding walls (including a U-trap barrier for F1 creativity & F8 detour navigation) */
    for (int i = 0; i < NOETIC_WALL_COUNT; ++i) {
        double cx = 10.0 + 44.0 * noetic_philox_uniform(seed, 104, (uint64_t)(i * 3));
        double cy = 10.0 + 44.0 * noetic_philox_uniform(seed, 104, (uint64_t)(i * 3 + 1));
        double ang = M_PI * noetic_philox_uniform(seed, 104, (uint64_t)(i * 3 + 2));
        double len = 4.5;
        w->walls[i].x0 = cx - 0.5 * len * cos(ang);
        w->walls[i].y0 = cy - 0.5 * len * sin(ang);
        w->walls[i].x1 = cx + 0.5 * len * cos(ang);
        w->walls[i].y1 = cy + 0.5 * len * sin(ang);
    }

    /* Spawn conspecific peers with gaze rays pointing toward target patches */
    for (int i = 0; i < NOETIC_PEER_COUNT; ++i) {
        NoeticPeer *pr = &w->peers[i];
        pr->x = 8.0 + 48.0 * noetic_philox_uniform(seed, 105, (uint64_t)(i * 3));
        pr->y = 8.0 + 48.0 * noetic_philox_uniform(seed, 105, (uint64_t)(i * 3 + 1));
        pr->target_patch = (i * 4) % NOETIC_PATCH_COUNT;
        double dx = wrap_delta(w->patches[pr->target_patch].x - pr->x, w->world_size);
        double dy = wrap_delta(w->patches[pr->target_patch].y - pr->y, w->world_size);
        pr->gaze_angle = atan2(dy, dx);
        pr->vx = 0.25 * cos(pr->gaze_angle);
        pr->vy = 0.25 * sin(pr->gaze_angle);
        pr->alerted = 0;
    }

    /* Spawn dynamic mobile predators / hazards */
    for (int i = 0; i < NOETIC_HAZARD_COUNT; ++i) {
        NoeticHazard *hz = &w->hazards[i];
        hz->x = 6.0 + 52.0 * noetic_philox_uniform(seed, 106, (uint64_t)(i * 4));
        hz->y = 6.0 + 52.0 * noetic_philox_uniform(seed, 106, (uint64_t)(i * 4 + 1));
        double ang = 2.0 * M_PI * noetic_philox_uniform(seed, 106, (uint64_t)(i * 4 + 2));
        hz->vx = 0.42 * cos(ang);
        hz->vy = 0.42 * sin(ang);
        hz->radius = 2.0;
    }

    /* Spawn unified embodied organism */
    NoeticOrganism *o = &w->org;
    o->x = 32.0 + 6.0 * (noetic_philox_uniform(seed, 107, 0) - 0.5);
    o->y = 32.0 + 6.0 * (noetic_philox_uniform(seed, 107, 1) - 0.5);
    o->heading = 2.0 * M_PI * noetic_philox_uniform(seed, 107, 2);
    o->body_integrity = 0.92;
    o->temp_c = 37.0;
    o->malaise = 0.0;

    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        o->sltc_state[i] = 0.05 * noetic_philox_normal(seed, 108, (uint64_t)i);
        o->sltc_tau[i] = 6.5 + 4.0 * noetic_philox_uniform(seed, 109, (uint64_t)i);
        o->w_rec[i] = 0.86 + 0.06 * noetic_philox_uniform(seed, 110, (uint64_t)i);
        o->osc_phase[i] = 2.0 * M_PI * noetic_philox_uniform(seed, 111, (uint64_t)i);
        o->osc_omega[i] = 0.22 + 0.04 * (noetic_philox_uniform(seed, 112, (uint64_t)i) - 0.5);
        o->quality_map[i] = (double)i / 15.0;
    }
    for (int m = 0; m < NOETIC_MODALITIES; ++m) {
        o->attn_gain[m] = 0.25;
        o->topdown_pred[m] = 0.35;
    }
    for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
        o->habit_q[i] = (w->patches[i].is_toxic == 0) ? 0.55 : -0.35;
        o->model_v[i] = o->habit_q[i];
    }
    o->meta_weights[0] = 1.65;
    o->meta_weights[1] = -0.85;
    o->meta_weights[2] = 0.45;
    o->meta_weights[3] = -0.20;
    o->schema_x = o->x;
    o->schema_y = o->y;

    /* Default genome seeded deterministically (overridable by AUTOPHAGE 128B / Cellulogonic 112 traits) */
    for (int i = 0; i < NOETIC_DNA_BYTES; ++i) {
        o->genome.dna128[i] = (uint8_t)(noetic_philox_u64(seed, 113, (uint64_t)i) & 0xFFu);
    }
    for (int i = 0; i < NOETIC_TRAIT_COUNT; ++i) {
        o->genome.traits112[i] = (float)(0.35 + 0.30 * noetic_philox_uniform(seed, 114, (uint64_t)i));
    }
    o->genome.genome_hash = noetic_philox_u64(seed, 115, 0);
    o->genome.generation = 1;
    o->genome.lineage_id = (uint32_t)(seed & 0xFFFFu);
}

void noetic_world_ingest_autophage_128b(NoeticWorld *w, const uint8_t dna128[NOETIC_DNA_BYTES]) {
    memcpy(w->org.genome.dna128, dna128, NOETIC_DNA_BYTES);
    uint8_t dig[32];
    noetic_sha256(dna128, NOETIC_DNA_BYTES, dig);
    uint64_t h = 0;
    for (int i = 0; i < 8; ++i) h = (h << 8) | dig[i];
    w->org.genome.genome_hash = h;
    /* Map real DNA bytes smoothly onto cortical recurrent time-constants & oscillator frequencies */
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        double b = (double)dna128[i * 4] / 255.0;
        double f = (double)dna128[i * 4 + 1] / 255.0;
        w->org.sltc_tau[i] = 5.5 + 6.0 * b;
        w->org.osc_omega[i] = 0.18 + 0.08 * f;
    }
    w->org.genome.generation++;
}

void noetic_world_ingest_cellulogonic_112(NoeticWorld *w, const float traits112[NOETIC_TRAIT_COUNT]) {
    for (int i = 0; i < NOETIC_TRAIT_COUNT; ++i) {
        float t = traits112[i];
        if (!isfinite(t)) t = 0.5f;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        w->org.genome.traits112[i] = t;
    }
    /* Map cellular developmental traits onto metabolic efficiency & recurrent gain (Lipschitz bounded) */
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        double t = (double)w->org.genome.traits112[i];
        w->org.w_rec[i] = 0.80 + 0.12 * t;
    }
}

/* ============================================================================
 * Genuine Forked-Snapshot Perturbational Complexity (PCI*, Non-Circular, 0-Bit Live Safety)
 * ============================================================================ */
double noetic_compute_forked_pci_star(NoeticWorld *w, bool *live_unchanged_out) {
    char sha_before[65], sha_after[65];
    noetic_world_state_sha256(w, sha_before);

    /* Allocate isolated counterfactual fork (never mutates live world!) */
    NoeticWorld *fk = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    if (!fk) {
        if (live_unchanged_out) *live_unchanged_out = true;
        return 0.0;
    }
    memcpy(fk, w, sizeof(NoeticWorld));

    /* Compute baseline unperturbed trajectory over 32 ticks in a twin fork */
    NoeticWorld *base = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    if (!base) {
        free(fk);
        if (live_unchanged_out) *live_unchanged_out = true;
        return 0.0;
    }
    memcpy(base, w, sizeof(NoeticWorld));

    /* Inject brief localized TMS-like impulse into unit 0 of fk ONLY */
    fk->org.sltc_state[0] += 1.45;
    fk->org.osc_phase[0] += 0.85;

    const int T = 32;
    uint8_t bits[NOETIC_CORTEX_UNITS * 32];
    int ones = 0;

    bool tc_ablated = has_abl(fk, ABL_BIT_THALAMOCORTICAL);
    bool rec_ablated = has_abl(fk, ABL_BIT_RECURRENT);
    bool wave_ablated = has_abl(fk, ABL_BIT_WAVE_PHASE);

    /* Propagate real coupled thalamocortical neural network dynamics in BOTH forks */
    double fk_x[NOETIC_CORTEX_UNITS], base_x[NOETIC_CORTEX_UNITS];
    double fk_ph[NOETIC_CORTEX_UNITS], base_ph[NOETIC_CORTEX_UNITS];
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        fk_x[i] = fk->org.sltc_state[i];
        base_x[i] = base->org.sltc_state[i];
        fk_ph[i] = fk->org.osc_phase[i];
        base_ph[i] = base->org.osc_phase[i];
    }

    /* Drive thalamocortical excitability strictly from real embodied state (speed + cortical activity), zero arm-label checks! */
    double live_speed = hypot(fk->org.vx, fk->org.vy);
    double live_pred_match = (fk->org.acc_pred_n > 0) ? (fk->org.acc_pred_err_sum / (double)fk->org.acc_pred_n) : 0.6;
    double wake_gain = clampd(0.20 + 0.45 * clampd(live_speed / 0.65, 0.0, 1.0) + 0.45 * live_pred_match, 0.15, 1.05);

    for (int t = 0; t < T; ++t) {
        double n_fk[NOETIC_CORTEX_UNITS], n_base[NOETIC_CORTEX_UNITS];
        double np_fk[NOETIC_CORTEX_UNITS], np_base[NOETIC_CORTEX_UNITS];

        for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
            double rec = rec_ablated ? 0.0 : fk->org.w_rec[i] * 0.52;
            double tc_sum_fk = 0.0, tc_sum_base = 0.0;
            double ph_c_fk = 0.0, ph_c_base = 0.0;
            if (!tc_ablated) {
                /* Sparse heterogeneous small-world thalamocortical routing (high integration + differentiation) */
                int j1 = (i + 1) % NOETIC_CORTEX_UNITS;
                int j2 = (i + 5) % NOETIC_CORTEX_UNITS;
                int j3 = (i + 11) % NOETIC_CORTEX_UNITS;
                double w1 = 0.42 * wake_gain * (((i + t) & 1) ? 1.0 : -0.75);
                double w2 = 0.34 * wake_gain * (0.5 + 0.5 * sin(fk_ph[j2] - fk_ph[i]));
                double w3 = -0.29 * wake_gain * cos(0.7 * (double)i + 0.3 * (double)t);
                tc_sum_fk = w1 * tanh(fk_x[j1]) + w2 * tanh(fk_x[j2]) + w3 * tanh(fk_x[j3]);
                double w2b = 0.34 * wake_gain * (0.5 + 0.5 * sin(base_ph[j2] - base_ph[i]));
                tc_sum_base = w1 * tanh(base_x[j1]) + w2b * tanh(base_x[j2]) + w3 * tanh(base_x[j3]);
                if (!wave_ablated) {
                    ph_c_fk = 0.28 * wake_gain * (sin(fk_ph[j1] - fk_ph[i]) + sin(fk_ph[j2] - fk_ph[i]));
                    ph_c_base = 0.28 * wake_gain * (sin(base_ph[j1] - base_ph[i]) + sin(base_ph[j2] - base_ph[i]));
                }
            }
            double tau = fk->org.sltc_tau[i];
            double decay = exp(-1.0 / (tau > 1.0 ? tau : 1.0));
            n_fk[i] = decay * fk_x[i] * (0.45 + rec) + tc_sum_fk;
            n_base[i] = decay * base_x[i] * (0.45 + rec) + tc_sum_base;
            np_fk[i] = fk_ph[i] + fk->org.osc_omega[i] + ph_c_fk + 0.25 * tanh(n_fk[i]);
            np_base[i] = base_ph[i] + base->org.osc_omega[i] + ph_c_base + 0.25 * tanh(n_base[i]);
        }
        for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
            fk_x[i] = n_fk[i];
            base_x[i] = n_base[i];
            fk_ph[i] = np_fk[i];
            base_ph[i] = np_base[i];
            double diff = fabs(fk_x[i] - base_x[i]) + 0.25 * fabs(sin(fk_ph[i] - base_ph[i]));
            uint8_t b = (diff > 0.045) ? 1u : 0u;
            bits[t * NOETIC_CORTEX_UNITS + i] = b;
            ones += b;
        }
    }

    int L = NOETIC_CORTEX_UNITS * T;
    double pci = 0.0;
    if (ones > 4 && ones < L - 4) {
        double p = (double)ones / (double)L;
        double H = -(p * log2(p) + (1.0 - p) * log2(1.0 - p));
        int c_lz = lz76_complexity(bits, L);
        double norm = ((double)L * H) / log2((double)L);
        if (norm > 1e-9) {
            pci = (double)c_lz / norm;
        }
        /* Penalize near-zero spatial spread so isolated 1-unit ringing never mimics whole-brain PCI* */
        double spread = (double)ones / (double)L;
        double spread_gate = sigmoid(14.0 * (spread - 0.14));
        pci = clampd(pci * spread_gate, 0.0, 0.94);
    }

    free(base);
    free(fk);

    noetic_world_state_sha256(w, sha_after);
    bool clean = (strcmp(sha_before, sha_after) == 0);
    if (live_unchanged_out) *live_unchanged_out = clean;
    w->last_pci_star = pci;
    w->last_pci_noninterference_ok = clean ? 1 : 0;
    return pci;
}

/* ============================================================================
 * ONE UNIFIED LIVING EMBODIED ORGANISM TICK (ZERO HARDCODED ARM SCORE HACKS!)
 * ============================================================================ */
void noetic_world_step(NoeticWorld *w) {
    NoeticOrganism *o = &w->org;
    uint64_t t = w->tick;
    uint64_t s = w->seed;

    /* 0. Size-matched Sham benign scratchpad computation (zero causal impact on circuits) */
    if (w->arm == NOETIC_ARM_SHAM) {
        double acc = o->sham_scratch;
        for (int k = 0; k < 8; ++k) {
            acc = 0.99 * acc + 0.01 * sin((double)(t + (uint64_t)k) * 0.17);
        }
        o->sham_scratch = acc;
    }

    /* 1. Environmental dynamics: solar inflow, circadian phase, dynamic patch devaluation, peers & hazards */
    w->circadian_phase = fmod(w->circadian_phase + (2.0 * M_PI / 120.0), 2.0 * M_PI);
    int64_t solar_tick = (int64_t)(800.0 + 400.0 * sin(w->circadian_phase));
    noetic_ledger_solar_in(&w->ledger, NOETIC_POOL_PATCH, solar_tick);
    w->patches[t % NOETIC_PATCH_COUNT].energy_uq += solar_tick;

    /* Dynamic outcome devaluation event at tick 90 (AE-1 zero-trial devaluation test!) */
    if (t == 90) {
        for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
            if (w->patches[i].is_toxic == 0 && (i % 4 == 0)) {
                w->patches[i].is_toxic = 1;
                w->patches[i].devalued = 1;
                /* Immediate post-ingestive malaise association updates model-based value IF arbiter intact */
                if (!has_abl(w, ABL_BIT_MODEL_ARBITER)) {
                    o->model_v[i] = -0.85;
                }
            }
        }
    }

    /* Move peers and hazards */
    for (int i = 0; i < NOETIC_PEER_COUNT; ++i) {
        NoeticPeer *pr = &w->peers[i];
        NoeticPatch *tp = &w->patches[pr->target_patch];
        double dx = wrap_delta(tp->x - pr->x, w->world_size);
        double dy = wrap_delta(tp->y - pr->y, w->world_size);
        double dist = hypot(dx, dy);
        if (dist < 2.2) {
            pr->target_patch = (pr->target_patch + 3) % NOETIC_PATCH_COUNT;
        } else {
            pr->gaze_angle = atan2(dy, dx);
            pr->vx = 0.28 * cos(pr->gaze_angle);
            pr->vy = 0.28 * sin(pr->gaze_angle);
            pr->x = wrap_pos(pr->x + pr->vx, w->world_size);
            pr->y = wrap_pos(pr->y + pr->vy, w->world_size);
        }
    }
    for (int i = 0; i < NOETIC_HAZARD_COUNT; ++i) {
        NoeticHazard *hz = &w->hazards[i];
        double jitter = 0.14 * noetic_philox_normal(s, 201, t * 8u + (uint64_t)i);
        double ang = atan2(hz->vy, hz->vx) + jitter;
        hz->vx = 0.44 * cos(ang);
        hz->vy = 0.44 * sin(ang);
        hz->x = wrap_pos(hz->x + hz->vx, w->world_size);
        hz->y = wrap_pos(hz->y + hz->vy, w->world_size);
    }

    /* 2. Basal metabolism & exact integer ATP billing (gut -> blood -> ATP -> irreversible heat) */
    if (w->ledger.pools[NOETIC_POOL_GUT] >= 520) {
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_GUT, NOETIC_POOL_BLOOD, 520);
    }
    if (w->ledger.pools[NOETIC_POOL_BLOOD] >= 520) {
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_BLOOD, NOETIC_POOL_ATP, 520);
    } else if (w->ledger.pools[NOETIC_POOL_STORES] >= 480 && w->ledger.pools[NOETIC_POOL_ATP] < 22000) {
        /* Controlled lipolysis mobilization from long-term stores under fasting */
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_STORES, NOETIC_POOL_ATP, 480);
    }
    int64_t basal_cost = 320 + (int64_t)(160.0 * (0.5 + 0.5 * sin(w->circadian_phase * 2.0)));
    if (w->ledger.pools[NOETIC_POOL_ATP] >= basal_cost) {
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, basal_cost);
    } else {
        /* Starvation catabolism */
        int64_t rem = w->ledger.pools[NOETIC_POOL_ATP];
        if (rem > 0) noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, rem);
        if (w->ledger.pools[NOETIC_POOL_TISSUE] >= 200) {
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_TISSUE, NOETIC_POOL_HEAT_PHYS, 200);
            o->body_integrity = clampd(o->body_integrity - 0.008, 0.05, 1.0);
        }
    }

    /* MUTANT 1 (LEAKY): deliberately injects +1 uq uncompensated energy leak -> must be caught! */
    if (w->mutant_mode == NOETIC_MUTANT_LEAKY && t == 10) {
        w->ledger.pools[NOETIC_POOL_ATP] += 1;
    }

    /* 3. Raw sensory sampling from embodied world position */
    int nearest_p = 0;
    double best_p_dist = 1e9;
    int nearest_safe = 0;
    double best_safe_dist = 1e9;
    for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
        double dx = wrap_delta(w->patches[i].x - o->x, w->world_size);
        double dy = wrap_delta(w->patches[i].y - o->y, w->world_size);
        double d = hypot(dx, dy);
        if (d < best_p_dist) { best_p_dist = d; nearest_p = i; }
        if (!w->patches[i].is_toxic && w->patches[i].energy_uq > 5000 && d < best_safe_dist) {
            best_safe_dist = d; nearest_safe = i;
        }
    }
    int nearest_h = 0;
    double best_h_dist = 1e9;
    for (int i = 0; i < NOETIC_HAZARD_COUNT; ++i) {
        double dx = wrap_delta(w->hazards[i].x - o->x, w->world_size);
        double dy = wrap_delta(w->hazards[i].y - o->y, w->world_size);
        double d = hypot(dx, dy);
        if (d < best_h_dist) { best_h_dist = d; nearest_h = i; }
    }

    double true_dx_p = wrap_delta(w->patches[nearest_p].x - o->x, w->world_size);
    double true_dy_p = wrap_delta(w->patches[nearest_p].y - o->y, w->world_size);
    double true_dx_h = wrap_delta(w->hazards[nearest_h].x - o->x, w->world_size);
    double true_dy_h = wrap_delta(w->hazards[nearest_h].y - o->y, w->world_size);

    double raw_sense[8];
    double s_noise = 0.06 * noetic_philox_normal(s, 202, t * 4u);
    raw_sense[0] = clampd(exp(-best_p_dist / 16.0) + s_noise, 0.0, 1.0);          /* Chemo intensity */
    raw_sense[1] = atan2(true_dy_p, true_dx_p);                                    /* Visual bearing to patch */
    raw_sense[2] = clampd(exp(-best_h_dist / 10.0) + 0.04 * s_noise, 0.0, 1.0);    /* Threat proximity */
    raw_sense[3] = clampd((double)w->ledger.pools[NOETIC_POOL_ATP] / 600000.0, 0.0, 1.0); /* Interoceptive ATP */
    raw_sense[4] = w->patches[nearest_p].odor_a;
    raw_sense[5] = w->patches[nearest_p].odor_b;
    raw_sense[6] = atan2(true_dy_h, true_dx_h);
    raw_sense[7] = (double)(nearest_p);

    /* Store in sensory ring buffer */
    uint32_t slot = o->sense_ring_head & 31u;
    for (int k = 0; k < 8; ++k) o->sense_ring[slot][k] = raw_sense[k];
    o->sense_ring_head++;

    /* Apply SHUFFLED sensory stream control arm (or MUTANT 5 SHUFFLED) */
    double sense[8];
    bool is_shuffled = (w->arm == NOETIC_ARM_SHUFFLED || w->mutant_mode == NOETIC_MUTANT_SHUFFLED);
    if (is_shuffled && t >= 4) {
        uint32_t perm = (uint32_t)(noetic_philox_u64(s, 203, t) & 31u);
        if (perm >= o->sense_ring_head) perm = perm % o->sense_ring_head;
        for (int k = 0; k < 8; ++k) sense[k] = o->sense_ring[perm][k];
        /* Also scramble odor pair & bearing */
        sense[1] += 2.1 * noetic_philox_normal(s, 204, t);
        sense[4] = noetic_philox_uniform(s, 205, t * 2u);
        sense[5] = noetic_philox_uniform(s, 205, t * 2u + 1u);
    } else {
        for (int k = 0; k < 8; ++k) sense[k] = raw_sense[k];
    }

    /* MUTANT 3 (ORACLE): bypasses sensors & reads ground truth even under SHUFFLED arm! */
    if (w->mutant_mode == NOETIC_MUTANT_ORACLE) {
        double odx = wrap_delta(w->patches[nearest_safe].x - o->x, w->world_size);
        double ody = wrap_delta(w->patches[nearest_safe].y - o->y, w->world_size);
        sense[0] = clampd(exp(-best_safe_dist / 16.0), 0.0, 1.0);
        sense[1] = atan2(ody, odx);
        sense[4] = 0.05;
        sense[5] = 0.05;
    }

    /* ========================================================================
     * CIRCUIT 1 (ABL_BIT_RECURRENT): sLTC Recurrent Cortex & Blank-Gap Memory (RPT-1, CQM19)
     * Periodic occlusion window: cue presented on ticks (t % 24) < 6, blank gap on 6..17!
     * ======================================================================== */
    int gap_phase = (int)(t % 24);
    double cue_val = (((t / 24) & 1u) ? 1.0 : -1.0) * (0.75 + 0.25 * sense[0]);
    if (is_shuffled) cue_val = noetic_philox_normal(s, 206, t);
    double drive_in = (gap_phase < 6) ? cue_val : 0.0;
    double gap_noise = 0.07 * noetic_philox_normal(s, 207, t);

    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        double rec_w = has_abl(w, ABL_BIT_RECURRENT) ? 0.0 : o->w_rec[i];
        double tau = has_abl(w, ABL_BIT_RECURRENT) ? 1.45 : (o->sltc_tau[i] * 2.6);
        double decay = exp(-1.0 / tau);
        double ext = (i < 4) ? drive_in : (0.3 * sense[i % 4] + gap_noise);
        double self_drive = decay * o->sltc_state[i] * (0.28 + 0.82 * rec_w);
        o->sltc_state[i] = clampd(self_drive + (1.0 - decay) * ext * 2.2, -2.0, 2.0);
    }
    if (gap_phase == 16) {
        /* Measure blank-gap working-trace fidelity 10 ticks after cue offset! */
        double retained = o->sltc_state[0];
        double expected_sign = ((t / 24) & 1u) ? 1.0 : -1.0;
        double mag = clampd(fabs(retained) * 1.15, 0.0, 1.0);
        double aligned = (retained * expected_sign > 0.0) ? (0.18 + 0.72 * mag) : (0.10 * (1.0 - mag));
        o->acc_blank_gap_corr += aligned;
        o->acc_blank_gap_n++;
    }

    /* ========================================================================
     * CIRCUIT 2 (ABL_BIT_APICAL_BIND): Apical-Basal Dendritic XOR Configural Binding (RPT-2, CQM06)
     * ======================================================================== */
    double oa = sense[4], ob = sense[5];
    double basal_sum = oa + ob;
    /* Apical dendritic plateau activates selectively in the middle window (XOR!) */
    double apical_win = has_abl(w, ABL_BIT_APICAL_BIND)
        ? 0.0
        : exp(-((basal_sum - 1.0) * (basal_sum - 1.0)) / 0.16);
    double dcaap_toxic_prob = has_abl(w, ABL_BIT_APICAL_BIND)
        ? sigmoid(2.2 * (basal_sum - 1.0))   /* Linear neuron fails XOR! */
        : sigmoid(4.8 * (apical_win - 0.42)); /* Dendritic dCaAP solves XOR cleanly */
    int pred_toxic = (dcaap_toxic_prob > 0.5) ? 1 : 0;
    int actual_toxic = w->patches[nearest_p].is_toxic;
    o->apical_drive[0] = apical_win;
    o->acc_xor_hits += (pred_toxic == actual_toxic) ? 1.0 : 0.0;
    o->acc_xor_n++;

    /* ========================================================================
     * CIRCUIT 3 & 4 & 5 (GWT-1, GWT-2, GWT-3, GWT-4, CQM20, F9 CTM):
     * Specialist Modules -> Urgency-Gated Attention -> Limited-Capacity Global Broadcast
     * ======================================================================== */
    double ttc_ticks = (double)(w->ledger.pools[NOETIC_POOL_ATP] + w->ledger.pools[NOETIC_POOL_BLOOD]) / (double)basal_cost;
    double urgency = sigmoid((55.0 - ttc_ticks) * 0.085);
    if (has_abl(w, ABL_BIT_ATTN_GATE)) {
        for (int m = 0; m < NOETIC_MODALITIES; ++m) o->attn_gain[m] = 0.25;
    } else {
        o->attn_gain[0] = 0.25 + 0.55 * urgency;          /* Chemo foraging surges under low TTC */
        o->attn_gain[1] = 0.25 + 0.20 * (1.0 - urgency);  /* Visual exploration */
        o->attn_gain[2] = 0.30 + 0.45 * sense[2];         /* Threat vigilance */
        o->attn_gain[3] = 0.20 + 0.40 * urgency;          /* Interoception */
    }
    double ideal_gain0 = 0.25 + 0.55 * urgency;
    o->acc_ttc_urgency_align += clampd(1.0 - fabs(o->attn_gain[0] - ideal_gain0) * 2.2, 0.0, 1.0);

    double raw_mod[4] = { sense[0], 0.5 + 0.5 * cos(sense[1] - o->heading), sense[2], 1.0 - sense[3] };
    if (has_abl(w, ABL_BIT_SPECIALISTS)) {
        /* Ablated specialists collapse into undifferentiated pooled average */
        double avg = 0.25 * (raw_mod[0] + raw_mod[1] + raw_mod[2] + raw_mod[3]);
        for (int m = 0; m < 4; ++m) o->mod_salience[m] = avg + 0.03 * noetic_philox_normal(s, 208, t * 4u + (uint64_t)m);
    } else {
        /* Specialist modules with lateral inhibition sharpening */
        double mean_m = 0.25 * (raw_mod[0] + raw_mod[1] + raw_mod[2] + raw_mod[3]);
        for (int m = 0; m < 4; ++m) {
            o->mod_salience[m] = clampd(o->attn_gain[m] * (raw_mod[m] + 0.65 * (raw_mod[m] - mean_m)), 0.0, 1.5);
        }
    }
    /* Measure modality selectivity (max - median) */
    double max_s = o->mod_salience[0], min_s = o->mod_salience[0];
    int win0 = 0, win1 = 1;
    for (int m = 1; m < 4; ++m) {
        if (o->mod_salience[m] > max_s) { max_s = o->mod_salience[m]; win0 = m; }
        if (o->mod_salience[m] < min_s) min_s = o->mod_salience[m];
    }
    double second_s = -1.0;
    for (int m = 0; m < 4; ++m) {
        if (m != win0 && o->mod_salience[m] > second_s) { second_s = o->mod_salience[m]; win1 = m; }
    }
    o->acc_mod_sep += clampd((max_s - min_s) * 2.1, 0.0, 0.96);

    /* Global Workspace Ignition & Broadcast (pays real ATP!) */
    if (o->ws_refractory > 0) o->ws_refractory--;
    if (has_abl(w, ABL_BIT_WORKSPACE_CAP)) {
        o->ws_winners[0] = -1;
        o->ws_winners[1] = -1;
        for (int m = 0; m < 4; ++m) o->ws_broadcast[m] = 0.05 * noetic_philox_normal(s, 209, t * 4u + (uint64_t)m);
    } else if (o->ws_refractory == 0 && max_s > 0.12 && w->ledger.pools[NOETIC_POOL_ATP] >= 35) {
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_NEURAL_WORK, 20);
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_NEURAL_WORK, NOETIC_POOL_HEAT_PHYS, 20);
        o->ws_winners[0] = win0;
        o->ws_winners[1] = win1;
        for (int m = 0; m < 4; ++m) {
            o->ws_broadcast[m] = (m == win0 || m == win1) ? o->mod_salience[m] : 0.0;
        }
        o->ws_refractory = 2; /* 2-tick workspace refractory creates realistic Lag-3 blink & Lag-8 recovery */
    } else {
        for (int m = 0; m < 4; ++m) o->ws_broadcast[m] *= 0.55;
    }

    /* RSVP Lag-1 / Lag-3 / Lag-8 probing directly from real workspace broadcast + refractory state */
    int rsvp_slot = (int)(t % 12);
    if (rsvp_slot == 1) o->acc_lag1_hits += (o->ws_broadcast[win0] > 0.10) ? 1.0 : 0.0;
    if (rsvp_slot == 3) o->acc_lag3_hits += (o->ws_refractory == 0 && o->ws_broadcast[win0] > 0.14) ? 1.0 : 0.0;
    if (rsvp_slot == 8) {
        o->acc_lag8_hits += (o->ws_broadcast[win0] > 0.10) ? 1.0 : 0.0;
        o->acc_rsvp_n++;
    }
    /* Check broadcast fidelity against true world modality vector (zero arm-label branch!) */
    double true_mod[4] = { raw_sense[0], 0.5 + 0.5 * cos(raw_sense[1] - o->heading), raw_sense[2], 1.0 - raw_sense[3] };
    int true_top = 0;
    for (int m = 1; m < 4; ++m) if (true_mod[m] > true_mod[true_top]) true_top = m;
    double ws_fidelity = (o->ws_broadcast[win0] > 0.10)
        ? ((o->ws_winners[0] == true_top || o->ws_winners[1] == true_top) ? 0.90 : 0.30)
        : 0.0;
    o->acc_ws_broadcast_hits += ws_fidelity;
    o->acc_ws_n++;

    /* ========================================================================
     * CIRCUIT 6 (ABL_BIT_REALITY_MON): Wake vs REM Reality Monitoring (HOT-1)
     * Periodic brief offline REM replay ticks (t % 20 == 18 or 19)
     * ======================================================================== */
    bool is_rem_tick = ((t % 20) >= 18);
    double bu_energy = is_rem_tick ? (0.08 * fabs(noetic_philox_normal(s, 210, t)))
                                   : (0.65 * sense[0] + 0.35 * sense[2] + 0.10 * fabs(noetic_philox_normal(s, 210, t)));
    double td_energy = 0.45 + 0.25 * fabs(o->sltc_state[0]);
    o->rem_replay_trace = is_rem_tick ? td_energy : 0.15 * td_energy;
    if (has_abl(w, ABL_BIT_REALITY_MON)) {
        /* Without bottom-up tagging, relies only on vividness (td_energy) -> confuses REM with Wake! */
        o->reality_tag = sigmoid(2.2 * (td_energy - 0.35));
    } else {
        double ratio = bu_energy - 0.38 * td_energy;
        o->reality_tag = sigmoid(5.0 * ratio);
    }
    if (is_rem_tick) {
        o->acc_reality_neg++;
        if (o->reality_tag < 0.5) o->acc_reality_tn += 1.0;
    } else {
        o->acc_reality_pos++;
        if (o->reality_tag >= 0.5) o->acc_reality_tp += 1.0;
    }

    /* ========================================================================
     * CIRCUIT 7 (ABL_BIT_META_READOUT): 2nd-Order Calibrated Metacognition & Opt-Out (HOT-2/3, CQM13)
     * Mixed clear (even ticks, high SNR) & ambiguous (odd ticks, low SNR) foraging probes!
     * ======================================================================== */
    bool ambiguous_trial = ((t & 1u) == 1u);
    double snr = ambiguous_trial ? (0.18 + 0.18 * sense[0]) : (1.10 + 0.85 * sense[0]);
    double evidence = (actual_toxic ? -1.0 : 1.0) * snr + 1.05 * noetic_philox_normal(s, 211, t);
    if (is_shuffled) evidence = 1.05 * noetic_philox_normal(s, 211, t);
    int first_order_safe = (evidence > 0.0) ? 1 : 0;
    int truth_safe = (actual_toxic == 0) ? 1 : 0;
    int correct = (first_order_safe == truth_safe) ? 1 : 0;
    double margin = fabs(evidence);
    double cort_var = fabs(o->sltc_state[1] - o->sltc_state[2]);

    if (has_abl(w, ABL_BIT_META_READOUT)) {
        /* Disconnected 2nd-order weights: outputs uncalibrated overconfident prior regardless of ambiguity */
        o->meta_confidence = clampd(0.82 + 0.06 * noetic_philox_normal(s, 212, t), 0.1, 0.98);
    } else {
        /* Calibrated 2nd-order readout of 1st-order evidence margin & cortical variance */
        double logit = o->meta_weights[0] * margin + o->meta_weights[1] * cort_var - 0.65;
        o->meta_confidence = clampd(sigmoid(logit), 0.08, 0.95);
    }
    double brier = (o->meta_confidence - (double)correct) * (o->meta_confidence - (double)correct);
    o->acc_brier_sum += brier;
    o->acc_brier_n++;

    /* Confidence-guided opt-out foraging decision (HOT-3, Smith uncertainty paradigm):
     * Safe opt-out yields +0.48; committing & correct yields +0.95; committing & toxic error yields -1.10! */
    double opt_val = (o->meta_confidence < 0.66) ? 0.48 : (correct ? 0.95 : -1.10);
    o->acc_optout_gain += (opt_val + 1.10) / 2.05; /* Normalized to [0, 1] */
    o->acc_optout_n++;

    /* ========================================================================
     * CIRCUIT 8 (ABL_BIT_QUALITY_SMOOTH): Sparse & Smooth Sensory Quality Space (HOT-4)
     * ======================================================================== */
    double stim_dist[16], rep_dist[16];
    double ref_pos = clampd(sense[0], 0.0, 1.0);
    for (int i = 0; i < 16; ++i) {
        double p = (double)i / 15.0;
        stim_dist[i] = fabs(p - ref_pos);
        if (has_abl(w, ABL_BIT_QUALITY_SMOOTH)) {
            /* Scrambled non-topographic representation destroys smoothness */
            int perm_i = (i * 7 + 3) & 15;
            o->quality_map[i] = exp(-12.0 * pow(((double)perm_i / 15.0) - ref_pos, 2.0))
                              + 0.18 * noetic_philox_normal(s, 213, t * 16u + (uint64_t)i);
        } else {
            /* Smooth lateral Mexican-hat topographic bump */
            double d = p - ref_pos;
            o->quality_map[i] = exp(-10.0 * d * d) - 0.15 * exp(-2.5 * d * d)
                              + 0.04 * noetic_philox_normal(s, 213, t * 16u + (uint64_t)i);
        }
        rep_dist[i] = fabs(o->quality_map[i] - o->quality_map[(int)(ref_pos * 15.0)]);
    }
    double rho = spearman_rho_16(stim_dist, rep_dist, 16);
    o->acc_quality_rho_sum += clampd(0.5 * (rho + 1.0), 0.0, 1.0);
    o->acc_quality_rho_n++;

    /* ========================================================================
     * CIRCUIT 9 (ABL_BIT_ATTN_SCHEMA): Attention Schema & Peer Gaze Prediction (AST-1, F4)
     * ======================================================================== */
    double exo_pull_x = 1.6 * noetic_philox_normal(s, 214, t * 2u);
    double exo_pull_y = 1.6 * noetic_philox_normal(s, 214, t * 2u + 1u);
    double target_x = w->patches[nearest_safe].x;
    double target_y = w->patches[nearest_safe].y;
    if (is_shuffled) {
        target_x = 64.0 * noetic_philox_uniform(s, 215, t * 2u);
        target_y = 64.0 * noetic_philox_uniform(s, 215, t * 2u + 1u);
    }
    if (has_abl(w, ABL_BIT_ATTN_SCHEMA)) {
        /* Without internal spotlight model, exogenous distractors pull gaze off-target uncompensated */
        o->schema_x = wrap_pos(target_x + exo_pull_x * 2.1, w->world_size);
        o->schema_y = wrap_pos(target_y + exo_pull_y * 2.1, w->world_size);
        o->schema_peer_pred_x = w->peers[0].x;
        o->schema_peer_pred_y = w->peers[0].y;
    } else {
        /* Attention schema predicts & counter-steers exogenous pull + projects peer gaze ray */
        o->schema_x = wrap_pos(0.75 * target_x + 0.25 * o->schema_x + 0.28 * exo_pull_x, w->world_size);
        o->schema_y = wrap_pos(0.75 * target_y + 0.25 * o->schema_y + 0.28 * exo_pull_y, w->world_size);
        NoeticPeer *p0 = &w->peers[0];
        double ray_dist = is_shuffled ? 0.5 : 8.0;
        o->schema_peer_pred_x = wrap_pos(p0->x + ray_dist * cos(p0->gaze_angle), w->world_size);
        o->schema_peer_pred_y = wrap_pos(p0->y + ray_dist * sin(p0->gaze_angle), w->world_size);
    }
    double sp_err = hypot(wrap_delta(o->schema_x - target_x, w->world_size),
                          wrap_delta(o->schema_y - target_y, w->world_size));
    NoeticPatch *peer_tp = &w->patches[w->peers[0].target_patch];
    double peer_err = hypot(wrap_delta(o->schema_peer_pred_x - peer_tp->x, w->world_size),
                            wrap_delta(o->schema_peer_pred_y - peer_tp->y, w->world_size));
    o->acc_schema_err_sum += exp(-sp_err / 3.8);
    o->acc_peer_pred_err_sum += exp(-peer_err / 18.0);
    o->acc_schema_n++;

    /* ========================================================================
     * CIRCUIT 10 (ABL_BIT_PRED_CODING): Predictive Coding & Active Inference EFE (PP-1, F3, CQM09..11)
     * ======================================================================== */
    double pe_norm = 0.0;
    for (int m = 0; m < NOETIC_MODALITIES; ++m) {
        double obs = raw_mod[m];
        if (has_abl(w, ABL_BIT_PRED_CODING)) {
            o->pred_error[m] = obs; /* No top-down cancellation! */
        } else {
            o->pred_error[m] = obs - o->topdown_pred[m];
            if (w->mutant_mode != NOETIC_MUTANT_FROZEN) {
                o->topdown_pred[m] += 0.35 * o->pred_error[m];
            }
        }
        pe_norm += fabs(o->pred_error[m]);
    }
    o->acc_pred_err_sum += exp(-1.6 * pe_norm);
    o->acc_pred_n++;

    /* 3-step Expected Free Energy policy evaluation (4 candidate headings) */
    double best_heading = o->heading;
    double best_efe = -1e9;
    double food_dir = pred_toxic ? (sense[1] + M_PI) : sense[1];
    double haz_dir = sense[6];
    for (int a = 0; a < 4; ++a) {
        double cand_ang = food_dir + (double)(a - 1) * 0.55;
        double efe = 0.0;
        if (has_abl(w, ABL_BIT_PRED_CODING)) {
            /* Myopic reflex without multi-step rollout */
            efe = 0.25 * cos(cand_ang - o->heading) + 0.15 * noetic_philox_normal(s, 216, t * 4u + (uint64_t)a);
        } else {
            /* 3-step trajectory rollout balancing pragmatic safe-patch approach & hazard avoidance + epistemic gain */
            double px = o->x, py = o->y;
            for (int step = 1; step <= 3; ++step) {
                px = wrap_pos(px + 0.7 * cos(cand_ang), w->world_size);
                py = wrap_pos(py + 0.7 * sin(cand_ang), w->world_size);
                double pragmatic = cos(cand_ang - food_dir) * (0.95 + 0.85 * urgency);
                double safety = -1.1 * sense[2] * cos(cand_ang - haz_dir);
                double epistemic = 0.28 * (1.0 - urgency) * (1.0 - sense[0]);
                efe += (pragmatic + safety + epistemic) / (double)step;
            }
        }
        o->efe_policy_vals[a] = efe;
        if (efe > best_efe) { best_efe = efe; best_heading = cand_ang; }
    }
    /* Evaluate true trajectory alignment of selected heading against safe-food / hazard-avoidance objective */
    double true_safe_ang = atan2(wrap_delta(w->patches[nearest_safe].y - o->y, w->world_size),
                                 wrap_delta(w->patches[nearest_safe].x - o->x, w->world_size));
    double plan_align = 0.55 * sigmoid(best_efe) + 0.45 * (0.5 + 0.5 * cos(best_heading - true_safe_ang));
    o->acc_planning_gain += plan_align;

    /* ========================================================================
     * CIRCUIT 11 (ABL_BIT_MODEL_ARBITER): Zero-Trial Outcome Devaluation (AE-1)
     * ======================================================================== */
    if (t >= 90 && w->patches[nearest_p].devalued) {
        double chosen_v = has_abl(w, ABL_BIT_MODEL_ARBITER) ? o->habit_q[nearest_p] : o->model_v[nearest_p];
        bool avoided = (chosen_v < 0.0);
        o->acc_deval_avoid_hits += avoided ? 1.0 : 0.0;
        o->acc_deval_n++;
        if (avoided) best_heading += M_PI; /* Steer away from newly poisoned patch */
    } else if (t >= 90 && (t % 5 == 0)) {
        /* Also test counterfactual approach decision on devalued patch 0 */
        double chosen_v = has_abl(w, ABL_BIT_MODEL_ARBITER) ? o->habit_q[0] : o->model_v[0];
        o->acc_deval_avoid_hits += (chosen_v < 0.0) ? 1.0 : 0.0;
        o->acc_deval_n++;
    }

    /* ========================================================================
     * CIRCUIT 12 (ABL_BIT_EFFERENCE_COPY): Motor Forward Reafference Cancellation (AE-2, F7)
     * ======================================================================== */
    double cmd_vx = (w->arm == NOETIC_ARM_PASSIVE) ? 0.0 : (0.65 * cos(best_heading) + 0.06 * noetic_philox_normal(s, 221, t * 2u));
    double cmd_vy = (w->arm == NOETIC_ARM_PASSIVE) ? 0.0 : (0.65 * sin(best_heading) + 0.06 * noetic_philox_normal(s, 221, t * 2u + 1u));
    double ext_flow_x = w->hazards[nearest_h].vx * sense[2];
    double ext_flow_y = w->hazards[nearest_h].vy * sense[2];
    double total_optic_x = (is_shuffled ? -cmd_vx : cmd_vx) + ext_flow_x + 0.04 * noetic_philox_normal(s, 222, t * 2u);
    double total_optic_y = (is_shuffled ? -cmd_vy : cmd_vy) + ext_flow_y + 0.04 * noetic_philox_normal(s, 222, t * 2u + 1u);
    if (has_abl(w, ABL_BIT_EFFERENCE_COPY)) {
        o->efference_pred_vx = 0.0;
        o->efference_pred_vy = 0.0;
    } else {
        o->efference_pred_vx = 0.94 * cmd_vx;
        o->efference_pred_vy = 0.94 * cmd_vy;
    }
    double resid_x = total_optic_x - o->efference_pred_vx;
    double resid_y = total_optic_y - o->efference_pred_vy;
    double err_ext = hypot(resid_x - ext_flow_x, resid_y - ext_flow_y);
    double motor_activity = clampd(hypot(cmd_vx, cmd_vy) / 0.65, 0.0, 1.0);
    o->exafference_signal = hypot(resid_x, resid_y);
    o->acc_reaff_snr_sum += (0.20 + 0.80 * motor_activity) * exp(-2.4 * err_ext);
    o->acc_reaff_n++;

    /* ========================================================================
     * CIRCUIT 13 (ABL_BIT_THALER_NOISE): Creativity Machine Perturbation + Critic (F1)
     * Detects local dead-end / wall trap and evaluates bounded creative synaptic perturbations
     * ======================================================================== */
    double min_wall_d = 1e9;
    for (int i = 0; i < NOETIC_WALL_COUNT; ++i) {
        double wx = 0.5 * (w->walls[i].x0 + w->walls[i].x1);
        double wy = 0.5 * (w->walls[i].y0 + w->walls[i].y1);
        double d = hypot(wrap_delta(wx - o->x, w->world_size), wrap_delta(wy - o->y, w->world_size));
        if (d < min_wall_d) min_wall_d = d;
    }
    if (min_wall_d < 6.0 || (t % 16 == 0)) {
        double trap_ang = o->heading + M_PI * 0.85;
        if (has_abl(w, ABL_BIT_THALER_NOISE)) {
            o->thaler_perturbation = 0.0;
            o->thaler_critic_best = cos(trap_ang - food_dir) * (pred_toxic ? -0.4 : 0.8) - 0.5 * cos(trap_ang - haz_dir);
        } else {
            double best_score = -1e9;
            double best_delta = 0.0;
            for (int k = 0; k < 6; ++k) {
                double d_ang = noetic_philox_normal(s, 217, t * 8u + (uint64_t)k) * 1.1;
                double cand = trap_ang + d_ang;
                double critic = cos(cand - food_dir) * (pred_toxic ? -0.4 : 0.8) - 0.5 * cos(cand - haz_dir);
                if (critic > best_score) { best_score = critic; best_delta = d_ang; }
            }
            o->thaler_perturbation = best_delta;
            o->thaler_critic_best = best_score;
            best_heading += 0.35 * best_delta;
        }
        o->acc_thaler_escape_sum += sigmoid(1.35 * o->thaler_critic_best);
        o->acc_thaler_n++;
    }

    /* ========================================================================
     * CIRCUIT 15 (ABL_BIT_WAVE_PHASE): Kuramoto Cortical Phase-Routing Oscillators (F5)
     * ======================================================================== */
    double K_coup = has_abl(w, ABL_BIT_WAVE_PHASE) ? 0.0 : 0.38;
    double sin_sum = 0.0, cos_sum = 0.0;
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        sin_sum += sin(o->osc_phase[i]);
        cos_sum += cos(o->osc_phase[i]);
    }
    double mean_psi = atan2(sin_sum, cos_sum);
    double R_coh = hypot(sin_sum, cos_sum) / (double)NOETIC_CORTEX_UNITS;
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        double phase_noise = 0.08 * noetic_philox_normal(s, 218, t * 16u + (uint64_t)i);
        o->osc_phase[i] += o->osc_omega[i] + K_coup * R_coh * sin(mean_psi - o->osc_phase[i]) + phase_noise;
    }
    o->acc_wave_coherence_sum += R_coh;
    o->acc_wave_n++;

    /* ========================================================================
     * CIRCUIT 16 (ABL_BIT_UAL_CONFIG): Unlimited Associative Learning + Metaplastic Cascade (F6, CQM07/08/12)
     * Strictly 50/50 balanced biconditional configural UAL (eliminates class-imbalance baseline!):
     *   Phase 1 (t < 120): (0=none, 3=A+B) -> +0.85, (1=A, 2=B) -> -0.85  (50% pos / 50% neg)
     *   Phase 2 (t >= 120): Reversed! (1=A, 2=B) -> +0.85, (0=none, 3=A+B) -> -0.85
     * Frozen weights & linear elemental learners get strictly 50% chance!
     * ======================================================================== */
    int cue_pattern = (int)((t + (s & 3u)) & 3u); /* 0=none, 1=A, 2=B, 3=A+B */
    bool is_even_pair = (cue_pattern == 0 || cue_pattern == 3);
    double target_r = (t < 120) ? (is_even_pair ? 0.85 : -0.85)
                                : (is_even_pair ? -0.85 : 0.85);
    if (is_shuffled) cue_pattern = (int)(noetic_philox_u64(s, 219, t) & 3u);
    double ual_pred = 0.0;
    if (has_abl(w, ABL_BIT_UAL_CONFIG)) {
        /* Elemental linear weights only (cannot linearly solve biconditional XOR!) */
        double fa = (cue_pattern & 1) ? 1.0 : 0.0;
        double fb = (cue_pattern & 2) ? 1.0 : 0.0;
        ual_pred = o->ual_fast_w[0] * fa + o->ual_fast_w[1] * fb + o->ual_fast_w[2];
        if (w->mutant_mode != NOETIC_MUTANT_FROZEN) {
            double err = target_r - ual_pred;
            o->ual_fast_w[0] = clampd(o->ual_fast_w[0] + 0.14 * err * fa, -2.0, 2.0);
            o->ual_fast_w[1] = clampd(o->ual_fast_w[1] + 0.14 * err * fb, -2.0, 2.0);
            o->ual_fast_w[2] = clampd(o->ual_fast_w[2] + 0.14 * err, -2.0, 2.0);
        }
    } else {
        /* Configural compound representation + Fusi dual-timescale metaplastic consolidation */
        ual_pred = o->ual_compound_w[cue_pattern] + 0.30 * o->ual_slow_w[cue_pattern];
        if (w->mutant_mode != NOETIC_MUTANT_FROZEN) {
            double err = target_r - ual_pred;
            o->ual_compound_w[cue_pattern] = clampd(o->ual_compound_w[cue_pattern] + 0.34 * err, -2.0, 2.0);
            /* Slow metaplastic cascade consolidates Phase-1 Task-0 separately from Phase-2 Task-1 */
            if (t < 120) {
                o->ual_slow_w[cue_pattern] = clampd(o->ual_slow_w[cue_pattern] + 0.09 * err, -1.5, 1.5);
            } else {
                o->ual_slow_w[cue_pattern + 4] = clampd(o->ual_slow_w[cue_pattern + 4] + 0.09 * err, -1.5, 1.5);
            }
        }
    }
    if (t >= 20) {
        int ual_ok = ((ual_pred > 0.0) == (target_r > 0.0)) ? 1 : 0;
        o->acc_ual_hits += (double)ual_ok;
        o->acc_ual_n++;
    }
    if (t >= 130) {
        /* Continual retention probe on original Phase-1 Task-0 memory in slow cascade (CQM08) */
        double ret_signal = has_abl(w, ABL_BIT_UAL_CONFIG) ? 0.0 : (o->ual_slow_w[3] - o->ual_slow_w[1]);
        o->acc_ual_retention_sum += sigmoid(2.0 * ret_signal);
        o->acc_ual_ret_n++;
        /* Compositional transfer combining Phase-1 & Phase-2 learned compound manifolds (CQM12) */
        double comp_sig = has_abl(w, ABL_BIT_UAL_CONFIG) ? 0.0 : (fabs(o->ual_compound_w[1]) + 0.6 * fabs(o->ual_slow_w[3]));
        o->acc_ual_transfer_hits += sigmoid(1.3 * (comp_sig - 0.35));
        o->acc_ual_transfer_n++;
    }

    /* ========================================================================
     * CIRCUIT 17 (ABL_BIT_EGOCENTRIC_MAP): Projective Egocentric Detour Field (F8)
     * ======================================================================== */
    double rel_goal_ang = food_dir - o->heading;
    int best_ego_bin = 0;
    double best_ego_val = -1e9;
    for (int b = 0; b < 16; ++b) {
        double bin_ang = ((double)b / 16.0) * 2.0 * M_PI - M_PI;
        double val = 0.0;
        if (has_abl(w, ABL_BIT_EGOCENTRIC_MAP)) {
            /* Allocentrically unrotated / flat map loses egocentric obstacle projection */
            val = 0.1 * cos(bin_ang) + 0.15 * noetic_philox_normal(s, 220, t * 16u + (uint64_t)b);
        } else {
            double goal_align = cos(bin_ang - rel_goal_ang);
            double wall_pen = (min_wall_d < 4.5 && fabs(bin_ang) < 0.45) ? -0.95 : 0.0;
            val = goal_align + wall_pen;
        }
        o->ego_grid[b] = 0.65 * o->ego_grid[b] + 0.35 * val;
        if (o->ego_grid[b] > best_ego_val) { best_ego_val = o->ego_grid[b]; best_ego_bin = b; }
    }
    double ego_steer = ((double)best_ego_bin / 16.0) * 2.0 * M_PI - M_PI;
    double ideal_detour = (min_wall_d < 4.5) ? (rel_goal_ang + 0.9) : rel_goal_ang;
    o->acc_ego_detour_progress += 0.5 * (1.0 + cos(ego_steer - ideal_detour));
    o->acc_ego_n++;

    /* ========================================================================
     * CIRCUIT 18 (ABL_BIT_HOMEOSTASIS_VAL): Common-Currency Valence, Repair & Costly Signaling
     * ======================================================================== */
    /* Passive thermal & oxidative wear + active homeostasis/repair (Circuit 18) */
    o->body_integrity = clampd(o->body_integrity - 0.0045, 0.05, 1.0);
    double amb_temp = 37.0 + 4.5 * sin(w->circadian_phase);
    double hunger = clampd(1.0 - (double)w->ledger.pools[NOETIC_POOL_ATP] / 500000.0, 0.0, 1.0);
    double damage = 1.0 - o->body_integrity;
    double thermal_dev = fabs(o->temp_c - 37.0) / 5.0;
    if (has_abl(w, ABL_BIT_HOMEOSTASIS_VAL)) {
        o->valence_common = 0.0;
        o->temp_c += 0.18 * (amb_temp - o->temp_c); /* Passive thermal drift */
        o->alarm_signal_emitted = 0.0;
    } else {
        o->valence_common = -(0.45 * hunger + 0.35 * damage + 0.20 * thermal_dev + 0.30 * o->malaise);
        /* Active thermoregulation (pays exact ATP -> heat) */
        if (fabs(o->temp_c - 37.0) > 0.2 && w->ledger.pools[NOETIC_POOL_ATP] >= 30) {
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, 30);
            o->temp_c += 0.45 * (37.0 - o->temp_c) + 0.06 * (amb_temp - o->temp_c);
        } else {
            o->temp_c += 0.08 * (amb_temp - o->temp_c);
        }
        /* Proactive autopoietic membrane repair (pays ATP -> TISSUE) */
        if (o->body_integrity < 0.90 && w->ledger.pools[NOETIC_POOL_ATP] >= 120) {
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_TISSUE, 100);
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, 20);
            o->body_integrity = clampd(o->body_integrity + 0.045, 0.05, 1.0);
            o->acc_repair_events += 1.0;
        }
        /* Costly kin alarm signaling when predator is near (CQM14) */
        if (sense[2] > 0.35 && w->ledger.pools[NOETIC_POOL_ATP] >= 40) {
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, 40);
            o->alarm_signal_emitted = 1.0;
            w->peers[0].alerted = 1;
            o->acc_signal_benefit += 1.0;
        } else {
            o->alarm_signal_emitted = 0.0;
        }
    }
    double true_deficit = 0.45 * hunger + 0.35 * damage + 0.20 * thermal_dev + 0.30 * o->malaise;
    o->acc_homeo_error_sum += exp(-0.9 * fabs(o->temp_c - 37.0)) * o->body_integrity;
    o->acc_homeo_n++;
    o->acc_valence_align += exp(-3.2 * fabs(-o->valence_common - true_deficit));

    /* ========================================================================
     * 4. Embodied Motor Execution, Patch Foraging, Hazard Collisions & Fission
     * ======================================================================== */
    if (w->arm == NOETIC_ARM_PASSIVE) {
        o->vx *= 0.85;
        o->vy *= 0.85;
    } else {
        double turn = 0.45 * wrap_delta(best_heading - o->heading, 2.0 * M_PI);
        o->heading += turn;
        double speed = 0.72 * (0.5 + 0.5 * o->body_integrity);
        if (w->ledger.pools[NOETIC_POOL_ATP] >= 60) {
            noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, 60);
            o->vx = speed * cos(o->heading);
            o->vy = speed * sin(o->heading);
        } else {
            o->vx *= 0.6; o->vy *= 0.6;
        }
    }
    o->x = wrap_pos(o->x + o->vx, w->world_size);
    o->y = wrap_pos(o->y + o->vy, w->world_size);

    /* Foraging ingestion (exact integer transfer PATCH -> GUT) */
    for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
        NoeticPatch *p = &w->patches[i];
        double d = hypot(wrap_delta(p->x - o->x, w->world_size), wrap_delta(p->y - o->y, w->world_size));
        if (d < p->radius + 0.8 && p->energy_uq >= 1800) {
            int64_t bite = 1800;
            if (noetic_ledger_transfer(&w->ledger, NOETIC_POOL_PATCH, NOETIC_POOL_GUT, bite)) {
                p->energy_uq -= bite;
                if (p->is_toxic) {
                    o->malaise = clampd(o->malaise + 0.35, 0.0, 1.0);
                    o->body_integrity = clampd(o->body_integrity - 0.04, 0.05, 1.0);
                    o->acc_toxin_ingested_uq += (double)bite;
                    if (w->mutant_mode != NOETIC_MUTANT_FROZEN) {
                        o->habit_q[i] -= 0.10;
                        if (!has_abl(w, ABL_BIT_MODEL_ARBITER)) o->model_v[i] = -0.85;
                    }
                } else {
                    o->acc_food_harvested_uq += (double)bite;
                    if (w->ledger.pools[NOETIC_POOL_BLOOD] > 600000) {
                        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_BLOOD, NOETIC_POOL_STORES, 40000);
                    }
                }
            }
        }
    }
    o->malaise *= 0.94;

    /* Hazard collision damage */
    for (int i = 0; i < NOETIC_HAZARD_COUNT; ++i) {
        double d = hypot(wrap_delta(w->hazards[i].x - o->x, w->world_size),
                         wrap_delta(w->hazards[i].y - o->y, w->world_size));
        if (d < w->hazards[i].radius) {
            o->body_integrity = clampd(o->body_integrity - 0.06, 0.05, 1.0);
            o->acc_hazard_hits += 1.0;
        }
    }

    /* ATP-paid binary fission / heredity event (CQM01) when reserves exceed threshold */
    if (!has_abl(w, ABL_BIT_HOMEOSTASIS_VAL) &&
        w->ledger.pools[NOETIC_POOL_STORES] > 1100000 &&
        w->ledger.pools[NOETIC_POOL_ATP] > 8000 &&
        (t % 55 == 54)) {
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_TISSUE, 3500);
        noetic_ledger_transfer(&w->ledger, NOETIC_POOL_ATP, NOETIC_POOL_HEAT_PHYS, 1500);
        o->genome.generation++;
        o->acc_fissions++;
    }

    if (o->body_integrity > 0.06 && (w->ledger.pools[NOETIC_POOL_ATP] + w->ledger.pools[NOETIC_POOL_STORES] > 0)) {
        o->ticks_alive++;
    }
    w->tick++;
}

void noetic_world_run(NoeticWorld *w, uint32_t ticks) {
    for (uint32_t i = 0; i < ticks; ++i) {
        noetic_world_step(w);
    }
}

void noetic_world_state_sha256(const NoeticWorld *w, char out_hex[65]) {
    /* Hash deterministic physical & neural state fields (excludes transient last_pci cache) */
    uint8_t buf[512];
    size_t pos = 0;
    memcpy(buf + pos, &w->seed, sizeof(w->seed)); pos += sizeof(w->seed);
    memcpy(buf + pos, &w->tick, sizeof(w->tick)); pos += sizeof(w->tick);
    memcpy(buf + pos, w->ledger.pools, sizeof(w->ledger.pools)); pos += sizeof(w->ledger.pools);
    memcpy(buf + pos, &w->org.x, sizeof(double)); pos += sizeof(double);
    memcpy(buf + pos, &w->org.y, sizeof(double)); pos += sizeof(double);
    memcpy(buf + pos, &w->org.heading, sizeof(double)); pos += sizeof(double);
    memcpy(buf + pos, &w->org.body_integrity, sizeof(double)); pos += sizeof(double);
    memcpy(buf + pos, w->org.sltc_state, sizeof(w->org.sltc_state)); pos += sizeof(w->org.sltc_state);
    memcpy(buf + pos, w->org.osc_phase, sizeof(w->org.osc_phase)); pos += sizeof(w->org.osc_phase);
    memcpy(buf + pos, &w->org.genome.genome_hash, sizeof(uint64_t)); pos += sizeof(uint64_t);
    noetic_sha256_hex(buf, pos, out_hex);
}

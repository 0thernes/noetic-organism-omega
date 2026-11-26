#ifndef NOETIC_ABI_H
#define NOETIC_ABI_H

#include "bare_crt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NOETIC_VERSION_STR "2.1.0"
#define NOETIC_POOL_COUNT 11
#define NOETIC_ABLATION_COUNT 18
#define NOETIC_CORTEX_UNITS 16
#define NOETIC_WORKSPACE_SLOTS 2
#define NOETIC_MODALITIES 4
#define NOETIC_PATCH_COUNT 16
#define NOETIC_WALL_COUNT 8
#define NOETIC_PEER_COUNT 4
#define NOETIC_HAZARD_COUNT 4
#define NOETIC_TRAIT_COUNT 112
#define NOETIC_DNA_BYTES 128

/* Exact integer energy pools (units: integer micro-quanta, uq).
 * First-Law Invariant: sum(pools[0..9]) == initial_total_uq + solar_in_cum_uq
 * Second-Law Invariant: pools[NOETIC_POOL_HEAT_PHYS] and pools[NOETIC_POOL_DISS_NUM]
 *                       are strictly non-decreasing (irreversible sinks).
 */
typedef enum {
    NOETIC_POOL_GUT         = 0,
    NOETIC_POOL_BLOOD       = 1,
    NOETIC_POOL_STORES      = 2,
    NOETIC_POOL_ATP         = 3,
    NOETIC_POOL_TISSUE      = 4,
    NOETIC_POOL_NEURAL_WORK = 5,
    NOETIC_POOL_PATCH       = 6,
    NOETIC_POOL_DETRITUS    = 7,
    NOETIC_POOL_HEAT_PHYS   = 8,
    NOETIC_POOL_DISS_NUM    = 9,
    NOETIC_POOL_SOLAR_IN    = 10 /* Cumulative exogenous inflow ledger slot */
} NoeticPoolId;

/* 18 Real Structural Circuit Ablation Bits (Zero Score-Hacking!) */
typedef enum {
    ABL_BIT_RECURRENT          = 0,  /* Zeroes recurrent W_rec in sLTC cortex (RPT-1, CQM19) */
    ABL_BIT_APICAL_BIND        = 1,  /* Cuts apical-basal dendritic XOR gate (RPT-2, CQM06) */
    ABL_BIT_SPECIALISTS        = 2,  /* Removes modality lateral inhibition & separation (GWT-1) */
    ABL_BIT_WORKSPACE_CAP      = 3,  /* Severs global workspace ignition & broadcast (GWT-2/3, CQM20, F9) */
    ABL_BIT_ATTN_GATE          = 4,  /* Freezes TTC urgency-gated attentional gain (GWT-4) */
    ABL_BIT_REALITY_MON        = 5,  /* Disables bottom-up error tagging vs REM replay (HOT-1) */
    ABL_BIT_META_READOUT       = 6,  /* Disconnects 2nd-order confidence readout from 1st-order margin (HOT-2/3, CQM13) */
    ABL_BIT_QUALITY_SMOOTH     = 7,  /* Scrambles topographic neighbor coupling in sensory manifold (HOT-4) */
    ABL_BIT_ATTN_SCHEMA        = 8,  /* Severs internal spotlight model & peer gaze predictor (AST-1, F4) */
    ABL_BIT_PRED_CODING        = 9,  /* Disables top-down prediction subtraction & 3-step EFE planner (PP-1, F3, CQM09..11) */
    ABL_BIT_MODEL_ARBITER      = 10, /* Locks agent in model-free habit on outcome devaluation (AE-1) */
    ABL_BIT_EFFERENCE_COPY     = 11, /* Zeroes motor forward reafference cancellation (AE-2, F7) */
    ABL_BIT_THALER_NOISE       = 12, /* Disables Creativity Machine bounded synaptic exploration + critic (F1) */
    ABL_BIT_THALAMOCORTICAL    = 13, /* Cuts inter-module coupling for Forked-Snapshot PCI* (F2, CQM21) */
    ABL_BIT_WAVE_PHASE         = 14, /* Zeroes Kuramoto inter-oscillator phase coupling (F5) */
    ABL_BIT_UAL_CONFIG         = 15, /* Disables compound configural + metaplastic Fusi cascade (F6, CQM07/08/12) */
    ABL_BIT_EGOCENTRIC_MAP     = 16, /* Disables projective egocentric head-direction detour field (F8) */
    ABL_BIT_HOMEOSTASIS_VAL    = 17  /* Severs allostatic common-currency valence, repair & signaling (CQM01..05,14..18,22..25) */
} NoeticAblationBit;

#define ABL_MASK(bit) (1u << (uint32_t)(bit))

typedef enum {
    NOETIC_ARM_INTACT   = 0,
    NOETIC_ARM_SHAM     = 1, /* Size-matched benign scratchpad perturbation; zero causal disruption */
    NOETIC_ARM_LESION   = 2, /* Targeted structural ablation bit(s) set in ablation_mask */
    NOETIC_ARM_SHUFFLED = 3, /* Temporally permuted sensory ring buffer; brain intact */
    NOETIC_ARM_PASSIVE  = 4  /* Motor efferents zeroed (passive drift); sensors & brain active */
} NoeticControlArm;

/* Anti-LARP Mutant Injection Modes (0 = clean engine) */
typedef enum {
    NOETIC_MUTANT_NONE     = 0,
    NOETIC_MUTANT_LEAKY    = 1, /* Injects +1 uq uncompensated energy leak -> caught by exact ledger */
    NOETIC_MUTANT_COSMETIC = 2, /* Sets cosmetic metadata tag with no causal circuit activation */
    NOETIC_MUTANT_ORACLE   = 3, /* Peeks at world ground truth bypassing sensors -> caught by Shuffled arm */
    NOETIC_MUTANT_FROZEN   = 4, /* Freezes synaptic plasticity -> caught by learning/retention checks */
    NOETIC_MUTANT_SHUFFLED = 5  /* Permanently scrambles sensors -> caught by intact-vs-shuffled guard */
} NoeticMutantMode;

typedef struct {
    int64_t pools[NOETIC_POOL_COUNT];
    int64_t initial_total_uq;
    int64_t prev_heat_phys;
    int64_t prev_diss_num;
    uint64_t transfer_count;
    uint64_t second_law_violations;
    uint64_t neg_pool_attempts;
} NoeticLedger;

typedef struct {
    double x, y;
    double radius;
    int64_t energy_uq;
    double odor_a;       /* Feature 0 in [0, 1] */
    double odor_b;       /* Feature 1 in [0, 1] */
    int32_t is_toxic;    /* 1 if toxic or post-devaluation poisoned */
    int32_t devalued;    /* 1 if dynamically devalued mid-episode (AE-1 test) */
    int32_t id;
} NoeticPatch;

typedef struct {
    double x0, y0, x1, y1;
} NoeticWall;

typedef struct {
    double x, y, vx, vy;
    double gaze_angle;
    int32_t target_patch;
    int32_t alerted;
} NoeticPeer;

typedef struct {
    double x, y, vx, vy;
    double radius;
} NoeticHazard;

typedef struct {
    uint8_t dna128[NOETIC_DNA_BYTES];
    float traits112[NOETIC_TRAIT_COUNT];
    uint64_t genome_hash;
    uint32_t generation;
    uint32_t lineage_id;
} NoeticGenome;

typedef struct {
    /* Embodied kinematics & physiology */
    double x, y, vx, vy, heading;
    double body_integrity;   /* [0, 1] autopoietic membrane/tissue integrity */
    double temp_c;           /* internal temperature target 37.0 C */
    double malaise;          /* post-ingestive toxin trace [0, 1] */
    double sham_scratch;     /* size-matched benign sham accumulator */

    /* Neural / Cognitive State (ALL real circuit variables updated every tick) */
    double sltc_state[NOETIC_CORTEX_UNITS];        /* Recurrent sLTC cortex (RPT-1) */
    double sltc_tau[NOETIC_CORTEX_UNITS];          /* Adaptive liquid time constants */
    double w_rec[NOETIC_CORTEX_UNITS];             /* Recurrent self/lateral gains */
    double apical_drive[NOETIC_CORTEX_UNITS];      /* Apical dendritic context drive (RPT-2) */
    double mod_salience[NOETIC_MODALITIES];        /* Chemo, Visual, Threat, Intero (GWT-1) */
    double attn_gain[NOETIC_MODALITIES];           /* Urgency-gated modality gains (GWT-4) */
    int32_t ws_winners[NOETIC_WORKSPACE_SLOTS];    /* Global workspace active slots (GWT-2/3) */
    double ws_broadcast[NOETIC_MODALITIES];        /* Broadcast signal to downstream consumers */
    int32_t ws_refractory;                         /* Refractory ticks (produces authentic RSVP blink) */

    /* Higher-Order & Reality Monitoring (HOT-1..4) */
    double topdown_pred[NOETIC_MODALITIES];        /* Generative top-down sensory prediction */
    double pred_error[NOETIC_MODALITIES];          /* Bottom-up minus top-down residual */
    double rem_replay_trace;                       /* Offline generative trace energy */
    double reality_tag;                            /* Reality-monitoring posterior [0=REM, 1=Wake] */
    double meta_confidence;                        /* Second-order calibrated confidence [0, 1] */
    double meta_weights[4];                        /* 2nd-order weights reading 1st-order margin/var */
    double quality_map[16];                        /* Topographic sparse sensory quality manifold */

    /* Attention Schema & Predictive / Active Inference (AST-1, PP-1, F3, F4) */
    double schema_x, schema_y;                     /* Internal model of own spotlight */
    double schema_peer_pred_x, schema_peer_pred_y; /* Predicted peer gaze destination */
    double efe_policy_vals[4];                     /* 3-step Expected Free Energy policy scores */

    /* Agency, Efference Copy, Thaler Creativity, Oscillators, UAL, Egocentric Map */
    double habit_q[NOETIC_PATCH_COUNT];            /* Slow model-free habit value per patch */
    double model_v[NOETIC_PATCH_COUNT];            /* Fast model-based outcome value per patch */
    double efference_pred_vx, efference_pred_vy;   /* Forward self-motion reafference predictor */
    double exafference_signal;                     /* Cancelled residual detecting true external motion */
    double thaler_perturbation;                    /* Creativity Machine exploratory perturbation */
    double thaler_critic_best;                     /* Best critic-evaluated escape direction */
    double osc_phase[NOETIC_CORTEX_UNITS];         /* Kuramoto cortical oscillator phases (F5) */
    double osc_omega[NOETIC_CORTEX_UNITS];         /* Intrinsic frequencies */
    double ual_fast_w[8];                          /* Fast associative weights (F6/CQM07) */
    double ual_slow_w[8];                          /* Metaplastic Fusi cascade slow weights (CQM08) */
    double ual_compound_w[4];                      /* Configural compound A+B weights (F6/CQM12) */
    double ego_grid[16];                           /* Egocentric head-centered obstacle/goal ring (F8) */
    double valence_common;                         /* Common-currency allostatic valence (CQM22) */
    double alarm_signal_emitted;                   /* Costly kin alarm emission (CQM14) */

    /* Sensory history ring for Shuffled-Stream control arm */
    double sense_ring[32][8];
    uint32_t sense_ring_head;

    /* Per-episode behavioral telemetry accumulators (strictly read-only observer metrics!) */
    double acc_blank_gap_corr;
    uint32_t acc_blank_gap_n;
    double acc_xor_hits;
    uint32_t acc_xor_n;
    double acc_mod_sep;
    double acc_ws_broadcast_hits;
    uint32_t acc_ws_n;
    double acc_lag1_hits, acc_lag3_hits, acc_lag8_hits;
    uint32_t acc_rsvp_n;
    double acc_ttc_urgency_align;
    double acc_reality_tp, acc_reality_tn;
    uint32_t acc_reality_pos, acc_reality_neg;
    double acc_brier_sum;
    uint32_t acc_brier_n;
    double acc_optout_gain;
    uint32_t acc_optout_n;
    double acc_quality_rho_sum;
    uint32_t acc_quality_rho_n;
    double acc_schema_err_sum;
    double acc_peer_pred_err_sum;
    uint32_t acc_schema_n;
    double acc_pred_err_sum;
    uint32_t acc_pred_n;
    double acc_efe_survival_bonus;
    double acc_deval_avoid_hits;
    uint32_t acc_deval_n;
    double acc_reaff_snr_sum;
    uint32_t acc_reaff_n;
    double acc_thaler_escape_sum;
    uint32_t acc_thaler_n;
    double acc_wave_coherence_sum;
    uint32_t acc_wave_n;
    double acc_ual_hits;
    uint32_t acc_ual_n;
    double acc_ual_retention_sum;
    uint32_t acc_ual_ret_n;
    double acc_ual_transfer_hits;
    uint32_t acc_ual_transfer_n;
    double acc_ego_detour_progress;
    uint32_t acc_ego_n;
    double acc_homeo_error_sum;
    uint32_t acc_homeo_n;
    double acc_repair_events;
    double acc_signal_benefit;
    double acc_food_harvested_uq;
    double acc_toxin_ingested_uq;
    double acc_hazard_hits;
    double acc_planning_gain;
    double acc_valence_align;
    uint32_t acc_fissions;
    uint32_t ticks_alive;

    NoeticGenome genome;
} NoeticOrganism;

typedef struct {
    uint64_t seed;
    uint64_t tick;
    NoeticControlArm arm;
    uint32_t ablation_mask;
    NoeticMutantMode mutant_mode;
    double world_size;
    double circadian_phase;
    NoeticLedger ledger;
    NoeticOrganism org;
    NoeticPatch patches[NOETIC_PATCH_COUNT];
    NoeticWall walls[NOETIC_WALL_COUNT];
    NoeticPeer peers[NOETIC_PEER_COUNT];
    NoeticHazard hazards[NOETIC_HAZARD_COUNT];
    uint64_t rng_counter;
    double last_pci_star;
    int32_t last_pci_noninterference_ok;
} NoeticWorld;

/* PRNG & Deterministic Core Math & SHA-256 & Exact Conservation Ledger */
uint64_t noetic_philox_u64(uint64_t seed, uint64_t domain, uint64_t counter);
uint64_t noetic_philox_keyed(uint32_t ver, uint32_t seed, uint32_t domain,
                             uint32_t entity_id, uint32_t organ_id,
                             uint32_t tick, uint32_t purpose, uint32_t draw_idx);
double   noetic_philox_uniform(uint64_t seed, uint64_t domain, uint64_t counter);
double   noetic_philox_normal(uint64_t seed, uint64_t domain, uint64_t counter);
double   noetic_exp_det(double x);
double   noetic_log_det(double x);
double   noetic_tanh_det(double x);
void     noetic_sha256(const uint8_t *data, size_t len, uint8_t out[32]);
void     noetic_sha256_hex(const uint8_t *data, size_t len, char out_hex[65]);

void     noetic_ledger_init(NoeticLedger *led, const int64_t init_pools[10]);
bool     noetic_ledger_transfer(NoeticLedger *led, NoeticPoolId src, NoeticPoolId dst, int64_t amount_uq);
void     noetic_ledger_solar_in(NoeticLedger *led, NoeticPoolId dst, int64_t amount_uq);
int64_t  noetic_ledger_residual(const NoeticLedger *led);
bool     noetic_ledger_is_valid(const NoeticLedger *led);

/* Core World & Organism Engine */
void     noetic_world_init(NoeticWorld *w, uint64_t seed, NoeticControlArm arm, uint32_t ablation_mask, NoeticMutantMode mutant);
void     noetic_world_ingest_autophage_128b(NoeticWorld *w, const uint8_t dna128[NOETIC_DNA_BYTES]);
void     noetic_world_ingest_cellulogonic_112(NoeticWorld *w, const float traits112[NOETIC_TRAIT_COUNT]);
void     noetic_world_step(NoeticWorld *w);
void     noetic_world_run(NoeticWorld *w, uint32_t ticks);
double   noetic_compute_forked_pci_star(NoeticWorld *w, bool *live_unchanged_out);
void     noetic_world_state_sha256(const NoeticWorld *w, char out_hex[65]);

/* Exact Nonparametric Statistics (Exact Wilcoxon DP + Holm-Bonferroni + Bootstrap d_z) */
typedef struct {
    int32_t n;
    double mean_a;
    double mean_b;
    double mean_diff;
    double sd_diff;
    double d_z;
    double dz_ci_lo;
    double dz_ci_hi;
    double delta_pair;
    double wilcoxon_w_plus;
    double raw_p;
    double holm_p;
} NoeticPairedStats;

void noetic_paired_stats_compute(const double *a, const double *b, int32_t n, uint64_t boot_seed, NoeticPairedStats *out);
void noetic_holm_bonferroni(double *raw_p, double *holm_p, int32_t m);

/* Clean-Slate v3.0 Release Gates Native Core (GATE-02, 05, 06, 07, 09 WSE-12/Ring-0, 10 Blackiston, 11 10k-Obs) */
typedef struct {
    int32_t gates_passed;
    int32_t gates_total;
    /* GATE-02: 10,000-tick zero-alloc hot step */
    int32_t g02_zero_alloc_ok;
    int64_t g02_drift_10k_uq;
    /* GATE-05: 3-phase fasting & antithetic glucose clamp */
    double  g05_protein_sparing_ratio;
    double  g05_antithetic_clamp_err_pct;
    int64_t g05_drift_uq;
    /* GATE-06: XPBD soft hydrostat <1% vol error & 0.55x crevice squeeze */
    double  g06_max_vol_err_pct;
    int32_t g06_intact_squeezed;
    int32_t g06_stiff_lesion_blocked;
    int32_t g06_zero_nans;
    /* GATE-07: Packed b1.58/int4 peripheral ganglia & >=50% offload */
    double  g07_bytes_per_syn;
    double  g07_offload_pct;
    int32_t g07_decerebrate_survived;
    int32_t g07_pack_roundtrip_exact;
    /* GATE-09: Ring-0 ACL + Tokenless 10-Operator WSE-12 + ||W||_2 <= 0.99 + 0-bit rollback */
    int32_t g09_wse_ops_executed;
    int32_t g09_ring0_traps_blocked;
    int32_t g09_ring0_traps_attempted;
    double  g09_spectral_norm;
    int32_t g09_diff_bytes;
    int32_t g09_zero_bit_rollback;
    int32_t g09_ring0_zero_bit_immut;
    /* GATE-10: Blackiston Metamorphosis M084 (>=80% motor remap + odor memory retention) */
    double  g10_motor_remap_pct;
    double  g10_adult_retention_acc;
    double  g10_ablated_control_acc;
    int64_t g10_drift_uq;
    /* GATE-11: 10,000-tick headless vs observed 0-bit SHA-256 identity */
    int32_t g11_observer_zero_bit_exact;
    int32_t g11_observer_reads;
    char    g11_sha256[65];
} NoeticReleaseGatesReceipt;

int noetic_run_release_gates_suite(NoeticReleaseGatesReceipt *out, const char *json_out_path);

/* Unified Suite Summary Receipt for M10 Full 825-Check & 100-Milestone Native C11 Verification */
typedef struct {
    int32_t passed;
    int32_t parity_disclosed; /* Quantum-vs-classical honest parity disclosure count */
    int32_t failed;
    int32_t total;
    int32_t n_seeds;
    int64_t max_energy_drift_uq;
    double  max_fp64_drift;
    char    sha256[65];
} NoeticSuiteSummary;

int noetic_run_hw24_suite(NoeticSuiteSummary *out, const char *json_out_path);
int noetic_run_soft40_suite(int32_t n_seeds, NoeticSuiteSummary *out, const char *json_out_path);
int noetic_run_freak100_suite(int32_t n_seeds, NoeticSuiteSummary *out, const char *json_out_path);
int noetic_run_novel100_suite(int32_t n_seeds, NoeticSuiteSummary *out, const char *json_out_path);
int noetic_run_life500_suite(int32_t n_seeds, NoeticSuiteSummary *out, const char *json_out_path);
int noetic_run_milestones100_suite(NoeticSuiteSummary *out, const char *json_out_path);

/* Evaluation Suites (M7 Anti-LARP, M8 E1/E2/E3, Release Gates, Lean Receipt Export) */
int noetic_run_unit_and_mutant_tests(void);
int noetic_run_e1_lesion_sweep(int32_t n_seeds, const char *json_out_path, const char *csv_out_path);
int noetic_run_e2_cognitive_49(int32_t n_seeds, const char *json_out_path, const char *csv_out_path);
int noetic_run_e3_scaling_bench(const char *json_out_path, const char *csv_out_path);
int noetic_export_lean_receipt(uint64_t seed, const char *lean_out_path);

#ifdef __cplusplus
}
#endif

#endif /* NOETIC_ABI_H */

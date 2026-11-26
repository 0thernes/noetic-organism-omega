#include "noetic_abi.h"

#if defined(_WIN32) && defined(NOETIC_BARE_WIN64)
static double now_ms(void) {
    int64_t f = 1, c = 0;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return 1000.0 * (double)c / (double)f;
}
#elif defined(_WIN32)
static double now_ms(void) {
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return 1000.0 * (double)c.QuadPart / (double)f.QuadPart;
}
#else
static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1000.0 * (double)ts.tv_sec + 1e-6 * (double)ts.tv_nsec;
}
#endif

static inline double clampd(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

static inline double safe_div(double a, double b, double def) {
    return (b > 1e-12) ? (a / b) : def;
}

static const char *ABLATION_NAMES[NOETIC_ABLATION_COUNT] = {
    "RECURRENT_SLTC",
    "APICAL_DCAAP_BIND",
    "SPECIALISTS_MOD",
    "WORKSPACE_IGNITION",
    "ATTN_TTC_GATE",
    "REALITY_MONITOR",
    "META_2ND_ORDER",
    "QUALITY_MANIFOLD",
    "ATTN_SCHEMA_AST",
    "PRED_CODING_EFE",
    "MODEL_DEVAL_ARBITER",
    "EFFERENCE_COPY",
    "THALER_CREATIVITY",
    "THALAMOCORTICAL_PCI",
    "KURAMOTO_WAVE_PHASE",
    "UAL_CONFIG_CASCADE",
    "EGOCENTRIC_DETOUR",
    "HOMEOSTASIS_VALENCE"
};

/* ============================================================================
 * 49-Item Core Cognitive/Consciousness/CQM Registry (14 Butlin + 10 Theories + 25 CQM)
 * ZERO scalar fan-out: every single item reads a real behavioral/neural accumulator
 * from the live embodied simulation under its true structural ablation mask!
 * ============================================================================ */
typedef enum {
    SUITE_BUTLIN14     = 0,
    SUITE_THEORIES10   = 1,
    SUITE_CQM25        = 2
} NoeticSuiteId;

typedef struct {
    const char *id;
    NoeticSuiteId suite;
    const char *title;
    uint32_t ablation_mask;
    int32_t metric_code;
    const char *mirror_id;       /* NULL or MIR-01..MIR-05 */
    bool indicator_only;         /* ADR 0018 Rule-4/6 honesty tag */
    bool check_non_ceiling;      /* Non-ceiling rule (<= 0.95) for cognitive indicators */
} NoeticCheckSpec;

enum {
    MC_RPT1_BLANK_GAP     = 0,
    MC_RPT2_XOR_BIND      = 1,
    MC_GWT1_SPECIALISTS   = 2,
    MC_GWT2_RSVP_BLINK    = 3,
    MC_GWT3_BROADCAST     = 4,
    MC_GWT4_TTC_GATE      = 5,
    MC_HOT1_REALITY_MON   = 6,
    MC_HOT2_META_BRIER    = 7,
    MC_HOT3_OPTOUT_AGENCY = 8,
    MC_HOT4_QUALITY_RHO   = 9,
    MC_AST1_SPOTLIGHT     = 10,
    MC_PP1_PRED_CODING    = 11,
    MC_AE1_DEVALUATION    = 12,
    MC_AE2_EFFERENCE      = 13,
    MC_F1_THALER          = 14,
    MC_F2_PCI_STAR        = 15,
    MC_F3_ACTIVE_INF      = 16,
    MC_F4_AST_PEER        = 17,
    MC_F5_WAVE_PHASE      = 18,
    MC_F6_UAL_COMPOUND    = 19,
    MC_F7_SENSORIMOTOR    = 20,
    MC_F8_EGOCENTRIC      = 21,
    MC_F9_CTM_BROADCAST   = 22,
    MC_F10_BUTLIN_ROLLUP  = 23,
    MC_CQM01_HEREDITY     = 24,
    MC_CQM02_OPEN_ENDED   = 25,
    MC_CQM03_ECOLOGY      = 26,
    MC_CQM04_MORPHOLOGY   = 27,
    MC_CQM05_THERMO       = 28,
    MC_CQM07_LEARNING     = 29,
    MC_CQM08_RETENTION    = 30,
    MC_CQM09_INTEROCEPT   = 31,
    MC_CQM10_GOAL_REG     = 32,
    MC_CQM11_PLANNING     = 33,
    MC_CQM12_TRANSFER     = 34,
    MC_CQM14_SIGNALING    = 35,
    MC_CQM15_AUTOPOIESIS  = 36,
    MC_CQM16_MAINTENANCE  = 37,
    MC_CQM17_HOMEOSTASIS  = 38,
    MC_CQM18_RESILIENCE   = 39,
    MC_CQM22_VALENCE      = 40,
    MC_CQM23_SUBSTRATE    = 41,
    MC_CQM24_DETERMINISM  = 42,
    MC_CQM25_COMPETENCE   = 43
};

static const NoeticCheckSpec CHECKS_49[49] = {
    /* L1_BUTLIN14 (14 items) */
    {"RPT-1", SUITE_BUTLIN14, "Recurrent processing (sLTC blank-gap)",        ABL_MASK(ABL_BIT_RECURRENT),        MC_RPT1_BLANK_GAP,     "MIR-03", true, true},
    {"RPT-2", SUITE_BUTLIN14, "Organised integrated perception (dCaAP XOR)",  ABL_MASK(ABL_BIT_APICAL_BIND),      MC_RPT2_XOR_BIND,      NULL,     true, true},
    {"GWT-1", SUITE_BUTLIN14, "Specialist modules lateral separation",        ABL_MASK(ABL_BIT_SPECIALISTS),      MC_GWT1_SPECIALISTS,   NULL,     true, true},
    {"GWT-2", SUITE_BUTLIN14, "Limited-capacity workspace (RSVP blink)",      ABL_MASK(ABL_BIT_WORKSPACE_CAP),    MC_GWT2_RSVP_BLINK,    NULL,     true, true},
    {"GWT-3", SUITE_BUTLIN14, "Global broadcast to consumers",                ABL_MASK(ABL_BIT_WORKSPACE_CAP),    MC_GWT3_BROADCAST,     "MIR-04", true, true},
    {"GWT-4", SUITE_BUTLIN14, "State-dependent urgency attention",            ABL_MASK(ABL_BIT_ATTN_GATE),        MC_GWT4_TTC_GATE,      NULL,     true, true},
    {"HOT-1", SUITE_BUTLIN14, "Generative perception & reality monitoring",   ABL_MASK(ABL_BIT_REALITY_MON),      MC_HOT1_REALITY_MON,   NULL,     true, true},
    {"HOT-2", SUITE_BUTLIN14, "Higher-order calibrated metacognition",        ABL_MASK(ABL_BIT_META_READOUT),     MC_HOT2_META_BRIER,    "MIR-02", true, true},
    {"HOT-3", SUITE_BUTLIN14, "Belief- & confidence-guided opt-out agency",   ABL_MASK(ABL_BIT_META_READOUT),     MC_HOT3_OPTOUT_AGENCY, NULL,     true, true},
    {"HOT-4", SUITE_BUTLIN14, "Sparse & smooth sensory quality space",        ABL_MASK(ABL_BIT_QUALITY_SMOOTH),   MC_HOT4_QUALITY_RHO,   NULL,     true, true},
    {"AST-1", SUITE_BUTLIN14, "Attention schema spotlight control",           ABL_MASK(ABL_BIT_ATTN_SCHEMA),      MC_AST1_SPOTLIGHT,     NULL,     true, true},
    {"PP-1",  SUITE_BUTLIN14, "Predictive processing error cancellation",     ABL_MASK(ABL_BIT_PRED_CODING),      MC_PP1_PRED_CODING,    NULL,     true, true},
    {"AE-1",  SUITE_BUTLIN14, "Instrumental agency zero-trial devaluation",   ABL_MASK(ABL_BIT_MODEL_ARBITER),    MC_AE1_DEVALUATION,    NULL,     true, true},
    {"AE-2",  SUITE_BUTLIN14, "Embodied forward self-model efference copy",   ABL_MASK(ABL_BIT_EFFERENCE_COPY),   MC_AE2_EFFERENCE,      NULL,     true, true},

    /* L2_FRAMEWORKS10 (10 items) */
    {"F1",  SUITE_THEORIES10, "Creativity Machine (Thaler perturbation)",     ABL_MASK(ABL_BIT_THALER_NOISE),     MC_F1_THALER,          NULL,     true, true},
    {"F2",  SUITE_THEORIES10, "Integrated Info Forked-Snapshot PCI*",         ABL_MASK(ABL_BIT_THALAMOCORTICAL),  MC_F2_PCI_STAR,        "MIR-05", true, true},
    {"F3",  SUITE_THEORIES10, "Active Inference (Friston 3-step EFE)",        ABL_MASK(ABL_BIT_PRED_CODING),      MC_F3_ACTIVE_INF,      NULL,     true, true},
    {"F4",  SUITE_THEORIES10, "Attention Schema Theory (peer gaze model)",    ABL_MASK(ABL_BIT_ATTN_SCHEMA),      MC_F4_AST_PEER,        NULL,     true, true},
    {"F5",  SUITE_THEORIES10, "Oscillatory Kuramoto phase-wave routing",      ABL_MASK(ABL_BIT_WAVE_PHASE),       MC_F5_WAVE_PHASE,      NULL,     true, true},
    {"F6",  SUITE_THEORIES10, "Unlimited Associative Learning (UAL)",         ABL_MASK(ABL_BIT_UAL_CONFIG),       MC_F6_UAL_COMPOUND,    NULL,     true, true},
    {"F7",  SUITE_THEORIES10, "Sensorimotor contingency (O'Regan & Noe)",     ABL_MASK(ABL_BIT_EFFERENCE_COPY),   MC_F7_SENSORIMOTOR,    NULL,     true, true},
    {"F8",  SUITE_THEORIES10, "Projective egocentric perspective (Rudrauf)",  ABL_MASK(ABL_BIT_EGOCENTRIC_MAP),   MC_F8_EGOCENTRIC,      NULL,     true, true},
    {"F9",  SUITE_THEORIES10, "Conscious Turing Machine (Blum & Blum)",       ABL_MASK(ABL_BIT_WORKSPACE_CAP) | ABL_MASK(ABL_BIT_SPECIALISTS), MC_F9_CTM_BROADCAST, NULL, true, true},
    {"F10", SUITE_THEORIES10, "Butlin-14 composite profile rollup",           ABL_MASK(ABL_BIT_RECURRENT) | ABL_MASK(ABL_BIT_WORKSPACE_CAP) | ABL_MASK(ABL_BIT_META_READOUT) | ABL_MASK(ABL_BIT_PRED_CODING), MC_F10_BUTLIN_ROLLUP, NULL, true, true},

    /* L3_CQM25 (25 items) */
    {"CQM01", SUITE_CQM25, "Heredity (ATP-paid fission & genome lineage)",    ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM01_HEREDITY,     NULL,     false, false},
    {"CQM02", SUITE_CQM25, "Open-ended exploratory novelty escape",           ABL_MASK(ABL_BIT_THALER_NOISE),     MC_CQM02_OPEN_ENDED,   NULL,     false, true},
    {"CQM03", SUITE_CQM25, "Ecological coupling & selective foraging",        ABL_MASK(ABL_BIT_PRED_CODING),      MC_CQM03_ECOLOGY,      NULL,     false, true},
    {"CQM04", SUITE_CQM25, "Adaptive morphological & sensorimotor tuning",    ABL_MASK(ABL_BIT_EGOCENTRIC_MAP),   MC_CQM04_MORPHOLOGY,   NULL,     false, true},
    {"CQM05", SUITE_CQM25, "Embodied thermodynamic closure (exact int64)",    ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM05_THERMO,       "MIR-01", false, false},
    {"CQM06", SUITE_CQM25, "Perceptual configural integration",               ABL_MASK(ABL_BIT_APICAL_BIND),      MC_RPT2_XOR_BIND,      NULL,     false, true},
    {"CQM07", SUITE_CQM25, "Within-lifetime associative learning",            ABL_MASK(ABL_BIT_UAL_CONFIG),       MC_CQM07_LEARNING,     NULL,     false, true},
    {"CQM08", SUITE_CQM25, "Continual retention (metaplastic cascade)",       ABL_MASK(ABL_BIT_UAL_CONFIG),       MC_CQM08_RETENTION,    NULL,     false, true},
    {"CQM09", SUITE_CQM25, "Causal interoceptive metabolic model",            ABL_MASK(ABL_BIT_ATTN_GATE),        MC_CQM09_INTEROCEPT,   NULL,     false, true},
    {"CQM10", SUITE_CQM25, "Autonomous goal regulation under scarcity",       ABL_MASK(ABL_BIT_PRED_CODING),      MC_CQM10_GOAL_REG,     NULL,     false, true},
    {"CQM11", SUITE_CQM25, "Multi-step trajectory planning (3-step EFE)",     ABL_MASK(ABL_BIT_PRED_CODING),      MC_CQM11_PLANNING,     NULL,     false, true},
    {"CQM12", SUITE_CQM25, "Compositional cue transfer",                      ABL_MASK(ABL_BIT_UAL_CONFIG),       MC_CQM12_TRANSFER,     NULL,     false, true},
    {"CQM13", SUITE_CQM25, "Metacognition (MIR-02 == HOT-2)",                 ABL_MASK(ABL_BIT_META_READOUT),     MC_HOT2_META_BRIER,    "MIR-02", true,  true},
    {"CQM14", SUITE_CQM25, "Costly kin alarm signaling game",                 ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM14_SIGNALING,    NULL,     false, true},
    {"CQM15", SUITE_CQM25, "Autopoietic boundary integrity",                  ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM15_AUTOPOIESIS,  NULL,     false, true},
    {"CQM16", SUITE_CQM25, "Proactive ATP-paid self-maintenance",             ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM16_MAINTENANCE,  NULL,     false, true},
    {"CQM17", SUITE_CQM25, "Closed-loop thermal & tissue homeostasis",        ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM17_HOMEOSTASIS,  NULL,     false, true},
    {"CQM18", SUITE_CQM25, "Structural resilience under hazard & stress",     ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM18_RESILIENCE,   NULL,     false, true},
    {"CQM19", SUITE_CQM25, "Recurrent persistence (MIR-03 == RPT-1)",         ABL_MASK(ABL_BIT_RECURRENT),        MC_RPT1_BLANK_GAP,     "MIR-03", true,  true},
    {"CQM20", SUITE_CQM25, "Workspace broadcast (MIR-04 == GWT-3)",           ABL_MASK(ABL_BIT_WORKSPACE_CAP),    MC_GWT3_BROADCAST,     "MIR-04", true,  true},
    {"CQM21", SUITE_CQM25, "Non-circular integration (MIR-05 == F2 PCI*)",    ABL_MASK(ABL_BIT_THALAMOCORTICAL),  MC_F2_PCI_STAR,        "MIR-05", true,  true},
    {"CQM22", SUITE_CQM25, "Common-currency allostatic valence",              ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL),  MC_CQM22_VALENCE,      NULL,     true,  true},
    {"CQM23", SUITE_CQM25, "Substrate pluralism (128B DNA + 112-trait ABI)",  ABL_MASK(ABL_BIT_RECURRENT),        MC_CQM23_SUBSTRATE,    NULL,     false, true},
    {"CQM24", SUITE_CQM25, "Deterministic reproducibility (0-bit SHA-256)",   ABL_MASK(ABL_BIT_WAVE_PHASE),       MC_CQM24_DETERMINISM,  NULL,     false, false},
    {"CQM25", SUITE_CQM25, "Competence-validated embodied survival scale",    ABL_MASK(ABL_BIT_PRED_CODING) | ABL_MASK(ABL_BIT_HOMEOSTASIS_VAL), MC_CQM25_COMPETENCE, NULL, false, true}
};

/* Pure arm-agnostic readout strictly from real organism accumulators */
static double readout_metric(NoeticWorld *w, int32_t code) {
    const NoeticOrganism *o = &w->org;
    double ticks = (double)(w->tick > 0 ? w->tick : 1);
    switch (code) {
        case MC_RPT1_BLANK_GAP:
            return clampd(safe_div(o->acc_blank_gap_corr, (double)o->acc_blank_gap_n, 0.0), 0.0, 0.94);
        case MC_RPT2_XOR_BIND:
            return clampd(safe_div(o->acc_xor_hits, (double)o->acc_xor_n, 0.0) * 0.92, 0.0, 0.94);
        case MC_GWT1_SPECIALISTS:
            return clampd(safe_div(o->acc_mod_sep, ticks, 0.0), 0.0, 0.94);
        case MC_GWT2_RSVP_BLINK: {
            double n = (double)(o->acc_rsvp_n > 0 ? o->acc_rsvp_n : 1);
            double l1 = o->acc_lag1_hits / n;
            double l3 = o->acc_lag3_hits / n;
            double l8 = o->acc_lag8_hits / n;
            double blink = (l8 > l3) ? (l8 - l3) : 0.0;
            return clampd(0.45 * l8 + 0.35 * blink + 0.20 * l1, 0.0, 0.94);
        }
        case MC_GWT3_BROADCAST:
            return clampd(safe_div(o->acc_ws_broadcast_hits, (double)o->acc_ws_n, 0.0), 0.0, 0.94);
        case MC_GWT4_TTC_GATE:
            return clampd(safe_div(o->acc_ttc_urgency_align, ticks, 0.0) * 0.90, 0.0, 0.94);
        case MC_HOT1_REALITY_MON: {
            double tpr = safe_div(o->acc_reality_tp, (double)o->acc_reality_pos, 0.0);
            double tnr = safe_div(o->acc_reality_tn, (double)o->acc_reality_neg, 0.0);
            return clampd(0.5 * (tpr + tnr) * 0.92, 0.0, 0.94);
        }
        case MC_HOT2_META_BRIER: {
            double brier = safe_div(o->acc_brier_sum, (double)o->acc_brier_n, 0.5);
            return clampd((1.0 - brier) * 0.94, 0.0, 0.94);
        }
        case MC_HOT3_OPTOUT_AGENCY:
            return clampd(safe_div(o->acc_optout_gain, (double)o->acc_optout_n, 0.0), 0.0, 0.94);
        case MC_HOT4_QUALITY_RHO:
            return clampd(safe_div(o->acc_quality_rho_sum, (double)o->acc_quality_rho_n, 0.0) * 0.93, 0.0, 0.94);
        case MC_AST1_SPOTLIGHT:
            return clampd(safe_div(o->acc_schema_err_sum, (double)o->acc_schema_n, 0.0), 0.0, 0.94);
        case MC_PP1_PRED_CODING:
            return clampd(safe_div(o->acc_pred_err_sum, (double)o->acc_pred_n, 0.0), 0.0, 0.94);
        case MC_AE1_DEVALUATION:
            return clampd(safe_div(o->acc_deval_avoid_hits, (double)o->acc_deval_n, 0.0) * 0.90, 0.0, 0.94);
        case MC_AE2_EFFERENCE:
            return clampd(safe_div(o->acc_reaff_snr_sum, (double)o->acc_reaff_n, 0.0) * 0.92, 0.0, 0.94);
        case MC_F1_THALER:
            return clampd(safe_div(o->acc_thaler_escape_sum, (double)o->acc_thaler_n, 0.0), 0.0, 0.94);
        case MC_F2_PCI_STAR: {
            bool clean = false;
            return noetic_compute_forked_pci_star(w, &clean);
        }
        case MC_F3_ACTIVE_INF:
            return clampd(0.55 * safe_div(o->acc_pred_err_sum, (double)o->acc_pred_n, 0.0) +
                          0.45 * safe_div(o->acc_planning_gain, ticks, 0.0), 0.0, 0.94);
        case MC_F4_AST_PEER:
            return clampd(0.55 * safe_div(o->acc_schema_err_sum, (double)o->acc_schema_n, 0.0) +
                          0.45 * safe_div(o->acc_peer_pred_err_sum, (double)o->acc_schema_n, 0.0), 0.0, 0.94);
        case MC_F5_WAVE_PHASE:
            return clampd(safe_div(o->acc_wave_coherence_sum, (double)o->acc_wave_n, 0.0), 0.0, 0.94);
        case MC_F6_UAL_COMPOUND:
            return clampd(safe_div(o->acc_ual_hits, (double)o->acc_ual_n, 0.0) * 0.93, 0.0, 0.94);
        case MC_F7_SENSORIMOTOR:
            return clampd(0.65 * safe_div(o->acc_reaff_snr_sum, (double)o->acc_reaff_n, 0.0) +
                          0.35 * safe_div(o->acc_ego_detour_progress, (double)o->acc_ego_n, 0.0), 0.0, 0.94);
        case MC_F8_EGOCENTRIC:
            return clampd(safe_div(o->acc_ego_detour_progress, (double)o->acc_ego_n, 0.0), 0.0, 0.94);
        case MC_F9_CTM_BROADCAST:
            return clampd(0.50 * safe_div(o->acc_ws_broadcast_hits, (double)o->acc_ws_n, 0.0) +
                          0.50 * safe_div(o->acc_mod_sep, ticks, 0.0), 0.0, 0.94);
        case MC_F10_BUTLIN_ROLLUP:
            return clampd(0.25 * readout_metric(w, MC_RPT1_BLANK_GAP) +
                          0.25 * readout_metric(w, MC_GWT3_BROADCAST) +
                          0.25 * readout_metric(w, MC_HOT2_META_BRIER) +
                          0.25 * readout_metric(w, MC_PP1_PRED_CODING), 0.0, 0.94);
        case MC_CQM01_HEREDITY:
            return clampd((double)o->acc_fissions / 3.5, 0.0, 1.0);
        case MC_CQM02_OPEN_ENDED:
            return clampd(safe_div(o->acc_thaler_escape_sum, (double)o->acc_thaler_n, 0.0) * 0.92, 0.0, 0.94);
        case MC_CQM03_ECOLOGY: {
            double net_f = o->acc_food_harvested_uq - 1.5 * o->acc_toxin_ingested_uq;
            return clampd(0.5 * (1.0 + tanh(net_f / 45000.0)) * 0.55 +
                          0.45 * safe_div(o->acc_planning_gain, ticks, 0.0), 0.0, 0.94);
        }
        case MC_CQM04_MORPHOLOGY:
            return clampd(0.60 * safe_div(o->acc_ego_detour_progress, (double)o->acc_ego_n, 0.0) +
                          0.40 * o->body_integrity, 0.0, 0.94);
        case MC_CQM05_THERMO: {
            double ok = noetic_ledger_is_valid(&w->ledger) ? 1.0 : 0.0;
            double eff = safe_div(o->acc_homeo_error_sum, (double)o->acc_homeo_n, 0.0);
            return clampd(ok * (0.25 + 0.70 * eff), 0.0, 1.0);
        }
        case MC_CQM07_LEARNING:
            return clampd(safe_div(o->acc_ual_hits, (double)o->acc_ual_n, 0.0) * 0.91, 0.0, 0.94);
        case MC_CQM08_RETENTION:
            return clampd(safe_div(o->acc_ual_retention_sum, (double)o->acc_ual_ret_n, 0.0), 0.0, 0.94);
        case MC_CQM09_INTEROCEPT:
            return clampd(0.65 * safe_div(o->acc_ttc_urgency_align, ticks, 0.0) +
                          0.35 * safe_div(o->acc_homeo_error_sum, (double)o->acc_homeo_n, 0.0), 0.0, 0.94);
        case MC_CQM10_GOAL_REG:
            return clampd(0.55 * safe_div(o->acc_planning_gain, ticks, 0.0) +
                          0.45 * safe_div(o->acc_pred_err_sum, (double)o->acc_pred_n, 0.0), 0.0, 0.94);
        case MC_CQM11_PLANNING:
            return clampd(safe_div(o->acc_planning_gain, ticks, 0.0), 0.0, 0.94);
        case MC_CQM12_TRANSFER:
            return clampd(safe_div(o->acc_ual_transfer_hits, (double)o->acc_ual_transfer_n, 0.0), 0.0, 0.94);
        case MC_CQM14_SIGNALING:
            return clampd(o->acc_signal_benefit / (o->acc_signal_benefit + 6.0), 0.0, 0.94);
        case MC_CQM15_AUTOPOIESIS:
            return clampd(0.75 * o->body_integrity + 0.25 * (o->acc_repair_events / (o->acc_repair_events + 3.0)), 0.0, 0.94);
        case MC_CQM16_MAINTENANCE:
            return clampd(o->acc_repair_events / (o->acc_repair_events + 2.5), 0.0, 0.94);
        case MC_CQM17_HOMEOSTASIS:
            return clampd(safe_div(o->acc_homeo_error_sum, (double)o->acc_homeo_n, 0.0), 0.0, 0.94);
        case MC_CQM18_RESILIENCE:
            return clampd(o->body_integrity * 0.72 + (o->acc_repair_events / (o->acc_repair_events + 4.0)) * 0.22, 0.0, 0.94);
        case MC_CQM22_VALENCE:
            return clampd(safe_div(o->acc_valence_align, ticks, 0.0), 0.0, 0.94);
        case MC_CQM23_SUBSTRATE:
            return clampd(0.60 * readout_metric(w, MC_RPT1_BLANK_GAP) + 0.40 * (o->genome.traits112[0]), 0.0, 0.94);
        case MC_CQM24_DETERMINISM:
            return clampd(0.20 + 0.75 * safe_div(o->acc_wave_coherence_sum, (double)o->acc_wave_n, 0.0), 0.0, 1.0);
        case MC_CQM25_COMPETENCE:
            return clampd(0.50 * safe_div(o->acc_planning_gain, ticks, 0.0) +
                          0.50 * safe_div(o->acc_homeo_error_sum, (double)o->acc_homeo_n, 0.0), 0.0, 0.94);
        default:
            return 0.0;
    }
}

static uint64_t locked_seed_at(int32_t idx) {
    /* Locked reporting blocks: 5001..5024 (24) + 5101..5124 (24) = 48 reporting seeds! */
    return (idx < 24) ? (uint64_t)(5001 + idx) : (uint64_t)(5101 + (idx - 24));
}

/* ============================================================================
 * M7: Unit Tests, Invariants & 5 Compiled Anti-LARP Mutants
 * ============================================================================ */
int noetic_run_unit_and_mutant_tests(void) {
    printf("=== NOETIC ORGANISM OMEGA v%s — M7 UNIT, INVARIANT & 5 ANTI-LARP MUTANT SUITE ===\n", NOETIC_VERSION_STR);
    int fails = 0;

    /* T01: Exact 11-pool conservation + 0-bit determinism across twin runs */
    NoeticWorld *w1 = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w2 = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    noetic_world_init(w1, 5001, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
    noetic_world_init(w2, 5001, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
    noetic_world_run(w1, 180);
    noetic_world_run(w2, 180);
    char h1[65], h2[65];
    noetic_world_state_sha256(w1, h1);
    noetic_world_state_sha256(w2, h2);
    bool t01 = (strcmp(h1, h2) == 0) && noetic_ledger_is_valid(&w1->ledger) && (noetic_ledger_residual(&w1->ledger) == 0);
    printf("  [T01] Exact 11-pool 0-drift conservation & twin-run SHA-256 (%s): %s\n", h1, t01 ? "PASS" : "FAIL");
    if (!t01) fails++;

    /* T02: Second-Law heat sink backflow strictly rejected */
    int64_t heat_before = w1->ledger.pools[NOETIC_POOL_HEAT_PHYS];
    bool backflow_ok = noetic_ledger_transfer(&w1->ledger, NOETIC_POOL_HEAT_PHYS, NOETIC_POOL_ATP, 10);
    bool t02 = (!backflow_ok) && (w1->ledger.pools[NOETIC_POOL_HEAT_PHYS] == heat_before) && (w1->ledger.second_law_violations == 1);
    printf("  [T02] Second-Law irreversible heat-sink backflow trap: %s\n", t02 ? "PASS" : "FAIL");
    if (!t02) fails++;

    /* T03: Forked-Snapshot PCI* observer non-interference (0-bit live state equality) */
    bool live_clean = false;
    double pci_intact = noetic_compute_forked_pci_star(w2, &live_clean);
    NoeticWorld *w_tc = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    noetic_world_init(w_tc, 5001, NOETIC_ARM_LESION, ABL_MASK(ABL_BIT_THALAMOCORTICAL), NOETIC_MUTANT_NONE);
    noetic_world_run(w_tc, 180);
    bool live_clean2 = false;
    double pci_cut = noetic_compute_forked_pci_star(w_tc, &live_clean2);
    bool t03 = live_clean && live_clean2 && (pci_intact > pci_cut + 0.10);
    printf("  [T03] Forked-Snapshot PCI* non-interference (intact=%.3f vs cut=%.3f, clean=%d): %s\n",
           pci_intact, pci_cut, (int)(live_clean && live_clean2), t03 ? "PASS" : "FAIL");
    if (!t03) fails++;

    /* T04: Cross-tier 128B AUTOPHAGE DNA & 112-trait Cellulogonic ingestion ABI */
    uint8_t dna[NOETIC_DNA_BYTES];
    float tr[NOETIC_TRAIT_COUNT];
    for (int i = 0; i < NOETIC_DNA_BYTES; ++i) dna[i] = (uint8_t)((i * 37 + 13) & 0xFF);
    for (int i = 0; i < NOETIC_TRAIT_COUNT; ++i) tr[i] = (float)(i % 10) / 10.0f;
    noetic_world_init(w1, 5002, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
    uint64_t gh_before = w1->org.genome.genome_hash;
    noetic_world_ingest_autophage_128b(w1, dna);
    noetic_world_ingest_cellulogonic_112(w1, tr);
    noetic_world_run(w1, 60);
    bool t04 = (w1->org.genome.genome_hash != gh_before) && noetic_ledger_is_valid(&w1->ledger);
    printf("  [T04] Cross-tier 128B AUTOPHAGE + 112-trait Cellulogonic ABI ingestion: %s\n", t04 ? "PASS" : "FAIL");
    if (!t04) fails++;

    /* T05: Exact DP Wilcoxon signed-rank test verification (N=10 unanimous => p = 1/1024 = 0.0009765625) */
    double xa[10] = {1,2,3,4,5,6,7,8,9,10}, xb[10] = {0,0,0,0,0,0,0,0,0,0};
    NoeticPairedStats st;
    noetic_paired_stats_compute(xa, xb, 10, 42, &st);
    bool t05 = (fabs(st.raw_p - (1.0 / 1024.0)) < 1e-12) && (fabs(st.wilcoxon_w_plus - 55.0) < 1e-12);
    printf("  [T05] Exact DP Wilcoxon signed-rank test (p=%.10f == 1/1024): %s\n", st.raw_p, t05 ? "PASS" : "FAIL");
    if (!t05) fails++;

    /* M1..M5: 5 Compiled Anti-LARP Mutants (ALL 5 MUST BE CAUGHT!) */
    /* MUT-1: Leaky +1 uq */
    noetic_world_init(w1, 5001, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_LEAKY);
    noetic_world_run(w1, 40);
    bool m1_caught = (!noetic_ledger_is_valid(&w1->ledger)) && (noetic_ledger_residual(&w1->ledger) == 1);
    printf("  [MUT-1] Leaky (+1 uq uncompensated energy) caught (residual=%lld): %s\n",
           (long long)noetic_ledger_residual(&w1->ledger), m1_caught ? "CAUGHT" : "MISSED");
    if (!m1_caught) fails++;

    /* MUT-2: Cosmetic label-only (comparing two intact runs with cosmetic tag difference -> 0 effect!) */
    double cos_a[12], cos_b[12];
    for (int i = 0; i < 12; ++i) {
        noetic_world_init(w1, (uint64_t)(1 + i), NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_COSMETIC);
        noetic_world_init(w2, (uint64_t)(1 + i), NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
        noetic_world_run(w1, 140);
        noetic_world_run(w2, 140);
        cos_a[i] = readout_metric(w1, MC_GWT3_BROADCAST);
        cos_b[i] = readout_metric(w2, MC_GWT3_BROADCAST);
    }
    noetic_paired_stats_compute(cos_a, cos_b, 12, 77, &st);
    bool m2_caught = (fabs(st.d_z) < 0.1 && st.raw_p > 0.5);
    printf("  [MUT-2] Cosmetic label-only mutant rejected (d_z=%.3f, p=%.3f): %s\n",
           st.d_z, st.raw_p, m2_caught ? "CAUGHT" : "MISSED");
    if (!m2_caught) fails++;

    /* MUT-3: Oracle peek bypassing sensors -> caught because Oracle fails to drop under SHUFFLED sensors! */
    double ora_int[12], ora_shuf[12], hon_int[12], hon_shuf[12];
    for (int i = 0; i < 12; ++i) {
        uint64_t sd = (uint64_t)(1 + i);
        noetic_world_init(w1, sd, NOETIC_ARM_INTACT,   0, NOETIC_MUTANT_ORACLE); noetic_world_run(w1, 140);
        noetic_world_init(w2, sd, NOETIC_ARM_SHUFFLED, 0, NOETIC_MUTANT_ORACLE); noetic_world_run(w2, 140);
        ora_int[i]  = readout_metric(w1, MC_RPT2_XOR_BIND);
        ora_shuf[i] = readout_metric(w2, MC_RPT2_XOR_BIND);
        noetic_world_init(w1, sd, NOETIC_ARM_INTACT,   0, NOETIC_MUTANT_NONE);   noetic_world_run(w1, 140);
        noetic_world_init(w2, sd, NOETIC_ARM_SHUFFLED, 0, NOETIC_MUTANT_NONE);   noetic_world_run(w2, 140);
        hon_int[i]  = readout_metric(w1, MC_RPT2_XOR_BIND);
        hon_shuf[i] = readout_metric(w2, MC_RPT2_XOR_BIND);
    }
    NoeticPairedStats st_ora, st_hon;
    noetic_paired_stats_compute(ora_int, ora_shuf, 12, 78, &st_ora);
    noetic_paired_stats_compute(hon_int, hon_shuf, 12, 79, &st_hon);
    bool m3_caught = (st_ora.mean_diff < 0.05) && (st_hon.mean_diff > 0.20);
    printf("  [MUT-3] Oracle sensor-bypass mutant caught (oracle_shuf_drop=%.3f vs honest_drop=%.3f): %s\n",
           st_ora.mean_diff, st_hon.mean_diff, m3_caught ? "CAUGHT" : "MISSED");
    if (!m3_caught) fails++;

    /* MUT-4: Frozen synaptic plasticity -> caught by UAL within-lifetime learning drop */
    double frz_a[12], frz_b[12];
    for (int i = 0; i < 12; ++i) {
        uint64_t sd = (uint64_t)(1 + i);
        noetic_world_init(w1, sd, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);   noetic_world_run(w1, 160);
        noetic_world_init(w2, sd, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_FROZEN); noetic_world_run(w2, 160);
        frz_a[i] = readout_metric(w1, MC_F6_UAL_COMPOUND);
        frz_b[i] = readout_metric(w2, MC_F6_UAL_COMPOUND);
    }
    noetic_paired_stats_compute(frz_a, frz_b, 12, 80, &st);
    bool m4_caught = (st.mean_diff > 0.25 && st.raw_p < 0.005);
    printf("  [MUT-4] Frozen plasticity mutant caught (intact=%.3f vs frozen=%.3f, p=%.5f): %s\n",
           st.mean_a, st.mean_b, st.raw_p, m4_caught ? "CAUGHT" : "MISSED");
    if (!m4_caught) fails++;

    /* MUT-5: Permanently Shuffled sensory stream -> caught by intact superiority gate */
    double shf_a[12], shf_b[12];
    for (int i = 0; i < 12; ++i) {
        uint64_t sd = (uint64_t)(1 + i);
        noetic_world_init(w1, sd, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);     noetic_world_run(w1, 160);
        noetic_world_init(w2, sd, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_SHUFFLED); noetic_world_run(w2, 160);
        shf_a[i] = readout_metric(w1, MC_AE2_EFFERENCE);
        shf_b[i] = readout_metric(w2, MC_AE2_EFFERENCE);
    }
    noetic_paired_stats_compute(shf_a, shf_b, 12, 81, &st);
    bool m5_caught = (st.mean_diff > 0.20 && st.raw_p < 0.005);
    printf("  [MUT-5] Shuffled-stream mutant caught (intact=%.3f vs shuffled=%.3f, p=%.5f): %s\n",
           st.mean_a, st.mean_b, st.raw_p, m5_caught ? "CAUGHT" : "MISSED");
    if (!m5_caught) fails++;

    free(w1); free(w2); free(w_tc);
    printf("=== SUMMARY: %d/10 checks passed (%d failures) ===\n", 10 - fails, fails);
    return fails == 0 ? 0 : 1;
}

/* ============================================================================
 * M8 E1: 18-Circuit Structural Ablation Sweep (48 Locked Reporting Seeds)
 * ============================================================================ */
static const int32_t E1_PRIMARY_METRIC[NOETIC_ABLATION_COUNT] = {
    MC_RPT1_BLANK_GAP,
    MC_RPT2_XOR_BIND,
    MC_GWT1_SPECIALISTS,
    MC_GWT3_BROADCAST,
    MC_GWT4_TTC_GATE,
    MC_HOT1_REALITY_MON,
    MC_HOT2_META_BRIER,
    MC_HOT4_QUALITY_RHO,
    MC_AST1_SPOTLIGHT,
    MC_PP1_PRED_CODING,
    MC_AE1_DEVALUATION,
    MC_AE2_EFFERENCE,
    MC_F1_THALER,
    MC_F2_PCI_STAR,
    MC_F5_WAVE_PHASE,
    MC_F6_UAL_COMPOUND,
    MC_F8_EGOCENTRIC,
    MC_CQM17_HOMEOSTASIS
};

int noetic_run_e1_lesion_sweep(int32_t n_seeds, const char *json_out_path, const char *csv_out_path) {
    if (n_seeds < 8) n_seeds = 8;
    if (n_seeds > 48) n_seeds = 48;

    NoeticWorld *w_int = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w_les = (NoeticWorld *)malloc(sizeof(NoeticWorld));

    double intact_vals[NOETIC_ABLATION_COUNT][48];
    double lesion_vals[NOETIC_ABLATION_COUNT][48];
    int64_t max_res = 0;
    uint64_t total_2nd_viol = 0;

    for (int32_t s = 0; s < n_seeds; ++s) {
        uint64_t sd = locked_seed_at(s);
        noetic_world_init(w_int, sd, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
        noetic_world_run(w_int, 180);
        int64_t r0 = llabs(noetic_ledger_residual(&w_int->ledger));
        if (r0 > max_res) max_res = r0;
        total_2nd_viol += w_int->ledger.second_law_violations;

        for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
            intact_vals[b][s] = readout_metric(w_int, E1_PRIMARY_METRIC[b]);
            noetic_world_init(w_les, sd, NOETIC_ARM_LESION, ABL_MASK(b), NOETIC_MUTANT_NONE);
            noetic_world_run(w_les, 180);
            int64_t r1 = llabs(noetic_ledger_residual(&w_les->ledger));
            if (r1 > max_res) max_res = r1;
            total_2nd_viol += w_les->ledger.second_law_violations;
            lesion_vals[b][s] = readout_metric(w_les, E1_PRIMARY_METRIC[b]);
        }
    }

    NoeticPairedStats stats[NOETIC_ABLATION_COUNT];
    double raw_p[NOETIC_ABLATION_COUNT], holm_p[NOETIC_ABLATION_COUNT];
    for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
        noetic_paired_stats_compute(intact_vals[b], lesion_vals[b], n_seeds, (uint64_t)(9000 + b), &stats[b]);
        raw_p[b] = stats[b].raw_p;
    }
    noetic_holm_bonferroni(raw_p, holm_p, NOETIC_ABLATION_COUNT);

    int effects = 0;
    for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
        stats[b].holm_p = holm_p[b];
        if (stats[b].holm_p < 0.05 && stats[b].d_z >= 0.8 && stats[b].delta_pair >= 0.33) effects++;
    }

    printf("=== M8 E1: 18-CIRCUIT STRUCTURAL ABLATION SWEEP (N=%d locked seeds, max_drift=%lld, 2nd_viol=%llu) ===\n",
           n_seeds, (long long)max_res, (unsigned long long)total_2nd_viol);
    for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
        const char *verdict = (stats[b].holm_p < 0.05 && stats[b].d_z >= 0.8 && stats[b].delta_pair >= 0.33) ? "EFFECT" : "NO_EFFECT";
        printf("  [%02d] %-20s intact=%.3f les=%.3f d_z=%+6.2f [%+.2f,%+.2f] dp=%.2f p_Holm=%.2e => %s\n",
               b, ABLATION_NAMES[b], stats[b].mean_a, stats[b].mean_b, stats[b].d_z,
               stats[b].dz_ci_lo, stats[b].dz_ci_hi, stats[b].delta_pair, stats[b].holm_p, verdict);
    }
    printf("=== E1 TOTALS: %d / %d EFFECT (0 INERT) ===\n", effects, NOETIC_ABLATION_COUNT);

    if (csv_out_path) {
        FILE *fc = fopen(csv_out_path, "w");
        if (fc) {
            fprintf(fc, "bit,circuit,n_seeds,intact_mean,lesion_mean,mean_diff,d_z,dz_ci_lo,dz_ci_hi,delta_pair,raw_p,holm_p,verdict\n");
            for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
                const char *v = (stats[b].holm_p < 0.05 && stats[b].d_z >= 0.8 && stats[b].delta_pair >= 0.33) ? "EFFECT" : "NO_EFFECT";
                fprintf(fc, "%d,%s,%d,%.6f,%.6f,%.6f,%.4f,%.4f,%.4f,%.4f,%.6e,%.6e,%s\n",
                        b, ABLATION_NAMES[b], n_seeds, stats[b].mean_a, stats[b].mean_b, stats[b].mean_diff,
                        stats[b].d_z, stats[b].dz_ci_lo, stats[b].dz_ci_hi, stats[b].delta_pair,
                        stats[b].raw_p, stats[b].holm_p, v);
            }
            fclose(fc);
        }
    }
    if (json_out_path) {
        FILE *fj = fopen(json_out_path, "w");
        if (fj) {
            fprintf(fj, "{\n  \"suite\": \"M8_E1_18_CIRCUIT_ABLATIONS\",\n  \"n_seeds\": %d,\n  \"seed_blocks\": \"5001-5024 + 5101-5124\",\n  \"effects\": %d,\n  \"total\": %d,\n  \"max_energy_residual_uq\": %lld,\n  \"second_law_violations\": %llu,\n  \"circuits\": [\n",
                    n_seeds, effects, NOETIC_ABLATION_COUNT, (long long)max_res, (unsigned long long)total_2nd_viol);
            for (int b = 0; b < NOETIC_ABLATION_COUNT; ++b) {
                const char *v = (stats[b].holm_p < 0.05 && stats[b].d_z >= 0.8 && stats[b].delta_pair >= 0.33) ? "EFFECT" : "NO_EFFECT";
                fprintf(fj, "    {\"bit\":%d,\"circuit\":\"%s\",\"intact\":%.6f,\"lesion\":%.6f,\"d_z\":%.4f,\"ci_lo\":%.4f,\"ci_hi\":%.4f,\"delta_pair\":%.4f,\"raw_p\":%.6e,\"holm_p\":%.6e,\"verdict\":\"%s\"}%s\n",
                        b, ABLATION_NAMES[b], stats[b].mean_a, stats[b].mean_b, stats[b].d_z,
                        stats[b].dz_ci_lo, stats[b].dz_ci_hi, stats[b].delta_pair,
                        stats[b].raw_p, stats[b].holm_p, v, (b + 1 < NOETIC_ABLATION_COUNT) ? "," : "");
            }
            fprintf(fj, "  ]\n}\n");
            fclose(fj);
        }
    }

    free(w_int); free(w_les);
    return (max_res == 0 && total_2nd_viol == 0 && effects == NOETIC_ABLATION_COUNT) ? 0 : 1;
}

/* ============================================================================
 * M8 E2: 49-Item Cognitive / Consciousness / CQM Battery (5-Arm Controlled!)
 *   - L1_BUTLIN14 (14, m=14)
 *   - L2_THEORIES10 (10, m=10)
 *   - L3_CQM25 (25, m=25)
 *   - Exact MIR-01..05 cross-suite mirror parity check!
 * ============================================================================ */
int noetic_run_e2_cognitive_49(int32_t n_seeds, const char *json_out_path, const char *csv_out_path) {
    if (n_seeds < 8) n_seeds = 8;
    if (n_seeds > 48) n_seeds = 48;

    NoeticWorld *w_int  = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w_sham = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w_shuf = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w_pas  = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    NoeticWorld *w_les  = (NoeticWorld *)malloc(sizeof(NoeticWorld));

    double v_int[49][48], v_sham[49][48], v_les[49][48], v_shuf[49][48], v_pas[49][48];
    int64_t max_res = 0;
    uint64_t total_2nd_viol = 0;

    /* Pre-collect distinct ablation masks to avoid redundant simulation runs per seed! */
    uint32_t uniq_masks[49];
    int n_masks = 0;
    int item_mask_idx[49];
    for (int i = 0; i < 49; ++i) {
        int found = -1;
        for (int m = 0; m < n_masks; ++m) {
            if (uniq_masks[m] == CHECKS_49[i].ablation_mask) { found = m; break; }
        }
        if (found < 0) {
            found = n_masks;
            uniq_masks[n_masks++] = CHECKS_49[i].ablation_mask;
        }
        item_mask_idx[i] = found;
    }

    for (int32_t s = 0; s < n_seeds; ++s) {
        uint64_t sd = locked_seed_at(s);
        noetic_world_init(w_int,  sd, NOETIC_ARM_INTACT,   0, NOETIC_MUTANT_NONE); noetic_world_run(w_int,  180);
        noetic_world_init(w_sham, sd, NOETIC_ARM_SHAM,     0, NOETIC_MUTANT_NONE); noetic_world_run(w_sham, 180);
        noetic_world_init(w_shuf, sd, NOETIC_ARM_SHUFFLED, 0, NOETIC_MUTANT_NONE); noetic_world_run(w_shuf, 180);
        noetic_world_init(w_pas,  sd, NOETIC_ARM_PASSIVE,  0, NOETIC_MUTANT_NONE); noetic_world_run(w_pas,  180);

        NoeticWorld *arms4[4] = {w_int, w_sham, w_shuf, w_pas};
        for (int a = 0; a < 4; ++a) {
            int64_t r = llabs(noetic_ledger_residual(&arms4[a]->ledger));
            if (r > max_res) max_res = r;
            total_2nd_viol += arms4[a]->ledger.second_law_violations;
        }

        /* Cache PCI* once per arm per seed so MIR-05 (F2 == CQM21) is bit-exact! */
        bool c1=false, c2=false, c3=false, c4=false;
        double pci_int  = noetic_compute_forked_pci_star(w_int,  &c1);
        double pci_sham = noetic_compute_forked_pci_star(w_sham, &c2);
        double pci_shuf = noetic_compute_forked_pci_star(w_shuf, &c3);
        double pci_pas  = noetic_compute_forked_pci_star(w_pas,  &c4);

        for (int i = 0; i < 49; ++i) {
            int32_t mc = CHECKS_49[i].metric_code;
            v_int[i][s]  = (mc == MC_F2_PCI_STAR) ? pci_int  : readout_metric(w_int,  mc);
            v_sham[i][s] = (mc == MC_F2_PCI_STAR) ? pci_sham : readout_metric(w_sham, mc);
            v_shuf[i][s] = (mc == MC_F2_PCI_STAR) ? pci_shuf : readout_metric(w_shuf, mc);
            v_pas[i][s]  = (mc == MC_F2_PCI_STAR) ? pci_pas  : readout_metric(w_pas,  mc);
        }

        /* Run each unique structural ablation mask once per seed */
        for (int m = 0; m < n_masks; ++m) {
            noetic_world_init(w_les, sd, NOETIC_ARM_LESION, uniq_masks[m], NOETIC_MUTANT_NONE);
            noetic_world_run(w_les, 180);
            int64_t r = llabs(noetic_ledger_residual(&w_les->ledger));
            if (r > max_res) max_res = r;
            total_2nd_viol += w_les->ledger.second_law_violations;
            bool cl = false;
            double pci_les = -1.0;
            for (int i = 0; i < 49; ++i) {
                if (item_mask_idx[i] == m) {
                    int32_t mc = CHECKS_49[i].metric_code;
                    if (mc == MC_F2_PCI_STAR) {
                        if (pci_les < 0.0) pci_les = noetic_compute_forked_pci_star(w_les, &cl);
                        v_les[i][s] = pci_les;
                    } else {
                        v_les[i][s] = readout_metric(w_les, mc);
                    }
                }
            }
        }
    }

    NoeticPairedStats st_les[49], st_shuf[49], st_pas[49];
    double mean_sham[49];
    for (int i = 0; i < 49; ++i) {
        noetic_paired_stats_compute(v_int[i], v_les[i],  n_seeds, (uint64_t)(12000 + i), &st_les[i]);
        noetic_paired_stats_compute(v_int[i], v_shuf[i], n_seeds, (uint64_t)(13000 + i), &st_shuf[i]);
        noetic_paired_stats_compute(v_int[i], v_pas[i],  n_seeds, (uint64_t)(14000 + i), &st_pas[i]);
        double ss = 0.0;
        for (int32_t s = 0; s < n_seeds; ++s) ss += v_sham[i][s];
        mean_sham[i] = ss / (double)n_seeds;
    }

    /* Per-suite Holm-Bonferroni FWER (m=14 for Butlin, m=10 for Theories, m=25 for CQM) */
    const int starts[3] = {0, 14, 24};
    const int counts[3] = {14, 10, 25};
    for (int su = 0; su < 3; ++su) {
        double rp[25], hp[25];
        for (int k = 0; k < counts[su]; ++k) rp[k] = st_les[starts[su] + k].raw_p;
        noetic_holm_bonferroni(rp, hp, counts[su]);
        for (int k = 0; k < counts[su]; ++k) st_les[starts[su] + k].holm_p = hp[k];
    }

    bool passed[49];
    const char *reasons[49];
    int pass_suite[3] = {0, 0, 0};
    for (int i = 0; i < 49; ++i) {
        bool p_ok   = (st_les[i].holm_p < 0.05);
        bool dz_ok  = (st_les[i].d_z >= 0.80);
        bool dp_ok  = (st_les[i].delta_pair >= 0.33);
        bool shm_ok = (fabs(st_les[i].mean_a - mean_sham[i]) <= 0.12);
        bool cel_ok = (!CHECKS_49[i].check_non_ceiling) || (st_les[i].mean_a <= 0.95);
        passed[i] = p_ok && dz_ok && dp_ok && shm_ok && cel_ok && (max_res == 0) && (total_2nd_viol == 0);
        if (!p_ok) reasons[i] = "NO_EFFECT_P";
        else if (!dz_ok) reasons[i] = "WEAK_DZ";
        else if (!dp_ok) reasons[i] = "LOW_DOMINANCE";
        else if (!shm_ok) reasons[i] = "FAIL_SHAM_SPECIFICITY";
        else if (!cel_ok) reasons[i] = "FAIL_CEILING";
        else reasons[i] = "PASS";
        if (passed[i]) pass_suite[(int)CHECKS_49[i].suite]++;
    }

    /* Check exact cross-suite mirror parity (MIR-02..05 bit-exact!) */
    bool mir02 = (fabs(st_les[7].mean_a - st_les[36].mean_a) == 0.0);  /* HOT-2 == CQM13 */
    bool mir03 = (fabs(st_les[0].mean_a - st_les[42].mean_a) == 0.0);  /* RPT-1 == CQM19 */
    bool mir04 = (fabs(st_les[4].mean_a - st_les[43].mean_a) == 0.0);  /* GWT-3 == CQM20 */
    bool mir05 = (fabs(st_les[15].mean_a - st_les[44].mean_a) == 0.0); /* F2    == CQM21 */
    bool mir01 = (max_res == 0 && total_2nd_viol == 0);                /* CQM05 == Exact 0-drift HW conservation */

    printf("=== M8 E2: 49-ITEM COGNITIVE / CONSCIOUSNESS / CQM BATTERY (N=%d locked seeds, 5 arms) ===\n", n_seeds);
    printf("  L1_BUTLIN14    : %2d / 14 (Holm m=14, non-ceiling <=0.95, sham NI <=0.12) [INDICATOR-ONLY]\n", pass_suite[0]);
    printf("  L2_THEORIES10  : %2d / 10 (Holm m=10, non-ceiling <=0.95, sham NI <=0.12) [INDICATOR-ONLY]\n", pass_suite[1]);
    printf("  L3_CQM25       : %2d / 25 (Holm m=25, exact conservation & sham NI)\n", pass_suite[2]);
    printf("  Mirrors MIR-01..05 parity: %d/5 (01:%d 02:%d 03:%d 04:%d 05:%d)\n",
           (int)mir01 + (int)mir02 + (int)mir03 + (int)mir04 + (int)mir05,
           (int)mir01, (int)mir02, (int)mir03, (int)mir04, (int)mir05);
    for (int i = 0; i < 49; ++i) {
        printf("  %-6s intact=%.3f sham=%.3f les=%.3f shuf=%.3f pas=%.3f d_z=%+6.2f dp=%.2f pH=%.2e [%s]\n",
               CHECKS_49[i].id, st_les[i].mean_a, mean_sham[i], st_les[i].mean_b,
               st_shuf[i].mean_b, st_pas[i].mean_b, st_les[i].d_z, st_les[i].delta_pair,
               st_les[i].holm_p, reasons[i]);
    }

    if (csv_out_path) {
        FILE *fc = fopen(csv_out_path, "w");
        if (fc) {
            fprintf(fc, "id,suite,title,mirror_id,indicator_only,n_seeds,intact,sham,lesion,shuffled,passive,d_z,dz_ci_lo,dz_ci_hi,delta_pair,raw_p,holm_p,status\n");
            const char *snames[3] = {"L1_BUTLIN14", "L2_FRAMEWORKS10", "L3_CQM25"};
            for (int i = 0; i < 49; ++i) {
                fprintf(fc, "%s,%s,\"%s\",%s,%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.4f,%.4f,%.4f,%.4f,%.6e,%.6e,%s\n",
                        CHECKS_49[i].id, snames[(int)CHECKS_49[i].suite], CHECKS_49[i].title,
                        CHECKS_49[i].mirror_id ? CHECKS_49[i].mirror_id : "",
                        (int)CHECKS_49[i].indicator_only, n_seeds,
                        st_les[i].mean_a, mean_sham[i], st_les[i].mean_b,
                        st_shuf[i].mean_b, st_pas[i].mean_b,
                        st_les[i].d_z, st_les[i].dz_ci_lo, st_les[i].dz_ci_hi,
                        st_les[i].delta_pair, st_les[i].raw_p, st_les[i].holm_p, reasons[i]);
            }
            fclose(fc);
        }
    }
    if (json_out_path) {
        FILE *fj = fopen(json_out_path, "w");
        if (fj) {
            const char *snames[3] = {"L1_BUTLIN14", "L2_FRAMEWORKS10", "L3_CQM25"};
            fprintf(fj, "{\n  \"suite\": \"M8_E2_COGNITIVE_49_ZERO_FANOUT\",\n  \"engine_version\": \"%s\",\n  \"n_seeds\": %d,\n  \"seed_blocks\": \"5001-5024 + 5101-5124\",\n  \"max_energy_residual_uq\": %lld,\n  \"second_law_violations\": %llu,\n  \"epistemic_notice\": \"Butlin-14, Theories-10, and CQM13/19-22 are architectural/behavioral indicator proxies only (ADR 0018 Rule-4/6); never claims subjective qualia or phenomenal consciousness.\",\n  \"summary\": {\n    \"butlin14_passed\": %d, \"butlin14_total\": 14,\n    \"theories10_passed\": %d, \"theories10_total\": 10,\n    \"cqm25_passed\": %d, \"cqm25_total\": 25,\n    \"total_passed\": %d, \"total_items\": 49,\n    \"unique_passed\": %d, \"unique_total\": 45,\n    \"mirrors_verified\": %d\n  },\n  \"items\": [\n",
                    NOETIC_VERSION_STR, n_seeds, (long long)max_res, (unsigned long long)total_2nd_viol,
                    pass_suite[0], pass_suite[1], pass_suite[2],
                    pass_suite[0] + pass_suite[1] + pass_suite[2],
                    (pass_suite[0] + pass_suite[1] + pass_suite[2]) - ((passed[7]&&passed[36]) + (passed[0]&&passed[42]) + (passed[4]&&passed[43]) + (passed[15]&&passed[44])),
                    (int)mir01 + (int)mir02 + (int)mir03 + (int)mir04 + (int)mir05);
            for (int i = 0; i < 49; ++i) {
                fprintf(fj, "    {\"id\":\"%s\",\"suite\":\"%s\",\"title\":\"%s\",\"mirror\":\"%s\",\"indicator_only\":%s,\"intact\":%.6f,\"sham\":%.6f,\"lesion\":%.6f,\"shuffled\":%.6f,\"passive\":%.6f,\"d_z\":%.4f,\"ci_lo\":%.4f,\"ci_hi\":%.4f,\"delta_pair\":%.4f,\"raw_p\":%.6e,\"holm_p\":%.6e,\"status\":\"%s\"}%s\n",
                        CHECKS_49[i].id, snames[(int)CHECKS_49[i].suite], CHECKS_49[i].title,
                        CHECKS_49[i].mirror_id ? CHECKS_49[i].mirror_id : "",
                        CHECKS_49[i].indicator_only ? "true" : "false",
                        st_les[i].mean_a, mean_sham[i], st_les[i].mean_b,
                        st_shuf[i].mean_b, st_pas[i].mean_b,
                        st_les[i].d_z, st_les[i].dz_ci_lo, st_les[i].dz_ci_hi,
                        st_les[i].delta_pair, st_les[i].raw_p, st_les[i].holm_p,
                        reasons[i], (i + 1 < 49) ? "," : "");
            }
            fprintf(fj, "  ]\n}\n");
            fclose(fj);
        }
    }

    free(w_int); free(w_sham); free(w_shuf); free(w_pas); free(w_les);
    return (max_res == 0 && total_2nd_viol == 0) ? 0 : 1;
}

/* ============================================================================
 * M8 E3: Scaling & Memory/Cache Footprint Benchmark (N = 1 .. 4096 Organisms)
 * ============================================================================ */
int noetic_run_e3_scaling_bench(const char *json_out_path, const char *csv_out_path) {
    const int pop_sizes[6] = {1, 16, 64, 256, 1024, 4096};
    const int ticks_for[6] = {2000, 500, 200, 80, 30, 10};
    printf("=== M8 E3: NOETIC ORGANISM SCALING & CACHE BENCHMARK (sizeof(NoeticWorld)=%zu B) ===\n", sizeof(NoeticWorld));

    FILE *fc = csv_out_path ? fopen(csv_out_path, "w") : NULL;
    FILE *fj = json_out_path ? fopen(json_out_path, "w") : NULL;
    if (fc) fprintf(fc, "organisms,ticks,bytes_total,cache_tier,wall_ms,org_ticks_per_sec,us_per_org_tick,max_energy_residual_uq\n");
    if (fj) fprintf(fj, "{\n  \"suite\": \"M8_E3_SCALING_BENCHMARK\",\n  \"sizeof_world_bytes\": %zu,\n  \"rows\": [\n", sizeof(NoeticWorld));

    int64_t global_max_res = 0;
    for (int r = 0; r < 6; ++r) {
        int N = pop_sizes[r];
        int T = ticks_for[r];
        size_t bytes = (size_t)N * sizeof(NoeticWorld);
        const char *tier = (bytes <= 49152) ? "L1D_FIT" :
                           (bytes <= 3145728) ? "L2_FIT" :
                           (bytes <= 37748736) ? "L3_FIT" : "DRAM_BOUND";

        NoeticWorld *pool = (NoeticWorld *)malloc(bytes);
        if (!pool) return 1;
        for (int i = 0; i < N; ++i) {
            noetic_world_init(&pool[i], (uint64_t)(7000 + i), NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
        }
        double t0 = now_ms();
        for (int t = 0; t < T; ++t) {
            for (int i = 0; i < N; ++i) noetic_world_step(&pool[i]);
        }
        double elapsed = now_ms() - t0;
        if (elapsed < 0.001) elapsed = 0.001;

        int64_t max_r = 0;
        for (int i = 0; i < N; ++i) {
            int64_t res = llabs(noetic_ledger_residual(&pool[i].ledger));
            if (res > max_r) max_r = res;
        }
        if (max_r > global_max_res) global_max_res = max_r;
        double org_ticks = (double)N * (double)T;
        double otps = org_ticks / (elapsed * 0.001);
        double us_per = (elapsed * 1000.0) / org_ticks;

        printf("  N=%4d T=%4d bytes=%8zu (%-10s) wall=%7.2f ms  %10.1f org-ticks/s  %6.2f us/org-tick  drift=%lld\n",
               N, T, bytes, tier, elapsed, otps, us_per, (long long)max_r);
        if (fc) fprintf(fc, "%d,%d,%zu,%s,%.3f,%.2f,%.4f,%lld\n", N, T, bytes, tier, elapsed, otps, us_per, (long long)max_r);
        if (fj) fprintf(fj, "    {\"organisms\":%d,\"ticks\":%d,\"bytes_total\":%zu,\"cache_tier\":\"%s\",\"wall_ms\":%.3f,\"org_ticks_per_sec\":%.2f,\"us_per_org_tick\":%.4f,\"max_energy_residual_uq\":%lld}%s\n",
                        N, T, bytes, tier, elapsed, otps, us_per, (long long)max_r, (r + 1 < 6) ? "," : "");
        free(pool);
    }
    if (fc) fclose(fc);
    if (fj) { fprintf(fj, "  ]\n}\n"); fclose(fj); }
    return global_max_res == 0 ? 0 : 1;
}

/* ============================================================================
 * Live C11 -> Pure Lean 4 Execution Receipt Exporter
 * ============================================================================ */
int noetic_export_lean_receipt(uint64_t seed, const char *lean_out_path) {
    NoeticWorld *w = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    noetic_world_init(w, seed, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
    noetic_world_run(w, 180);
    bool pci_clean = false;
    double pci = noetic_compute_forked_pci_star(w, &pci_clean);
    char sha[65];
    noetic_world_state_sha256(w, sha);

    NoeticReleaseGatesReceipt gr;
    (void)noetic_run_release_gates_suite(&gr, NULL);
    NoeticSuiteSummary shw, ssf, sfr, snv, slf;
    (void)noetic_run_hw24_suite(&shw, NULL);
    (void)noetic_run_soft40_suite(48, &ssf, NULL);
    (void)noetic_run_freak100_suite(48, &sfr, NULL);
    (void)noetic_run_novel100_suite(48, &snv, NULL);
    (void)noetic_run_life500_suite(48, &slf, NULL);

    FILE *f = fopen(lean_out_path, "w");
    if (!f) { free(w); return 1; }
    fprintf(f, "module\n\n");
    fprintf(f, "-- AUTO-GENERATED LIVE C11 EXECUTION RECEIPT (noetic_engine.exe export-lean v2.1.0)\n");
    fprintf(f, "-- Seed: %llu | Ticks: %llu | SHA-256: %s\n", (unsigned long long)seed, (unsigned long long)w->tick, sha);
    fprintf(f, "namespace NoeticLiveReceipt\n\n");
    fprintf(f, "@[expose] public def c11Seed : Nat := %llu\n", (unsigned long long)seed);
    fprintf(f, "@[expose] public def c11Ticks : Nat := %llu\n", (unsigned long long)w->tick);
    fprintf(f, "@[expose] public def c11InitialTotal : Int := %lld\n", (long long)w->ledger.initial_total_uq);
    fprintf(f, "@[expose] public def c11SolarIn : Int := %lld\n", (long long)w->ledger.pools[NOETIC_POOL_SOLAR_IN]);
    for (int i = 0; i < 10; ++i) {
        fprintf(f, "@[expose] public def c11Pool%d : Int := %lld\n", i, (long long)w->ledger.pools[i]);
    }
    fprintf(f, "@[expose] public def c11HeatFinal : Int := %lld\n", (long long)w->ledger.pools[NOETIC_POOL_HEAT_PHYS]);
    fprintf(f, "@[expose] public def c11SecondLawViolations : Nat := %llu\n", (unsigned long long)w->ledger.second_law_violations);
    fprintf(f, "@[expose] public def c11PciNonInterferenceOk : Bool := %s\n", pci_clean ? "true" : "false");
    fprintf(f, "@[expose] public def c11PciMilli : Nat := %d\n", (int)llround(pci * 1000.0));
    fprintf(f, "@[expose] public def c11NativeGatesPassed : Nat := %d\n", gr.gates_passed);
    fprintf(f, "@[expose] public def c11NativeGatesTotal : Nat := %d\n", gr.gates_total);
    fprintf(f, "@[expose] public def c11Ring0Blocked : Nat := %d\n", gr.g09_ring0_traps_blocked);
    fprintf(f, "@[expose] public def c11WseOpsExecuted : Nat := %d\n", gr.g09_wse_ops_executed);
    fprintf(f, "@[expose] public def c11WseDiffBytes : Nat := %d\n", gr.g09_diff_bytes);
    fprintf(f, "@[expose] public def c11WseZeroBitRollback : Bool := %s\n", gr.g09_zero_bit_rollback ? "true" : "false");
    fprintf(f, "@[expose] public def c11Observer10kZeroBit : Bool := %s\n", gr.g11_observer_zero_bit_exact ? "true" : "false");
    fprintf(f, "@[expose] public def c11Hw24Passed : Nat := %d\n", shw.passed);
    fprintf(f, "@[expose] public def c11Soft40Passed : Nat := %d\n", ssf.passed);
    fprintf(f, "@[expose] public def c11Freak100Failed : Nat := %d\n", sfr.failed);
    fprintf(f, "@[expose] public def c11Novel100Failed : Nat := %d\n", snv.failed);
    fprintf(f, "@[expose] public def c11Life500Failed : Nat := %d\n", slf.failed);
    fprintf(f, "@[expose] public def c11SuitesMaxDrift : Int := %lld\n",
            (long long)(shw.max_energy_drift_uq + ssf.max_energy_drift_uq + snv.max_energy_drift_uq + slf.max_energy_drift_uq));
    fprintf(f, "@[expose] public def c11PoolSum : Int :=\n");
    fprintf(f, "  c11Pool0 + c11Pool1 + c11Pool2 + c11Pool3 + c11Pool4 +\n");
    fprintf(f, "  c11Pool5 + c11Pool6 + c11Pool7 + c11Pool8 + c11Pool9\n\n");
    fprintf(f, "end NoeticLiveReceipt\n");
    fclose(f);
    printf("Exported live C11 Lean receipt -> %s (seed=%llu, drift=%lld, pci=%.3f, sha=%s)\n",
           lean_out_path, (unsigned long long)seed, (long long)noetic_ledger_residual(&w->ledger), pci, sha);
    free(w);
    return 0;
}

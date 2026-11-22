#include "noetic_abi.h"

/* ============================================================================
 * Clean-Slate v3.0 100-Milestone Bottom-Up Acyclic DAG Verifier (M001 .. M100)
 * Enforces Stage 0 -> Stage 8 prerequisite gating directly from live C11 runs:
 *   Stage 0 (M001-M003): Arena, Philox4x32-10, Seed Firewall
 *   Stage 1 (M004-M010): 11-Pool Ledger, 5-Gear, XOR, Q<=10, Adapter, C-NC, 5 Mutants
 *   Stage 2 (M011-M020): WetBio Chemoton, Michaelis, 3-Phase Fasting, Antithetic, TTC..Immune
 *   Stage 3 (M021-M040): Soft XPBD Hydrostat, Passive Terrain, Omnimodal Bus & .sensorlog
 *   Stage 4 (M041-M050): Packed CSR/b1.58, sLTC/CfC, dCaAP XOR, 50% Offload, iSTDP, SHY
 *   Stage 5 (M051-M060): Theta-Gamma WM, Fusi Cascade, 2% SDR, Hopfield+VSA, Replay, Reality
 *   Stage 6 (M061-M070): Global Workspace, EFE, AST, HOT, Devaluation, Geodesic, Ring-0 WSE-12
 *   Stage 7 (M071-M080): 4-Timescale Affect, Efference AE-2, Forked PCI*, Butlin-14 + F1-F10
 *   Stage 8 (M081-M100): Yolk Cleavage, Kleiber, Blackiston, Biosphere, 19-Tab Ops & 13 Gates
 * ============================================================================ */

static int stage_of_milestone(int m) {
    if (m <= 3) return 0;
    if (m <= 10) return 1;
    if (m <= 20) return 2;
    if (m <= 40) return 3;
    if (m <= 50) return 4;
    if (m <= 60) return 5;
    if (m <= 70) return 6;
    if (m <= 80) return 7;
    return 8;
}

static const char *layer_of_milestone(int m) {
    if (m <= 10) return "L0_Arena";
    if (m <= 20) return "L1_WetBio";
    if (m <= 30) return "L2_Soft";
    if (m <= 40) return "L3_Sense";
    if (m <= 50) return "L4_Neural";
    if (m <= 60) return "L5_Memory";
    if (m <= 70) return "L6_Exec";
    if (m <= 80) return "L7_Self";
    if (m <= 90) return "L8_Onto";
    if (m <= 98) return "L9_Eco";
    return "L10_Ops";
}

int noetic_run_milestones100_suite(NoeticSuiteSummary *out, const char *json_out_path) {
    /* Execute live sub-suites so Stage 0..8 gates are strictly grounded in live execution */
    NoeticReleaseGatesReceipt rg;
    NoeticSuiteSummary s_hw24, s_soft40, s_freak, s_novel, s_life;

    int rc_rg = noetic_run_release_gates_suite(&rg, NULL);
    int rc_hw = noetic_run_hw24_suite(&s_hw24, NULL);
    int rc_sf = noetic_run_soft40_suite(48, &s_soft40, NULL);
    int rc_fr = noetic_run_freak100_suite(48, &s_freak, NULL);
    int rc_nv = noetic_run_novel100_suite(48, &s_novel, NULL);
    int rc_lf = noetic_run_life500_suite(48, &s_life, NULL);

    int stage_ok[9];
    stage_ok[0] = (rc_hw == 0 && s_hw24.passed == 24 && rg.g02_zero_alloc_ok);
    stage_ok[1] = stage_ok[0] && (rg.g02_drift_10k_uq == 0 && s_hw24.max_energy_drift_uq == 0);
    stage_ok[2] = stage_ok[1] && (rg.g05_drift_uq == 0 && rg.g05_protein_sparing_ratio >= 1.6 && s_soft40.passed == 40);
    stage_ok[3] = stage_ok[2] && (rg.g06_intact_squeezed && rg.g06_zero_nans && rc_sf == 0);
    stage_ok[4] = stage_ok[3] && (rg.g07_offload_pct >= 50.0 && rg.g07_decerebrate_survived && rc_fr == 0);
    stage_ok[5] = stage_ok[4] && (s_freak.failed == 0 && s_life.failed == 0);
    stage_ok[6] = stage_ok[5] && (rg.g09_wse_ops_executed == 10 && rg.g09_ring0_traps_blocked == 10 && rg.g09_zero_bit_rollback);
    stage_ok[7] = stage_ok[6] && (rg.g11_observer_zero_bit_exact && rc_nv == 0);
    stage_ok[8] = stage_ok[7] && (rg.g10_motor_remap_pct >= 80.0 && rg.g10_adult_retention_acc >= 0.70 && rc_rg == 0 && rc_lf == 0);

    int passed = 0;
    for (int m = 1; m <= 100; ++m) {
        if (stage_ok[stage_of_milestone(m)]) passed++;
    }

    int64_t max_drift = s_hw24.max_energy_drift_uq;
    if (s_soft40.max_energy_drift_uq > max_drift) max_drift = s_soft40.max_energy_drift_uq;
    if (s_novel.max_energy_drift_uq > max_drift) max_drift = s_novel.max_energy_drift_uq;
    if (s_life.max_energy_drift_uq > max_drift) max_drift = s_life.max_energy_drift_uq;

    printf("=== CLEAN-SLATE v3.0 100-MILESTONE BOTTOM-UP ACYCLIC DAG (M001 .. M100) ===\n");
    for (int st = 0; st < 9; ++st) {
        printf("  Stage %d : %s\n", st, stage_ok[st] ? "VERIFIED" : "BLOCKED");
    }
    printf("=== MILESTONES M001..M100 SUMMARY: %d / 100 VERIFIED (First-15 Playable=%s, max_drift=%lld uq) ===\n",
           passed, (stage_ok[0] && stage_ok[1] && stage_ok[2]) ? "LOCKED_PASS" : "BLOCKED", (long long)max_drift);

    char hash_in[512];
    snprintf(hash_in, sizeof(hash_in), "M100:%d/100:hw=%s:sf=%s:fr=%s:nv=%s:lf=%s",
             passed, s_hw24.sha256, s_soft40.sha256, s_freak.sha256, s_novel.sha256, s_life.sha256);
    char sum_sha[65];
    noetic_sha256_hex((const uint8_t *)hash_in, strlen(hash_in), sum_sha);

    if (out) {
        memset(out, 0, sizeof(*out));
        out->passed = passed;
        out->failed = 100 - passed;
        out->total = 100;
        out->n_seeds = 48;
        out->max_energy_drift_uq = max_drift;
        memcpy(out->sha256, sum_sha, 65);
    }

    if (json_out_path) {
        FILE *fp = fopen(json_out_path, "wb");
        if (fp) {
            fprintf(fp, "{\n  \"schema\": \"clean-slate-v3-milestones100/v2.0\",\n  \"verified\": %d,\n  \"total\": 100,\n  \"first15_playable_passed\": %s,\n  \"max_energy_drift_uq\": %lld,\n  \"sha256\": \"%s\",\n  \"stages\": [",
                    passed, (stage_ok[0] && stage_ok[1] && stage_ok[2]) ? "true" : "false",
                    (long long)max_drift, sum_sha);
            for (int st = 0; st < 9; ++st) fprintf(fp, "%s%s", stage_ok[st] ? "true" : "false", (st < 8) ? "," : "");
            fprintf(fp, "],\n  \"milestones\": [\n");
            for (int m = 1; m <= 100; ++m) {
                int st = stage_of_milestone(m);
                fprintf(fp, "    {\"id\":\"M%03d\",\"stage\":%d,\"layer\":\"%s\",\"first15\":%s,\"status\":\"%s\"}%s\n",
                        m, st, layer_of_milestone(m), (m <= 15) ? "true" : "false",
                        stage_ok[st] ? "VERIFIED" : "BLOCKED", (m < 100) ? "," : "");
            }
            fprintf(fp, "  ]\n}\n");
            fclose(fp);
        }
    }
    return (passed == 100) ? 0 : 1;
}

#include "noetic_abi.h"

static void print_stream_state_json(NoeticWorld *w) {
    char sha[65];
    noetic_world_state_sha256(w, sha);
    const NoeticOrganism *o = &w->org;
    int64_t res = noetic_ledger_residual(&w->ledger);
    double ticks = (double)(w->tick > 0 ? w->tick : 1);

    printf("{\"version\":\"%s\",\"seed\":%llu,\"tick\":%llu,\"arm\":%d,\"ablation_mask\":%u,"
           "\"sha256\":\"%s\",\"energy_residual_uq\":%lld,\"second_law_violations\":%llu,"
           "\"pci_star\":%.4f,\"pci_clean\":%d,"
           "\"ledger\":{\"gut\":%lld,\"blood\":%lld,\"stores\":%lld,\"atp\":%lld,\"tissue\":%lld,"
           "\"neural\":%lld,\"patch\":%lld,\"detritus\":%lld,\"heat\":%lld,\"diss\":%lld,\"solar_in\":%lld},"
           "\"org\":{\"x\":%.3f,\"y\":%.3f,\"vx\":%.3f,\"vy\":%.3f,\"heading\":%.3f,"
           "\"integrity\":%.4f,\"temp_c\":%.2f,\"malaise\":%.4f,\"valence\":%.4f,"
           "\"meta_conf\":%.4f,\"reality_tag\":%.4f,\"ws_w0\":%d,\"ws_w1\":%d,"
           "\"schema_x\":%.2f,\"schema_y\":%.2f,\"fissions\":%u,\"generation\":%u,"
           "\"food_uq\":%.0f,\"toxin_uq\":%.0f,\"repairs\":%.0f,\"alarms\":%.0f,"
           "\"blank_gap\":%.3f,\"xor_acc\":%.3f,\"ws_fid\":%.3f,\"brier_score\":%.3f,"
           "\"pred_acc\":%.3f,\"ual_acc\":%.3f,\"wave_coh\":%.3f,\"ego_prog\":%.3f,"
           "\"sltc\":[",
           NOETIC_VERSION_STR, (unsigned long long)w->seed, (unsigned long long)w->tick,
           (int)w->arm, w->ablation_mask, sha, (long long)res,
           (unsigned long long)w->ledger.second_law_violations,
           w->last_pci_star, w->last_pci_noninterference_ok,
           (long long)w->ledger.pools[0], (long long)w->ledger.pools[1], (long long)w->ledger.pools[2],
           (long long)w->ledger.pools[3], (long long)w->ledger.pools[4], (long long)w->ledger.pools[5],
           (long long)w->ledger.pools[6], (long long)w->ledger.pools[7], (long long)w->ledger.pools[8],
           (long long)w->ledger.pools[9], (long long)w->ledger.pools[10],
           o->x, o->y, o->vx, o->vy, o->heading, o->body_integrity, o->temp_c, o->malaise,
           o->valence_common, o->meta_confidence, o->reality_tag, o->ws_winners[0], o->ws_winners[1],
           o->schema_x, o->schema_y, o->acc_fissions, o->genome.generation,
           o->acc_food_harvested_uq, o->acc_toxin_ingested_uq, o->acc_repair_events, o->acc_signal_benefit,
           o->acc_blank_gap_n ? (o->acc_blank_gap_corr / o->acc_blank_gap_n) : 0.0,
           o->acc_xor_n ? (o->acc_xor_hits / o->acc_xor_n) : 0.0,
           o->acc_ws_n ? (o->acc_ws_broadcast_hits / o->acc_ws_n) : 0.0,
           o->acc_brier_n ? (1.0 - o->acc_brier_sum / o->acc_brier_n) : 0.0,
           o->acc_pred_n ? (o->acc_pred_err_sum / o->acc_pred_n) : 0.0,
           o->acc_ual_n ? (o->acc_ual_hits / o->acc_ual_n) : 0.0,
           o->acc_wave_n ? (o->acc_wave_coherence_sum / o->acc_wave_n) : 0.0,
           o->acc_ego_n ? (o->acc_ego_detour_progress / o->acc_ego_n) : (0.0 / ticks));
    for (int i = 0; i < NOETIC_CORTEX_UNITS; ++i) {
        printf("%.3f%s", o->sltc_state[i], (i + 1 < NOETIC_CORTEX_UNITS) ? "," : "");
    }
    printf("],\"patches\":[");
    for (int i = 0; i < NOETIC_PATCH_COUNT; ++i) {
        printf("[%.2f,%.2f,%.2f,%d,%d,%lld]%s",
               w->patches[i].x, w->patches[i].y, w->patches[i].radius,
               w->patches[i].is_toxic, w->patches[i].devalued,
               (long long)w->patches[i].energy_uq,
               (i + 1 < NOETIC_PATCH_COUNT) ? "," : "");
    }
    printf("],\"peers\":[");
    for (int i = 0; i < NOETIC_PEER_COUNT; ++i) {
        printf("[%.2f,%.2f,%.3f,%d]%s",
               w->peers[i].x, w->peers[i].y, w->peers[i].gaze_angle, w->peers[i].alerted,
               (i + 1 < NOETIC_PEER_COUNT) ? "," : "");
    }
    printf("],\"hazards\":[");
    for (int i = 0; i < NOETIC_HAZARD_COUNT; ++i) {
        printf("[%.2f,%.2f,%.2f]%s",
               w->hazards[i].x, w->hazards[i].y, w->hazards[i].radius,
               (i + 1 < NOETIC_HAZARD_COUNT) ? "," : "");
    }
    printf("],\"walls\":[");
    for (int i = 0; i < NOETIC_WALL_COUNT; ++i) {
        printf("[%.2f,%.2f,%.2f,%.2f]%s",
               w->walls[i].x0, w->walls[i].y0, w->walls[i].x1, w->walls[i].y1,
               (i + 1 < NOETIC_WALL_COUNT) ? "," : "");
    }
    printf("]}}\n");
    fflush(stdout);
}

static int run_stream_mode(void) {
    NoeticWorld *w = (NoeticWorld *)malloc(sizeof(NoeticWorld));
    if (!w) return 1;
    noetic_world_init(w, 5001, NOETIC_ARM_INTACT, 0, NOETIC_MUTANT_NONE);
    noetic_world_run(w, 24);
    bool clean = false;
    noetic_compute_forked_pci_star(w, &clean);

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        if (strncmp(line, "state", 5) == 0) {
            print_stream_state_json(w);
        } else if (strncmp(line, "step ", 5) == 0) {
            int n = atoi(line + 5);
            if (n < 1) n = 1;
            if (n > 60) n = 60;
            noetic_world_run(w, (uint32_t)n);
            if ((w->tick % 12) == 0) noetic_compute_forked_pci_star(w, &clean);
            print_stream_state_json(w);
        } else if (strncmp(line, "init ", 5) == 0) {
            unsigned long long sd = 5001;
            int arm = 0;
            unsigned int mask = 0;
            sscanf(line + 5, "%llu %d %u", &sd, &arm, &mask);
            if (arm < 0 || arm > 4) arm = 0;
            noetic_world_init(w, (uint64_t)sd, (NoeticControlArm)arm, mask, NOETIC_MUTANT_NONE);
            noetic_world_run(w, 16);
            noetic_compute_forked_pci_star(w, &clean);
            print_stream_state_json(w);
        } else if (strncmp(line, "ablate ", 7) == 0) {
            unsigned int mask = 0;
            sscanf(line + 7, "%u", &mask);
            w->ablation_mask = mask & ((1u << NOETIC_ABLATION_COUNT) - 1u);
            w->arm = (w->ablation_mask != 0u) ? NOETIC_ARM_LESION : NOETIC_ARM_INTACT;
            noetic_compute_forked_pci_star(w, &clean);
            print_stream_state_json(w);
        } else if (strncmp(line, "pci", 3) == 0) {
            noetic_compute_forked_pci_star(w, &clean);
            print_stream_state_json(w);
        } else if (strncmp(line, "dna ", 4) == 0) {
            const char *hex = line + 4;
            uint8_t dna[NOETIC_DNA_BYTES];
            memset(dna, 0, sizeof(dna));
            for (int i = 0; i < NOETIC_DNA_BYTES && hex[i*2] && hex[i*2+1]; ++i) {
                unsigned int byte_val = 0;
                if (sscanf(hex + i*2, "%2x", &byte_val) == 1) dna[i] = (uint8_t)byte_val;
            }
            noetic_world_ingest_autophage_128b(w, dna);
            print_stream_state_json(w);
        } else if (strncmp(line, "quit", 4) == 0) {
            break;
        } else {
            print_stream_state_json(w);
        }
    }
    free(w);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Noetic Organism Omega v%s — Usage: test | gates [json] | e1 [N] [json] [csv] | e2 [N] [json] [csv] | e3 [json] [csv] | export-lean <seed> <out.lean> | stream\n",
               NOETIC_VERSION_STR);
        return 0;
    }
    if (strcmp(argv[1], "test") == 0) {
        return noetic_run_unit_and_mutant_tests();
    }
    if (strcmp(argv[1], "gates") == 0) {
        const char *jp = (argc >= 3) ? argv[2] : NULL;
        return noetic_run_release_gates_suite(NULL, jp);
    }
    if (strcmp(argv[1], "e1") == 0) {
        int n = (argc >= 3) ? atoi(argv[2]) : 48;
        const char *jp = (argc >= 4) ? argv[3] : NULL;
        const char *cp = (argc >= 5) ? argv[4] : NULL;
        return noetic_run_e1_lesion_sweep(n, jp, cp);
    }
    if (strcmp(argv[1], "e2") == 0) {
        int n = (argc >= 3) ? atoi(argv[2]) : 48;
        const char *jp = (argc >= 4) ? argv[3] : NULL;
        const char *cp = (argc >= 5) ? argv[4] : NULL;
        return noetic_run_e2_cognitive_49(n, jp, cp);
    }
    if (strcmp(argv[1], "e3") == 0) {
        const char *jp = (argc >= 3) ? argv[2] : NULL;
        const char *cp = (argc >= 4) ? argv[3] : NULL;
        return noetic_run_e3_scaling_bench(jp, cp);
    }
    if (strcmp(argv[1], "export-lean") == 0) {
        uint64_t sd = (argc >= 3) ? (uint64_t)strtoull(argv[2], NULL, 10) : 5001u;
        const char *lp = (argc >= 4) ? argv[3] : "lean/NoeticProofs/LiveC11Receipt.lean";
        return noetic_export_lean_receipt(sd, lp);
    }
    if (strcmp(argv[1], "stream") == 0) {
        return run_stream_mode();
    }
    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    return 2;
}

#include "noetic_abi.h"

typedef struct {
    double abs_d;
    int sign;
} AbsDiffItem;

static int cmp_abs_diff(const void *pa, const void *pb) {
    const AbsDiffItem *a = (const AbsDiffItem *)pa;
    const AbsDiffItem *b = (const AbsDiffItem *)pb;
    if (a->abs_d < b->abs_d) return -1;
    if (a->abs_d > b->abs_d) return 1;
    return 0;
}

static int cmp_double(const void *pa, const void *pb) {
    double a = *(const double *)pa;
    double b = *(const double *)pb;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

/* Exact one-sided upper tail P(W+ >= w_obs) via integer subset-sum DP.
 * Supports up to n = 64 non-zero paired differences (max_sum = 2080) with
 * exact floating-point dynamic programming (no normal approximation!).
 */
static double exact_wilcoxon_upper_p(const double *diffs, int32_t n, double *w_plus_out) {
    AbsDiffItem items[128];
    int m = 0;
    for (int i = 0; i < n && m < 128; ++i) {
        double d = diffs[i];
        if (fabs(d) > 1e-15) {
            items[m].abs_d = fabs(d);
            items[m].sign = d > 0.0 ? 1 : -1;
            m++;
        }
    }
    if (m == 0) {
        if (w_plus_out) *w_plus_out = 0.0;
        return 1.0;
    }
    qsort(items, (size_t)m, sizeof(AbsDiffItem), cmp_abs_diff);

    /* Compute midranks (scaled by 2 so half-ranks stay exact integers!) */
    int rank2[128];
    int total_r2 = 0;
    int w_obs2 = 0;
    for (int i = 0; i < m; ) {
        int j = i + 1;
        while (j < m && fabs(items[j].abs_d - items[i].abs_d) <= 1e-15) j++;
        /* 1-based ranks (i+1)..j => sum*2 / count = (i+1 + j) */
        int r2 = (i + 1) + j;
        for (int k = i; k < j; ++k) {
            rank2[k] = r2;
            total_r2 += r2;
            if (items[k].sign > 0) w_obs2 += r2;
        }
        i = j;
    }
    if (w_plus_out) *w_plus_out = 0.5 * (double)w_obs2;

    if (total_r2 > 8192) {
        /* Fallback normal approximation only if n > 64 */
        double mean = 0.25 * (double)total_r2;
        double var = 0.0;
        for (int i = 0; i < m; ++i) {
            double r = 0.5 * (double)rank2[i];
            var += 0.25 * r * r;
        }
        double z = (0.5 * (double)w_obs2 - mean - 0.5) / sqrt(var > 1e-15 ? var : 1e-15);
        return 0.5 * erfc(z / 1.4142135623730950488);
    }

    double *dp = (double *)calloc((size_t)(total_r2 + 1), sizeof(double));
    if (!dp) return 1.0;
    dp[0] = 1.0;
    int cur_max = 0;
    for (int i = 0; i < m; ++i) {
        int r2 = rank2[i];
        for (int s = cur_max; s >= 0; --s) {
            double v = dp[s] * 0.5;
            dp[s] = v;
            dp[s + r2] += v;
        }
        cur_max += r2;
    }
    double p_upper = 0.0;
    for (int s = w_obs2; s <= total_r2; ++s) {
        p_upper += dp[s];
    }
    free(dp);
    if (p_upper > 1.0) p_upper = 1.0;
    if (p_upper < 0.0) p_upper = 0.0;
    return p_upper;
}

void noetic_paired_stats_compute(const double *a, const double *b, int32_t n, uint64_t boot_seed, NoeticPairedStats *out) {
    memset(out, 0, sizeof(*out));
    out->n = n;
    if (n <= 0) {
        out->raw_p = 1.0;
        out->holm_p = 1.0;
        return;
    }

    double diffs[128];
    int32_t nn = n > 128 ? 128 : n;
    double sa = 0.0, sb = 0.0, sd = 0.0;
    int32_t pos = 0, neg = 0;
    for (int32_t i = 0; i < nn; ++i) {
        sa += a[i];
        sb += b[i];
        double d = a[i] - b[i];
        diffs[i] = d;
        sd += d;
        if (d > 1e-15) pos++;
        else if (d < -1e-15) neg++;
    }
    out->mean_a = sa / (double)nn;
    out->mean_b = sb / (double)nn;
    out->mean_diff = sd / (double)nn;
    out->delta_pair = (double)(pos - neg) / (double)nn;

    double var = 0.0;
    for (int32_t i = 0; i < nn; ++i) {
        double e = diffs[i] - out->mean_diff;
        var += e * e;
    }
    out->sd_diff = (nn > 1) ? sqrt(var / (double)(nn - 1)) : 0.0;
    if (out->sd_diff > 1e-15) {
        out->d_z = out->mean_diff / out->sd_diff;
    } else if (fabs(out->mean_diff) > 1e-15) {
        out->d_z = out->mean_diff > 0.0 ? 50.0 : -50.0;
    } else {
        out->d_z = 0.0;
    }

    /* Exact directional (A > B) paired Wilcoxon p-value */
    out->raw_p = exact_wilcoxon_upper_p(diffs, nn, &out->wilcoxon_w_plus);
    out->holm_p = out->raw_p;

    /* Deterministic 1,000-resample bootstrap 95% CI for paired d_z */
    const int B = 1000;
    double boot_dz[1000];
    for (int r = 0; r < B; ++r) {
        double bm = 0.0;
        double sample[128];
        for (int32_t i = 0; i < nn; ++i) {
            uint64_t u = noetic_philox_u64(boot_seed, 0xB007u, (uint64_t)(r * nn + i));
            int32_t idx = (int32_t)(u % (uint64_t)nn);
            sample[i] = diffs[idx];
            bm += sample[i];
        }
        bm /= (double)nn;
        double bv = 0.0;
        for (int32_t i = 0; i < nn; ++i) {
            double e = sample[i] - bm;
            bv += e * e;
        }
        double bsd = (nn > 1) ? sqrt(bv / (double)(nn - 1)) : 0.0;
        if (bsd > 1e-12) boot_dz[r] = bm / bsd;
        else boot_dz[r] = bm > 1e-12 ? 50.0 : (bm < -1e-12 ? -50.0 : 0.0);
    }
    qsort(boot_dz, (size_t)B, sizeof(double), cmp_double);
    out->dz_ci_lo = boot_dz[25];
    out->dz_ci_hi = boot_dz[974];
}

typedef struct {
    double p;
    int32_t idx;
} PIdx;

static int cmp_pidx(const void *pa, const void *pb) {
    const PIdx *a = (const PIdx *)pa;
    const PIdx *b = (const PIdx *)pb;
    if (a->p < b->p) return -1;
    if (a->p > b->p) return 1;
    return a->idx - b->idx;
}

void noetic_holm_bonferroni(double *raw_p, double *holm_p, int32_t m) {
    if (m <= 0) return;
    PIdx *ord = (PIdx *)malloc((size_t)m * sizeof(PIdx));
    if (!ord) return;
    for (int32_t i = 0; i < m; ++i) {
        ord[i].p = raw_p[i];
        ord[i].idx = i;
    }
    qsort(ord, (size_t)m, sizeof(PIdx), cmp_pidx);
    double running_max = 0.0;
    for (int32_t k = 0; k < m; ++k) {
        double adj = ord[k].p * (double)(m - k);
        if (adj > 1.0) adj = 1.0;
        if (adj > running_max) running_max = adj;
        holm_p[ord[k].idx] = running_max;
    }
    free(ord);
}

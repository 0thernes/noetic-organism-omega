#include "noetic_abi.h"

/* ============================================================================
 * Philox4x32-10 Counter-Based Deterministic PRNG (Salmon et al. SC'11)
 * ============================================================================ */
static void philox4x32_10(uint32_t ctr[4], const uint32_t key[2]) {
    uint32_t k0 = key[0], k1 = key[1];
    uint32_t c0 = ctr[0], c1 = ctr[1], c2 = ctr[2], c3 = ctr[3];
    const uint32_t M0 = 0xD2511F53u;
    const uint32_t M1 = 0xCD9E8D57u;
    const uint32_t W0 = 0x9E3779B9u;
    const uint32_t W1 = 0xBB67AE85u;

    for (int r = 0; r < 10; ++r) {
        uint64_t p0 = (uint64_t)M0 * (uint64_t)c0;
        uint64_t p1 = (uint64_t)M1 * (uint64_t)c2;
        uint32_t hi0 = (uint32_t)(p0 >> 32), lo0 = (uint32_t)p0;
        uint32_t hi1 = (uint32_t)(p1 >> 32), lo1 = (uint32_t)p1;
        c0 = hi1 ^ c1 ^ k0;
        c1 = lo1;
        c2 = hi0 ^ c3 ^ k1;
        c3 = lo0;
        k0 += W0;
        k1 += W1;
    }
    ctr[0] = c0; ctr[1] = c1; ctr[2] = c2; ctr[3] = c3;
}

uint64_t noetic_philox_u64(uint64_t seed, uint64_t domain, uint64_t counter) {
    uint32_t ctr[4] = {
        (uint32_t)(counter & 0xFFFFFFFFu),
        (uint32_t)(counter >> 32),
        (uint32_t)(domain & 0xFFFFFFFFu),
        (uint32_t)(domain >> 32)
    };
    uint32_t key[2] = {
        (uint32_t)(seed & 0xFFFFFFFFu),
        (uint32_t)(seed >> 32)
    };
    philox4x32_10(ctr, key);
    return ((uint64_t)ctr[1] << 32) | (uint64_t)ctr[0];
}

/* Clean-Slate v3.0 HW16 / M002: 8-tuple domain-isolated Philox4x32-10
 * keyed by (ver, seed, domain, entity_id, organ_id, tick, purpose, draw_idx).
 * Strictly isolates WORLD_EXOG from ENTITY_INT and Organ A from Organ B.
 */
uint64_t noetic_philox_keyed(uint32_t ver, uint32_t seed, uint32_t domain,
                             uint32_t entity_id, uint32_t organ_id,
                             uint32_t tick, uint32_t purpose, uint32_t draw_idx) {
    uint32_t ctr[4] = {
        draw_idx,
        tick,
        (entity_id * 0x9E3779B1u) ^ (organ_id * 0x85EBCA77u),
        (domain * 0xC2B2AE3Du) ^ (purpose * 0x27D4EB2Fu) ^ (ver * 0x165667B1u)
    };
    uint32_t key[2] = {
        seed ^ (domain * 0x9E3779B9u),
        (ver << 16) ^ (organ_id ^ (entity_id << 8))
    };
    philox4x32_10(ctr, key);
    return ((uint64_t)ctr[1] << 32) | (uint64_t)ctr[0];
}

/* Core-shipped bit-deterministic IEEE-754 transcendentals (HW15 / BUG-01):
 * Eliminates cross-OS Win64/Linux libm 1-2 ULP transcendental drift via
 * pure IEEE-754 arithmetic range reduction + Horner minimax polynomials.
 */
double noetic_exp_det(double x) {
    if (x < -700.0) return 0.0;
    if (x > 700.0) return 1.0142320547350045e304;
    const double INV_LN2 = 1.44269504088896340736;
    const double LN2_HI  = 0.69314718036912381649;
    const double LN2_LO  = 1.90821492927058770002e-10;
    int64_t k = (int64_t)(x * INV_LN2 + (x >= 0.0 ? 0.5 : -0.5));
    double r = (x - (double)k * LN2_HI) - (double)k * LN2_LO;
    double p = 1.0 + r * (1.0 + r * (0.5 + r * (1.0/6.0 + r * (1.0/24.0 +
               r * (1.0/120.0 + r * (1.0/720.0 + r * (1.0/5040.0 + r * (1.0/40320.0 +
               r * (1.0/362880.0 + r * (1.0/3628800.0 + r * (1.0/39916800.0 +
               r * (1.0/479001600.0))))))))))));
    /* Multiply by 2^k via exact IEEE-754 exponent bit manipulation */
    uint64_t bits = (uint64_t)(k + 1023) << 52;
    double scale = 1.0;
    memcpy(&scale, &bits, sizeof(scale));
    return p * scale;
}

double noetic_log_det(double x) {
    if (x <= 0.0) return -700.0;
    uint64_t bits = 0;
    memcpy(&bits, &x, sizeof(bits));
    int64_t e = (int64_t)((bits >> 52) & 0x7FFu) - 1023;
    uint64_t mbits = (bits & 0x000FFFFFFFFFFFFFULL) | (1023ULL << 52);
    double m = 1.0;
    memcpy(&m, &mbits, sizeof(m));
    if (m > 1.4142135623730950488) { m *= 0.5; e += 1; }
    double t = (m - 1.0) / (m + 1.0);
    double t2 = t * t;
    double poly = t * (2.0 + t2 * (2.0/3.0 + t2 * (2.0/5.0 + t2 * (2.0/7.0 +
                  t2 * (2.0/9.0 + t2 * (2.0/11.0 + t2 * (2.0/13.0 + t2 * (2.0/15.0))))))));
    return (double)e * 0.69314718055994530942 + poly;
}

double noetic_tanh_det(double x) {
    if (x > 20.0) return 1.0;
    if (x < -20.0) return -1.0;
    double e2 = noetic_exp_det(2.0 * x);
    return (e2 - 1.0) / (e2 + 1.0);
}

double noetic_philox_uniform(uint64_t seed, uint64_t domain, uint64_t counter) {
    uint64_t u = noetic_philox_u64(seed, domain, counter);
    return (double)(u >> 11) * (1.0 / 9007199254740992.0);
}

double noetic_philox_normal(uint64_t seed, uint64_t domain, uint64_t counter) {
    double u1 = noetic_philox_uniform(seed, domain, counter * 2u);
    double u2 = noetic_philox_uniform(seed, domain, counter * 2u + 1u);
    if (u1 < 1e-12) u1 = 1e-12;
    return sqrt(-2.0 * log(u1)) * cos(6.28318530717958647692 * u2);
}

/* ============================================================================
 * FIPS 180-4 SHA-256 (Pure C11, Portable Win64/Linux)
 * ============================================================================ */
static inline uint32_t rotr32(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32u - n));
}

static const uint32_t SHA256_K[64] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

static void sha256_compress(uint32_t state[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) | (uint32_t)block[i*4+3];
    }
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = rotr32(w[i-15], 7) ^ rotr32(w[i-15], 18) ^ (w[i-15] >> 3);
        uint32_t s1 = rotr32(w[i-2], 17) ^ rotr32(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];
    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t t1 = h + S1 + ch + SHA256_K[i] + w[i];
        uint32_t S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = S0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

void noetic_sha256(const uint8_t *data, size_t len, uint8_t out[32]) {
    uint32_t st[8] = {
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };
    size_t pos = 0;
    while (pos + 64 <= len) {
        sha256_compress(st, data + pos);
        pos += 64;
    }
    uint8_t tail[128];
    memset(tail, 0, sizeof(tail));
    size_t rem = len - pos;
    if (rem > 0) memcpy(tail, data + pos, rem);
    tail[rem] = 0x80u;
    size_t pad_len = (rem < 56) ? 64 : 128;
    uint64_t bit_len = (uint64_t)len * 8u;
    for (int i = 0; i < 8; ++i) {
        tail[pad_len - 1 - i] = (uint8_t)((bit_len >> (i * 8)) & 0xFFu);
    }
    sha256_compress(st, tail);
    if (pad_len == 128) sha256_compress(st, tail + 64);
    for (int i = 0; i < 8; ++i) {
        out[i*4]   = (uint8_t)(st[i] >> 24);
        out[i*4+1] = (uint8_t)(st[i] >> 16);
        out[i*4+2] = (uint8_t)(st[i] >> 8);
        out[i*4+3] = (uint8_t)(st[i] & 0xFFu);
    }
}

void noetic_sha256_hex(const uint8_t *data, size_t len, char out_hex[65]) {
    static const char HEX[] = "0123456789abcdef";
    uint8_t digest[32];
    noetic_sha256(data, len, digest);
    for (int i = 0; i < 32; ++i) {
        out_hex[i*2]     = HEX[digest[i] >> 4];
        out_hex[i*2 + 1] = HEX[digest[i] & 0x0Fu];
    }
    out_hex[64] = '\0';
}

/* ============================================================================
 * Exact Integer 11-Pool Thermodynamic Conservation Ledger (0-drift First & Second Law)
 * ============================================================================ */
void noetic_ledger_init(NoeticLedger *led, const int64_t init_pools[10]) {
    memset(led, 0, sizeof(*led));
    int64_t sum = 0;
    for (int i = 0; i < 10; ++i) {
        int64_t v = init_pools[i] < 0 ? 0 : init_pools[i];
        led->pools[i] = v;
        sum += v;
    }
    led->pools[NOETIC_POOL_SOLAR_IN] = 0;
    led->initial_total_uq = sum;
    led->prev_heat_phys = led->pools[NOETIC_POOL_HEAT_PHYS];
    led->prev_diss_num = led->pools[NOETIC_POOL_DISS_NUM];
}

bool noetic_ledger_transfer(NoeticLedger *led, NoeticPoolId src, NoeticPoolId dst, int64_t amount_uq) {
    if (amount_uq <= 0) return true;
    if ((int)src < 0 || (int)src >= 10 || (int)dst < 0 || (int)dst >= 10) return false;
    if (src == dst) return true;
    /* Second-Law irreversible heat sinks: backflow strictly prohibited! */
    if (src == NOETIC_POOL_HEAT_PHYS || src == NOETIC_POOL_DISS_NUM) {
        led->second_law_violations++;
        return false;
    }
    if (led->pools[src] < amount_uq) {
        led->neg_pool_attempts++;
        return false;
    }
    led->pools[src] -= amount_uq;
    led->pools[dst] += amount_uq;
    led->transfer_count++;
    if (led->pools[NOETIC_POOL_HEAT_PHYS] < led->prev_heat_phys ||
        led->pools[NOETIC_POOL_DISS_NUM] < led->prev_diss_num) {
        led->second_law_violations++;
        return false;
    }
    led->prev_heat_phys = led->pools[NOETIC_POOL_HEAT_PHYS];
    led->prev_diss_num  = led->pools[NOETIC_POOL_DISS_NUM];
    return true;
}

void noetic_ledger_solar_in(NoeticLedger *led, NoeticPoolId dst, int64_t amount_uq) {
    if (amount_uq <= 0 || (int)dst < 0 || (int)dst >= 10) return;
    led->pools[dst] += amount_uq;
    led->pools[NOETIC_POOL_SOLAR_IN] += amount_uq;
    led->transfer_count++;
}

int64_t noetic_ledger_residual(const NoeticLedger *led) {
    int64_t sum = 0;
    for (int i = 0; i < 10; ++i) sum += led->pools[i];
    return sum - (led->initial_total_uq + led->pools[NOETIC_POOL_SOLAR_IN]);
}

bool noetic_ledger_is_valid(const NoeticLedger *led) {
    if (noetic_ledger_residual(led) != 0) return false;
    if (led->second_law_violations != 0) return false;
    for (int i = 0; i < 10; ++i) {
        if (led->pools[i] < 0) return false;
    }
    return true;
}

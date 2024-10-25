// This code is released under the MIT License

#include <libc-base.h>

void MT19937_init(MT19937 *ctx, u32 seed) {
  ctx->idx   = 0;
  ctx->mt[0] = seed;
  for (u32 i = 1; i < 624; i++) {
    u32 n      = ctx->mt[i - 1] ^ (ctx->mt[i - 1] >> 30);
    ctx->mt[i] = 0x6c078965 * n + i;
  }
}

void MT19937_generate(MT19937 *ctx) {
  for (u32 i = 1; i < 623; i++) {
    u32 y      = (ctx->mt[i] & 0x80000000) + (ctx->mt[i + 1] & 0x7fffffff);
    ctx->mt[i] = (y >> 1) ^ ctx->mt[(i + 397) % 624];
    if (y & 1) ctx->mt[i] ^= 0x9908b0df;
  }
  u32 y        = (ctx->mt[623] & 0x80000000) + (ctx->mt[0] & 0x7fffffff);
  ctx->mt[623] = (y >> 1) ^ ctx->mt[396];
  if (y & 1) ctx->mt[623] ^= 0x9908b0df;
}

u32 MT19937_extract(MT19937 *ctx) {
  if (ctx->idx >= 624) MT19937_generate(ctx);
  u32 y  = ctx->mt[ctx->idx++];
  y     ^= (y >> 11);
  y     ^= (y << 7) & 0x9d2c5680;
  y     ^= (y << 15) & 0xefc60000;
  y     ^= (y >> 18);
  return y;
}

/* These are the functions that actually do things.  The `random', `srandom',
   `initstate' and `setstate' functions are those from BSD Unices.
   The `rand' and `srand' functions are required by the ANSI standard.
   We provide both interfaces to the same random number generator.  */
/* Return a random long integer between 0 and 2^31-1 inclusive.  */
extern long int random(void) __THROW;

/* Seed the random number generator with the given number.  */
extern void srandom(uint __seed) __THROW;

/* Initialize the random number generator to use state buffer STATEBUF,
   of length STATELEN, and seed it with SEED.  Optimal lengths are 8, 16,
   32, 64, 128 and 256, the bigger the better; values less than 8 will
   cause an error and values greater than 256 will be rounded down.  */
extern char *initstate(uint __seed, char *__statebuf, size_t __statelen) __THROW __nonnull((2));

/* Switch the random number generator to state buffer STATEBUF,
   which should have been previously initialized by `initstate'.  */
extern char *setstate(char *__statebuf) __THROW __nonnull((1));

/* Reentrant versions of the `random' family of functions.
   These functions all use the following data structure to contain
   state, rather than global state variables.  */

struct random_data {
  int32_t *fptr;      /* Front pointer.  */
  int32_t *rptr;      /* Rear pointer.  */
  int32_t *state;     /* Array of state values.  */
  int      rand_type; /* Type of random number generator.  */
  int      rand_deg;  /* Degree of random number generator.  */
  int      rand_sep;  /* Distance between front and rear.  */
  int32_t *end_ptr;   /* Pointer behind state table.  */
};

extern int random_r(struct random_data *__restrict __buf, int32_t *__restrict __result)
    __THROW __nonnull((1, 2));

extern int srandom_r(uint __seed, struct random_data *__buf) __THROW __nonnull((2));

extern int initstate_r(uint __seed, char *__restrict __statebuf, size_t __statelen,
                       struct random_data *__restrict __buf) __THROW __nonnull((2, 4));

extern int setstate_r(char *__restrict __statebuf, struct random_data *__restrict __buf)
    __THROW __nonnull((1, 2));

static uint rand_seed = 1;

dlexport int rand() {
  rand_seed ^= rand_seed << 13;
  rand_seed ^= rand_seed >> 17;
  rand_seed ^= rand_seed << 5;
  return rand_seed & INT_MAX;
}

dlexport void srand(uint seed) {
  rand_seed = seed;
}

dlexport int rand_r(uint *_seed) {
  uint seed  = *_seed;
  seed      ^= seed << 13;
  seed      ^= seed >> 17;
  seed      ^= seed << 5;
  *_seed     = seed;
  return seed & INT_MAX;
}

#if 0
/* System V style 48-bit random number generator functions.  */

/* Return non-negative, double-precision floating-point value in [0.0,1.0).  */
extern double drand48(void) __THROW;
extern double erand48(unsigned short int __xsubi[3]) __attr_NTHLF_nnul(1);

/* Return non-negative, long integer in [0,2^31).  */
extern long int lrand48(void) __THROW;
extern long int nrand48(unsigned short int __xsubi[3]) __attr_NTHLF_nnul(1);

/* Return signed, long integers in [-2^31,2^31).  */
extern long int mrand48(void) __THROW;
extern long int jrand48(unsigned short int __xsubi[3]) __attr_NTHLF_nnul(1);

/* Seed random number generator.  */
extern void                srand48(long int __seedval) __THROW;
extern unsigned short int *seed48(unsigned short int __seed16v[3]) __attr_NTHLF_nnul(1);
extern void                lcong48(unsigned short int __param[7]) __attr_NTHLF_nnul(1);

/* Data structure for communication with thread safe versions.  This
   type is to be regarded as opaque.  It's only exported because users
   have to allocate objects of this type.  */
struct drand48_data {
  unsigned short int                   __x[3];     /* Current state.  */
  unsigned short int                   __old_x[3]; /* Old state.  */
  unsigned short int                   __c;        /* Additive const. in congruential formula.  */
  unsigned short int                   __init;     /* Flag for initializing.  */
  __extension__ unsigned long long int __a;        /* Factor in congruential
                                               formula.  */
};

/* Return non-negative, double-precision floating-point value in [0.0,1.0).  */
extern int drand48_r(struct drand48_data *__restrict __buffer, double *__restrict __result)
    __THROW __nonnull((1, 2));
extern int erand48_r(unsigned short int __xsubi[3], struct drand48_data *__restrict __buffer,
                     double *__restrict __result) __THROW __nonnull((1, 2));

/* Return non-negative, long integer in [0,2^31).  */
extern int lrand48_r(struct drand48_data *__restrict __buffer, long int *__restrict __result)
    __THROW __nonnull((1, 2));
extern int nrand48_r(unsigned short int __xsubi[3], struct drand48_data *__restrict __buffer,
                     long int *__restrict __result) __THROW __nonnull((1, 2));

/* Return signed, long integers in [-2^31,2^31).  */
extern int mrand48_r(struct drand48_data *__restrict __buffer, long int *__restrict __result)
    __THROW __nonnull((1, 2));
extern int jrand48_r(unsigned short int __xsubi[3], struct drand48_data *__restrict __buffer,
                     long int *__restrict __result) __THROW __nonnull((1, 2));

/* Seed random number generator.  */
extern int srand48_r(long int __seedval, struct drand48_data *__buffer) __THROW __nonnull((2));

extern int seed48_r(unsigned short int __seed16v[3], struct drand48_data *__buffer)
    __THROW __nonnull((1, 2));

extern int lcong48_r(unsigned short int __param[7], struct drand48_data *__buffer)
    __THROW __nonnull((1, 2));

/* Return a random integer between zero and 2**32-1 (inclusive).  */
extern u32 arc4random(void) __THROW __wur;

/* Fill the buffer with random data.  */
extern void arc4random_buf(void *__buf, size_t __size) __attr_NTHLF_nnul(1);

/* Return a random number between zero (inclusive) and the specified
   limit (exclusive).  */
extern u32 arc4random_uniform(u32 __upper_bound) __THROW __wur;
#endif

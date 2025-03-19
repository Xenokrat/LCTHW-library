#include <lcthw/bstrlib.h>
#include <lcthw/hashmap.h>
#include <lcthw/hashmap_algos.h>
#include <lcthw/darray.h>
#include "minunit.h"

struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("test data 3");

char *test_fnv1a()
{
	uint32_t hash = Hashmap_fnv1a_hash(&test1);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_fnv1a_hash(&test2);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_fnv1a_hash(&test3);
	mu_assert(hash != 0, "Bas hash.")
	
	return NULL;
}

char *test_adler32()
{
	uint32_t hash = Hashmap_adler32_hash(&test1);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_adler32_hash(&test2);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_adler32_hash(&test3);
	mu_assert(hash != 0, "Bas hash.")
	
	return NULL;
}

char *test_djb()
{
	uint32_t hash = Hashmap_djb_hash(&test1);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_djb_hash(&test2);
	mu_assert(hash != 0, "Bas hash.")

	hash = Hashmap_djb_hash(&test3);
	mu_assert(hash != 0, "Bas hash.")
	
	return NULL;
}

#define BUCKETS 100
#define BUFFER_LEN 20
#define NUM_KEYS BUCKETS * 1000
enum {
  ALGO_FNV1A,
  ALGO_ADLER32,
  ALGO_DJB,
  ALGO_LL,
  ALGO_JENK,
  ALGO_POLY
};

int 
gen_keys(DArray *keys,
         int     num_keys)
{
	int i = 0;
	FILE *urand = fopen("/dev/urandom", "r");
	int result = -1; /* default to failure condition */
	int rc = 0;
	bstring key = NULL;

	struct bStream *stream = bsopen((bNread) fread, urand);
	check(stream != NULL, "Failed to open /dev/urandom");

	key = bfromcstr("");

	/* FNV1A histogram */
	for (i = 0; i < num_keys; i++) {
		rc = bsread(key, stream, BUFFER_LEN);
		check(rc >= 0, "Failed to read from /dev/urandom");

		DArray_push(keys, bstrcpy(key));
	}

	result = 0; /* all good */

error: /* fallthrough */
	if (stream) bsclose(stream);
	if (urand)  fclose(urand);
	if (key)    bdestroy(key);
	return result;
}

void
destroy_keys(DArray *keys)
{
	int i = 0;
	for (i = 0; i < NUM_KEYS; i++) {
		bdestroy(DArray_get(keys, i));
	}

	DArray_destroy(keys);
}

void
fill_distribution(int          *stats,
                  DArray       *keys,
                  Hashmap_hash  hash_func)
{
	int i = 0;
	uint32_t hash = 0;

	for (i = 0; i < DArray_count(keys); i++) {
		hash = hash_func(DArray_get(keys, i));
		stats[hash % BUCKETS] += 1;
	}
}

char *
test_distribution()
{
	int i = 0;
	int stats[6][BUCKETS] = { { 0 } };
	DArray *keys = DArray_create(0, NUM_KEYS, DArray_expand);

	mu_assert(gen_keys(keys, NUM_KEYS) == 0,
	          "Failed to generate random keys.");

	fill_distribution(stats[ALGO_FNV1A],   keys, Hashmap_fnv1a_hash);
	fill_distribution(stats[ALGO_ADLER32], keys, Hashmap_adler32_hash);
	fill_distribution(stats[ALGO_DJB],     keys, Hashmap_djb_hash);
	fill_distribution(stats[ALGO_LL],      keys, Hashmap_lose_lose_hash);
	fill_distribution(stats[ALGO_JENK],    keys, Hashmap_jenkins_hash);
	fill_distribution(stats[ALGO_POLY],    keys, Hashmap_polynomial_hash);

	fprintf(stdout, "FNV\tA32\tDJB\tLL\tJENK\tPOLY\n");

	for (i = 0; i < BUCKETS; i++) {
		fprintf(stdout, "%d\t%d\t%d\t%d\t%d\t%d\n",
		        stats[ALGO_FNV1A][i],
		        stats[ALGO_ADLER32][i],
		        stats[ALGO_DJB][i],
		        stats[ALGO_LL][i],
		        stats[ALGO_JENK][i],
		        stats[ALGO_POLY][i]
		);
	}

	destroy_keys(keys);

	return NULL;
}

char *all_tests()
{
	mu_suite_start();

	mu_run_test(test_fnv1a);
	mu_run_test(test_adler32);
	mu_run_test(test_djb);
	mu_run_test(test_distribution);

	return NULL;
}

RUN_TESTS(all_tests);

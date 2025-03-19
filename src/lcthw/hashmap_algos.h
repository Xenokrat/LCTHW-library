#ifndef hashmap_algos_h
#define hashmap_algos_h

#include <stdint.h>

uint32_t Hashmap_fnv1a_hash(void *data);

uint32_t Hashmap_adler32_hash(void *data);

uint32_t Hashmap_djb_hash(void *data);

uint32_t Hashmap_lose_lose_hash(void *data);

uint32_t Hashmap_jenkins_hash(void *data);

uint32_t Hashmap_jenkins_hash(void *data);

uint32_t Hashmap_polynomial_hash(void *data);

#endif // hashmap_algos_h

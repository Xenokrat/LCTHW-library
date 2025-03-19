#ifndef _lcthw_Hashmap_h
#define _lcthw_Hashmap_h

#include <stdint.h>
#include <lcthw/darray.h>
#include <lcthw/darray_algos.h>

#define DEFAULT_NUMBER_OF_BUCKETS 100
#define SALT_LENGTH 5

typedef int (*Hashmap_compare) (void *a, void *b);
typedef uint32_t (*Hashmap_hash) (void *key);

typedef struct Hashmap {
  uint32_t         buckets_count;
  DArray          *buckets;
  Hashmap_compare  compare;
  Hashmap_hash     hash;
  char             salt[SALT_LENGTH];
} Hashmap;

typedef struct HashmapNode {
  void     *key;
  void     *data;
  uint32_t  hash;
} HashmapNode;

typedef int (*Hashmap_traverse_cb) (HashmapNode *node);

Hashmap *Hashmap_create(uint32_t        buckets_count,
                        Hashmap_compare compare,
                        Hashmap_hash    hash);
void Hashmap_destroy(Hashmap *map);

int Hashmap_set(Hashmap *map, void *key, void *data);
void *Hashmap_get(Hashmap *map, void *key);

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb);

void *Hashmap_delete(Hashmap *map, void *key);

uint32_t _jenkins_hash(void *data);

uint32_t _polynomial_hash(void *data);

#endif // _lcthw_Hashmap_h

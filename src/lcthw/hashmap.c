#undef NDEBUG
#include <stdint.h>
#include <lcthw/hashmap.h>
#include <lcthw/dbg.h>
#include <lcthw/bstrlib.h>

#define DEFAULT_HASH _polynomial_hash

static int default_compare(void *a, void *b)
{
  return bstrcmp((bstring) a, (bstring) b);
}

/**
 * Simple Bob's Jenkins's hash algorithm taken from the
 * wikipedia description.
 */
uint32_t _jenkins_hash(void *a)
{
  size_t len = blength((bstring) a);
  char *key = bdata((bstring) a);
  uint32_t hash = 0;
  uint32_t i    = 0;

  for (hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

/**
 * Polynomial rolling hash function
 */
uint32_t _polynomial_hash(void *a)
{
  size_t i = 0;
  size_t len = blength((bstring) a);
  char *key = bdata((bstring) a);
  
  const int p = 53;
  const int m = 1e9 + 9;

  uint32_t hash = 0;
  uint32_t p_pow = 1;

  for (i = 0; i < len; i++) {
    hash += (key[i] - 'a' + 1) * p_pow % m;
    p_pow = (p_pow * p) % m;
  }

  return hash;
}

static inline void rand_salt(char *dest, size_t length)
{
  char charset[] = "0123456789"
                   "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  while (length > 0) {
    length--;
    size_t index = (double) rand() / RAND_MAX * (sizeof(charset) - 1);
    *dest = charset[index];
    dest++;
  }
  *dest = '\0';
}

Hashmap *Hashmap_create(uint32_t        buckets_count,
                        Hashmap_compare compare,
                        Hashmap_hash    hash)
{
  Hashmap *map = calloc(1, sizeof(Hashmap));
  check_mem(map);

  map->buckets_count = buckets_count;
  map->compare = compare == NULL ? default_compare : compare;
  map->hash = hash == NULL ? DEFAULT_HASH : hash;
  map->buckets = DArray_create(sizeof(DArray *), buckets_count,
                               DArray_expand);
  
  rand_salt(map->salt, SALT_LENGTH);

  map->buckets->end = map->buckets->max; // fake out expanding it
  check_mem(map->buckets);

  return map;
  
error:
  if (map) {
    DArray_destroy(map->buckets);
    Hashmap_destroy(map);
  }
 
  return NULL;
}

void Hashmap_destroy(Hashmap *map)
{
  if (map == NULL) return;
  int i = 0;
  int j = 0;

  if (map) {
    if (map->buckets) {
      for (i = 0; i < DArray_count(map->buckets); i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if (bucket) {
          for (j = 0; j < DArray_count(bucket); j++) {
            free(DArray_get(bucket, j));
          }
          DArray_destroy(bucket);
        }
      }
      DArray_destroy(map->buckets);
    }

    free(map);
  }
}


static inline HashmapNode *Hashmap_node_create(int   hash,
                                               void *key,
                                               void *data)
{
  HashmapNode *node = calloc(1, sizeof(HashmapNode));
  check_mem(node);

  node->key = key;
  node->data = data;
  node->hash = hash;

  return node;
  
error:
  return NULL;
}

static inline int Hashmap_find_bucket_n(Hashmap  *map,
                                        void     *key,
                                        uint32_t *hash_out)
{
  bstring bsalt = NULL;
  check_mem(map);
  
  bsalt = bfromcstr(map->salt);
  int rc = bconcat(bsalt, (bstring) key);
  check(rc == 0, "Failed to make a salt-bstring");
  
  uint32_t hash = map->hash((void *) bsalt);
  int bucket_n = hash % map->buckets_count;
  check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);
  // store it for the return so the caller can use it
  *hash_out = hash;

  free(bsalt);
  return bucket_n;

error:
  if (bsalt)
    free(bsalt);
  return -1;
}
                                        
static inline DArray *Hashmap_find_bucket(Hashmap  *map,
                                          void     *key,
                                          int       create,
                                          uint32_t *hash_out)
{
  check_mem(map);

  int bucket_n = Hashmap_find_bucket_n(map, key, hash_out);
  check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);

  DArray *bucket = DArray_get(map->buckets, bucket_n);

  if (!bucket && create) {
    // new bucket, set it up
    bucket = DArray_create(sizeof(void *),
                           map->buckets_count,
                           DArray_expand);
    check_mem(bucket);
    DArray_set(map->buckets, bucket_n, bucket);
  }

  return bucket;
  
error:
  return NULL;
}

static inline int Hashmap_get_node(Hashmap *map, uint32_t hash,
                                   DArray *bucket, void *key)
{
  int i = 0;

  for (i = 0; i < DArray_end(bucket); i++) {
    debug("TRY: %d", i);
    HashmapNode *node = DArray_get(bucket, i);
    if (node == NULL) return -1;
    if (node->hash == hash && map->compare(node->key, key) == 0)
      return i;
  }

  return -1;
}
                                         
int Hashmap_set(Hashmap *map, void *key, void *data)
{
  check_mem(map);
  
  uint32_t hash = 0;
  DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
  check(bucket, "Error can't create bucket.");
  
  if (Hashmap_get(map, key) == NULL) {
    HashmapNode *node = Hashmap_node_create(hash, key, data);
    check_mem(node);

    // DArray_push(bucket, node);
    DArray_sort_add(bucket, node, DArray_qsort_lib,
                    (DArray_compare) map->compare);
  } else {
    int i = Hashmap_get_node(map, hash, bucket, key);
    check(i != -1, "Should get node No");

    HashmapNode *node = DArray_get(bucket, i);
    check(node != NULL,
          "Failed to get node from bucket when it should exists.");
    node->data = data;
    DArray_qsort_lib(bucket, (DArray_compare) map->compare);
  }
  return 0;

error:
  return -1;
}


void *Hashmap_get(Hashmap *map, void *key)
{
  check_mem(map);
  
  uint32_t hash = 0;
  DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
  if (!bucket) return NULL;

  int i = Hashmap_get_node(map, hash, bucket, key);
  if (i == -1) return NULL;

  HashmapNode *node = DArray_get(bucket, i);
  check(node != NULL,
        "Failed to get node from bucket when it should exists.");

  return node->data;

 error:
  return NULL;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
  check_mem(map);
  
  int i  = 0;
  int j  = 0;
  int rc = 0;

  for (i = 0; i < DArray_count(map->buckets); i++) {
    DArray *bucket = DArray_get(map->buckets, i);
    if (bucket) {
      for (j = 0; j < DArray_count(bucket); j++) {
        HashmapNode *node = DArray_get(bucket, j);
        rc = traverse_cb(node);
        if (rc != 0)
          return rc;
      }
    }
  }

  return 0;
error:
  return -1;
}

void *Hashmap_delete(Hashmap *map, void *key)
{
  check_mem(map);
  
  uint32_t hash = 0;
  DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
  if (!bucket)
    return NULL;

  int i = Hashmap_get_node(map, hash, bucket, key);
  if (i == -1)
    return NULL;

  HashmapNode *node = DArray_get(bucket, i);
  void *data = node->data;
  free(node);

  HashmapNode *ending = DArray_pop(bucket);

  if (ending != node) {
    // alright looks like it's not the last one, swap it
    DArray_set(bucket, i, ending);
  }

  /* Remove bucket if it's empty */
  if (bucket->end <= 1) {
    int bucket_n = Hashmap_find_bucket_n(map, key, &hash);
    assert(bucket >= 0 && "Got wrong bucket no");
    DArray_destroy(DArray_get(map->buckets, bucket_n));
    map->buckets->contents[(size_t) bucket_n] = NULL;
  }

  return data;

error:
  return NULL;
}

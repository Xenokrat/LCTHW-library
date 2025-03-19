#include "minunit.h"
#include <assert.h>
#include <time.h>
#include <lcthw/hashmap.h>
#include <lcthw/bstrlib.h>

#define TEST_STR_SIZE 1000
#define TEST_STR_LEN  10

Hashmap *map = NULL;
static int traverse_called = 0;
struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("test data 3");
struct tagbstring expect1 = bsStatic("THE VALUE 1");
struct tagbstring expect2 = bsStatic("THE VALUE 2");
struct tagbstring expect3 = bsStatic("THE VALUE 3");

static int traverse_good_cb(HashmapNode *node)
{
  debug("KEY: %s", bdata((bstring) node->key));
  traverse_called++;
  return 0;
}

static int traverse_fail_cb(HashmapNode *node)
{
  debug("KEY: %s", bdata((bstring) node->key));
  traverse_called++;

  if (traverse_called == 2)
    return 1;
  else
    return 0;
}

static inline void rand_string(char *dest, size_t length)
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

char *test_create()
{
  map = Hashmap_create(100, NULL, NULL);
  mu_assert(map != NULL, "Failed to create map.");

  return NULL;
}

char *test_destroy()
{
  Hashmap_destroy(map);

  return NULL;
}

char *test_get_set()
{
  int rc = Hashmap_set(map, &test1, &expect1);
  mu_assert(rc == 0, "Failed to set &test1");
  bstring result = Hashmap_get(map, &test1);
  mu_assert(result == &expect1, "Wrong value to &test1");
  
  rc = Hashmap_set(map, &test2, &expect2);
  mu_assert(rc == 0, "Failed to set &test2");
  result = Hashmap_get(map, &test2);
  mu_assert(result == &expect2, "Wrong value to &test2");
  
  rc = Hashmap_set(map, &test3, &expect3);
  mu_assert(rc == 0, "Failed to set &test3");
  result = Hashmap_get(map, &test3);
  mu_assert(result == &expect3, "Wrong value to &test3");
  
  rc = Hashmap_set(map, &test3, &expect1);
  mu_assert(rc == 0, "Failed to set &test3");
  result = Hashmap_get(map, &test3);
  mu_assert(result == &expect1, "Wrong value to &test3");
  
  rc = Hashmap_set(map, &test3, &expect3);
  mu_assert(rc == 0, "Failed to set &test3");
  result = Hashmap_get(map, &test3);
  mu_assert(result == &expect3, "Wrong value to &test3");

  return NULL;
}

char *test_traverse()
{
  int rc = Hashmap_traverse(map, traverse_good_cb);
  mu_assert(rc == 0, "Failed to traverse");
  mu_assert(traverse_called == 3, "Wrong count traverse");

  traverse_called = 0;
  rc = Hashmap_traverse(map, traverse_fail_cb);
  mu_assert(rc == 1, "Failed to traverse");
  mu_assert(traverse_called == 2, "Wrong number traverse for fail");

  return NULL;
}

char *test_delete()
{
  bstring deleted = (bstring) Hashmap_delete(map, &test1);
  mu_assert(deleted != NULL, "Got NULL on delete.");
  mu_assert(deleted == &expect1, "Should get test1");
  bstring result = Hashmap_get(map, &test1);
  mu_assert(result == NULL, "Should delete.");
  
  deleted = (bstring) Hashmap_delete(map, &test2);
  mu_assert(deleted != NULL, "Got NULL on delete.");
  mu_assert(deleted == &expect2, "Should get test2");
  result = Hashmap_get(map, &test2);
  mu_assert(result == NULL, "Should delete.");
  
  deleted = (bstring) Hashmap_delete(map, &test3);
  mu_assert(deleted != NULL, "Got NULL on delete.");
  mu_assert(deleted == &expect3, "Should get test3");
  result = Hashmap_get(map, &test3);
  mu_assert(result == NULL, "Should delete.");
  
  return NULL;
}

char *test_performance()
{
  size_t i = 0;
  float start_time = 0.0;
  float end_time   = 0.0;
  bstring strings[TEST_STR_SIZE];
  char buffer[TEST_STR_LEN + 1];
  
  printf("\n=== TEST HASHMAP SPEED ===\n");
  
  for (i = 0; i < TEST_STR_SIZE; i++) {
    rand_string(buffer, TEST_STR_LEN);
    strings[i] = bfromcstr(buffer);
  }

  Hashmap *map_wide = Hashmap_create(1000, NULL, NULL);
  Hashmap *map_thin = Hashmap_create(10  , NULL, NULL);

  start_time = (float)clock() / CLOCKS_PER_SEC; 
  for (i = 0; i < TEST_STR_SIZE; i++) {
    Hashmap_set(map_wide, strings[i], NULL);
  }
  end_time = (float)clock() / CLOCKS_PER_SEC; 
  printf("Hashmap_set for WIDE map is %f\n", end_time - start_time);

  start_time = (float)clock() / CLOCKS_PER_SEC; 
  for (i = 0; i < TEST_STR_SIZE; i++) {
    Hashmap_set(map_thin, strings[i], NULL);
  }
  end_time = (float)clock() / CLOCKS_PER_SEC; 
  printf("Hashmap_set for THIN map is %f\n", end_time - start_time);

  /* Cleanup */
  for (i = 0; i < TEST_STR_SIZE; i++) {
    free(strings[i]);
  }
  Hashmap_destroy(map_wide);
  Hashmap_destroy(map_thin);
  
  return NULL;
}

char *all_tests()
{
  mu_suite_start();

  mu_run_test(test_create);
  mu_run_test(test_get_set);
  mu_run_test(test_traverse);
  mu_run_test(test_delete);
  mu_run_test(test_destroy);
  mu_run_test(test_performance);
  
  return NULL;
}

RUN_TESTS(all_tests);

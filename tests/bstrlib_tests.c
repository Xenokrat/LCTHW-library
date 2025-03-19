#include <stdio.h>
#include <lcthw/bstrlib.h>
#include "minunit.h"

/*
 * bstrcpy: Copy a bstring.
 * bassign: Set one bstring to another.
 * bassigncstr: Set a bstring to a C string's contents.
 * bassignblk: Set a bstring to a C string but give the length.
 * bdestroy: Destroy a bstring.
 * bconcat: Concatenate one bstring onto another.
 * bstricmp: Compare two bstrings returning the same result as strcmp.
 * biseq: Tests if two bstrings are equal.
 * binstr: Tells if one bstring is in another.
 * bfindreplace: Find one bstring in another, then replace it with a third.
 * bsplit: Split a bstring into a bstrList.
 * bformat: Do a format string, which is super handy.
 * blength: Get the length of a bstring.
 * bdata: Get the data from a bstring.
 * bchar: Get a char from a bstring.
 */

char *test_bfromcstr()
{
  const char *cstr = "test c string";
  bstring bstr = bfromcstr(cstr);
  printf("Got bstring: %s\n", bstr->data);

  /* test with length */
  const void *cstr2 = (const void *) cstr;
  int len = strlen(cstr);

  bstring bstr2 = blk2bstr(cstr2, len);
  printf("Got bstring: %s\n", bstr2->data);

  mu_assert(strcmp((const char*) bstr->data,
                   (const char*) bstr2->data) == 0, "Got wrong bstrings");

  bstring bstr3 = bstrcpy(bstr2);
  mu_assert(bstr3 != NULL, "Fail to bstrcpy");
  mu_assert(strcmp((const char*) bstr2->data,
                   (const char*) bstr3->data) == 0, "Got wrong bstrings");

  /* test bassign */
  bstr = bfromcstr("hello");
  int rc = bassign(bstr3, bstr);
  mu_assert(rc == 0, "Failed to bassign");
  mu_assert(strcmp((const char*) bstr3->data,
                   (const char*) "hello") == 0, "Got wrong bstrings");

  /* test bassigncstr */
  rc = bassigncstr(bstr3, "hello bassigncstr");
  mu_assert(rc == 0, "Failed to bassigncstr");
  mu_assert(strcmp((const char*) bstr3->data,
                   (const char*) "hello bassigncstr") == 0, "Got wrong bstrings");

  /* test bassignblk */
  rc = bassignblk(bstr3, (void *) "assing blk", 10);
  mu_assert(rc == 0, "Failed to bassignblk");
  mu_assert(strcmp((const char*) bstr3->data,
                   (const char*) "assing blk") == 0, "Got wrong bstrings");

  /* test bdestroy */
  rc = bdestroy(bstr3);
  mu_assert(rc == 0, "Failed to destroy bstr3");

  /* test bconcat */
  bstr = bfromcstr("hello ");
  bstr2 = bfromcstr("world!");
  rc = bconcat(bstr, bstr2);
  mu_assert(rc == 0, "Failed to concat");
  mu_assert(strcmp((const char*) bstr->data,
                   (const char*) "hello world!") == 0,
            "Got concat wrong!");

  /* test bstricmp */
  bstr2 = bfromcstr("hello world!");
  rc = bstricmp(bstr, bstr2);
  mu_assert(rc == 0, "String should be equal!");

  /* test bseq */
  rc = biseq(bstr, bstr2);
  mu_assert(rc == 1, "String should be equal!");

  /* test binstr */
  rc = binstr(bfromcstr("hello world"),
              0,
              bfromcstr("world"));
  mu_assert(rc == 6, "Failed to find substr");
  rc = binstr(bfromcstr("hello world"),
              0,
              bfromcstr("orange"));
  mu_assert(rc == BSTR_ERR, "Failed to not find substr");

  /* test bfindreplace */
  rc = bfindreplace(bstr = bfromcstr("Best editor is vim for sure"),
                    bfromcstr("vim"),
                    bfromcstr("emacs"),
                    0);
  mu_assert(rc == 0, "Error when find-replace");
  mu_assert(biseq(bstr, bfromcstr("Best editor is emacs for sure")) == 1,
            "Failed to find-replace");

  /* test bsplit */
  bstr = bfromcstr("heh,mda,lol");
  struct bstrList *list = bsplit(bstr, ',');
  mu_assert(biseq(list->entry[0], bfromcstr("heh")) == 1 &&
            biseq(list->entry[1], bfromcstr("mda")) == 1 &&
            biseq(list->entry[2], bfromcstr("lol")) == 1,
            "Failed to split string");
  bstrListDestroy(list);

  /* test bformat */
  bstr = bfromcstr("world");
  bstr2 = bformat("Hello %s!", bstr->data);
  mu_assert(biseq(bstr2, bfromcstr("Hello world!")) == 1, "Failed to bformat");

  /* test blength */
  mu_assert(blength(bstr2) == 12, "Failed to count blength");

  /* test bdata */
  mu_assert(strcmp(bdata(bfromcstr("hello")), "hello") == 0,
            "Failed to get bdata");
           
  /* test bchar */
  mu_assert(bchar(bfromcstr("hello"), 3) == 'l', "Failed to get bchar");
  
  return NULL;
}

char *all_tests()
{
  mu_suite_start();

  mu_run_test(test_bfromcstr);
  return NULL;
}

RUN_TESTS(all_tests);

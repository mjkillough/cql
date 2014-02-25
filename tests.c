#include "greatest.h"


extern SUITE(page_pool_suite); // tests_index.c
extern SUITE(btree_suite); // tests_index.c

GREATEST_MAIN_DEFS();


int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(page_pool_suite);
    RUN_SUITE(btree_suite);
    GREATEST_MAIN_END();
}

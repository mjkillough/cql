#include "greatest.h"


extern SUITE(index_suite); // tests_index.c


GREATEST_MAIN_DEFS();


int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(index_suite);
    GREATEST_MAIN_END();
}

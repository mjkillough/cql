#include "greatest.h"


extern SUITE(example_suite); // tests_example.c


GREATEST_MAIN_DEFS();


int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(example_suite);
    GREATEST_MAIN_END();
}

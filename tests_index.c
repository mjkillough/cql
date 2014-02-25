#include "greatest.h"

#include "index.h"

TEST test_page_pool_init__normal() {
    // The page_pool_t structure should be correctly initialized.
    page_pool_t *pool = page_pool_init(5);

    ASSERT(pool != NULL);
    ASSERT_EQ(pool->max_len, 5);

    // should have empty page_t* array
    ASSERT_EQ(pool->len, 0);
    for (int i = 0; i < pool->max_len; i++)
        ASSERT_EQ(pool->pages[0], NULL);

    PASS();
}

TEST test_page_pool_init__too_small() {
    // Creating a page_pool with max_len=0 should fail.
    page_pool_t *pool = page_pool_init(0);
    ASSERT_EQ(pool, NULL);

    PASS();
}

TEST test_page_pool_create_page__normal() {
    // Allocate a single page in a pool, ensure it's zeroed.
    page_pool_t *pool = page_pool_init(2);
    size_t index = 1337;
    page_t *page = page_pool_create_page(pool, &index);

    ASSERT_EQ(index, 0);
    ASSERT(page != NULL);
    for (int i = 0; i < PAGE_SIZE; i++)
        ASSERT_EQ(page->data[i], 0);

    PASS();
}

TEST test_page_pool_create_page__twice() {
    // Allow two pages to be allocated in the same pool.
    page_pool_t *pool = page_pool_init(3);
    size_t index1 = 1337;
    size_t index2 = 1338;
    page_t *page1 = page_pool_create_page(pool, &index1);
    page_t *page2 = page_pool_create_page(pool, &index2);

    ASSERT_EQ(index1, 0);
    ASSERT_EQ(index2, 1);
    ASSERT(page1 != NULL);
    ASSERT(page2 != NULL);
    ASSERT(page1 != page2);

    PASS();
}

TEST test_page_pool_create_page__pool_full() {
    // Error if the pool is already full.
    page_pool_t *pool = page_pool_init(1);
    size_t index1 = 1337;
    size_t index2 = 1338;
    page_t *page1 = page_pool_create_page(pool, &index1);
    page_t *page2 = page_pool_create_page(pool, &index2);

    ASSERT_EQ(index1, 0);
    ASSERT(page1 != NULL);
    ASSERT_EQ(index2, 1338);
    ASSERT(page2 == NULL);

    PASS();
}


TEST test_page_pool_get_page__normal() {
    // Get a page from a pool when all is fine and dandy.
    page_pool_t *pool = page_pool_init(2);
    size_t index1 = 1337;
    size_t index2 = 1338;
    page_t *page1 = page_pool_create_page(pool, &index1);
    page_t *page2 = page_pool_create_page(pool, &index2);

    ASSERT_EQ(index1, 0);
    ASSERT_EQ(index2, 1);
    ASSERT(page1 != NULL);
    ASSERT(page2 != NULL);
    ASSERT(page1 != page2);

    page_t *page1_again = page_pool_get_page(pool, 0);
    page_t *page2_again = page_pool_get_page(pool, 1);

    ASSERT_EQ(page1, page1_again);
    ASSERT_EQ(page2, page2_again);

    PASS();
}

TEST test_page_pool_get_page__not_allocated() {
    // Error appropriately when getting a page that has not been allocated yet.
    page_pool_t *pool = page_pool_init(2);
    page_t *page;

    page = page_pool_get_page(pool, 1);
    ASSERT_EQ(page, NULL);

    // there could be other problems getting the 0th page
    page = page_pool_get_page(pool, 0);
    ASSERT_EQ(page, NULL);

    PASS();
}

TEST test_page_pool_get_page__outside_max_len() {
    // Error appropriately when trying to get an index outside the max_len.
    page_pool_t *pool = page_pool_init(2);
    page_t *page1, *page2;
    size_t index = 1337;

    // test w/ empty pool
    page1 = page_pool_get_page(pool, 3);
    ASSERT_EQ(page1, NULL);

    // add something to pool
    page2 = page_pool_create_page(pool, &index);
    ASSERT_EQ(index, 0);
    ASSERT(page2 != NULL);

    // test w/ non-empty pool
    page1 = page_pool_get_page(pool, 3);
    ASSERT_EQ(page1, NULL);

    PASS();
}

GREATEST_SUITE(page_pool_suite) {
    RUN_TEST(test_page_pool_init__normal);
    RUN_TEST(test_page_pool_init__too_small);
    RUN_TEST(test_page_pool_create_page__normal);
    RUN_TEST(test_page_pool_create_page__twice);
    RUN_TEST(test_page_pool_create_page__pool_full);
    RUN_TEST(test_page_pool_get_page__normal);
    RUN_TEST(test_page_pool_get_page__not_allocated);
    RUN_TEST(test_page_pool_get_page__outside_max_len);
}

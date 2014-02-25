#include "greatest.h"

#include "index.h"


/* page_pool tests */

TEST test_page_pool_init__normal(void)
{
    // The page_pool_t structure should be correctly initialized.
    page_pool_t *pool = page_pool_init(5);

    ASSERT(pool != NULL);
    ASSERT_EQ(pool->max_len, 5);

    // should have empty page_t* array
    ASSERT_EQ(pool->len, 0);
    for (int i = 0; i < pool->max_len; i++)
        ASSERT_EQ(pool->pages[0], NULL);

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_init__too_small(void)
{
    // Creating a page_pool with max_len=0 should fail.
    page_pool_t *pool = page_pool_init(0);
    ASSERT_EQ(pool, NULL);

    PASS();
}


TEST test_page_pool_init__twice(void)
{
    // Allow multiple page_pools.
    page_pool_t *pool1 = page_pool_init(1);
    page_pool_t *pool2 = page_pool_init(1);

    ASSERT(pool1 != NULL);
    ASSERT(pool2 != NULL);
    ASSERT(pool1 != pool2);

    page_pool_free(pool1);
    page_pool_free(pool2);

    PASS();
}


TEST test_page_pool_create_page__normal(void)
{
    // Allocate a single page in a pool, ensure it's zeroed.
    page_pool_t *pool = page_pool_init(2);
    size_t index = 1337;
    page_t *page = page_pool_create_page(pool, &index);

    ASSERT_EQ(index, 0);
    ASSERT(page != NULL);
    for (int i = 0; i < PAGE_SIZE; i++)
        ASSERT_EQ(page->data[i], 0);

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_create_page__twice(void)
{
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

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_create_page__pool_full(void)
{
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

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_create_page__two_pools(void)
{
    // Different pools should allocate different pages.
    page_pool_t *pool1 = page_pool_init(1);
    page_pool_t *pool2 = page_pool_init(1);
    size_t index1, index2;
    page_t *page1 = page_pool_create_page(pool1, &index1);
    page_t *page2 = page_pool_create_page(pool2, &index2);

    ASSERT_EQ(index1, 0);
    ASSERT_EQ(index2, 0);
    ASSERT(page1 != NULL);
    ASSERT(page2 != NULL);
    ASSERT(page1 != page2);

    page_pool_free(pool1);
    page_pool_free(pool2);

    PASS();
}


TEST test_page_pool_get_page__normal(void)
{
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

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_get_page__not_allocated(void)
{
    // Error appropriately when getting a page that has not been allocated yet.
    page_pool_t *pool = page_pool_init(2);
    page_t *page;

    page = page_pool_get_page(pool, 1);
    ASSERT_EQ(page, NULL);

    // there could be other problems getting the 0th page
    page = page_pool_get_page(pool, 0);
    ASSERT_EQ(page, NULL);

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_get_page__outside_max_len(void)
{
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

    page_pool_free(pool);

    PASS();
}


TEST test_page_pool_get_page__two_pools(void)
{
    // Different pools should give different pages.
    page_pool_t *pool1 = page_pool_init(1);
    page_pool_t *pool2 = page_pool_init(1);
    size_t index1, index2;
    page_t *page1 = page_pool_create_page(pool1, &index1);
    page_t *page2 = page_pool_create_page(pool2, &index2);

    ASSERT_EQ(index1, 0);
    ASSERT_EQ(index2, 0);
    ASSERT(page1 != NULL);
    ASSERT(page2 != NULL);
    ASSERT(page1 != page2);

    page_t *page1_again = page_pool_get_page(pool1, 0);
    page_t *page2_again = page_pool_get_page(pool2, 0);

    ASSERT_EQ(page1, page1_again);
    ASSERT_EQ(page2, page2_again);

    page_pool_free(pool1);
    page_pool_free(pool2);

    PASS();
}


TEST test_page_pool_free__empty(void)
{
    // Should be able to free an empty page_pool.
    page_pool_t *pool = page_pool_init(3);
    page_pool_free(pool);

    // XXX how to check the free()?
    PASS();
}


TEST test_page_pool_free__nonempty(void)
{
    // Freeing a non-empty page_pool should free the pages.
    page_pool_t *pool = page_pool_init(3);
    size_t index;
    page_pool_create_page(pool, &index);
    page_pool_free(pool);

    // XXX how to check the free()?
    PASS();
}


TEST test_page_pool_free__null(void)
{
    // Nothing bad should happen (except maybe an error msg) if we pass NULL.
    page_pool_free(NULL);

    PASS();
}


GREATEST_SUITE(page_pool_suite)
{
    RUN_TEST(test_page_pool_init__normal);
    RUN_TEST(test_page_pool_init__too_small);
    RUN_TEST(test_page_pool_init__twice);

    RUN_TEST(test_page_pool_create_page__normal);
    RUN_TEST(test_page_pool_create_page__twice);
    RUN_TEST(test_page_pool_create_page__pool_full);
    RUN_TEST(test_page_pool_create_page__two_pools);

    RUN_TEST(test_page_pool_get_page__normal);
    RUN_TEST(test_page_pool_get_page__not_allocated);
    RUN_TEST(test_page_pool_get_page__outside_max_len);
    RUN_TEST(test_page_pool_get_page__two_pools);

    RUN_TEST(test_page_pool_free__empty);
    RUN_TEST(test_page_pool_free__nonempty);
    RUN_TEST(test_page_pool_free__null);
}


/* btree tests */

typedef struct {
    page_pool_t *pool;
} test_btree_environ_t;


void test_btree_setup(test_btree_environ_t *environ)
{
    environ->pool = page_pool_init(10);
}


void test_btree_teardown(test_btree_environ_t *environ)
{
    page_pool_free(environ->pool);
}


TEST test_btree_allocate__normal(test_btree_environ_t *environ)
{
    // Allocate a regular btree.
    btree_t *btree = btree_allocate(environ->pool, sizeof(unsigned int), sizeof(int));
    ASSERT(btree != NULL);

    btree_free(btree);

    PASS();
}


TEST test_btree_allocate__key_size_0(test_btree_environ_t *environ)
{
    // Do not allow the key_size to be 0.
    btree_t *btree = btree_allocate(environ->pool, 0, sizeof(int));
    ASSERT_EQ(btree, NULL);

    PASS();
}


TEST test_btree_allocate__data_size_0(test_btree_environ_t *environ)
{
    // Do not allow the data_size to be 0.
    btree_t *btree = btree_allocate(environ->pool, sizeof(unsigned int), 0);
    ASSERT_EQ(btree, NULL);

    PASS();
}


TEST test_btree_allocate__null_pool(test_btree_environ_t *environ)
{
    // Require a pool.
    btree_t *btree = btree_allocate(NULL, sizeof(unsigned int), sizeof(int));

    PASS();
}


TEST test_btree_allocate__twice_on_same_pool(test_btree_environ_t *environ)
{
    // Allocate two btrees backed by the same pool.
    btree_t *btree1 = btree_allocate(environ->pool, sizeof(unsigned int), sizeof(int));
    btree_t *btree2 = btree_allocate(environ->pool, sizeof(unsigned int), sizeof(int));

    ASSERT(btree1 != NULL);
    ASSERT(btree2 != NULL);
    ASSERT(btree1 != btree2);

    PASS();
}


GREATEST_SUITE(btree_suite)
{
    test_btree_environ_t environ;

    SET_SETUP((greatest_setup_cb *)test_btree_setup, &environ);
    SET_TEARDOWN((greatest_setup_cb *)test_btree_teardown, &environ);

    #define BTREE_RUN_TEST(NAME) RUN_TEST1(NAME, (test_btree_environ_t*)&environ)

    BTREE_RUN_TEST(test_btree_allocate__normal);
    BTREE_RUN_TEST(test_btree_allocate__key_size_0);
    BTREE_RUN_TEST(test_btree_allocate__data_size_0);
    BTREE_RUN_TEST(test_btree_allocate__null_pool);
    BTREE_RUN_TEST(test_btree_allocate__twice_on_same_pool);
}

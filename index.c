#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"


page_pool_t* page_pool_init(size_t max_len)
{
    if (max_len == 0) {
        printf("Cannot initialize page_pool with max_len 0\n");
        return NULL;
    }
    page_pool_t *pool = (page_pool_t*)malloc(sizeof(page_pool_t)+sizeof(page_t*)*max_len);
    if (pool == NULL) {
        printf("Failed to allocate page_pool_t\n");
        return NULL;
    }
    memset(pool->pages, 0, sizeof(page_t*) * max_len);
    pool->max_len = max_len;
    pool->len = 0;
    return pool;
}

page_t* page_pool_create_page(page_pool_t *pool, size_t *index)
{
    if (pool->len >= pool->max_len) {
        printf("Cannot allocate page, pool is full\n");
        return NULL;
    }
    page_t *page = (page_t*)malloc(sizeof(page_t));
    if (page == NULL) {
        printf("Cannot allocate memory for page_t\n");
        return NULL;
    }
    memset(page, 0, sizeof(page_t));
    *index = pool->len++;
    pool->pages[*index] = page;
    return page;
}

page_t* page_pool_get_page(page_pool_t *pool, size_t index)
{
    if (index >= pool->len) {
        printf("Page %zu is not allocated\n", index);
        return NULL;
    }
    return pool->pages[index];
}

// TODO: add page_free -- why would you ever want to deallocate? :)


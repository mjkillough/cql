#ifndef INDEX_H
#define INDEX_H

#define PAGE_SIZE 256

typedef struct {
    char data[PAGE_SIZE];
} page_t;

typedef struct {
    size_t max_len;
    size_t len;
    page_t *pages[];
} page_pool_t;

page_pool_t* page_pool_init(size_t max_pages);
page_t* page_pool_create_page(page_pool_t *pool, size_t *index);
page_t* page_pool_get_page(page_pool_t *pool, size_t index);
void page_pool_free(page_pool_t *pool);

typedef enum {
    NODE_TYPE_INTERNAL,
    NODE_TYPE_LEAF
} node_type_t;

typedef struct {
    node_type_t node_type_t;
    size_t index;
} relation_t;

typedef struct {
    char **keys;
    relation_t **children;
} internal_node_t;

typedef struct {
    char *key;
    char *data;
} kvp_t;

typedef struct {
    kvp_t **key_value_pairs;
    size_t next;
    size_t prev;
} leaf_node_t;

typedef struct {
    size_t order;
    size_t key_size;
    size_t data_size;
    internal_node_t *root;
} btree_t;

btree_t* btree_allocate(page_pool_t *pool, size_t order, size_t key_size, size_t data_size);
void btree_insert(btree_t *tree, page_pool_t *pool, char *key, char *data);


#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // printf("ALLOC ENTRY: %s\n", path);
    // printf("ENTRY: \n%s\n", content);
    struct cache_entry *ce = malloc(sizeof (struct cache_entry));

    ce->path = strdup(path);
    ce->content_length = content_length;
    ce->content_type = strdup(content_type);
    ce->content = malloc(content_length);
    memcpy(ce->content, content, content_length);
    ce->next = NULL;
    ce->prev = NULL;

    return ce;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    free(entry->path);
    free(entry->content);
    // free(entry->content_length);
    free(entry->content_type);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // printf("dllist insert\n");
    // Insert at the head of the list
    if (cache->head == NULL) {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    } else {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head) {
        if (ce == cache->tail) {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;

        } else {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}


/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // printf("CREATE CACHE\n");
    struct cache *c = malloc(sizeof(struct cache));

    struct hashtable *ht = hashtable_create(hashsize, NULL);

    c->max_size = max_size;
    c->cur_size = 0;
    c->index = ht;
    c->head = NULL;
    c->tail = NULL;
    // printf("assigned values\n");

    return c;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL) {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // printf("PUT ENTRY\n");
    struct cache_entry *new_entry;
    new_entry = alloc_entry(path, content_type, content, content_length);
    // printf("PUT:\n%s\n %s\n %s\n %d\n", path, content_type, content, content_length);

    dllist_insert_head(cache, new_entry);
    hashtable_put(cache->index, new_entry->path, new_entry);
    cache->cur_size++;

    if (cache->cur_size > cache->max_size) {
        // printf("REMOVING FROM TAIL\n");
        struct cache_entry *tail = dllist_remove_tail(cache);
        hashtable_delete(cache->index, tail->path);
        // free_entry(tail);

        if (cache->cur_size > cache->max_size) {
            cache->cur_size--;        
        }
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // printf("GET CACHE\n");
    struct cache_entry *entry = hashtable_get(cache->index, path);
    // printf("%p\n", entry);
    if (entry == NULL) {
        // free_entry(entry);
        // printf("file not in cache\n");
        return NULL;
    }
    // printf("moved head to file location\n");
    // printf("%s\n", entry->path);
    // printf("CONTENT: %s\n", entry->content);
    dllist_move_to_head(cache, entry);
    return entry;
}

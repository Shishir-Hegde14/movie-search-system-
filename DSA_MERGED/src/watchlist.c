#include "watchlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *checked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Error: Out of memory allocating %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static char *string_duplicate(const char *src) {
    if (!src) return NULL;
    size_t n = strlen(src);
    char *copy = (char *)checked_malloc(n + 1);
    memcpy(copy, src, n + 1);
    return copy;
}

static void watchlist_init(Watchlist *list) {
    if (!list) return;
    list->name = NULL;
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void watchlist_free(Watchlist *list) {
    if (!list) return;
    free(list->name);
    free(list->items);
    watchlist_init(list);
}

void watchlist_manager_init(WatchlistManager *manager) {
    if (!manager) return;
    manager->lists = NULL;
    manager->count = 0;
    manager->capacity = 0;
}

static int ensure_watchlist_capacity(WatchlistManager *manager) {
    if (manager->count == manager->capacity) {
        size_t new_capacity = manager->capacity == 0 ? 4 : manager->capacity * 2;
        Watchlist *grown = (Watchlist *)realloc(manager->lists, new_capacity * sizeof(Watchlist));
        if (!grown) return 0;
        for (size_t i = manager->capacity; i < new_capacity; ++i) {
            watchlist_init(&grown[i]);
        }
        manager->lists = grown;
        manager->capacity = new_capacity;
    }
    return 1;
}

int watchlist_create(WatchlistManager *manager, const char *name) {
    if (!manager || !name || name[0] == '\0') return 0;
    if (!ensure_watchlist_capacity(manager)) {
        fprintf(stderr, "Error: Unable to allocate memory for new watchlist\n");
        return 0;
    }
    Watchlist *list = &manager->lists[manager->count];
    watchlist_init(list);
    list->name = string_duplicate(name);
    manager->count++;
    return 1;
}

int watchlist_delete(WatchlistManager *manager, size_t index) {
    if (!manager || index >= manager->count) return 0;
    watchlist_free(&manager->lists[index]);
    if (index != manager->count - 1) {
        memmove(&manager->lists[index], &manager->lists[index + 1], (manager->count - index - 1) * sizeof(Watchlist));
    }
    manager->count--;
    return 1;
}

int watchlist_rename(WatchlistManager *manager, size_t index, const char *new_name) {
    if (!manager || index >= manager->count || !new_name || new_name[0] == '\0') return 0;
    Watchlist *list = &manager->lists[index];
    char *copy = string_duplicate(new_name);
    free(list->name);
    list->name = copy;
    return 1;
}

static int watchlist_ensure_item_capacity(Watchlist *list) {
    if (list->count == list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        size_t *grown = (size_t *)realloc(list->items, new_capacity * sizeof(size_t));
        if (!grown) return 0;
        list->items = grown;
        list->capacity = new_capacity;
    }
    return 1;
}

int watchlist_add_movie(WatchlistManager *manager, size_t index, size_t movie_index) {
    if (!manager || index >= manager->count) return 0;
    Watchlist *list = &manager->lists[index];
    if (!watchlist_ensure_item_capacity(list)) {
        fprintf(stderr, "Error: Unable to expand watchlist items\n");
        return 0;
    }
    list->items[list->count++] = movie_index;
    return 1;
}

int watchlist_remove_movie(WatchlistManager *manager, size_t index, size_t position) {
    if (!manager || index >= manager->count) return 0;
    Watchlist *list = &manager->lists[index];
    if (position == 0 || position > list->count) return 0;
    size_t internal_index = position - 1;
    if (internal_index < list->count - 1) {
        memmove(&list->items[internal_index], &list->items[internal_index + 1], (list->count - internal_index - 1) * sizeof(size_t));
    }
    list->count--;
    return 1;
}

void watchlist_print_summary(const WatchlistManager *manager) {
    if (!manager || manager->count == 0) {
        printf("(no watchlists created yet)\n");
        return;
    }
    for (size_t i = 0; i < manager->count; ++i) {
        const Watchlist *list = &manager->lists[i];
        printf("%2zu) %s [%zu movies]\n", i + 1, list->name ? list->name : "(untitled)", list->count);
    }
}

void watchlist_print_detail(const WatchlistManager *manager, const MovieDatabase *db, size_t index) {
    if (!manager || index >= manager->count) {
        printf("Invalid watchlist selection.\n");
        return;
    }
    const Watchlist *list = &manager->lists[index];
    printf("\nWatchlist: %s\n", list->name ? list->name : "(untitled)");
    if (!list->items || list->count == 0) {
        printf("  (no movies added yet)\n");
        return;
    }
    for (size_t i = 0; i < list->count; ++i) {
        size_t movie_index = list->items[i];
        if (movie_index >= db->count) {
            printf("  %2zu) [invalid movie reference]\n", i + 1);
            continue;
        }
        const Movie *movie = &db->movies[movie_index];
        printf("  %2zu) %s (%s)\n", i + 1, movie->title ? movie->title : "(no title)", movie->release_year ? movie->release_year : "n/a");
    }
}

void watchlist_manager_free(WatchlistManager *manager) {
    if (!manager) return;
    for (size_t i = 0; i < manager->count; ++i) {
        watchlist_free(&manager->lists[i]);
    }
    free(manager->lists);
    manager->lists = NULL;
    manager->count = 0;
    manager->capacity = 0;
}


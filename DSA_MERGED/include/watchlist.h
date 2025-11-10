#ifndef WATCHLIST_H
#define WATCHLIST_H

#include <stddef.h>

#include "movie.h"

typedef struct {
    char *name;
    size_t *items;
    size_t count;
    size_t capacity;
} Watchlist;

typedef struct {
    Watchlist *lists;
    size_t count;
    size_t capacity;
} WatchlistManager;

void watchlist_manager_init(WatchlistManager *manager);
void watchlist_manager_free(WatchlistManager *manager);

int watchlist_create(WatchlistManager *manager, const char *name);
int watchlist_delete(WatchlistManager *manager, size_t index);
int watchlist_rename(WatchlistManager *manager, size_t index, const char *new_name);

int watchlist_add_movie(WatchlistManager *manager, size_t index, size_t movie_index);
int watchlist_remove_movie(WatchlistManager *manager, size_t index, size_t position);

void watchlist_print_summary(const WatchlistManager *manager);
void watchlist_print_detail(const WatchlistManager *manager, const MovieDatabase *db, size_t index);

#endif /* WATCHLIST_H */


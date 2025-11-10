#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>

typedef struct {
    char **entries;
    size_t count;
    size_t capacity;
    size_t max_entries;
} SearchHistory;

void history_init(SearchHistory *history, size_t max_entries);
void history_record(SearchHistory *history, const char *query);
void history_print(const SearchHistory *history);
void history_clear(SearchHistory *history);

#endif /* HISTORY_H */


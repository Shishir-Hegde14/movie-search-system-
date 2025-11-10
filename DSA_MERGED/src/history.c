#include "history.h"

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

void history_init(SearchHistory *history, size_t max_entries) {
    if (!history) return;
    history->entries = NULL;
    history->count = 0;
    history->capacity = 0;
    history->max_entries = max_entries > 0 ? max_entries : 100;
}

void history_record(SearchHistory *history, const char *query) {
    if (!history || !query || query[0] == '\0') return;

    if (history->count == history->max_entries) {
        free(history->entries[0]);
        memmove(&history->entries[0], &history->entries[1], (history->count - 1) * sizeof(char *));
        history->count--;
    }

    if (history->count == history->capacity) {
        size_t new_capacity = history->capacity == 0 ? 16 : history->capacity * 2;
        char **grown = (char **)realloc(history->entries, new_capacity * sizeof(char *));
        if (!grown) {
            fprintf(stderr, "Warning: Failed to expand search history buffer\n");
            return;
        }
        history->entries = grown;
        history->capacity = new_capacity;
    }

    history->entries[history->count++] = string_duplicate(query);
}

void history_print(const SearchHistory *history) {
    if (!history || history->count == 0) {
        printf("No search history yet.\n");
        return;
    }
    printf("\n--- Search History (most recent first) ---\n");
    size_t display = history->count;
    if (display > 50) display = 50;
    for (size_t i = 0; i < display; ++i) {
        size_t idx = history->count - 1 - i;
        printf("%2zu) %s\n", i + 1, history->entries[idx]);
    }
    if (history->count > display) {
        printf("... %zu more entries not shown ...\n", history->count - display);
    }
}

void history_clear(SearchHistory *history) {
    if (!history) return;
    for (size_t i = 0; i < history->count; ++i) {
        free(history->entries[i]);
    }
    free(history->entries);
    history->entries = NULL;
    history->count = 0;
    history->capacity = 0;
}


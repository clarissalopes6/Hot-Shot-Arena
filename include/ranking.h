#ifndef RANKING_H
#define RANKING_H

#include <stddef.h>

typedef struct {
    char name[64];
    float time;
} RankEntry;

int add_ranking_entry(const char *path, const char *name, float time);
int load_ranking(const char *path, RankEntry **out_entries);
int save_ranking(const char *path, RankEntry *entries, int count);

#endif

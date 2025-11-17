#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap(RankEntry *a, RankEntry *b) {
    RankEntry tmp = *a;
    *a = *b;
    *b = tmp;
}

static void quicksort(RankEntry *arr, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    RankEntry pivot = arr[(left + right) / 2];
    while (i <= j) {
        while (arr[i].time < pivot.time) i++;
        while (arr[j].time > pivot.time) j--;
        if (i <= j) {
            swap(&arr[i], &arr[j]);
            i++; j--;
        }
    }
    if (left < j) quicksort(arr, left, j);
    if (i < right) quicksort(arr, i, right);
}

int load_ranking(const char *path, RankEntry **out_entries) {
    FILE *f = fopen(path, "r");
    if (!f) {
        *out_entries = NULL;
        return 0;
    }
    RankEntry *arr = NULL;
    int capacity = 0;
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char name[64];
        float time;
        if (sscanf(line, "%63[^,],%f", name, &time) == 2) {
            if (count + 1 > capacity) {
                capacity = capacity == 0 ? 8 : capacity * 2;
                arr = realloc(arr, sizeof(RankEntry) * capacity);
            }
            strncpy(arr[count].name, name, sizeof(arr[count].name)-1);
            arr[count].name[sizeof(arr[count].name)-1] = '\0';
            arr[count].time = time;
            count++;
        }
    }
    fclose(f);
    *out_entries = arr;
    return count;
}

int save_ranking(const char *path, RankEntry *entries, int count) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s,%.3f\n", entries[i].name, entries[i].time);
    }
    fclose(f);
    return count;
}

int add_ranking_entry(const char *path, const char *name, float time) {
    RankEntry *arr = NULL;
    int count = load_ranking(path, &arr);
    RankEntry ne;
    strncpy(ne.name, name, sizeof(ne.name)-1);
    ne.name[sizeof(ne.name)-1] = '\0';
    ne.time = time;
    RankEntry *newarr = realloc(arr, sizeof(RankEntry) * (count + 1));
    if (!newarr) {
        free(arr);
        return 0;
    }
    newarr[count] = ne;
    count++;
   
    quicksort(newarr, 0, count - 1);
    int saved = save_ranking(path, newarr, count);
    free(newarr);
    return saved;
}

#include <stdio.h>
#include <stdlib.h>

#include "map.h"

Map *
map_create()
{
	Map *map = malloc(sizeof(Map));
	map->len = 0;
	map->cap = DEFAULT_CAP;
	map->entry = malloc(sizeof(struct entry) * map->cap);
	return map;
}

void
map_destroy(Map *map)
{
	free(map->entry);
	free(map);
}

void
map_set(Map *map, int key, void *value)
{
	while (map->len >= map->cap) {
		map->cap *= CAP_MULT;
		map->entry = realloc(map->entry, sizeof(struct entry) * map->cap);
	}
	int i; /* insert index */
	for (i = 0; i < map->len && map->entry[i].key <= key; i++) {}
	/* i >= map->len || map->entry[i].key > map.key */
	/* copy values from insert index */
	for (int k = i; k < map->len; k++) {
		map->entry[k + 1] = map->entry[k];
	}
	map->entry[i] = (struct entry) {key, value};
	map->len++;
}

static int
getindex(Map *map, int key, int start, int end)
{
	if (map->len == 0 || start == end) {
		return -1;
	}
	int mid = (end - start) / 2;
	printf("%d\n", mid);
	struct entry e = map->entry[mid];
	if (key < e.key) {
		return getindex(map, key, start, mid);
	} else if (e.key < key) {
		return getindex(map, key, mid, end);
	}
	return mid;
}

void *
map_get(Map *map, int key)
{
	int index = getindex(map, key, 0, map->len);
	return (index >= 0) ? map->entry[index].value : NULL;
}

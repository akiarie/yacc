#include <stdlib.h>
#include <assert.h>

#include "maps.h"

static struct ientry
entry_create(int key, const void *value)
{
	assert(value != NULL);
	return (struct ientry) { key, value };
}

struct intmap *
intmap_create()
{
	return (struct intmap *) calloc(1, sizeof(struct intmap));
}

void
intmap_destroy(struct intmap *map)
{
	free(map->entry);
	free(map);
}

static int
map_getindex(struct intmap *map, int key)
{
	for (int i = 0; i < map->n; i++) {
		if (map->entry[i].key == key) {
			return i;
		}
	}
	return -1;
}

void *
intmap_get(struct intmap *map, int key)
{
	int index = map_getindex(map, key);
	if (index != -1) {
		return (void *) map->entry[index].value;
	}
	return NULL;
}

void
intmap_set(struct intmap *map, int key, const void *value)
{
	assert(map_getindex(map, key) == -1);
	map->entry = realloc(map->entry, sizeof(struct ientry) * ++map->n);
	map->entry[map->n - 1] = entry_create(key, value);
}

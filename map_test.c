#include <stdlib.h>
#include <assert.h>

#include "map.h"

int
main()
{
	Map *map = map_create();
	assert(map_get(map, 20) == NULL);
	map_set(map, 17, "hello");
	map_set(map, 47, "world");
	map_set(map, 25, "today");
	assert(map_get(map, 16) == NULL);
	assert(map_get(map, 18) == NULL);
	assert(map_get(map, 19) == NULL);
	map_destroy(map);
}

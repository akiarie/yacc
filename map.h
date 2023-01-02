#ifndef YY_MAP
#define YY_MAP

#define DEFAULT_CAP 10
#define CAP_MULT 2

typedef struct {
	struct entry {
		int key;
		void *value;
	} *entry;
	size_t len, cap;
} Map;

Map *
map_create();

void
map_destroy(Map *);

void
map_set(Map *, int key, void *value);

void *
map_get(Map *, int key);

#endif

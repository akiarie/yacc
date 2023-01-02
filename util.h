#ifndef BNF_UTIL
#define BNF_UTIL
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

char *
repeat(char c, size_t reps);

char *
dynamic_str(const char *);

struct map {
	struct entry {
		char *key;
		const void *value;
	} **entries;
	size_t n;
};

struct map *
map_create();

void
map_destroy(struct map *);

void *
map_get(struct map *, const char *key);

int
map_getindex(struct map *map, const char *key);

void *
map_set_act(struct map *, const char *key, const void *value, bool overwrite);

void
map_set(struct map *, const char *key, const void *value);

#define map_setow(M, K, V, destroy) \
	void *old = map_set_act(M, K, V, true); \
	assert(old != NULL); \
	destroy(old);

struct circuitbreaker;

struct circuitbreaker *
circuitbreaker_create(void *);

struct circuitbreaker *
circuitbreaker_copy(struct circuitbreaker *tr);

void
circuitbreaker_destroy(struct circuitbreaker *);

bool
circuitbreaker_append(struct circuitbreaker *tr, void *s);


struct strbuilder;

struct strbuilder *
strbuilder_create();

int
strbuilder_printf(struct strbuilder *b, const char *fmt, ...);

int
strbuilder_vprintf(struct strbuilder *b, const char *fmt, va_list ap);

void
strbuilder_putc(struct strbuilder *b, char c);

char *
strbuilder_build(struct strbuilder *b);

#endif

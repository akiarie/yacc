#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "util.h"

char *
repeat(char c, size_t reps)
{
	char *s = malloc(sizeof(char) * (reps + 1));
	for (int i = 0; i < reps; i++) {
		s[i] = c;
	}
	s[reps] = '\0';
	return s;
}

char *
dynamic_str(const char *s)
{
	int len = strlen(s) + 1;
	char *t = malloc(sizeof(char) * len);
	strncpy(t, s, len);
	return t;
}

static struct entry
entry_create(const char *key, const void *value)
{
	assert(key != NULL && value != NULL);
	return (struct entry) { dynamic_str(key), value };
}

static void
entry_destroy(struct entry e)
{
	free(e.key);
}

struct map *
map_create()
{
	return (struct map *) calloc(1, sizeof(struct map));
}

void
map_destroy(struct map *map)
{
	for (int i = 0; i < map->n; i++) {
		entry_destroy(map->entry[i]);
	}
	free(map->entry);
	free(map);
}

int
map_getindex(struct map *map, const char *key)
{
	assert(key != NULL);
	for (int i = 0; i < map->n; i++) {
		if (strcmp(map->entry[i].key, key) == 0) {
			return i;
		}
	}
	return -1;
}

void *
map_get(struct map *map, const char *key)
{
	int index = map_getindex(map, key);
	if (index != -1) {
		return (void *) map->entry[index].value;
	}
	return NULL;
}

void *
map_set_act(struct map *map, const char *key, const void *value, bool overwrite)
{
	int index = map_getindex(map, key);
	if (index >= 0) {
		assert(overwrite);
		void *old = (void *) map->entry[index].value;
		map->entry[index].value = value;
		return old;
	}
	map->entry = realloc(map->entry, sizeof(struct entry) * ++map->n);
	map->entry[map->n - 1] = entry_create(key, value);
	return NULL;
}

void
map_set(struct map *map, const char *key, const void *value)
{
	map_set_act(map, key, value, false);
}

/* circuitbreaker: tracker to prevent ε-loops */
struct circuitbreaker {
	void *s;
	struct circuitbreaker *next;
};

static int
circuitbreaker_len(struct circuitbreaker *tr)
{
	int n = 0;
	for (struct circuitbreaker *next = tr; tr != NULL; tr = tr->next) {
		n++;
	}
	return n;
}

struct circuitbreaker *
circuitbreaker_create(void *s)
{
	struct circuitbreaker *tr = (struct circuitbreaker *)
		calloc(1, sizeof(struct circuitbreaker));
	tr->s = s;
	return tr;
}

struct circuitbreaker *
circuitbreaker_copy(struct circuitbreaker *tr)
{
	struct circuitbreaker *new = circuitbreaker_create(tr->s);
	if (tr->next != NULL) {
		new->next = circuitbreaker_copy(tr->next);
	}
	return new;
}


void
circuitbreaker_destroy(struct circuitbreaker *tr)
{
	assert(tr != NULL);
	if (tr->next != NULL) {
		circuitbreaker_destroy(tr->next);
	}
	free(tr);
}

bool
circuitbreaker_append(struct circuitbreaker *tr, void *s)
{
	for (; tr->s != s; tr = tr->next) {
		if (tr->next == NULL) {
			tr->next = circuitbreaker_create(s);
			return true;
		}
	}
	return false;
}

struct strbuilder {
	size_t cap;
	char *buf;
};

#define CAP_DEFAULT 100
#define CAP_MULT 2

struct strbuilder *
strbuilder_create()
{
	struct strbuilder *b = malloc(sizeof(struct strbuilder));
	b->cap = CAP_DEFAULT;
	b->buf = malloc(sizeof(char) * b->cap);
	sprintf(b->buf, "");
	return b;
}

char *
strbuilder_build(struct strbuilder *b)
{
	assert(b != NULL);
	int len = strlen(b->buf) + 1;
	char *s = malloc(sizeof(char) * len);
	snprintf(s, len, "%s", b->buf);
	free(b->buf);
	free(b);
	return s;
}

static void
strbuilder_realloc(struct strbuilder *b, size_t len)
{
	/* cap must be strictly > for null termination */
	while (b->cap <= len) {
		b->cap *= CAP_MULT;
		b->buf = realloc(b->buf, sizeof(char) * b->cap);
	}
}

static void
strbuilder_append(struct strbuilder *b, char *s, size_t len)
{
	int buflen = strlen(b->buf);
	strbuilder_realloc(b, buflen + len);
	size_t newlen = buflen + len + 1;
	strncpy(b->buf + buflen, s, newlen - buflen);
}

int
strbuilder_vprintf(struct strbuilder *b, const char *fmt, va_list ap)
{
	size_t len; char *buf;
	FILE *out = open_memstream(&buf, &len);
	int r = vfprintf(out, fmt, ap);
	fclose(out);
	strbuilder_append(b, buf, len);
	free(buf);
	return r;
}

int
strbuilder_printf(struct strbuilder *b, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = strbuilder_vprintf(b, fmt, ap);
	va_end(ap);
	return r;
}

int
strbuilder_puts(struct strbuilder *b, char *s)
{
	size_t len = strlen(s);
	strbuilder_append(b, s, len);
	return len;
}

void
strbuilder_putc(struct strbuilder *b, char c)
{
	strbuilder_printf(b, "%c", c);
}

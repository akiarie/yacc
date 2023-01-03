#ifndef YACC_MAPS
#define YACC_MAPS

struct intmap {
	struct ientry {
		int key;
		const void *value;
	} *entry;
	size_t n;
};

struct intmap *
intmap_create();

void
intmap_destroy(struct intmap *);

void *
intmap_get(struct intmap *, int key);

void
intmap_set(struct intmap *, int key, const void *value);

#endif

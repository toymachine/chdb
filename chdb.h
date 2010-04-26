#ifndef CHDB_H
#define CHDB_H

typedef struct _CHDB CHDB;

unsigned int murmurhash( const void * key, int len, unsigned int seed );

CHDB *chdb_open(const char *pathname);
int chdb_get(const CHDB *chdb, const char *key, size_t key_len, char **value, size_t *value_len);

#endif

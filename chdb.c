#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//unsigned int MurmurHashNeutral2 ( const void * key, int len, unsigned int seed )
//(http://sites.google.com/site/murmurhash/)
unsigned int murmurhash( const void * key, int len, unsigned int seed )
{
    //printf("mmhash: %.*s, len: %d, seed: %u\n", len, (char *)key, len, seed);

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k;

        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

typedef struct _CHDB CHDB;

struct _CHDB
{
    int fd;
    char *map;
    int version;
    int N;
    int K;
    int junk1;
    int junk2;
    size_t G_offset;
    size_t V_offset;
};

int _chdb_read_int(const CHDB *chdb, size_t offset)
{
    return ntohl(*((int *)(chdb->map + offset)));
}

/**
 * structure of chdb
 *
 * |+++++++++++|
 * | meta data | magic, version, number of keys (K), size of hashfunction table (N), f1 junk and f2 junk
 * |+++++++++++|
 * | graph (G) | graph part of the perfect hash (N * 4 bytes)
 * |+++++++++++|
 * | offsets   | offsets of key/value pairs indexed by hash (K * 4 bytes)
 * |+++++++++++|
 * | kv pairs  | actual key/value pairs, each items is 4 bytes key len KL, KL bytes key, 4 bytes value len VL, VL bytes value
 * |+++++++++++|
 */
CHDB *chdb_open(const char *pathname)
{
    int fd = open(pathname, O_RDONLY);
    if(fd == -1) {
        printf("could not open file: %s", pathname);
        abort();
    }
    struct stat st;
    if(-1 == fstat(fd, &st)) {
        printf("could not stat");
        abort();
    }
    char *map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(MAP_FAILED == map) {
        printf("could not map");
        abort();
    }

    CHDB *chdb = malloc(sizeof(CHDB));
    chdb->fd = fd;
    chdb->map = map;

    size_t offset = 0;

    if(0 != strncmp(chdb->map, "CHDB", 4)) {
        printf("not a chdb file");
        abort();
    }
    offset +=4;
    chdb->version = _chdb_read_int(chdb, offset);
    offset +=4;
    chdb->N = _chdb_read_int(chdb, offset);
    offset +=4;
    chdb->K = _chdb_read_int(chdb, offset);
    offset +=4;
    chdb->junk1 = _chdb_read_int(chdb, offset);
    offset +=4;
    chdb->junk2 = _chdb_read_int(chdb, offset);
    offset += 4;
    chdb->G_offset = offset;
    chdb->V_offset = chdb->G_offset + (chdb->N * 4);

    //printf("version: %d, N: %d, K: %d\n", chdb->version, chdb->N, chdb->K);
    //printf("junk1: %u, junk2: %u\n", chdb->junk1, chdb->junk2);
    //printf("G_offset: %d, V_offset: %d\n", chdb->G_offset, chdb->V_offset);

    return chdb;
}

unsigned int _chdb_perfecthash(const CHDB *chdb, const char *key, size_t key_len)
{
    #define G(i) (_chdb_read_int(chdb, chdb->G_offset + (i * 4)))
    #define F1() (murmurhash(key, key_len, chdb->junk1) % chdb->N)
    #define F2() (murmurhash(key, key_len, chdb->junk2) % chdb->N)

    //printf("f1: %d\n", murmurhash(key, key_len, chdb->junk1) % chdb->N);
    //printf("f2: %d\n", murmurhash(key, key_len, chdb->junk2) % chdb->N);

    return ( G(F1()) + G(F2())) % chdb->N;
}


int chdb_get(const CHDB *chdb, const char *key, size_t key_len, char **value, size_t *value_len)
{
    *value = NULL;
    *value_len = 0;

    //printf("lookup key: %.*s, len: %d\n", key_len, key, key_len);
    unsigned int hash = _chdb_perfecthash(chdb, key, key_len);
    //printf("hash: %u, K: %d\n", hash, chdb->K);
    if(hash == 0 || hash > chdb->K) {
        //printf("-1 1\n");
        return -1;
    }

    size_t kv_offset = _chdb_read_int(chdb, chdb->V_offset + (hash * 4));
    size_t key_found_len = _chdb_read_int(chdb, kv_offset);
    char *key_found = chdb->map + kv_offset + 4;
    if((key_found_len != key_len) ||
        (0 != strncmp(key, key_found, key_len))) {
        //printf("-1 2 %.*s, %.*s\n", key_len, key, key_found_len, key_found);
        return -1;
    }

    size_t value_offset = kv_offset + 4 + key_found_len;
    *value_len = _chdb_read_int(chdb, value_offset);
    *value = chdb->map + value_offset + 4;

    return 0;
}


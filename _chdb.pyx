#
# chdb.pyx
#
"""
"""
cdef extern from "chdb.h":
    unsigned int murmurhash(char *key, int len, unsigned int seed )

cdef extern from "Python.h":
    object PyString_FromStringAndSize(char *, int)
    int PyString_AsStringAndSize(object obj, char **s, Py_ssize_t *len) except -1

def _murmurhash(key, seed = 0):
    cdef char *key_chars
    cdef Py_ssize_t key_len
    PyString_AsStringAndSize(key, &key_chars, &key_len)
    return murmurhash(key_chars, key_len, seed)


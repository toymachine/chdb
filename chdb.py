import _chdb

from _chdb import _murmurhash as murmurhash

if __name__ == '__main__':
    #chdb = CHDB(sys.argv[1])
    #rint repr(chdb.get(sys.argv[2]))
    print murmurhash("blaat")

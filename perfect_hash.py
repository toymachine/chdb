#!/usr/bin/env python

# perfect_hash.py
#
# Outputs Python code for a minimal perfect hash.
# The hash is produced using the algorithm described in
# "Optimal algorithms for minimal perfect hashing",
# G. Havas, B.S. Majewski.  Available as a technical report
# from the CS department, University of Queensland
# (ftp://ftp.cs.uq.oz.au/).

# The algorithm works like this:
#   0) You have K keys, that you want to perfectly hash to a bunch
#      of hash values.
#
#   1) Choose a number N larger than K.  This is the number of
#      vertices in a graph G, and also the size of the resulting table.
#
#   2) Pick two random hash functions f1, f2, that output values from
#      0...N-1.
#
#   3) for key in keys:
#          h1 = f1(key) ; h2 = f2(key)
#          Draw an edge between vertices h1 and h2 of the graph.
#          Associate the desired hash value with that edge.
#
#   4) Check if G is acyclic; if no, go back to step 2.
#
#   5) Assign values to each vertex such that, for each edge, you can
#      add the values for the two vertices and get the desired value
#      for that edge -- which is the desired hash key.  This task is
#      dead easy, because the graph is acyclic.  This is done by
#      picking a vertex V, and assigning it a value of 0.  You then do a
#      depth-first search, assigning values to new vertices so that
#      they sum up properly.
#
#   6) f1, f2, and G now make up your perfect hash function.

import sys, random, string

import chdb

class Hash:
    """Random hash function
    For simplicity and speed, this doesn't implement any byte-level hashing
    scheme.  Instead, a random string is generated and prefixing to
    str(key), and then Python's hashing function is used."""

    def __init__(self, N):
        self.N = N
        #junk = ""
        #for i in range(10):
        #    junk = junk + random.choice(string.letters + string.digits)
        #self.junk = junk
        self.junk = random.randint(0, (1 << 32) - 1)

    def __call__(self, key):
        #key = self.junk + str(key)
        return chdb.murmurhash(key, self.junk) % self.N

    def generate_code(self):
        return "lambda key: hash(%s + str(key)) %% %i" % (repr(self.junk),
                                                          self.N)

WHITE, GREY, BLACK = 0,1,2
class Graph:
    """Graph class.  This class isn't particularly efficient or general,
    and only has the features I needed to implement this algorithm.

    num_vertices -- number of vertices
    edges -- maps 2-tuples of vertex numbers to the value for this
             edge.  If there's an edge between v1 and v2 (v1<v2),
             (v1,v2) is a key and the value is the edge's value.
    reachable_list -- maps a vertex V to the list of vertices
                      to which V is connected by edges.  Used
                      for traversing the graph.
    values -- numeric value for each vertex
    """

    def __init__(self, num_vertices):
        self.num_vertices = num_vertices
        self.edges = {}
        self.reachable_list = {}
        self.values = [-1] * num_vertices

    def connect(self, vertex1, vertex2, value):
        """Connect 'vertex1' and 'vertex2' with an edge, with associated
        value 'value'"""

        if vertex1 > vertex2: vertex1, vertex2 = vertex2, vertex1
#        if self.edges.has_key( (vertex1, vertex2) ):
#            raise ValueError, 'Collision: vertices already connected'
        self.edges[ (vertex1, vertex2) ] = value

        # Add vertices to each other's reachable list
        if not self.reachable_list.has_key( vertex1 ):
            self.reachable_list[ vertex1 ] = [vertex2]
        else:
            self.reachable_list[vertex1].append(vertex2)

        if not self.reachable_list.has_key( vertex2 ):
            self.reachable_list[ vertex2 ] = [vertex1]
        else:
            self.reachable_list[vertex2].append(vertex1)

    def get_edge_value(self, vertex1, vertex2):
        """Retrieve the value corresponding to the edge between
        'vertex1' and 'vertex2'.  Raises KeyError if no such edge"""
        if vertex1 > vertex2:
            vertex1, vertex2 = vertex2, vertex1
        return self.edges[ (vertex1, vertex2) ]

    def is_acyclic(self):
        "Returns true if the graph is acyclic, otherwise false"

        # This is done by doing a depth-first search of the graph;
        # painting each vertex grey and then black.  If the DFS
        # ever finds a vertex that isn't white, there's a cycle.
        colour = {}
        for i in range(self.num_vertices): colour[i] = WHITE

        # Loop over all vertices, taking white ones as starting
        # points for a traversal.
        for i in range(self.num_vertices):
            if colour[i] == WHITE:

                # List of vertices to visit
                visit_list = [ (None,i) ]

                # Do a DFS
                while visit_list:
                    # Colour this vertex grey.
                    parent, vertex = visit_list[0] ; del visit_list[0]
                    colour[vertex] = GREY

                    # Make copy of list of neighbours, removing the vertex
                    # we arrived here from.
                    neighbours = self.reachable_list.get(vertex, []) [:]
                    if parent in neighbours: neighbours.remove( parent )

                    for neighbour in neighbours:
                        if colour[neighbour] == WHITE:
                            visit_list.insert(0, (vertex, neighbour) )
                        elif colour[neighbour] != WHITE:
                            # Aha!  Already visited this node,
                            # so the graph isn't acyclic.
                            return 0

                    colour[vertex] = BLACK

        # We got through, so the graph is acyclic.
        return 1

    def assign_values(self):
        """Compute values for each vertex, so that they sum up
        properly to the associated value for each edge."""

        # Also done with a DFS; I simply copied the DFS code
        # from is_acyclic().  (Should generalize the logic so
        # one function could be used from both methods,
        # but I couldn't be bothered.)

        colour = {}
        for i in range(self.num_vertices): colour[i] = WHITE

        # Loop over all vertices, taking white ones as starting
        # points for a traversal.
        for i in range(self.num_vertices):
            if colour[i] == WHITE:
                # Set this vertex's value, arbitrarily, to zero.
                self.set_vertex_value( i, 0 )

                # List of vertices to visit
                visit_list = [ (None,i) ]

                # Do a DFS
                while visit_list:
                    # Colour this vertex grey.
                    parent, vertex = visit_list[0] ; del visit_list[0]
                    colour[vertex] = GREY

                    # Make copy of list of neighbours, removing the vertex
                    # we arrived here from.
                    neighbours = self.reachable_list.get(vertex, []) [:]
                    if parent in neighbours: neighbours.remove( parent )

                    for neighbour in self.reachable_list.get(vertex, []):
                        edge_value = self.get_edge_value( vertex, neighbour )
                        if colour[neighbour] == WHITE:
                            visit_list.insert(0, (vertex, neighbour) )

                            # Set new vertex's value to the desired
                            # edge value, minus the value of the
                            # vertex we came here from.
                            new_val = (edge_value -
                                       self.get_vertex_value( vertex ) )
                            self.set_vertex_value( neighbour,
                                                   new_val % self.num_vertices)

                    colour[vertex] = BLACK

        # Returns nothing
        return

    def __getitem__(self, index):
        if index < self.num_vertices: return index
        raise IndexError

    def get_vertex_value(self, vertex):
        "Get value for a vertex"
        return self.values[ vertex ]

    def set_vertex_value(self, vertex, value):
        "Set value for a vertex"
        self.values[ vertex ] = value

    def generate_code(self, width = 70):
        "Return nicely formatted table"
        print '[',
        pos = 0
        for v in self.values:
            v=str(v)+','
            print v,
            pos = pos + len(v) + 1
            if pos > width: print '\n ', ; pos = 0
        print ']'

def generate_hash(kvs, c = 1.1):
    """Print out code for a perfect minimal hash.  Input is a list of
    (key, desired hash value) tuples.  """

    # K is the number of keys.
    K = len(kvs)

    # We will be generating graphs of size N, where N = c * K.
    # The larger C is, the fewer trial graphs will need to be made, but
    # the resulting table is also larger.  Increase this starting value
    # if you're impatient.  After 5 failures, c will be increased by 0.1.

    num_graphs = 0                      # Number of trial graphs so far
    sys.stderr.write('Generating graphs... ')

    while 1:
        # N is the number of vertices in the graph G
        N = int(c*K)
        num_graphs = num_graphs + 1
        if (num_graphs % 5) == 0:
            # 5 failures, so increase c slightly
            c = c+ 0.1 ; sys.stderr.write(' -- increasing c to %0.1f\n' % c)
            sys.stderr.write('Generating graphs... ')

        # Output a progress message
        sys.stderr.write( str(num_graphs) + ' ')
        sys.stderr.flush()

        # Create graph w/ N vertices
        G = Graph(N)
        f1 = Hash(N)                    # Create 2 random hash functions
        f2 = Hash(N)

        # Connect vertices given by the values of the two hash functions
        # for each key.  Associate the desired hash value with each
        # edge.
        for i, (k, _) in enumerate(kvs):
            h1 = f1(k) ; h2 = f2(k)
            G.connect( h1,h2, i + 1)

        # Check if the resulting graph is acyclic; if it is,
        # we're done with step 1.
        if G.is_acyclic(): break

    # Now we have an acyclic graph, so we assign values to each vertex
    # such that, for each edge, you can add the values for the two vertices
    # involved and get the desired value for that edge -- which is the
    # desired hash key.  This task is dead easy, because the graph is acyclic.
    sys.stderr.write('\nAcyclic graph found; computing vertex values...\n')
    G.assign_values()

    # Sanity check the result by actually verifying that all the keys
    # hash to the right value.
    sys.stderr.write('Checking uniqueness of hash values...\n')
    ##print 'Hash values:'
    for i, (k, v) in enumerate(kvs):
        #print i, k, repr(v[0:40])
        perfecthash = (G.values[ f1(k) ] + G.values[ f2(k) ] ) % N
        ##print k, f1(k), f2(k), G.values[ f1(k) ], G.values[ f2(k)], perfecthash, v
        if perfecthash != (i + 1):
            print perfecthash, i, N
            assert False

    import struct
    assert struct.calcsize("!I") == 4

    def write_int(f, i):
        f.write(struct.pack("!I", i))

    f = open('hash.chdb', 'wb')
    f.write("CHDB")
    write_int(f, 1) #version number
    write_int(f, N) #size of G
    write_int(f, len(kvs)) #number of keys
    write_int(f, f1.junk)
    write_int(f, f2.junk)
    #values of G
    for i, value in enumerate(G.values):
        #print 'G::', i, value
        write_int(f, value)
    #key/value offsets
    pos_offsets = f.tell()
    for _ in range(len(kvs) + 1):
        write_int(f, 0)
    #key/values themselves
    for i, (k, v) in enumerate(kvs):
        start_offset = f.tell()
        write_int(f, len(k))
        f.write(k)
        write_int(f, len(v))
        f.write(v)
        end_offset = f.tell()
        f.seek(pos_offsets + (4 * (i + 1)))
        #print i, start_offset
        write_int(f, start_offset)
        f.seek(end_offset)
    f.close()

def rands(n):
    return ''.join([random.choice(string.letters) for i in range(n)])

if __name__ == '__main__':
    f = open(sys.argv[1])
    keys = set()
    kvs = []
    for line in f:
        line = line[:-1]
        key, value = line.split('\t', 1)
        key = key.strip()
        print repr(key), repr(value)
        if key in keys:
            assert False, "duplicate key"
        else:
            keys.add(key)
        kvs.append((key, value))
    generate_hash( kvs, 1.7 )
    f.close()


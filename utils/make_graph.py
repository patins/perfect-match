import numpy.random

def make_graph(graph_name, graph_size, distribution, *argv):
    distribution = getattr(numpy.random, distribution)
    print(distribution)
    f = open("{}.mtx".format(graph_name), "w")
    f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
    f.write("{} {} {}\n".format(graph_size, graph_size, int(graph_size *(graph_size-1)/2)))
    
    for i in range(graph_size):
        for j in range(i+1, graph_size):
            f.write("{} {} {}\n".format(i, j, distribution(*argv)))

    f.close()

make_graph("normal", 100, "normal", 5, 1)

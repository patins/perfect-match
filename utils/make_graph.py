import numpy as np
from scipy.sparse import coo_matrix
from scipy.io import mmwrite

def make_graph(graph_name, graph_size, distribution_name, **dist_kwargs):
    seed = 42
    np.random.seed(seed)
    distribution = getattr(np.random, distribution_name)
    trow = np.array([i for i in range(graph_size) for j in range(i+1, graph_size)])
    tcol = np.array([j for i in range(graph_size) for j in range(i+1, graph_size)])
    row = np.append(trow, tcol)
    col = np.append(tcol, trow)
    dist_kwargs['size'] = len(trow)
    values = np.tile(distribution(**dist_kwargs), 2)
    matrix = coo_matrix((values, (row, col)), shape=(graph_size, graph_size))
    mmwrite('../data/{}.mtx'.format(graph_name), matrix, "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))

make_graph("normal", 100, "normal", loc=5, scale=1)

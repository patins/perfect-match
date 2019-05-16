import numpy as np
from scipy.sparse import coo_matrix
from scipy.sparse import random
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
    mmwrite('../data/{}.mtx'.format(graph_name),
     matrix,
      "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))

def make_bipartite_graph(graph_name, left_size, right_size, distribution_name, **dist_kwargs):
    seed = 42
    np.random.seed(seed)
    distribution = getattr(np.random, distribution_name)
    left_ids = np.arange(0, left_size, dtype=np.int)
    right_ids = np.arange(left_size, left_size + right_size, dtype=np.int)
    trow = np.repeat(left_ids, right_size)
    tcol = np.tile(right_ids, left_size)
    row = np.append(trow, tcol)
    col = np.append(tcol, trow)
    dist_kwargs['size'] = len(trow)
    values = np.tile(distribution(**dist_kwargs), 2)
    matrix = coo_matrix((values, (row, col)), shape=(left_size+right_size, left_size+right_size))
    mmwrite('../data/{}.mtx'.format(graph_name), matrix, "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))

def make_incomplete_bipartite_graph(graph_name, left_size, right_size, distribution_name, prob_delete_edge, **dist_kwargs):
    seed = 42
    np.random.seed(seed)
    distribution = getattr(np.random, distribution_name)
    left_ids = np.arange(0, left_size, dtype=np.int)
    right_ids = np.arange(left_size, left_size + right_size, dtype=np.int)
    trow = np.repeat(left_ids, right_size)
    tcol = np.tile(right_ids, left_size)
    row = np.append(trow, tcol)
    col = np.append(tcol, trow)
    dist_kwargs['size'] = len(trow)
    raw_values = distribution(**dist_kwargs) * (np.random.uniform(size=len(trow)) < prob_delete_edge)
    values = np.tile(raw_values, 2)
    num_edges = sum([values[i] > 0 for i in range(len(values))])
    avg_degree = num_edges/(left_size + right_size)

    max_deg = 0
    for i in range(left_size + right_size):
        deg = 0
        for j in values[i:i+right_size]:
            if j> 0:
                deg += 1
        max_deg = max(deg, max_deg)
    print(max_deg)
    print(len(raw_values)) 
    print(num_edges/2, avg_degree)  
    matrix = coo_matrix((values, (row, col)), shape=(left_size+right_size, left_size+right_size))
    mmwrite('../data/{}.mtx'.format(graph_name), matrix, "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))

V = [100, 400, 500, 1000, 1500, 2000, 2500]

for i in V:
    make_incomplete_bipartite_graph("lognormal_incomplete_{}".format(i), int(i/2), int(i/2), "lognormal", 0.5, mean=0, sigma=1) 

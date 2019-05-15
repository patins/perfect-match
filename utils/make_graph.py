import numpy as np
from scipy.sparse import coo_matrix
from scipy.sparse import random
from scipy.io import mmwrite
from scipy.stats import lognorm


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
    mmwrite('../data/dyngreedy/{}.mtx'.format(graph_name),
     matrix,
      "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))
    matrix_reg = matrix.A
    biggest_value = np.unravel_index(np.argmax(matrix_reg, axis=None), matrix_reg.shape)
    print("The biggest value is at ", biggest_value, "with value", matrix.A[biggest_value])
    matrix_reg[biggest_value] = 0
    matrix_reg[biggest_value[::-1]] = 0
    mmwrite('../data/dyngreedy/{}_minus_one.mtx'.format(graph_name),
     coo_matrix(matrix_reg),
      "Matrix generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))



def make_incomplete_graph(graph_name, graph_size, distribution_name, **dist_kwargs):
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
    new_matrix = matrix.A
    for i in range(graph_size):
        for j in range(graph_size):
            if np.random.random() < 0.25:
                new_matrix[i, j] = 0
                new_matrix[j, i] = 0
    matrix = coo_matrix(new_matrix)
    mmwrite('../data/dyngreedy/{}.mtx'.format(graph_name),
     matrix,
      "Matrix incomplete generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))
    matrix_reg = matrix.A
    biggest_value = np.unravel_index(np.argmax(matrix_reg, axis=None), matrix_reg.shape)
    print("The biggest value is at ", biggest_value, "with value", matrix.A[biggest_value])
    matrix_reg[biggest_value] = 0
    matrix_reg[biggest_value[::-1]] = 0
    mmwrite('../data/dyngreedy/{}_minus_one.mtx'.format(graph_name),
     coo_matrix(matrix_reg),
      "Matrix incomplete generated with {} distribution {}. Seed {}".format(distribution_name, dist_kwargs, seed))


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

#make_incomplete_graph("normal_25", 25)
make_incomplete_graph("lognormal_10_incomplete", 10, "lognormal", mean=0, sigma=1)
make_incomplete_graph("lognormal_25_incomplete", 25, "lognormal", mean=0, sigma=1)
make_incomplete_graph("lognormal_100_incomplete", 100, "lognormal", mean=0, sigma=1)
make_incomplete_graph("lognormal_250_incomplete", 250, "lognormal", mean=0, sigma=1)
make_incomplete_graph("lognormal_1000_incomplete", 1000, "lognormal", mean=0, sigma=1)
make_incomplete_graph("lognormal_2500_incomplete", 2500, "lognormal", mean=0, sigma=1)
#make_bipartite_graph("normal_bipartite", 100, 100, "normal", loc=5, scale=1)

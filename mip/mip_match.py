from gurobipy import *
import numpy as np
from scipy.io import mmread
from collections import defaultdict

def build_model(matrix, b):
    """
    Matrix is assumed to be symmetric. Non-zero entries are considered to be matchable.
    """
    model = Model('mip_match')

    upper_right = [(matrix.row[i], matrix.col[i], matrix.data[i]) for i in range(matrix.getnnz()) if matrix.row[i] < matrix.col[i]]
    print('Upper right done')

    vMatches = model.addVars(
            [(i, j) for (i, j, v) in upper_right],
            vtype=GRB.BINARY,
            name=['m_{}_{}'.format(i, j) for (i, j, v) in upper_right])

    print('Matches done')

    model.setObjective(quicksum(np.array([v for (i, j, v) in upper_right]) * vMatches.values()), GRB.MAXIMIZE)
    
    print('Objective done')

    match_counts = defaultdict(lambda: 0)

    for (i, j, v) in upper_right:
        match_counts[i] += vMatches[(i, j)]
        match_counts[j] += vMatches[(i, j)]

    print('Match counts done')

    for i, match_count in match_counts.items():
        model.addConstr(match_count <= b, 'mcc_{}'.format(i))

    print('Match constraints done')

    model.params.MIPGap = 0
    model.optimize()

    print([(i, j) for (i, j, v) in upper_right if vMatches[(i, j)].X == 1])

if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print('Syntax is <mtx_filename> <b>')
        sys.exit(1)
    matrix_filename, b = sys.argv[-2:]
    try:
        b = int(b)
    except ValueError:
        print('Syntax is <mtx_filename> <b>')
        sys.exit(1)
    build_model(mmread(matrix_filename), b)

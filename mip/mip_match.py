from gurobipy import *
import numpy


def generate_indices(n):
    return [(i, j) for i in range(n) for j in range(i+1, n)]

def build_model_from_graph(graph_file, b):
    with open(graph_file) as f:
        lines = f.readlines()
        edges = lines[2:]
        n = int(lines[1].split(" ")[0])

    weights = [float(edge.split(" ")[-1]) for edge in edges]
    
    build_model(n, b, numpy.array(weights))
 
def build_model(n, b, weights=[]):
    model = Model('mip_match')
    indices = generate_indices(n)
    vMatches = model.addVars(indices, vtype=GRB.BINARY, name=['m_{}_{}'.format(i, j) for (i, j) in indices])

    if weights == []:
        weights = numpy.random.normal(loc=10, size=len(indices))
    model.setObjective(sum(weights * vMatches.values()), GRB.MINIMIZE)

    for i in range(n):
        match_count = 0
        for j in range(0, i):
            match_count += vMatches[(j, i)]
        for j in range(i+1, n):
            match_count += vMatches[(i, j)]
        model.addConstr(match_count >= b, 'mcc_{}'.format(i))

    model.params.MIPGap = 0
    model.optimize()

    print([idx for idx in indices if vMatches[idx].X == 1])


build_model_from_graph("../data/normal.mtx", 5)


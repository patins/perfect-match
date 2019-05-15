

struct edge_t {
  float weight;
  uint32_t id;
};

bool edge_t_comparator(const edge_t &a, const edge_t &b) {
  return a.weight > b.weight;
}

sparseEdge* phil_sort(sparseMatrix &matrix) {
  edge_t *edge_list = new edge_t[matrix.numEdges];
  for (size_t i = 0; i < matrix.numEdges; i++) {
    edge_list[i].weight = matrix.edges[i].weight;
    edge_list[i].id = i;
  }

  sort(edge_list, edge_list + matrix.numEdges, edge_t_comparator);

  sparseEdge *results = new sparseEdge[matrix.numEdges];
  
  for (size_t i = 0; i < matrix.numEdges; i++)
    results[i] = matrix.edges[edge_list[i].id];

  delete[] edge_list;

  return results;
}

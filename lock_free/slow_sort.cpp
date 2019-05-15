
/*
sparseEdge* sort_edges(sparseMatrix &matrix) {
  double *weights = new double[matrix.numEdges];
  for (size_t i = 0; i < matrix.numEdges; i++)
    weights[i] = matrix.edges[i].weight;

  vertex_id_t *sorted_ids = new vertex_id_t[matrix.numEdges];
  for (size_t i = 0; i < matrix.numEdges; i++)
    sorted_ids[i] = i;

  std::sort(sorted_ids, sorted_ids + matrix.numEdges,
      [&weights](const vertex_id_t& a, const vertex_id_t& b) -> bool
      {
        return weights[a] > weights[b];
      });


  sparseEdge *edges = new sparseEdge[matrix.numEdges];
  for (size_t i = 0; i < matrix.numEdges; i++)
    edges[i] = matrix.edges[sorted_ids[i]];

  delete[] weights;
  delete[] sorted_ids;

  return edges;
}
*/

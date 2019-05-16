// Stores an entire matrix (including 0s)
typedef struct denseMatrix {
  double* data;
  int numRows;
  int numColumns;
} denseMatrix;

// Stores individual edges (or entries of a dense matrix) and uses 0 indexing
typedef struct sparseEdge {
  double weight;
  int row;
  int column;
  bool matched;
} sparseEdge;

typedef struct sparseMatrix {
  sparseEdge* edges;
  int numEdges;
  int numRows;
  int numColumns;
} sparseMatrix;

denseMatrix read_symmetric_dense_matrix_file(char *filename);
sparseMatrix* read_symmetric_sparse_matrix_file(char *filename);
bool compareEdges(sparseEdge a, sparseEdge b);
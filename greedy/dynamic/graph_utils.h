using namespace std;

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

// Comparator for sparseEdge priority as a min-heap (lowest weights at top)
typedef struct compareEdges_queue {
    bool operator()(sparseEdge const& e1, sparseEdge const& e2)
    {
        return e1.weight < e2.weight;
    }
} compareEdges_queue;

// Vertex component to for dynamic updates
typedef struct vertex {
  vector<sparseEdge>* adjacent_edges; // TODO: change to a BST
  int adjacent_edge_count;
  priority_queue<sparseEdge, vector<sparseEdge>, compareEdges_queue>* matched_edges;
  int matched_edge_count;
} vertex;

// Wrapper to store sparseEdges and vertices with some metadata
typedef struct sparseMatrix {
  sparseEdge* edges;
  vertex* vertices;
  int numEdges;
  int numRows;
  int numColumns;
  double totalWeight;
} sparseMatrix;

denseMatrix read_symmetric_dense_matrix_file(char *filename);
sparseMatrix* read_symmetric_sparse_matrix_file(char *filename);
bool compareEdges(sparseEdge a, sparseEdge b);
bool compareEdgesIncreasing(sparseEdge a, sparseEdge b);
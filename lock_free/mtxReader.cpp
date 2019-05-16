#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include "mtxReader.h"

denseMatrix read_symmetric_dense_matrix_file(char *filename) {
  std::ifstream fin(filename);
  int M, N, L;
  while (fin.peek() == '%') fin.ignore(2048, '\n');
  fin >> M >> N >> L;

  // Create your matrix:
  double* matrixData;			     // Creates a pointer to the array
  matrixData = new double[M*N];	     // Creates the array of M*N size
  std::fill(matrixData, matrixData + M*N, 0.); // From <algorithm>, zeros all entries.

  // Read the data
  for (int l = 0; l < L; l++)
  {
  	int m, n;
  	float data;
  	fin >> m >> n >> data;
  	matrixData[(m-1) + (n-1)*M] = data;
    matrixData[(n-1) + (m-1)*N] = data;
  }

  fin.close();
  denseMatrix matrixFromFile;
  matrixFromFile.data = matrixData;
  matrixFromFile.numRows =  M;
  matrixFromFile.numColumns = N;
  return matrixFromFile;
}

sparseMatrix* read_symmetric_sparse_matrix_file(char *filename) {
  std::ifstream fin(filename);
  int M, N, L;
  while (fin.peek() == '%') fin.ignore(2048, '\n');
  fin >> M >> N >> L;

  sparseEdge* matrixEdges;
  matrixEdges = new sparseEdge[L];

  // TODO -- change sort to a streaming sort, like heapsort
  // TODO -- make this an array of pointers rather than storing structs in mem
  for (int l = 0; l < L; l++)
  {
  	int m, n;
  	float data;
    fin >> m >> n >> data;
  	sparseEdge edge;
    edge.weight = abs(data);
    edge.row = m-1;
    edge.column = n-1;
    matrixEdges[l] = edge;
  }
  fin.close();

  sparseMatrix* matrix = (sparseMatrix*) malloc(sizeof(sparseMatrix));
  matrix->edges = matrixEdges;
  matrix->numEdges = L;
  matrix->numRows = M;
  matrix->numColumns = N;
  return matrix;
}

bool compareEdges(sparseEdge a, sparseEdge b) { return a.weight > b.weight; }


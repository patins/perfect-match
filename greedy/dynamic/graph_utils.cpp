#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <queue>
#include "graph_utils.h"

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
  	double data;
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
  vertex* matrixVertices;
  matrixVertices = new vertex[M];

  // Step 1: Initialize all vertices
  for (int m = 0; m < M; m++) {
    vertex v;
    v.adjacent_edge_count = 0;
    v.matched_edge_count = 0;
    std::priority_queue<sparseEdge, std::vector<sparseEdge>, compareEdges_queue> p;
    v.matched_edges = &p;
    v.adjacent_edges = new sparseEdge[M]; // TODO: can save memory or use a BST 
    matrixVertices[m] = v;
  }

  // TODO -- change sort to a streaming sort, like heapsort
  // TODO -- make this an array of pointers rather than storing structs in mem
  // Step 2: Iterate through edges
  for (int l = 0; l < L; l++)
  {
  	int m, n;
  	double data;
    fin >> m >> n >> data;

    // Initialize edge to be added to edge list
  	sparseEdge edge;
    edge.weight = data;
    edge.row = m-1;
    edge.column = n-1;
    edge.matched = false;
    matrixEdges[l] = edge;

    // Add edge to its endpoint vertices
    vertex src = matrixVertices[m-1];
    vertex dst = matrixVertices[n-1];
    src.adjacent_edges[src.adjacent_edge_count] = edge;
    dst.adjacent_edges[dst.adjacent_edge_count] = edge;
    src.adjacent_edge_count++;
    dst.adjacent_edge_count++;
  }

  fin.close();

  sparseMatrix* matrix = (sparseMatrix*) malloc(sizeof(sparseMatrix));
  matrix->edges = matrixEdges;
  matrix->vertices = matrixVertices;
  matrix->numEdges = L;
  matrix->numRows = M;
  matrix->numColumns = N;
  return matrix;
}

bool compareEdges(sparseEdge a, sparseEdge b) { return a.weight > b.weight; }


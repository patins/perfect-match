#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "../utils/mtxReader.h"

using namespace std;
using namespace std::chrono;

double greedy(sparseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  int* numMatches = new int[matrix->numRows];
  int** matches = (int**) malloc(matrix->numRows * sizeof(int*));
  for(int i = 0; i < matrix->numRows ; i++){
    numMatches[i] = 0;
    matches[i] = (int*) malloc(B * sizeof(int));
  }
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){
    int r = matrix->edges[i].row;
    int c = matrix->edges[i].column;
    if(c != r && numMatches[r]<B && numMatches[c]<B){
         matches[r][numMatches[r]] = c;
         matches[c][numMatches[c]] = r;
         numMatches[r]++ ;
         numMatches[c]++ ;
         totalWeight += matrix->edges[i].weight;
       }
  }
  return totalWeight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  double totalWeight = greedy(matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <queue>
#include <assert.h>
#include "graph_utils.h"

using namespace std;
using namespace std::chrono;

double greedy_dynamic_match(sparseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){
    sparseEdge edge = matrix->edges[i];
    int r = edge.row;
    int c = edge.column;
    vertex src = matrix->vertices[r];
    vertex dst = matrix->vertices[c];

    if(c != r && src.matched_edge_count < B && dst.matched_edge_count < B){
         // Step 1: Set edge as matched
         edge.matched = true;

         // Step 2: Update src and dst vertices
         src.matched_edges->push(edge);
         dst.matched_edges->push(edge);
         src.matched_edge_count++;
         dst.matched_edge_count++;
         matrix->vertices[r] = src;
         matrix->vertices[c] = dst;

         // Step 3: Add to total weight
         totalWeight += edge.weight;
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

  // Algorithm starts here:
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  // matching M;
  double totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));
  // double totalWeight = update_greedy

  // End algorithm
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

//
// double greedy_updateable(sparseMatrix* matrix, int B, matching* M) {
//   M->incoming_matches = new priority_queue<
//     sparseEdge,
//     vector<sparseEdge>,
//     compareEdges_queue
//   >[matrix->numRows];
//   sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
//   int* numMatches = new int[matrix->numRows];
//   int** matches = new int*[matrix->numRows];
//   for(int i = 0; i < matrix->numRows ; i++){
//     numMatches[i] = 0;
//     matches[i] = new int[matrix->numRows];
//   }
//   double totalWeight = 0;
//   for(int i = 0; i < matrix->numEdges; i++){
//     int r = matrix->edges[i].row;
//     int c = matrix->edges[i].column;
//     if(c != r && numMatches[r]<B && numMatches[c]<B){
//          matches[r][c] = 1;
//          matches[c][r] = 1;
//          numMatches[r]++ ;
//          numMatches[c]++ ;
//          totalWeight += matrix->edges[i].weight;
//          M->incoming_matches[r].push(matrix->edges[i]);
//          M->incoming_matches[c].push(matrix->edges[i]);
//        }
//   }
//   M->outgoing_matches = matches;
//   M->numNodes = matrix->numRows;
//
//   return totalWeight;
// }

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
  matrix->totalWeight = totalWeight;
  return totalWeight;
}

// double update_matching(sparseMatrix* matrix, int B, sparseEdge* new_edge) {
//   assert(!edge->matched);
//   vertex src = matrix->vertices[new_edge->row];
//   vertex dst = matrix->vertices[new_edge->column];
//   int src_num_matches = src.matched_edge_count;
//   int dst_num_matches = dst.matched_edge_count;
//   sparseEdge src_lowest_edge = src.matched_edges->top();
//   sparseEdge dst_lowest_edge = dst.matched_edges->top();
//   double new_edge_weight = new_edge->weight;
//   bool scan_required = false;
//
//   // TODO: avoid computation by separating conditions into bools
//   if (src_num_matches < B && dst_num_matches < B) {
//     // Add new edge to matching!
//     src.matched_edges->push(new_edge);
//     dst.matched_edges->push(new_edge);
//     src.matched_edge_count++;
//     dst.matched_edge_count++;
//     matrix->totalWeight += new_edge_weight;
//   } else if (src_num_matches == B && dst_num_matches < B && new_edge_weight > src_lowest_edge.weight ) {
//     // Remove match from src, add new edge
//     scan_required = true;
//     sparseEdge edgeToRemove =
//
//   } else if (dst_num_matches == B && src_num_matches < B && new_edge_weight > dst_lowest_edge.weight ) {
//     // Remove match from dst, add new edge
//     scan_required = true;
//   } else if (src_num_matches == B && dst_num_matches == B && new_edge_weight > src_lowest_edge.weight && new_edge_weight > dst_lowest_edge.weight) {
//     // Remove match from both src and dst, add new edge
//     scan_required = true;
//     int other_endpoint = src.adjacent_edges->front();
//     src.adjacent_edges.remove()
//   }
//
//   if (scan_required) {
//     // Do the scan
//   }
//
//
//   )
//
//   // Do final updates
//   src.adjacent_edges.push_back(new_edge);
//   dst.adjacent_edges.push_back(new_edge);
//   src.adjacent_edge_count++;
//   dst.adjacent_edge_count++;
//   sort(src.adjacent_edges->begin(), src.adjacent_edges->end(), compareEdgesIncreasing);
//   sort(dst.adjacent_edges->begin(), dst.adjacent_edges->end(), compareEdgesIncreasing);
//   matrix->vertices[new_edge->row] = src;
//   matrix->vertices[new_edge->column] = dst;
//   return matrix->totalWeight;
// }

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();

  // Algorithm starts here -----------------
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  double totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));

  // End algorithm -------------------------

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

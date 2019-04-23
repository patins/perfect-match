#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <queue>
#include <limits>
#include <set>
#include <assert.h>
#include "graph_utils.h"

using namespace std;
using namespace std::chrono;

double greedy_dynamic_match(sparseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){
    sparseEdge* edge = matrix->edges[i];
    int r = edge->row;
    int c = edge->column;
    vertex* src = matrix->vertices[r];
    vertex* dst = matrix->vertices[c];

    if(c != r && src->matched_edge_count < B && dst->matched_edge_count < B){
         // Step 1: Set edge as matched
         edge->matched = true;

         // Step 2: Update src and dst vertices
         src->matched_edges->insert(edge);
         dst->matched_edges->insert(edge);
         src->matched_edge_count++;
         dst->matched_edge_count++;
         matrix->vertices[r] = src;
         matrix->vertices[c] = dst;

         // Step 3: Add to total weight
         totalWeight += edge->weight;
    }
  }
  matrix->totalWeight = totalWeight;
  return totalWeight;
}

double update_matching(sparseMatrix* matrix, int B, sparseEdge* new_edge) {
  assert(!new_edge->matched);
  vertex* src = matrix->vertices[new_edge->row];
  vertex* dst = matrix->vertices[new_edge->column];
  int src_num_matches = src->matched_edge_count;
  int dst_num_matches = dst->matched_edge_count;
  sparseEdge* src_lowest_edge = *(src->matched_edges->begin());
  sparseEdge* dst_lowest_edge = *(dst->matched_edges->begin());
  double new_edge_weight = new_edge->weight;
  bool scan_required1 = false;
  bool scan_required2 = false;
  sparseEdge* edgeToRemove1 = (sparseEdge*) NULL;
  sparseEdge* edgeToRemove2 = (sparseEdge*) NULL;
  vertex* scanVertex1;
  vertex* scanVertex2;

  // TODO: avoid computation by separating conditions into bools
  if (src_num_matches < B && dst_num_matches < B) {
    // Add new edge to matching!
    src->matched_edges->insert(new_edge);
    dst->matched_edges->insert(new_edge);
    src->matched_edge_count++;
    dst->matched_edge_count++;
    matrix->totalWeight += new_edge_weight;
    new_edge->matched = true;

  } else if (src_num_matches == B &&
     dst_num_matches < B &&
      new_edge_weight > src_lowest_edge->weight ) {
    // Remove match from src, add new edge
    scan_required1 = true;
    edgeToRemove1 = src_lowest_edge;
    if (matrix->vertices[edgeToRemove1->row] == src){
      scanVertex1 = matrix->vertices[edgeToRemove1->column];
    } else {
      scanVertex1 = matrix->vertices[edgeToRemove1->row];
    }

    scanVertex1->matched_edges->erase(edgeToRemove1);
    src->matched_edges->erase(edgeToRemove1);
    matrix->totalWeight -= edgeToRemove1->weight;
    src->matched_edge_count--;
    scanVertex1->matched_edge_count--;
    edgeToRemove1->matched = false;

    src->matched_edges->insert(new_edge);
    dst->matched_edges->insert(new_edge);
    src->matched_edge_count++;
    dst->matched_edge_count++;
    matrix->totalWeight += new_edge_weight;
    new_edge->matched = true;

  } else if (dst_num_matches == B &&
     src_num_matches < B &&
      new_edge_weight > dst_lowest_edge->weight ) {
    // Remove match from dst, add new edge
    scan_required2 = true;
    edgeToRemove2 = dst_lowest_edge;
    if (matrix->vertices[edgeToRemove1->row] == dst){
      scanVertex2 = matrix->vertices[edgeToRemove1->column];
    } else {
      scanVertex2 = matrix->vertices[edgeToRemove1->row];
    }

    scanVertex2->matched_edges->erase(edgeToRemove2);
    dst->matched_edges->erase(edgeToRemove2);
    matrix->totalWeight -= edgeToRemove2->weight;
    dst->matched_edge_count--;
    scanVertex2->matched_edge_count--;
    edgeToRemove2->matched = false;

    src->matched_edges->insert(new_edge);
    dst->matched_edges->insert(new_edge);
    src->matched_edge_count++;
    dst->matched_edge_count++;
    matrix->totalWeight += new_edge_weight;
    new_edge->matched = true;

  } else if (src_num_matches == B &&
     dst_num_matches == B &&
      new_edge_weight > src_lowest_edge->weight &&
       new_edge_weight > dst_lowest_edge->weight) {
    // Remove match from both src and dst, add new edge
    scan_required1 = true;
    scan_required2 = true;
    edgeToRemove1 = dst_lowest_edge;
    edgeToRemove2 = src_lowest_edge;
    if (matrix->vertices[edgeToRemove1->row] == dst){
      scanVertex2 = matrix->vertices[edgeToRemove1->column];
    } else {
      scanVertex2 = matrix->vertices[edgeToRemove1->row];
    }
    if (matrix->vertices[edgeToRemove1->row] == src){
      scanVertex1 = matrix->vertices[edgeToRemove1->column];
    } else {
      scanVertex1 = matrix->vertices[edgeToRemove1->row];
    }

    scanVertex1->matched_edges->erase(edgeToRemove1);
    src->matched_edges->erase(edgeToRemove1);
    src->matched_edge_count--;
    scanVertex1->matched_edge_count--;
    matrix->totalWeight -= edgeToRemove1->weight;
    edgeToRemove1->matched = false;

    scanVertex2->matched_edges->erase(edgeToRemove2);
    dst->matched_edges->erase(edgeToRemove2);
    dst->matched_edge_count--;
    scanVertex2->matched_edge_count--;
    matrix->totalWeight -= edgeToRemove2->weight;
    edgeToRemove2->matched = false;

    src->matched_edges->insert(new_edge);
    dst->matched_edges->insert(new_edge);
    src->matched_edge_count++;
    dst->matched_edge_count++;
    matrix->totalWeight += new_edge_weight;
    new_edge->matched = true;

  }

  double max_weight_1 = 0;
  double second_max_weight_1 = 0;
  sparseEdge* new_match_1 = (sparseEdge*) NULL;
  sparseEdge* second_new_match_1 = (sparseEdge*) NULL;
  vertex* other_end;

  if (scan_required1) {
    for (auto it = scanVertex1->adjacent_edges->begin(); it != scanVertex1->adjacent_edges->end(); ++it) {
      if (matrix->vertices[(*it)->row] == scanVertex1){
        other_end = matrix->vertices[(*it)->column];
      } else {
        other_end = matrix->vertices[(*it)->row];
      }
      if ((*it)->weight > max_weight_1 && !(*it)->matched && other_end->matched_edge_count < B){
        second_max_weight_1 = max_weight_1;
        second_new_match_1 = new_match_1;
        max_weight_1 = (*it)->weight;
        new_match_1 = *it;
      } else if ((*it)->weight > second_max_weight_1 && !(*it)->matched && other_end->matched_edge_count < B){
        second_max_weight_1 = (*it)->weight;
        second_new_match_1 = *it;
      }
    }
  }

  double max_weight_2 = 0;
  double second_max_weight_2 = 0;
  sparseEdge* new_match_2 = (sparseEdge*) NULL;
  sparseEdge* second_new_match_2 = (sparseEdge*) NULL;

  if (scan_required2) {
    for (auto it = scanVertex2->adjacent_edges->begin(); it != scanVertex2->adjacent_edges->end(); ++it) {
      if (matrix->vertices[(*it)->row] == scanVertex2){
        other_end = matrix->vertices[(*it)->column];
      } else {
        other_end = matrix->vertices[(*it)->row];
      }
      if ((*it)->weight > max_weight_2 && !(*it)->matched && other_end->matched_edge_count < B){
        second_max_weight_2 = max_weight_2;
        second_new_match_2 = new_match_2;
        max_weight_2 = (*it)->weight;
        new_match_2 = *it;
      } else if ((*it)->weight > second_max_weight_2 && !(*it)->matched && other_end->matched_edge_count < B){
        second_max_weight_2 = (*it)->weight;
        second_new_match_2 = *it;
      }
    }
  }

  double max_weight_both = 0;
  sparseEdge* new_match_both;

  if(new_match_1 != (sparseEdge*) NULL && new_match_2 != (sparseEdge*) NULL){
    if( max_weight_1 >  max_weight_2){
      max_weight_both = max_weight_1;
      new_match_both = new_match_1;
    } else {
      max_weight_both = max_weight_2;
      new_match_both = new_match_2;
    }

    matrix->vertices[new_match_both->row]->matched_edges->insert(new_match_both);
    matrix->vertices[new_match_both->column]->matched_edges->insert(new_match_both);

    matrix->vertices[new_match_both->row]->matched_edge_count++;
    matrix->vertices[new_match_both->column]->matched_edge_count++;

    matrix->totalWeight += max_weight_both;
    new_match_both->matched = true;
  }

  if(new_match_1 != (sparseEdge*) NULL && scanVertex1->matched_edge_count < B){
    vertex* other_end;
    if (matrix->vertices[new_match_1->row] == scanVertex1){
      other_end = matrix->vertices[new_match_1->column];
    } else {
      other_end = matrix->vertices[new_match_1->row];
    }
    if (other_end->matched_edge_count < B){
      matrix->vertices[new_match_1->row]->matched_edges->insert(new_match_1);
      matrix->vertices[new_match_1->column]->matched_edges->insert(new_match_1);

      matrix->vertices[new_match_1->row]->matched_edge_count++;
      matrix->vertices[new_match_1->column]->matched_edge_count++;

      matrix->totalWeight += max_weight_1;
      new_match_1->matched = true;
    } else if (second_new_match_1 != (sparseEdge*) NULL) {
      if (matrix->vertices[second_new_match_1->row] == scanVertex1){
        other_end = matrix->vertices[second_new_match_1->column];
      } else {
        other_end = matrix->vertices[second_new_match_1->row];
      }
      if (other_end->matched_edge_count < B){
        matrix->vertices[second_new_match_1->row]->matched_edges->insert(second_new_match_1);
        matrix->vertices[second_new_match_1->column]->matched_edges->insert(second_new_match_1);

        matrix->vertices[second_new_match_1->row]->matched_edge_count++;
        matrix->vertices[second_new_match_1->column]->matched_edge_count++;

        matrix->totalWeight += second_max_weight_1;
        second_new_match_1->matched = true;
      }
    }
  }

  if(new_match_2 != (sparseEdge*) NULL && scanVertex2->matched_edge_count < B){
    vertex* other_end;
    if (matrix->vertices[new_match_2->row] == scanVertex2){
      other_end = matrix->vertices[new_match_2->column];
    } else {
      other_end = matrix->vertices[new_match_2->row];
    }
    if (other_end->matched_edge_count < B){
      matrix->vertices[new_match_2->row]->matched_edges->insert(new_match_2);
      matrix->vertices[new_match_2->column]->matched_edges->insert(new_match_2);

      matrix->vertices[new_match_2->row]->matched_edge_count++;
      matrix->vertices[new_match_2->column]->matched_edge_count++;

      matrix->totalWeight += max_weight_2;
      new_match_2->matched = true;
    } else if (second_new_match_2 != (sparseEdge*) NULL) {
      if (matrix->vertices[second_new_match_2->row] == scanVertex2){
        other_end = matrix->vertices[second_new_match_2->column];
      } else {
        other_end = matrix->vertices[second_new_match_2->row];
      }
      if (other_end->matched_edge_count < B){
        matrix->vertices[second_new_match_2->row]->matched_edges->insert(second_new_match_2);
        matrix->vertices[second_new_match_2->column]->matched_edges->insert(second_new_match_2);

        matrix->vertices[second_new_match_2->row]->matched_edge_count++;
        matrix->vertices[second_new_match_2->column]->matched_edge_count++;

        matrix->totalWeight += second_max_weight_2;
        second_new_match_2->matched = true;
      }
    }
  }

  for (int i = 0; i < matrix->numRows; i++) {
    assert(matrix->vertices[i]->matched_edge_count <= B);
  }


  // Do final updates
  src->adjacent_edges->push_back(new_edge);
  dst->adjacent_edges->push_back(new_edge);
  src->adjacent_edge_count++;
  dst->adjacent_edge_count++;
  sort(src->adjacent_edges->begin(), src->adjacent_edges->end(), compareEdgesIncreasing);
  sort(dst->adjacent_edges->begin(), dst->adjacent_edges->end(), compareEdgesIncreasing);
  matrix->vertices[new_edge->row] = src;
  matrix->vertices[new_edge->column] = dst;
  return matrix->totalWeight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();

  // Algorithm starts here -----------------
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  sparseEdge* new_edge = new sparseEdge;
  new_edge->row = 16;
  new_edge->column = 2;
  new_edge->matched = false;
  new_edge->weight = 8.852731490654721e+00;

  double totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));
  double newTotal = update_matching(matrix, atoi(argv[2]), new_edge);
  // End algorithm -------------------------

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight 1 " << totalWeight << endl;
  cout << "Weight 2 " << newTotal << endl;
  return 1;
}

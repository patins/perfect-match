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

#define NDEBUG
#define LOWEST_MATCHED_EDGE(x) *(x->matched_edges->begin())

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

void add_edge(sparseEdge* e, sparseMatrix* matrix){
  assert(!(e->matched));
  vertex* src = matrix->vertices[e->row];
  vertex* dst = matrix->vertices[e->column];
  src->matched_edges->insert(e);
  dst->matched_edges->insert(e);
  src->matched_edge_count++;
  dst->matched_edge_count++;
  matrix->totalWeight += e->weight;
  e->matched = true;
}

void remove_edge(sparseEdge*e, sparseMatrix* matrix){
  assert(e->matched);
  vertex* src = matrix->vertices[e->row];
  vertex* dst = matrix->vertices[e->column];
  src->matched_edges->erase(e);
  dst->matched_edges->erase(e);
  matrix->totalWeight -= e->weight;
  src->matched_edge_count--;
  dst->matched_edge_count--;
  e->matched = false;
}

vertex* other_end(sparseEdge* e, vertex* v, sparseMatrix* matrix){
  vertex* oe;
  if (matrix->vertices[e->row] == v){
    oe = matrix->vertices[e->column];
  } else {
    oe = matrix->vertices[e->row];
  }
  return oe;
}

sparseEdge* max_weight_unmatched_edge(vertex* v, sparseMatrix* matrix, int B){
  vertex* oe;
  double max_val;
  sparseEdge* max_edge;

  max_edge = (sparseEdge*) NULL;
  max_val = 0;

  for (auto it = v->adjacent_edges->begin(); it != v->adjacent_edges->end(); ++it) {
    oe = other_end((*it), v, matrix);
    if(!(*it)->matched && (*it)->weight > max_val){
      if(oe->matched_edge_count < B){
          max_val = (*it)->weight;
          max_edge = (*it);
        } else {
          sparseEdge* other_end_lme = LOWEST_MATCHED_EDGE(oe);
          if((*it)->weight > other_end_lme->weight){
            max_val = (*it)->weight;
            max_edge = (*it);
          }
        }
      }
    }
    return max_edge;
}

void loop_matching_from_vertex(vertex* v, sparseMatrix* matrix, int B){
  int i = 0;
  vertex* curr_v = v;
  sparseEdge* curr_e = (sparseEdge*) NULL;
  curr_e = LOWEST_MATCHED_EDGE(curr_v);
  while (curr_e != (sparseEdge*) NULL){
    if( i%2 == 0 ){
      remove_edge(curr_e, matrix);
      curr_v = other_end(curr_e, curr_v, matrix);
      sparseEdge* lol;
      lol = max_weight_unmatched_edge(curr_v, matrix,B);
      assert(curr_e != lol);
      curr_e = lol;
      i += 1;
    } else {
      add_edge(curr_e, matrix);
      curr_v = other_end(curr_e, curr_v, matrix);
      if(curr_v->matched_edge_count <= B){
        curr_e = (sparseEdge*) NULL;
      } else {
        curr_e = LOWEST_MATCHED_EDGE(curr_v);
      }
      i += 1;
    }
  }
}

void loop_matching_from_vertex_double(vertex* v, vertex* u, sparseMatrix* matrix, int B){
  vertex* curr_v_1 = v;
  vertex* curr_v_2 = u;
  sparseEdge* curr_e_1 = LOWEST_MATCHED_EDGE(curr_v_1);
  sparseEdge* curr_e_2 = LOWEST_MATCHED_EDGE(curr_v_2);
  remove_edge(curr_e_1, matrix);
  curr_v_1 = other_end(curr_e_1, curr_v_1, matrix);
  remove_edge(curr_e_2, matrix);
  curr_v_2 = other_end(curr_e_2, curr_v_2, matrix);
  curr_e_1 = max_weight_unmatched_edge(curr_v_1, matrix,B);
  curr_e_2 = max_weight_unmatched_edge(curr_v_2, matrix,B);

  while (curr_e_1 != (sparseEdge*) NULL && curr_e_2 != (sparseEdge*) NULL){
    if(curr_e_1 != (sparseEdge*) NULL && curr_e_2 == (sparseEdge*) NULL){
        add_edge(curr_e_1, matrix);
        curr_v_1 = other_end(curr_e_1, curr_v_1, matrix);
        if(curr_v_1->matched_edge_count <= B){
          curr_e_1 = (sparseEdge*) NULL;
        } else {
          curr_e_1 = LOWEST_MATCHED_EDGE(curr_v_1);
          remove_edge(curr_e_1, matrix);
          curr_v_1 = other_end(curr_e_1, curr_v_1, matrix);
          sparseEdge* lol;
          lol = max_weight_unmatched_edge(curr_v_1, matrix,B);
          assert(curr_e_1 != lol);
          curr_e_1 = lol;
        }
      }
    else if(curr_e_1 == (sparseEdge*) NULL && curr_e_2 != (sparseEdge*) NULL){
      add_edge(curr_e_2, matrix);
      curr_v_2 = other_end(curr_e_2, curr_v_2, matrix);
      if(curr_v_2->matched_edge_count <= B){
        curr_e_2 = (sparseEdge*) NULL;
      } else {
        curr_e_2 = LOWEST_MATCHED_EDGE(curr_v_2);
        remove_edge(curr_e_2, matrix);
        curr_v_2 = other_end(curr_e_2, curr_v_2, matrix);
        sparseEdge* lol;
        lol = max_weight_unmatched_edge(curr_v_2, matrix,B);
        assert(curr_e_2 != lol);
        curr_e_2 = lol;
      }
    }
    else if(curr_e_1->weight > curr_e_2->weight){
      add_edge(curr_e_1, matrix);
      curr_v_1 = other_end(curr_e_1, curr_v_1, matrix);
      if(curr_v_1->matched_edge_count <= B){
        curr_e_1 = (sparseEdge*) NULL;
      } else {
        curr_e_1 = LOWEST_MATCHED_EDGE(curr_v_1);
        remove_edge(curr_e_1, matrix);
        curr_v_1 = other_end(curr_e_1, curr_v_1, matrix);
        sparseEdge* lol;
        lol = max_weight_unmatched_edge(curr_v_1, matrix,B);
        assert(curr_e_1 != lol);
        curr_e_1 = lol;
      }
    }
    else{
      add_edge(curr_e_2, matrix);
      curr_v_2 = other_end(curr_e_2, curr_v_2, matrix);
      if(curr_v_2->matched_edge_count <= B){
        curr_e_2 = (sparseEdge*) NULL;
      } else {
        curr_e_2 = LOWEST_MATCHED_EDGE(curr_v_2);
        remove_edge(curr_e_2, matrix);
        curr_v_2 = other_end(curr_e_2, curr_v_2, matrix);
        sparseEdge* lol;
        lol = max_weight_unmatched_edge(curr_v_2, matrix,B);
        assert(curr_e_2 != lol);
        curr_e_2 = lol;
      }
    }
  }
}

double update_matching(sparseMatrix* matrix, int B, sparseEdge* new_edge) {
  assert(!new_edge->matched);
  vertex* src = matrix->vertices[new_edge->row];
  vertex* dst = matrix->vertices[new_edge->column];
  int src_num_matches = src->matched_edge_count;
  int dst_num_matches = dst->matched_edge_count;
  double new_edge_weight = new_edge -> weight;
  sparseEdge* src_lowest_edge = LOWEST_MATCHED_EDGE(src);
  sparseEdge* dst_lowest_edge = LOWEST_MATCHED_EDGE(dst);

  src->adjacent_edges->push_back(new_edge);
  dst->adjacent_edges->push_back(new_edge);
  src->adjacent_edge_count++;
  dst->adjacent_edge_count++;
  sort(src->adjacent_edges->begin(), src->adjacent_edges->end(), compareEdgesIncreasing);
  sort(dst->adjacent_edges->begin(), dst->adjacent_edges->end(), compareEdgesIncreasing);
  matrix->vertices[new_edge->row] = src;
  matrix->vertices[new_edge->column] = dst;

  // TODO: avoid computation by separating conditions into bools
  if (src_num_matches < B && dst_num_matches < B) {
    // Add new edge to matching!
    // cout << "no collision" << endl;
    add_edge(new_edge, matrix);
  } else if (src_num_matches == B &&
     dst_num_matches < B &&
      new_edge_weight > src_lowest_edge->weight ) {
    // Remove match from src, add new edge
    // cout << "src collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex(src, matrix, B);
  } else if (dst_num_matches == B &&
     src_num_matches < B &&
      new_edge_weight > dst_lowest_edge->weight ) {
    // Remove match from dst, add new edge
    // cout << "dst collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex(dst, matrix, B);
  } else if (src_num_matches == B &&
     dst_num_matches == B &&
      new_edge_weight > src_lowest_edge->weight &&
       new_edge_weight > dst_lowest_edge->weight) {
    // Remove match from both src and dst, add new edge
    // cout << "both collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex_double(src,dst, matrix, B);
  }

  for (int i = 0; i < matrix->numRows; i++) {
    assert(matrix->vertices[i]->matched_edge_count <= B);
  }


  // Do final updates

  return matrix->totalWeight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  // Algorithm starts here -----------------
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  sparseEdge* new_edge = new sparseEdge;
  new_edge->row = 1-1;
  new_edge->column = 1-1;
  new_edge->matched = false;
  new_edge->weight = 24.85530664763014+00;

  double totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));
  auto start = high_resolution_clock::now();
  double newTotal = update_matching(matrix, atoi(argv[2]), new_edge);
  // End algorithm -------------------------

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Original weight " << totalWeight << endl;
  cout << "New weight " << newTotal << endl;
  return 1;
}

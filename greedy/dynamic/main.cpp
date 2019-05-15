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
#define OTHER_END(e, v, matrix) ((matrix->vertices[e->row] == v) ? (matrix->vertices[e->column]) : ( matrix->vertices[e->row]))

double greedy_dynamic_match(sparseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){

    sparseEdge* edge = matrix->edges[i];
    //cout << " Current edge from " << edge->row << " to " << edge->column << " weight " << edge->weight << endl;
    int r = edge->row;
    int c = edge->column;
    vertex* src = matrix->vertices[r];
    vertex* dst = matrix->vertices[c];

    if(c != r && src->matched_edge_count < B && dst->matched_edge_count < B){
         // Step 1: Set edge as matched
         edge->matched = true;
         //cout << " Added edge from " << edge->row << " to " << edge->column << " weight " << edge->weight << endl;
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
  // cout << endl << endl;
  return totalWeight;
}


double greedy_dynamic_match_naive_update(sparseMatrix* matrix, int B, sparseEdge* new_edge) {
  for(int i=0; i < matrix->numRows; i++){
    matrix->vertices[i]->matched_edge_count = 0;
  }
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  bool new_edge_seen = false;
  double totalWeight = 0;
  int r;
  int c;
  vertex* src;
  vertex* dst;
  for(int i = 0; i < matrix->numEdges; i++){
    sparseEdge* edge = matrix->edges[i];
    if(!new_edge_seen && new_edge->weight > edge->weight){
      new_edge_seen = true;
      r = new_edge->row;
      c = new_edge->column;
      src = matrix->vertices[r];
      dst = matrix->vertices[c];

      if(c != r && src->matched_edge_count < B && dst->matched_edge_count < B){
           // Step 1: Set edge as matched
           if(!new_edge->matched){
             // cout << " Added edge from " << new_edge->row << " to " << new_edge->column << " weight " << new_edge->weight << endl;
             src->matched_edges->insert(new_edge);
             dst->matched_edges->insert(new_edge);
             // Step 3: Add to total weight
             totalWeight += new_edge->weight;
             new_edge->matched = true;
           }
           src->matched_edge_count++;
           dst->matched_edge_count++;
         }
    }
    r = edge->row;
    c = edge->column;
    src = matrix->vertices[r];
    dst = matrix->vertices[c];
    if(c != r && src->matched_edge_count < B && dst->matched_edge_count < B){
         // Step 1: Set edge as matched
         if(!edge->matched){
           // cout << " Added edge from " << edge->row << " to " << edge->column << " weight " << edge->weight << endl;
           edge->matched = true;

           // Step 2: Update src and dst vertices
           src->matched_edges->insert(edge);
           dst->matched_edges->insert(edge);

           // Step 3: Add to total weight
           totalWeight += edge->weight;
         }

         src->matched_edge_count++;
         dst->matched_edge_count++;

    } else if (edge->matched) {
        // cout << " Removed edge from " << edge->row << " to " << edge->column << " weight " << edge->weight << endl;
        src->matched_edges->erase(edge);
        dst->matched_edges->erase(edge);
        totalWeight -= edge->weight;
        edge->matched = false;
      }
    }
  return matrix->totalWeight + totalWeight;
}

void add_edge(sparseEdge* e, sparseMatrix* matrix){
  assert(!(e->matched));
  // cout << " Added edge from " << e->row << " to " << e->column << " weight " << e->weight << endl;
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
  // cout << " Removed edge from " << e->row << " to " << e->column << " weight " << e->weight << endl;
  vertex* src = matrix->vertices[e->row];
  vertex* dst = matrix->vertices[e->column];
  src->matched_edges->erase(e);
  dst->matched_edges->erase(e);
  matrix->totalWeight -= e->weight;
  src->matched_edge_count--;
  dst->matched_edge_count--;
  e->matched = false;
}

sparseEdge* max_weight_unmatched_edge(vertex* v, sparseMatrix* matrix, int B) {
  if (v->matched_edge_count == B) {
    return (sparseEdge*) NULL;
  }
  sparseEdge* lowest_matched_edge = LOWEST_MATCHED_EDGE(v);
  int end_index = lower_bound(v->adjacent_edges->begin(), v->adjacent_edges->end(), lowest_matched_edge, compareEdgesIncreasing) - v->adjacent_edges->begin();
  sparseEdge* current_edge;
  vertex* oe;
  for (int i = end_index - 1; i >= 0; i--) {
    current_edge = v->adjacent_edges->at(i);
    assert(!current_edge->matched);
    oe = OTHER_END(current_edge, v, matrix);
    if (oe->matched_edge_count < B) {
      return current_edge;
    } else {
      sparseEdge* other_end_lme = LOWEST_MATCHED_EDGE(oe);
      if (current_edge->weight > other_end_lme->weight) {
        return current_edge;
      }
    }
  }
  return (sparseEdge*) NULL;
}

void loop_matching_from_vertex(vertex* v, sparseMatrix* matrix, int B){
  int i = 0;
  vertex* curr_v = v;
  sparseEdge* curr_e = (sparseEdge*) NULL;
  curr_e = LOWEST_MATCHED_EDGE(curr_v);
  while (curr_e != (sparseEdge*) NULL) {
    if( i % 2 == 0 ){
      remove_edge(curr_e, matrix);
      curr_v = OTHER_END(curr_e, curr_v, matrix);
      sparseEdge* lol;
      lol = max_weight_unmatched_edge(curr_v, matrix,B);
      assert(curr_e != lol);
      curr_e = lol;
      i += 1;
    } else {
      add_edge(curr_e, matrix);
      curr_v = OTHER_END(curr_e, curr_v, matrix);
      curr_e = (curr_v->matched_edge_count <= B) ? ((sparseEdge*) NULL) : (LOWEST_MATCHED_EDGE(curr_v));
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
  curr_v_1 = OTHER_END(curr_e_1, curr_v_1, matrix);
  remove_edge(curr_e_2, matrix);
  curr_v_2 = OTHER_END(curr_e_2, curr_v_2, matrix);
  curr_e_1 = max_weight_unmatched_edge(curr_v_1, matrix,B);
  curr_e_2 = max_weight_unmatched_edge(curr_v_2, matrix,B);

  while (curr_e_1 != (sparseEdge*) NULL || curr_e_2 != (sparseEdge*) NULL){
    //  cout << " Considering edge from " << curr_e_1->row << " to " << curr_e_1->column << " weight " << curr_e_1->weight << endl;
    //  cout << " Considering edge from " << curr_e_2->row << " to " << curr_e_2->column << " weight " << curr_e_2->weight << endl<<endl;
    if((curr_e_1 != (sparseEdge*) NULL && curr_e_2 == (sparseEdge*) NULL) ||
      (curr_e_1 != (sparseEdge*) NULL && (curr_e_1->weight > curr_e_2->weight))) {
        if(curr_e_1->matched){
          curr_e_1 = max_weight_unmatched_edge(curr_v_1, matrix,B);
        }
        if(curr_v_1->matched_edge_count == B){
          curr_e_1 = (sparseEdge*) NULL;
        }
        if(curr_e_1 != (sparseEdge*) NULL) {
          add_edge(curr_e_1, matrix);
          curr_v_1 = OTHER_END(curr_e_1, curr_v_1, matrix);
          if(curr_v_1->matched_edge_count <= B){
            curr_e_1 = (sparseEdge*) NULL;
          } else {
            curr_e_1 = LOWEST_MATCHED_EDGE(curr_v_1);
            remove_edge(curr_e_1, matrix);
            curr_v_1 = OTHER_END(curr_e_1, curr_v_1, matrix);
            sparseEdge* candidate;
            candidate = max_weight_unmatched_edge(curr_v_1, matrix,B);
            assert(curr_e_1 != candidate);
            curr_e_1 = candidate;
          }
        }
      }
    else if((curr_e_1 == (sparseEdge*) NULL && curr_e_2 != (sparseEdge*) NULL) ||
      (curr_e_2 != (sparseEdge*) NULL && (curr_e_1->weight <= curr_e_2->weight))) {
        if(curr_e_2->matched){
          curr_e_2 = max_weight_unmatched_edge(curr_v_1, matrix,B);
        }
        if(curr_v_2->matched_edge_count == B){
          curr_e_2 = (sparseEdge*) NULL;
        }
        if(curr_e_2 != (sparseEdge*) NULL) {
        add_edge(curr_e_2, matrix);
        curr_v_2 = OTHER_END(curr_e_2, curr_v_2, matrix);
        if(curr_v_2->matched_edge_count <= B){
          curr_e_2 = (sparseEdge*) NULL;
        } else {
          curr_e_2 = LOWEST_MATCHED_EDGE(curr_v_2);
          remove_edge(curr_e_2, matrix);
          curr_v_2 = OTHER_END(curr_e_2, curr_v_2, matrix);
          sparseEdge* candidate;
          candidate = max_weight_unmatched_edge(curr_v_2, matrix,B);
          assert(curr_e_2 != candidate);
          curr_e_2 = candidate;
        }
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
    // cout << endl << "no collision" << endl;
    add_edge(new_edge, matrix);
  } else if (src_num_matches == B &&
     dst_num_matches < B &&
      new_edge_weight > src_lowest_edge->weight ) {
    // Remove match from src, add new edge
    // cout << endl << "src collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex(src, matrix, B);
  } else if (dst_num_matches == B &&
     src_num_matches < B &&
      new_edge_weight > dst_lowest_edge->weight ) {
    // Remove match from dst, add new edge
    // cout << endl << "dst collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex(dst, matrix, B);
  } else if (src_num_matches == B &&
     dst_num_matches == B &&
      new_edge_weight > src_lowest_edge->weight &&
       new_edge_weight > dst_lowest_edge->weight) {
    // Remove match from both src and dst, add new edge
     // cout << endl << "both collision" << endl;
    add_edge(new_edge, matrix);
    loop_matching_from_vertex_double(src,dst, matrix, B);
  }

  for (int i = 0; i < matrix->numRows; i++) {
    assert(matrix->vertices[i]->matched_edge_count <= B);
  }

  return matrix->totalWeight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  } else if (argc != 6) {
    cout << "New edge not specified.";
    return 0;
  }
  // Algorithm starts here -----------------
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  sparseEdge* new_edge = new sparseEdge;
  new_edge->row = atoi(argv[3])-1;
  new_edge->column = atoi(argv[4])-1;
  new_edge->matched = false;
  new_edge->weight = atof(argv[5]);

  double totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));

  auto start = high_resolution_clock::now();
  double naiveWeight = greedy_dynamic_match_naive_update(matrix, atoi(argv[2]), new_edge);
  auto stop = high_resolution_clock::now();
  auto duration_g = duration_cast<microseconds>(stop - start);
  matrix = read_symmetric_sparse_matrix_file(argv[1]);
  new_edge->matched = false;
  for(int i=0; i < matrix->numEdges; i++){
    matrix->edges[i]->matched = false;
  }
  totalWeight = greedy_dynamic_match(matrix, atoi(argv[2]));
  start = high_resolution_clock::now();
  double newTotal = update_matching(matrix, atoi(argv[2]), new_edge);
  // End algorithm -------------------------

  stop = high_resolution_clock::now();
  auto duration_d = duration_cast<microseconds>(stop - start);
  cout << "b: " << argv[2] << endl;
  cout << "Time taken by dynamic: "
    << duration_d.count() << " microseconds" << endl;
    cout << "Time taken by greedy: "
      << duration_g.count() << " microseconds" << endl;
  cout << "Original weight " << totalWeight << endl;
  cout << "New weight " << newTotal << endl;
  cout << "Naive new weight " << naiveWeight << endl;
  return 1;
}

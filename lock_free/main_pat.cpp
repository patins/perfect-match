#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "../utils/mtxReader.h"
#include <queue>
#include <numeric>
#include <vector>
#include <list>
#include <deque>

using namespace std;
using namespace std::chrono;
bool compareSparseEdge(const sparseEdge& a, const sparseEdge& b) {
  return (a.row == b.row) ? a.weight > b.weight : a.row < b.row;
}

bool compareSparseEdgeRowC(const sparseEdge& a, int b) {
  return a.row < b;
}

bool compareSparseEdgeColumn(const sparseEdge& a, const sparseEdge& b) {
  return a.column < b.column;
}

bool compareSparseEdgeMinWeight(const sparseEdge& a, const sparseEdge &b) {
  return a.weight > b.weight;
}

struct vertexWeightPair {
  int vertex;
  double weight;
};

double lock_free_matching(sparseMatrix *matrix, int b) {
  vector<int> *vertex_queue = new vector<int>();
  // initialize vertex queue
  for (int i = 0; i < matrix->numRows; i++) {
    vertex_queue->push_back(i);
  }
  vector<int> *new_vertex_queue;

  vector<vertexWeightPair> *vertex_suitors = new vector<vertexWeightPair>[matrix->numRows]();

  sort(matrix->edges, matrix->edges + matrix->numEdges, compareSparseEdge);

  double match_weight = 0;

  while (!vertex_queue->empty()) {
    new_vertex_queue = new vector<int>();

    // TODO when this is parallelized we need a lock free way of adding to it
    vector<sparseEdge> proposed_matches;

    for (int v : *vertex_queue) {
      int unmatched_spots = b - vertex_suitors[v].size();
      // returns the first instance of edge with row v, or last edge in array
      sparseEdge *e = lower_bound(matrix->edges, matrix->edges + matrix->numEdges, v, compareSparseEdgeRowC);
      // for all edges with row v
      int v_proposed_matches = 0;
      for (; (e != matrix->edges + matrix->numEdges) && e->row == v && v_proposed_matches < unmatched_spots; e++) {
        bool already_matched = false;
        for (sparseEdge &e2 : vertex_suitors[v]) {
          if ((e2.row == e->row && e2.column == e->column) || (e2.row == e->column && e2.column == e->row)) {
            already_matched = true;
            break;
          }
        }

        if (!already_matched) {
          proposed_matches.push_back(*e);
          v_proposed_matches++;
        }
      }
    }

    sort(proposed_matches.begin(), proposed_matches.end(), compareSparseEdgeColumn);

    vector<sparseEdge> completed_matches;

    for (sparseEdge &e : proposed_matches) {
      vector<sparseEdge> *suitor_heap = vertex_suitors + e.column;
      if (suitor_heap->size() < b) {
        suitor_heap->push_back(e);
        push_heap(suitor_heap->begin(), suitor_heap->end(), compareSparseEdgeMinWeight);
        completed_matches.push_back(e);
        match_weight += e.weight;
      } else if (suitor_heap->front().weight < e.weight) {
        // TODO this needs to be removed from the other heap as well
        pop_heap(suitor_heap->begin(), suitor_heap->end(), compareSparseEdgeMinWeight);
        match_weight -= suitor_heap->back().weight;

        suitor_heap->pop_back();
        suitor_heap->push_back(e);
        push_heap(suitor_heap->begin(), suitor_heap->end(), compareSparseEdgeMinWeight);
        completed_matches.push_back(e);
        match_weight += e.weight;
      }
    }

    // sort for parallel

    for (sparseEdge &e : completed_matches) {
      vector<sparseEdge> *suitor_heap = vertex_suitors + e.row;
      suitor_heap->push_back(e);
      push_heap(suitor_heap->begin(), suitor_heap->end(), compareSparseEdgeMinWeight);
    }

    delete vertex_queue;
    vertex_queue = new_vertex_queue;
    new_vertex_queue = NULL;
  }

  delete vertex_queue;

  for (int r = 0; r < matrix->numRows; r++) {
    cout << r << ": ";
    for (sparseEdge &e : vertex_suitors[r]) {
      cout << e.weight << " ";
    }
    cout << endl;
  }
  cout << " ---- " << endl;
  
  for (int r = 0; r < matrix->numRows; r++) {
    cout << r << ": ";
    for (sparseEdge &e : vertex_suitors[r]) {
      if (e.row == r)
        cout << e.column << " ";
      else
        cout << e.row << " ";
    }
    cout << endl;
  }
  cout << " ---- " << endl;

  for (int r = 0; r < matrix->numRows; r++) {
    for (sparseEdge &e : vertex_suitors[r]) {
      bool valid = false;
      if (e.row == r) {
        for (sparseEdge &e2 : vertex_suitors[e.column]) {
          if ((e2.row == e.row && e2.column == e.column) || (e2.row == e.column && e2.column == e.row)) {
            if (!valid)
              valid = true;
            else
              cout << "BAD2" << endl;
          }
        }
      } else {
        for (sparseEdge &e2 : vertex_suitors[e.row]) {
          if ((e2.row == e.row && e2.column == e.column) || (e2.row == e.column && e2.column == e.row)) {
            if (!valid)
              valid = true;
            else
              cout << "BAD2" << endl;
          }
        }
      }
      if(!valid)
        cout << "BAD " << e.row << " " << e.column << endl;
    }
  }

  delete[] vertex_suitors;
  return match_weight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();
  sparseMatrix *matrix = read_symmetric_sparse_matrix_file(argv[1]);
  double totalWeight = lock_free_matching(matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

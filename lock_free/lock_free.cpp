#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cassert>
#include "../utils/mtxReader.h"
#include "ips4o.hpp"
#include <set>

using namespace std;
using namespace std::chrono;

const bool DEBUG = false;

typedef uint32_t vertex_id_t;
typedef double weight_t;

struct suitor_t {
  vertex_id_t suitor_id;
  weight_t weight;
};

struct bump_result_t {
  bool inserted;
  bool removed;
  vertex_id_t removed_id;
};

bool suitors_comparator(suitor_t &a, suitor_t &b) {
  return a.weight > b.weight;
}

class Suitors {
  size_t _capacity;
public:
  vector<suitor_t> _suitors;
  Suitors(size_t capacity): _capacity(capacity) {}
  bool is_suitor(vertex_id_t v) {
    for (suitor_t suitor : _suitors) {
      if (suitor.suitor_id == v) {
        return true;
      }
    }
    return false;
  }
  bump_result_t attempt_bump(suitor_t proposed_suitor) {
    bump_result_t bump_result = {
      .inserted = false,
      .removed = false,
      .removed_id = 0
    };
    if (_suitors.size() == _capacity) {
      suitor_t lowest_weight_suitor = _suitors[0];
      if (proposed_suitor.weight > lowest_weight_suitor.weight) {
        bump_result.removed = true;
        bump_result.removed_id = lowest_weight_suitor.suitor_id;
        pop_heap(_suitors.begin(), _suitors.end(), suitors_comparator);
        _suitors.pop_back();
      }
    }
    if (_suitors.size() < _capacity) {
      _suitors.push_back(proposed_suitor);
      push_heap(_suitors.begin(), _suitors.end(), suitors_comparator);
      bump_result.inserted = true;
    }
    return bump_result;
  }
  void add(suitor_t suitor) {
    assert(_suitors.size() < _capacity);
    _suitors.push_back(suitor);
    push_heap(_suitors.begin(), _suitors.end(), suitors_comparator);
  }
  void remove(vertex_id_t v) {
    int i = 0;
    bool should_erase = false;
    for (i = 0; i < _suitors.size(); i++) {
      if (_suitors[i].suitor_id == v) {
        should_erase = true;
        break;
      }
    }
    if (should_erase) {
      if (DEBUG)
        cout << "REM SUCCESS" << endl;
      _suitors.erase(_suitors.begin() + i);
      make_heap(_suitors.begin(), _suitors.end(), suitors_comparator);
    }
  }
  size_t size() {
    return _suitors.size();
  }
};

struct candidate_match_t {
  vertex_id_t source_id;
  vertex_id_t suitor_id;
  weight_t weight;
};

struct match_update_t {
  vertex_id_t source_id;
  vertex_id_t suitor_id;
  weight_t weight;
  bool remove;
};

bool candidate_match_comparator(const candidate_match_t &a, const candidate_match_t &b) {
  return a.source_id > b.source_id;
}

bool match_update_comparator(const match_update_t &a, const match_update_t &b) {
  if (a.source_id == b.source_id) {
    if (a.suitor_id == b.suitor_id)
      return a.remove < b.remove;
    return a.suitor_id > b.suitor_id;
  }
  return a.source_id > b.source_id;
}

int extract_column(int64_t x) {
  return (-x) & 0xFFFFull;
}

float extract_weight(int64_t x) {
  int64_t y = (-x) >> 32;
  return (*reinterpret_cast<float*>(&y));
}

void generate_candidate_matches(vector<int64_t>* adjacency_lists, size_t b, set<vertex_id_t> &queue, vector<Suitors> &suitors, vector<candidate_match_t> &candidate_matches) {
  vector<vertex_id_t> queue_vector;
  queue_vector.insert(queue_vector.end(), queue.begin(), queue.end());

  #pragma omp parallel
  {
    vector<candidate_match_t> local_candidate_matches;

    #pragma omp for
    for (size_t i = 0; i < queue_vector.size(); i++) {
      vertex_id_t v = queue_vector[i];
      size_t target_count = b - suitors[v].size();
      size_t proposed_count = 0;

      // propose the best b matches for v
      for (vector<int64_t>::iterator iterator = adjacency_lists[v].begin(); iterator < adjacency_lists[v].end() && proposed_count < target_count; iterator++) {
        vertex_id_t u = extract_column(*iterator);
        float weight = extract_weight(*iterator);
        
        if (!suitors[u].is_suitor(v)) {
          candidate_match_t candidate_match = {
            .source_id = u,
            .suitor_id = v,
            .weight = weight
          };

          if (DEBUG)
            cout << "PROP " << u << " " << v << endl;

          local_candidate_matches.push_back(candidate_match);
          proposed_count++;
        }
      }
    }

    #pragma omp critical
    {
      candidate_matches.insert(candidate_matches.end(), local_candidate_matches.begin(), local_candidate_matches.end());
    }
  }
}

void generate_match_updates(sparseMatrix &matrix, set<vertex_id_t> &queue, vector<Suitors> &suitors, vector<candidate_match_t> &candidate_matches, vector<match_update_t> &match_updates) {
  vector<int> starting_points;
  starting_points.push_back(0);
  for (size_t i = 1; i < candidate_matches.size(); i++) {
    if (candidate_matches[i].source_id != candidate_matches[i-1].source_id)
      starting_points.push_back(i);
  }

  #pragma omp parallel
  {
    vector<match_update_t> local_match_updates;
    std::set<vertex_id_t> local_queue;

    #pragma omp for
    for (size_t i = 0; i < starting_points.size(); i++) {
      int starting_point = starting_points[i];
      for (size_t j = starting_point; candidate_matches[starting_point].source_id == candidate_matches[j].source_id && j < candidate_matches.size(); j++) {
        candidate_match_t candidate_match = candidate_matches[j];

        suitor_t proposed_suitor = {
          .suitor_id = candidate_match.suitor_id,
          .weight = candidate_match.weight
        };
        bump_result_t bump_result = suitors[candidate_match.source_id].attempt_bump(proposed_suitor);
        if (bump_result.inserted) {
          if (DEBUG) {
            if (bump_result.removed)
              cout << "BUMP " << candidate_match.source_id << " " << candidate_match.suitor_id << " " << bump_result.removed_id << endl;
            else
              cout << "ADD  " << candidate_match.source_id << " " << candidate_match.suitor_id << endl;
          }

          match_update_t insert_update = {
            .source_id = candidate_match.suitor_id,
            .suitor_id = candidate_match.source_id,
            .weight = candidate_match.weight,
            .remove = false
          };
          local_match_updates.push_back(insert_update);
        }
        if (bump_result.removed) {
          match_update_t remove_update = {
            .source_id = bump_result.removed_id,
            .suitor_id = candidate_match.source_id,
            .weight = 0,
            .remove = true
          };
          local_match_updates.push_back(remove_update);
          local_queue.insert(bump_result.removed_id);
        }

      }
    }
    #pragma omp critical
    {
      match_updates.insert(match_updates.end(), local_match_updates.begin(), local_match_updates.end());
      queue.insert(local_queue.begin(), local_queue.end());
    }
  }
}

void process_match_updates(sparseMatrix &matrix, vector<Suitors> &suitors, vector<match_update_t> &match_updates) {
  vector<int> starting_points;
  starting_points.push_back(0);
  for (size_t i = 1; i < match_updates.size(); i++) {
    if (match_updates[i].source_id != match_updates[i-1].source_id)
      starting_points.push_back(i);
  }

  #pragma omp parallel for
  for (size_t i = 0; i < starting_points.size(); i++) {
    int starting_point = starting_points[i];
    for (size_t j = starting_point; match_updates[starting_point].source_id == match_updates[j].source_id && j < match_updates.size(); j++) {
      match_update_t match_update = match_updates[j];
      if (match_update.remove) {
        if (DEBUG)
          cout << "REMB " << match_update.source_id << " " << match_update.suitor_id << endl;
        suitors[match_update.source_id].remove(match_update.suitor_id);
      } else {
        if (!suitors[match_update.source_id].is_suitor(match_update.suitor_id)) {
          suitor_t added_suitor = {
            .suitor_id = match_update.suitor_id,
            .weight = match_update.weight
          };
          if (DEBUG)
            cout << "ADDB " << match_update.source_id << " " << match_update.suitor_id << endl;
          suitors[match_update.source_id].add(added_suitor);
        }
      }
    }
  }
}

int64_t build_adj(sparseEdge &edge) {
  int64_t weight_bits = *reinterpret_cast<int64_t*>(&edge.weight);
  return -((weight_bits << 32) | ((int64_t) edge.column));
}

double lock_free_matching(sparseMatrix &matrix, size_t b) {
  set<vertex_id_t> queue;
  vector<Suitors> suitors (matrix.numRows, Suitors(b));
  vector<candidate_match_t> candidate_matches;
  candidate_matches.reserve(matrix.numRows * b);
  vector<match_update_t> match_updates;
  match_updates.reserve(matrix.numRows * b);

  chrono::microseconds match_gen(0);
  chrono::microseconds match_sort(0);
  chrono::microseconds update_gen(0);
  chrono::microseconds update_sort(0);
  chrono::microseconds update_exc(0);

  const int edge_start_i = matrix.numRows / 2;
  for (vertex_id_t i = edge_start_i; i < matrix.numRows; i++)
    queue.insert(i);

  // requires non-negative edge weights
  vector<int64_t> adjacency_list[matrix.numRows];

  auto start = high_resolution_clock::now();

  for (size_t i = 0; i < matrix.numEdges; i++) {
    sparseEdge edge = matrix.edges[i];
    int64_t adj = build_adj(edge);
    adjacency_list[edge.row].push_back(adj);
  }

  auto stop = high_resolution_clock::now();
  auto edge_build = duration_cast<microseconds>(stop - start);

  start = high_resolution_clock::now();
  auto start_edge_sort = start;

  #pragma omp parallel for
  for (size_t i = edge_start_i; i < matrix.numRows; i++) {
    partial_sort(adjacency_list[i].begin(), adjacency_list[i].begin()+b, adjacency_list[i].end());
  }

  stop = high_resolution_clock::now();
  auto edge_sort = duration_cast<microseconds>(stop - start);

  // while the queue isn't empty
  while (queue.size() != 0) {
    start = high_resolution_clock::now();
    generate_candidate_matches(adjacency_list, b, queue, suitors, candidate_matches);
    stop = high_resolution_clock::now();
    match_gen += duration_cast<microseconds>(stop-start);

    queue.clear();

    start = high_resolution_clock::now();
    if (candidate_matches.size() > 0)
      ips4o::parallel::sort(&candidate_matches.front(), &candidate_matches.back(), candidate_match_comparator);
    stop = high_resolution_clock::now();
    match_sort += duration_cast<microseconds>(stop-start);

    start = high_resolution_clock::now();
    generate_match_updates(matrix, queue, suitors, candidate_matches, match_updates);
    stop = high_resolution_clock::now();
    update_gen += duration_cast<microseconds>(stop-start);

    candidate_matches.clear();

    start = high_resolution_clock::now();
    if (match_updates.size() > 0)
      ips4o::parallel::sort(&match_updates.front(), &match_updates.back(), match_update_comparator);
    stop = high_resolution_clock::now();
    update_sort += duration_cast<microseconds>(stop-start);

    start = high_resolution_clock::now();
    process_match_updates(matrix, suitors, match_updates);
    stop = high_resolution_clock::now();
    update_exc += duration_cast<microseconds>(stop-start);

    match_updates.clear();

    if (DEBUG) {
      for (int i = 0; i < matrix.numRows; i++) {
        if(suitors[i].size() > b) {
          cout << "FAILED " << i << endl;
          for (suitor_t j : suitors[i]._suitors) {
            cout << j.suitor_id << " ";
          }
          assert(false);
        }
      }
      cout << "Iteration complete" << endl;
    }
  }

  stop = high_resolution_clock::now();
  auto sort_to_verify = duration_cast<microseconds>(stop-start_edge_sort);

  cout << edge_build.count() << "," << edge_sort.count() << "," << match_gen.count() << "," << match_sort.count() << "," << update_gen.count() << "," << update_sort.count() << "," << update_exc.count() << "," << sort_to_verify.count() << endl;

  double weight = 0;
  for (int i = 0; i < matrix.numRows; i++) {
    Suitors iSuitors = suitors[i];
    for (suitor_t j : iSuitors._suitors) {
      if (DEBUG)
        assert(suitors[j.suitor_id].is_suitor(i));
      weight += j.weight;
    }
  }

  return weight;
}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  sparseMatrix *matrix = read_symmetric_sparse_matrix_file(argv[1]);
  auto start = high_resolution_clock::now();
  double totalWeight = lock_free_matching(*matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

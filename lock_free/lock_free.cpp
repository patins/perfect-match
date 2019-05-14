#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cassert>
#include "../utils/mtxReader.h"
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
    //assert(_suitors.size() < _capacity);
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

bool weight_comparator(sparseEdge &a, sparseEdge &b) {
  return a.weight > b.weight;
}

bool candidate_match_comparator(candidate_match_t &a, candidate_match_t &b) {
  return a.source_id > b.source_id;
}

bool match_update_comparator(match_update_t &a, match_update_t &b) {
  if (a.source_id == b.source_id) {
    if (a.suitor_id == b.suitor_id)
      return a.remove < b.remove;
    return a.suitor_id > b.suitor_id;
  }
  return a.source_id > b.source_id;
}

double lock_free_matching(sparseMatrix &matrix, size_t b) {
  vector<vertex_id_t> queue;
  
  vector<Suitors> suitors(matrix.numRows, Suitors(b));

  for (vertex_id_t i = 0; i < matrix.numRows; i++)
    queue.push_back(i);

  sparseEdge *edges_by_weight = new sparseEdge[matrix.numEdges];
  copy(matrix.edges, matrix.edges + matrix.numEdges, edges_by_weight);
  sort(edges_by_weight, edges_by_weight + matrix.numEdges, weight_comparator);
  // put the highest weight edges at the front

  // while the queue isn't empty
  while (queue.size() != 0) {
    vector<candidate_match_t> candidate_matches;

    for (vertex_id_t v : queue) {
      size_t target_count = b - suitors[v].size();
      size_t proposed_count = 0;

      for (size_t i = 0; i < matrix.numEdges && proposed_count < target_count; i++) {
        sparseEdge edge = edges_by_weight[i];
        vertex_id_t u;
        if (edge.row == v)
          u = edge.column;
        else if (edge.column == v)
          u = edge.row;
        else
          continue;

        if (!suitors[u].is_suitor(v)) {
          candidate_match_t candidate_match = {
            .source_id = u,
            .suitor_id = v,
            .weight = edge.weight
          };

          candidate_matches.push_back(candidate_match);
          proposed_count++;
        }
      }
    }

    queue.clear();

    sort(candidate_matches.begin(), candidate_matches.end(), candidate_match_comparator);

    vector<match_update_t> match_updates;

    for (candidate_match_t candidate_match : candidate_matches) {
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
        match_updates.push_back(insert_update);
      }
      if (bump_result.removed) {
        match_update_t remove_update = {
          .source_id = bump_result.removed_id,
          .suitor_id = candidate_match.source_id,
          .weight = 0,
          .remove = true
        };
        match_updates.push_back(remove_update);
        if (queue.size() > 0 && queue.back() != bump_result.removed_id)
          queue.push_back(bump_result.removed_id);
      }
    }

    sort(match_updates.begin(), match_updates.end(), match_update_comparator);

    if (DEBUG)
      cout << "UPDATING STEP" << endl;

    for (match_update_t match_update : match_updates) {
      if (match_update.remove) {
        if (DEBUG)
          cout << "REMB " << match_update.source_id << " " << match_update.suitor_id << endl;
        suitors[match_update.source_id].remove(match_update.suitor_id);
      } else {
        suitor_t added_suitor = {
          .suitor_id = match_update.suitor_id,
          .weight = match_update.weight
        };
        if (DEBUG)
          cout << "ADDB " << match_update.source_id << " " << match_update.suitor_id << endl;
        suitors[match_update.source_id].add(added_suitor);
      }
    }

    for (int i = 0; i < matrix.numRows; i++) {
      //assert(suitors[i].size() <= b);
    }
    if (DEBUG)
      cout << "Iteration complete" << endl;
  }

  double weight = 0;
  for (int i = 0; i < matrix.numRows; i++) {
    Suitors iSuitors = suitors[i];
    for (suitor_t j : iSuitors._suitors) {
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

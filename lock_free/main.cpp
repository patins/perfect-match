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

using namespace std;
using namespace std::chrono;

class compareWeight
{
public:
    bool operator()(pair<int,int> n1,pair<int,int> n2) {
        return n1.second<n2.second;
    }
};

int max_element_index(double *arr, int size) {
  int max_index = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] > arr[max_index]) {
      max_index = i;
    }
  }
  return max_index;
}

void get_largest_elements_indices_row(denseMatrix* matrix, int row, int x, int* indices){
  /*
  Method that returns an array of indices of the x largest elements of a given row of a matrix.

  Parameters:
    matrix - a dense matrix
    row - the row number of interest
    x - the number of smallest elements of the array

  Returns:
    indices - a pointer to an array of indices at which the smallest elements of the array occur.

  */

  //We find the largest element of the array x times, each time zeroing out the maximum
  //element out and saving the value in a temp array so we can find the x-largest elements.
  double arr[matrix->numColumns];
  for(int j=0; j < matrix->numColumns; j++){
      arr[j] = matrix->data[row * matrix->numColumns + j];
  }
  for(int i=0; i< x; i++){
    indices[i] = max_element_index(arr, matrix->numColumns);//max_element(arr, arr + matrix-> numColumns) - arr;
    arr[i] = 0;
  }
}

void  get_largest_elements_indices_col(denseMatrix* matrix, int col, list<int> &candidates, int * indices){
  /*
  Method that returns an array of vertices (from a list of candidate vertices) that correspond to the weights of the edges from vertex i to vertex col in sorted order.

  Parameters:
    matrix - a dense matrix of weights
    col- the col number of interest
    candidateVertices- a list of vertices

  Returns:
    indices - a pointer to an array of indices at which the smallest elements of the array occur.

  */

  //We find the maximum element of the array repeatedly, each time zeroing out the maximum 
  //element out and saving the value in a temp array so we can find the x-largest elements.
  //list<int> candidates = *candidateVertices;
  int size = (int) candidates.size();
  double arr[size];
  int temp[size];

  int count = 0;
  for(int i: candidates){
    arr[count] = matrix->data[i * matrix-> numColumns + col]; //add candidate edge weights to an array
    temp[count] = i; //keep track of corresponding vertices
    count +=1;
  } 

  for(int i=0; i< size; i++){
    //int arr_max_index = max_element(arr, arr + size) - arr; //maximum weight index in array
    int arr_max_index = max_element_index(arr, size);
    indices[i] = temp[arr_max_index]; //find corresponding vertex
    arr[arr_max_index] = 0; //zero  out weight
  }

}


double lock_free_matching(denseMatrix* matrix, int B) {
  /*
  algorithm for finding a bipartite B-matching that maximizes 
  the weight of the matching. each vertex is matched with at most B 
  other vertices
  */
  //priority queue of vertices that do not have at least B matches yet
  vector<int> vertices_to_match;
  make_heap(vertices_to_match.begin(), vertices_to_match.end());

  //array of pointers to the heap of matches for each vertex
  priority_queue<pair<int,double>,vector<pair<int,double> >,compareWeight> vertex_heap_pointers[matrix-> numRows];

  //array of pointers to store B-largest edges sorted by weight for each vertex
  int* vertex_sorted_edges[matrix-> numRows];
  list<int> second_vertex_sorted_edges[matrix->numRows];

  //there's definitely a more efficient way to add the numbers 0
  //through numRows to a queue and make these heaps 
  for(int i = 0; i< matrix -> numRows; i++){
    vertices_to_match.push_back(i);
    push_heap(vertices_to_match.begin(), vertices_to_match.end());
    vertex_sorted_edges[i] = (int*) malloc(B * sizeof(int));
  }

  double match_weight = 0;
  while (!vertices_to_match.empty()) {
    //generate candidate matches for first vertex
    //#pragma omp parallel for
    for (int i: vertices_to_match) {
      int num_candidates = B - vertex_heap_pointers[i].size();
      int* candidate_vertices = (int*) malloc(num_candidates * sizeof(int)); 
      get_largest_elements_indices_row(matrix, i, num_candidates, candidate_vertices);
      vertex_sorted_edges[i] = candidate_vertices;
      for (int j=0; j < num_candidates; j++) {
        second_vertex_sorted_edges[candidate_vertices[j]].push_back(i);
      }
    }

    int** second_candidate_vertices = (int**) malloc(matrix->numColumns * sizeof(int*));

    //sort candidate matches by second vertex
    //#pragma omp parallel for
    for (int j=0; j < matrix->numColumns; j++) {
      int size = (second_vertex_sorted_edges[j]).size();
      second_candidate_vertices[j] = (int *) malloc(sizeof(int) * size);
      get_largest_elements_indices_col(matrix, j, second_vertex_sorted_edges[j], second_candidate_vertices[j]);
    }

    // can this be parallelized?  #pragma omp parallel for

    vector<int> new_vertices_to_match;
    make_heap(new_vertices_to_match.begin(), new_vertices_to_match.end());

    //update first and second vertex heaps
    for (int j=0; j< matrix-> numColumns; j++) {
      for (int i=0; i < second_vertex_sorted_edges[j].size(); i++) {
        int k = second_candidate_vertices[j][i]; 
        int index = k * matrix-> numColumns + j;
        if (vertex_heap_pointers[j].empty()) {
          vertex_heap_pointers[k].push(make_pair(i, matrix->data[index]));
          vertex_heap_pointers[k].push(make_pair(j, matrix->data[index]));
          match_weight += matrix->data[index];
        } else if(matrix->data[index] < vertex_heap_pointers[j].top().second) {
          pair<int, double> removed = vertex_heap_pointers[j].top();
          vertex_heap_pointers[j].pop();
          vertex_heap_pointers[j].push(make_pair(k, matrix->data[index]));
          new_vertices_to_match.push_back(removed.first);
          push_heap(new_vertices_to_match.begin(), new_vertices_to_match.end());
          vertex_heap_pointers[k].push(make_pair(j, matrix->data[index]));
          match_weight += matrix->data[index] - removed.second;
        }
      }
    }

    vertices_to_match = new_vertices_to_match;

    for (int i = 0; i < matrix->numRows; i++) {
      second_vertex_sorted_edges[i].clear();
    }
  }

  for (int i = 0; i < matrix->numRows; i++) {
    /*for (pair<int, double> p : vertex_heap_pointers[i]) {

    }*/
  }

  return match_weight;
}


int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();
  denseMatrix matrix = read_symmetric_dense_matrix_file(argv[1]);
  double totalWeight = lock_free_matching(&matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

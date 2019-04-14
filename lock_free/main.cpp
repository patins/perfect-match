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

using namespace std;
using namespace std::chrono;

class compareWeight
{
public:
    bool operator()(pair<int,int> n1,pair<int,int> n2) {
        return n1.second<n2.second;
    }
};

int* get_largest_elements_indices_row(denseMatrix* matrix, int row, int x){
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
  int indices[x];
  int temp[x];
  for(int i=0; i< x; i++){
    indices[i] = max_element(matrix->data[row].begin(), matrix->data[row].end()) - matrix->data[row].begin();
    temp[i] = matrix[row][indices[i]];
    matrix->data[row][indices[i]] = 0;
  }
 
  //We restore the original values of the array.
  for(int i=0; i<x; i++){
    arr[indices[i]] = temp[i];
  }

  free(temp);
  return &indices;
}

int* get_largest_elements_indices_col(denseMatrix* matrix, int col, list<int>* candidateVertices){
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
  int indices[x];
  double arr[x];
  int temp[x];

  count = 0
  for(int i: candidateEdges){
    arr[count] = matrix->data[i][col]; //add candidate edge weights to an array
    temp[count] = i; //keep track of corresponding vertices
    count +=1;
  } 

  for(int i=0; i< candidateVertices.size(); i++){
    arr_max_index = max_element(arr.begin(), arr.end()) - arr.begin(); //maximum weight index in array
    indices[i] = temp[arr_max_index]; //find corresponding vertex
    arr[arr_max_index] = 0; //zero  out weight
  }

  free(arr); 
  return &indices;
}

double lock_free_matching(denseMatrix* matrix, int B) {
  /*
  algorithm for finding a bipartite B-matching that maximizes 
  the weight of the matching. each vertex is matched with at most B 
  other vertices
  */
  //priority queue of vertices that do not have at least B matches yet
  priority_queue<int> vertices_to_match;

  //array of pointers to the heap of matches for each vertex
  priority_queue<pair<int,int>,vector<pair<int,int>>,CompareWeight>* vertex_heap_pointers[matrix-> numRows];
  
  //array of pointers to store B-largest edges sorted by weight for each vertex
  int* vertex_sorted_edges[matrix-> numRows];
  list<int>* second_vertex_sorted_edges[matrix->numCols];

  //there's definitely a more efficient way to add the numbers 0
  //through numRows to a queue and make these heaps 
  for(int i = 0; i< matrix -> numRows; i++){
    vertices_to_match.push(i);
    vertex_sorted_edges[i] = (int*) malloc(B * sizeof(int));
  }

  match_weight = 0;
  while(!vertices_to_match.empty()){

  //generate candidate matches for first vertex
   #pragma omp parallel for
   for(int i: vertices_to_match){
     int* candidate_vertices = get_largest_elements_indices_row(matrix, i,  B - &vertex_heap_pointers[i].size());
     vertex_sorted_edges[i] = indices;
     for(int j: candidate_vertices){
       second_vertex_sorted_edges.push_back(i);
     }
   }

 //sort candidate matches by second vertex
   #pragma omp parallel for
   for(int j=0; j< matrix-> numCols; j++){
     int* candidate_vertices_second = get_largest_elements_indices_col(matrix, j, second_vertex_sorted_edges[j]);
   }
   
  // can this be parallelized?  #pragma omp parallel for
  
  priority_queue<int> new_vertices_to_match;

  //update first and second vertex heaps
   for(int j=0; j< matrix-> numCols; j++){
     for(int i: second_candidate_indices){
       if(&vertex_heap_pointers[j].empty()){
         &vertex_heap_pointers[j].push(make_pair(i, matrix->data[i][j]));
         &vertex_heap_pointers[i].push(make_pair(j, matrix->data[i][j]));
         match_weight += matrix -> data[i][j];
       }
       else if(matrix->data[i][j] < &vertex_heap_pointers[j].front()){
         removed =  &vertex_heap_pointers[j].pop();
         &vertex_heap_pointers[j].push(make_pair(i, matrix->data[i][j]));
         new_vertices_to_match.push(removed[0]);
         &vertex_heap_pointers[i].push(make_pair(j, matrix->data[i][j]));
         match_weight += matrix->data[i][j] - removed[1];
       }
     }
   }

    vertices_to_match = new_vertices_to_match;
  }

    return match_weight;

}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();
  denseMatrix matrix = read_symmetric_sparse_matrix_file(argv[1]);
  double totalWeight = lock_free_matching(&matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

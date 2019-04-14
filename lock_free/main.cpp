#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "../utils/mtxReader.h"
#include <queue>
using namespace std;
using namespace std::chrono;

class compareWeight
{
public:
    bool operator()(pair<int,int> n1,pair<int,int> n2) {
        return n1.second>n2.second;
    }
};


double lock_free_matching(denseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  int* numMatches = new int[matrix->numRows];
  int** matches = (int**) malloc(matrix->numRows * sizeof(int*));

  //priority of queue of vertices that do not have at least B matches yet
  priority_queue<int> vertices_to_match;

  //array of pointers to the heap of matches for each vertex
  priority_queue<pair<int,int>,vector<pair<int,int>>,CompareWeight>* vertex_heap_pointers[matrix-> numRows];
  
  //there's definitely a more efficient way to add the numbers 0
  //through numRows to a queue and make these heaps 
  for(int i = 0; i< matrix -> numRows; i++){
    vertices_to_match.push(i);
  }
 
  while(!vertices_to_match.empty()){
   #pragma omp parallel for
   for(int i= 0; i< matrix-> numRows; i++){
    
   }

  } 
  for(int i = 0; i < matrix->numRows ; i++){
    numMatches[i] = 0;
    matches[i] = (int*) malloc(B * sizeof(int));
  }
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){
    int r = matrix->edges[i].row;
    int c = matrix->edges[i].column;
    if(c != r && numMatches[r]<B && numMatches[c]<B){
         matches[r][numMatches[r]] = c;
         matches[c][numMatches[c]] = r;
         numMatches[r]++ ;
         numMatches[c]++ ;
         totalWeight += matrix->edges[i].weight;
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
  denseMatrix matrix = read_symmetric_sparse_matrix_file(argv[1]);
  double totalWeight = lock_free_matching(&matrix, atoi(argv[2]));
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

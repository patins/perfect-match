#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <queue>
#include "../utils/mtxReader.h"

using namespace std;
using namespace std::chrono;



struct compareEdges_queue {
    bool operator()(sparseEdge const& e1, sparseEdge const& e2)
    {
        return e1.weight < e2.weight;
    }
};


typedef struct matching {
  priority_queue<sparseEdge, vector<sparseEdge>, compareEdges_queue>* incoming_matches;
  int** outgoing_matches;
  int* numMatches;
  int numNodes;
} matching;


 double greedy_queue(char* filename, int B){
   std::ifstream fin(filename);
   int M, N, L;
   while (fin.peek() == '%') fin.ignore(2048, '\n');
   fin >> M >> N >> L;

   priority_queue<sparseEdge, vector<sparseEdge>, compareEdges_queue> Q;

   for (int l = 0; l < L; l++)
   {
   	int m, n;
   	double data;
     fin >> m >> n >> data;
   	sparseEdge edge;
     edge.weight = data;
     edge.row = m-1;
     edge.column = n-1;
     Q.push(edge);
   }
   fin.close();

   int* numMatches = new int[M];
   int** matches = new int*[M];
   for(int i = 0; i < M ; i++){
     numMatches[i] = 0;
     matches[i] = new int[B];
   }
   double totalWeight = 0;
   while(!Q.empty()){
     sparseEdge iter_edge = Q.top();
     Q.pop();
     int r = iter_edge.row;
     int c = iter_edge.column;
     if(c != r && numMatches[r]<B && numMatches[c]<B){
          matches[r][numMatches[r]] = c;
          matches[c][numMatches[c]] = r;
          numMatches[r]++ ;
          numMatches[c]++ ;
          totalWeight += iter_edge.weight;
        }
   }
   return totalWeight;
 }


double greedy(sparseMatrix* matrix, int B) {
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  int* numMatches = new int[matrix->numRows];
  int** matches = (int**) malloc(matrix->numRows * sizeof(int*));
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

double greedy_updateable(sparseMatrix* matrix, int B, matching* M) {
  M->incoming_matches = new priority_queue<
    sparseEdge,
    vector<sparseEdge>,
    compareEdges_queue
  >[matrix->numRows];
  sort(matrix->edges, matrix->edges + matrix->numEdges, compareEdges);
  int* numMatches = new int[matrix->numRows];
  int** matches = new int*[matrix->numRows];
  for(int i = 0; i < matrix->numRows ; i++){
    numMatches[i] = 0;
    matches[i] = new int[matrix->numRows];
  }
  double totalWeight = 0;
  for(int i = 0; i < matrix->numEdges; i++){
    int r = matrix->edges[i].row;
    int c = matrix->edges[i].column;
    if(c != r && numMatches[r]<B && numMatches[c]<B){
         matches[r][c] = 1;
         matches[c][r] = 1;
         numMatches[r]++ ;
         numMatches[c]++ ;
         totalWeight += matrix->edges[i].weight;
         M->incoming_matches[r].push(matrix->edges[i]);
         M->incoming_matches[c].push(matrix->edges[i]);
       }
  }
  M->outgoing_matches = matches;
  M->numNodes = matrix->numRows;

  return totalWeight;
}

double edge_updateable(sparseEdge e, matching* M, int B){

}

int main(int argc, char **argv) {
  if (argc == 2) {
    cout << "Input file and number of matches not specified.";
    return 0;
  }
  auto start = high_resolution_clock::now();

  // Algorithm starts here:
  sparseMatrix* matrix = read_symmetric_sparse_matrix_file(argv[1]);
  matching M;
  double totalWeight = greedy_updateable(matrix, atoi(argv[2]), &M);
  //double totalWeight = greedy(matrix, atoi(argv[2]));
  //double totalWeight = greedy_queue(argv[1], atoi(argv[2]));

  // End algorithm
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: "
    << duration.count() << " microseconds" << endl;
  cout << "Weight " << totalWeight << endl;
  return 1;
}

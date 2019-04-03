#ifndef BMATCHING_H
#define BMATCHING_H

#include <omp.h>
#include <cmath>
#include <iostream>
#include "mtxReader.h"
//#include <immintrin.h>
#include <getopt.h>
#include "mic_utils.h"
using namespace std;

#define CHUNK 256
#define DYN
#define BSIZE 256
#define LBUF

extern int pivotID;
extern float pivotW;
extern int ptype;

struct Param
{
    int cstart;
    int cend;
    int ostart;
    int oend;
};

struct Info
{
    int id;
    float weight;
};

class Node 
{
    public:
    int maxSize;
    int curSize;
    Info* heap;
    Info minEntry;

    void print();
    __forceinline int min_id()
    {
        return (curSize>0 && curSize==maxSize)?heap[0].id:-1;
    }

    __forceinline  float min_weight()
    {
        return (curSize>0 && curSize==maxSize)?heap[0].weight:0.0;
    }

    __forceinline int find_id(int idx)
    {
        for(int i=0;i<curSize;i++)
            if(heap[i].id==idx)
                return 1;
        return 0;
    }

    void Add(float wt, int id);
    void AddHeap(float wt, int id);
    
};

struct Walker
{
    int midx1;
    int midx2;
    float W1;
    float W2;
    float* M1;
    float* M2;
};
//Edge gEdge;

void bSuitorBPQD(CSR* G, int *b, int *nlocks, Node* S, 
        int* start, int* end, char* mark, int type, int stepM, bool verbose);
void bSuitor(CSR* G, int* b, Node* S, int algo, bool verbose);

int custom_sort(Edge* verInd, int start, int end, int part, int* order, int type);
int custom_sort_optimized(Edge* verInd, int start, int end, int part, int* order, 
        int type, Edge* neighbors);
int pivoting(Edge* verInd, int start, int end, int id, float weight, int* pindx, int step);

bool comparator(Edge left, Edge right);
bool predicate(Edge left);
bool verifyMatching(CSR* g, Node* S, int n);
void printMatching(Node* S, int n);


struct bmatching_parameters
{
    char* problemname;
    char* bFileName;
    int b;
    int algorithm;
    bool verbose;
    bool bipartite;

    bmatching_parameters();
    void usage();
    bool parse(int argc, char** argv);

};

#endif //BMATCHING_H

/****************************************************************************
*                                                                           *
* b-Suitor computes 1/2-approximation solution of a b-Matching on a graph.  *
* Copyright (C) 2017  Arif Khan, Purdue University                          *
*                                                                           *
* This file is part of b-Suitor.                                            *
*                                                                           *
* b-Suitor is free software: you can redistribute it and/or modify          *    
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation, either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* b-Suitor is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
*                                                                           *
*****************************************************************************/

#include "bMatching.h"
#include "mtxReader.h"
#include "pcl_stack.h"
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <float.h>

using namespace std;

bmatching_parameters::bmatching_parameters():problemname(NULL),bFileName(NULL),b(5),verbose(false),
    bipartite(false)
{}

void bmatching_parameters::usage()
{
    const char *params =
	"\n\n"
    "Usage: %s -f <problemname> -e <bfilename> -b <bval> -a <algorithm> -v -g\n\n"
	"	-f problemname  : file containing graph. Currently inputs .cbin files\n"
	"	-e bfilename    : (Optional input) A file containing integer b values in each line \n"
	"	-b bval         : constant b value if b=0 then randomly generated.\n"
	"	-a algorithm    : Algorithm 0:unsorted 1:partial sorted mode (default)\n"
    "   -t              : bipartite graph \n";
    "   -v              : verbose \n\n";
    fprintf(stderr, params);
}

bool bmatching_parameters::parse(int argc, char** argv)
{
    static struct option long_options[]=
    {
        // These options don't take extra arguments
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"bipartite", no_argument, NULL, 'g'},
        
        // These do
        {"problem", required_argument, NULL, 'f'},
        {"bVar", required_argument, NULL, 'e'},
        {"b", required_argument, NULL, 'b'},
        {"a", required_argument, NULL, 'a'},
        {NULL, no_argument, NULL, 0}
    };

    static const char *opt_string="vhtf:e:b:a:";
    int opt, longindex;
    opt=getopt_long(argc,argv,opt_string,long_options,&longindex);
    while(opt != -1)
    {
        switch(opt)
        {
            case 'v': verbose=true; 
                      break;
            
            case 't': bipartite=true; 
                      break;

            case 'h': usage(); 
                      return false; 
                      break;

            case 'f': problemname=optarg; 
                      if(problemname==NULL)
                      {
                        cerr<<"Problem file is not speficied"<<endl;
                        return false;  
                      }
                      break;

            case 'e': bFileName=optarg;
                      if(bFileName==NULL)
                      {
                        cerr<<"b Value file is not speficied"<<endl;
                        return false;  
                      }
                      break;
            case 'b': b=atoi(optarg);
                      if(b<0)
                      {
                        cerr<<"the b value can't be negative"<<endl;
                        return false;
                      }
                      break;
            case 'a': algorithm=atoi(optarg);
                      if(algorithm<0)
                      {
                        cerr<<"the a value can't be negative"<<endl;
                        return false;
                      }
                      break;
        }
        opt=getopt_long(argc,argv,opt_string,long_options,&longindex);
    }

    return true;
}

int main(int argc, char** argv)
{
    bmatching_parameters opts;
    if(!opts.parse(argc,argv))
    {
        cout<<"Argument Parsing Done..!!"<<endl;
        return -1;
    }
    double t1;
    int numThreads,best;
    double rt_start = omp_get_wtime();	
    
    /********* Reading the INPUT ******/
    CSR G;
    //G.readCSRbin(opts.problemname,0);
    if(opts.bipartite==false)
        G.readMtxG(opts.problemname);
    else
        G.readMtxB(opts.problemname);
    
    double rt_end = omp_get_wtime();	
    cout<<"Graph (" << G.nVer << ", " << G.nEdge/2 << ") Reading Done....!! took " 
        << rt_end - rt_start <<endl;

    
    /*********** Memory Allocation *************/
    int *b=(int*)_mm_malloc(G.nVer*sizeof(int),64);
    
    //Per vertex heap data structure that holds the final matching
    Node* S=(Node*)_mm_malloc(G.nVer*sizeof(Node),64);      //Heap data structure
   
    #pragma omp parallel
    numThreads=omp_get_num_threads();
    

    int avgb=0;
    if(opts.bFileName!=NULL)
    {    
        avgb=readBfile(opts.bFileName,b,G.nVer);
        if(avgb==-1)
        {
            cout<<"b value file reading error!"<<endl;
            return false;
        }

    }
    else
    {
        
        /************ Assigning bValues **************/
        for(int i=0;i<G.nVer;i++)
        {
            if(opts.b>0)
            {    
                int card=0;
                for(int j=G.verPtr[i];j<G.verPtr[i+1];j++)
                    if(G.verInd[j].weight>0)
                        card++;

                if(card>opts.b)
                    b[i]=opts.b;
                else b[i]=card;
            }
            else
            {
                //int deg=(int)sqrt(G.verPtr[i+1]-G.verPtr[i]);
                int deg=0;
                for(int j=G.verPtr[i];j<G.verPtr[i+1];j++)
                    if(G.verInd[j].weight>0)
                        deg++;
                deg=(int)sqrt(deg);
                if(deg==0)
                    b[i]=0;
                else
                {
                    if(deg==1)
                        b[i]=1;
                    else
                        b[i]=floor(deg);
                        //b[i]=rand()%deg+1;
                }
            }
            avgb+=b[i];
        }
        avgb=avgb/G.nVer;
    }
    
    if(opts.b>0)
        cout<<"bValue is constant for each vertex"<<endl;
    else
        cout<<"bValue is randomly generated, b_avg= "<<avgb<<endl;

    for(int i=0;i<G.nVer;i++)       
        if(b[i]>0)    
            S[i].heap=(Info*)_mm_malloc(b[i]*sizeof(Info),64);   //Each heap of size b
        else
            S[i].heap=(Info*)_mm_malloc(1*sizeof(Info),64);      //Each heap of size b

    cout << "Input Processing Done: " << omp_get_wtime() - rt_end <<endl;
    
    bSuitor(&G,b,S,opts.algorithm,opts.verbose); 
    
    
    for(int i=0;i<G.nVer;i++)
        _mm_free(S[i].heap);
    
    _mm_free(S);
    _mm_free(b);

    return 0;
}

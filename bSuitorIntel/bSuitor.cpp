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
#include <cstring>
using namespace std;

int pivotID;
float pivotW;
int ptype;

void Node::print()
{
    int i;
    printf("Printing the heap array\n");
    for(i=0;i<curSize;i++)
    {
        printf("%d %f\n",heap[i].id,heap[i].weight);
    }
}

bool verifyMatching(CSR* g, Node* S, int n)
{
    bool flag=false;
    float weight=0.0;
    //#pragma omp parallel for schedule(static)
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<S[i].curSize;j++)
        {
            int a=S[i].heap[j].id;
            weight+=S[i].heap[j].weight;
            if(a!=-1 && !S[a].find_id(i))
            {    
                cout<<"("<<i<<","<<a<<") :";
                for(int k=0;k<S[i].curSize;k++)
                    cout<<S[i].heap[k].id<<"("<<S[i].heap[k].weight<<")"<<" ";
                cout<<" I ";
                for(int k=0;k<S[a].curSize;k++)
                    cout<<S[a].heap[k].id<<"("<<S[a].heap[k].weight<<")"<<" ";
                cout<<endl;
                
                cout<<"i: ";
                for(int k=0;k<n;k++)
                    if(k!=i && S[k].find_id(i))
                    cout<<k<<" ";
                cout<<endl;
                cout<<"a: ";
                for(int k=0;k<n;k++)
                    if(k!=a && S[k].find_id(a))
                    cout<<k<<" ";
                cout<<endl;
                cout<<"i: ";
                for(int k=g->verPtr[i];k<g->verPtr[i+1];k++)
                    cout<<g->verInd[k].id<<" "<<g->verInd[k].weight<<",";
                cout<<endl;
                cout<<"a: ";
                for(int k=g->verPtr[a];k<g->verPtr[a+1];k++)
                    cout<<g->verInd[k].id<<" "<<g->verInd[k].weight<<",";
                cout<<endl;
                flag=true;
                break;
            }
        }
        if(flag)
            break;
    }

    
    
    if(flag)
        return false;
    else
    {
        cout<<"Matching Weight: "<<weight/2.0<<endl;
        return true;
    }
}

void printMatching(Node* S, int n)
{

    cout<<"---- Matching Output----"<<endl;
    for(int i=0;i<n;i++)
    {
        cout<<i<<" :";
        for(int j=0;j<S[i].curSize;j++)
            cout<<S[i].heap[j].id<<" ";
        cout<<endl;
    }
    cout<<endl;
}


__forceinline bool heapComp(Info left, Info right)
{
    return (left.weight<right.weight || (left.weight==right.weight && left.id<right.id));
}

__forceinline bool comparator(Edge left, Edge right)
{
    return (left.weight > right.weight || (left.weight==right.weight && left.id > right.id));
}


__forceinline bool comparatorE(EdgeE left, EdgeE right)
{
    return (left.weight > right.weight || (left.weight==right.weight && left.id > right.id));
}



void Node::AddHeap(float wt, int idx )
{
    if(curSize==maxSize)
    {
        if(maxSize>2)
        {
            //heap[0].weight=wt;
            //heap[0].id=idx;

            /// Only heapify one branch of the heap tree
            int small,ri,li,pi=0;
            int done=0;
            
            if(heap[2].weight >heap[1].weight || (heap[2].weight==heap[1].weight && heap[2].id>heap[1].id))
                small=1;
            else
                small=2;
                
            if(wt>heap[small].weight || (wt==heap[small].weight && idx>heap[small].id))
            {
                heap[0].weight=heap[small].weight;
                heap[0].id=heap[small].id;
                heap[small].weight=wt;
                heap[small].id=idx;
            }   
            else
            {
                heap[0].weight=wt;
                heap[0].id=idx;
            }

            pi=small;
            while(!done)
            {
                li=2*pi+1;
                ri=2*pi+2;
                small=pi;

                if(li <maxSize && (heap[li].weight< heap[small].weight || (heap[li].weight == heap[small].weight && heap[li].id < heap[small].id )))
                    small=li;
                if(ri <maxSize && (heap[ri].weight< heap[small].weight || (heap[ri].weight == heap[small].weight && heap[ri].id < heap[small].id)))
                    small=ri;

                if(small != pi)
                {
                    wt=heap[pi].weight;
                    idx=heap[pi].id;

                    heap[pi].weight=heap[small].weight;
                    heap[pi].id=heap[small].id;

                    heap[small].weight=wt;
                    heap[small].id=idx;
                }
                else done=1;

                pi=small;
            }
        }
        else
        {
            if(maxSize != 1 && (wt>heap[1].weight || (wt==heap[1].weight && idx > heap[1].id)))
            {
                heap[0].weight=heap[1].weight;
                heap[0].id=heap[1].id;
                heap[1].weight=wt;
                heap[1].id=idx;
            }
            else
            {
                heap[0].weight=wt;
                heap[0].id=idx;
            }
        }

        minEntry.id=heap[0].id;
        minEntry.weight=heap[0].weight;
    }
    else
    {
        heap[curSize].weight=wt;
        heap[curSize].id=idx;
        curSize++;
        if(curSize==maxSize)
        {    
            sort(heap,heap+curSize,heapComp);
            minEntry.weight=heap[0].weight;
            minEntry.id=heap[0].id;
        }
    }            
}


int custom_sort_optimized(Edge* verInd, int start, int end, 
        int step, int* order, int type, Edge* neighbors)
{
    int part=start+step;
    int tstart, tend,tpart,k,length;
    int id,p,r,tid;
    double weight;
    __declspec(aligned(64)) Edge temp;
    __declspec(aligned(64)) Edge median;

    switch(type)
    {
        case 1: break;
        case 2: part=end;
                sort(verInd+start,verInd+end,comparator);
                //csort(verInd,start,end);
                break;
        
        case 3: break;
        case 4: if(part>=end)
                {
                    part=end;
                    sort(verInd+start,verInd+end,comparator);
                }
                else
                {   
                    tend=end-1;
                    tstart=start;
                    k=step+1;

                    while(true)
                    {
                        p=tstart;
                        r=tend;
                        weight = verInd[(r+p)/2].weight;
                        id=verInd[(r+p)/2].id;
			median = verInd[(r+p)/2];

#if defined _ACTIVATE_MIC_CODE_ && defined __MIC__
			int num = (tend - tstart + 1);
			int num_aligned = num/16 * 16;
			SIMDFPTYPE v_weight = _MM_SET(weight);
			SIMDINTTYPE v_id = _MM_SET_INT(id);
			Edge * write_1 = verInd + tstart;
			Edge * write_2 = neighbors;
			int cnt_write_1 = 0;
			int cnt_write_2 = 0;

			for(int i = tstart; i < tstart + num_aligned; i+=16)
			{
				// AOS, hence two loads required to get 16 elements
				SIMDINTTYPE v_i = _MM_LOADU_INT((int *)(verInd + i));	
				SIMDINTTYPE v_i_16 = _MM_LOADU_INT((int *)(verInd + i + 8));

				// First element in structure is id, and second is weight.
				// Mask with 0xAAAA to get weight, and 0x5555 to get id
				SIMDMASKTYPE v_gt_weight_1 = _MM_CMP_LT_MASK(0xAAAA, v_weight, _mm512_castsi512_ps(v_i));	
				SIMDMASKTYPE v_eq_weight_1 = _MM_CMP_EQ_MASK(0xAAAA, _mm512_castsi512_ps(v_i), v_weight);
				SIMDMASKTYPE v_gt_id_1     = _MM_CMP_LT_MASK_INT(0x5555, v_id, v_i);
				SIMDMASKTYPE v_mask_left_1 = _mm512_kor(v_gt_weight_1, _mm512_kand(v_eq_weight_1, (v_gt_id_1<<1)));
				unsigned int cnt_1 = _mm_countbits_32(v_mask_left_1);
				v_mask_left_1 = ((v_mask_left_1 >> 1)) | v_mask_left_1;
				_MM_STOREU_MASK_INT((int *)(write_1), v_mask_left_1, v_i);	
				write_1 += cnt_1; cnt_write_1 += cnt_1;

				SIMDMASKTYPE v_lt_weight_1 = _MM_CMP_LT_MASK(0xAAAA, _mm512_castsi512_ps(v_i), v_weight);	
				SIMDMASKTYPE v_lt_id_1     = _MM_CMP_LT_MASK_INT(0x5555, v_i, v_id);
				SIMDMASKTYPE v_mask_right_1 = _mm512_kor(v_lt_weight_1, _mm512_kand(v_eq_weight_1, (v_lt_id_1<<1)));
				unsigned int cnt_right_1 = _mm_countbits_32(v_mask_right_1);
				v_mask_right_1 = ((v_mask_right_1 >> 1)) | v_mask_right_1;
				_MM_STOREU_MASK_INT((int *)(write_2), v_mask_right_1, v_i);	
				write_2 += cnt_right_1; cnt_write_2 += cnt_right_1;


				SIMDMASKTYPE v_gt_weight_2 = _MM_CMP_LT_MASK(0xAAAA, v_weight, _mm512_castsi512_ps(v_i_16));	
				SIMDMASKTYPE v_eq_weight_2 = _MM_CMP_EQ_MASK(0xAAAA, _mm512_castsi512_ps(v_i_16), v_weight);
				SIMDMASKTYPE v_gt_id_2     = _MM_CMP_LT_MASK_INT(0x5555, v_id, v_i_16);
				SIMDMASKTYPE v_mask_left_2 = _mm512_kor(v_gt_weight_2, _mm512_kand(v_eq_weight_2, (v_gt_id_2<<1)));
				unsigned int cnt_2 = _mm_countbits_32(v_mask_left_2);
				v_mask_left_2 = ((v_mask_left_2 >> 1)) | v_mask_left_2;
				_MM_STOREU_MASK_INT((int *)(write_1), v_mask_left_2, v_i_16);	
				write_1 += cnt_2; cnt_write_1 += cnt_2;

				SIMDMASKTYPE v_lt_weight_2 = _MM_CMP_LT_MASK(0xAAAA, _mm512_castsi512_ps(v_i_16), v_weight);	
				SIMDMASKTYPE v_lt_id_2     = _MM_CMP_LT_MASK_INT(0x5555, v_i_16, v_id);
				SIMDMASKTYPE v_mask_right_2 = _mm512_kor(v_lt_weight_2, _mm512_kand(v_eq_weight_2, (v_lt_id_2<<1)));
				unsigned int cnt_right_2 = _mm_countbits_32(v_mask_right_2);
				v_mask_right_2 = ((v_mask_right_2 >> 1)) | v_mask_right_2;
				_MM_STOREU_MASK_INT((int *)(write_2), v_mask_right_2, v_i_16);	
				write_2 += cnt_right_2; cnt_write_2 += cnt_right_2;
			}

			for(int i = tstart + num_aligned; i <= tend; i++)
			{
				if( verInd[i].weight > weight || (verInd[i].weight==weight && verInd[i].id>id))
				{
					*write_1 = verInd[i];
					write_1++; cnt_write_1++;
				}
				else if(verInd[i].weight < weight || (verInd[i].weight==weight && verInd[i].id<id))
				{
					*write_2 = verInd[i];
					write_2++; cnt_write_2++;
				}	
			}

			// add the median at the end of write_1
			*write_1 = median;
                        write_1++; cnt_write_1++;
			
 			// copy back
			int cnt_write_2_aligned = cnt_write_2/8 * 8;

			for(int i = 0; i < cnt_write_2_aligned; i+=8)
			{
				_MM_STOREU_INT((int *)(write_1), _MM_LOAD_INT((int *)(neighbors + i))); write_1 += 8;
			}
			for(int i = cnt_write_2_aligned; i < cnt_write_2; i++)
			{
				*write_1 = neighbors[i];
				write_1++;
			}
			
			r = tstart + cnt_write_1 - 1;	
#else
                        while ( p < r )
                        {
                            while ( verInd[p].weight > weight || (verInd[p].weight==weight && verInd[p].id>id))
                                p++;
                            while ( verInd[r].weight < weight || (verInd[r].weight==weight && verInd[r].id < id) )
                                r--;
                            
                            if(verInd[p].id==verInd[r].id)
                                p++;
                            else
                                if( p < r ) 
                                {
				    *(long long int *)(&temp) = *(long long int *)(verInd + p);
				    *(long long int *)(verInd + p) = *(long long int *)(verInd + r);
				    *(long long int *)(verInd + r) = *(long long int *)(&temp);
                                }
                        }
#endif
                        length=r-start+1;
                        if(length==k)
                            break;
                        else
                        {
                            if(length > k)
                                tend=r-1;
                            else
                                tstart=r+1;
                        }
                        if(tstart==tend)
                            break;

                    }

                    //nth_element(verInd+start,verInd+part,verInd+end,comparator);
                    sort(verInd+start,verInd+part,comparator);
                }
                break;

        default: sort(verInd+start,verInd+end,comparator);
    }
    return part;
}

int pivoting(Edge* verInd, int start, int end, int id, float weight, int* pindx,int step)
{
    int tstart,tend,r,p;
    Edge temp;
    
    tend=end-1;
    tstart=start;
    p=tstart;
    r=tend;
    while ( p < r )
    {
        while ( verInd[p].weight > weight || (verInd[p].weight==weight && verInd[p].id>id))
        {
            //if(p>tend)
                //break;
            p++;
        }
        while ( verInd[r].weight < weight || (verInd[r].weight==weight && verInd[r].id < id) )
        { 
            //if(r<tstart)
                //break;
            r--;
        }
        if(verInd[p].id==verInd[r].id)
            p++;
        else
            if( p < r ) 
            {
                /*temp.id= verInd[p].id;
                temp.weight=verInd[p].weight;

                verInd[p].id  = verInd[r].id;
                verInd[p].weight = verInd[r].weight;

                verInd[r].id = temp.id;
                verInd[r].weight = temp.weight;*/
                p++;
                r--;
            }
    }

    
    /*if(r<start)
        return start;
    else
    {
        if(r==tend)
        {
            sort(verInd+start,verInd+end,comparator);
            return end;
        }
        else
        {
            sort(verInd+start,verInd+r+1,comparator);
            return r+1;
        }
    }*/

    if(r<start)
        (*pindx)=start;
    else
    {
        if(r==tend)
            (*pindx)=end;
        else
            (*pindx)=r+1;
    }
    if(((*pindx)-start)>step)
        step=(*pindx)-start;
    return custom_sort(verInd,start,end,step,NULL,4);
}

int custom_sort(Edge* verInd, int start, int end, int step, int* order, int type)
{
    int part=start+step;
    int tstart, tend,tpart,k,length;
    int id,p,r,tid;
    float weight;
    Edge temp;

    switch(type)
    {
        case 1: break;
        case 2: part=end;
                sort(verInd+start,verInd+end,comparator);
                //csort(verInd,start,end);
                break;
        
        case 3: break;
        case 4: if(part>=end)
                {
                    part=end;
                    sort(verInd+start,verInd+end,comparator);
                }
                else
                {   
                    tend=end-1;
                    tstart=start;
                    k=step+1;
                    while(true)
                    {
                        p=tstart;
                        r=tend;
                        weight = verInd[r].weight;
                        id=verInd[r].id;
                        while ( p < r )
                        {
                            while ( verInd[p].weight > weight || (verInd[p].weight==weight && verInd[p].id>id))
                                p++;
                            while ( verInd[r].weight < weight || (verInd[r].weight==weight && verInd[r].id < id) )
                                r--;
                            
                            if(verInd[p].id==verInd[r].id)
                                p++;
                            else
                                if( p < r ) 
                                {
                                    temp.id= verInd[p].id;
                                    temp.weight=verInd[p].weight;

                                    verInd[p].id  = verInd[r].id;
                                    verInd[p].weight = verInd[r].weight;

                                    verInd[r].id = temp.id;
                                    verInd[r].weight = temp.weight;
                                }
                        }

                        length=r-start+1;
                        if(length==k)
                            break;
                        else
                        {
                            if(length > k)
                                tend=r-1;
                            else
                                tstart=r+1;
                        }
                        if(tstart==tend)
                            break;
                    }

                    //nth_element(verInd+start,verInd+part,verInd+end,comparator);
                    sort(verInd+start,verInd+part,comparator);
                }
                break;

        default: sort(verInd+start,verInd+end,comparator);
    }
    return part;
}

void bSuitor(CSR* G, int* b, Node* S, int algo, bool verbose)
{

    int numThreads, stepM=3,type=4;
    double t1;
    /*********** Memory Allocation *************/
    
    int* nlocks=(int*) _mm_malloc(G->nVer*sizeof(int),64);  //required for all schemes
    int* start=(int*) _mm_malloc(G->nVer*sizeof(int),64);
    int* end=(int*) _mm_malloc(G->nVer*sizeof(int),64);
    char* mark=(char*) _mm_malloc(G->nEdge*sizeof(char),64);  //required for part sorted
    
    #pragma omp parallel
    numThreads=omp_get_num_threads();
    
    if(!verbose)
        t1=omp_get_wtime();

    //type 1,2,4 = DU DS DP
    if(algo==0)
        type=1;

    bSuitorBPQD(G,b,nlocks,S,start,end,mark,type,stepM,verbose);
    
    if(!verbose)
    {
        t1=omp_get_wtime()-t1;
        cout<<"Total Matching Time: "<<t1<<endl;
    }
    else
    {
        if(verifyMatching(G,S,G->nVer))
            cout<<"General Matching Verified..!!"<<endl;
        else
            cout<<"We are Screwed..!!"<<endl;
    }

    //printMatching(S,g.nVer);
    
    _mm_free(nlocks);
    _mm_free(start);
    _mm_free(end);
    _mm_free(mark);
    
    return;

}

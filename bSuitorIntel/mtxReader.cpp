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

#include "mtxReader.h"
#include <cstring>
using namespace std;

bool CSR::readMtxB(char* filename)
{
    int count=0,i,j;
    int inp, m1, sym, edgecnt_;
    int numRow, numCol, nonZeros, numEdges;
    double f;
    string s;
    ifstream inf;

    inf.open(filename, ios::in);

    if(inf.is_open())
    {
        size_t found1, found2, found3;
        getline(inf,s);
        found1 = s.find("pattern");
        if (found1 != string::npos)
            m1 = 2;
        else
            m1 = 3;
        found1 = s.find("symmetric");
        found2 = s.find("hermitian");
        found3 = s.find("skew-symmetric");
        if (found1 != string::npos || found2 != string::npos || found3 != string::npos)
            sym = 1;
        else
            sym = 0;
        while(inf.peek()=='%')
            getline(inf,s);
        
        if(sym==0)
        {    
            cout<<endl<<"WARNING..!!"<<endl;
            cout<<"The mtx file contains full matrix"<<endl;
            cout<<"User has to make sure that input file has both (i,j) and (j,i) present."<<endl;
            cout<<"Matching is not defined if w(i,j) != w(j,i)"<<endl<<endl;
        }

 
        inf>>inp;
        numRow=inp;
        inf>>inp;
        numCol=inp;
        inf>>inp;
        nonZeros=inp;

        bipartite=true;
        lVer=numRow;
        rVer=numCol;
        nVer=lVer+rVer;


        count=inp;
        
        vector<vector<int> > graphCRSIdx(nVer);
        vector<vector<double> > graphCRSVal(nVer);
        int diag=0;
        
        while(count>0) 
        {     
            inf>>i; 
            inf>>j;

            j+=lVer; //adjusting for the right hand vertices
            
            if(m1==3) 
                inf>>f; 
            else
                f=drand48();

            if(i==j)
                diag++;
            else
            {
                graphCRSIdx[i-1].push_back(j-1); 
                graphCRSVal[i-1].push_back(f);
                if(sym)
                {
                    graphCRSIdx[j-1].push_back(i-1); 
                    graphCRSVal[j-1].push_back(f); 
                }
            }   
            count--; 
        }     
        inf.close(); 
     
        numEdges = nonZeros;  
        if(sym == 1) //symmetric matrix
            numEdges = nonZeros*2 - 2*diag;

        nEdge=numEdges;
        
        verPtr=new int[nVer+1];
        verInd=new Edge[nEdge];

        verPtr[0]=0;
        int max=0,offset; 
        for(int i=1;i<=nVer;i++)
        {
            
            offset=graphCRSIdx[i-1].size();
            verPtr[i]=verPtr[i-1]+offset;
            count=verPtr[i-1];
            //cout<<i-1<<" "<<verPtr[i-1]<<" "<<verPtr[i]<<": ";
            for(int j=0;j<offset;j++)
            {
                verInd[count].id=graphCRSIdx[i-1][j];
                verInd[count].weight=graphCRSVal[i-1][j];
                count++;

                //cout<<verInd[count-1]<<" ";
            }
            //cout<<endl;
            if(offset>max)
                max=offset;
        }
        
        assert(count==nEdge);
        maxDeg=max;

        cout<<lVer<<" "<<rVer<<" "<<nVer<<" "<<nEdge<<endl;

    }
    else return false;

    int flag=0;
    for(int i=0;i<lVer;i++)
    {    
        for(j=verPtr[i];j<verPtr[i+1];j++)
            if(verInd[j].id<lVer)
            {    
                flag=1;
                break;
            }
    
        if(flag==1)
        {            
            cout<<"Bad Graph left: "<<i<<endl;
            break;
        }
    }

    flag=0;
    for(int i=lVer;i<nVer;i++)
    {    
        for(j=verPtr[i];j<verPtr[i+1];j++)
            if(verInd[j].id>=lVer)
            {    
                flag=1;
                break;
            }
        if(flag==1)
        {
            cout<<"Bad Graph Right: "<<i<<endl;
            break;
        }
    }
   
   return true;
}

bool CSR::readMtxG(char* filename)
{
    int count=0,i,j;
    int inp, m1, sym, edgecnt_;
    int numRow, numCol, nonZeros, numEdges;
    double f;
    string s;
    ifstream inf;

    inf.open(filename, ios::in);

    if(inf.is_open())
    {
        size_t found1, found2, found3;
        getline(inf,s);
        found1 = s.find("pattern");
        if (found1 != string::npos)
            m1 = 2;
        else
            m1 = 3;
        found1 = s.find("symmetric");
        found2 = s.find("hermitian");
        found3 = s.find("skew-symmetric");
        if (found1 != string::npos || found2 != string::npos || found3 != string::npos)
            sym = 1;
        else
            sym = 0;
        while(inf.peek()=='%')
            getline(inf,s);
        
        if(sym==0)
        {    
            cout<<endl<<"WARNING..!!"<<endl;
            cout<<"The mtx file contains full matrix"<<endl;
            cout<<"User has to make sure that input file has both (i,j) and (j,i) present."<<endl;
            cout<<"Matching is not defined if w(i,j) != w(j,i)"<<endl<<endl;
        }
 
        inf>>inp;
        numRow=inp;
        inf>>inp;
        numCol=inp;
        inf>>inp;
        nonZeros=inp;

        if(numRow==numCol)
            bipartite=false;
        else
            return false;

        count=inp;
        
        vector<vector<int> > graphCRSIdx(numRow);
        vector<vector<double> > graphCRSVal(numRow);
        int diag=0;
        
        while(count>0) 
        {     
            inf>>i; 
            inf>>j; 
            if(m1==3) 
                inf>>f; 
            else
                f=1.0;

            if(i==j)
                diag++;
            else
            {
                graphCRSIdx[i-1].push_back(j-1); 
                graphCRSVal[i-1].push_back(f);
                if(sym) 
                {     
                    graphCRSIdx[j-1].push_back(i-1); 
                    graphCRSVal[j-1].push_back(f); 
                } 
            }   
            count--; 
        }     
        inf.close(); 
     
        numEdges = nonZeros;  
        if(sym == 1) //symmetric matrix 
            numEdges = nonZeros*2 - 2*diag; 
     
        nVer=numRow; 
        lVer=0;
        rVer=nVer;
        nEdge=numEdges;
        
        verPtr=new int[nVer+1];
        verInd=new Edge[nEdge];

        verPtr[0]=0;
        int max=0,offset; 
        for(int i=1;i<=nVer;i++)
        {
            offset=graphCRSIdx[i-1].size();
            verPtr[i]=verPtr[i-1]+offset;
            count=verPtr[i-1];
            //cout<<i-1<<" "<<verPtr[i-1]<<" "<<verPtr[i]<<": ";
            for(int j=0;j<offset;j++)
            {
                verInd[count].id=graphCRSIdx[i-1][j];
                verInd[count].weight=graphCRSVal[i-1][j];
                count++;

                //cout<<verInd[count-1]<<" ";
            }
            //cout<<endl;
            if(offset>max)
                max=offset;
        }
        
        assert(count==nEdge);
        maxDeg=max;

    }
    else return false;
   
   return true;
}

bool CSR::mtxB2csrbin(char* filename, char* outfile)
{
    /*int count=0,i,j;
    int inp, m1, sym, edgecnt_;
    int numRow, numCol, nonZeros, numEdges;
    double f;
    string s;
    ifstream inf;

    inf.open(filename, ios::in);

    if(inf.is_open())
    {
        size_t found1, found2, found3;
        getline(inf,s);
        found1 = s.find("pattern");
        if (found1 != string::npos)
            m1 = 2;
        else
            m1 = 3;
        found1 = s.find("symmetric");
        found2 = s.find("hermitian");
        found3 = s.find("skew-symmetric");
        if (found1 != string::npos || found2 != string::npos || found3 != string::npos)
            sym = 1;
        else
            sym = 0;
        while(inf.peek()=='%')
            getline(inf,s);
 
        inf>>inp;
        numRow=inp;
        inf>>inp;
        numCol=inp;
        inf>>inp;
        nonZeros=inp;

        bipartite=false;
        nVer=numRow+numCol;
        lVer=numRow;
        rVer=numCol;
        count=inp;
 
        vector<vector<int> > graphCRSIdx(nVer);
        vector<vector<double> > graphCRSVal(nVer);
        int diag=0;
	    
        while(count>0) 
        {     
            inf>>i; 
            inf>>j; 
            j+=sVer; // Adjust for right hand side of biaprtite graph

            if(m1==3) 
                inf>>f; 
            else
                f=1.0;

            if(i==j)
                diag++;
            else
            {
                graphCRSIdx[i-1].push_back(j-1); 
                graphCRSVal[i-1].push_back(f);
                if(sym) 
                {     
                    graphCRSIdx[j-1].push_back(i-1); 
                    graphCRSVal[j-1].push_back(f); 
                } 
            }   
            count--; 
        }  
        inf.close(); 
 
     
        numEdges = nonZeros;  
        if(sym == 1) //symmetric matrix 
            numEdges = nonZeros*2 - 2*diag; 
     
        nEdge=numEdges;
    
        int* _verPtr=new int[nVer+1];
        int* _verInd=new int[nEdge];
        double* _verWt=new double[nEdge];

        _verPtr[0]=0;
        int max=0,offset;
        for(int i=1;i<=nVer;i++)
        {
            offset=graphCRSIdx[i-1].size();
            _verPtr[i]=_verPtr[i-1]+offset;
            count=_verPtr[i-1];
            //cout<<i-1<<" "<<verPtr[i-1]<<" "<<verPtr[i]<<": ";
            for(int j=0;j<offset;j++)
            {
                _verInd[count]=graphCRSIdx[i-1][j];
                _verWt[count]=graphCRSVal[i-1][j];
                count++;

                //cout<<verInd[count-1]<<" ";
            }
            //cout<<endl;
            if(offset>max)
                max=offset;
        }
        
        assert(count==nEdge);

        ///////////////// reading and csr format done //////////////
        // So write as binary file
        
        ofstream of;
        of.open(outfile,ios::out|ios::binary);
        of.write((char*)&nVer, sizeof(int));
        of.write((char*)&sVer, sizeof(int));
        of.write((char*)&nEdge, sizeof(int));
        of.write((char*)&max, sizeof(int));
        of.write((char*)&_verPtr[0], sizeof(int) * (nVer+1));
        of.write((char*)&_verInd[0], sizeof(int) * nEdge);
        of.write((char*)&_verWt[0], sizeof(double) * nEdge);
        of.close();
    }*/
    return true;
}

bool CSR::mtxG2csrbin(char* filename, char* outfile)
{
    int count=0,i,j;
    int inp, m1, sym, edgecnt_;
    int numRow, numCol, nonZeros, numEdges;
    double f;
    string s;
    ifstream inf;

    inf.open(filename, ios::in);

    if(inf.is_open())
    {
        size_t found1, found2, found3;
        getline(inf,s);
        found1 = s.find("pattern");
        if (found1 != string::npos)
            m1 = 2;
        else
            m1 = 3;
        found1 = s.find("symmetric");
        found2 = s.find("hermitian");
        found3 = s.find("skew-symmetric");
        if (found1 != string::npos || found2 != string::npos || found3 != string::npos)
            sym = 1;
        else
            sym = 0;
        while(inf.peek()=='%')
            getline(inf,s);
 
        inf>>inp;
        numRow=inp;
        inf>>inp;
        numCol=inp;
        inf>>inp;
        nonZeros=inp;

        if(numRow==numCol)
            bipartite=false;
        else return false;
 
        count=inp;
 
 
        vector<vector<int> > graphCRSIdx(numRow);
        vector<vector<double> > graphCRSVal(numRow);
        int diag=0;
	    
        while(count>0) 
        {     
            inf>>i; 
            inf>>j; 
            if(m1==3) 
                inf>>f; 
            else
                f=1.0;

            if(i==j)
                diag++;
            else
            {
                graphCRSIdx[i-1].push_back(j-1); 
                graphCRSVal[i-1].push_back(f);
                if(sym) 
                {     
                    graphCRSIdx[j-1].push_back(i-1); 
                    graphCRSVal[j-1].push_back(f); 
                } 
            }   
            count--; 
        }  
        inf.close(); 
 
     
        numEdges = nonZeros;  
        if(sym == 1) //symmetric matrix 
            numEdges = nonZeros*2 - 2*diag; 
     
     
        nVer=numRow; 
        nEdge=numEdges;
    
        int* _verPtr=new int[nVer+1];
        int* _verInd=new int[nEdge];
        double* _verWt=new double[nEdge];

        _verPtr[0]=0;
        int max=0,offset;
        for(int i=1;i<=nVer;i++)
        {
            offset=graphCRSIdx[i-1].size();
            _verPtr[i]=_verPtr[i-1]+offset;
            count=_verPtr[i-1];
            //cout<<i-1<<" "<<verPtr[i-1]<<" "<<verPtr[i]<<": ";
            for(int j=0;j<offset;j++)
            {
                _verInd[count]=graphCRSIdx[i-1][j];
                _verWt[count]=graphCRSVal[i-1][j];
                count++;

                //cout<<verInd[count-1]<<" ";
            }
            //cout<<endl;
            if(offset>max)
                max=offset;
        }
        
        assert(count==nEdge);

        ///////////////// reading and csr format done //////////////
        // So write as binary file
        
        ofstream of;
        of.open(outfile,ios::out|ios::binary);
        of.write((char*)&nVer, sizeof(int));
        //of.write((char*)&sVer, sizeof(int));
        of.write((char*)&nEdge, sizeof(int));
        of.write((char*)&max, sizeof(int));
        of.write((char*)&_verPtr[0], sizeof(int) * (nVer+1));
        of.write((char*)&_verInd[0], sizeof(int) * nEdge);
        of.write((char*)&_verWt[0], sizeof(double) * nEdge);
        of.close();
    }
    return true;
}

bool CSR::readCSRbin(char* filename, int opt)
{
    ifstream inf;
    inf.open(filename,ios::in|ios::binary);
    if(inf.is_open())
    {
        inf.read((char*)&nVer,sizeof(unsigned int));
        inf.read((char*)&nEdge,sizeof(unsigned int));
        inf.read((char*)&maxDeg,sizeof(unsigned int)); 
        
        verPtr=new int[nVer+1];
        verInd=new Edge[nEdge];
        unsigned int* _verInd=new unsigned int[nEdge];
        float* _verWt=new float[nEdge];
        
        inf.read((char*)&verPtr[0],sizeof(unsigned int)*(nVer+1));
        inf.read((char*)&_verInd[0],sizeof(unsigned int)*nEdge);
        inf.read((char*)&_verWt[0],sizeof(float)*nEdge);
        inf.close();

        
        if(opt==1)/// Save edge weight and degree dist
        {
            unsigned int pos=strrchr(filename, '.')-filename+1;
            filename[pos]='\0';
            char edges[100],degree[100];
            strcpy(edges,filename);
            strcpy(degree,filename);
            strcat(edges,"edges");
            strcat(degree,"degree");
            
            ofstream of1,of2;
            of1.open(degree, ios::out);
            of2.open(edges, ios::out);
            for(unsigned int i=0;i<nVer;i++)
                of1<<(verPtr[i+1]-verPtr[i])<<endl;
            for(unsigned int i=0;i<nEdge;i++)
                of2<<_verWt[i]<<endl;
            of1.close();
            of2.close();
        }
        if(opt==2) /// Do METIS STUFF
        {
            ;
        }

        #pragma omp parallel for schedule(dynamic,64)
        for(unsigned int i=0;i<nEdge;i++)
        {
            verInd[i].id=_verInd[i];
            verInd[i].weight=(float)_verWt[i];
            //if(verInd[i].weight>0)
                //verInd[i].ewt=verInd[i].weight/2.0;
            //else verInd[i].ewt=0.0;
        }

        delete _verInd;
        delete _verWt;
        return true;
    }
    else
    return false;

}


bool CSR::readCSRbinBipartite(char* filename, int opt)
{
    ifstream inf;
    inf.open(filename,ios::in|ios::binary);
    bipartite=true;
    if(inf.is_open())
    {
        inf.read((char*)&lVer,sizeof(unsigned int));
        inf.read((char*)&rVer,sizeof(unsigned int));
        inf.read((char*)&nEdge,sizeof(unsigned int));
        inf.read((char*)&maxDeg,sizeof(unsigned int)); 
        
        nVer=lVer+rVer;
        verPtr=new int[nVer+1];
        verInd=new Edge[nEdge];
        unsigned int* _verInd=new unsigned int[nEdge];
        float* _verWt=new float[nEdge];
        
        inf.read((char*)&verPtr[0],sizeof(unsigned int)*(nVer+1));
        inf.read((char*)&_verInd[0],sizeof(unsigned int)*nEdge);
        inf.read((char*)&_verWt[0],sizeof(float)*nEdge);
        inf.close();

        
        if(opt==1)/// Save edge weight and degree dist
        {
            unsigned int pos=strrchr(filename, '.')-filename+1;
            filename[pos]='\0';
            char edges[100],degree[100];
            strcpy(edges,filename);
            strcpy(degree,filename);
            strcat(edges,"edges");
            strcat(degree,"degree");
            
            ofstream of1,of2;
            of1.open(degree, ios::out);
            of2.open(edges, ios::out);
            for(unsigned int i=0;i<nVer;i++)
                of1<<(verPtr[i+1]-verPtr[i])<<endl;
            for(unsigned int i=0;i<nEdge;i++)
                of2<<_verWt[i]<<endl;
            of1.close();
            of2.close();
        }
        if(opt==2) /// Do METIS STUFF
        {
            ;
        }

        #pragma omp parallel for schedule(dynamic,64)
        for(unsigned int i=0;i<nEdge;i++)
        {
            verInd[i].id=_verInd[i];
            verInd[i].weight=(float)_verWt[i];
            //if(verInd[i].weight>0)
                //verInd[i].ewt=verInd[i].weight/2.0;
            //else verInd[i].ewt=0.0;
        }

        delete _verInd;
        delete _verWt;
        return true;
    }
    else
    return false;

}


int readBfile(char* filename, int* b, int nVer)
{
    int i,j=0,avgb=0,count=nVer;
    ifstream inf;

    inf.open(filename, ios::in);

    if(inf.is_open())
    {
        while(count>0) 
        {     
            inf>>i;
            b[j]=i;
            count--;
            j++;
            avgb+=i;
            if(inf.eof())
            {
                cout<<"b value file does not have all b-values:"<<endl;
                cout<<"Number of vertices: "<<nVer<<" but only "<<nVer-count<<" b values"<<endl;
                return -1;
            }
        }
    }
    else return -1;

    return avgb/nVer;
}

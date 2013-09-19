#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <queue>
#include <algorithm>
#include <fstream>
#include "helper.h"
#include <sys/time.h>
#include <omp.h>
using namespace std;

#define INF 1000
#define SANITY_CHECK 1

struct KIJ {
	int i;
	int j;
	int k;
public:
	KIJ(int k1, int i1, int j1): i(i1), j(j1), k(k1){}
	bool operator==(const KIJ& b){
		return k == b.k && i == b.i && j == b.j;
	}
		
};

struct DEPS {
	struct KIJ kij;		
	pair<int,int> ij;
	pair<int,int> ik;
	pair<int,int> kj;
public:
	DEPS(KIJ kij1, pair<int,int> ij1, pair<int,int> ik1, pair<int,int> kj1):kij(kij1), ij(ij1), ik(ik1), kj(kj1){}
};

struct Node{
	struct KIJ kij;
	list<struct Node*> m_children;
	
	Node(struct KIJ kij1) : kij(kij1) {}
};

class DepGraphs {
public:
	void addDependency(struct KIJ* parent, struct KIJ *child){
		struct Node* fromNode = findNode(parent);
		if(fromNode == NULL)
			addNode(parent);
		fromNode = findNode(parent);
		struct Node *childNode = findNode(child);
		if(childNode == NULL)
			addNode(child);
		childNode = findNode(child);
		fromNode->m_children.push_back(childNode);
		std::list<struct Node*>::iterator childIter = std::find(m_heads.begin(), m_heads.end(), childNode);
		if(childIter != m_heads.end())
			m_heads.remove(childNode);
	}

	struct Node* findNode(struct KIJ* key){
		list<struct Node*>::iterator headIter = m_heads.begin();
		for(; headIter != m_heads.end() ; ++ headIter){
			queue<struct Node*> nodeQ;
			nodeQ.push(*headIter);
			while(!nodeQ.empty()){
				struct Node* frontElem = nodeQ.front();
				nodeQ.pop();
				if(frontElem->kij == *key)
					return frontElem;
				list<struct Node*>::iterator childIter = frontElem->m_children.begin();
				for(; childIter != frontElem->m_children.end(); ++childIter)
					nodeQ.push(*childIter);
			}
		}
		return NULL;
	}
	
	void addNode(struct KIJ* kij){
		if(findNode(kij) != NULL)
			return;
		struct Node* newNode = new struct Node(*kij);
		m_heads.push_back(newNode);
	}

	

	void print(){
		list<struct Node*>::iterator headIter = m_heads.begin();
		for(; headIter != m_heads.end() ; ++ headIter){
			printNode(*headIter, 0);	
		}		
	}

    void printDot(){
        ofstream dotFile;
        dotFile.open("dep.dot", ios::out);
        dotFile << "digraph { " << endl;
    	list<struct Node*>::iterator headIter = m_heads.begin();
		for(; headIter != m_heads.end() ; ++ headIter){
			printNodeDot(dotFile, *headIter);	
            dotFile << "\"(" <<  (*headIter)->kij.k << "," << (*headIter)->kij.i << "," << (*headIter)->kij.j << ")\"" << endl;
		}
        dotFile << "}";

        
    }
private:
	void printNode(struct Node* n, int level){
		for(int i = 0 ; i < level; ++i)	
			cout << "--";
		cout << " ( " <<  n->kij.k << " " << n->kij.i << " " << n->kij.j << " ) " << endl;
		list<struct Node*>::iterator childIter = n->m_children.begin();
		for(; childIter != n->m_children.end(); ++childIter)
			printNode(*childIter, level+1);
	}

    void printNodeDot(ofstream& dotFile, struct Node* n){
		//for(int i = 0 ; i < level; ++i)	
		//	cout << "--";
		//cout << " ( " <<  n->kij.k << " " << n->kij.i << " " << n->kij.j << " ) " << endl;
		list<struct Node*>::iterator childIter = n->m_children.begin();
		for(; childIter != n->m_children.end(); ++childIter){
            dotFile << "\"(" <<  n->kij.k << "," << n->kij.i << "," << n->kij.j << ")\"->";
            dotFile << "\"(" <<  (*childIter)->kij.k << "," << (*childIter)->kij.i << "," << (*childIter)->kij.j << ")\"" << endl;
			printNodeDot(dotFile, *childIter);
        }
	}
	list<struct Node*> m_heads; //we might have more than one graph
	
};

DepGraphs gDepGraphs; //do not use globals!

void constructDepGraph(list<struct DEPS*> *depsList){
	list<struct DEPS*>::reverse_iterator curIter = depsList->rbegin();
	for(; curIter != depsList->rend(); ++curIter){
		list<struct DEPS*>::reverse_iterator prevEltIter = curIter;
		gDepGraphs.addNode(&((*curIter)->kij));
		++prevEltIter;		
		cout << " ( " << (*curIter)->kij.k << " " << (*curIter)->kij.i << " " << (*curIter)->kij.j << " ) depends on ";
		bool foundik = false, foundkj = false;
		while(prevEltIter != depsList->rend() && !(foundik && foundkj)){
			if(!foundik && (*curIter)->ik == (*prevEltIter)->ij){
				cout << " ( " << (*prevEltIter)->kij.k << " " << (*prevEltIter)->kij.i << " " << (*prevEltIter)->kij.j << " ) ";
				gDepGraphs.addDependency(&(*prevEltIter)->kij, &(*curIter)->kij);	
				foundik = true;
			}
			if(!foundkj && (*curIter)->kj == (*prevEltIter)->ij){
				cout << " ( " << (*prevEltIter)->kij.k << " " << (*prevEltIter)->kij.i << " " << (*prevEltIter)->kij.j << " ) ";
				gDepGraphs.addDependency(&(*prevEltIter)->kij, &(*curIter)->kij);	
				foundkj = true;
			}
			++prevEltIter;	
		}
		if(!foundik && !foundkj)
			cout << "nothing.";
		cout << endl;
	}
}

double doFloyd(int **dist, int n){
    struct timeval before, after;

    gettimeofday(&before, NULL);

   //cout << "< k i j > : < i j >  < i j > < i k > < k j >" << endl;
    for(int k = 0 ; k < n ; ++k)
        for(int i = 0 ; i < n ; ++i)
            for(int j = 0 ; j < n ; ++j){
                //cout << "< " << k << " " << i << " " << j << " > : < " << i << " " << j << " >  < " << i << " " << j << " > < "<< i << " " << k << " > < " << k << " " << j << " >" << endl; 
                //depsList.push_back(new struct DEPS(*(new struct KIJ(k,i,j)), pair<int,int>(i,j), pair<int,int>(i,k), pair<int,int>(k,j)));
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }

    gettimeofday(&after, NULL);

    return time_diff(before, after);
}

double doParFloyd(int **distPar, int n){
    struct timeval before, after;

    gettimeofday(&before, NULL);
    for(int k = 0 ; k < n ; ++k)
#pragma omp parallel for
        for(int i = 0 ; i < n ; ++i) {
            for(int j = 0 ; j < n ; ++j){
//                if(i != k && j != k)
                    distPar[i][j] = min(distPar[i][j], distPar[i][k] + distPar[k][j]);
            }
        }

    gettimeofday(&after, NULL);

    return time_diff(before, after);

}

int main() {
	
	int **dist, **distPar, **distCopy, n;
	cout << "Enter the number of vertices" << endl;
	cin >> n;
	
	dist = new int*[n];	
    distPar = new int*[n];
    distCopy = new int*[n];

	srand(time(NULL));

	for(int i = 0 ; i < n ; ++i){
		dist[i] = new int[n];
		distCopy[i] = new int[n];
        distPar[i] = new int[n];
		for(int j = 0 ; j < n ; ++j){
			distCopy[i][j] = rand()%100;
        }
	}
    
    cout << "Done Intializing" << endl;
	//list<struct DEPS*> depsList;
    
    cout << "Running sequential floyd..." << endl;
    const int iterations = 25;
    double* floydTime = new double[iterations];
    for(int i = 0 ; i < iterations ; ++i){
        copy(distCopy, dist, n);
        floydTime[i] = doFloyd(dist, n);
    }

    cout << "Floyd Sequential : Mean - " << average(floydTime, iterations) << "ms, Median - " << median(floydTime, iterations) << "ms." << endl;

    cout << "Running parallel floyd..." << endl;
    double* parfloydTime = new double[iterations];
    for(int i = 0 ; i < iterations ; ++i){
        copy(distCopy, distPar, n);
        parfloydTime[i] = doParFloyd(distPar, n);
    }

    cout << "Floyd Parallel : Mean - " << average(parfloydTime, iterations) << "ms, Median - " << median(parfloydTime, iterations) << "ms." << endl;


#if SANITY_CHECK   
    cout << "New floyd " << (isEqual(dist, distPar, n) ? "valid" : "invalid") << endl; 
#endif

	//constructDepGraph(&depsList);
	//gDepGraphs.print();
	//gDepGraphs.printDot();
	//print(dist, n);	
	return 0;
}

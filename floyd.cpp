#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <list>
using namespace std;

#define INF 1000

struct KIJ {
	int i;
	int j;
	int k;
public:
	KIJ(int k1, int i1, int j1): i(i1), j(j1), k(k1){
		
	}
		
};

struct DEPS {
	struct KIJ kij;		
	pair<int,int> ij;
	pair<int,int> ik;
	pair<int,int> kj;
public:
	DEPS(KIJ kij1, pair<int,int> ij1, pair<int,int> ik1, pair<int,int> kj1):kij(kij1), ij(ij1), ik(ik1), kj(kj1){
	}
};

class DepGraph {
public:
	
};

void constructDepGraph(list<struct DEPS*> *depsList){
	list<struct DEPS*>::reverse_iterator curIter = depsList->rbegin();
	for(; curIter != depsList->rend(); ++curIter){
		list<struct DEPS*>::reverse_iterator prevEltIter = curIter;
		++prevEltIter;		
		cout << " ( " << (*curIter)->kij.k << " " << (*curIter)->kij.i << " " << (*curIter)->kij.j << " ) depends on ";
		bool foundik = false, foundkj = false;
		while(prevEltIter != depsList->rend() && !(foundik && foundkj)){
			if(!foundik && (*curIter)->ik == (*prevEltIter)->ij){
				cout << " ( " << (*prevEltIter)->kij.k << " " << (*prevEltIter)->kij.i << " " << (*prevEltIter)->kij.j << " ) ";
				foundik = true;
			}
			if(!foundkj && (*curIter)->kj == (*prevEltIter)->ij){
				cout << " ( " << (*prevEltIter)->kij.k << " " << (*prevEltIter)->kij.i << " " << (*prevEltIter)->kij.j << " ) ";
				foundkj = true;
			}
			++prevEltIter;	
		}
		if(!foundik && !foundkj)
			cout << "nothing.";
		cout << endl;
	}
}

void print(int **p, int n){
	for(int i = 0 ; i < n ; ++i){
		for(int j = 0 ; j < n ; ++j)
			cout << p[i][j] << " ";
		cout << endl;
	}
}

int main() {
	
	int **dist, n;
	cout << "Enter the number of vertices" << endl;
	cin >> n;
	
	dist = new int*[n];	

	srand(time(NULL));

	for(int i = 0 ; i < n ; ++i){
		dist[i] = new int[n];
		for(int j = 0 ; j < n ; ++j)
			dist[i][j] = rand()%100;
	}
	
	list<struct DEPS*> depsList;
	cout << "< k i j > : < i j >  < i j > < i k > < k j >" << endl;
	for(int k = 0 ; k < n ; ++k)
		for(int i = 0 ; i < n ; ++i)
			for(int j = 0 ; j < n ; ++j){
//				cout << "< " << k << " " << i << " " << j << " > : < " << i << " " << j << " >  < " << i << " " << j << " > < "<< i << " " << k << " > < " << k << " " << j << " >" << endl; 
				depsList.push_back(new struct DEPS(*(new struct KIJ(k,i,j)), pair<int,int>(i,j), pair<int,int>(i,k), pair<int,int>(k,j)));
				dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
			}
	constructDepGraph(&depsList);
	//print(dist, n);	
	return 0;
}
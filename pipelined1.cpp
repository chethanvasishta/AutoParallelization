#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>
#include <omp.h>
using namespace std;

#define SANITY_CHECK 1

/* Returns time elapsed in ms */
double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff/1000;
}

bool isEqual(int **a, int **b, int n){

    for(int i = 0 ; i < n ; ++i)
        for(int j = 0 ; j < n ; ++j)
            if(a[i][j] != b[i][j])
                return false;
    return true;

}

int main(){
    int n;
    int **dist, **distCopy, **distPar;
    cout << "Enter the size of the array" << endl;
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
			dist[i][j] = rand()%100;
            distCopy[i][j] = dist[i][j];
            distPar[i][j] = dist[i][j];
        }
	}

    cout << "done initializing ... " << endl;
   
    //non-parallel  

    struct timeval before, after;

    gettimeofday(&before, NULL);

    for(int i = 0 ; i < n ; ++i){
        for(int j = 0 ; j < n ; ++j){
            dist[i][j] = dist[j][i];
        }
    }

    gettimeofday(&after, NULL);

    cout << "Non - parallel execution took " << time_diff(before, after) << " milliseconds " << endl; 

    //parallel with j
    
    gettimeofday(&before, NULL);
   
    for(int j = n-1 ; j >= 0 ; --j){
#pragma omp parallel for
        for(int i = 0 ; i < n ; ++i){
            distPar[i][j] = distPar[j][i];
        }
    }

    gettimeofday(&after, NULL);
    
    cout << "Parallel execution along j took " << time_diff(before, after) << " milliseconds " << endl;

#if SANITY_CHECK
   
    cout << "Parallelization is " << (isEqual(dist, distPar,n) ? "sane" : "insane") << endl;
    
#endif //SANITY_CHECK

    //copy back
    
    for(int i = 0 ; i < n ; ++i){
        for(int j = 0 ; j < n ; ++j){
            distPar[i][j] = distCopy[i][j];
        }
    }

    //parallel with i
    //Might be slower than j because of caching issues
    
    gettimeofday(&before, NULL);
   
    for(int i = 0 ; i < n ; ++i){
#pragma omp parallel for
        for(int j = n-1 ; j >= 0 ; --j){
            distPar[i][j] = distPar[j][i];
        }
    }

    gettimeofday(&after, NULL);
    
    cout << "Parallel execution along i took " << time_diff(before, after) << " milliseconds " << endl;

#if SANITY_CHECK
   
    cout << "Parallelization is " << (isEqual(dist, distPar, n) ? "sane" : "insane") << endl;
    
#endif //SANITY_CHECK

    return 0;
}

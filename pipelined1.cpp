#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>
#include <omp.h>
#include <algorithm>
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

double doSequential(int **a, int n){

    struct timeval before, after;

    gettimeofday(&before, NULL);

    for(int i = 0 ; i < n ; ++i){
        for(int j = 0 ; j < n ; ++j){
            a[i][j] = a[j][i];
        }
    }

    gettimeofday(&after, NULL);

    return time_diff(before, after);

}

double doParallelJ(int **a, int n){

    struct timeval before, after;
    
    gettimeofday(&before, NULL);

    for(int j = n-1 ; j >= 0 ; --j){
omp_set_num_threads(8);
#pragma omp parallel for
        for(int i = 0 ; i < n ; ++i){
            a[i][j] = a[j][i];
        }
    }

    gettimeofday(&after, NULL);

    return time_diff(before, after);
}

double doParallelI(int **a, int n){
    
    struct timeval before, after;

    gettimeofday(&before, NULL);

    for(int i = 0 ; i < n ; ++i){
omp_set_num_threads(8);
#pragma omp parallel for
        for(int j = n-1 ; j >= 0 ; --j){
            a[i][j] = a[j][i];
        }
    }
    gettimeofday(&after, NULL);

    return time_diff(before, after);
}


void copy(int **src, int** dest, int n){
    for(int i = 0 ; i < n ; ++i){
        for(int j = 0 ; j < n ; ++j){
            dest[i][j] = src[i][j];
        }
    }
}

double average(double *x, int n){
    double avg = 0.0f;
    for(int i = 0 ; i < n ; ++i)
        avg += x[i];
    return avg/n;
}

double median(double *x, int n){
    sort(x, x+n);
    return x[n/2];
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
			distCopy[i][j] = rand()%100;
        }
	}

    cout << "done initializing ... " << endl;
  
    const int iterations = 25;

    //non-parallel  
   
    double *seqTime = new double[iterations];
    for(int i = 0 ; i < iterations ; ++i){
        copy(distCopy, dist, n);
        seqTime[i] = doSequential(dist, n);
    }
    cout << "Non - parallel execution : Mean - " << average(seqTime, iterations) << " milliseconds, Median -  " << median(seqTime, iterations) << " milliseconds " <<endl;
    
    //parallel with j
    
    double *parJTime = new double[iterations];
    for(int i = 0 ; i < iterations ; ++i){
        copy(distCopy, distPar, n);
        parJTime[i] = doParallelJ(distPar, n);    
    }
    cout << "Parallel execution with j: Mean - " << average(parJTime, iterations) << " milliseconds, Median -  " << median(parJTime, iterations) << " milliseconds " <<endl;

#if SANITY_CHECK
   
    cout << "Parallelization is " << (isEqual(dist, distPar, n) ? "sane" : "insane") << endl;
    
#endif //SANITY_CHECK

    //parallel with i
    //Might be slower than j because of caching issues
    
    double *parITime = new double[iterations];
    for(int i = 0 ; i < iterations ; ++i){
        copy(distCopy, distPar, n);
        parITime[i] = doParallelI(distPar, n);    
    }
    cout << "Parallel execution with i: Mean - " << average(parITime, iterations) << " milliseconds, Median -  " << median(parITime, iterations) << " milliseconds " <<endl;
   
#if SANITY_CHECK
    cout << "Parallelization is " << (isEqual(dist, distPar, n) ? "sane" : "insane") << endl;
#endif //SANITY_CHECK

    return 0;
}

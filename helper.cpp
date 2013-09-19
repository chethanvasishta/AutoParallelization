#include "helper.h"
#include <algorithm>
#include <iostream>
using namespace std;
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

void print(int **p, int n){
	for(int i = 0 ; i < n ; ++i){
		for(int j = 0 ; j < n ; ++j)
			cout << p[i][j] << " ";
		cout << endl;
	}
}

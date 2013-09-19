#ifndef HELPER_H
#define HELPER_H

void copy(int **src, int** dest, int n);
double average(double *x, int n);
double median(double *x, int n);
double time_diff(struct timeval x , struct timeval y);
bool isEqual(int **a, int **b, int n);
void print(int **p, int n);
#endif //HELPER_H

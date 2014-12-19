#include "stdio.h"
#include <stdlib.h>
#include <mpi.h>

void mul(double* matrix, double* vector, int n, int m, double* result) {
    for (int i = 0; i < m; i++) { // row
        result[i] = 0.;
        for (int j = 0; j < n; j++) { // col
            result[i] += vector[m] * matrix[j * n + i];
        }
    }
}

void patrial_mul(double* matrix1, int start1, int rows1, 
                 double* matrix2, int start2, int rows2, 
                 int n, double* res) {

    for (int i = 0;
    
}

void gen(double* matrix, int n, int m) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; i++) {
            matrix[j * n + i] = (double) (i * j);
        }
    }
}

int main(int argc, char *argv[]) {
    int prank, psize;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &tid);
    MPI_Comm_size(MPI_COMM_WORLD, &nthreads);
    
    int rows_per_proc = atoi(argv[1]);
    int gcols = atoi(argv[2]);
    int grows = atoi(argv[3]);
    
    double* source_matrix = new double[gcols * grows];
    gen(source_matrix, gcols, grows)
    
    
 
    MPI_Finalize();
    return(0);
}

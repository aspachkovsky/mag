


void mul(double* matrix, double* vector, int n, int m, double* result) {
    for (int i = 0; i < m; i++) { // row
        result[i] = 0.;
        for (int j = 0; j < n; j++) { // col
            result[i] += vector[m] * matrix[j * n + i];
        }
    }
}

void gen(double* matrix, int n, int m) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; i++) {
            matrix[j * n + i] = (double) (i * j);
        }
    }
}

int main(int argc, char* argv[]) {
    
    int procnum = atoi(argv[1]);
    int colperproc = atoi(argv[2]);
    int gn = atoi(argv[3]);
    int gm = atoi(argv[4]);

    

    exit(0);
}

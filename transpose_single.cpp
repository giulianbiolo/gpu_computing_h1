#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <chrono>


void SimpleTranspose(int N, double  **A, double  **B) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[j][i] = A[i][j];
        }
    }
    return;
}

void BlockTranspose(int N, int b, double  **A, double  **B) {
    for (int i = 0; i < N; i += b) {
        for (int j = 0; j < N; j += b) {
            for (int k = i; k < i + b; k++) {
                for (int l = j; l < j + b; l++) {
                    B[l][k] = A[k][l];
                }
            }
        }
    }
    return;
}

typedef struct MeasureRes {
    int64_t micros;
    double dummy_sum;
} MeasureRes;

MeasureRes exec_simple_n(int N) {
    MeasureRes res;
    double  **Amat = (double  **)malloc(N * sizeof(double*));
    double  **Bmat = (double  **)malloc(N * sizeof(double*));
    for (int i = 0; i < N; i++) {
        Amat[i] = (double*)malloc(N * sizeof(double));
        Bmat[i] = (double*)malloc(N * sizeof(double));
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Amat[i][j] = i * N + j + 1;  // 1 2 3 4 5 6 7 8 9 10 ...
            Bmat[i][j] = 0.0;
        }
    }
    auto start_simple = std::chrono::high_resolution_clock::now();
    SimpleTranspose(N, Amat, Bmat);
    auto end_simple = std::chrono::high_resolution_clock::now();

    auto elapsed_simple = end_simple - start_simple;
    res.micros = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_simple).count();
    // Free the memory
    for (int i = 0; i < N; i++) {
        free((void*)Amat[i]);
        free((void*)Bmat[i]);
    }
    free(Amat);
    free(Bmat);
    return res;
}

MeasureRes exec_block_n(int N, int B) {
    MeasureRes res;
    double  **Amat = (double  **)malloc(N * sizeof(double*));
    double  **Bmat = (double  **)malloc(N * sizeof(double*));
    for (int i = 0; i < N; i++) {
        Amat[i] = (double*)malloc(N * sizeof(double));
        Bmat[i] = (double*)malloc(N * sizeof(double));
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Amat[i][j] = i * N + j + 1;  // 1 2 3 4 5 6 7 8 9 10 ...
            Bmat[i][j] = 0.0;
        }
    }
    auto start_block = std::chrono::high_resolution_clock::now();
    BlockTranspose(N, B, Amat, Bmat);
    auto end_block = std::chrono::high_resolution_clock::now();

    auto elapsed_block = end_block - start_block;
    res.micros = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_block).count();
    // Free the memory
    for (int i = 0; i < N; i++) {
        free((void*)Amat[i]);
        free((void*)Bmat[i]);
    }
    free(Amat);
    free(Bmat);
    return res;
}

int main(int argc, char *argv[]) {
    int N = (argc >= 2 ) ? atoi(argv[1]) : 10; // Take the first argument as N, default = 10
    N = 1<<N;
    int B = (argc >= 3 ) ? atoi(argv[2]) : 0;  // Take the second argument as B, default = 0 (simple transpose)
    if (B == 0) {
        MeasureRes res_simple = exec_simple_n(N);
        printf("Simple Transpose: %ld ns, Dummy Sum: %f\n", res_simple.micros, res_simple.dummy_sum);
    } else {
        MeasureRes res_block = exec_block_n(N, B);
        printf("Block Transpose: %ld ns, Dummy Sum: %f\n", res_block.micros, res_block.dummy_sum);
    }
    return 0;
}

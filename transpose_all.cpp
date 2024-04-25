#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <chrono>


void SimpleTranspose(int N, double **A, double **B) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[j][i] = A[i][j];
        }
    }
    return;
}

void BlockTranspose(int N, int b, double **A, double **B) {
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
    double **Amat = (double**)malloc(N * sizeof(double*));
    double **Bmat = (double**)malloc(N * sizeof(double*));
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
    // Dummy Sum
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res.dummy_sum += Bmat[i][j];
        }
    }
    // Free the memory
    for (int i = 0; i < N; i++) {
        free(Amat[i]);
        free(Bmat[i]);
    }
    free(Amat);
    free(Bmat);
    return res;
}

MeasureRes exec_block_n(int N, int B) {
    MeasureRes res;
    double **Amat = (double**)malloc(N * sizeof(double*));
    double **Bmat = (double**)malloc(N * sizeof(double*));
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
    // Dummy Sum
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            res.dummy_sum += Bmat[i][j];
        }
    }
    // Free the memory
    for (int i = 0; i < N; i++) {
        free(Amat[i]);
        free(Bmat[i]);
    }
    free(Amat);
    free(Bmat);
    return res;
}

int main(int argc, char *argv[]) {
    int N = (argc >= 2 ) ? atoi(argv[1]) : 10; // Take the first argument as N, default = 10
    int B = (argc >= 3 ) ? atoi(argv[2]) : 128;  // Take the second argument as B, default = 4
    // N = (1 << N); // N = 2^N
    for (int i = 1; i <= N; i++) {
        int N2 = (1 << i);
        printf("%d : { ", N2);
        // Execute 50 times and take the average time
        __int128_t sum = 0;
        int64_t max_micros = 0;
        int64_t min_micros = 1000000000000000;
        double dummy;
        for (int k = 0; k < 50; k++) {
            MeasureRes res = exec_simple_n(N2);
            dummy = res.dummy_sum;
            sum += res.micros;
            if (res.micros > max_micros) max_micros = res.micros;
            if (res.micros < min_micros) min_micros = res.micros;
        }
        int64_t micros_simple = (int64_t)((double)sum / (double)50.0);
        //double dummy_sum = res.dummy_sum;
        // printf("Dummy Sum: %f\n", dummy_sum);
        printf("'S': { 'MIN': %.3f, 'MAX': %.3f, 'MED': %.3f }, ", min_micros / 1000.0f, max_micros / 1000.0f, micros_simple / 1000.0f);
        for (int j = 2; j <= B; j *= 2) {
            if (N2 < j) break;
            __int128_t sum = 0;
            int64_t max_micros = 0;
            int64_t min_micros = 1000000000000000;
            double dummy;
            for (int k = 0; k < 50; k++) {
                MeasureRes res = exec_block_n(N2, j);
                sum += res.micros;
                dummy = res.dummy_sum;
                if (res.micros > max_micros) max_micros = res.micros;
                if (res.micros < min_micros) min_micros = res.micros;
            }
            int64_t micros_block = (int64_t)((double)sum / (double)50.0);
            // printf("Dummy Sum: %f\n", dummy_sum);
            printf("'B%d': { 'MIN': %.3f, 'MAX': %.3f, 'MED': %.3f } , ", j, min_micros / 1000.0f, max_micros / 1000.0f, micros_block / 1000.0f);
        }
        printf("},\n");
    }
    return 0;
}

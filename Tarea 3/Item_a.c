#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define M 6000

long double matrix[M][M];
long double aux[M][M];

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int randomNumber(){
    srand(rdtsc());
    return (rand() % 9)+1;
}

void init(){ //Initializing the matrix
    int i,j,sign;
    for(i=0;i<M;++i){
        for(j=0;j<M;++j){
            sign = randomNumber();
            if(sign%2 == 0){
                matrix[i][j] = randomNumber();
                aux[i][j] = matrix[i][j];
            }
            else{
                matrix[i][j] = randomNumber() * (-1);
                aux[i][j] = matrix[i][j];
            }
        }
    }
}

void printMatrix(){
    int i,j;
    printf("\n");
    for(i = 0; i < M; ++i){
        for(j = 0; j < M; ++j){
            if(matrix[i][j] >= 0){
                printf(" ");
            }
            printf("%Lf ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//==================================================================================
int serialGauss(){
    int i,k,j;
    long double mi,mk;
    for(k=0;k<M;++k){
        for(i=k+1;i<M;++i){
            if(matrix[k][k] != 0){
                mi = matrix[i][k];
                mk = matrix[k][k];
                for(j=k;j<M;++j){
                    matrix[i][j] -= (mi/mk)*matrix[k][j];
                }
            }
            else{
                printf("Fallo al dividir por 0. Retornando... \n");
                return 1;
            }
        }
    }
}

long double serialCalculation(){
    long double det = 1;
    int i;
    for(i = 0; i<M ; ++i) det *= matrix[i][i];
    return det;
}

//========================================================================

long double parallelCalculation(long double matrix[M][M]){
    long double det = 1;
    #pragma omp paralallel for
        for(int i = 0; i<M ; ++ i) det *= matrix[i][i];
    return det;
}

void parallelGauss(long double matrix[M][M]){
    int i,k,j;
    long double mi,mk;
    for(k=0;k<M;++k){
        for(i=k+1;i<M;++i){
            if(matrix[k][k] != 0){
                mi = matrix[i][k];
                mk = matrix[k][k];
                #pragma omp parallel for
                for(j=k;j<M;++j){
                    matrix[i][j] -= (mi/mk)*matrix[k][j];
                }
            }
            else{
                printf("Indeterminate division... \n");
            }
        }
    }
}

int main(){
    FILE *  f;
    f = fopen("Item a.txt","w");
    clock_t start_t = clock();
    clock_t end_t = clock();
    double start,seconds;
    fprintf(f,"Initializing the matrix...\n");
    printf("Initializing the matrix...\n");
    init();
    //fprintf(f,"Showing original matrix...\n");
    //printMatrix(); //In console

    fprintf(f,"=========SERIAL CALCULATION===========\n");
    printf("=========SERIAL CALCULATION===========\n");
    fprintf(f,"Creating U matrix...\n");
    printf("Creating U matrix...\n");
    //fprintf(f,"Showing triangular matrix...\n");
    //printMatrix(); //In console
    start_t = clock();
    if(serialGauss() != 1){
        fprintf(f,"Matrix's determinant: %Lf\n",serialCalculation());
        printf("Matrix's determinant: %Lf\n",serialCalculation());
    }
    else{
        fprintf(f,"Matrix's determinant is 0.\n");
        printf("Matrix's determinant is 0.\n");
    }
    start_t = clock() - start_t;
    seconds = (double)start_t/CLOCKS_PER_SEC;
    fprintf(f,"Ejecution time without OpenMP was: %f seconds\n",seconds);
    printf("Ejecution time without OpenMP was: %f seconds\n",seconds);

    fprintf(f,"=========PARALLEL CALCULATION============\n");
    printf("=========PARALLEL CALCULATION============\n");
    fprintf(f,"Creating U matrix...\n");
    printf("Creating U matrix...\n");
    start = omp_get_wtime();
    parallelGauss(aux);
    fprintf(f,"Matrix's determinant: %Lf\n",parallelCalculation(aux));
    printf("Matrix's determinant: %Lf\n",parallelCalculation(aux));
    start = omp_get_wtime() - start;
    fprintf(f,"Ejecution time with OpenMP was: %f seconds.",start);
    printf("Ejecution time with OpenMP was: %f seconds.",start);
    fclose(f);
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define M 3001

long double matrix[M][M];

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
            }
            else{
                matrix[i][j] = randomNumber() * (-1);
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

//========================================CALCULO EN SERIE=================================================
int gauss(){
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

long double calcularEnSerie(){
    long double det = 1;
    int i;
    for(i = 0; i<M ; ++i) det *= matrix[i][i];
}

//======================================CALCULO EN PARALELO==============================================

long double calcularEnParalelo(){
    long double det = 1;
    #pragma omp paralallel for
        for(int i = 0; i<M ; ++ i) det *= matrix[i][i];
    return det;
}

int gaussParalelo(){
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

int main(){
    clock_t start_t = clock();
    clock_t end_t = clock();
    double start,end,seconds;
    init();
    printf("Imprimiendo matriz original:\n");
    printMatrix();

    printf("=========CALCULO EN SERIE===========\n");
    printf("Creando matriz U...\n");
    start_t = clock();
    if(gaussSerie() != 1){
        printf("El determinante de la matriz original es: %Lf\n",calcularEnSerie());
    }
    else{
        printf("El determinante de la matriz es 0.\n");
    }
    end_t = clock();
    seconds = (double)(end_t-start_t/CLOCKS_PER_SEC);
    printf("El tiempo de ejecución sin OpenMP fue: %f segundos\n",seconds);

    printf("=========CALCULO EN PARALELO============\n");
    printf("Creando matriz U...\n");
    start = omp_get_wtime();
    if(gaussParalelo() != 1){
        printf("El determinante de la matriz original es: %Lf\n",calcularEnParalelo());
    }
    else{
        printf("El determinante de la matriz es 0.\n");
    }
    end = omp_get_wtime();
    printf("El tiempo de ejecución con OpenMP fue: %f segundos.",end-start);

    return 0;
}
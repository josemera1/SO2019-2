#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define M 50000000

int array[M];

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int randomNumber(){
    srand(rdtsc());
    return (rand() % 10);
}

void init(){ //Initializing the matrix
    int i,j,sign;
    for(i=0;i<M;++i){
        sign = randomNumber();
        if(sign%2 == 0){
            array[i] = randomNumber();
        }
        else{
            array[i] = randomNumber() * (-1);
        }
    }
}

void printArray(){
    int i,j;
    printf("\n");
    for(i = 0; i < M; ++i) printf("%d ",array[i]);
    printf("\n");
}

void printArray2(int * array[M]){
    int i,j;
    printf("\n");
    for(i = 0; i < M; ++i) printf("%d ",* array[i]);
    printf("\n");
}

void merge(int * array, int start, int end){
    int i;
    int middle = (start + end) / 2;
    int temp_index = 0;
    int * temp = malloc(sizeof(int) * (end - start + 1));
    int left = start;
    int right = middle + 1;
    while((left <= middle) && (right <= end)) {
        if (array[left] < array[right]) {
            temp[temp_index] = array[left];
            temp_index += 1;
            left += 1;
        }
        else {
            temp[temp_index] = array[right];
            temp_index += 1;
            right += 1;
        }
    }
    while(left <= middle) {
        temp[temp_index] = array[left];
        temp_index += 1;
        left += 1;
    }
    while(right <= end) {
        temp[temp_index] = array[right];
        temp_index += 1;
        right += 1;
    }
    for(i = start; i <= end; i++) array[i] = temp[i - start];
}

//========================================CALCULO EN SERIE=================================================

void mergeSortInSeries(int* array, int start, int end) {
    if(start < end) {
        int middle = (start + end) / 2;
        mergeSortInSeries(array, start, middle);
        mergeSortInSeries(array, middle + 1, end);
        merge(array, start, end);
    }
}

//======================================CALCULO EN PARALELO==============================================

void mergeSortInParallel(int* array, int start, int end) {
    if(start < end) {
        int middle = (start + end) / 2;
        #pragma omp task
        mergeSortInParallel(array, start, middle);
        mergeSortInParallel(array, middle + 1, end);
        #pragma omp taskwait
        merge(array, start, end);
    }
}


int main(){
    FILE *f;
    f = fopen("Item b.txt","w");
    clock_t start_t;
    double start,seconds;
    fprintf(f,"Inicializando array...\n");
    init();
    int * array2 = array;
    //printf(f,"Imprimiendo array original:\n");
    //printArray(array); // en consola

    fprintf(f,"=========CALCULO EN SERIE===========\n");
    start_t = clock();
    mergeSortInSeries(array,0,M-1);
    start_t = clock()- start_t;
    seconds = (double)start_t/CLOCKS_PER_SEC;
    fprintf(f,"El arreglo final es el siguiente:\n");
    //printArray(array); // en consola
    fprintf(f,"El tiempo de ejecución sin OpenMP fue: %f segundos\n",seconds);

    fprintf(f,"=========CALCULO EN PARALELO============\n");
    start = omp_get_wtime();
    mergeSortInParallel(array2,0,M-1);
    start = omp_get_wtime() - start;
    //fprintf("El arreglo final es el siguiente:\n");
    //printArray2(array2); // en consola
    fprintf(f,"El tiempo de ejecución con OpenMP fue: %f segundos.\n",start);

    fclose(f);
    return 0;
}

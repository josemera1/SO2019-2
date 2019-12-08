#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define M 5

int matrix[M][M];

int calculateInSeries(int matrix[M][M], int dim);

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int sgn (int x){
	if (x%2==0) return(1);
	return(-1);
}

int randomNumber(){
    srand(rdtsc());
    return (rand() % 10);
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
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void matrix_adj(int matrix[M][M], int matrix_aux[M][M], int dim, int m, int n) {
	int i,j,p,q;
	for (j=0, q=0; j<dim; ++j) 
		if (j!=n) {
			for (i=0, p=0; i<dim; ++i)
				if (i!=m) {
					matrix_aux[p][q]=matrix[i][j];
					p++;
				}
			q++;
		}
}

int det_matrix_adj(int matrix[M][M], int dim, int m, int n) {
	int matrix_aux[M][M];
	if (dim==1){
        return(1);
    }
	matrix_adj(matrix,matrix_aux,dim,m,n);
	return calculateInSeries(matrix_aux,dim-1);
}

int calculateInSeries(int matrix[M][M], int dim) {
	int i,j,dim_aux,det;
	for (i=0, j=0, det=0, dim_aux=dim; i<dim; ++i)
		det += sgn(i+j)*matrix[i][j]*det_matrix_adj(matrix,dim_aux,i,j);
	return(det);
}

void calculateInParallel(){
    //ImplementarOpenMP
}

int main(){
    init();
    printf("=====================\n");
    printMatrix();
    printf("El determinante es %d\n",calculateInSeries(matrix,M));
    printf("=====================\n");
    return 0;
}
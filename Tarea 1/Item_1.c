#include <sys/types.h>
#include <stdlib.h> //for syscall exit
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h> //for syscall wait}

int createP(int n, int c){
    if (n <= 0){
        printf("No se crean procesos... adios\n");
        exit(0);
    }
    if (n == 1){
        printf("Hola soy el ultimo/unico proceso %d\n", getpid());
    }
    else{
        int Node;
        Node = fork();
        if (c%2==0){
            if (Node == 0){
                sleep(1);
                printf("Hola soy el hijo %d y mi padre es %d\n", getpid(), getppid());
            }
            else{
                c++;
                createP(n - 1,c);
                wait(NULL);
            }
        }
        else{
            if (Node == 0){
                sleep(1);
                c++;
                printf("Hola soy el hijo %d y mi padre es %d\n", getpid(), getppid());
                createP(n-1,c);
            }
            else{
                wait(NULL);
            }
        }
    }
}

int main(){
    int n;
    printf("Ingrese la cantidad de procesos a crear: ");
    scanf("%d", &n);
    createP(n,2);
    return 0;
}
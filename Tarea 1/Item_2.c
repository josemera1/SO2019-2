#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define chmax (1 << 4)
#define LSIZE (1 << 5)

void asamblea(int outfd)
{
    char line[LSIZE];
    memset(line, '-', LSIZE - 1);
    int option;
    while (1)
    {
        printf("%s\n", line);
        printf("Estimado alumno, porfavor seleccione una opcion:\n");
        printf("1] Mantenerse en las clases (escribir notas referentes a su clase)\n");
        printf("2] Ir a paro (cerrar el archivo correspondiente a los apuntes)\n");
        printf("3] Ir a toma (Utilizar archivos con sillas y armas WTF)\n");
        printf("o.c] Cerrar\n");
        printf("%s\n", line);
        printf("Considere que la opción 1] y 3] son modo ESCRITURA y LECTURA, respectivamente\n");
        scanf("%d", &option);
        if (option == 1)
        {
            printf("Usando syscall fork...\n");
            int pid = fork();
            if (pid < 0)
            {
                perror("ERROR EN LA BIFURCACION\n");
                printf("Usando syscall exit...\n");
                exit(-1);
            }
            else if (pid == 0)
            {
                printf("Hola, soy el proceso 'normal'\n");
                printf("%s\n", line);
                char str_input[chmax];
                printf("ESCRIBA SU APUNTE (NO MAS DE 16 CARACTERES)\n");
                scanf("%s", str_input);
                str_input[strlen(str_input) + 1] = '\0';
                str_input[strlen(str_input)] = '\n';
                printf("Ejecutando syscall write...\n");
                write(outfd, str_input, strlen(str_input));
                printf("SENTENCIA GUARDADA\n");
            }
            else
            {
                printf("Usando syscall wait...\n");
                wait(NULL);
            }
        }
        else if (option == 2)
        {
            printf("Usando syscall fork...\n");
            int pid = fork();
            if (pid < 0)
            {
                perror("ERROR EN LA BIFURCACION\n");
                printf("Usando syscall exit...\n");
                exit(-1);
            }
            else if (pid == 0)
            {
                printf("Hola, soy el proceso 'paro'\n");
                printf("%s\n", line);
                printf("Usando syscall close\n");
                printf("Cerrando archivo uwu\n");
                printf("Ejecutando syscall close...\n");
                close(outfd);
            }
            else
            {
                printf("Usando syscall wait...\n");
                wait(NULL);
            }
        }
        else if (option == 3)
        {
            printf("Usando syscall fork...\n");
            int pid = fork();
            if (pid < 0)
            {
                perror("ERROR EN LA BIFURCACION\n");
                printf("Usando syscall exit...\n");
                exit(-1);
            }
            else if (pid == 0)
            {
                printf("Hola, soy el proceso 'toma'\n");
                printf("%s\n", line);
                char *myargs[3];
                myargs[0] = strdup("cat");
                myargs[1] = strdup("toma.txt");
                myargs[2] = NULL;
                execvp(myargs[0], myargs);
            }
            else
            {
                printf("Usando syscall wait...\n");
                wait(NULL);
            }
        }
        else
        {
            printf("Asamblea finalizada...\n");
            break;
        }
    }
}

int main()
{
    printf("Hola, soy el proceso 'alumno', con pid %d\n", getpid());
    printf("AQUI SE ESTA DESARROLLANDO LA CLASE...\n");
    printf("Ejecutando syscall open...\n");
    int fd = open("letter.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (!fd)
    {
        perror("ERROR DE APERTURA\n");
        exit(1);
    }
    asamblea(fd);
}
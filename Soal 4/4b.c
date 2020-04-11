#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int matrix[4][5];

struct args {
    int x;
};

void* count(void *arg) {
    struct args* data;
    data = (struct args *) arg;
    int x = data->x;
    x = ((x * (x + 1)) / 2);
    printf("%d\t", x);

}

int main () {
    int baris = 4;
    int kolom = 5;
    int *matriks;
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*baris*kolom, IPC_CREAT | 0666);
    matriks = (int *)shmat(shmid, NULL, 0);
    printf("Matriks Perkalian\n");
    for (int i = 0; i < baris; i++){
        for (int j = 0; j < kolom; j++){
            printf("%d\t", matriks[i*kolom + j]);
            matrix[i][j] = matriks[i*kolom + j];
        }
        printf("\n");
    }

    printf("\nMatriks Penjumlahan\n");
    pthread_t tid[20];
    for (int i = 0; i < baris; i++) {
        for (int j = 0; j < kolom; j++){
            struct args* data = (struct args*)malloc(sizeof(struct args));
            data->x = matrix[i][j];
            pthread_create(&(tid[i * kolom + j]), NULL, count, (void*)data);
            pthread_join(tid[i * kolom + j], NULL);
        }
        printf("\n");
    }
    shmdt(matriks);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

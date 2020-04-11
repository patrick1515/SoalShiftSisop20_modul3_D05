#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

pthread_t tid[4];

int A[4][2] = { {1, 2},
                {3, 4},
                {5, 6},
                {7, 8} },
    B[2][5] = { {1, 3, 5, 7, 9},
                {2, 4, 6, 8, 10} },
    C[4][5];

void* count(void *arg) {
    pthread_t id = pthread_self();
    if (pthread_equal(id, tid[0])){     
        for (int i = 0; i < 5; i++) 
            C[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i];
    }
    else if (pthread_equal(id, tid[1])){
        for (int i = 0; i < 5; i++)
            C[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i];
    }
    else if (pthread_equal(id, tid[2])){
        for (int i = 0; i < 5; i++)
            C[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i];
    }
    else if (pthread_equal(id, tid[3])){ 
        for (int i = 0; i < 5; i++)
            C[3][i] = A[3][0]*B[0][i] + A[3][1]*B[1][i];
    }
}

int main () {
    for (int i = 0; i < 4; i++) {
        int err = pthread_create(&(tid[i]), NULL, &count, NULL);
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    int baris = 4;
    int kolom = 5;
    int *matriks;
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*baris*kolom, IPC_CREAT| 0666);
    matriks = (int *)shmat(shmid, NULL, 0);
    for (int i = 0; i < baris; i++){
        for (int j = 0; j < kolom; j++){
            matriks[i*kolom + j] = C[i][j];
            printf("%d\t", matriks[i*kolom + j]);
        }
        printf("\n");   
    }
    sleep(10);
    shmdt(matriks);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

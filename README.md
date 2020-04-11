# SoalShiftSISOP20_modul3_D05
## Kelompok D05
* 05111840000085 - Imanuel Banifernando Simatupang 
* 05111840000098 - Patrick Cipta Winata
### 1. Soal Nomor 1 
## 4a
a. Buatlah program untuk melakukan perkalian matriks. Ukuran matriks pertama adalah 4x2, dan matriks kedua 2x5. Isi dari matriks didefinisikan di dalam kodingan. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka). 
b. Tampilkan matriks hasil perkalian tadi ke layar. 

Deklarasi matriks beserta ukurannya
```
    int A[4][2] = { {1, 2},
                {3, 4},
                {5, 6},
                {7, 8} },
    B[2][5] = { {1, 3, 5, 7, 9},
                {2, 4, 6, 8, 10} },
    C[4][5];
```
Lalu, membuat thread untuk melakukan perkalian matriks dan kemudian dimasukkan ke dalam suatu shared memory agar bisa kembali diakses di 4b.
```
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
```
Untuk menampilkan hasil matriks perkalian menggunakan code di bawah ini
``` 
    for (int i = 0; i < baris; i++){
        for (int j = 0; j < kolom; j++){
            matriks[i*kolom + j] = C[i][j];
            printf("%d\t", matriks[i*kolom + j]);
        }
        printf("\n");   
    }
```
Dan yang terakhir untuk detach segment dari data menggunakan shmdt dan untuk mengubah informasi tentang shared memory menggunakan shmctl
```
   shmdt(matriks);
   shmctl(shmid, IPC_RMID, NULL);
```
## 4b
a. Buatlah program yang akan mengambil variabel hasil perkalian matriks dari program "4a.c" (program sebelumnya), dan tampilkan hasil matriks tersebut ke layar. 
b. Setelah ditampilkan, berikutnya untuk setiap angka dari matriks tersebut, carilah nilai faktorialnya, dan tampilkan hasilnya ke layar dengan format seperti matriks. 

Menggunakan deret geometri untuk mendapatkan matriks penjumlahan
 ```
    void* count(void *arg) {
    struct args* data;
    data = (struct args *) arg;
    int x = data->x;
    x = ((x * (x + 1)) / 2);
    printf("%d\t", x);
}
```
Lalu, untuk menampilkan hasil matriks perkalian dari 4a dan hasil matriks penjumlahan menggunakan code berikut
``` 
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
```
## 4c
Pada program ini, Norland diminta mengetahui jumlah file dan folder di direktori saat ini dengan command "ls | wc -l". dan harus menggunakan IPC Pipes
Pada soal ini, menggunakan 2 pipes dan menggunakan fork.
```  
    int fd1[2];  
	int fd2[2]; 
	pid_t p; 
    int status;
	if (pipe(fd1)==-1){ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 
	if (pipe(fd2)==-1){ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 
	p = fork();
```
Pipe pertama adalah fd1 dan pipe kedua adalah fd2. Ketika fork dapat dilakukan dan membuat child, maka pipe pertama akan diduplikasi dan pipe pertama tersebut akan ditutup. Lalu, program akan mengeksekusi command ls. 
Ketika child selesai melakukan proses, program menjalankan proses parentnya. Program akan menutup pipe kedua, pipe pertama akan diduplikasi, pipe kedua akan diduplikasi dan pipe pertama akan ditutup. Setelah itu, program akan mengeksekusi command wc -l.
```
   p = fork(); 
	if (p < 0){ 
		fprintf(stderr, "Fork Failed" ); 
		return 1; 
	}  
	else if (p == 0){ 
        dup2(fd1[1], 1);
        close(fd1[0]);
        close(fd1[1]);
        char *argm1[] = {"ls", NULL};
        execv("/bin/ls", argm1);
	} 
  else{
        close(fd2[0]);
        close(fd2[1]);
        dup2(fd1[0], 0);
        dup2(fd2[1], 1);
        close(fd1[0]);
        close(fd1[1]);
        char *argm2[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argm2);
  }
```
   





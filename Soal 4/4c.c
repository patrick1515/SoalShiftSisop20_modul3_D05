#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{
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
} 

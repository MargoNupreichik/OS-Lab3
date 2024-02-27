#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct {
	int flag;
	char symb;
}targs;

int fd[2];
int size_buff = 1024;

void* process_writer(void *arg){
	printf("\nthread1 started working\n");
	targs *args = (targs*) arg;
	
	// create buffer
	char buff[size_buff];
	rlimit rlim;
	int counter = 0;
	while(args->flag == 0) {
		getrlimit(RLIMIT_CPU, &rlim);
		strcpy(buff, (std::__cxx11::to_string(rlim.rlim_max)).c_str());
		write(fd[1], &buff, counter);
		counter++;
		if (counter >= size_buff) counter = 0;
		sleep(1);
		strcpy(buff, "\0");	
	}
	
	pthread_exit((void*)1);
}

void* process_reader(void *arg){
	printf("\nthread2 started working\n");
	targs *args = (targs*) arg;
	
	char* buffer;
	errno = 0;
	
	int res_read;
	
	while(args->flag == 0) {
		res_read = read(fd[0], &buffer, size_buff);
		if (res_read == 0) {
			printf("Reader: No data or end of file\n");
		}
		if (res_read == -1) {
			printf("Reader: error - %s\n", strerror(errno));
		}
		if (res_read > 0){
			printf("Reader: %c\n", buffer);
		}
		sleep(1);	
	}	
	
	pthread_exit((void*)2);
}

int main() {
	printf("programm started working\n");
	
	targs arg1; targs arg2;
	arg1.flag = 0; arg2.flag = 0;
	arg1.symb = '1'; arg2.symb = '2';
	
	pthread_t id1;
	pthread_t id2;
	
	pipe(fd);
	
 	pthread_create(&id1, NULL, process_writer, &arg1);
 	pthread_create(&id2, NULL, process_reader, &arg2);
 	printf("\nprogramm waits for keystroke\n");
 	getchar();
 	arg1.flag = 1; arg2.flag = 1;
 	
 	int* exictode1, exitcode2;
 	
 	pthread_join(id1, (void**)&exictode1);
 	pthread_join(id2, (void**)&exitcode2);
 	
 	close(fd[0]);
 	close(fd[1]);
 	
 	printf("programm ended working\n");
	return 0;
}

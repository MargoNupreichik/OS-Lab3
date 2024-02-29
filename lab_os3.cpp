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
int size_buff = 512;

void* process_writer(void *arg){
	printf("\nthread1 started working\n");
	targs *args = (targs*) arg;
	
	// create buffer
	char buff[size_buff];
	
	rlimit rlim;
	
	while(args->flag == 0) {
		memset(buff, 0, size_buff);
		getrlimit(RLIMIT_AS, &rlim);
		size_t counter = (size_t)(sprintf(buff, "%ju", (uintmax_t)rlim.rlim_cur));
		// sprintf(buff, "%li", rlim.rlim_max);
		int rv = write(fd[1], buff, counter);
		if (rv == -1){
			printf("Writer: error %s\n", strerror(rv));
		} else {
			printf("Writer: %s\n", buff);
		}
		sleep(1);
	}
	
	pthread_exit((void*)1);
}

void* process_reader(void *arg){
	printf("\nthread2 started working\n");
	targs *args = (targs*) arg;
	
	char buffer[size_buff];
	errno = 0;
	
	int rv;
	
	while(args->flag == 0) {
		memset(buffer, 0, size_buff);
		rv = read(fd[0], buffer, (size_t)size_buff);
		if (rv == 0) {
			printf("Reader: No data or end of file\n");
		}
		if (rv == -1) {
			printf("Reader: error - %s\n", strerror(errno));
		}
		if (rv > 0){
			printf("Reader: %s\n", buffer);
		}
		sleep(1);	
	}	
	pthread_exit((void*)2);
}

int main(int argc, char* argv[]) {
	printf("programm started working\n");
	
	targs arg1; targs arg2;
	arg1.flag = 0; arg2.flag = 0;
	arg1.symb = '1'; arg2.symb = '2';
	
	int rv;
	
	if (strcmp(argv[1], "1\0") == 0){
		rv = pipe(fd);
	}
	else if (strcmp(argv[1], "2\0") == 0){
		pipe2(fd, O_NONBLOCK);
	}
	else if (strcmp(argv[1], "3\0") == 0){
		rv = pipe(fd);
		fcntl(fd[0], F_SETFL, O_NONBLOCK);
		fcntl(fd[1], F_SETFL, O_NONBLOCK);
	}
	else return 0;
	
	pthread_t id1;
	pthread_t id2;
	
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

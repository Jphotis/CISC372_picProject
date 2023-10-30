#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;

// Thread method
void* Hello(void* rank){
	printf("Hello threaded world! I am thread %ld of %d\n", (long)rank, thread_count);
	return NULL;
}

int main(int argc,char* argv[]){

	long thread;				// Thread number
	pthread_t* thread_handles;	// Thread handles

	thread_count=strtol(argv[1],NULL,10);
	thread_handles=(pthread_t*)malloc(thread_count*sizeof(pthread_t));

	printf("%s %s\n", argv[0], argv[1]);	// Why doesn't this work?
	
	// Creates threads and has each thread run Hello	
	for (thread=0; thread<thread_count; thread++)
		pthread_create(&thread_handles[thread],NULL,&Hello, (void*)thread);
	
	// Only done by the parent thread
	printf("Hello from the parent thread\n");

	// Joins all threads
	for (thread=0;thread<thread_count;thread++)
		pthread_join(thread_handles[thread],NULL);

	// Frees all memory from threads
	free(thread_handles); 
	return 0;
}

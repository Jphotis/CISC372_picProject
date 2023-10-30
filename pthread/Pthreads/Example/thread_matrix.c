#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

// The threaded version of the program

// The object that goes inside of thread call
struct threadData{
	int* array;
	int rank;
};
	
void* Add_element(void * data) {		
	struct threadData* dp=(struct threadData*)data;
	for (int i = 0; i < 100000000; i++) {
			dp->array[(long)dp->rank]++;	
	}
	return NULL;	
}

int main(int argc, char* argv[]) {

	int thread_count = 9;	
	long t1,t2;             
    t1=time(NULL);	
	
	long thread;
	pthread_t* thread_handles;
	thread_handles=(pthread_t*)malloc(thread_count*sizeof(pthread_t));
	
			
	int arr[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	for (thread=0; thread<thread_count; thread++) {
		struct threadData data;
		data.array=arr;
		data.rank=thread;
		pthread_create(&thread_handles[thread],NULL,&Add_element, (void*)&data);	
	}
	
	for(thread=0; thread<thread_count; thread++) {
		pthread_join(thread_handles[thread],NULL);
	}

	for (int k = 0; k < 9; k++) {
		printf("%d ", arr[k]);
	}
	printf("\n");
	
	
	t2=time(NULL);
	printf("Took %ld seconds\n",t2-t1);

	free(thread_handles);
	return 0;
}

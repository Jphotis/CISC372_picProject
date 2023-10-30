#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>





typedef struct {
	int data1;
	int data2;
} Mem;



int main() {


	// Scalar	
	int *scalar = (int*)malloc(sizeof(int));
	*scalar = 3;
	printf("%d\n", *scalar);	
	printf("%p\n", scalar);	
	free(scalar);
	printf("\n");
	
	// 1D Array	
	int *arr1d = (int*)malloc(5 * sizeof(int));
	int arr1d_size = 5;
	
	for (int i = 0; i < arr1d_size; i++) {
		arr1d[i] = 2*i;
		printf("%d ", arr1d[i]);
	}
	printf("\n");
	printf("%p\n", arr1d);
	free(arr1d);
	printf("\n");

	
	// Pointer to array of pointers to structs

	int thread_count = 5;
	Mem * data[thread_count];
	Mem *(*p)[] = &data;	

	for (int thread = 0; thread<thread_count; thread++)	{
		data[thread] = (Mem *) malloc(sizeof(Mem));
		(*p)[thread]->data1 = thread * 5;
		(*p)[thread]->data2 = thread * 5 + 1;
		printf("%d: %d %d\n", thread, (*p)[thread]->data1, (*p)[thread]->data2);
	}
	
	for (int thread = 0; thread<thread_count; thread++) {
		free(data[thread]);
	}


	return 0;

}

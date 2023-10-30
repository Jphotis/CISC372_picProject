#include <stdio.h>
#include <time.h>

// The serial version of the program




	




int main() {



	long t1,t2;             
    t1=time(NULL);	
	
	int arr[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	

	for (int k = 0; k < 9; k++) {
		for (int i = 0; i < 100000000; i++) {
			arr[k]++;
		}
	}		

	for (int k = 0; k < 9; k++) {
		printf("%d ", arr[k]);
	}
	printf("\n");			
	
	
	t2=time(NULL);
	printf("Took %ld seconds\n",t2-t1);


	return 0;
}

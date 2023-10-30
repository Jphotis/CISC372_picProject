#include <stdio.h>


typedef double Matrix[3][3];



int main() {

	

	double array2d[2][2] = {{3, 4}, {1, 2}};
	printf("%f ", array2d[0][0]);
	printf("%f ", array2d[0][1]);
	printf("%f ", array2d[1][0]);
	printf("%f ", array2d[1][1]);	

	Matrix algs[] = {
					{{0,-1,0},{-1,4,-1},{0,-1,0}},
					{{0,-1,0},{-1,5,-1},{0,-1,0}},
					{{1/9.0,1/9.0,1/9.0},{1/9.0,1/9.0,1/9.0},{1/9.0,1/9.0,1/9.0}}      // Box blur	
	};


	return 0;
}

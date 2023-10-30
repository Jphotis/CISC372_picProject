#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//An array of kernel matrices to be used for image convolution.  
//The indexes of these match the enumeration from the header file. ie. algorithms[BLUR] returns the kernel corresponding to a box blur.
Matrix algorithms[]={									// Image processing kernal matrices:
    {{0,-1,0},{-1,4,-1},{0,-1,0}},										// Edge detection
    {{0,-1,0},{-1,5,-1},{0,-1,0}},										// Sharpen
    {{1/9.0,1/9.0,1/9.0},{1/9.0,1/9.0,1/9.0},{1/9.0,1/9.0,1/9.0}},		// Box blur
    {{1.0/16,1.0/8,1.0/16},{1.0/8,1.0/4,1.0/8},{1.0/16,1.0/8,1.0/16}},	// Gaussian blur
    {{-2,-1,0},{-1,1,1},{0,1,2}},										// Emboss
    {{0,0,0},{0,1,0},{0,0,0}}											// Identity
};

// The object that will go inside of each pthread call
typedef struct {
	Image* srcImage;
	Image* destImage;
	Matrix algorithm;
	int start;
	int end;
	long thread_num;
} threadData;





//getPixelValue - Computes the value of a specific pixel on a specific channel using the selected convolution kernel
//Paramters: srcImage:  An Image struct populated with the image being convoluted
//           x: The x coordinate of the pixel
//          y: The y coordinate of the pixel
//          bit: The color channel being manipulated
//          algorithm: The 3x3 kernel matrix to use for the convolution
//Returns: The new value for this x,y pixel and bit channel
uint8_t getPixelValue(Image* srcImage,int x,int y,int bit,Matrix algorithm){
    int px,mx,py,my,i,span;
    span=srcImage->width*srcImage->bpp;
    // for the edge pixes, just reuse the edge pixel
    px=x+1; py=y+1; mx=x-1; my=y-1;
    if (mx<0) mx=0;
    if (my<0) my=0;
    if (px>=srcImage->width) px=srcImage->width-1;
    if (py>=srcImage->height) py=srcImage->height-1;
    uint8_t result=
        algorithm[0][0]*srcImage->data[Index(mx,my,srcImage->width,bit,srcImage->bpp)]+
        algorithm[0][1]*srcImage->data[Index(x,my,srcImage->width,bit,srcImage->bpp)]+
        algorithm[0][2]*srcImage->data[Index(px,my,srcImage->width,bit,srcImage->bpp)]+
        algorithm[1][0]*srcImage->data[Index(mx,y,srcImage->width,bit,srcImage->bpp)]+
        algorithm[1][1]*srcImage->data[Index(x,y,srcImage->width,bit,srcImage->bpp)]+
        algorithm[1][2]*srcImage->data[Index(px,y,srcImage->width,bit,srcImage->bpp)]+
        algorithm[2][0]*srcImage->data[Index(mx,py,srcImage->width,bit,srcImage->bpp)]+
        algorithm[2][1]*srcImage->data[Index(x,py,srcImage->width,bit,srcImage->bpp)]+
        algorithm[2][2]*srcImage->data[Index(px,py,srcImage->width,bit,srcImage->bpp)];
    return result;
}

//convolute:  Applies a kernel matrix to an image
//Parameters: srcImage: The image being convoluted
//            destImage: A pointer to a  pre-allocated (including space for the pixel array) structure to receive the convoluted image.  It should be the same size as srcImage
//            algorithm: The kernel matrix to use for the convolution
//Returns: Nothing
void serial_convolute(Image* srcImage,Image* destImage,Matrix algorithm){
    int row,pix,bit,span;
    span=srcImage->bpp*srcImage->bpp;

    for (row=0;row<srcImage->height;row++){
        for (pix=0;pix<srcImage->width;pix++){
            for (bit=0;bit<srcImage->bpp;bit++){
                destImage->data[Index(pix,row,srcImage->width,bit,srcImage->bpp)]=getPixelValue(srcImage,pix,row,bit,algorithm);

            }
        }
    }
}

// Threaded Convolute
void* convolute(void * data){
    int row,pix,bit,span;

	threadData* dp = (threadData*)data;
	
	Image* srcImage = dp->srcImage;
	Image* destImage = dp->destImage;
	Matrix algorithm;
	algorithm[0][0] = dp->algorithm[0][0];
    algorithm[0][1] = dp->algorithm[0][1];
    algorithm[0][2] = dp->algorithm[0][2];
    algorithm[1][0] = dp->algorithm[1][0];
    algorithm[1][1] = dp->algorithm[1][1];
    algorithm[1][2] = dp->algorithm[1][2];
    algorithm[2][0] = dp->algorithm[2][0];
    algorithm[2][1] = dp->algorithm[2][1];
    algorithm[2][2] = dp->algorithm[2][2];
	int start = dp->start;
	int end = dp->end;

	long thread_num1 = dp->thread_num;

	printf("Hello. Thread %ld begins.\n", thread_num1);

/*		
struct threadData {
    	Image* srcImage;
    	Image* destImage;
    	Matrix algorithm;
    	int start;
    	int end;
  	};	
*/
    for (row = start ;row < end;row++){
        for (pix=0;pix<srcImage->width;pix++){
            for (bit=0;bit<srcImage->bpp;bit++){
                destImage->data[Index(pix,row,srcImage->width,bit,srcImage->bpp)]=getPixelValue(srcImage,pix,row,bit,algorithm);

            }
        }
    }
	
	printf("Goodbye. Thread %ld ends.\n", thread_num1);
	
	return NULL;
}


//Usage: Prints usage information for the program
//Returns: -1
int Usage(){
	printf("Usage: image <filename> <type> \n\twhere type is one of (edge,sharpen,blur,gauss,emboss,identity). \n\n");
    return -1;
}

//GetKernelType: Converts the string name of a convolution into a value from the KernelTypes enumeration
//Parameters: type: A string representation of the type
//Returns: an appropriate entry from the KernelTypes enumeration, defaults to IDENTITY, which does nothing but copy the image.
enum KernelTypes GetKernelType(char* type){
    if (!strcmp(type,"edge")) return EDGE;
    else if (!strcmp(type,"sharpen")) return SHARPEN;
    else if (!strcmp(type,"blur")) return BLUR;
    else if (!strcmp(type,"gauss")) return GAUSE_BLUR;
    else if (!strcmp(type,"emboss")) return EMBOSS;
    else return IDENTITY;
}

//main:
//argv is expected to take 2 arguments.  First is the source file name (can be jpg, png, bmp, tga).  Second is the lower case name of the algorithm.
int main(int argc,char* argv[]){
    long t1,t2;					// Statistics
    t1=time(NULL);

    if (argc!=3)			// Prints usage if invalid input
		 return Usage();

	// Initialize variables needed for threads	
	int thread_count;	
	long thread;
	pthread_t* thread_handles;
	thread_count = 4;		// Thread count is always 4 for Darwin
	thread_handles=(pthread_t*)malloc(thread_count*sizeof(pthread_t));

	
	

    stbi_set_flip_vertically_on_load(0);		 

    char* fileName=argv[1];										// Receives command line arguments
    if (!strcmp(argv[1],"pic4.jpg")&&!strcmp(argv[2],"gauss")){			// Thanks Silber
        printf("You have applied a gaussian filter to Gauss which has caused a tear in the time-space continum.\n");
    }
    enum KernelTypes type=GetKernelType(argv[2]);

	// Defines the images
    Image srcImage,destImage,bwImage;	

	// Loads the input image
    srcImage.data=stbi_load(fileName,&srcImage.width,&srcImage.height,&srcImage.bpp,0);
    if (!srcImage.data){
        printf("Error loading file %s.\n",fileName);
        return -1;
    }
	
	// Initializes the destination image
    destImage.bpp=srcImage.bpp;
    destImage.height=srcImage.height;
    destImage.width=srcImage.width;
    destImage.data=malloc(sizeof(uint8_t)*destImage.width*destImage.bpp*destImage.height);


	/*
	struct threadData {
    	Image* srcImage;
    	Image* destImage;
    	Matrix algorithm;
    	int start;
    	int end;
		long thread_num;
  	};
	*/
	
	printf("%d\n", srcImage.height);
	

	threadData *data[thread_count];
	threadData *(*p)[] = &data;
		
	for (thread = 0; thread<thread_count; thread++) {
		
		// threadData *data = (threadData *) malloc(sizeof(threadData));
		data[thread] = (threadData *) malloc(sizeof(threadData));

		(*p)[thread]->srcImage = &srcImage;
		(*p)[thread]->destImage = &destImage;
		(*p)[thread]->algorithm[0][0] = algorithms[type][0][0];
        (*p)[thread]->algorithm[0][1] = algorithms[type][0][1];
        (*p)[thread]->algorithm[0][2] = algorithms[type][0][2];
        (*p)[thread]->algorithm[1][0] = algorithms[type][1][0];
        (*p)[thread]->algorithm[1][1] = algorithms[type][1][1];
        (*p)[thread]->algorithm[1][2] = algorithms[type][1][2];
        (*p)[thread]->algorithm[2][0] = algorithms[type][2][0];
        (*p)[thread]->algorithm[2][1] = algorithms[type][2][1];
        (*p)[thread]->algorithm[2][2] = algorithms[type][2][2];

		(*p)[thread]->start = thread * srcImage.height/ thread_count;
		(*p)[thread]->end = (thread + 1) * (srcImage.height / thread_count);
				
		(*p)[thread]->thread_num = thread;
		printf("Initializing thread %ld.\n", (*p)[thread]->thread_num);

		pthread_create(&thread_handles[thread],NULL,&convolute, (void*)data[thread]);
	}
	for (thread=0; thread<thread_count; thread++) {
		pthread_join(thread_handles[thread],NULL);
	}	
	
	for (thread=0; thread<thread_count; thread++) {
		free(data[thread]);
	}
	// Processes the destination image
    // serial_convolute(&srcImage,&destImage,algorithms[type]);
    stbi_write_png("output.png",destImage.width,destImage.height,destImage.bpp,destImage.data,destImage.bpp*destImage.width);
    stbi_image_free(srcImage.data);
    
    free(destImage.data);
    t2=time(NULL);
    printf("Took %ld seconds\n",t2-t1);		// Ends and displays the time


	free(thread_handles);

    return 0;
}




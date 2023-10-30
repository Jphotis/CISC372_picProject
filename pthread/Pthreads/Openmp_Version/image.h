#ifndef ___IMAGE
#define ___IMAGE
#include <stdint.h>

// A macro
#define Index(x,y,width,bit,bpp) y*width*bpp+bpp*x+bit

typedef struct{
    uint8_t* data;
    int width;
    int height;
    int bpp;
} Image;

enum KernelTypes{EDGE=0,SHARPEN=1,BLUR=2,GAUSE_BLUR=3,EMBOSS=4,IDENTITY=5};

typedef double Matrix[3][3];

// Function declarations
uint8_t getPixelValue(Image* srcImage,int x,int y,int bit,Matrix algorithm);
void convolute_serial(Image* srcImage,Image* destImage,Matrix algorithm);
void * convolute(void * data);
int Usage();
enum KernelTypes GetKernelType(char* type);

#endif

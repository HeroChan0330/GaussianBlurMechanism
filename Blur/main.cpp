#include <iostream>
#include "BMP.h"
#include <time.h>

using namespace std;

float*createGaussianKernel(int radius){
    float sigma=radius/3;
    float weightsSum=0;
    int weightsLen=radius*2+1;
    float *weights=new float[weightsLen*weightsLen];

    //printf("GaussianKernel:\n");
    for(int u=-radius;u<=radius;u++){ //row
        for(int v=-radius;v<=radius;v++){ //collum
            int i=u+radius,j=v+radius;
            weights[j*weightsLen+i]=1/(6.28*sigma*sigma)*exp((-u*u-v*v)/(2*sigma*sigma));
            weightsSum+=weights[j*weightsLen+i];
        }
    }
    for(int u=-radius;u<=radius;u++){ //row
        for(int v=-radius;v<=radius;v++){ //collum
            int i=u+radius,j=v+radius;
            weights[j*weightsLen+i]/=weightsSum;
            //printf("%f ",weights[j*weightsLen+i]);
        }
        //printf("\n");
    }
    return weights;

}

int main()
{
    BMP bmp=BMP::FromFile("1080P.bmp");

    printf("Begin\n");


    //均值模糊
    /*
    bmp.AverageBlur2(50);
    bmp.Save("1080POutputA.bmp");
    */

    //3次均值模糊模拟高斯模糊
    /*
    bmp.AverageBlur2(50);
    bmp.AverageBlur2(50);
    bmp.AverageBlur2(50);
    bmp.Save("1080POutputA2.bmp");
    */

    //高斯模糊
    /*
    float*weights=createGaussianKernel(50);
    bmp.GaussianBlur(weights,50);
    bmp.Save("1080POutputG.bmp");
    */

    printf("Finished\n");
    return 0;
}

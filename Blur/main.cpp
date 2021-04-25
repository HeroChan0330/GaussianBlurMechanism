#include <iostream>
#include "BMP.h"
#include <time.h>

using namespace std;

float*createGaussianKernel(int radius){
    float sigma=radius/2;
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
    printf("Begin\n");


    BMP bmp1=BMP::FromFile("1080P.bmp");
    //均值模糊，无优化
    uint32_t t=clock();
    bmp1.AverageBlur(10);
    bmp1.Save("1080POutputA1.bmp");
    cout<<"Time cost(AverageBlur):"<<clock()-t<<"ms"<<endl;


    BMP bmp2=BMP::FromFile("1080P.bmp");
    //均值模糊，行模糊+列模糊
    t=clock();
    bmp2.AverageBlur2(10);
    bmp2.Save("1080POutputA2.bmp");
    cout<<"Time cost(AverageBlur2):"<<clock()-t<<"ms"<<endl;

    BMP bmp3=BMP::FromFile("1080P.bmp");
    //均值模糊，累加优化
    t=clock();
    bmp3.AverageBlur3(10);
    bmp3.Save("1080POutputA3.bmp");
    cout<<"Time cost(AverageBlur3):"<<clock()-t<<"ms"<<endl;


    BMP bmp4=BMP::FromFile("1080P.bmp");
    //高斯模糊
    t=clock();
    float*weights=createGaussianKernel(10);
    bmp4.GaussianBlur(weights,10);
    bmp4.Save("1080POutputG.bmp");
    delete weights;
    cout<<"Time cost(GaussianBlur):"<<clock()-t<<"ms"<<endl;


    BMP bmp5=BMP::FromFile("1080P.bmp");
    //大半径高斯模糊
    t=clock();
    weights=createGaussianKernel(50);
    bmp4.GaussianBlur(weights,50);
    bmp4.Save("1080POutputG2.bmp");
    delete weights;
    cout<<"Time cost(GaussianBlur r=50):"<<clock()-t<<"ms"<<endl;

    BMP bmp6=BMP::FromFile("1080P.bmp");
    //多次优化均值模糊模拟高斯模糊
    t=clock();
    bmp3.AverageBlur3(50);
    bmp3.AverageBlur3(50);
    bmp3.AverageBlur3(50);
    bmp3.Save("1080POutputA4.bmp");
    cout<<"Time cost(AverageBlur3×3 r=50):"<<clock()-t<<"ms"<<endl;

    printf("Finished\n");
    return 0;
}

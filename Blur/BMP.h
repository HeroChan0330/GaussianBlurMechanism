#ifndef __BMP_H__
#define __BMP_H__

#include "BMPStructer.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>


class BMP{
public:
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    uint32_t stride;
    byte*content;
    BMP(uint32_t width,uint32_t height);
    BMP();
    ~BMP();
    static BMP FromFile(char*path);
    void Save(char*path);
    BMP Clone();

    void AverageBlur(int radius);//均值模糊

    void GaussianBlur(float*weights,int radius);
    void AverageBlur2(int radius);//优化均值模糊
private:


};

#endif // __BMP_H__

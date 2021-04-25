#include "BMP.h"

BMP::BMP(uint32_t width,uint32_t height){
    this->file.bfType=19778;
    this->file.bfSize=4;
    this->file.bfReserved1=0;
    this->file.bfReserved2=54;
    this->file.bfOffBits=977534976;//不知道这个值有什么含义，反正就是这样....
    this->info.biSize=40;
    this->info.biWidth=width;
    this->info.biHeight=height;
    this->info.biPlanes=1;
    this->info.biBitCount=24;
    this->info.biCompression=0;
    //result->info.biSizeImage=
    this->info.biXPelsPerMeter=144;
    this->info.biYPelsPerMeter=144;
    this->info.biClrUsed=0;
    this->info.biClrImportant=0;

    this->stride=(((this->info.biWidth*this->info.biBitCount)+31)>>5)<<2;
    this->info.biSizeImage=this->stride*this->info.biHeight;
    this->content=(byte*)malloc(this->info.biSizeImage);//分配内存
}
BMP::BMP(){
}
BMP::~BMP(){
    delete(this->content);
    this->content=NULL;
}
BMP BMP::FromFile(char*path){
    FILE*fp=fopen(path,"rb+");
    BMP result;
    fread(&result.file,1,14,fp);
    fread(&result.info,1,40,fp);
    result.content=(byte*)malloc(result.info.biSizeImage);//分配内存
    fread(result.content,result.info.biSizeImage,1,fp);
    fclose(fp);
    result.stride=(((result.info.biWidth*result.info.biBitCount)+31)>>5)<<2;

    return result;
}
void BMP::Save(char*path){
    FILE*fp=fopen(path,"wb+");
    fwrite(&this->file,1,14,fp);
    fwrite(&this->info,1,40,fp);
    fwrite(this->content,1,this->info.biSizeImage,fp);
    fclose(fp);
}

BMP BMP::Clone(){
    BMP result;
    memcpy(&result,this,sizeof(BMP));
    return result;
}


void BMP::AverageBlur(int radius){
    if(radius<=0){
        return;
    }
    byte*newContent=new byte[this->info.biSizeImage];//分配内存
    for(int y=0;y<info.biHeight;y++){
        for(int x=0;x<info.biWidth;x++){

            int cnt=0;
            int bSum=0,gSum=0,rSum=0;
            for(int j=-radius;j<=radius;j++){//垂直方向求和
                for(int i=-radius;i<=radius;i++){//水平方向求和
                    if(i+x>=0&&i+x<info.biWidth && y+j>=0&&y+j<info.biHeight){
                        cnt++;
                        bSum+=content[(y+j)*stride+3*(x+i)+0];
                        gSum+=content[(y+j)*stride+3*(x+i)+1];
                        rSum+=content[(y+j)*stride+3*(x+i)+2];
                    }
                }
            }
            newContent[y*stride+3*x+0]=bSum/cnt;
            newContent[y*stride+3*x+1]=gSum/cnt;
            newContent[y*stride+3*x+2]=rSum/cnt;

        }
    }
    delete this->content;
    this->content=newContent;
}

void BMP::AverageBlur2(int radius){
    if(radius<=0){
        return;
    }
    byte*newContent=new byte[this->info.biSizeImage];//分配内存
    byte*contentPtrN,*contentPtrO;
    //水平方向模糊
    for(int j=0;j<info.biHeight;j++){
        contentPtrO=&content[j*stride];
        contentPtrN=&newContent[j*stride];

        for(int i=0;i<radius;i++){
            int rSum=0,gSum=0,bSum=0;
            for(int u=-i;u<=radius;u++){
                bSum+=*(contentPtrO+u*3);
                gSum+=*(contentPtrO+u*3+1);
                rSum+=*(contentPtrO+u*3+2);
            }
            *(contentPtrN++)=bSum/(i+radius+1);
            *(contentPtrN++)=gSum/(i+radius+1);
            *(contentPtrN++)=rSum/(i+radius+1);

            contentPtrO+=3;
        }
        for(int i=radius;i<info.biWidth-radius;i++){
            int rSum=0,gSum=0,bSum=0;
            for(int u=-radius;u<=radius;u++){
                bSum+=*(contentPtrO+u*3);
                gSum+=*(contentPtrO+u*3+1);
                rSum+=*(contentPtrO+u*3+2);
            }
            *(contentPtrN++)=bSum/(2*radius+1);
            *(contentPtrN++)=gSum/(2*radius+1);
            *(contentPtrN++)=rSum/(2*radius+1);

            contentPtrO+=3;
        }
        for(int i=info.biWidth-radius;i<info.biWidth;i++){
            int rSum=0,gSum=0,bSum=0;
            int rLen=info.biWidth-i;
            for(int u=-radius;u<rLen;u++){
                //printf("FK?\n");
                bSum+=*(contentPtrO+u*3);
                gSum+=*(contentPtrO+u*3+1);
                rSum+=*(contentPtrO+u*3+2);
            }

            *(contentPtrN++)=bSum/(radius+rLen);
            *(contentPtrN++)=gSum/(radius+rLen);
            *(contentPtrN++)=rSum/(radius+rLen);

            contentPtrO+=3;
        }
    }

    //垂直方向模糊
    for(int i=0;i<info.biWidth;i++){
        contentPtrO=&content[3*i];
        contentPtrN=&newContent[3*i];

        for(int j=0;j<radius;j++){
            int rSum=0,gSum=0,bSum=0;
            for(int v=-j;v<=radius;v++){
                int s=v*stride;
                bSum+=*(contentPtrN+s);
                gSum+=*(contentPtrN+s+1);
                rSum+=*(contentPtrN+s+2);
            }
            *(contentPtrO)=bSum/(radius+1+j);
            *(contentPtrO+1)=gSum/(radius+1+j);
            *(contentPtrO+2)=rSum/(radius+1+j);
            contentPtrO+=stride;
            contentPtrN+=stride;
        }

        for(int j=radius;j<info.biHeight-radius;j++){
            int rSum=0,gSum=0,bSum=0;
            for(int v=-radius;v<=radius;v++){
                int s=v*stride;
                bSum+=*(contentPtrN+s);
                gSum+=*(contentPtrN+s+1);
                rSum+=*(contentPtrN+s+2);
            }
            *(contentPtrO)=bSum/(2*radius+1);
            *(contentPtrO+1)=gSum/(2*radius+1);
            *(contentPtrO+2)=rSum/(2*radius+1);
            contentPtrO+=stride;
            contentPtrN+=stride;
        }

        for(int j=info.biHeight-radius;j<info.biHeight;j++){
            int rSum=0,gSum=0,bSum=0;
            int cLen=info.biHeight-j;
            for(int v=-radius;v<cLen;v++){
                int s=v*stride;
                bSum+=*(contentPtrN+s);
                gSum+=*(contentPtrN+s+1);
                rSum+=*(contentPtrN+s+2);
            }
            *(contentPtrO)=bSum/(radius+cLen);
            *(contentPtrO+1)=gSum/(radius+cLen);
            *(contentPtrO+2)=rSum/(radius+cLen);
            contentPtrO+=stride;
            contentPtrN+=stride;
        }

    }
    delete newContent;
}

void BMP::AverageBlur3(int radius){
    if(radius<=0||radius>=info.biWidth||radius>=info.biHeight){
        return;
    }
    byte*newContent=new byte[this->info.biSizeImage];//分配内存
    byte*contentPtrN,*contentPtrO;
    int xStride=(2*radius+1)*3;
    int yStride=(2*radius+1)*stride;

    //水平方向模糊
    for(int j=0;j<info.biHeight;j++){
        contentPtrO=&content[j*stride];
        contentPtrN=&newContent[j*stride];
        int bSum=0,gSum=0,rSum=0;
        int cnt=0;
        for(int u=0;u<=radius;u++){
            bSum+=*(contentPtrO++);
            gSum+=*(contentPtrO++);
            rSum+=*(contentPtrO++);
            cnt++;
        }
        *(contentPtrN++)=bSum/cnt;
        *(contentPtrN++)=gSum/cnt;
        *(contentPtrN++)=rSum/cnt;

        for(int i=1;i<=radius;i++){
            bSum+=*(contentPtrO++);
            gSum+=*(contentPtrO++);
            rSum+=*(contentPtrO++);
            cnt++;
            *(contentPtrN++)=bSum/cnt;
            *(contentPtrN++)=gSum/cnt;
            *(contentPtrN++)=rSum/cnt;
        }

        for(int i=radius+1;i<info.biWidth-radius;i++){
            bSum=bSum+*(contentPtrO)-*(contentPtrO-xStride);
            contentPtrO++;
            gSum=gSum+*(contentPtrO)-*(contentPtrO-xStride);
            contentPtrO++;
            rSum=rSum+*(contentPtrO)-*(contentPtrO-xStride);
            contentPtrO++;
            *(contentPtrN++)=bSum/cnt;
            *(contentPtrN++)=gSum/cnt;
            *(contentPtrN++)=rSum/cnt;
        }
        for(int i=info.biWidth-radius;i<info.biWidth;i++){
            bSum-=*(contentPtrO-xStride);
            contentPtrO++;
            gSum-=*(contentPtrO-xStride);
            contentPtrO++;
            rSum-=*(contentPtrO-xStride);
            contentPtrO++;
            cnt--;
            *(contentPtrN++)=bSum/cnt;
            *(contentPtrN++)=gSum/cnt;
            *(contentPtrN++)=rSum/cnt;
        }

    }

    //垂直方向模糊
    for(int i=0;i<info.biWidth;i++){
        contentPtrO=&content[i*3];
        contentPtrN=&newContent[i*3];
        int bSum=0,gSum=0,rSum=0;
        int cnt=0;
        for(int v=0;v<=radius;v++){
            bSum+=*(contentPtrN);
            gSum+=*(contentPtrN+1);
            rSum+=*(contentPtrN+2);
            contentPtrN+=stride;
            cnt++;
        }
        *(contentPtrO)=bSum/cnt;
        *(contentPtrO+1)=gSum/cnt;
        *(contentPtrO+2)=rSum/cnt;
        contentPtrO+=stride;

        for(int j=1;j<=radius;j++){
            bSum+=*(contentPtrN);
            gSum+=*(contentPtrN+1);
            rSum+=*(contentPtrN+2);
            contentPtrN+=stride;
            cnt++;
            *(contentPtrO)=bSum/cnt;
            *(contentPtrO+1)=gSum/cnt;
            *(contentPtrO+2)=rSum/cnt;
            contentPtrO+=stride;
        }

        for(int j=radius+1;j<info.biHeight-radius;j++){
            bSum=bSum+*(contentPtrN)-*(contentPtrN-yStride);
            gSum=gSum+*(contentPtrN+1)-*(contentPtrN+1-yStride);
            rSum=rSum+*(contentPtrN+2)-*(contentPtrN+2-yStride);
            contentPtrN+=stride;
            *(contentPtrO)=bSum/cnt;
            *(contentPtrO+1)=gSum/cnt;
            *(contentPtrO+2)=rSum/cnt;
            contentPtrO+=stride;
        }
        for(int j=info.biHeight-radius;j<info.biHeight;j++){
            bSum-=*(contentPtrN-yStride);
            gSum-=*(contentPtrN+1-yStride);
            rSum-=*(contentPtrN+2-yStride);
            contentPtrN+=stride;
            cnt--;
            *(contentPtrO)=bSum/cnt;
            *(contentPtrO+1)=gSum/cnt;
            *(contentPtrO+2)=rSum/cnt;
            contentPtrO+=stride;
        }

    }
    delete newContent;
}




void BMP::GaussianBlur(float*weights,int radius){
    byte*newContent=new byte[this->info.biSizeImage];//分配内存
    //float weightsSum=0;

    byte*contentPtrO=content;
    byte*contentPtrN=newContent;
    float*weightPtr=weights;
    for(int j=0;j<info.biHeight;j++){
        contentPtrO=&content[j*stride];
        contentPtrN=&newContent[j*stride];

        for(int i=0;i<info.biWidth;i++){
            float rSum=0,gSum=0,bSum=0;
            weightPtr=weights;
            for(int u=-radius;u<=radius;u++){
                for(int v=-radius;v<=radius;v++){
                    if(u+i<0||u+i>=info.biWidth||v+j<0||v+j>=info.biHeight){
                        weightPtr++;
                        continue;
                    }
                    //weightsSum+=weights[weightIndex];
                    bSum+=*(contentPtrO+v*stride+3*u+0)*(*weightPtr);
                    gSum+=*(contentPtrO+v*stride+3*u+1)*(*weightPtr);
                    rSum+=*(contentPtrO+v*stride+3*u+2)*(*weightPtr);
                    weightPtr++;
                }
            }
            //bSum/=weightsSum;
            //gSum/=weightsSum;
            //rSum/=weightsSum;
            (*contentPtrN)=bSum;
            contentPtrN++;
            (*contentPtrN)=gSum;
            contentPtrN++;
            (*contentPtrN)=rSum;
            contentPtrN++;

            contentPtrO+=3;
        }
    }
    delete this->content;
    this->content=newContent;

    //delete weights;
}



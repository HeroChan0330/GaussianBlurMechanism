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
    int *temp=new int[this->info.biWidth*this->info.biHeight*3];
    int *cnts=new int[this->info.biWidth*this->info.biHeight];
    //int cnts=(2*radius+1)*(2*radius+1);
    int bLen=3*(2*radius+1);
    int rLen=3*info.biWidth;

    byte*contentPtrO,*contentPtrN;
    int*tempPtr,*cntsPtr;

    for(int y=0;y<info.biHeight;y++){
        tempPtr=&temp[y*info.biWidth*3];
        cntsPtr=&cnts[y*info.biWidth];
        contentPtrO=&content[y*stride];


        *(tempPtr)=0;
        *(tempPtr+1)=0;
        *(tempPtr+2)=0;
        for(int i=0;i<=radius;i++){
            *(tempPtr)+=*(contentPtrO++);
            *(tempPtr+1)+=*(contentPtrO++);
            *(tempPtr+2)+=*(contentPtrO++);
        }
        *cntsPtr=radius+1;
        tempPtr+=3;
        cntsPtr++;

        for(int x=1;x<info.biWidth;x++){
            if(x<=radius){
                (*tempPtr)=*(tempPtr-3)+*(contentPtrO++);
                tempPtr++;
                (*tempPtr)=*(tempPtr-3)+*(contentPtrO++);
                tempPtr++;
                (*tempPtr)=*(tempPtr-3)+*(contentPtrO++);
                tempPtr++;

                *cntsPtr=x+radius+1;
                cntsPtr++;
            }
            else if(x>radius&&x<info.biWidth-radius){
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3)+*contentPtrO;
                tempPtr++;contentPtrO++;
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3)+*contentPtrO;
                tempPtr++;contentPtrO++;
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3)+*contentPtrO;
                tempPtr++;contentPtrO++;
                *cntsPtr=2*radius+1;
                cntsPtr++;
            }else{
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3);
                tempPtr++;contentPtrO++;
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3);
                tempPtr++;contentPtrO++;
                (*tempPtr)=*(tempPtr-3)-*(contentPtrO-bLen-3);
                tempPtr++;contentPtrO++;
                *cntsPtr=radius+info.biWidth-x;
                cntsPtr++;
            }
        }
    }

    for(int y=0;y<info.biHeight;y++){
        tempPtr=&temp[y*rLen];
        cntsPtr=&cnts[y*info.biWidth];
        contentPtrO=&content[y*stride];
        contentPtrN=&newContent[y*stride];
        for(int x=0;x<info.biWidth;x++){
            int cnt=0;
            int bSum=0,gSum=0,rSum=0;
            for(int v=-radius;v<=radius;v++){
                if(y+v>=0&&y+v<info.biHeight){
                    cnt+=*(cntsPtr+v*info.biWidth);
                    bSum+=*(tempPtr+v*rLen);
                    gSum+=*(tempPtr+v*rLen+1);
                    rSum+=*(tempPtr+v*rLen+2);
                }
            }
            cntsPtr++;
            newContent[y*stride+3*x+0]=bSum/cnt;
            newContent[y*stride+3*x+1]=gSum/cnt;
            newContent[y*stride+3*x+2]=rSum/cnt;
            tempPtr+=3;
        }
    }

    delete this->content;
    delete temp;
    //delete cnts;
    this->content=newContent;
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



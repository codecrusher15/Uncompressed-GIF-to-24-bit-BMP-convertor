//Headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_header.h"

//Function definitions

//This function reads the header of the given GIF file
void header(FILE* fp)
{
    //collecting signature(Always 'GIF')
    for(register int i=0;i<3;i++)
        GIF.HEADER.signature[i] = getc(fp);

    //collecting version( 87a / 89a)
    for(register int i=0;i<3;i++)
        GIF.HEADER.version[i] = getc(fp);
}
//This function reads the logical screen descriptor of GIF file
//Dimensions of canvas
//Packed field
//Background
//pixelapect ratio
void logical_screen_descriptor(FILE* fp)
{
    //Minnimum screen Dimensions
    for(register int i=0;i<2;i++)
        fread(&GIF.LSD.min_dimensions[i],sizeof(short int),1,fp);

    //packed feild
    //GIF.LSD.packedfield[i] represents the ith bit of packed field
    unsigned char a;
    fread(&a,sizeof(char),1,fp);
    for(register int i=0;i<8;i++,a=a/2)
        GIF.LSD.packedfield[i] = a%2;

    //Backgroundcolor
    //The 7th bit in packedfield represents global color flag
    //if the flag is 1 then background color exists else its set to zero
    //GIF.LSD.backgroundcolor[i] represents the ith bit of packed field
    unsigned char b;
    fread(&b,sizeof(char),1,fp);
    for(register int i=0;i<8;i++,b=b/2)
        GIF.LSD.backgroundcolor[i] = b%2;

    //pixelaspectratio
    //if the integer is n then aspect ratio is (n+15)/64 for n!=0
    unsigned char c;
    fread(&c,sizeof(char),1,fp);
    GIF.LSD.pixelaspect = (int)c;
}
//Every 3bytes represents a color 
//Size of GCT is mentioned in packed field in LSD
//Maximum size of global color table is 3*256 bytes
//It is an optional field
void global_color_table(FILE *fp)
{
    if(GIF.LSD.packedfield[7]!=0)
    {   
        //finding the entries in global color table
        int N = 0,size = 1,power=1;  
        for(register int i=0;i<3;i++)
        {
            N = N + power*(GIF.LSD.packedfield[i]);
            power = 2*power;
        }

        for(register int i=1;i<=N+1;i++)
            size = size*2;

        //collecting entries
        unsigned char a;
        for(register int i=0;i<size;i++)
        {
            for(register int j=0;j<3;j++)
            {
                fread(&a,sizeof(char),1,fp);
                GIF.GCT.entries[i][j] = a;
            }
        }
    }
}
//This function skips the extension block
//The order of contents is not sequential so we used an infinite loop to read the data
//The loop breaks when image seperator of Image descriptor is reached
void sort(FILE *fp)
{
    unsigned char a;
    fread(&a,sizeof(char),1,fp);
    // variables used inside the loop
    unsigned char b,ch[256];
    int n,i;short int j;

    while((int)a==33)
    {
        fread(&b,sizeof(char),1,fp);
        int n = (int)b;
        switch(n)
        {
            case 249 ://graphic extension block
        
                fread(&i,sizeof(int),1,fp);
                fread(&j,sizeof(short int),1,fp);
                break;

            case 255 ://Application extension

                fread(&b,sizeof(char),1,fp);
                n = (int)b;
                fread(&ch,n,1,fp);

                //data subblock
               while(1)
                {
                    int m;
                    unsigned char e;
                    fread(&e,sizeof(char),1,fp);
                    m = (int)e;
                    if(m==0)
                        break;
                    for(register int i=0;i<m;i++)
                        fread(&e,sizeof(char),1,fp);
                }
                break;

            case 254 ://comment extension

                while(1)
                    {
                        int m;
                        unsigned char e;
                        fread(&e,sizeof(char),1,fp);
                        m = (int)e;
                        if(m==0)
                            break;
                        for(register int i=0;i<m;i++)
                            fread(&e,sizeof(char),1,fp);
                    }
                break;

            case 1 ://plain text extension

                fread(&b,sizeof(char),1,fp);
                 n = (int)b;
                 fread(&ch,n,1,fp);

                 while(1)
                 {
                    int r;unsigned char s;
                    fread(&s,sizeof(char),1,fp);
                    r = (int)s;
                    if (r==0)
                        break;
                    for(register int i=0;i<r;i++)
                        fread(&s,sizeof(char),1,fp);
                 }
                break;
        }
        fread(&a,sizeof(char),1,fp);
    }
}
//This function reads the Image desciptor
//Image seperator
//Image location
//Image Dimensions
//packed field
void image_descriptor(FILE *fp)
{

    //Image location
    for(register int i=0;i<2;i++)
    {
        fread(&GIF.ID.img_loc[i],sizeof(short int),1,fp);
    }
    //Image size
    for(register int i=0;i<2;i++)
    {
        fread(&GIF.ID.img_size[i],sizeof(short int),1,fp);
    }
    //packed field GIF.ID.packedfield[j] represents jth bit of packed field
    unsigned char b;
    fread(&b,sizeof(char),1,fp);
    for(register int j=0;j<8;j++,b=b/2)
    {
        GIF.ID.packedfield[j]=(int)b%2;
    }
}
//Every 3bytes represents a color 
//Size of LCT is mentioned in packed field in image descriptor
//Maximum size of local color table is 3*256 bytes
//It is an optional field
void local_color_table(FILE *fp)
{
    if(GIF.ID.packedfield[7]!=0)
    {
        //finding the entries in local color table
        int N = 0,size = 1,power = 1;  
        for(register int i=0;i<3;i++)
        {
            N = N + power*(GIF.ID.packedfield[i]);
            power = power*2;
        }

        for(register int i=1;i<=N+1;i++)
            size = 2*size;

        //collecting entries
        unsigned char a;
        for(register int i=0;i<size;i++)
        {
            for(register int j=0;j<3;j++)
            {
                fread(&a,sizeof(char),1,fp);
                GIF.LCT.entries[i][j] = a;
            }
        }
    }
}
//Contains pixel data
//writes pixel data into BMP file
void image_data(FILE *fp,FILE *np)
{
    //LZW minimum code
    unsigned char a;
    fread(&a,sizeof(char),1,fp);
    int bits = (int)a + 1;

    //start code
    int size = 1;
    for(register int i=1;i<bits;i++)
        size = 2*size;

    //this the maximum size of code_stream in uncompressed gif
    int img_ht = GIF.ID.img_size[1],img_width = GIF.ID.img_size[0];
    int index[img_ht][img_width],row = 0,col = 0;
    int array[bits],fresh[8],count = 0;

    //Data sub-blocks
    while(1)
    {
        //Block introducer
        unsigned char b;int n;
        fread(&b,sizeof(char),1,fp);
        n = (int)b;

        //Block Terminator
        if(n==0)
            break;

        //Reading info in a sub-block
        for(register int i=0;i<n;i++)
        {
            unsigned char c;
            fread(&c,1,1,fp);

            for(register int j=0;j<8;j++,c=c/2)
                fresh[j] = c%2;

            int k=0;
            while(1)
            {
                for(;(count<bits)&&(k<8);k++,count++)
                    array[count] = fresh[k];

                //We can form the index when all the bits are in place
                if(count==bits)
                {
                    count = 0;
                    int num = 0,power = 1;
                    for(register int l=0;l<bits;l++,power=power*2)
                        num = num + power*(array[l]);
                    
                    if(col==img_width)
                    {
                        row = row + 1;
                        col = 0;
                    }

                    //Start and Stop codes are to be ignored
                    if(num<size)
                    {
                        index[row][col] = num;
                        col = col + 1;
                    }
                }

                //All the bits are here are used
                if(k==8)
                    break;
            }
        }
    }
    //Anti-interlacer
    anti_interlacer(img_ht,img_width,index);

    //sending pixel data
    pixel_make(np,img_width,index);
    
    //End
}
//Removes interlace
void anti_interlacer(int i,int j,int a[ ][j])
{
    if(GIF.ID.packedfield[6]!=0)
    {
        //copying a into b
        int b[i][j];
        for(register int l=0;l<i;l++)
        {
            for(register int m=0;m<j;m++)
            {
                b[l][m] = a[l][m];
            }
        }

        register int j=0;int k;
        //reversing pass 1
        for(register int l=0;l<i;l=l+8,j++)
        {
            for(register int m=0;m<j;m++)
            {
                a[l][m] = b[l/8][m];
            }
        }
        //reverting pass 2
        k = j;
        for(register int l=4;l<i;l=l+8,j++)
        {
            for(register int m=0;m<j;m++)
            {                    
                a[l][m] = b[l/8 +k][m];
            }
        }
        //reverting pass 3
        k = j;
        for(register int l=2;l<i;l=l+4,j++)
        {
            for(register int m=0;m<j;m++)
            {
                a[l][m] = b[l/4 +k][m];
            }
        }
        //reversing pass 4
        k = j;
        for(register int l=1;l<i;l=l+2,j++)
        {
            for(register int m=0;m<j;m++)
            {
                a[l][m] = b[l/2 +k][m];
            }
        }

    }
}
//It writes header and DIB header of the BMP file
void bmp_maker(FILE *np)
{
    BMP.BFH.name[0] = 'B';
    BMP.BFH.name[1] = 'M';
    int num = (get_imagewidth(GIF.ID.img_size[0]))*(GIF.ID.img_size[1]);
    BMP.BFH.size = 54 +3*num;
    BMP.BFH.unused = 0;
    BMP.BFH.ptr_bitmap = 54;
    //Making the header
    fwrite(&BMP.BFH.name[0],1,1,np);
    fwrite(&BMP.BFH.name[1],1,1,np);
    fwrite(&BMP.BFH.size,4,1,np);
    fwrite(&BMP.BFH.unused,4,1,np);
    fwrite(&BMP.BFH.ptr_bitmap,4,1,np);

    BMP.BMID.size = 40;
    BMP.BMID.width = GIF.ID.img_size[0] + GIF.ID.img_size[0]%4;
    BMP.BMID.height = GIF.ID.img_size[1];
    BMP.BMID.color_planes = 1;
    BMP.BMID.compression = 0;
    BMP.BMID.bits = 24;
    BMP.BMID.palette = 0;
    BMP.BMID.important_colors = 0;
    BMP.BMID.horizontal_pixel = (GIF.LSD.pixelaspect + 15)/64;
    BMP.BMID.vertical_pixel = (GIF.LSD.pixelaspect + 15)/64;
    BMP.BMID.size_raw_bitmapdata = 0;
    fwrite(&BMP.BMID.size,4,1,np);
    fwrite(&BMP.BMID.width,4,1,np);
    fwrite(&BMP.BMID.height,4,1,np);
    fwrite(&BMP.BMID.color_planes,2,1,np);
    fwrite(&BMP.BMID.bits,2,1,np);
    fwrite(&BMP.BMID.compression,4,1,np);
    fwrite(&BMP.BMID.size_raw_bitmapdata,4,1,np);
    fwrite(&BMP.BMID.horizontal_pixel,4,1,np);
    fwrite(&BMP.BMID.vertical_pixel,4,1,np);
    fwrite(&BMP.BMID.palette,4,1,np);
    fwrite(&BMP.BMID.important_colors,4,1,np);
}
//Finds the closest integer divisible by 4
int get_imagewidth(int n)
{
    while(n%4!=0)
    {
        n++;
    }
    return(n);
}
//writes pixel data into BMP file
void pixel_make(FILE *np,int j,int index[ ][j])
{
    int img_ht = GIF.ID.img_size[1],img_width = GIF.ID.img_size[0];
    for(register int l=img_ht-1;l>=0;l=l-1)
    {
        for(register int m=0;m<img_width;m++)
        {
            for(register int k=2;k>=0;k=k-1)
            {
                if(GIF.ID.packedfield[7])
                    fwrite(&GIF.LCT.entries[index[l][m]][k],1,1,np);
                else 
                    fwrite(&GIF.GCT.entries[index[l][m]][k],1,1,np);
                
            }
        }
        //Image width should always be a multiple of four
        for(register int m=img_width;m<get_imagewidth(img_width);m++)
        {
            unsigned char e=0;
            for(register int k=2;k>=0;k=k-1)
            {
                    fwrite(&e,1,1,np);
            }
        }
    }
}


//Prototypes
void header(FILE *fp);
void logical_screen_descriptor(FILE* fp);
void global_color_table(FILE *fp);
void sort(FILE *fp);
void application_extension(FILE *fp);
void comment_extension(FILE *fp);
void graphics_control_extension(FILE *fp);
void image_descriptor(FILE *fp);
void local_color_table(FILE *fp);
void image_data(FILE *fp,FILE *np);
void anti_interlacer(int i,int j,int a[ ][j]);
void bmp_maker(FILE *np);
void pixel_make(FILE *np,int j,int index[ ][j]);
int get_imagewidth(int n);

//Structures

//GIF format (important fields)
struct LocalColorTable
{
    unsigned char entries[256][3];//we may not use all of the entries
};
struct ImageDescriptor
{
   // unsigned char a;
    unsigned short int img_loc[2];
    unsigned short int img_size[2];
    int packedfield[8];
};
struct GlobalColorTable
{
    unsigned char entries[256][3];//we many not use all the entries though
};
struct LogicalScreenDescriptor
{
    unsigned short int min_dimensions[2];int packedfield[8];
    int backgroundcolor[8];int pixelaspect;
};
struct Header
{
    char signature[3];char version[3];
};
struct gif
{
    struct Header HEADER;struct LogicalScreenDescriptor LSD;struct GlobalColorTable GCT;
    struct ImageDescriptor ID;struct LocalColorTable LCT;
}GIF;

//BMP format
struct BMP_FILE_HEADER
{
	char name[2];	
	int size ;		//size of image
	int unused;
	int ptr_bitmap;		//offset for pixel data
};
struct BIT_MAP_INFO_HEADER
{
	int size;		//size of this header(40)
	int width;		//width of this image	
	int height;		//height of this image
	short color_planes;
	short bits;
	int compression;
	int size_raw_bitmapdata;
	int horizontal_pixel;
	int vertical_pixel;
	int palette;
	int important_colors;
};
struct BMP_DATA
{
	struct BMP_FILE_HEADER BFH;
	struct BIT_MAP_INFO_HEADER BMID;
}BMP;

//Declaring structues globally

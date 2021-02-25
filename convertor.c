//Header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_header.h"

//main function
int main()
{ 
    /*   FILE OPENING  */
    FILE *fp,*np;char fname[36];
    printf("File name:");
    fgets(fname,36,stdin);
    int len = strlen(fname);
    fname[len - 1] = '\0';


    //fp ---> GIF file
    //np ---> BMP file
    fp = fopen(fname,"r");
    np = fopen("output.BMP","wb");

    /*  FUNCTION CALLS  */
    header(fp);
    logical_screen_descriptor(fp);
    global_color_table(fp);
    sort(fp);
    image_descriptor(fp);
    local_color_table(fp);

    /*   WRITING DATA   */
    bmp_maker(np);
    image_data(fp,np);

    /* CLOSING FILES   */
    fclose(fp);
    fclose(np);

    return 0;
}
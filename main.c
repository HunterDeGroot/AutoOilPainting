#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//Print info about how to use the app
void usage(){
    printf( "Usage: progName <infile> <outfile> [parallel] [threshold] [filter]\n" );
    printf( "<infile> = Input bmp file\n" );
    printf( "<outfile> = Output bmp file\n" );
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        return 1;
    }
    
    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    
    Image *image = (Image *) malloc(sizeof(Image));
    if (image == NULL) {
        fprintf(stderr, "Out of memory\n");
        return(-1);
    }
    
    if (ImageLoad(inputFileName, image)==-1) {
        printf ("Can't read the image\n");
        return -1;
    }
    
    struct timeval startTimeOfDay, finishTimeOfDay;
    gettimeofday (&startTimeOfDay, NULL);
    
    printf("Apply Filter\n");
    
    // hard coding the radius to be five
    // bitmap rep of implementation
    
    //+5	. . . . 1 0 1 . . . .
    //+4	. . 3 2 1 0 1 2 3 . .
    //+3	. . 3 2 1 0 1 2 3 . .
    //+2	. 4 3 2 1 0 1 2 3 4 .
	//+1	5 4 3 2 1 0 1 2 3 4 5
	//		5 4 3 2 1 0 1 2 3 4 5
	//-1	5 4 3 2 1 0 1 2 3 4 5
	//-2	. 4 3 2 1 0 1 2 3 4 .
	//-3	. . 3 2 1 0 1 2 3 . .
	//-4	. . 3 2 1 0 1 2 3 . .
	//-5	. . . . 1 0 1 . . . .

    
    
    int x,y;
    // each pixel
    for (y=0;y<3*image->sizeY;y+=3) {
        for (x=0;x<3*image->sizeX;x+=3) {
        
        	int xd,yd;
        	int intensityCount[20] = { 0 };
        	int averageR[20] = { 0 };
			int averageB[20] = { 0 };
			int averageG[20] = { 0 }; 
			
			        	
        	// each row +5 thru -5
        	for (yd = y - 15; yd <= y + 15; yd+=3) { 
        		// each column that goes in circle radius 5
        		int limit;
        		switch(  yd - y ) 
				{
    				case -15:
        				limit = 1;
        				break;
    				case -12:
        				limit = 3;
        				break;
        			case -9:
        				limit = 3;
        				break;
        			case -6:
        				limit = 4;
        				break;
        			case 6:
        				limit = 4;
        				break;
        			case 9:
        				limit = 3;
        				break;
        			case 12:
        				limit = 3;
        				break;
        			case 15:
        				limit = 1;
        				break;
    				default :
       					limit = 5;
				}
        		for (xd = x - limit * 3; xd <= x + limit * 3; xd+=3){
        		
        			int i = yd*image->sizeX + xd;
        			if(i > 0 && i < 3*image->sizeY*image->sizeX + 3*image->sizeX){
        			
        				int r = image->data[i];
						int g = image->data[i+1];
						int b = image->data[i+2];
        			
        				int curIntensity = (int)((double)((r+g+b)/3)*20)/255.0f;
						intensityCount[curIntensity]++;
						averageR[curIntensity] += r;
						averageG[curIntensity] += g;
						averageB[curIntensity] += b;
					}
        		}
        	}	
       		int c;
       		int max = 1;
    		int index = 0;
    		for(c = 0; c < 20; c++){
        		if(max < intensityCount[c]){
        			max = intensityCount[c];
        			index = c;
        		}
        	}
         	int i = y*image->sizeX + x;
        	if(i > 0 && i < 3*image->sizeY*image->sizeX + 3*image->sizeX){
				image->data[i] = averageR[index]/max;
				image->data[i+1] = averageG[index]/max;
				image->data[i+2] = averageB[index]/max;
        	}
        }
    }
    
    if (ImageSave(outputFileName, image)==-1) return(-1);
    free(image);
    return 0;
}
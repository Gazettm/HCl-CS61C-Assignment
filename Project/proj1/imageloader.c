/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *fp = fopen(filename,"r");
	if (!fp)
	{
		perror("无法打开文件");
		return NULL;
	}
	char kind[4];
	int cols, rows, maxval;
	int ret = fscanf(fp, "%s %d %d %d", kind, &cols, &rows, &maxval);
	if(ret != 4)
	{
		printf("ppm头部格式错误\n");
		fclose(fp);
		return NULL;
	}
	Image *I = malloc(sizeof(Image));
	if (!I)
	{
		fclose(fp);
		return NULL;
	}
	I -> rows = rows;
	I -> cols = cols;
	I -> image = malloc(sizeof(Color *) * rows);
	if (!I->image)
	{
		free(I);
		fclose(fp);
		return NULL;
	}
	for(int i = 0;i < rows;i++){
		I -> image[i]  = malloc(sizeof(Color) * cols);
	}
	int R,G,B;
	for(int i = 0;i < rows;i++){
		for(int j = 0;j < cols;j++){
			fscanf(fp,"%d   %d   %d",&R,&G,&B);
			I -> image[i][j].R = R;
			I -> image[i][j].G = G;
			I -> image[i][j].B = B;
		}
	}
	fclose(fp);
	return I;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
#ifdef _WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	int rows = image -> rows;
	int cols = image -> cols;
	printf("P3\n%d %d\n255\n",cols,rows);
	for(int i = 0;i < rows;i++){
		for(int j = 0;j < cols;j++){
			Color pix = image ->image[i][j];
			printf("%3d %3d %3d", pix.R, pix.G, pix.B);
			if(j != cols - 1)printf("   ");
		}
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	if (image == NULL)
		return;
	int rows = image -> rows;
	for(int i = 0;i < rows;i++){
		free(image -> image[i]);
	}
	free(image -> image);
	free(image);
}

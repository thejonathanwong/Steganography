#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "tiffio.h"

#define TWIDTH 128 //tile width

int main(int argc, char * argv[]) {

	if(argc != 2) {
		fprintf(stderr, "USAGE: %s <TIFF Image File>\n", argv[0]);
		exit(1);
	}

	TIFF* tif = TIFFOpen(argv[1], "r");
	if(tif == NULL) {//ensures tif file can be opened
		perror("Unable to open file");
		exit(1);
	}

	//vars to read image
	uint32 imageLength;
	uint32 imageWidth;
	uint16 nsamples;
	uint16 bitspersample;
	uint16 planar;
	uint16 photo;

	//set vars for reading image
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
	tsize_t scanlineSize = TIFFScanlineSize(tif);

	//printf("%d %d\n", planar, photo);

	//set memory size of image
	tdata_t imageBuf;
	imageBuf = _TIFFmalloc(imageLength*scanlineSize);

	//loop to read TIFF image
	int i;
	for(i = 0; i < imageLength; i++) {
		TIFFReadScanline(tif, imageBuf+i*scanlineSize, i, 1);
	}
	//maximum number of complete tiles width and height wise
	int maxWTiles = imageWidth/TWIDTH;
	int maxHTiles = imageLength/TWIDTH;

	//build tile and sums all bits besides lowest bit of each pixel
	int j;
	int pval;
	int psum = 0;
	int maxVal = pow(2,bitspersample);
	int bitmask = maxVal-2;
	int tileR[TWIDTH*TWIDTH];
	for(j = 0; j < TWIDTH; j++) {
		for(i = 0; i < TWIDTH; i++) {
			pval = *((uint8 *) imageBuf + 3*i + j*scanlineSize);
			tileR[i + j*TWIDTH] = pval;
			psum += pval & bitmask;
		}
	}

	//stores sum in first lim pixels of tile
	int lim = log2(maxVal*TWIDTH*TWIDTH);
	int foundPSum = 0;
	for(i = 0; i < lim; i++) {
		foundPSum <<= 1;
		foundPSum += tileR[i]&1;
	}
	int slen = abs(psum - foundPSum)/2; //number of chars in the encoded string

	int originX = 0;
	int originY = 0;
	int colour = 0;
	int origin, cindex;

	char sbuf[slen + 1];
	int sindex;
	for(sindex = 0; sindex < slen; sindex++) {

		// vars to control origin of the tile in the original image
		colour = ((sindex+1)/maxWTiles) / maxHTiles; //determines the colour to be written to
		cindex = (sindex+1) % (maxWTiles*maxHTiles); //resets origin to 0,0 when switching colour
		originX = (3*TWIDTH)*((cindex)%maxWTiles);
		originY = (TWIDTH*scanlineSize) * ((cindex)/maxWTiles);

		// build tile and sums all bits besides lowest bit of each pixel 
		psum = 0;
		origin = colour + originX + originY;
		for(j = 0; j < TWIDTH; j++) {
			for(i = 0; i < TWIDTH; i++) {
				pval = *((uint8 *) imageBuf + origin + 3*i + j*scanlineSize);
				tileR[i + j*TWIDTH] = pval;
				psum += pval & bitmask;
			}
		}

		//loop to find the original psum that was encoded in the least significant bit of the 
		//first lim pixels
		foundPSum = 0;
//		char bitstring[lim+1];
		for(i = 0; i < lim; i++) {
			foundPSum <<= 1;
			foundPSum += tileR[i]&1;
//			bitstring[i] = (tileR[i]&1)+'0';
		}
//		bitstring[lim] = 0;
//		printf("bitstring = %s\n", bitstring);
		sbuf[sindex] = abs(psum - foundPSum)/2; //ignores whether the tile was modified via
												//addition or subtraction
	}
	sbuf[slen] = 0;

	printf("Decrypted string:\n%s", sbuf);

	_TIFFfree(imageBuf);

	TIFFClose(tif);
	return 0;
}



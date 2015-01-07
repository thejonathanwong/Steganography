#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "tiffio.h"

#define TWIDTH 128
#define SBUFSIZE 1000

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

	//seeds rand to choose random pixels in a tile
	srand(time(NULL));

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

//	printf("%d %d\n", planar, photo);

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
	int maxChars = 3 * maxWTiles * maxHTiles;

	//prompt asking for string to encode
	printf ( "Maximum number of chars that can be encoded in this image: %d\n" ,
			maxChars);
	printf("Enter in a string to encode:\n");
	char sbuf[SBUFSIZE];
	if(fgets(sbuf, SBUFSIZE, stdin) == NULL) {
		fprintf(stderr, "Unable to read string to be encoded.\n");
		exit(1);
	}
	int slen = strlen(sbuf);

	//NEED TO DOUBLE CHECK THIS VALUE
//	printf("maxnumchars = %d\n", maxWTiles*maxHTiles);
	if(slen > maxChars) {
		fprintf(stderr, "String is too long to encode for the image\n");
		exit(1);
	}


	printf("maxW = %d, maxH = %d\n", maxWTiles, maxHTiles);

	int j;
	int pval;
	int maxVal = pow(2,bitspersample);
	int bitmask = maxVal-2;
	int tileR[TWIDTH*TWIDTH];
	int lim = log2(maxVal*TWIDTH*TWIDTH);

	int originX = 0;
	int originY = 0;
	int colour = 0; 
	int origin, cindex;

	int sindex;
	for(sindex = 0; sindex <= slen; sindex ++) {

		int psum = 0;

		// vars to control origin of the tile in the original image
		cindex = sindex % (maxWTiles*maxHTiles); //resets origin to 0,0 when switching colour
		originX = (3*TWIDTH) * (cindex%maxWTiles); //x coordinate of origin
		originY = (TWIDTH*scanlineSize) * (cindex/maxWTiles); //y coordinate
		colour = (sindex/maxWTiles) / maxHTiles; //determines the colour to be written to
		
		// build tile and sums all bits besides lowest bit of each pixel 
		origin = colour + originX + originY;
		for(j = 0; j < TWIDTH; j++) {
			for(i = 0; i < TWIDTH; i++) {
				pval = *((uint8 *) imageBuf + origin + 3*i + j*scanlineSize); //val at the pixel
				tileR[i + j*TWIDTH] = pval; //sets tile to pixel value
				psum += pval & bitmask; //sums the bitmasked values
			}
		}
//		printf("origin = %d, psum = %d\n", origin, psum);

		//stores sum in first lim pixels of tile
		char bitstring[lim+1];
		for(i = 0; i < lim; i++) {
			tileR[lim - i - 1] &= bitmask;
			if((psum>>i)&1) {
				tileR[lim - i - 1] += 1;
			}
			bitstring[lim-i-1] = (tileR[lim-i-1]&1) + '0';//((psum>>i)&1) + '0';
//			printf("%d\n",tileR[lim-i]&1);
		}
		bitstring[lim] = 0;
//		printf("bitstring = %s\n",bitstring);

		//modifies tile pixel values
		int s;
		if(!sindex) {
			s = slen;
		} else {
			printf("sbuf = %d\n", sbuf[sindex-1]);
			s = sbuf[sindex-1];
		}
		int px;
		while(s) {
			px = rand()%(TWIDTH*TWIDTH);
			if(tileR[px] < (maxVal-2)) {
				if(psum > (TWIDTH*TWIDTH*(maxVal-16))) {
					tileR[px] -= 2;
				} else {
					tileR[px] += 2;
				}
				s--;
			}
		}

		int newsum = 0;
		for(j = 0; j < TWIDTH; j++) {
			for(i = 0; i < TWIDTH; i++) {
				newsum += tileR[i+j*TWIDTH]& bitmask;
			}
		}
		printf("newsum = %d\n", newsum);

		int putAvg = 0;
		char bitstring2[lim+1];
		for(i = 0; i < lim; i++) {
			putAvg <<=1;
			putAvg += tileR[i]&1;
			bitstring2[i] = ((tileR[i]&1)) + '0';
		}
		bitstring2[lim]=0;
//		printf("bitstring2 = %s\n", bitstring2);
		printf("putAvg = %d\n", putAvg);

		//loop to write tile back into imageBuf
		for(j = 0; j < TWIDTH; j++) {
			for(i = 0; i < TWIDTH; i++) {
				*((uint8 *) imageBuf + origin + 3*i + j*scanlineSize) = tileR[i + j*TWIDTH];
			}
		}
	}




	//opens output image
	TIFF* otif = TIFFOpen("out.tif", "w");
	if(tif == NULL) {//ensures tif file can be opened
		perror("Unable to open file");
		exit(1);
	}

	//set necessary fields
	TIFFSetField(otif, TIFFTAG_IMAGEWIDTH, imageWidth);
	TIFFSetField(otif, TIFFTAG_IMAGELENGTH, imageLength);
	TIFFSetField(otif, TIFFTAG_BITSPERSAMPLE, bitspersample);
	TIFFSetField(otif, TIFFTAG_SAMPLESPERPIXEL, nsamples);
	TIFFSetField(otif, TIFFTAG_PLANARCONFIG, planar);	
	TIFFSetField(otif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);	
	TIFFSetField(otif, TIFFTAG_PHOTOMETRIC, photo);
	TIFFSetField(otif, TIFFTAG_ROWSPERSTRIP, 1);	

	//loop to write output image
	for(i = 0; i < imageLength; i++) {
		TIFFWriteScanline(otif, imageBuf+i*scanlineSize, i, 1);
	}

	_TIFFfree(imageBuf);

	TIFFClose(otif);
	TIFFClose(tif);
	return 0;
}



//Axel 56130500236

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
	IplImage* img = 0;
	int height,width,step,channels;
	unsigned char *data;

	setbuf(stdout, NULL);
	// load an image
	img= cvLoadImage("retina.tiff");
	if(!img){
		printf("Could not load image file: %s\n",argv[1]);
		exit(0);
	}

	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	printf("Processing a %dx%d image with %d channels\n",height,width,channels);
	printf("step = %d\n", step);


	IplImage* imgRed = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGrayscale = cvCreateImage(cvGetSize(img), 8, 1);

	//change to grayscale
	cvCvtColor(img, imgGrayscale, CV_RGB2GRAY);

	// create a window
	cvNamedWindow("Gray scale", CV_WINDOW_AUTOSIZE);
	cvNamedWindow( "Pseudo color rainbow" );
	cvNamedWindow( "Pseudo color 1" );
	cvNamedWindow( "Pseudo color 2" );

	// Create an image for the output
	IplImage* imgPseudoColor = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
	IplImage* imgPseudoColor1 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
	IplImage* imgPseudoColor2 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );

	//Pseudo coloring
	unsigned char *dataGrayscale;
	unsigned char *dataRed, *dataGreen, *dataBlue;
	dataRed = (uchar *)imgRed->imageData;
	dataGreen = (uchar *)imgGreen->imageData;
	dataBlue = (uchar *)imgBlue->imageData;
	dataGrayscale = (uchar *)imgGrayscale->imageData;

	int i,j;
	uchar val;

	// PSEUDO COLOR RAINBOW
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			val = dataGrayscale[i*width+j];

			if(val<=64){
				dataRed[i*width+j]=0;
				dataGreen[i*width+j]=0;
				dataBlue[i*width+j]=val*4;
			}else if(val<=128){
				dataRed[i*width+j]=0;
				dataGreen[i*width+j]=(val-64)*4;
				dataBlue[i*width+j]=255;
			}else if(val<=192){
				dataRed[i*width+j]=0;
				dataGreen[i*width+j]=255;
				dataBlue[i*width+j]=(192-val)*4;
			}else{
				dataRed[i*width+j]=(val-192)*4;
				dataGreen[i*width+j]=(255-val)*4;
				dataBlue[i*width+j]=0;
			}
		}
	}

	// merge three planes back into one image
	cvMerge(imgBlue, imgGreen, imgRed, NULL, imgPseudoColor);

	// PSEUDO COLOR 1
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			val = dataGrayscale[i*width+j];

			dataRed[i*width+j]=val;
			dataGreen[i*width+j]=val;
			dataBlue[i*width+j]=255;
		}
	}

	// merge three planes back into one image
	cvMerge(imgBlue, imgGreen, imgRed, NULL, imgPseudoColor1);

	// PSEUDO COLOR 2
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			val = dataGrayscale[i*width+j];

			if(val>128){
				dataRed[i*width+j]=val;
				dataGreen[i*width+j]=510-2*val;
				dataBlue[i*width+j]=0;
			}else{
				dataRed[i*width+j]=0;
				dataGreen[i*width+j]=val;
				dataBlue[i*width+j]=255-2*val;
			}
		}

	}
	// merge three planes back into one image
	cvMerge(imgBlue, imgGreen, imgRed, NULL, imgPseudoColor2);

	// show the image
	cvShowImage("Gray scale", imgGrayscale );
	cvShowImage("Pseudo color rainbow", imgPseudoColor);
	cvShowImage("Pseudo color 1", imgPseudoColor1);
	cvShowImage("Pseudo color 2", imgPseudoColor2);

	// wait for a key
	cvWaitKey(0);

	// release the image
	cvReleaseImage(&imgGrayscale);
	cvReleaseImage(&imgPseudoColor);
	cvReleaseImage(&imgPseudoColor1);
	cvReleaseImage(&imgPseudoColor2);

	return 0;
}



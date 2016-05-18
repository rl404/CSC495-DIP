// Axel 56130500236

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>

using namespace cv;


// 3x3 Structuring element
/*
  	  | 1	1	1 |
	  |	1	1	1 |
  	  | 1	1	1 |
 */

int B[3][3]= {
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1}};

int _dilation_no = 0; // Initially, perform no dilation.
int _erosion_no = 0;
IplImage* img = 0;
IplImage* imgBW = 0;
IplImage* imgOutB = 0;
IplImage* imgOutD = 0;
IplImage* imgOutE = 0;


void doDilation(int iter_no1, int iter_no2, IplImage* imgBW, IplImage* imgOut) {
	int height, width, step;

	// get the image data
	height = imgBW->height;
	width = imgBW->width;
	step = imgBW->widthStep;

	// Split the 3-channel black&white image into 3 planes.
	IplImage* BChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* GChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* RChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.

	cvSplit(imgBW, BChannel, GChannel, RChannel, NULL);

	// Use one of the channel, say, B because the input image is a binary image.
	unsigned char *data;
	data = (uchar *) BChannel->imageData;

	unsigned char *dataOut;
	dataOut = (uchar *) imgOut->imageData;
	cvSetZero(imgOut); // Clear the output.

	int step_3 = step / 3;
	int i, j;
	int r, c;
	int sum = 0;

	// DILATION --------------------------------
	for (i = 1; i < height-1; i++) {
		for (j = 1; j < width-1; j++) {
			dataOut[i * step_3 + j] = data[i * step_3 + j];
		}
	}

	for(int cnt = iter_no1; cnt > 0; cnt-- ) {
		for (i = 1; i < height-1; i++) {
			for (j = 1; j < width-1; j++) {
				sum = 0;
				for (r = 0; r < 3; r++){
					for (c = 0; c < 3; c++){
						sum += data[(i+r-1) * step_3 + (j+c-1)] * B[r][c];
					}
				}
				// There are some hits.
				if (sum != 0){
					dataOut[i * step_3 + j] = 255;
				}
				else {
					dataOut[i * step_3 + j] = 0;
				}
			}
		}
		cvSetZero(BChannel);
		cvCopy(imgOut, BChannel);
	}

	cvShowImage("Dilation", imgOut); // Needed here.

	// release the images
	cvReleaseImage(&BChannel);
	cvReleaseImage(&GChannel);
	cvReleaseImage(&RChannel);

	return;
}

void doErosion(int iter_no1, int iter_no2, IplImage* imgBW, IplImage* imgOut) {
	int height, width, step;

	// get the image data
	height = imgBW->height;
	width = imgBW->width;
	step = imgBW->widthStep;

	// Split the 3-channel black&white image into 3 planes.
	IplImage* BChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* GChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* RChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.

	cvSplit(imgBW, BChannel, GChannel, RChannel, NULL);

	// Use one of the channel, say, B because the input image is a binary image.
	unsigned char *data;
	data = (uchar *) BChannel->imageData;

	unsigned char *dataOut;
	dataOut = (uchar *) imgOut->imageData;
	cvSetZero(imgOut); // Clear the output.

	int step_3 = step / 3;
	int i, j;
	int sum = 0;
	bool d[width*height];

	// EROSION ---------------------------------------
	for (i = 1; i < height-1; i++) {
		for (j = 1; j < width-1; j++) {
			dataOut[i * step_3 + j] = data[i * step_3 + j];
		}
	}

	for(int cnt = iter_no2; cnt > 0; cnt-- ) {
		for (i = 1; i < height-1; i++) {
			for (j = 1; j < width-1; j++) {
				d[i * step_3 + j] = false;
				sum = 0;

				if(dataOut[i * step_3 + j] != 0){
					if(dataOut[(i+1) * step_3 + j] != 0){sum++;}
					if(dataOut[(i-1) * step_3 + j] != 0){sum++;}
					if(dataOut[i * step_3 + (j+1)] != 0){sum++;}
					if(dataOut[i * step_3 + (j-1)] != 0){sum++;}

					if(dataOut[(i+1) * step_3 + (j+1)] != 0){sum++;}
					if(dataOut[(i-1) * step_3 + (j-1)] != 0){sum++;}
					if(dataOut[(i-1) * step_3 + (j+1)] != 0){sum++;}
					if(dataOut[(i+1) * step_3 + (j-1)] != 0){sum++;}
				}

				if(sum==8){
					d[i * step_3 + j] = true;
				}
			}
		}

		for (i = 0; i < height-1; i++) {
			for (j = 0; j < width-1; j++) {

				if(d[i * step_3 + j] == true){
					dataOut[i * step_3 + j] = 255;
				}else{
					dataOut[i * step_3 + j] = 0;
				}
			}
		}
		cvSetZero(BChannel);
		cvCopy(imgOut, BChannel);
	}

	cvShowImage("Erosion", imgOut); // Needed here.

	// release the images
	cvReleaseImage(&BChannel);
	cvReleaseImage(&GChannel);
	cvReleaseImage(&RChannel);

	return;
}

void boundary(IplImage* imgBW, IplImage* imgOut2){
	int height, width, step;

	// get the image data
	height = imgBW->height;
	width = imgBW->width;
	step = imgBW->widthStep;

	// Split the 3-channel black&white image into 3 planes.
	IplImage* BChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* GChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.
	IplImage* RChannel = cvCreateImage(cvGetSize(img), 8, 1); // One channel.

	cvSplit(imgBW, BChannel, GChannel, RChannel, NULL);

	// Use one of the channel, say, B because the input image is a binary image.
	unsigned char *data;
	data = (uchar *) BChannel->imageData;

	unsigned char *dataOut2;
	dataOut2 = (uchar *) imgOut2->imageData;
	cvSetZero(imgOut2); // Clear the output.

	int step_3 = step / 3;
	int i, j, r ,c;
	int sum = 0;
	bool d[width*height];

	// DILATION --------------------------------
	for (i = 1; i < height-1; i++) {
		for (j = 1; j < width-1; j++) {
			sum = 0;
			for (r = 0; r < 3; r++){
				for (c = 0; c < 3; c++){
					sum += data[(i+r-1) * step_3 + (j+c-1)] * B[r][c];
				}
			}
			// There are some hits.
			if (sum != 0){
				dataOut2[i * step_3 + j] = 255;
			}
			else {
				dataOut2[i * step_3 + j] = 0;
			}
		}
	}
	cvSetZero(BChannel);
	cvCopy(imgOut2, BChannel);

	// EROSION ---------------------------------------
	for (i = 1; i < height-1; i++) {
		for (j = 1; j < width-1; j++) {
			d[i * step_3 + j] = false;
			sum = 0;

			if(dataOut2[i * step_3 + j] != 0){
				if(dataOut2[(i+1) * step_3 + j] != 0){sum++;}
				if(dataOut2[(i-1) * step_3 + j] != 0){sum++;}
				if(dataOut2[i * step_3 + (j+1)] != 0){sum++;}
				if(dataOut2[i * step_3 + (j-1)] != 0){sum++;}

				if(dataOut2[(i+1) * step_3 + (j+1)] != 0){sum++;}
				if(dataOut2[(i-1) * step_3 + (j-1)] != 0){sum++;}
				if(dataOut2[(i-1) * step_3 + (j+1)] != 0){sum++;}
				if(dataOut2[(i+1) * step_3 + (j-1)] != 0){sum++;}
			}

			if(sum==8){
				d[i * step_3 + j] = true;
			}
		}
	}

	for (i = 0; i < height-1; i++) {
		for (j = 0; j < width-1; j++) {
			if(d[i * step_3 + j] == true){
				dataOut2[i * step_3 + j] = 0;
			}
		}
	}
	cvSetZero(BChannel);
	cvCopy(imgOut2, BChannel);

	cvShowImage("Boundary", imgOut2); // Needed here.

	// release the images
	cvReleaseImage(&BChannel);
	cvReleaseImage(&GChannel);
	cvReleaseImage(&RChannel);

	return;
}

void updateInteration(int arg, void*)
{
	doDilation(_dilation_no, _erosion_no, imgBW, imgOutD);
	doErosion(_dilation_no, _erosion_no, imgBW, imgOutE);
}

int main(int argc, char *argv[]) {
	int height, width, step, channels;
	unsigned char *data;

	setbuf(stdout, NULL);

	// load an image
	img = cvLoadImage("d.png");
	if (!img) {
		printf("Could not load image file: %s\n", argv[1]);
		exit(0);
	}

	// get the image data
	height = img->height;
	width = img->width;
	step = img->widthStep;
	channels = img->nChannels;
	data = (uchar *) img->imageData;

	printf("Processing a %dx%d image with %d channels\n", height, width,
			channels);
	printf("step = %d\n", step);

	// Create a binary image.
	imgBW = cvCreateImage(cvGetSize(img), 8, 3); // Three channels.
	cvThreshold(img, imgBW, 0, 255, CV_THRESH_BINARY);


	// Create an output image (binary image).
	imgOutD = cvCreateImage(cvGetSize(img), 8, 1);
	imgOutE = cvCreateImage(cvGetSize(img), 8, 1);
	imgOutB = cvCreateImage(cvGetSize(img), 8, 1);
	cvSetZero(imgOutB);
	cvSetZero(imgOutD);
	cvSetZero(imgOutE);

	// create a window
	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE); // Fixed size
	cvNamedWindow("Binary", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Dilation", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Erosion", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Boundary", CV_WINDOW_AUTOSIZE);

	// Morphological processing...
	createTrackbar("Dilation", "Dilation", &_dilation_no, 10, updateInteration);
	createTrackbar("Erosion", "Erosion", &_erosion_no, 10, updateInteration);
	updateInteration(0, 0);

	// show the image
	cvShowImage("Original", img);
	cvShowImage("Binary", imgBW);
	cvShowImage("Dilation", imgOutD);
	cvShowImage("Erosion", imgOutE);

	boundary(imgBW, imgOutB);

	// wait for a key
	cvWaitKey(0);

	// release the images
	cvReleaseImage(&img);
	cvReleaseImage(&imgBW);
	cvReleaseImage(&imgOutB);
	cvReleaseImage(&imgOutD);
	cvReleaseImage(&imgOutE);

	return 0;
}


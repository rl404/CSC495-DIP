// Axel 56130500236

#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>



void ordered_dithering(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;
	int th[4][4] = {{1,9,3,11},		//wiki
					{13,5,15,7},
					{4,12,2,10},
					{16,8,14,6}}

					/*{{0,8,2,10},	//slide
					{12,4,14,6},
					{3,11,1,9},
					{15,7,13,5}}*/;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){

			int tx = i % 4;
			int ty = j % 4;

			data[i*width+j] = 16*data[i*width+j] / 255;
			data[i*width+j]++;

			if(data[i*width+j] > th[tx][ty]){
				data[i*width+j] = 255;
			}else{
				data[i*width+j] = 0;
			}

		}
	}

	return;
}

void min_dithering(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;
	int error = 0;
	int th [2][2] = {0,2,1,1}; //wiki


	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){
			if(data[i*width+j] <= 128){
				error = data[i*width+j] + data[i*width+j+1];
				data[i*width+j] = 0;
			}else{
				error = data[i*width+j]-255 + data[i*width+j+1];
				data[i*width+j] = 255;
			}

			int tx = i % 2;
			int ty = j % 2;

			data[(i+tx)*width+(j+ty)] = data[(i+tx)*width+(j+ty)] + error*th[tx][ty]/4;

			/*data[i*width+j+1] = data[i*width+j+1] + error*2/4;
			data[(i+1)*width+j] = data[(i+1)*width+j] + error*1/4;
			data[(i+1)*width+j+1] = data[(i+1)*width+j+1] + error*1/4;*/

			j++;
		}
		i++;
	}

	return;
}

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	IplImage* img = 0;
	int height,width,step,channels;
	unsigned char *data;

	// load an image
	img= cvLoadImage("plane.tiff");
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

	IplImage* imgRed2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue2 = cvCreateImage(cvGetSize(img), 8, 1);

	IplImage* imgRed3 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen3 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue3 = cvCreateImage(cvGetSize(img), 8, 1);

	// Split image onto the color planes
	cvSplit(img, imgBlue, imgGreen, imgRed, NULL);

	// Add weight to rgb values
	// Y = 0.2126*R + 0.7152*G + 0.0722*B.
	cvAddWeighted( imgRed, 0.2126 , imgGreen, 0.7152 , 0.0, imgGrayscale );
	cvAddWeighted( imgGrayscale, 1.0, imgBlue, 0.0722 , 0.0, imgGrayscale );


	// create a window
	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Gray scale", CV_WINDOW_AUTOSIZE);

	cvNamedWindow("Ordered Dither Color" , CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Ordered Dither Grayscale" , CV_WINDOW_AUTOSIZE);

	cvNamedWindow("Min avg Dither Color" , CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Min avg Dither Grayscale" , CV_WINDOW_AUTOSIZE);

	// Create an image for the output
	IplImage* imgOrderedG = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
	cvCopy(imgGrayscale,imgOrderedG);

	cvSplit(img, imgRed2, imgGreen2, imgBlue2, NULL);
	IplImage* imgOrderedDither = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );

	IplImage* imgMinG = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
	cvCopy(imgGrayscale,imgMinG);

	cvSplit(img, imgRed3, imgGreen3, imgBlue3, NULL);
	IplImage* imgMinDither = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );

	//Dither the image.
	ordered_dithering(imgOrderedG);

	ordered_dithering(imgRed2);
	ordered_dithering(imgGreen2);
	ordered_dithering(imgBlue2);
	cvMerge(imgRed2, imgGreen2, imgBlue2, NULL, imgOrderedDither);

	min_dithering(imgMinG);

	min_dithering(imgRed3);
	min_dithering(imgGreen3);
	min_dithering(imgBlue3);
	cvMerge(imgRed3, imgGreen3, imgBlue3, NULL, imgMinDither);

	// show the image
	cvShowImage("Original", img);
	cvShowImage("Gray scale", imgGrayscale );

	cvShowImage("Ordered Dither Color", imgOrderedDither);
	cvShowImage("Ordered Dither Grayscale", imgOrderedG);

	cvShowImage("Min avg Dither Color", imgMinDither);
	cvShowImage("Min avg Dither Grayscale", imgMinG);

	// wait for a key
	cvWaitKey(0);

	// release the image
	cvReleaseImage(&img);
	cvReleaseImage(&imgGrayscale);
	cvReleaseImage(&imgOrderedDither);
	cvReleaseImage(&imgMinDither);
	cvReleaseImage(&imgOrderedG);
	cvReleaseImage(&imgMinG);

	return 0;
}

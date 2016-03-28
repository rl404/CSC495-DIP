// Axel 56130500236

#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <math.h>

void norm_diff(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fx = 0, fy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){

			d[i][j] = data[i*width+j];

			/*
			fx = data[(i+1)*width+j] - data[i*width+j];
			fy = data[i*width+j+1] - data[i*width+j];

			int a = sqrt(pow(fx,2)+pow(fy,2));

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;*/
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){
			fx = d[i+1][j] - d[i][j];
			fy = d[i][j+1] - d[i][j];

			double a = sqrt(pow(fx,2)+pow(fy,2));

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

void roberts(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fx = 0, fy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){
			fx = d[i+1][j+1] - d[i][j];
			fy = d[i][j+1] - d[i+1][j];

			int a = sqrt(pow(fx,2)+pow(fy,2));

			if(a > 255){a=255;}
			if(a < 0){a=0;}
			data[i*width+j] = a;
		}
	}

	return;
}

void sobel(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fx = 0, fy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=1; i < width-1; i++){
		for(int j=1; j < height-1; j++){
			fx = - d[i-1][j-1] + d[i+1][j+1]
				- 2*d[i-1][j] + 2*d[i+1][j]
				- d[i-1][j+1] + d[i+1][j+1];
			fy = - d[i-1][j-1] - 2*d[i][j-1] - d[i+1][j-1]
				+ d[i-1][j+1] + 2*d[i][j+1] + d[i+1][j+1];

			int a = sqrt(pow(fx,2)+pow(fy,2));

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

void laplacian(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fx = 0, fy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){

			fx = d[i+1][j] + d[i-1][j] - 2*d[i][j];
			fy = d[i][j+1] + d[i][j-1] - 2*d[i][j];

			int a = fx+fy;
			//int a = data[i*width+j] - fx-fy;

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

void laplacian2(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fx = 0, fy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){

			fx = d[i+1][j] + d[i-1][j] - 2*d[i][j];
			fy = d[i][j+1] + d[i][j-1] - 2*d[i][j];

			//int a = fx+fy;
			int a = data[i*width+j] - fx-fy;

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

void variant(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fxy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){

			fxy = -d[i-1][j-1] - d[i][j-1] - d[i+1][j-1]
				- d[i-1][j] + 9*d[i][j] - d[i+1][j]
				- d[i-1][j+1] - d[i][j+1] - d[i+1][j+1];

			//int a = fx+fy;
			int a = fxy;

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

void sobel2(IplImage* img){
	int height,width,step,channels;
	unsigned char *data;
	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	int d[width][height];

	double fxy = 0;

	for(int i=0; i < width; i++){
		for(int j=0; j < height; j++){
			d[i][j] = data[i*width+j];
		}
	}

	for(int i=0; i < width-1; i++){
		for(int j=0; j < height-1; j++){

			fxy = d[i-1][j+1] + 2*d[i][j+1] + d[i+1][j+1]
				- d[i-1][j-1] - 2*d[i][j-1] - d[i+1][j-1]
				+ d[i+1][j-1] + 2*d[i+1][j] + d[i+1][j+1]
				- d[i-1][j-1] - 2*d[i-1][j] - d[i-1][j+1];

			//int a = fx+fy;
			int a = d[i][j] - fxy;

			if(a > 255){a=255;}
			if(a < 0){a=0;}

			data[i*width+j] = a;
		}
	}

	return;
}

int main(int argc, char *argv[])
{
	IplImage* img = 0;
	int height,width,step,channels;
	unsigned char *data;

	// load an image
	img= cvLoadImage("kantai.png");
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

	IplImage* imgGrayscale = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1); // 8-bit grayscale is enough.
	// convert to grayscale.
	cvCvtColor(img, imgGrayscale, CV_BGR2GRAY);

	// Create an image for the outputs
	IplImage* imgSobelX = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 ); // to prevent overflow.
	IplImage* imgSobelY = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
	IplImage* imgSobelAdded = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
	IplImage* imgSobel = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 ); // final image is enough to be an 8-bit plane.


	// Sobel
	cvSobel(imgGrayscale, imgSobelX, 1, 0, 3);
	cvSobel(imgGrayscale, imgSobelY, 0, 1, 3);
	cvAdd(imgSobelX, imgSobelY, imgSobelAdded);
	cvConvertScaleAbs(imgSobelAdded, imgSobel); //scaled to 8-bit level; important for visibility.


	//----------------------- OULINE EXTRACTION -------------------------------
	// Normal diff
	IplImage* imgNormDiff = cvCreateImage(cvGetSize(img), 8, 1);
	cvCopy(imgGrayscale,imgNormDiff);
	norm_diff(imgNormDiff);

	// Roberts
	IplImage* imgRoberts = cvCreateImage(cvGetSize(img), 8, 1);
	cvCopy(imgGrayscale,imgRoberts);
	roberts(imgRoberts);

	// Sobel
	IplImage* imgSobel2 = cvCreateImage(cvGetSize(img), 8, 1);
	cvCopy(imgGrayscale,imgSobel2);
	sobel(imgSobel2);

	// Laplacian
	IplImage* imgLap = cvCreateImage(cvGetSize(img), 8, 1);
	cvCopy(imgGrayscale,imgLap);
	laplacian(imgLap);

	//--------------------------- ENHANCEMENT --------------------------------
	// Laplacian
	IplImage* imgLap2 = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* imgRed = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue = cvCreateImage(cvGetSize(img), 8, 1);

	cvSplit(img, imgRed, imgGreen, imgBlue, NULL);

	laplacian2(imgBlue);
	laplacian2(imgGreen);
	laplacian2(imgRed);
	cvMerge(imgRed,imgGreen,imgBlue, NULL, imgLap2);

	// Variant
	IplImage* imgVariant = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* imgRed2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue2 = cvCreateImage(cvGetSize(img), 8, 1);

	cvSplit(img, imgRed2, imgGreen2, imgBlue2, NULL);

	variant(imgBlue2);
	variant(imgGreen2);
	variant(imgRed2);
	cvMerge(imgRed2,imgGreen2,imgBlue2, NULL, imgVariant);

	// Sobel
	IplImage* imgSobel3 = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* imgRed3 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgGreen3 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgBlue3 = cvCreateImage(cvGetSize(img), 8, 1);

	cvSplit(img, imgRed3, imgGreen3, imgBlue3, NULL);

	sobel2(imgBlue3);
	sobel2(imgGreen3);
	sobel2(imgRed3);
	cvMerge(imgRed3,imgGreen3,imgBlue3, NULL, imgSobel3);




	// create a window
	cvNamedWindow("Original", CV_WINDOW_KEEPRATIO);

	cvNamedWindow("Normal different line", CV_WINDOW_KEEPRATIO);
	cvNamedWindow("Roberts line", CV_WINDOW_FREERATIO);
	cvNamedWindow("Sobel line", CV_WINDOW_FREERATIO);
	cvNamedWindow("Laplacian line", CV_WINDOW_KEEPRATIO);

	cvNamedWindow("Laplacian Color", CV_WINDOW_KEEPRATIO);
	cvNamedWindow("Variant", CV_WINDOW_KEEPRATIO);
	cvNamedWindow("Sobel", CV_WINDOW_KEEPRATIO);
	/*cvNamedWindow( "Sobel-x" );
  cvNamedWindow( "Sobel-y" );
  cvNamedWindow( "Sobel-Added" );
  cvNamedWindow( "Sobel-Added (scaled)" );*/

	// show the image
	cvShowImage("Original", img);
	cvShowImage("Normal different line", imgNormDiff);
	cvShowImage("Roberts line",imgRoberts);
	cvShowImage("Sobel line", imgSobel2);
	cvShowImage("Laplacian line", imgLap);

	cvShowImage("Laplacian Color", imgLap2);
	cvShowImage("Variant", imgVariant);
	cvShowImage("Sobel", imgSobel3);

	/*cvShowImage("Sobel-x", imgSobelX);
  cvShowImage("Sobel-y", imgSobelY);
  cvShowImage("Sobel-Added", imgSobelAdded);
  cvShowImage("Sobel-Added (scaled)", imgSobel);*/

	// wait for a key
	cvWaitKey(0);

	// release the image
	cvReleaseImage(&img);
	cvReleaseImage(&imgGrayscale);
	cvReleaseImage(&imgNormDiff);
	cvReleaseImage(&imgRoberts);
	cvReleaseImage(&imgSobel2);
	cvReleaseImage(&imgLap);

	cvReleaseImage(&imgLap2);
	cvReleaseImage(&imgVariant);
	cvReleaseImage(&imgSobel3);

	cvReleaseImage(&imgSobelX);
	cvReleaseImage(&imgSobelY);
	cvReleaseImage(&imgSobelAdded);
	cvReleaseImage(&imgSobel);


	return 0;
}





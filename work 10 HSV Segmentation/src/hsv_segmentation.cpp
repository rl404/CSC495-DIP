//Axel 56130500236

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>

using namespace cv;

// Initial values
int _hueLower = 0;
int _hueUpper = 180;
int _saturationLower = 0;
int _saturationUpper = 255;
int _valueLower = 0;
int _valueUpper = 255;

IplImage* img = 0;
IplImage* img2 = 0;
IplImage* imgSeg = 0;

IplImage* DrawHistogram(CvHistogram *hist, float scaleX=2, float scaleY=2){
	float histMax = 0;
	cvGetMinMaxHistValue(hist, 0, &histMax, 0, 0);
	IplImage* imgHist = cvCreateImage(cvSize(256*scaleX, 64*scaleY), 8 ,1); // Hue has values from 0 to 179.
	cvZero(imgHist);
	cvSet(imgHist, cvScalar(255));

	for(int i=0; i< 255; i++)
	{
		//float histValue = cvQueryHistValue_1D(hist, i); //not working, use the one below instead.
		float histValue = cvGetReal1D(hist->bins, i);
		float nextValue = cvGetReal1D(hist->bins, i+1);

		CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
		CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
		CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
		CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);

		int numPts = 5;
		CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};
		cvFillConvexPoly(imgHist, pts, numPts, cvScalar(0));

	}
	return imgHist;
}




void updateHueSaturationValue( int arg, void* )
{
	int hueLower = _hueLower;
	int hueUpper = _hueUpper;

	int saturationLower = _saturationLower;
	int saturationUpper = _saturationUpper;
	int valueLower = _valueLower;
	int valueUpper = _valueUpper;


	int i,j;

	// Split the image into 3 planes, namely, B, G, and R.
	IplImage* Bchannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Gchannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Rchannel = cvCreateImage(cvGetSize(img), 8, 1);
	cvSplit(img, Bchannel, Gchannel, Rchannel, NULL);

	unsigned char *dataB;
	dataB = (uchar *)Bchannel->imageData;
	unsigned char *dataG;
	dataG = (uchar *)Gchannel->imageData;
	unsigned char *dataR;
	dataR = (uchar *)Rchannel->imageData;

	IplImage* Bchannel2 = cvCreateImage(cvGetSize(img2), 8, 1);
	IplImage* Gchannel2 = cvCreateImage(cvGetSize(img2), 8, 1);
	IplImage* Rchannel2 = cvCreateImage(cvGetSize(img2), 8, 1);
	cvSplit(img2, Bchannel2, Gchannel2, Rchannel2, NULL);

	unsigned char *dataB2;
	dataB2 = (uchar *)Bchannel2->imageData;
	unsigned char *dataG2;
	dataG2 = (uchar *)Gchannel2->imageData;
	unsigned char *dataR2;
	dataR2 = (uchar *)Rchannel2->imageData;

	// Transform RGB to HSV
	IplImage* img_hsv = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img,img_hsv,CV_BGR2HSV);

	// Split the HSV into 3 planes, namely, H, S, and V.
	IplImage* Hchannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Schannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Vchannel = cvCreateImage(cvGetSize(img), 8, 1);
	cvSplit(img_hsv, Hchannel, Schannel, Vchannel, NULL);


	// Note that the Hue value in OpenCV is in the range from 0 to 179. A total of 180.
	// However, Saturation and Value are from 0 to 255.
	unsigned char *dataH;
	dataH = (uchar *)Hchannel->imageData;
	unsigned char *dataS;
	dataS = (uchar *)Schannel->imageData;
	unsigned char *dataV;
	dataV = (uchar *)Vchannel->imageData;

	int step = Hchannel->widthStep; // Same for the other channels.
	int hue,saturn,val;
	for(i=0; i< Hchannel->height; i++){
		for(j=0; j< Hchannel->width; j++){

			// Hue
			hue = dataH[i*step+j];
			saturn = dataS[i*step+j];
			val = dataV[i*step+j];
			if((hue > hueLower) && (hue < hueUpper)){
				dataB[i*step+j] = dataB2[i*step+j];
				dataG[i*step+j] = dataG2[i*step+j];
				dataR[i*step+j] = dataR2[i*step+j];
			}
			else if((saturn > saturationLower) && (saturn < saturationUpper)){
				dataB[i*step+j] = dataB2[i*step+j];
				dataG[i*step+j] = dataG2[i*step+j];
				dataR[i*step+j] = dataR2[i*step+j];
			}
			else if((val > valueLower) && (val < valueUpper)){
				dataB[i*step+j] = dataB2[i*step+j];
				dataG[i*step+j] = dataG2[i*step+j];
				dataR[i*step+j] = dataR2[i*step+j];
			}
		}
	}

	IplImage* imgSeg = cvCreateImage(cvGetSize(img), 8, 3);
	cvMerge(Bchannel, Gchannel, Rchannel, NULL, imgSeg);

	// Hue histogram
	cvCvtColor(imgSeg,img_hsv,CV_BGR2HSV); // reused img_hsv.
	cvSplit(img_hsv, Hchannel, Schannel, Vchannel, NULL); // reused Hchannel and others.

	int numBins = 256;
	float range[] = {0, 255};
	float *ranges[] = {range};
	CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);

	cvClearHist(hist);
	cvCalcHist(&Rchannel, hist, 0, 0);
	IplImage* imgHistR = DrawHistogram(hist);
	cvShowImage("R Histogram", imgHistR);

	cvClearHist(hist);
	cvCalcHist(&Gchannel, hist, 0, 0);
	IplImage* imgHistG = DrawHistogram(hist);
	cvShowImage("G Histogram", imgHistG);

	cvClearHist(hist);
	cvCalcHist(&Bchannel, hist, 0, 0);
	IplImage* imgHistB = DrawHistogram(hist);
	cvShowImage("B Histogram", imgHistB);

	// Image releases
	cvReleaseImage(&img_hsv);
	cvReleaseImage(&Hchannel);
	cvReleaseImage(&Schannel);
	cvReleaseImage(&Vchannel);

	cvReleaseImage(&Bchannel);
	cvReleaseImage(&Gchannel);
	cvReleaseImage(&Rchannel);
	cvReleaseImage(&Bchannel2);
	cvReleaseImage(&Gchannel2);
	cvReleaseImage(&Rchannel2);

	cvShowImage("After", imgSeg); // Show the segmentation.


}

int main(int argc, char *argv[])
{
	int height,width,step,channels;
	int height2,width2,step2,channels2;
	unsigned char *data, *data2;

	setbuf(stdout, NULL);

	// load an image
	img = cvLoadImage("a.jpg");
	img2 = cvLoadImage("building.tiff");
	if(!img || !img2){
		printf("Could not load image file: %s\n",argv[1]);
		exit(0);
	}

	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	data      = (uchar *)img->imageData;

	height2    = img2->height;
	width2     = img2->width;
	step2      = img2->widthStep;
	channels2  = img2->nChannels;
	data2      = (uchar *)img2->imageData;

	printf("Processing a %dx%d image with %d channels\n",height,width,channels);
	printf("step = %d\n", step);

	// create a window
	cvNamedWindow("Object", CV_WINDOW_AUTOSIZE); // Fixed size
	cvNamedWindow("Background", CV_WINDOW_AUTOSIZE); // Fixed size
	cvNamedWindow("After", CV_WINDOW_NORMAL); // Re-sizable

	createTrackbar("Hue-L", "After", &_hueLower, 179, updateHueSaturationValue);
	createTrackbar("Hue-U", "After", &_hueUpper, 179, updateHueSaturationValue);

	createTrackbar("Saturation-L", "After", &_saturationLower, 255, updateHueSaturationValue);
	createTrackbar("Saturation-U", "After", &_saturationUpper, 255, updateHueSaturationValue);

	createTrackbar("Value-L", "After", &_valueLower, 255, updateHueSaturationValue);
	createTrackbar("Value-U", "After", &_valueUpper, 255, updateHueSaturationValue);


	updateHueSaturationValue(0, 0);

	// show the image
	cvShowImage("Object", img );
	cvShowImage("Background", img2 );

	// wait for a key
	cvWaitKey(0);

	// release the images
	cvReleaseImage(&img);
	cvReleaseImage(&img2);
	cvReleaseImage(&imgSeg);

	return 0;
}



// Axel 56130500236

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>
#include <iostream>
using namespace std;

IplImage* DrawHistogram(CvHistogram *hist, float scaleX=2, float scaleY=2){
    float histMax = 0;
    cvGetMinMaxHistValue(hist, 0, &histMax, 0, 0);
    IplImage* imgHist = cvCreateImage(cvSize(256*scaleX, 64*scaleY), 8 ,1);

    cvZero(imgHist); //clear all elements to be zeros.
    cvSet(imgHist, cvScalar(255)); //set all to be 255; white background.

    for(int i=0; i< 255; i++) {
    	//float histValue = cvQueryHistValue_1D(hist, i); //not working, use the one below instead.
    	float histValue = cvGetReal1D(hist->bins, i);
        //float nextValue = cvQueryHistValue_1D(hist, i+1); //not working.
        float nextValue = cvGetReal1D(hist->bins, i+1);

        CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
        CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
        CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
        CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);

        int numPts = 5;
        CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};
        cvFillConvexPoly(imgHist, pts, numPts, cvScalar(0)); //filled with black color.
    }
    return imgHist;
}

void logTrans(CvHistogram *hist, IplImage* img){
	int height    = img->height;
	int width     = img->width;
	int step      = img->widthStep;

	double val;
	unsigned char *data;
	data = (uchar *)img->imageData;

	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			val = data[x*step+y];
			val = val/255;
			val = log(1+val);
			data[x*step+y] = val * 255;
		}
	}
}

void powerLaw(CvHistogram *hist, IplImage* img){
	int height    = img->height;
	int width     = img->width;
	int step      = img->widthStep;

	double val,max=0,min=255,avg,sum;
	double power;
	unsigned char *data;
	data = (uchar *)img->imageData;

	for(int x = 0; x < height; x++){
			for(int y = 0; y < width; y++){
				val = data[x*step+y];
				val = val/255;
				if(val>max){max=val;}
				if(val<min){min=val;}

				sum += val;
			}
	}

	avg = sum/(height*width);

	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			val = data[x*step+y];
			val = val/255;

			if((max-min)/2 < 0.5){
				power = 5.0;
				if((max-min)/4 < 0.25){
					power = 2.5;
				}
			}else if((max-min)/2 >= 0.5){
				power = 10.0;
				if(3*(max-min)/4 > 0.75){
					power = 25.0;
				}
			}

			val = pow(val,power);
			data[x*step+y] = val * 255;
		}
	}

	return;
}

void equalizeHistogram(CvHistogram *hist, IplImage* img){

	int height    = img->height;
	int width     = img->width;
	int step      = img->widthStep;

	int n = height * width;

	float sk,sum = 0.0;
	uchar val;
	int f[256] = {0};

	unsigned char *data;
	data = (uchar *)img->imageData;

	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			val = data[x*step+y];
			f[(int)val]++;
		}
	}

	for(int x = 0; x < height; x++){
		for(int y = 0; y < width; y++){
			val = data[x*step+y];

			sum = 0.0;
			for(int j = 0; j <= val; j++){
				sum += float(f[j]);
			}

			sk = 255 * sum /(float)n;
			data[x*step+y] = sk;
		}
	}

	return;
}

int main(int argc, char *argv[]){

	char* image = "mandrill.tiff";

    IplImage* img = cvLoadImage(image);

    setbuf(stdout, NULL);
    if(!img){
      printf("Could not load image file: %s\n",argv[1]);
      exit(0);
    }

    int height,width,step,channels,o,g,he,hc,pl,lt;

	printf("%s \nOption: (1 = yes, 0 = no)\n",image);
	printf("Show original? "); cin >> o;
	printf("Show grayscale? "); cin >> g;
	printf("Show histogram equalization? "); cin >> he;
	printf("Show histogram color? "); cin >> hc;
	printf("Show power law? "); cin >> pl;
	printf("Show log transformation? "); cin >> lt;

    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;

    // Checking...
    printf("Processing a %dx%d image with %d channels\n",height,width,channels);

    // Histogram.
    int numBins = 256;
    float range[] = {0, 255};
    float *ranges[] = { range };
    CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
    cvClearHist(hist);

    // Create image(s).
    IplImage* imgRed = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgGreen = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgBlue = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgGrayscale = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgHistEqualized = cvCreateImage(cvGetSize(img), 8, 1);

    IplImage* imgHistRed = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgHistGreen = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgHistBlue = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgHistColor = cvCreateImage(cvGetSize(img), 8, 3);

    IplImage* imgPowerLaw = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* imgLogTrans = cvCreateImage(cvGetSize(img), 8, 1);

    // Split image into the color planes.
    cvSplit(img, imgBlue, imgGreen, imgRed, NULL);
	cvAddWeighted( imgRed, 0.2126 , imgGreen, 0.7152 , 0.0, imgGrayscale );
	cvAddWeighted( imgGrayscale, 1.0, imgBlue, 0.0722 , 0.0, imgGrayscale );

	cvSplit(img,imgHistRed,imgHistGreen,imgHistBlue,NULL);

	// ORIGINAL
    cvClearHist(hist);
    cvCalcHist(&imgGrayscale, hist, 0, 0);
    IplImage* imgHistGrayscale = DrawHistogram(hist);

    // HIST EQUALIZED BLACK WHITE
  	cvCopy(imgGrayscale, imgHistEqualized);
    equalizeHistogram(hist, imgHistEqualized);
    //cvEqualizeHist(imgGrayscale, imgHistEqualized);

    cvClearHist(hist);
    cvCalcHist(&imgHistEqualized, hist, 0, 0);
    IplImage* imgHistHistEqualized = DrawHistogram(hist);

    // HIST EQUALIZED COLOR --------------------------------------------
    // RED
	equalizeHistogram(hist, imgHistRed);
	cvClearHist(hist);
	cvCalcHist(&imgHistEqualized, hist, 0, 0);
	IplImage* imgHistHistRed = DrawHistogram(hist);

    // GREEN
	equalizeHistogram(hist, imgHistRed);
	cvClearHist(hist);
	cvCalcHist(&imgHistEqualized, hist, 0, 0);
	IplImage* imgHistHistGreen = DrawHistogram(hist);

    // BLUE
	equalizeHistogram(hist, imgHistRed);
	cvClearHist(hist);
	cvCalcHist(&imgHistEqualized, hist, 0, 0);
	IplImage* imgHistHistBlue = DrawHistogram(hist);

	cvMerge(imgHistRed, imgHistGreen, imgHistBlue, NULL, imgHistColor);


    // POWER LAW ----------------------------------------------------------
    cvCopy(imgGrayscale, imgPowerLaw);
	powerLaw(hist, imgPowerLaw);

	cvClearHist(hist);
	cvCalcHist(&imgPowerLaw, hist, 0, 0);
	IplImage* imgHistPowerLaw = DrawHistogram(hist);

	// LOG TRANSFORMATION -------------------------------------------------
	cvCopy(imgGrayscale, imgLogTrans);
	logTrans(hist, imgLogTrans);

	cvClearHist(hist);
	cvCalcHist(&imgLogTrans, hist, 0, 0);
	IplImage* imgHistLogTrans = DrawHistogram(hist);

	//show window(s).
	if(o==1){
		cvNamedWindow("Original");
		cvShowImage("Original", img);
	}

	if(g==1){
		cvNamedWindow("Gray scale");
		cvNamedWindow("Gray scale image");
		cvShowImage("Gray scale", imgHistGrayscale);
		cvShowImage("Gray scale image", imgGrayscale);
	}

	if(he==1){
		cvNamedWindow("Histogram equalization");
		cvNamedWindow("Histogram equalization image");
		cvShowImage("Histogram equalization", imgHistHistEqualized);
		cvShowImage("Histogram equalization image", imgHistEqualized);
	}

	if(hc==1){
		cvNamedWindow("Histogram red");
		cvNamedWindow("Histogram red image");
		cvNamedWindow("Histogram green");
		cvNamedWindow("Histogram green image");
		cvNamedWindow("Histogram blue");
		cvNamedWindow("Histogram blue image");
		cvNamedWindow("Histogram color image");

		cvShowImage("Histogram red", imgHistHistRed);
		cvShowImage("Histogram red image", imgHistRed);
		cvShowImage("Histogram green", imgHistHistGreen);
		cvShowImage("Histogram green image", imgHistGreen);
		cvShowImage("Histogram blue", imgHistHistBlue);
		cvShowImage("Histogram blue image", imgHistBlue);
		cvShowImage("Histogram color image", imgHistColor);
	}

	if(pl==1){
		cvNamedWindow("Power Law");
		cvNamedWindow("Power Law image");
		cvShowImage("Power Law", imgHistPowerLaw);
		cvShowImage("Power Law image", imgPowerLaw);
	}

	if(lt==1){
		cvNamedWindow("Log Transformation");
		cvNamedWindow("Log Transformation image");
		cvShowImage("Log Transformation", imgHistLogTrans);
		cvShowImage("Log Transformation image", imgLogTrans);
	}

	cvWaitKey(0);

    cvDestroyWindow("Original");
	cvDestroyWindow("Gray scale");
	cvDestroyWindow("Gray scale image");
	cvDestroyWindow("Histogram equalization");
	cvDestroyWindow("Histogram equalization image");
	cvDestroyWindow("Histogram red");
	cvDestroyWindow("Histogram red image");
	cvDestroyWindow("Histogram green");
	cvDestroyWindow("Histogram green image");
	cvDestroyWindow("Histogram blue");
	cvDestroyWindow("Histogram blue image");
	cvDestroyWindow("Histogram color image");
	cvDestroyWindow("Power Law");
	cvDestroyWindow("Power Law image");
	cvDestroyWindow("Log Transformation");
	cvDestroyWindow("Log Transformation");

	cvReleaseImage(&img);
	cvReleaseImage(&imgGrayscale);
	cvReleaseImage(&imgHistGrayscale);
	cvReleaseImage(&imgHistEqualized);
	cvReleaseImage(&imgHistHistEqualized);
	cvReleaseImage(&imgHistRed);
	cvReleaseImage(&imgHistGreen);
	cvReleaseImage(&imgHistBlue);
	cvReleaseImage(&imgHistColor);
	cvReleaseImage(&imgPowerLaw);
	cvReleaseImage(&imgHistPowerLaw);
	cvReleaseImage(&imgLogTrans);
	cvReleaseImage(&imgHistLogTrans);

    return 0;
}





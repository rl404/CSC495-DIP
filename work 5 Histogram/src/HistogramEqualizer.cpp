// Axel 56130500236

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>

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



int main(int argc, char *argv[])
{
    IplImage* img = cvLoadImage("lowContrastPollen.jpg");
	//IplImage* img = cvLoadImage("darkpollen.jpg");

    setbuf(stdout, NULL);
    if(!img){
      printf("Could not load image file: %s\n",argv[1]);
      exit(0);
    }
    int height,width,step,channels;


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

    // Split image into the color planes.
    cvSplit(img, imgBlue, imgGreen, imgRed, NULL);
    // Convert the image into its corresponding gray-scale image.
	// Add weight to rgb values
    // Y = 0.2126*R + 0.7152*G + 0.0722*B.
	cvAddWeighted( imgRed, 0.2126 , imgGreen, 0.7152 , 0.0, imgGrayscale );
	cvAddWeighted( imgGrayscale, 1.0, imgBlue, 0.0722 , 0.0, imgGrayscale );

	// Compute and draw histogram(s).
    cvClearHist(hist);
    cvCalcHist(&imgGrayscale, hist, 0, 0);
    IplImage* imgHistGrayscale = DrawHistogram(hist);

    // Work on the histogram equalization.
  	cvCopy(imgGrayscale, imgHistEqualized);
    equalizeHistogram(hist, imgHistEqualized);
    //cvEqualizeHist(imgGrayscale, imgHistEqualized);

  	// Compute and draw histogram(s) of the equalized-histogram image.
    cvClearHist(hist);
    cvCalcHist(&imgHistEqualized, hist, 0, 0);
    IplImage* imgHistHistEqualized = DrawHistogram(hist);

    //show window(s).
    cvNamedWindow("Gray scale");
    cvNamedWindow("Gray scale image");
    cvNamedWindow("Histogram equalization");
    cvNamedWindow("Histogram equalization image");

    cvShowImage("Gray scale", imgHistGrayscale);
    cvShowImage("Gray scale image", imgGrayscale);
    cvShowImage("Histogram equalization", imgHistHistEqualized);
    cvShowImage("Histogram equalization image", imgHistEqualized);

    cvWaitKey(0);

    // destroy window(s).
    cvDestroyWindow("Gray scale");
    cvDestroyWindow("Gray scale image");
    cvDestroyWindow("Histogram equalization");
    cvDestroyWindow("Histogram equalization image");

    // release the image
    cvReleaseImage(&imgGrayscale);
    cvReleaseImage(&imgHistGrayscale);
    cvReleaseImage(&imgHistEqualized);
    cvReleaseImage(&imgHistHistEqualized);

    return 0;
}





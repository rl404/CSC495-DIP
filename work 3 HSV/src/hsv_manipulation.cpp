//Axel 56130500236

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>
#include <math.h>

using namespace cv;

// Initial values
int _hue = 0;
int _saturation = 0;
int _value = 0;
int height,width,step,channels;
IplImage* img = 0;
unsigned char *data;

int mod (int a, int b)
{
   int ret = a % b;
   if(ret < 0)
     ret+=b;
   return ret;
}

void updateHueSaturationValue( int arg, void* )
{
	int hue = _hue; // Range from 0 to 179.
	int saturation = _saturation;
	int value = _value;
	int i,j,k;

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

	int val,val1,val2;
	uchar red, green, blue;
	double min, max, delta, r,g,b;

	for(i=0; i< Hchannel->height; i++){
		for(j=0; j< Hchannel->width; j++){

			/*red = data[i*step+j*channels+2];
			green = data[i*step+j*channels+1];
			blue = data[i*step+j*channels+0];

			for(k=0;k<channels;k++){
				val1 += (0.2126 * red)+(0.7152 * green)+(0.0722 * blue);
			}

			val1 = val1/3;
			val2 = (0.2126 * red)+(0.7152 * green)+(0.0722 * blue);
			val = val2;

			if(val>=255){val=255;}

			dataV[i*width+j] = val *_value/255 ;

			b = (blue-val)/1.8556;
			r = (red-val)/1.5748;

			dataS[i*width+j] = sqrt(pow(b,2) + pow(r,2)) * _saturation/255;
			dataH[i*width+j] = atan(r/b) * _hue*2/255;*/

		}
	}

	// After modifying HSV, we need to merge and convert it to RGB for display.
	IplImage* img_after = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* img_hsv_after = cvCreateImage(cvGetSize(img), 8, 3);
	cvMerge(Hchannel, Schannel, Vchannel, NULL, img_hsv_after);
	cvCvtColor(img_hsv_after, img_after, CV_HSV2BGR);

	cvReleaseImage(&img_hsv_after); // Deallocate some images.
	cvReleaseImage(&img_hsv);
	cvReleaseImage(&Hchannel);
	cvReleaseImage(&Schannel);
	cvReleaseImage(&Vchannel);

	cvShowImage("After", img_after); // Show the updated image.
}


int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	// load an image
	img= cvLoadImage("a.jpg");
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

	// create a window
	cvNamedWindow("Before", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("After", CV_WINDOW_AUTOSIZE);

	createTrackbar("Hue", "After", &_hue, 179, updateHueSaturationValue);
	createTrackbar("Saturation", "After", &_saturation, 255, updateHueSaturationValue);
	createTrackbar("Value", "After", &_value, 255, updateHueSaturationValue);
	updateHueSaturationValue(0, 0);

	// show the image
	cvShowImage("Before", img );

	// wait for a key
	cvWaitKey(0);

	// release the image
	cvReleaseImage(&img);

	return 0;
}




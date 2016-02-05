
// Axel 56130500236

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <stdio.h>

using namespace cv;


// Initial values
int _hue = 0;
int _saturation = 0;
int _value = 0;
IplImage* img = 0;

void updateHueSaturationValue( int arg, void* )
{
    int hue = _hue; // Range from 0 to 179.
    int saturation = _saturation;
    int value = _value;
    int i,j;

    // Transform RGB to HSV
    IplImage* img_hsv = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img,img_hsv,CV_BGR2HSV);
    IplImage* img_hsv2 = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img,img_hsv2,CV_BGR2HSV);

    // Split the HSV into 3 planes, namely, H, S, and V.
    IplImage* Hchannel = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* Schannel = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage* Vchannel = cvCreateImage(cvGetSize(img), 8, 1);
    cvSplit(img_hsv, Hchannel, Schannel, Vchannel, NULL);

    IplImage* Hchannel2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Schannel2 = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* Vchannel2 = cvCreateImage(cvGetSize(img), 8, 1);
	cvSplit(img_hsv2, Hchannel2, Schannel2, Vchannel2, NULL);

    // Note that the Hue value in OpenCV is in the range from 0 to 179. A total of 180.
    // However, Saturation and Value are from 0 to 255.
    unsigned char *dataH;
    dataH = (uchar *)Hchannel->imageData;
    unsigned char *dataS;
    dataS = (uchar *)Schannel->imageData;
    unsigned char *dataV;
    dataV = (uchar *)Vchannel->imageData;

    unsigned char *dataH2;
	dataH2 = (uchar *)Hchannel2->imageData;
	unsigned char *dataS2;
	dataS2 = (uchar *)Schannel2->imageData;
	unsigned char *dataV2;
	dataV2 = (uchar *)Vchannel2->imageData;

    int step = Hchannel->widthStep; // Same for the other channels.
    int val,h,s,v,h2,s2,v2;
    int min=255,max=0,r,g,b;
    for(i=0; i< Hchannel->height; i++){
  	  for(j=0; j< Hchannel->width; j++){

			h = dataH[i*step+j] + _hue;
			s = dataS[i*step+j] + _saturation;
			v = dataV[i*step+j] + _value;

			if(h>=179){
				dataH[i*step+j] = 179;
			}else{
				dataH[i*step+j] = h;
			}

			if(s>=255){
				dataS[i*step+j] = 255;
			}else{
				dataS[i*step+j] = s;
			}

			if(v>=255){
				dataV[i*step+j] = 255;
			}else{
				dataV[i*step+j] = v;
			}

			//-----------CONTRAST------------------------

			if(min > dataS2[i*step+j]){min = dataS2[i*step+j];}
			if(min > dataV2[i*step+j]){min = dataV2[i*step+j];}

			if(max < dataS2[i*step+j]){max = dataS2[i*step+j];}
			if(max < dataV2[i*step+j]){max = dataV2[i*step+j];}

			s2 = 255 * (dataS2[i*step+j] - min) /(max-min);
			v2 = 255 * (dataV2[i*step+j] - min) /(max-min);

			if(v2>255){
				dataV2[i*step+j] = 255;
			}else{
				dataV2[i*step+j] = v2;
			}

			if(s2>255){
				dataS2[i*step+j] = 255;
			}else{
				dataS2[i*step+j] = s2;
			}

  	  }
    }

    // After modifying HSV, we need to merge and convert it to RGB for display.
    IplImage* img_after = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage* img_hsv_after = cvCreateImage(cvGetSize(img), 8, 3);

    IplImage* img_con = cvCreateImage(cvGetSize(img), 8, 3);
    IplImage* img_after_con = cvCreateImage(cvGetSize(img), 8, 3);

    cvMerge(Hchannel, Schannel, Vchannel, NULL, img_hsv_after);
    cvMerge(Hchannel2, Schannel2, Vchannel2, NULL, img_after_con);

    cvCvtColor(img_hsv_after, img_after, CV_HSV2BGR);
    cvCvtColor(img_after_con, img_con, CV_HSV2BGR);

    cvReleaseImage(&img_hsv_after); // Deallocate some images.
    cvReleaseImage(&img_hsv);


    cvReleaseImage(&Hchannel);
    cvReleaseImage(&Schannel);
    cvReleaseImage(&Vchannel);

    cvReleaseImage(&Hchannel2);
	cvReleaseImage(&Schannel2);
	cvReleaseImage(&Vchannel2);

    cvShowImage("After", img_after); // Show the updated image.
    cvShowImage("Contrast", img_con);
}


int main(int argc, char *argv[])
{
  int height,width,step,channels;
  unsigned char *data;

  setbuf(stdout, NULL);

  // load an image
  img= cvLoadImage("b.jpg");
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
  cvNamedWindow("After", 1);
  cvNamedWindow("Contrast", 1);

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



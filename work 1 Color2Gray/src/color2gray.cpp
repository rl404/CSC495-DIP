
//CSC492/495
//pornchai@sit.kmutt.ac.th

#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>

int main(int argc, char *argv[])
{
  IplImage* img = 0;
  int height,width,step,channels;
  unsigned char *data, *data_r, *data_g, *data_b,
  *data_rgb_weighted, *data_rgb_avg, *data_sepia, *data_binary;
  uchar *data_8bits;
  int i,j,k;

  setbuf(stdout, NULL);
  // load an image
  img= cvLoadImage("pepper.tiff");
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
  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
  cvMoveWindow("mainWin", 10, 10); //positioned at (x,y).

  cvNamedWindow( "Copy" );
  cvNamedWindow( "Gray scale (weighted)" );
  cvNamedWindow( "Gray scale (average)" );
  //cvNamedWindow( "Use red color channel" );
  //cvNamedWindow( "Use green color channel" );
  //cvNamedWindow( "Use blue color channel" );
  cvNamedWindow( "8 bit gray scale (one channel)" );
  cvNamedWindow( "Binary" );
  cvNamedWindow( "Sepia tone" );

  // Create an image for the output
  IplImage* copy = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 ); //three channels.
  cvCopy(img, copy);

  IplImage* out_r = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
  IplImage* out_g = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
  IplImage* out_b = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
  IplImage* out_rgb_avg = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
  IplImage* out_rgb_weighted = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
  IplImage* out_8bits = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 ); //one channel.
  IplImage* out_binary = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
  IplImage* out_sepia = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );

  data_rgb_avg = (uchar *) out_rgb_avg->imageData;
  data_rgb_weighted = (uchar *) out_rgb_weighted->imageData;
  data_r = (uchar *) out_r->imageData;
  data_g = (uchar *) out_g->imageData;
  data_b = (uchar *) out_b->imageData;
  data_8bits = (uchar *) out_8bits->imageData;
  data_binary = (uchar *) out_binary->imageData;
  data_sepia = (uchar *) out_sepia->imageData;

  int intensity_val_avg = 0;
  uchar intensity_val_weighted = 0;

  for(i=0;i<height;i++){
	  for(j=0;j<width;j++){

		  intensity_val_avg = 0;
		  for(k=0;k<channels;k++){
			  intensity_val_avg += data[i*step+j*channels+k];
		  }
		  //Intensity_val_avg is equal to (R+G+B)/3.0;
		  intensity_val_avg = (uchar) (intensity_val_avg /3.0);

		  // Weighted intensity (Y) of RGB.
		  // Y = 0.2126*R + 0.7152*G + 0.0722*B.
		  intensity_val_weighted = (uchar) (0.2126 * data[i*step+j*channels+2])+
		  				  (uchar) (0.7152 * data[i*step+j*channels+1]) +
		  				  (uchar) (0.0722 * data[i*step+j*channels+0]);

		  // Three channels.
		  for(k=0;k<channels;k++){
			  data_rgb_avg[i*step+j*channels+k]= (uchar) intensity_val_avg;
			  data_rgb_weighted[i*step+j*channels+k]= (uchar) intensity_val_weighted;
			  data_r[i*step+j*channels+k]= data[i*step+j*channels+2];
			  data_g[i*step+j*channels+k]= data[i*step+j*channels+1];
			  data_b[i*step+j*channels+k]= data[i*step+j*channels+0];
		  }

		  // One channel.
		  int step_3 = step/3;
		  data_8bits[i*step_3+j]= (uchar) intensity_val_weighted;

		  // binary = black white 128
		  int blackwhite = 0;
		  blackwhite = data[i*step+j*channels+2]*11 + data[i*step+j*channels+1]*16 + data[i*step+j*channels+0]*5;
		  blackwhite = (uchar) (blackwhite/32);

		  if(blackwhite>=128){blackwhite=255;}

		  data_binary[i*step_3+j] = (uchar) blackwhite;


		  //sepia tone
		  /*outputRed = (inputRed * .393) + (inputGreen *.769) + (inputBlue * .189)
		    outputGreen = (inputRed * .349) + (inputGreen *.686) + (inputBlue * .168)
		    outputBlue = (inputRed * .272) + (inputGreen *.534) + (inputBlue * .131)*/

		  data_sepia[i*step+j*channels+2] = .393 * data[i*step+j*channels+2] + .768 * data[i*step+j*channels+1] + .189 * data[i*step+j*channels+0];
		  data_sepia[i*step+j*channels+1] = .349 * data[i*step+j*channels+2] + .686 * data[i*step+j*channels+1] + .168 * data[i*step+j*channels+0];
		  data_sepia[i*step+j*channels+0] = .272 * data[i*step+j*channels+2] + .534 * data[i*step+j*channels+1] + .131 * data[i*step+j*channels+0];

		  if(data_sepia[i*step+j*channels+2]>=255){data_sepia[i*step+j*channels+2]=255;}
		  if(data_sepia[i*step+j*channels+1]>=255){data_sepia[i*step+j*channels+1]=255;}
		  if(data_sepia[i*step+j*channels+0]>=255){data_sepia[i*step+j*channels+0]=255;}
	  }
  }

  // show the image
  cvShowImage("mainWin", img );
  cvShowImage("Copy", copy);
  cvShowImage("Gray scale (average)", out_rgb_avg);
  cvShowImage("Gray scale (weighted)", out_rgb_weighted);
  //cvShowImage("Use red color channel", out_r);
  //cvShowImage("Use green color channel", out_g);
  //cvShowImage("Use blue color channel", out_b);
  cvShowImage("8 bit gray scale (one channel)", out_8bits);
  cvShowImage("Binary", out_binary);
  cvShowImage("Sepia", out_sepia);

  // wait for a key
  cvWaitKey(0);

  // release the image
  cvReleaseImage(&img);
  cvReleaseImage(&copy);
  cvReleaseImage(&out_rgb_avg);
  cvReleaseImage(&out_rgb_weighted);
  cvReleaseImage(&out_r);
  cvReleaseImage(&out_g);
  cvReleaseImage(&out_b);
  cvReleaseImage(&out_8bits);
  cvReleaseImage(&out_binary);
  cvReleaseImage(&out_sepia);

  return 0;
}



//Axel 56130500236

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/core.hpp>
#include <iostream>
using namespace std;
using namespace cv; //important.
#include <math.h>

float d0;

double pixelDistance(double u, double v)
{
    return sqrt(u*u + v*v);
}

double gaussianLowPassCoeff(float u, float v, float d0)
{
	float d = pixelDistance(u, v);
    return exp((-d*d) / (2*d0*d0));
}

double gaussianHighPassCoeff(float u, float v, float d0)
{
	return (1.0 - gaussianLowPassCoeff(u,v,d0));
}

Mat createGaussianLowPassOrHighPassFilter(int type, Size size, float cutoffInPixels)
{
    Mat gf(size, CV_32F);

    Point center(size.width / 2, size.height / 2);

    if (type == 0){ //low pass
		for(int u = 0; u < gf.rows; u++)
		{
			for(int v = 0; v < gf.cols; v++)
			{
				gf.at<float>(u, v) = gaussianLowPassCoeff(u - center.x, v - center.y, cutoffInPixels);
			}
		}
    } else { //high pass
    	for(int u = 0; u < gf.rows; u++)
    	{
    		for(int v = 0; v < gf.cols; v++)
    		{
    			gf.at<float>(u, v) = gaussianHighPassCoeff(u - center.x, v - center.y, cutoffInPixels);
    		}
    	}
    }

    // Make two planes for convolution in a frequency domain.
	vector<Mat> ch;
	ch.push_back(gf);
	ch.push_back(gf);
	Mat complexImg;
	merge(ch, complexImg);
	gf.release();

    return complexImg;
}

double idealLowPassCoeff(float u, float v, float d0, int m, int n)
{
	float d = sqrt(pow((u-m/2),2) + pow((v-n/2),2));

	if(d>d0){return 0;}
	else{return 1;}
}

double idealHighPassCoeff(float u, float v, float d0, int m, int n)
{
	return (1.0 - idealLowPassCoeff(u,v,d0,m,n));
}

Mat createIdealLowPassOrHighPassFilter(int type, Size size, float cutoffInPixels, int m, int n)
{
    Mat gf(size, CV_32F);

    if (type == 0){ //low pass
		for(int u = 0; u < gf.rows; u++)
		{
			for(int v = 0; v < gf.cols; v++)
			{
				gf.at<float>(u, v) = idealLowPassCoeff(u, v, cutoffInPixels, m, n);
			}
		}
    } else { //high pass
    	for(int u = 0; u < gf.rows; u++)
    	{
    		for(int v = 0; v < gf.cols; v++)
    		{
    			gf.at<float>(u, v) = idealHighPassCoeff(u, v, cutoffInPixels, m, n);
    		}
    	}
    }

    // Make two planes for convolution in a frequency domain.
	vector<Mat> ch;
	ch.push_back(gf);
	ch.push_back(gf);
	Mat complexImg;
	merge(ch, complexImg);
	gf.release();

    return complexImg;
}

double butterWorthLowPassCoeff(float u, float v, float d0, int n)
{
	float d = pixelDistance(u, v);
	return ( 1 / (1 + pow((float) (d / d0), (float) (2 * n))));
}

double butterWorthHighPassCoeff(float u, float v, float d0, int n)
{
	float d = pixelDistance(u, v);
	return ( 1 / (1 + pow((float) (d0 / d), (float) (2 * n))));
}

Mat createButterWorthLowPassOrHighPassFilter(int type, Size size, float cutoffInPixels, int n)
{
    Mat gf(size, CV_32F);

    Point center(size.width / 2, size.height / 2);

    if (type == 0){ //low pass
		for(int u = 0; u < gf.rows; u++)
		{
			for(int v = 0; v < gf.cols; v++)
			{
				gf.at<float>(u, v) = butterWorthLowPassCoeff(u - center.x, v - center.y, cutoffInPixels, n);
			}
		}
    } else { //high pass
    	for(int u = 0; u < gf.rows; u++)
    	{
    		for(int v = 0; v < gf.cols; v++)
    		{
    			gf.at<float>(u, v) = butterWorthHighPassCoeff(u - center.x, v - center.y, cutoffInPixels, n);
    		}
    	}
    }

    // Make two planes for convolution in a frequency domain.
	vector<Mat> ch;
	ch.push_back(gf);
	ch.push_back(gf);
	Mat complexImg;
	merge(ch, complexImg);
	gf.release();

    return complexImg;
}

void shiftDFT(Mat& fImage )
{
  	Mat tmp, q0, q1, q2, q3;

	fImage = fImage(Rect(0, 0, fImage.cols & -2, fImage.rows & -2));

	int cx = fImage.cols/2;
	int cy = fImage.rows/2;

	q0 = fImage(Rect(0, 0, cx, cy));
	q1 = fImage(Rect(cx, 0, cx, cy));
	q2 = fImage(Rect(0, cy, cx, cy));
	q3 = fImage(Rect(cx, cy, cx, cy));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}


int main(int argc, char ** argv)
{
    Mat I = imread("a.png", CV_LOAD_IMAGE_GRAYSCALE);
    if( I.empty()){
    	cout << "Could not open the image file.\n";
        return -1;
    }
    imshow("Input Image", I);

    // FOURIER TRANSFORMATION --------------------------------------------
    Mat padded; //expand input image to optimal size
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols ); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    dft(complexI, complexI);
    split(complexI, planes);
    Mat magI;
    magnitude(planes[0], planes[1], magI);
    magI += Scalar::all(1);
    log(magI, magI);
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
    shiftDFT(magI);
    normalize(magI, magI, 0, 1, CV_MINMAX);
    //imshow("Spectrum magnitude", magI);

    // FILTERING ----------------------------------------------------------
    shiftDFT(complexI);
    Mat gf;

    // GAUSSIAN LOW + HIGH PASS
    d0 = 0.1 * complexI.rows;
    //gf = createGaussianLowPassOrHighPassFilter(0, complexI.size(), d0);

    // IDEAL LOW + HIGH PASS
    d0 = 80;
    //gf = createIdealLowPassOrHighPassFilter(0, complexI.size(), d0, m, n);

    // BUTTERWORTH LOW + HIGH PASS
    d0 = 80;
    gf = createButterWorthLowPassOrHighPassFilter(0, complexI.size(), d0, 2);

    Mat gPlanes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    split(gf, gPlanes);
    imshow("Filter", gPlanes[0]);
    Mat result;
	mulSpectrums(complexI,gf,result,DFT_COMPLEX_OUTPUT);
	shiftDFT(result);
	Mat InverseImg;
	dft(result,InverseImg, DFT_INVERSE|DFT_SCALE);

	// INVERSE FOURIER TRANSFORMATION --------------------------------------
	Mat invPlanes[2];
	split(InverseImg, invPlanes);
	Mat invImg;
	invImg = invPlanes[0];
	Mat finalImage;
	invImg.convertTo(finalImage, CV_8U);
	normalize(finalImage, finalImage, 0, 255, CV_MINMAX);
	imshow("Filtered image", finalImage);

	// SAVE THE FILTERED IMAGE ---------------------------------------------
	imwrite( "result.jpg", finalImage );

    waitKey();
    return 0;
}

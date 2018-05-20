#ifndef  _Tuan_Kyou_

#define _Tuan_Kyou_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdio>
using namespace cv;
using namespace std;
#define RED_CHANNEL 2
#define GREEN_CHANNEL 3
#define BLUE_CHANNEL 4


class IplImageWrapper {
public:  
	IplImage * _srcImg;  
	IplImage* _destImg;  
	int _width, _height;  
	IplImage *r_plane, *b_plane, *g_plane; 
	IplImage *neg;
public:  
	IplImageWrapper();  
	~IplImageWrapper();    
	void LoadImage(char* path);  
	void ShowImage(char* windowName, int img = 0);  
	void RGB2GRAY1();  
	void RGB2GRAY2();   
	void PixToPlane();   
	void PixToPlane2();
	void NegativeCV();
	void NegativeCV2();
	void Add(int a);
	void Add(int a, int b, int c);

};

IplImageWrapper::IplImageWrapper() {
	_srcImg = NULL;
	_destImg = NULL;
}

IplImageWrapper::~IplImageWrapper() {
	if (_srcImg != NULL) {
		cvReleaseImage(&_srcImg);
	}
	if (_destImg != NULL) {
		cvReleaseImage(&_destImg);
	}
}

void IplImageWrapper::LoadImage(char *path) {
	if (_srcImg != NULL) {
		cvReleaseImage(&_srcImg);
	}
	if (strlen(path) == 0) {
		return;
	}
	_srcImg = cvLoadImage(path);
	if (_srcImg != NULL) {
		_width = _srcImg->width;
		_height = _srcImg->height;
	}
}

void IplImageWrapper::ShowImage(char* windowName, int img) {
	IplImage* image = NULL;  
	if (img == 0)   
		image = _srcImg; 
	else if (img == 1)   
		image = _destImg;  
	else if (img == 2)   
		image = b_plane;
	else if (img == 3)   
		image = g_plane;  
	else if (img == 4)   
		image = r_plane;  
	else if (img == 5)
		image = neg;
	if (image != NULL) {
		cvNamedWindow(windowName, CV_GUI_NORMAL);
		cvShowImage(windowName, image);
		cvWaitKey();
		cvDestroyWindow(windowName);
	}
}

void IplImageWrapper::RGB2GRAY1() { 
	if (_destImg != NULL) 
		cvReleaseImage(&_destImg); 
	_destImg = cvCreateImage(cvSize(_width, _height), IPL_DEPTH_8U, 1); 
	cvCvtColor(_srcImg, _destImg, CV_RGB2GRAY); 
}

void IplImageWrapper::NegativeCV() {
	RGB2GRAY1();
	if (neg != NULL) {
		cvReleaseImage(&neg);
	}
	neg = cvCloneImage(_destImg);
	int step = neg->widthStep;
	int channels = neg->nChannels;
	int i, j;
	uchar* data;
	data = (uchar*)neg->imageData;
	for (i = 0; i < _height; i++)
		for (j = 0; j < _width; j++) {
			data[i*step + j * channels] = ~data[i*step + j * channels];
			//data[i*step + j * channels + 1] = ~data[i*step + j * channels + 1];
			//data[i*step + j * channels + 2] = ~data[i*step + j * channels + 2];
		}
}

void IplImageWrapper::Add(int x) {
	RGB2GRAY1();
	if (neg != NULL) {
		cvReleaseImage(&neg);
	}
	neg = cvCloneImage(_destImg);
	int step = neg->widthStep;
	int channels = neg->nChannels;
	int i, j;
	uchar* data;
	data = (uchar*)neg->imageData;
	for (i = 0; i < _height; i++)
		for (j = 0; j < _width; j++) {
			data[i*step + j * channels] += x;
			//data[i*step + j * channels + 1] += x;
			//data[i*step + j * channels + 2] += x;
		}
}

void IplImageWrapper::Add(int a, int b , int c) {
	neg = cvCloneImage(_srcImg);
	int step = neg->widthStep;
	int channels = neg->nChannels;
	int i, j;
	uchar* data;
	data = (uchar*)neg->imageData;
	for (i = 0; i < _height; i++)
		for (j = 0; j < _width; j++) {
			data[i*step + j * channels] += a;
			data[i*step + j * channels + 1] += b;
			data[i*step + j * channels + 2] += c;
		}
}

void IplImageWrapper::NegativeCV2() {
	neg = cvCloneImage(_srcImg);
	int step = neg->widthStep;
	int channels = neg->nChannels;
	int i, j;
	uchar* data;
	data = (uchar*)neg->imageData;
	for (i = 0; i < _height; i++)
		for (j = 0; j < _width; j++) {
			data[i*step + j * channels] = ~data[i*step + j * channels];
			data[i*step + j * channels + 1] = ~data[i*step + j * channels + 1];
			data[i*step + j * channels + 2] = ~data[i*step + j * channels + 2];
		}
}


void IplImageWrapper::RGB2GRAY2() {
	int step, channels;  
	step = _srcImg->widthStep;  
	channels = _srcImg->nChannels;

	_destImg = cvCloneImage(_srcImg);  
	uchar* dataGray;  
	dataGray = (uchar*)_destImg->imageData;  
	int i, j;  
	//Gray = 0.299 * R + 0.587 * G + 0.114 * B 
	for (i = 0; i < _height; i++)   
		for (j = 0; j < _width; j++) {
		uchar r, g, b, gray_value;  
		b = dataGray[i*step + j * channels]; 
		g = dataGray[i*step + j * channels + 1];
		r = dataGray[i*step + j * channels + 2];    
		gray_value = (int)(r*0.3 + g * 0.59 + b * 0.11);   
		dataGray[i*step + j * channels] = 
			dataGray[i*step + j * channels + 1] = 
			dataGray[i*step + j * channels + 2] = gray_value;
	}
}

void IplImageWrapper::PixToPlane() { 
	r_plane = cvCreateImage(cvSize(_width, _height), 8, 3); 
	g_plane = cvCreateImage(cvSize(_width, _height), 8, 3); 
	b_plane = cvCreateImage(cvSize(_width, _height), 8, 3);  
	cvSplit(_srcImg, b_plane, g_plane, r_plane, NULL); 
}
void IplImageWrapper::PixToPlane2() {

	int i, j, step, channels;  
	step = _srcImg->widthStep;  
	channels = _srcImg->nChannels;

	b_plane = cvCloneImage(_srcImg); 
	g_plane = cvCloneImage(_srcImg); 
	r_plane = cvCloneImage(_srcImg);

	uchar *dataB, *dataG, *dataR;  
	dataB = (uchar *)b_plane->imageData;  
	dataG = (uchar *)g_plane->imageData;  
	dataR = (uchar *)r_plane->imageData;

	for (i = 0; i < _height; i++)   
		for (j = 0; j < _width; j++) {
			dataB[i*step + j * channels + 1] = 0;    
			dataB[i*step + j * channels + 2] = 0;
			dataG[i*step + j * channels] = 0;    
			dataG[i*step + j * channels + 2] = 0;
			dataR[i*step + j * channels] = 0;   
			dataR[i*step + j * channels + 1] = 0;
		}
}





void Prog1() { // Show picture
	IplImage *imp = cvLoadImage("Lena.png", CV_LOAD_IMAGE_COLOR);
	if (imp != NULL) {
		cvNamedWindow("Image");
		cvShowImage("Image", imp);
		cvWaitKey();
		cvReleaseImage(&imp);
		cvDestroyWindow("Image");
	}
}

void Prog2() {// Show video
	cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
	CvCapture *capture = cvCreateFileCapture("hi.mp4");
	IplImage *frame;
	while (1) {
		frame = cvQueryFrame(capture);
		if (!frame)
			break;
		cvShowImage("Video", frame);
		char c = cvWaitKey(100);
		if (c == 'a')
			break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Video");
}

void Prog3() {// Show grayscale video
	cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
	CvCapture *capture = cvCreateFileCapture("hi.mp4");
	IplImage *frame;
	IplImage *gray;
	while (1) {
		frame = cvQueryFrame(capture);
		if (!frame)
			break;
		gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		cvCvtColor(frame, gray, CV_RGB2GRAY);
		cvShowImage("Video", gray);
		cvReleaseImage(&gray);
		char c = cvWaitKey(60);
		if (c == 'a')
			break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Video");
}

void Prog4() { // PixToPlane
	/*
	1. cvCvtPixToPlane

	It is same with split function in current version.
	So channels of the input image are seperately stored.

	ex) old version
	iplImage img; //BGR 3 colors image
	iplImage B,G,R;
	cvCvtPixToPlane(img, B, G, R, NULL);


	ex) current version

	Mat img; //BGR 3 colors image
	vector< Mat > BGR(3);
	split(img, BGR);
	BGR[0]; //B Mat
	BGR[1]; //G Mat
	BGR[2]; //R Mat

	For C:
	cvSplit
	*/
	IplImage *imp = cvLoadImage("Lena.png", CV_LOAD_IMAGE_COLOR);
	if (imp != NULL) {
		vector<IplImage*> RGB(3);
		for (int i = 0; i < 3; i++) {
			RGB[i] = cvCreateImage(cvGetSize(imp), imp->depth, 1);
		}
		cvSplit(imp, RGB[0], RGB[1], RGB[2], NULL);
		cvNamedWindow("Image");
		cvShowImage("Image", imp);
		cvWaitKey();

		cvDestroyWindow("Image");
		for (int i = 0; i < 3; i++) {
			cvNamedWindow("Image");
			cvShowImage("Image", RGB[i]);
			cvWaitKey();
			cvDestroyWindow("Image");
		}

		IplImage *merge = cvCreateImage(cvGetSize(imp), imp->depth, 3);
		cvMerge(RGB[0], RGB[1], RGB[2], 0, merge);
		cvNamedWindow("Image");
		cvShowImage("Image", merge);
		cvWaitKey();
		cvDestroyWindow("Image");
		cvReleaseImage(&imp);
		cvReleaseImage(&merge);
		for (int i = 0; i < 3; i++) {
			cvReleaseImage(&RGB[i]);
		}
	}
}
#endif // ! _Tuan_Kyou_









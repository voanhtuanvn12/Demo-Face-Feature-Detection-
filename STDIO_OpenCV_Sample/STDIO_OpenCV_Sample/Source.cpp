#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>


/*
using namespace std;
using namespace cv;

// Functions for facial feature detection
static void help();
static void detectFaces(Mat&, vector<Rect_<int> >&, string);
static void detectEyes(Mat&, vector<Rect_<int> >&, string);
static void detectNose(Mat&, vector<Rect_<int> >&, string);
static void detectMouth(Mat&, vector<Rect_<int> >&, string);
static void detectFacialFeaures(Mat&, const vector<Rect_<int> >, string, string, string);

string input_image_path;
string face_cascade_path, eye_cascade_path, nose_cascade_path, mouth_cascade_path;

int main(int argc, char** argv)
{
	cv::CommandLineParser parser(argc, argv,
		"{eyes||}{nose||}{mouth||}{help h||}");
	if (parser.has("help"))
	{
		help();
		return 0;
	}
	input_image_path = parser.get<string>(0);
	face_cascade_path = parser.get<string>(1);
	eye_cascade_path = parser.has("eyes") ? parser.get<string>("eyes") : "";
	nose_cascade_path = parser.has("nose") ? parser.get<string>("nose") : "";
	mouth_cascade_path = parser.has("mouth") ? parser.get<string>("mouth") : "";
	if (input_image_path.empty() || face_cascade_path.empty())
	{
		cout << "IMAGE or FACE_CASCADE are not specified";
		return 1;
	}
	// Load image and cascade classifier files
	Mat image;
	image = imread(input_image_path);

	// Detect faces and facial features
	vector<Rect_<int> > faces;
	detectFaces(image, faces, face_cascade_path);
	detectFacialFeaures(image, faces, eye_cascade_path, nose_cascade_path, mouth_cascade_path);

	imshow("Result", image);

	waitKey(0);
	return 0;
}

static void help()
{
	cout << "\nThis file demonstrates facial feature points detection using Haarcascade classifiers.\n"
		"The program detects a face and eyes, nose and mouth inside the face."
		"The code has been tested on the Japanese Female Facial Expression (JAFFE) database and found"
		"to give reasonably accurate results. \n";

	cout << "\nUSAGE: ./cpp-example-facial_features [IMAGE] [FACE_CASCADE] [OPTIONS]\n"
		"IMAGE\n\tPath to the image of a face taken as input.\n"
		"FACE_CASCSDE\n\t Path to a haarcascade classifier for face detection.\n"
		"OPTIONS: \nThere are 3 options available which are described in detail. There must be a "
		"space between the option and it's argument (All three options accept arguments).\n"
		"\t-eyes=<eyes_cascade> : Specify the haarcascade classifier for eye detection.\n"
		"\t-nose=<nose_cascade> : Specify the haarcascade classifier for nose detection.\n"
		"\t-mouth=<mouth-cascade> : Specify the haarcascade classifier for mouth detection.\n";


	cout << "EXAMPLE:\n"
		"(1) ./cpp-example-facial_features image.jpg face.xml -eyes=eyes.xml -mouth=mouth.xml\n"
		"\tThis will detect the face, eyes and mouth in image.jpg.\n"
		"(2) ./cpp-example-facial_features image.jpg face.xml -nose=nose.xml\n"
		"\tThis will detect the face and nose in image.jpg.\n"
		"(3) ./cpp-example-facial_features image.jpg face.xml\n"
		"\tThis will detect only the face in image.jpg.\n";

	cout << " \n\nThe classifiers for face and eyes can be downloaded from : "
		" \nhttps://github.com/opencv/opencv/tree/master/data/haarcascades";

	cout << "\n\nThe classifiers for nose and mouth can be downloaded from : "
		" \nhttps://github.com/opencv/opencv_contrib/tree/master/modules/face/data/cascades\n";
}

static void detectFaces(Mat& img, vector<Rect_<int> >& faces, string cascade_path)
{
	CascadeClassifier face_cascade;
	face_cascade.load(cascade_path);

	face_cascade.detectMultiScale(img, faces, 1.15, 3, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	return;
}

static void detectFacialFeaures(Mat& img, const vector<Rect_<int> > faces, string eye_cascade,
	string nose_cascade, string mouth_cascade)
{
	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		// Mark the bounding box enclosing the face
		Rect face = faces[i];
		rectangle(img, Point(face.x, face.y), Point(face.x + face.width, face.y + face.height),
			Scalar(255, 0, 0), 1, 4);

		// Eyes, nose and mouth will be detected inside the face (region of interest)
		Mat ROI = img(Rect(face.x, face.y, face.width, face.height));

		// Check if all features (eyes, nose and mouth) are being detected
		bool is_full_detection = false;
		if ((!eye_cascade.empty()) && (!nose_cascade.empty()) && (!mouth_cascade.empty()))
			is_full_detection = true;

		// Detect eyes if classifier provided by the user
		if (!eye_cascade.empty())
		{
			vector<Rect_<int> > eyes;
			detectEyes(ROI, eyes, eye_cascade);

			// Mark points corresponding to the centre of the eyes
			for (unsigned int j = 0; j < eyes.size(); ++j)
			{
				Rect e = eyes[j];
				circle(ROI, Point(e.x + e.width / 2, e.y + e.height / 2), 3, Scalar(0, 255, 0), -1, 8);
				// rectangle(ROI, Point(e.x, e.y), Point(e.x+e.width, e.y+e.height),
				// Scalar(0, 255, 0), 1, 4);
			}
		}

		// Detect nose if classifier provided by the user
		double nose_center_height = 0.0;
		if (!nose_cascade.empty())
		{
			vector<Rect_<int> > nose;
			detectNose(ROI, nose, nose_cascade);

			// Mark points corresponding to the centre (tip) of the nose
			for (unsigned int j = 0; j < nose.size(); ++j)
			{
				Rect n = nose[j];
				circle(ROI, Point(n.x + n.width / 2, n.y + n.height / 2), 3, Scalar(0, 255, 0), -1, 8);
				nose_center_height = (n.y + n.height / 2);
			}
		}

		// Detect mouth if classifier provided by the user
		double mouth_center_height = 0.0;
		if (!mouth_cascade.empty())
		{
			vector<Rect_<int> > mouth;
			detectMouth(ROI, mouth, mouth_cascade);

			for (unsigned int j = 0; j < mouth.size(); ++j)
			{
				Rect m = mouth[j];
				mouth_center_height = (m.y + m.height / 2);

				// The mouth should lie below the nose
				if ((is_full_detection) && (mouth_center_height > nose_center_height))
				{
					rectangle(ROI, Point(m.x, m.y), Point(m.x + m.width, m.y + m.height), Scalar(0, 255, 0), 1, 4);
				}
				else if ((is_full_detection) && (mouth_center_height <= nose_center_height))
					continue;
				else
					rectangle(ROI, Point(m.x, m.y), Point(m.x + m.width, m.y + m.height), Scalar(0, 255, 0), 1, 4);
			}
		}

	}

	return;
}

static void detectEyes(Mat& img, vector<Rect_<int> >& eyes, string cascade_path)
{
	CascadeClassifier eyes_cascade;
	eyes_cascade.load(cascade_path);

	eyes_cascade.detectMultiScale(img, eyes, 1.20, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	return;
}

static void detectNose(Mat& img, vector<Rect_<int> >& nose, string cascade_path)
{
	CascadeClassifier nose_cascade;
	nose_cascade.load(cascade_path);

	nose_cascade.detectMultiScale(img, nose, 1.20, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	return;
}

static void detectMouth(Mat& img, vector<Rect_<int> >& mouth, string cascade_path)
{
	CascadeClassifier mouth_cascade;
	mouth_cascade.load(cascade_path);

	mouth_cascade.detectMultiScale(img, mouth, 1.20, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	return;
}
*/

#include <algorithm>
#include <functional>
using namespace std;
using namespace cv;

bool mycompare(Rect a, Rect b) {
	return a.x < b.x;
}

// Function Headers
void detectAndDisplay(Mat frame);

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string left_eye_cascade_name = "haarcascade_lefteye_2splits.xml";
string right_eye_cascade_name = "haarcascade_righteye_2splits.xml";
string smile_eye_cascade_name = "haarcascade_smile.xml";
string xxx = "haarcascade_eye.xml";
CascadeClassifier face_cascade, right_eye_cascade, left_eye_cascade, smile_cascade, xxx_cascade;
string window_name = "Capture - Face detection";
int filenumber; // Number of file to be saved
string filename;

// Function main
int main(void)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) {
		printf("--(!)Error loading\n");
		return (-1);
	}
	if (!right_eye_cascade.load(right_eye_cascade_name)) {
		printf("--(!)Error loading\n");
		return (-1);
	}
	if (!left_eye_cascade.load(left_eye_cascade_name)) {
		printf("--(!)Error loading\n");
		return (-1);
	}
	if (!smile_cascade.load(smile_eye_cascade_name)) {
		printf("--(!)Error loading\n");
		return (-1);
	}

	if (!xxx_cascade.load(xxx)) {
		printf("--(!)Error loading\n");
		return (-1);
	}
	// Read the image file
	Mat frame = imread("c.jpg");

	// Apply the classifier to the frame
	if (!frame.empty()) {
		detectAndDisplay(frame);
	}
	else {
		printf(" --(!) No captured frame -- Break!");
	}

	while (1) {
		int c = waitKey(10);
		if (c == 'a')
			break;
	}


	return 0;
}

// Function detectAndDisplay
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	std::vector<Rect> right_eye, left_eye, smile, xxx_xxx;
	Mat frame_gray;
	Mat crop;
	Mat res;
	Mat gray;
	string text;
	stringstream sstm;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	//GaussianBlur(frame_gray, frame_gray, Size(0, 0),2.5);
	equalizeHist(frame_gray, frame_gray);

	// Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	right_eye_cascade.detectMultiScale(frame_gray, right_eye, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	left_eye_cascade.detectMultiScale(frame_gray, left_eye, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	smile_cascade.detectMultiScale(frame_gray, smile, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	xxx_cascade.detectMultiScale(frame_gray, xxx_xxx, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	// Set Region of Interest
	cv::Rect roi_b;
	cv::Rect roi_c;

	cv::Rect r_e_x, r_e_y;


	size_t ic = 0; // ic is index of current element
	int ac = 0; // ac is area of current element

	size_t ib = 0; // ib is index of biggest element
	int ab = 0; // ab is area of biggest element


	// Giả sử như có 1 face 
	if (faces.size() == 1) {
		int max_x = 0, max_y = 0;
		double distance = 0;

		crop = frame(faces[0]);
		resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
		cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

										   // Form a filename
		filename = "";
		stringstream ssfn;
		ssfn << filenumber << ".png";
		filename = ssfn.str();
		filenumber++;

		imwrite(filename, gray);



		Point pt1(faces[0].x, faces[0].y); // Display detected faces on main window - live stream from camera
		Point pt2((faces[0].x + faces[0].height), (faces[0].y + faces[0].width));
		rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);

		// Tìm 2 điểm trong mắt mà cách xa nhau nhất
		vector<Rect> v;
		for (int i = 0; i < left_eye.size(); i++) {
			v.push_back(left_eye[i]);
		}
		for (int i = 0; i < right_eye.size(); i++) {
			v.push_back(right_eye[i]);
		}

		sort(v.begin(), v.end(), mycompare);
		rectangle(frame, Point(v[0].x, v[0].y), Point(v[0].x + v[0].width, v[0].y + v[0].height), Scalar(0, 255, 0), 2, 8, 0);
		int lastE = v.size() - 1;
		rectangle(frame, Point(v[lastE].x, v[lastE].y), Point(v[lastE].x + v[lastE].width, v[lastE].y + v[lastE].height), Scalar(0, 255, 0), 2, 8, 0);
		distance = double(v[lastE].x*v[lastE].x + v[lastE].y*v[lastE].y);
		cout << " Vo " << v[0].x << " " << v[0].y << endl;
		cout << "V last " << v[lastE].x << " " << v[lastE].y << endl;
		cout << "face " << faces[0].x << " " << faces[0].y << endl;
		cout << "face 1 " << faces[0].x + faces[0].height << " " << faces[0].y + faces[0].width << endl;
		for (int i = 0; i < smile.size(); i++) {
			cout << smile[i].x << " " << smile[i].y << endl;
			if (smile[i].x >= v[0].x && smile[i].x <= v[lastE].x && smile[i].y > max(v[0].y, v[lastE].y)) {
				cout << smile[i].x << " " << smile[i].y << " vo ne " << endl;
				if (smile[i].y > (faces[0].y + int(faces[0].width * 2.0 / 3.0)) && (smile[i].y)<(faces[0].y + faces[0].width))
					rectangle(frame, Point(smile[i].x, smile[i].y), Point(smile[i].x + smile[i].width, smile[i].y + smile[i].height), Scalar(0, 255, 0), 2, 8, 0);
			}

		}
	}


	else {
		for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces) //

		{
			int max_x = 0, max_y = 0;
			double distance = 0;
			roi_c.x = faces[ic].x;
			roi_c.y = faces[ic].y;
			roi_c.width = (faces[ic].width);
			roi_c.height = (faces[ic].height);

			ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

			roi_b.x = faces[ib].x;
			roi_b.y = faces[ib].y;
			roi_b.width = (faces[ib].width);
			roi_b.height = (faces[ib].height);

			ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

			if (ac > ab)
			{
				ib = ic;
				roi_b.x = faces[ib].x;
				roi_b.y = faces[ib].y;
				roi_b.width = (faces[ib].width);
				roi_b.height = (faces[ib].height);
			}

			crop = frame(roi_b);
			resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
			cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

											   // Form a filename
			filename = "";
			stringstream ssfn;
			ssfn << filenumber << ".png";
			filename = ssfn.str();
			filenumber++;

			imwrite(filename, gray);



			Point pt1(faces[ic].x, faces[ic].y); // Display detected faces on main window - live stream from camera
			Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
			rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);

			Point pt3(right_eye[ic].x, right_eye[ic].y);

			Point pt4((right_eye[ic].x + right_eye[ic].height), (right_eye[ic].y + right_eye[ic].width));
			distance = max(distance, double(right_eye[ic].x*right_eye[ic].x + right_eye[ic].y*right_eye[ic].y));
			//max_x = max(max_x,right_eye[ic].x);
			//max_y = max(max_y, right_eye[ic].y);
			rectangle(frame, pt3, pt4, Scalar(0, 255, 0), 2, 8, 0);

			Point pt5(left_eye[ic].x, left_eye[ic].y);
			Point pt6((left_eye[ic].x + left_eye[ic].height), (left_eye[ic].y + left_eye[ic].width));
			//max_x = max(max_x, left_eye[ic].x);
			//max_y = max(max_y, left_eye[ic].y);
			distance = max(distance, double(left_eye[ic].x*left_eye[ic].x + left_eye[ic].y*left_eye[ic].y));
			rectangle(frame, pt5, pt6, Scalar(0, 255, 0), 2, 8, 0);


			//Point pt7(smile[ic].x, smile[ic].y);
			//Point pt8((smile[ic].x + smile[ic].height), (smile[ic].y + smile[ic].width));
			//rectangle(frame, pt5, pt6, Scalar(0, 255, 0), 2, 8, 0);
			for (int i = 0; i < smile.size(); i++) {
				if (double(smile[i].x*smile[i].x + smile[i].y*smile[i].y) > distance  && smile[i].x >= right_eye[ic].x && smile[i].x <= left_eye[ic].x
					&& smile[i].y > max(right_eye[ic].y, left_eye[ic].x) && smile[i].y > (faces[ic].y + faces[ic].width / 2) && (smile[i].y + smile[i].height) < (faces[ic].y + faces[ic].width))
					rectangle(frame, Point(smile[i].x, smile[i].y), Point(smile[i].x + smile[i].width, smile[i].y + smile[i].height), Scalar(0, 255, 0), 2, 8, 0);
			}
		}
	}





	// Show image
	sstm << "Crop area size: " << roi_b.width << "x" << roi_b.height << " Filename: " << filename;
	text = sstm.str();

	putText(frame, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
	imshow("original", frame);

	if (!crop.empty())
	{
		imshow("detected", crop);
	}
	else
		destroyWindow("detected");
}


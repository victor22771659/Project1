#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/video/tracking.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>

using namespace cv;
using namespace std;


//Store click point
/*
Point2f point;
static void onMouse(int event, int x, int y, int , void* )
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}*/


int main(int argc, const char** argv)
{
	Mat frame1,preFrame;//Q1-2
	FILE *fp;
	vector<Point2f> iniPoints;
	int *coordination = new int[14];
	char *buf = new char[10];
	int x=0,y=0;
	int num_trackPoint = 7;
	fp = fopen("hw3.1.txt", "r");
	fstream cfp;
	if (fp == NULL){
		cout << "Can't find file!!Error!!" << endl;
		return -1;
	}

	namedWindow("Frame1", 1);
	VideoCapture cap("HW_video.mp4");

	if (!cap.isOpened()){
		std::cout << "Cannot open the video file" << std::endl;
		return -1;
	}

	cap.read(frame1);
	frame1.copyTo(preFrame);//Q1-2
	fscanf(fp, "%*[^\n]");
	for (size_t i = 0; fscanf(fp, "%s %d %d", buf, &x, &y) != EOF; i++){
		iniPoints.push_back(Point(x, y));
		circle(frame1, iniPoints[i], 2, Scalar(0, 255, 0), 2, 8, 0);
		cout << "Point" << i + 1 << "=" << iniPoints[i] << endl;
	}
	
	imshow("Frame1", frame1);
	//setMouseCallback("Frame1", onMouse, 0);

	//Q1-2
	Mat gray, preGray, nextFrame;
	vector<Point2f> pointsRegister,trackPoints[2];
	vector<uchar> status;
	vector<float> err;
	Size winSize(9,9);
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.03);
	size_t num_Frame;
	cout << "Please type a number for define tracking time:";
	cin >> num_Frame;

	//Copy iniPoints to trackPoints[0]
	trackPoints[0].resize(iniPoints.size());
	memcpy(&trackPoints[0][0], &iniPoints[0], iniPoints.size() * sizeof(Point2f));

	//Read numbe of frame then store tracking points
	cvtColor(preFrame, preGray, COLOR_BGR2GRAY);
	for (int j = 0; j<num_trackPoint; j++){
		pointsRegister.push_back(trackPoints[0][j]);
	}

	for (size_t i = 0; i < num_Frame; i++){
		cap.read(nextFrame);
		cvtColor(nextFrame, gray, COLOR_BGR2GRAY);
		calcOpticalFlowPyrLK(preGray, gray, trackPoints[0], trackPoints[1], status, err, winSize,
			3, termcrit, 0, 0.001);

		for (int k = 0; k<num_trackPoint;k++){
			pointsRegister.push_back(trackPoints[1][k]);
		}
		memcpy(&trackPoints[0][0], &trackPoints[1][0], trackPoints[1].size()*sizeof(Point2f));
		gray.copyTo(preGray);
	}
	
	cfp.open("hw3.2.txt", ios::out);
	int tmp1,tmp2;
	for (size_t l = 0; l < pointsRegister.size(); l++){
			cout << pointsRegister[l] << endl;
			tmp1 = (int)pointsRegister[l].x;
			tmp2 = (int)pointsRegister[l].y;
			cfp << tmp1 << " " << tmp2 << "\r\n";
		}
	cap.release();
	cfp.close();
	
	//Q2
	Mat tmpVideo;
	namedWindow("HW3_video", 1);
	cap.open("HW_video.mp4");
	fp = fopen("hw3.2.txt", "r");
	while (!feof(fp)){
		cap.read(tmpVideo);
		for (int i = 0; i < 7 && fscanf(fp, "%d %d", &x, &y)!=EOF; i++){
			circle(tmpVideo, Point(x, y), 2, Scalar(0, 255, 0), 2, 8, 0);
			cout << Point(x, y) << endl;
		}
		imshow("HW3_video", tmpVideo);
		waitKey(30);
	}
	cap.release();
	
	//Q3
	Mat videoBuf;
	cap.open("HW_video.mp4");
	cap.read(videoBuf);
	size_t n = num_trackPoint;
	int p = 0;
	VideoWriter output_src("point_tracking.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, videoBuf.size(), 1);
	fp = fopen("hw3.3.txt", "r");
	while (!feof(fp) || !videoBuf.empty()){
		for (int i = 0; i < 7 && fscanf(fp, "%d %d", &x, &y) != EOF; i++){
			circle(videoBuf, Point(x, y), 2, Scalar(0, 255, 0), 2, 8, 0);
		}
		if (waitKey(10) > 0)
			break;
		output_src << videoBuf;
		imshow("HW3_video", videoBuf);
		cap.read(videoBuf);
	}
	fclose(fp);
	delete[] coordination;
	delete[] buf;
	waitKey(0);
	return 0;
}

//從檔案讀取開始
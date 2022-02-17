#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"


#include "Armor_find.h"
#include "S_Mat.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

VideoCapture Avideo(const string& videoName = "Armor")
{
	if (videoName == "Armor")
		return VideoCapture("D:/app/resource/vision project/lab/1.mp4");
}

const int v_thresh = 100;
const int v_contourMinArea = 20;
const int v_higherDiff = 143;

int main()
{
	Mat frame;
	VideoCapture cap = Avideo();

	while (cap.isOpened())
	{
		cap >> frame;
		resize(frame, frame, Size(800, 450));					// 16:9

		Armor_find armor(frame);
		bool findArmor = armor.findLightBar(FINDBAR_VER::VER1);
		
		char state = waitKey(27);
		if (state == 'p')
			waitKey();
		else if (state == 27)
			break;
	}

	return 0;
}

//#pragma once
//#include <iostream>
//#include <fstream>
//#include <sstream>
//
//#include "json/json.h";
//#include "json/forwards.h"
//
//using namespace std;
//
//int main()
//{
//	Json::Value jsonItem;
//	jsonItem["item1"] = "one";
//	jsonItem["item2"] = 2;
//	jsonItem["item1.0"] = 1.0;
//	jsonItem["item2.0"] = 2.0;
//
//	jsonItem["object1"]["object1_item1"] = "aaa";
//
//	string str = jsonItem.toStyledString();
//	ofstream out("str.json");
//	out << str;
//	out.close();
//
//	cout << str;
//	
//	ifstream input("str.json");
//	Json::Value root;
//	Json::CharReaderBuilder builder;
//	JSONCPP_STRING errs;
//
//	int bbb;
//	bbb = jsonItem["item1"];
//	if (!parseFromStream(builder, input, &root, &errs))
//	{
//		cout << errs << endl;
//		return -1;
//	}
//	cout << "------------Here input read" << root << endl;
//
//	return 0;
//}

//#include <opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	Mat src = imread("E:/desktop/imgs/lena.jpg");
//
//	for (int i = 0; i < src.rows; i++)
//	{
//		for (int j = 0; j < src.cols; j++)
//		{
//			auto data = src.ptr<Vec3b>(i)[j];
//			cout << data << " ";
//		}
//		cout << endl;
//	}
//	
//}


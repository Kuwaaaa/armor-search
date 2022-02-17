#include "opencv2/calib3d.hpp"

#include "Armor_find.h"
#include "common.h"

#include <algorithm>

#define ARMOR_DEBUG

using namespace std;
using namespace cv;

int Armor_find::v_thresh = 100;
int Armor_find::v_contourMinArea = 20;
int Armor_find::v_higherDiff = 143;

void Armor_find::proProcess()
{
	cvtColor(frame_src, smat["gray"], COLOR_BGR2GRAY);
	threshold(smat["gray"], smat["thresh"], v_thresh, 255, THRESH_BINARY);
}


float Armor::getAngleDiff() const
{
	float angle_diff = abs(l_lightBar.angle - r_lightBar.angle); //get the abs of angle_diff 灯条的角度差
	return angle_diff;
}

float Armor::getDeviationAngle() const
{
	float delta_x = r_lightBar.center.x - l_lightBar.center.x; //Δx
	float delta_y = r_lightBar.center.y - l_lightBar.center.y; //Δy
	float deviationAngle = abs(atan(delta_y / delta_x)) * 180 / CV_PI; //tanθ=Δy/Δx
	return deviationAngle;
}

float Armor::getDislocationX() const
{
	float meanLen = (l_lightBar.length + r_lightBar.length) / 2;
	float xDiff = abs(l_lightBar.center.x - r_lightBar.center.x); //x distance ration y轴方向上的距离比值（y轴距离与灯条平均值的比）
	float xDiff_ratio = xDiff / meanLen;
	return xDiff_ratio;
}

float Armor::getDislocationY() const
{
	float meanLen = (l_lightBar.length + r_lightBar.length) / 2;
	float yDiff = abs(l_lightBar.center.y - r_lightBar.center.y);  //y distance ration x轴方向上的距离比值（x轴距离与灯条平均值的比）
	float yDiff_ratio = yDiff / meanLen;
	return yDiff_ratio;
}

float Armor::getLengthRation() const
{
	float length_diff = abs(l_lightBar.length - r_lightBar.length);
	float lengthDiffRation = length_diff / MAX(l_lightBar.length, r_lightBar.length);
	return lengthDiffRation;
}

bool Armor::isSuitLightBar()
{
#ifdef ARMOR_DEBUG
	cout << "----------------------AngleDiff " << getAngleDiff() << endl
		 << "-----------------------DeviationAngle  " << getDeviationAngle() << endl
	 	 << "---------------------------------DislocationX  " << getDislocationX() << endl 
	 	 << "---------------------------------DislocationY  " << getDislocationY() << endl
		 << "--------------------------------------LengthRation  " << getLengthRation() << endl;
#endif
	return
		this->getAngleDiff()		 < max_angle_diff		&&		// angle difference judge the angleDiff should be less than max_angle_diff 灯条角度差判断，需小于允许的最大角差
		this->getDeviationAngle()	 < max_deviation_angle  &&		// deviation angle judge: the horizon angle of the line of centers of lights 灯条错位度角(两灯条中心连线与水平线夹角)判断 
		this->getDislocationX()		 < max_x_diff_ratio     &&		// dislocation judge: the x and y can not be too far 灯条位置差距 两灯条中心x、y方向差距不可偏大（用比值作为衡量依据）
		this->getDislocationY()		 < max_y_diff_ratio     &&		// dislocation judge: the x and y can not be too far 灯条位置差距 两灯条中心x、y方向差距不可偏大（用比值作为衡量依据）
		this->getLengthRation()		 < max_lengthDiff_ratio;
}

cv::Mat Armor_find::transArmorImg(Point2f src[], Point2f dst[])
{
	Mat warpPerspective_mat = getPerspectiveTransform(src, dst);
	Mat warpPerspective_dst;
	warpPerspective(frame_src, warpPerspective_dst, warpPerspective_mat, frame_size, INTER_NEAREST, BORDER_CONSTANT, Scalar(0));

	return warpPerspective_dst;
	
}

cv::Mat Armor_find::getArmorImg()
{
	for (auto& armor : armors)
	{
		Size lSize(armor.l_lightBar.size.width, armor.l_lightBar.size.height * 2);
		Size rSize(armor.r_lightBar.size.width, armor.r_lightBar.size.height * 2);
		float diffX = (armor.r_lightBar.center.x - armor.l_lightBar.center.x) / 2;
		float height = (lSize.height + rSize.height) / 2;
		float centerX = armor.l_lightBar.center.x + diffX;
		float centerY = (armor.l_lightBar.center.y + armor.r_lightBar.center.y) / 2;

		float angle = (armor.l_lightBar.angle + armor.r_lightBar.angle) / 2;
		Size exSize(diffX, height);

		RotatedRect exLRect(armor.l_lightBar.center, lSize, armor.l_lightBar.angle);
		RotatedRect exRRect(armor.r_lightBar.center, rSize, armor.r_lightBar.angle);

		
		Point2f dstPoint[4];
		Point2f exLPoint[4], exRPoint[4];

		exLRect.points(exLPoint);
		exRRect.points(exRPoint);

		// 填充 armor 的装甲板的平面顶点
		armor.ArmorRoiPoint.push_back(exLPoint[0]);
		armor.ArmorRoiPoint.push_back(exLPoint[1]);
		armor.ArmorRoiPoint.push_back(exRPoint[2]);
		armor.ArmorRoiPoint.push_back(exRPoint[3]);

		dstPoint[0] = Point2f(0, 0);
		dstPoint[1] = Point2f(frame_size.width, 0);
		dstPoint[2] = Point2f(frame_size.width, frame_size.height);
		dstPoint[3] = Point2f(0, frame_size.height);

		RotatedRect exRect(Point2f(centerX, centerY), exSize, angle);

		drawRorect(smat["Armor"], exRect);
	}
	return Mat();
}

bool Armor_find::findLightBar_ver1()
{
	// proProcess
	proProcess();

	// 以源图像创建3个副本
	smat.cloneMat(smat(),
		{
			"result",
			"result2",
			"result_armor_light",
		});
	    
	vector<vector<Point>> contours;
	vector<RotatedRect> Rrect;

	// 寻找灯条轮廓
	findContours(smat["thresh"], contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (auto contour = contours.begin(); contour != contours.end(); )
	{
		if (contour->size() < 5)
		{
			contour = contours.erase(contour);
			continue;
		}
		if (contourArea(*contour) < v_contourMinArea)
		{
			contour = contours.erase(contour);
			continue;
		}
		RotatedRect rect = fitEllipse(*contour);
		if (rect.angle < 45)
			Rrect.push_back(rect);

		drawRorect(smat["result_armor_light"], rect);
		contour++;
	}
	drawContours(smat["result"], contours, -1, Scalar(0, 255, 0), 2);

	// 排序灯条
	sort(contours.begin(), contours.end(),
		[](const vector<Point>& contour1, const vector<Point>& contour2)
		{
			return contour1[0].x < contour2[0].x;
		});

	static int framePos = 1;
	smat.imshow("result_armor_light");

	// 找出包含装甲板的灯条
	armors.clear();
	IsFindArmor = false;
	for(int i = 0;i < Rrect.size(); i++)
		for (int j = i + 1; j < Rrect.size(); j++)
		{
			Armor armor(Rrect[i], Rrect[j]);
			if (armor.isSuitLightBar())
			{
				cout << "-----------Armor index  L: " << i << endl
					<< "-----------             R:" << j << endl;
				IsFindArmor = true;
				armors.push_back(armor);
			}
		}
	smat["Armor"];
	if (IsFindArmor)
	{
		getArmorImg();
		for (const auto& armor : armors)
			{
				angleSolver.SolvePnP(armor.ArmorRoiPoint);
				cout << "---------------------------------------------- POS_X: " << angleSolver.x_pos << endl;
				cout << "---------------------------------------------- POS_Y: " << angleSolver.y_pos << endl;
				cout << "---------------------------------------------- POS_Z: " << angleSolver.z_pos << endl;
				cout << "---------------------------------------------- DISTANCE: " << angleSolver.distance << endl;
				
			}
	}
	
	smat.setAllText(to_string(framePos));
	framePos++;


	smat.imshow("Armor");
	smat.imshow("result");
	if (!IsFindArmor)
		return false;
	else
		return true;

}


Armor_find::Armor_find(const cv::Mat& src)
	:frame_src(src)	,smat(src),frame_size(src.size())
{
	angleSolver.LoadCaramePar("../carame.xml");
	smat["thresh"];
	smat["gray"];
}

bool Armor_find::findLightBar(FINDBAR_VER ver /*= FINDBAR_VER::VER1*/)
{
	if (ver == FINDBAR_VER::VER1)
		return findLightBar_ver1();
}

LightBar::LightBar(cv::RotatedRect rect)
{
	lightRect = rect;
	size = rect.size;
	length = MAX(lightRect.size.width, lightRect.size.height);
	center = lightRect.center;

	if (lightRect.angle > 90)
		angle = lightRect.angle - 180;
	else
		angle = lightRect.angle;
}

void AngleSolver::LoadCaramePar(const std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		cout << "OPEN FILE FAILED:: " << path << endl;
		exit(0);
	}

	double fc1, fc2, cc1, cc2, k1, k2;
	fs["fc1"] >> fc1;
	fs["fc2"] >> fc2;
	fs["cc1"] >> cc1;
	fs["cc2"] >> cc2;
	fs["kc1"] >> k1;
	fs["kc2"] >> k2;
	
	fs.release();
	CameraMatrix = (Mat_<double>(3, 3) << fc1, 0, cc1, 0, fc2, cc2, 0, 0, 1);
	DistMatrix = (Mat_<double>(5, 1) << k1, k2, 0, 0, 0);
}

void AngleSolver::setPoint(std::vector<cv::Point2f> cameraPoint)
{
	CarmeraPoint = cameraPoint;
	int tar_width = 125;
	int tar_height = 55;

	float half_x = tar_width / 2.0;
	float half_y = tar_height / 2.0;

	WorldPoint.clear();
	WorldPoint.push_back(Point3f(-half_x, half_y, 0));
	WorldPoint.push_back(Point3f(half_x, half_y, 0));
	WorldPoint.push_back(Point3f(half_x, -half_y, 0));
	WorldPoint.push_back(Point3f(-half_x, -half_y, 0));
}

void AngleSolver::SolvePnP(std::vector<cv::Point2f> ArmorVertices)
{
	setPoint(ArmorVertices);
	solvePnP(WorldPoint, CarmeraPoint, CameraMatrix, DistMatrix, rVec, tVec);

	x_pos = tVec.at<double>(0, 0);
	y_pos = tVec.at<double>(1, 0);
	z_pos = tVec.at<double>(2, 0);

	distance = sqrt(x_pos * x_pos + y_pos * y_pos + z_pos * z_pos);
}

void AngleSolver::CaclCamera(std::vector<cv::Point2f> ArmorVertices)
{
	setPoint(ArmorVertices);
	calibrateCamera(WorldPoint, CarmeraPoint, Size(800, 450), CameraMatrix, DistMatrix, rVec, tVec);
	
	x_pos = tVec.at<double>(0, 0);
	y_pos = tVec.at<double>(1, 0);
	z_pos = tVec.at<double>(2, 0);

	distance = sqrt(x_pos * x_pos + y_pos * y_pos + z_pos * z_pos);
}

void AngleSolver::savaCarameData()
{
	FileStorage fs("carameData", FileStorage::WRITE);
	
	double fc1, fc2, cc1, cc2, k1, k2;
	fc1 = CameraMatrix.at<double>(0, 0);
	cc1 = CameraMatrix.at<double>(0, 2);
	fc2 = CameraMatrix.at<double>(1, 2);
	cc2 = CameraMatrix.at<double>(1, 3);
	
	k1 = DistMatrix.at<double>(0, 1);
	k2 = DistMatrix.at<double>(0, 2);

	fs << "fc1" << fc1;
	fs << "fc2" << fc2;
	fs << "cc1" << cc1;
	fs << "cc2" << cc2;
	fs << "k1" << k1;
	fs << "k2" << k2;

	fs.release();
}

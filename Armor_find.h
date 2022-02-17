#pragma once

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

#include "S_Mat.h"

#include <iostream>
#include <string>
#include <vector>

class Armor_find;
class AngleSolver;


enum class FINDBAR_VER : int
{
	VER1
};

struct LightBar
{
	cv::RotatedRect lightRect;
	cv::Point2f center;
	cv::Size size;
	float length;
	float angle;

	LightBar() = default;
	LightBar(cv::RotatedRect rect);
};

struct Armor {
	friend class Armor_find;
private:
	LightBar l_lightBar;
	LightBar r_lightBar;
public:
	std::vector<cv::Point2f> ArmorRoiPoint;

public:
	float getAngleDiff() const;

	// deviation angle : the horizon angle of the line of centers of lights ������λ�Ƚ�(����������������ˮƽ�߼н�) 
	float getDeviationAngle() const;

	// dislocation judge X: r-l light center distance ration on the X-axis ����λ�ò�� ����������x�������ֵ
	float getDislocationX() const;

	// dislocation judge Y:  r-l light center distance ration on the Y-axis ����λ�ò�� ����������Y�������ֵ
	float getDislocationY() const;

	// length difference ration: the length difference ration r-l lights ���ҵ������Ȳ��ֵ
	float getLengthRation() const;

	// �ж��Ƿ��Ǻ��ʵĵ���
	bool isSuitLightBar();

	Armor(LightBar l, LightBar r) : l_lightBar(l), r_lightBar(r) {}
	Armor() = default;
};

class AngleSolver
{
private:
	std::vector<cv::Point3f> WorldPoint;
	std::vector<cv::Point2f> CarmeraPoint;
public:
	void LoadCaramePar(const std::string& path);

	void setPoint(std::vector<cv::Point2f> cameraPoint);

	void SolvePnP(std::vector<cv::Point2f> ArmorVertices);

	void CaclCamera(std::vector<cv::Point2f> ArmorVertices);

	void savaCarameData();

	cv::Mat rVec;
	cv::Mat tVec;

	double x_pos;
	double y_pos;
	double z_pos;

	double distance;

	cv::Mat CameraMatrix;
	cv::Mat DistMatrix;
};

class Armor_find
{
private:
	bool TestMod = true;
private:
	cv::Mat frame_src;
	cv::Size frame_size;
	S_Mat smat;
	AngleSolver angleSolver;

	std::vector<Armor> armors;

	bool IsFindArmor = false;

	static int v_thresh;
	static int v_contourMinArea;
	static int v_higherDiff;

private:
	bool findLightBar_ver1();
	// Ԥ����
	void proProcess();
	
	// ת��Դͼ��
	cv::Mat transArmorImg(cv::Point2f src[],cv::Point2f dst[]);

	// ������������ת����λ�û�ȡװ�װ� roi
	cv::Mat getArmorImg();

public:
	Armor_find(const cv::Mat&);

	bool findLightBar(FINDBAR_VER ver = FINDBAR_VER::VER1);

};


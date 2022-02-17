#pragma once
#include <string>

struct ImgInfo
{
	std::string name;
	// ����޸ĵĽڵ�
	int LastNode;
	// ����ʹ�õĽڵ�
	std::string RecomNode;
	// ��ǰ�ļ�ʱ��
	std::string Date;
	// Ԥ������ֵ
	int ProThresh;
	// ����
	std::string Response = "û�����⣡";

	ImgInfo() = default;
};

const int color_threshold = 100 - 20;   //color threshold for colorImg from substract channels ͨ�������colorImgʹ�õĶ�ֵ����ֵ
const int bright_threshold = 60;  //color threshold for brightImg ����ͼ��ֵ����ֵ
 
const float min_area = 50;		// min area of light bar �����������С���
const float max_angle = 45;	//max angle of light bar ������������ƫ��

const float max_angle_diff = 6; //max angle difference between two light bars ��������֮����������ǶȲ�
const float max_lengthDiff_ratio = 0.5; //max length ratio difference between two light bars ��������֮���������󳤶Ȳ��ֵ
const float max_deviation_angle = 50; //max deviation angle ����������λ��
 
const float max_y_diff_ratio = 0.5;  //max y 
const float max_x_diff_ratio = 4.5;  //max x

#define CARAMEPAR "D:/VsProjects/ArmorRecognition/carame.xml"
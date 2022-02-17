#pragma once
#include <string>

struct ImgInfo
{
	std::string name;
	// 最后修改的节点
	int LastNode;
	// 建议使用的节点
	std::string RecomNode;
	// 当前文件时间
	std::string Date;
	// 预处理阈值
	int ProThresh;
	// 感想
	std::string Response = "没有问题！";

	ImgInfo() = default;
};

const int color_threshold = 100 - 20;   //color threshold for colorImg from substract channels 通道相减的colorImg使用的二值化阈值
const int bright_threshold = 60;  //color threshold for brightImg 亮度图二值化阈值
 
const float min_area = 50;		// min area of light bar 灯条允许的最小面积
const float max_angle = 45;	//max angle of light bar 灯条允许的最大偏角

const float max_angle_diff = 6; //max angle difference between two light bars 两个灯条之间允许的最大角度差
const float max_lengthDiff_ratio = 0.5; //max length ratio difference between two light bars 两个灯条之间允许的最大长度差比值
const float max_deviation_angle = 50; //max deviation angle 两灯条最大错位角
 
const float max_y_diff_ratio = 0.5;  //max y 
const float max_x_diff_ratio = 4.5;  //max x

#define CARAMEPAR "D:/VsProjects/ArmorRecognition/carame.xml"
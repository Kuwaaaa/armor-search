#ifndef SMAT
#define SMAT

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Windows.h>


// 创建窗口用的参数集合
// 目前会在 imshow（）时抛出异常，猜测是目标 mat 没有创建
// 2020/8/06 / 14:10
// 新增了自动排版
// 2020/8/06 / 14:15
// 莫名奇妙的又行了？？？ 好像没改什么的说   
// 2020/8/10 / 11:06
// 草  不知道为什么这个文件会导致编译时模板错误   看都看不懂QAQ
// 2020/9/5  / 18:42
// 猜测是头文件包含冲突，发现是源文件中 <opencv2/opencv.cpp> 冲突乐    删了就好乐
// 2020/11/7 / 10:??
// 新增了以名字快速创建副本功能 → cloneMat(Mat,{string...})
// 2020/11/8 / 1:59
// 糟了 让我给整明白了
// 冲突的原因是 opencv 中头文件是以 .hpp（头文件 .h 和 cpp 结合到一起的形式) 然后这个 S_Mat.h 也在把声明和定义放在一起
// 与其他定义文件中使用 opencv 的 .hpp 重复定义，就冲突乐。
// 解决办法就是将这个 S_Mat.h 的声明和定义分开，这样就好乐

// 小函数
// --------------------------------

// 设置文本
void setText(cv::Mat mat, const std::string& text, cv::Scalar color = (0,255,0), cv::Point position = cv::Point(10,30));
// 画旋转矩形
void drawRorect(cv::Mat& src, const cv::RotatedRect& rect, cv::Scalar color = {0,255,0});

// --------------------------------

// 为了创建回调函数所用的 struct
struct Mat_data
{
	typedef void(*CallBack) (int, void*);

	const std::string valueName;
	int* Value;
	const int maxValue;
	// backCalledFun
	CallBack Pf;
	// Last parameter of createWindow
	void* Param;

	Mat_data() = default;
	Mat_data(const std::string& name, int* value, const int mv, CallBack pf, void* p)
		: valueName(name), Value(value), maxValue(mv), Pf(pf), Param(p) { }
};

class S_Mat {
	using S_data = std::pair<cv::Mat, std::vector<Mat_data>>;	// 好复杂啊 李在赣神魔
private:
	bool isAutoWidth;											// 废弃的一个功能    好像是自动调整宽度
	int AutoWidth = 1000;										// 同上
	int index_x = 0, index_y = 0;								// x、y 轴上整齐排列渲染的图像的下标
	int window_offset_x = 0, window_offset_y = 0;				// 换行、列后增加的偏移值
	int window_x, window_y;										// 当前机器的屏幕大小
	int windowsMod = 1;											// 废弃的不知道什么东西
	const cv::Mat _src;											// 源图像
	int setTextIndex = 0;
	std::map < std::string, S_data> m_Mat;						// 存储 Mat 的 map

	// 快速创建窗口使用的工具函数
	void createTra(const Mat_data&, const std::string&);

	// 快速 clone 使用的工具函数
	void clone(const cv::Mat&, const std::string&);
public:
	// 构造函数
	S_Mat(cv::Mat src, int winMod = 1, bool isAuto = false);

	// 输出 smat 内所有 key
	std::vector<std::string> showAllname();

	// 快速 imshow 
	void imshow(const std::string& name);

	// imshow 所有 Mat
	void imshowAll();

	// 对所有 Mat 设置文字	(这些 Mat 的 Size 不应该过小)
	void setAllText(const std::string& text, cv::Scalar color = cv::Scalar(0,255,0), cv::Point position = cv::Point(10,30)) const;

	// 创建调参窗口
	void createTrackbar(const std::string&, const std::string&, int*, const int&, void(*)(int, void*) = nullptr, void* = nullptr);

	// 快速创建副本
	void cloneMat(const cv::Mat&, const std::string&, int num = 1);

	// 快速创建副本 (自定义命名重载版本)
	void cloneMat(const cv::Mat&, const std::initializer_list<std::string> matNames);

	// 使用 map[] 创建或寻找元素
	// 这玩意儿是有点危险的，如果用户使用一个未定义的名字来获取图像(例如 Mat 名字打错了)，该函数还是会正常返回一个 Mat，这将有可能导致错误(未能获取本想获得的 Mat），并且难以查找。
	cv::Mat& operator[] (const std::string& name)
	{
		// 在 m_Mat 中寻找名字，如果没有这个 key , 则将该 key 对应的图像（还未创建的）copy 为src
		// 这可能在一定程度上能解决上述问题
		if (m_Mat.find(name) == m_Mat.end())
			_src.copyTo(m_Mat[name].first);
		return m_Mat[name].first;
	}
	// 返回 src
	const cv::Mat& operator() () const	{
		return src();
	}
	// return src
	const cv::Mat& src() const { return _src; }
	~S_Mat() { setTextIndex = 0; }
};

#endif // !SMAT

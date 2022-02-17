#ifndef SMAT
#define SMAT

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Windows.h>


// ���������õĲ�������
// Ŀǰ���� imshow����ʱ�׳��쳣���²���Ŀ�� mat û�д���
// 2020/8/06 / 14:10
// �������Զ��Ű�
// 2020/8/06 / 14:15
// Ī������������ˣ����� ����û��ʲô��˵   
// 2020/8/10 / 11:06
// ��  ��֪��Ϊʲô����ļ��ᵼ�±���ʱģ�����   ����������QAQ
// 2020/9/5  / 18:42
// �²���ͷ�ļ�������ͻ��������Դ�ļ��� <opencv2/opencv.cpp> ��ͻ��    ɾ�˾ͺ���
// 2020/11/7 / 10:??
// �����������ֿ��ٴ����������� �� cloneMat(Mat,{string...})
// 2020/11/8 / 1:59
// ���� ���Ҹ���������
// ��ͻ��ԭ���� opencv ��ͷ�ļ����� .hpp��ͷ�ļ� .h �� cpp ��ϵ�һ�����ʽ) Ȼ����� S_Mat.h Ҳ�ڰ������Ͷ������һ��
// �����������ļ���ʹ�� opencv �� .hpp �ظ����壬�ͳ�ͻ�֡�
// ����취���ǽ���� S_Mat.h �������Ͷ���ֿ��������ͺ���

// С����
// --------------------------------

// �����ı�
void setText(cv::Mat mat, const std::string& text, cv::Scalar color = (0,255,0), cv::Point position = cv::Point(10,30));
// ����ת����
void drawRorect(cv::Mat& src, const cv::RotatedRect& rect, cv::Scalar color = {0,255,0});

// --------------------------------

// Ϊ�˴����ص��������õ� struct
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
	using S_data = std::pair<cv::Mat, std::vector<Mat_data>>;	// �ø��Ӱ� ���ڸ���ħ
private:
	bool isAutoWidth;											// ������һ������    �������Զ��������
	int AutoWidth = 1000;										// ͬ��
	int index_x = 0, index_y = 0;								// x��y ��������������Ⱦ��ͼ����±�
	int window_offset_x = 0, window_offset_y = 0;				// ���С��к����ӵ�ƫ��ֵ
	int window_x, window_y;										// ��ǰ��������Ļ��С
	int windowsMod = 1;											// �����Ĳ�֪��ʲô����
	const cv::Mat _src;											// Դͼ��
	int setTextIndex = 0;
	std::map < std::string, S_data> m_Mat;						// �洢 Mat �� map

	// ���ٴ�������ʹ�õĹ��ߺ���
	void createTra(const Mat_data&, const std::string&);

	// ���� clone ʹ�õĹ��ߺ���
	void clone(const cv::Mat&, const std::string&);
public:
	// ���캯��
	S_Mat(cv::Mat src, int winMod = 1, bool isAuto = false);

	// ��� smat ������ key
	std::vector<std::string> showAllname();

	// ���� imshow 
	void imshow(const std::string& name);

	// imshow ���� Mat
	void imshowAll();

	// ������ Mat ��������	(��Щ Mat �� Size ��Ӧ�ù�С)
	void setAllText(const std::string& text, cv::Scalar color = cv::Scalar(0,255,0), cv::Point position = cv::Point(10,30)) const;

	// �������δ���
	void createTrackbar(const std::string&, const std::string&, int*, const int&, void(*)(int, void*) = nullptr, void* = nullptr);

	// ���ٴ�������
	void cloneMat(const cv::Mat&, const std::string&, int num = 1);

	// ���ٴ������� (�Զ����������ذ汾)
	void cloneMat(const cv::Mat&, const std::initializer_list<std::string> matNames);

	// ʹ�� map[] ������Ѱ��Ԫ��
	// ����������е�Σ�յģ�����û�ʹ��һ��δ�������������ȡͼ��(���� Mat ���ִ����)���ú������ǻ���������һ�� Mat���⽫�п��ܵ��´���(δ�ܻ�ȡ�����õ� Mat�����������Բ��ҡ�
	cv::Mat& operator[] (const std::string& name)
	{
		// �� m_Mat ��Ѱ�����֣����û����� key , �򽫸� key ��Ӧ��ͼ�񣨻�δ�����ģ�copy Ϊsrc
		// �������һ���̶����ܽ����������
		if (m_Mat.find(name) == m_Mat.end())
			_src.copyTo(m_Mat[name].first);
		return m_Mat[name].first;
	}
	// ���� src
	const cv::Mat& operator() () const	{
		return src();
	}
	// return src
	const cv::Mat& src() const { return _src; }
	~S_Mat() { setTextIndex = 0; }
};

#endif // !SMAT

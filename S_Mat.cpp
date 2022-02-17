#include "S_Mat.h"


void S_Mat::createTra(const Mat_data& data, const std::string& matName)
{
	cv::createTrackbar(data.valueName, matName, data.Value, data.maxValue, data.Pf, data.Param);
}

S_Mat::S_Mat(cv::Mat src, int winMod /* = 1*/, bool isAuto /* = false*/)
	: _src(src), windowsMod(winMod)
{
	isAutoWidth = isAuto;

	// ��õ�ǰ��������Ļ��С
	window_x = GetSystemMetrics(SM_CXSCREEN);
	window_y = GetSystemMetrics(SM_CYSCREEN);
	m_Mat["src"].first = src;
}

std::vector<std::string> S_Mat::showAllname()
{
	std::vector<std::string> names;
	for (const auto& mat : m_Mat)
		names.push_back(mat.first);
	return names;
}

void S_Mat::imshow(const std::string& name) 
{
	int src_width = _src.size().width;
	int src_height = _src.size().height;
	static bool IsFirst = true;
	int offset_x = window_x / 10;
	int offset_y = window_y / 10;

	/*if (isAutoWidth)
	{
		while (m_Mat[name].first.size().width < AutoWidth)
		{
			pyrUp(m_Mat[name].first.clone(), m_Mat[name].first,cv::Size(m_Mat[name].first.size()) * 2);
		}
	}*/
	// ���㵱ǰ���ڵ� x �����Ƿ񳬹���Ļ�Ŀ�
	// ��ֹ���ڵ��ұ߽���ʾ����Ļ�⣬�� index + 1
	if ((index_x + 1) * src_width >= window_x + offset_x)
	{
		index_y++;
		index_x = 0;
	}
	// ��������Ѿ���������Ļ�������´����Ͻǿ�ʼ����,Ϊ�˷�����ק���ڣ�����ƫ��ֵС�����ƶ�����
	if ((index_y + 1) * src_height >= window_y + offset_y)
	{
		index_y = 0;
		window_offset_x += 50;
		window_offset_y += 20;
	}
	// ���㵱ǰ���ڵ���Ļ����
	int win_x = index_x * src_width + window_offset_x;
	int win_y = index_y * src_height + window_offset_y;

	cv::namedWindow(name, windowsMod);
	cv::moveWindow(name, win_x, win_y);
	// �Բ���(0,0)�������д��ڵĻص������������Ͳ���������������ûص�������
	for (const auto& it : m_Mat[name].second)
		// ���û�д���ص��������Ͳ���������-0-
		// ��Щ�ص�����ֻ�����ֶ�����һ��
		if (it.Pf != nullptr)
			if (IsFirst)
			{
				it.Pf(0, 0);
				IsFirst = false;
			}
	cv::imshow(name, m_Mat[name].first);

	index_x++;
}

void S_Mat::imshowAll()
{
	for (const auto& it : m_Mat)
		imshow(it.first);
}

void S_Mat::setAllText(const std::string& text, cv::Scalar color /*= { 0,255,0 }*/, cv::Point position /*= {10,30}*/) const
{
	static int textIndex = 0;
	cv::Point pos = position;
	pos.y += textIndex * 20;							// �Զ�����		Ŀǰû����
	for (auto& it : m_Mat)
		setText(it.second.first, text, color, pos);

}

void S_Mat::createTrackbar(const std::string& matName, const std::string& valueName, int* value, const int& max, void(*pf)(int, void*), void* param)
{
	cv::namedWindow(matName);
	Mat_data data(valueName, value, max, pf, param);
	m_Mat[matName].second.push_back(data);
	createTra(data, matName);
}

void S_Mat::clone(const cv::Mat& src, const std::string& copyName)
{
	m_Mat[copyName].first = src.clone();
}

// ���� dst dst2 dst3 ������������ʽ��������
void S_Mat::cloneMat(const cv::Mat& src,const std::string& name, int num /*= 1*/)
{
	std::string t_name;
	for (int i = 1; i < num; i++)
	{
		t_name = name + std::to_string(i);
		clone(src, t_name);
	}
}
// initializer_list   ʮ�ֿ���
void S_Mat::cloneMat(const cv::Mat& src, const std::initializer_list<std::string> matNames)
{
	for (const auto& matName : matNames)
		clone(src, matName);
}

void setText(cv::Mat mat, const std::string& text, cv::Scalar color /*= cv::Scalar(0,255,0)*/, cv::Point position /*= cv::Point(10,30)*/)
{
	putText(mat, text, position, 0, 1, color, 4);
}

void drawRorect(cv::Mat& src, const cv::RotatedRect& rect, cv::Scalar color /*= {0,255,0}*/)
{
	cv::Point2f p[4];
	rect.points(p);
	for (int i = 0; i < 4; i++)
	{
		line(src, p[i], p[(i + 1) % 4], color, 3);
	}
}

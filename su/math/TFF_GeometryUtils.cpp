////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 ������������Ϣ�Ƽ����޹�˾(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_GeometryUtils.cpp
//	@brief		TFF_GeometryUtils source file
//	@author		gaoyang
//	@date		2017-02-14
//  @version 
//    - v1.0	2017-02-14	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////



#include <common/math/TFF_GeometryUtils.h>
#include <common/math/TFF_MathUtils.h>
#include <cassert>
#include "marco.h"
#include "TFF_Float2.h"
//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
/*
bool GeometryUtils::IsPointOnPoint(const unsigned char x, const unsigned char y, const unsigned char x1, const unsigned char y1, const int radius)
{
	if (x >= x1 - radius && y >= y1 - radius && x <= x1 + radius && y <= y1 + radius)
	{
		return true;
	}

	return false;
}
*/

/*
bool GeometryUtils::IsPointOnLine(const int x, const int y, const unsigned char* pointList, const int pointNum, const int lineWidth)
{
	float tmp;
	if (lineWidth < 1.0)
	{
		tmp = 1.0;
	}
	else 
	{
		tmp = (float)lineWidth;
	}

	for (int i = 0, j = 1; j < pointNum; i++, j++)
	{
		if ((PointLineLength((float)x, (float)y, pointList[i * 2], pointList[i * 2 + 1], pointList[j * 2], pointList[j * 2 + 1])) <= (tmp))
		{
			return true;
		}
		else 
		{
			continue;
		}
	}

	return false;
}
*/

//bool GeometryUtils::IsPointOnPoly(const unsigned char x, const unsigned char y, const unsigned char* pointList, const size_t  pointNum)
//{
//	size_t i, j;
//	i = j = 0;
//	int c = 0;
//	for (i = 0, j = pointNum - 1; i < pointNum; j = i++) 
//	{
//		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
//		{
//			continue;
//		}
//		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
//			((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
//			(x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
//		{
//			c = !c;
//		}
//	}
//
//	if (c != 0)
//	{
//		return true;
//	}
//
//	return false;
//}

//bool GeometryUtils::IsPointOnPoly(const float x, const float y, const float* pointList, const size_t  pointNum)
//{
//	size_t i, j;
//	i = j = 0;
//	int c = 0;
//	for (i = 0, j = pointNum - 1; i < pointNum; j = i++)
//	{
//		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
//		{
//			continue;
//		}
//
//		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
//			((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
//			(x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
//		{
//			c = !c;
//		}
//	}
//
//	if (c != 0)
//	{
//		return true;
//	}
//
//	return false;
//}

//bool GeometryUtils::IsPointOnPoly(const double x, const double y, const double* pointList, const size_t  pointNum)
//{
//	size_t i, j;
//	i = j = 0;
//	int c = 0;
//	for (i = 0, j = pointNum - 1; i < pointNum; j = i++) 
//	{
//		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
//		{
//			continue;
//		}
//		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
//			((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
//			(x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
//		{
//			c = !c;
//		}
//	}
//
//	if (c != 0)
//	{
//		return true;
//	}
//
//	return false;
//}

//bool GeometryUtils::IsPointOnPoly(const float x, const float y, const vector<float>& pointList)
//{
//	size_t i, j;
//	i = j = 0;
//	int c = 0;
//	size_t  pointNum = pointList.size() / 2;
//	for (i = 0, j = pointNum - 1; i < pointNum; j = i++)
//	{
//		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
//		{
//			continue;
//		}
//
//		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
//			((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
//			(x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
//		{
//			c = !c;
//		}
//	}
//
//	if (c != 0)
//	{
//		return true;
//	}
//
//	return false;
//}

//bool GeometryUtils::IsPointOnPoly(const double x, const double y, const vector<double>& pointList)
//{
//	size_t i, j;
//	i = j = 0;
//	int c = 0;
//	size_t  pointNum = pointList.size() / 2;
//	for (i = 0, j = pointNum - 1; i < pointNum; j = i++)
//	{
//		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
//		{
//			continue;
//		}
//
//		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
//			((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
//			(x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
//		{
//			c = !c;
//		}
//	}
//
//	if (c != 0)
//	{
//		return true;
//	}
//
//	return false;
//}

float GeometryUtils::CalcPolygonArea(const float* x, const float* y, const size_t  num)
{
	assert(nullptr != x && nullptr != y);
	float area = 0.0;
	int j = 0;

	for (size_t i = 0; i < num; ++i)
	{
		j = (i + 1) % num;
		area += x[i] * y[j];
		area -= y[i] * x[j];
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

double GeometryUtils::CalcPolygonArea(const double* x, const double* y, const size_t  num)
{
	assert(nullptr != x && nullptr != y);
	double area = 0.0;
	int j = 0;

	// x1, x2, x3
	// y1, y2, y3
	for (size_t i = 0; i < num; ++i)
	{
		j = (i + 1) % num;
		area += x[i] * y[j];
		area -= y[i] * x[j];

		/*
		x1, y2
		y1, x2

		x2, y3,
		y2, x3, 

		x3, y1,
		y3, x1
		*/
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

float GeometryUtils::CalcPolygonArea(const float* pointList, const size_t  num)
{
	assert(nullptr != pointList);
	float area = 0.0;
	int j = 0;

	// x1, y1, x2, y2, x3, y3 ...
	size_t index = 0;
	for (size_t i = 0; i < num; i+=2)
	{
		j = (i + 1) % num;
		index = j + 2;
		if (index >= num)
		{
			index = 1;
		}
		area += pointList[i] * pointList[index];

		index = i + 2;
		if (index >= num)
		{
			index = 0;
		}
		area -= pointList[i + 1] * pointList[index];
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

double GeometryUtils::CalcPolygonArea(const double* pointList, const size_t  num)
{
	assert(nullptr != pointList);
	double area = 0.0;
	int j = 0;

	// x1, y1, x2, y2, x3, y3 ...
	size_t index = 0;
	for (size_t i = 0; i < num; i+=2)
	{
		j = (i + 1) % num;
		index = j + 2;
		if (index >= num)
		{
			index = 1;
		}
		area += pointList[i] * pointList[index];

		index = i + 2;
		if (index >= num)
		{
			index = 0;
		}
		area -= pointList[i + 1] * pointList[index];
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

float GeometryUtils::CalcPolygonArea(const std::vector<float>& pointList)
{
	float area = 0.0;
	int j = 0;

	// x1, y1, x2, y2, x3, y3 ...
	size_t  num = pointList.size();
	size_t index = 0;
	for (size_t i = 0; i < num; i+=2)
	{
		j = (i + 1) % num;
		index = j + 2;
		if (index >= num)
		{
			index = 1;
		}
		area += pointList[i] * pointList[index];

		index = i + 2;
		if (index >= num)
		{
			index = 0;
		}
		area -= pointList[i + 1] * pointList[index];
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

double GeometryUtils::CalcPolygonArea(const std::vector<double>& pointList)
{
	double area = 0.0;
	int j = 0;

	// x1, y1, x2, y2, x3, y3 ...
	size_t  num = pointList.size();
	size_t index = 0;
	for (size_t i = 0; i < num; i+=2)
	{
		j = (i + 1) % num;
		index = j + 2;
		if (index >= num)
		{
			index = 1;
		}
		area += pointList[i] * pointList[index];

		index = i + 2;
		if (index >= num)
		{
			index = 0;
		}
		area -= pointList[i + 1] * pointList[index];
	}

	area /= 2;
	return (area < 0 ? -area : area);
}

void GeometryUtils::GetCircleRect(const common::Double2& center, const double radius, common::Rectangle<double>& rect)
{
	double latitude = center.Y();
	double longitude = center.X();

	double degree = (24901 * 1609) / 360.0;
	double raidusMile = radius;

	double dpmLat = 1 / degree;
	double radiusLat = dpmLat*raidusMile;
	double minLat = latitude - radiusLat;
	double maxLat = latitude + radiusLat;

	double mpdLng = degree * std::cos(latitude * (TFF_PI / 180));
	double dpmLng = 1 / mpdLng;
	double radiusLng = dpmLng * raidusMile;
	double minLng = longitude - radiusLng;
	double maxLng = longitude + radiusLng;

	rect.g_left = minLng;
	rect.g_top = minLat;
	rect.g_right = maxLng;
	rect.g_bottom = maxLat;
}  

double GeometryUtils::GetDistance(const common::Double2& point1, const common::Double2& point2)
{
	/*//double radLat1 = t_deg2rad(point1.Y());
	//double radLat2 = t_deg2rad(point2.Y());  
	//double a = radLat1 - radLat2;  
	//double b = t_deg2rad(point1.X()) - t_deg2rad(point2.X());  

	//double s = 2 * t_asin(std::sqrt(t_pow(t_sin(a / 2), 2) +
	//	t_cos(radLat1) * t_cos(radLat2) * t_pow(t_sin(b / 2), 2)));  
	//s = s * TFF_EARTH_RADIUS; 
	//s = t_round(s * 10000, 6) / 10000;
	////s = Math.Round(s * 10000) / 10000;  
	//return s;

	double radLat1 = t_deg2rad(point1.Y());
	double radLng1 = t_deg2rad(point1.X());
	double radLat2 = t_deg2rad(point2.Y());
	double radLng2 = t_deg2rad(point2.X());

	//double s = t_acos(t_cos(radLat1) * t_cos(radLat2) * t_cos(radLng1 - radLng2) + t_sin(radLat1) * t_sin(radLat2));
	//s = s * TFF_EARTH_RADIUS;  
	//s = t_round(s * 10000) / 10000;  
	//return s;

	double a = radLat1 - radLat2;
	double b = radLng1 - radLng2;
	double result = 2 * t_asin(std::sqrt(t_pow(t_sin(a / 2), 2) + t_cos(radLat1) * t_cos(radLat2) * t_pow(t_sin(b / 2), 2))) * TFF_EARTH_RADIUS;
	result *= 1000;
	return result;*/
    return 0;
}

float area(const common::Float2& a, const common::Float2& b, const common::Float2& c) //�����������ʽ1/2 * cross(Point A, Point B)
{
	/*common::Float2 A, B;
	A = b - a;
	B = c - a;
	return t_cross_product(A.X(), A.Y(), B.X(), B.Y());//����Ҫ�ٳ���2����Ϊ�����Ľ���е����ˣ�*/

    return 0;
}

double Area(const common::Float2& p1, const common::Float2& p2, const common::Float2& p3)
{
	return ((p2.X() - p1.X()) * (p3.Y() - p1.Y()) - (p3.X() - p1.X()) * (p2.Y() - p1.Y())) * 0.5;
}

//common::Float2 GeometryUtils::GetPolygonCenterPoint(const std::vector<common::Float2>& vertices)
//{
//	common::Float2 centerPoint;
//
//
//	double area = 0.0;//��������
//	double Gx = 0.0, Gy = 0.0;// ���ĵ�x��y
//	for (int i = 1; i <= vertices.size(); i++)
//	{
//		double iLat = vertices[(i % vertices.size())].Y();
//		double iLng = vertices[(i % vertices.size())].X();
//		double nextLat = vertices[(i - 1)].Y();
//		double nextLng = vertices[(i - 1)].X();
//		double temp = (iLat * nextLng - iLng * nextLat) / 2.0;
//		area += temp;
//		Gx += temp * (iLat + nextLat) / 3.0;
//		Gy += temp * (iLng + nextLng) / 3.0;
//	}
//	if (!MathUtils::IsZero(area))
//	{
//		Gx = Gx / area;
//		Gy = Gy / area;
//	}
//	centerPoint.Set(Gy, Gx);
//
//
//	return centerPoint;
//}

int GeometryUtils::GetTextPositionOfBrokenLine(const char* pCaption, const int PointNum, float* pPos, const float percentage, const int FontWidth, const int FontDistance, int* Result)
{
	int CharNum = 0;//�ַ�����
	float TotalLenth = 0.0f;//�߶��ܳ���
	float *eachlenth = nullptr;//ÿ�γ��ȵ�����
	float charpos[200];//�ַ����߶����ľ�������
	memset(charpos, 0.0f, sizeof(charpos));
	float CaptionWidth = 0.0f;//���ֵ��ܿ��
	float CaptionStart = 0.0f;//���ֵ���ʼ�����
	float CaptionEnd = 0.0f;//���ֵĽ��������
	float tmpResult[400];//�洢�м���
	memset(tmpResult, 0.0f, sizeof(tmpResult));

	int i = 0;
	float dtemp = 0.0f;


	CharNum = 0;
	CaptionWidth = 0;
	i=0;
	while (pCaption[i]!=0) {
		i++;
		CharNum++;
		if ((pCaption[i] & 0x80) && pCaption[i]!=0x80) {
			CaptionWidth = CaptionWidth + FontWidth * 2;
			i++;
		}
		else{
			CaptionWidth = CaptionWidth + FontWidth;
		}
	}

	//�����ܳ���

	TotalLenth = 0;
	//int lenx = 0;
	//int leny = 0;
	float lenx = 0.0f;
	float leny = 0.0f;

	eachlenth=(float*)malloc((PointNum)*sizeof(float));

	for(i=0;i<PointNum-1;i++){
		lenx = std::abs(pPos[i*2]-pPos[i*2+2]);
		leny = std::abs(pPos[i*2+1]-pPos[i*2+3]);
		eachlenth[i] = std::sqrt(double(lenx*lenx + leny*leny));
		TotalLenth = TotalLenth + eachlenth[i];
	}

	//����ÿ���ַ����߶γ����ϵ�λ�� 
	if (CaptionWidth > TotalLenth) {
		free(eachlenth);
		return 0;
	}
	dtemp = TotalLenth*percentage;
	dtemp=CaptionWidth;
	if (dtemp>CaptionWidth) {
		CaptionWidth = dtemp;
	}
	CaptionStart = (TotalLenth - CaptionWidth) / 2;
	CaptionEnd = CaptionStart + CaptionWidth;
	dtemp = CaptionWidth / (CharNum - 1);

	//	charpos = (float *)malloc(CharNum * sizeof(float));
	charpos[0] = CaptionStart;
	for(i=0;i<CharNum-1;i++){
		charpos[i+1] = charpos[i] + dtemp;
	}

	int CurrentChar;//��ǰҪѰ��λ�õ��ַ���index
	float CurrentLenth;
	float PrevLenth;//��ǰ���ڴ�����߶ε����ľ���
	float l;
	float K;//�߶ε�б��
	float k;//���ߵ�б��
	float k1;
	float k2;

	CurrentChar = 0;
	CurrentLenth = 0;

	//ת��Ϊ������λ��
	//	tmpResult=(float*)malloc(CharNum * sizeof(float) * 2);

	for(i=0;i<PointNum-1;i++){
		PrevLenth=CurrentLenth;
		CurrentLenth = CurrentLenth + eachlenth[i];
		while ((CurrentLenth >= charpos[CurrentChar]) && (CurrentChar<CharNum)) {
			//��ǰ���ڱ�����

			l = charpos[CurrentChar] - PrevLenth;

			tmpResult[2*CurrentChar] = pPos[i*2] - (pPos[i*2] - pPos[i*2+2]) * l / eachlenth[i];
			tmpResult[2*CurrentChar+1] = pPos[i*2+1] - (pPos[i*2+1] - pPos[i*2+3]) * l /eachlenth[i];

			if (pPos[i*2+2]==pPos[i*2]) {
				//y���ƽ����
				if (pPos[i*2+3]<pPos[i*2+1]) {
					//��������
					Result[2*CurrentChar] = tmpResult[2*CurrentChar] - FontDistance;
				}
				else{
					//��������
					Result[2*CurrentChar] = tmpResult[2*CurrentChar] + FontDistance;
				}

				Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1];
			}
			else{
				if (pPos[i*2+1]==pPos[i*2+3]) {
					//x���ƽ����
					if (pPos[i*2+2]>pPos[i*2]) {
						//��������
						Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - FontDistance;					
					}
					else{
						//��������
						Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + FontDistance;
					}
					Result[2*CurrentChar] = tmpResult[2*CurrentChar];
				}
				else{

					k1=pPos[i*2+3]-pPos[i*2+1];
					k2=pPos[i*2+2]-pPos[i*2];
					K=k1/k2;
					k = -1/K;
					if (k>0) {
						//��x������
						if (k1<0) {
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] - (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - (k/sqrt(1+k*k)) * FontDistance; 
						}

						else{
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] + (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + (k/sqrt(1+k*k)) * FontDistance; 
						}
					}
					else{
						//��x��ɶ۽�
						if (k1<0) {
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] - (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - (k/sqrt(1+k*k)) * FontDistance; 
						}
						else{
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] + (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + (k/sqrt(1+k*k)) * FontDistance; 
						}
					}
				}
			}

			CurrentChar++;
		}
	}


	free(eachlenth);
	//	free(tmpResult);
	//	free(charpos);



	return CurrentChar;
}

int GeometryUtils::GetTextPositionOfBrokenLine(const char* pCaption, int PointNum, float* pPos1, const float percentage, const float FontWidth, const float FontDistance, float* Result)
{
	/*if (0 == PointNum || nullptr == pPos1)
	{
		return 0;
	}

	int CharNum = 0;//�ַ�����
	float TotalLenth = 0.0f;//�߶��ܳ���
	float *eachlenth = nullptr;//ÿ�γ��ȵ�����
	float charpos[200];//�ַ����߶����ľ�������
	memset(charpos, 0.0f, sizeof(charpos));
	float CaptionWidth = 0.0f;//���ֵ��ܿ��
	float CaptionStart = 0.0f;//���ֵ���ʼ�����
	float CaptionEnd = 0.0f;//���ֵĽ��������
	float tmpResult[400];//�洢�м���
	memset(tmpResult, 0.0f, sizeof(tmpResult));

	int i = 0;
	float dtemp = 0.0f;


	CharNum = 0;
	CaptionWidth = 0;
	i=0;
	while (pCaption[i]!=nullptr) {
		i++;
		CharNum++;
		if ((pCaption[i] & 0x80) && pCaption[i]!=0x80) {
			CaptionWidth = CaptionWidth + FontWidth * 2;
			i++;
		}
		else{
			CaptionWidth = CaptionWidth + FontWidth;
		}
	}


	// ��ȡ�����еİ�Χ��.
	common::Rectangle<float> pointRect;
	pointRect.g_left = 9999.0f;
	pointRect.g_top = 9999.0f;
	pointRect.g_right = -9999.0f;
	pointRect.g_bottom = -9999.0f;
	for (int i = 0; i < PointNum; ++i)
	{
		pointRect.g_left = std::min(pointRect.g_left, pPos1[i * 2]);
		pointRect.g_top = std::min(pointRect.g_top, pPos1[i * 2 + 1]);
		pointRect.g_right = std::max(pointRect.g_right, pPos1[i * 2]);
		pointRect.g_bottom = std::max(pointRect.g_bottom, pPos1[i * 2 + 1]);
	}

	// ��������.
	common::Float2 beginPoint(pPos1[0], pPos1[1]);
	common::Float2 endPoint(pPos1[PointNum * 2 - 2], pPos1[PointNum * 2 - 1]);
	float gridWidth = 0.005f;
	float gridHeight = gridWidth;
	int gridXCount = (int)std::ceilf(pointRect.Width() / gridWidth);
	int gridYCount = (int)std::ceilf(pointRect.Height() / gridHeight);
	std::vector<float> dstPointArr;
	// ������ʼ��ͽ�����.
	dstPointArr.push_back(beginPoint.X());
	dstPointArr.push_back(beginPoint.Y());
	int gridXNum = -1;
	int gridYNum = -1;
	int oldGridXNum = -1;
	int oldGridYNum = -1;
	for (int i = 1; i < PointNum - 1; ++i)
	{
		gridXNum = (int)std::ceilf((pPos1[i * 2] - beginPoint.X()) / gridWidth);
		gridYNum = (int)std::ceilf((pPos1[i * 2 + 1] - beginPoint.Y()) / gridHeight);
		if (oldGridXNum != gridXNum && oldGridYNum != gridYNum)
		{
			dstPointArr.push_back(pPos1[i * 2]);
			dstPointArr.push_back(pPos1[i * 2 + 1]);

			oldGridXNum = gridXNum;
			oldGridYNum = gridYNum;
		}
	}
	dstPointArr.push_back(endPoint.X());
	dstPointArr.push_back(endPoint.Y());

	PointNum = int(dstPointArr.size() / 2);
	if (PointNum == 0)
	{
		return 0;
	}
	int totalBytes = PointNum * 2 * sizeof(float);
	float* pPos = (float*)malloc(totalBytes);
	memcpy(pPos, &dstPointArr[0], totalBytes);

	//�����ܳ���
	TotalLenth = 0;
	//int lenx = 0;
	//int leny = 0;
	float lenx = 0.0f;
	float leny = 0.0f;

	eachlenth=(float*)malloc((PointNum)*sizeof(float));

	for(i=0;i<PointNum-1;i++){
		lenx = std::abs(pPos[i*2]-pPos[i*2+2]);
		leny = std::abs(pPos[i*2+1]-pPos[i*2+3]);
		eachlenth[i] = std::sqrt(double(lenx*lenx + leny*leny));
		TotalLenth = TotalLenth + eachlenth[i];
	}

	//����ÿ���ַ����߶γ����ϵ�λ�� 
	if (CaptionWidth > TotalLenth) {
		free(eachlenth);
		return 0;
	}
	dtemp = TotalLenth*percentage;
	dtemp = CaptionWidth * 1.7; // ���������ּ��.
	if (dtemp>CaptionWidth) {
		CaptionWidth = dtemp;
	}
	CaptionStart = (TotalLenth - CaptionWidth) / 2;
	CaptionEnd = CaptionStart + CaptionWidth;
	dtemp = CaptionWidth / (CharNum - 1);

	//	charpos = (float *)malloc(CharNum * sizeof(float));
	charpos[0] = CaptionStart;
	for(i=0;i<CharNum-1;i++){
		charpos[i+1] = charpos[i] + dtemp;
	}

	int CurrentChar;//��ǰҪѰ��λ�õ��ַ���index
	float CurrentLenth;
	float PrevLenth;//��ǰ���ڴ�����߶ε����ľ���
	float l;
	float K;//�߶ε�б��
	float k;//���ߵ�б��
	float k1;
	float k2;

	CurrentChar = 0;
	CurrentLenth = 0;

	//ת��Ϊ������λ��
	//	tmpResult=(float*)malloc(CharNum * sizeof(float) * 2);

	for(i=0;i<PointNum-1;i++){
		PrevLenth=CurrentLenth;
		CurrentLenth = CurrentLenth + eachlenth[i];
		while ((CurrentLenth >= charpos[CurrentChar]) && (CurrentChar<CharNum)) {
			//��ǰ���ڱ�����

			l = charpos[CurrentChar] - PrevLenth;

			tmpResult[2*CurrentChar] = pPos[i*2] - (pPos[i*2] - pPos[i*2+2]) * l / eachlenth[i];
			tmpResult[2*CurrentChar+1] = pPos[i*2+1] - (pPos[i*2+1] - pPos[i*2+3]) * l /eachlenth[i];

			if (pPos[i*2+2]==pPos[i*2]) {
				//y���ƽ����
				if (pPos[i*2+3]<pPos[i*2+1]) {
					//��������
					Result[2*CurrentChar] = tmpResult[2*CurrentChar] - FontDistance;
				}
				else{
					//��������
					Result[2*CurrentChar] = tmpResult[2*CurrentChar] + FontDistance;
				}

				Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1];
			}
			else{
				if (pPos[i*2+1]==pPos[i*2+3]) {
					//x���ƽ����
					if (pPos[i*2+2]>pPos[i*2]) {
						//��������
						Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - FontDistance;					
					}
					else{
						//��������
						Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + FontDistance;
					}
					Result[2*CurrentChar] = tmpResult[2*CurrentChar];
				}
				else{

					k1=pPos[i*2+3]-pPos[i*2+1];
					k2=pPos[i*2+2]-pPos[i*2];
					K=k1/k2;
					k = -1/K;
					if (k>0) {
						//��x������
						if (k1<0) {
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] - (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - (k/sqrt(1+k*k)) * FontDistance; 
						}

						else{
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] + (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + (k/sqrt(1+k*k)) * FontDistance; 
						}
					}
					else{
						//��x��ɶ۽�
						if (k1<0) {
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] - (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] - (k/sqrt(1+k*k)) * FontDistance; 
						}
						else{
							//��������
							Result[2*CurrentChar] = tmpResult[2*CurrentChar] + (1/sqrt(1+k*k)) * FontDistance;
							Result[2*CurrentChar+1] = tmpResult[2*CurrentChar+1] + (k/sqrt(1+k*k)) * FontDistance; 
						}
					}
				}
			}

			CurrentChar++;
		}
	}


	free(eachlenth);
	free(pPos);
	//	free(tmpResult);
	//	free(charpos);

	return CurrentChar;*/
    return 0;
}

void GeometryUtils::CalcPolygonText(float xmin, float ymin, float xmax, float ymax, int Number, float* PointList, int ytest, float* x,float* y)
{
	int k;
	float yt, r = -1.0f;
	float t,xt;
	int quan = 0;
	t = ymax - (ymax - ymin) / 10;
	ymin = ymin + (ymax - ymin) / 10;
	ymax = t;
	for (k = 0; k < ytest; ++k)
	{
		//���������,����Խ��Ч��Խ��,��Ч�ʽ���
		yt = ymin + (ymax - ymin) / (ytest + 1) * (k + 1);		//ȡ��������
		if (k == 1)
		{
			quan = 1;
		}
		else 
		{
			quan = 0;
		}
		if ((t = GetMaxXRange(xmin - 2.0f, yt, Number, PointList, &xt, quan)) > r)
		{
			r = t;
			*x = xt;
			*y = yt;
			//std::cout << "x: " << xt << ", y: " << yt << std::endl;
		}
	}	
}

common::Float2 GeometryUtils::CalcPointPos(const common::Float2& pos, const common::Float2& widthAndHeight, std::vector<common::Rectangle<float>>& rectBeingUsed, const common::Float2& miss)
{
	float FivePiexl = 2.0f;
	common::Rectangle<float> m_rectTmp;
	m_rectTmp.Set(pos.X(),pos.Y(),pos.X()+widthAndHeight.X(),pos.Y()+widthAndHeight.Y());

	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return pos;
	}

	common::Float2 resultF;
	// �ֱ��ж������ļ�������
	//1	--	2 .-		3 +- 
	//4	-.	5 ..		6 +.
	//7	-+	8 .+		9 ++

	//6
	resultF.X() = pos.X() + FivePiexl + miss.X();
	resultF.Y() = pos.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//3
	resultF.X() = pos.X() + FivePiexl + miss.X();
	resultF.Y() = pos.Y() - FivePiexl - widthAndHeight.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//9
	resultF.X() = pos.X() + FivePiexl + miss.X();
	resultF.Y() = pos.Y() + FivePiexl + miss.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//2
	resultF.X() = pos.X();
	resultF.Y() = pos.Y() - FivePiexl - widthAndHeight.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//8
	resultF.X() = pos.X();
	resultF.Y() = pos.Y() + FivePiexl + miss.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//	1
	resultF.X() = pos.X() - FivePiexl - widthAndHeight.X();
	resultF.Y() = pos.Y() - FivePiexl - widthAndHeight.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//4
	resultF.X() = pos.X() - FivePiexl - widthAndHeight.X();
	resultF.Y() = pos.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}
	//7
	resultF.X() = pos.X() - FivePiexl - widthAndHeight.X();
	resultF.Y() = pos.Y() + FivePiexl + miss.Y();
	m_rectTmp.Set(resultF.X(),resultF.Y(),resultF.X()+widthAndHeight.X(),resultF.Y()+widthAndHeight.Y());
	if (!RectIsBeingUsed(m_rectTmp, rectBeingUsed))
	{
		rectBeingUsed.push_back(m_rectTmp);
		return resultF;
	}

	resultF.X() = 0;
	resultF.Y() = 0;
	return resultF;
}

bool GeometryUtils::RectIsBeingUsed(const common::Rectangle<float>& rc, const std::vector<common::Rectangle<float>>& rectBeingUsed)
{
	size_t  rectUsedSize = rectBeingUsed.size();
	for (size_t i = 0; i < rectUsedSize; ++i)
	{
		if (rc.InterSects(rectBeingUsed[i]))
		{
			return true;
		}
	}

	return false;
}


//������������
inline float GetVectorLength(const float vecX, const float vecY)
{
	return sqrt(vecX*vecX+vecY*vecY);
}

//���㵥λ����
inline void GetUnitVector(float& vecX, float& vecY)
{
	float vecLength = GetVectorLength(vecX, vecY);
	if(vecLength <= 0)
	{
		return;
	}
	vecX = vecX/vecLength;
	vecY = vecY/vecLength;
}

//���㴹ֱ����
inline void GetVeticalVec(const float vecX, const float vecY, float& vecResultX, float& vecResultY)
{
	vecResultX = vecY;
	vecResultY = -vecX;
	return GetUnitVector(vecResultX, vecResultY);
}

//����ƫ�Ƶ�
inline void GetOffsetPoint(const float vecX1, const float vecY1, const float vecX2, const float vecY2, const int offsetFactor, float& vecX, float& vecY, float& pointX, float& pointY)
{
	float vecResultX1, vecResultY1, vecResultX2, vecResultY2;
	GetVeticalVec(vecX1, vecY1, vecResultX1, vecResultY1);
	GetVeticalVec(vecX2, vecY2, vecResultX2, vecResultY2);
	//vecResultX1 = vecX1;
	//vecResultY1 = vecY1;
	//vecResultX2 = -vecX2;
	//vecResultY2 = -vecY2;
	//GetUnitVector(vecResultX1, vecResultY1);
	//GetUnitVector(vecResultX2, vecResultY2);

	vecX = vecResultX1+vecResultX2;
	vecY = vecResultY1+vecResultY2;
	GetUnitVector(vecX, vecY);

	float cosA = (vecX*vecX1+vecY*vecY1)/GetVectorLength(vecX, vecY)/GetVectorLength(vecX1, vecY1);
	float sinA = sqrt(1-cosA*cosA);

	if (sinA <= 0)
	{
		sinA = 1;
	}


	vecX = vecX*offsetFactor / sinA;
	vecY = vecY*offsetFactor / sinA;

	pointX = pointX + vecX;
	pointY = pointY + vecY;		

}
//����ƫ����
float* GeometryUtils::CalcEccentricityPoint(const float* pPointList, const int pointNum, const int offsetFactor, const bool isPolygon, int& newPointNum)
{
	// TODO(gaoyang): pointNumΪ��2
	// ��������Ϊ�������������У�������new�����ڴ棬�ⲿ�ͷ�
	//pPointList ����������������
	//pointNum �������
	//OffsetFactor <0Ϊ���� >0Ϊ���� ����ֵΪƫ�ƾ���
	//isPolygon �Ƿ�Ϊ�����
	//newPointNum �������
	//Add By Zyw 2018-05-08
	float* pNewPointList = nullptr;
	if(nullptr == pPointList || pointNum <= 0)
	{
		newPointNum = 0;
		return pNewPointList;
	}
	newPointNum = pointNum+1;
	pNewPointList = new float[newPointNum*2];
	memset(pNewPointList, 0, sizeof(float)*newPointNum*2);

	pNewPointList[0] = pPointList[0];
	pNewPointList[1] = pPointList[1];

	int i = 0;
	float vecX1, vecX2, vecY1, vecY2, vecX, vecY, vecResultX, vecResultY;

	newPointNum = 1;
	for(i = 1; i < pointNum; i++)
	{
		vecX = pPointList[i*2] - pPointList[i*2-2];
		vecY = pPointList[i*2+1] - pPointList[i*2-1];
		if(GetVectorLength(vecX, vecY) > 0.001) //���㲻�غ�
		{
			pNewPointList[newPointNum*2] = pPointList[i*2];
			pNewPointList[newPointNum*2+1] = pPointList[i*2+1];
			newPointNum++;
		}
	}

	if(isPolygon && newPointNum >= 3)
	{
		vecX = pNewPointList[0]-pNewPointList[newPointNum*2-2];
		vecY = pNewPointList[1]-pNewPointList[newPointNum*2-1];

		if(GetVectorLength(vecX, vecY) > 0.001)	//�������β���պ�
		{
			pNewPointList[newPointNum*2] = pPointList[0];
			pNewPointList[newPointNum*2+1] = pPointList[1];
			newPointNum++;

		}
	}

	float* pTempPointList = new float[newPointNum*2];
	memcpy(pTempPointList, pNewPointList, sizeof(float)*newPointNum*2);

	if(newPointNum == 1)
	{
		pNewPointList[0] = pTempPointList[0] + offsetFactor;
		pNewPointList[1] = pTempPointList[1];
	}
	else if(newPointNum == 2)
	{
		vecX = pTempPointList[2] - pTempPointList[0];
		vecY = pTempPointList[3] - pTempPointList[1];
		GetVeticalVec(vecX, vecY, vecResultX, vecResultY);

		pNewPointList[0] = pTempPointList[0] + vecResultX*offsetFactor;
		pNewPointList[1] = pTempPointList[1] + vecResultY*offsetFactor;
		pNewPointList[2] = pTempPointList[2] + vecResultX*offsetFactor;
		pNewPointList[3] = pTempPointList[3] + vecResultY*offsetFactor;

	}
	else if(newPointNum >= 3)
	{
		for(i = 1; i < newPointNum-1; i++)
		{
			vecX1 = pTempPointList[i*2] - pTempPointList[i*2-2];
			vecY1 = pTempPointList[i*2+1] - pTempPointList[i*2-1];

			vecX2 = pTempPointList[i*2+2] - pTempPointList[i*2];
			vecY2 = pTempPointList[i*2+3] - pTempPointList[i*2+1];

			pNewPointList[i*2] = pTempPointList[i*2];
			pNewPointList[i*2+1] = pTempPointList[i*2+1];

			GetOffsetPoint(vecX1, vecY1, vecX2, vecY2, offsetFactor, vecX, vecY, pNewPointList[i*2], pNewPointList[i*2+1]);

			if(i == 1 && !isPolygon)
			{
				pNewPointList[0] = pTempPointList[0] + vecX;
				pNewPointList[1] = pTempPointList[1] + vecY;
			}
			if(i == newPointNum-2 && !isPolygon)
			{
				pNewPointList[newPointNum*2-2] = pTempPointList[newPointNum*2-2] + vecX;
				pNewPointList[newPointNum*2-1] = pTempPointList[newPointNum*2-1] + vecY;
			}
		}

		if(isPolygon)
		{	
			vecX1 = pTempPointList[2] - pTempPointList[0];
			vecY1 = pTempPointList[3] - pTempPointList[1];

			vecX2 = pTempPointList[newPointNum*2-2] - pTempPointList[newPointNum*2-4];
			vecY2 = pTempPointList[newPointNum*2-1] - pTempPointList[newPointNum*2-3];


			pNewPointList[0] = pTempPointList[0];
			pNewPointList[1] = pTempPointList[1];

			GetOffsetPoint(vecX1, vecY1, vecX2, vecY2, offsetFactor, vecX, vecY, pNewPointList[0], pNewPointList[1]);

			pNewPointList[newPointNum*2-2] = pNewPointList[0];
			pNewPointList[newPointNum*2-1] = pNewPointList[1];

		}

	}

	delete[] pTempPointList;
	return pNewPointList;
}

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Protected:
float GeometryUtils::CalDisp(float x1, float y1, float x2, float y2)
{
	return (float)sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float GeometryUtils::PointLineLength(float xx, float yy, float x1, float y1, float x2, float y2)
{
	float a, b, c, ang1, ang2, ang;
	float pi = 3.141592653589793;
	a = CalDisp(x1, y1, xx, yy); if (a == 0.0)return 0.0;
	b = CalDisp(x2, y2, xx, yy); if (b == 0.0)return 0.0;
	c = CalDisp(x1, y1, x2, y2);
	if (c == 0.0) return a;
	if (a<b)
	{
		if (y1 == y2)
		{
			if (x1<x2)
				ang1 = 0;
			else
				ang1 = (float)pi;
		}
		else
		{
			ang1 = (float)acos((x2 - x1) / c);
			if (y1>y2)ang1 = (float)pi * 2 - ang1;
		}
		ang2 = (float)acos((xx - x1) / a);
		if (y1>yy)ang2 = (float)pi * 2 - ang2;
		ang = ang2 - ang1;
		if (ang<0)ang = -ang;
		if (ang>pi) ang = (float)pi * 2 - ang;
		if (ang>pi / 2)
			return a;
		else
			return (a*(float)sin(ang));
	}
	else
	{
		if (y1 == y2)
		{
			if (x1<x2)
				ang1 = (float)pi;
			else
				ang1 = 0;
		}
		else
		{
			ang1 = (float)acos((x1 - x2) / c);
			if (y2>y1)ang1 = (float)pi * 2 - ang1;
		}
		ang2 = (float)acos((xx - x2) / b);
		if (y2>yy)ang2 = (float)pi * 2 - ang2;
		ang = ang2 - ang1;
		if (ang<0) ang = -ang;
		if (ang>pi) ang = (float)pi * 2 - ang;
		if (ang>pi / 2)
			return b;
		else
			return(b*(float)sin(ang));
	}
}

float GeometryUtils::GetMaxXRange(float x, float y, int Numble, float* PointList, float* xx, int quan)
{
	int i, j, k, m;
	float c = 0.0f;
	int s = 0;
	float xt[100];
	float xy[50];
	float xc[50];
	xt[1] = xt[0] = x;
	
	for (i = 0, j = Numble-1; i < Numble; j = i++) 
	{
		if (((PointList[i*2+1] <= y) && (y < PointList[j*2+1])) || ((PointList[j*2+1] < y) && (y <= PointList[i*2+1]))) 
		{
			xt[s++] = (PointList[j*2] - PointList[i*2]) * (y - PointList[i*2+1]) / (PointList[j*2+1] - PointList[i*2+1]) + PointList[i*2];
			if((PointList[i*2+1] == y) && ((PointList[j*2+1]-y)>0) == ((PointList[((i+1)%Numble)*2+1]-y)>0)) // ������.
			{	
				s--;
			}
		}
	}
	if (s == 0)
	{
		*xx = xt[0];
		return 0;
	}
	if (s < 3)
	{
		*xx = (xt[1] + xt[0]) / 2;
		return std::abs(xt[1] - xt[0]) + std::abs(xt[1] - xt[0]) * quan / 4;
	}
	//Ϊxt��������
	for (i = 0; i < s; ++i)
		for (j = i+1; j < s; ++j)
			if (xt[i] > xt[j])
			{
				c = xt[i];
				xt[i] = xt[j];
				xt[j] = c;
			}
			
			/*
			if(xt[0]==xt[1]){	//ǰ�������
			for(i=1;i<s;i++)
			xt[i]=xt[i+1];
			s--;
			}
			if(xt[s-1]==xt[s-2]){	//���������
			s--;
			}
			*/
			
			for (k = 0; k < s; k+=2)
			{
				xy[k/2] = xt[k+1] - xt[k];
				xc[k/2] = (xt[k+1] + xt[k]) / 2;
			}

			//Ϊxy��������
			for(k = 0; k < s/2; ++k)
				for (m = k+1; m < s/2; ++m)
					if (xy[k] < xy[m])
					{
						c = xy[k];
						xy[k] = xy[m];
						xy[m] = c;
						c = xc[k];
						xc[k] = xc[m];
						xc[m] = c;
					}

					*xx = xc[0];
					return xy[0] + xy[0] * quan / 4;
}

// TODO:
void GeometryUtils::GetBoundryRing(const float* pointList2, const int pointNum2, OGRPolygon& pClipPolygon2, const int vacuateFactor, const bool bVacuate)
{
    /*OGRLinearRing ClipRing2;
    if (bVacuate)
    {
		//��ϡ
		VacuateNTHPointAlgModel nthpModel;

		nthpModel.SetN(vacuateFactor);
        std::vector<double> outputVec;
        VacuateAlgUtils::Vacuate(&nthpModel, (float*)pointList2, pointNum2 * 2, outputVec);

        *//*for (size_t i = 0; i < outputVec.size()/2; ++i)
        {
            ClipRing2.addPoint(outputVec[2 * i], outputVec[2 * i + 1]);
        }*//*

        double* padx = (double*)malloc(outputVec.size()/2 * sizeof(double));
        double* pady = (double*)malloc(outputVec.size()/2 * sizeof(double));
        for (unsigned int k = 0; k < outputVec.size()/2; ++k)
        {
            padx[k] = outputVec[2 * k];
            pady[k] = outputVec[2 * k + 1];
        }
        ClipRing2.setPoints((int)(outputVec.size()/2), padx, pady);
        free(padx);
        free(pady);
    }
    else
    {
        *//*for (size_t i = 0; i < pointNum2; ++i)
        {
            ClipRing2.addPoint(pointList2[2 * i], pointList2[2 * i + 1]);
        }*//*
        double* padx = (double*)malloc(pointNum2 * sizeof(double));
        double* pady = (double*)malloc(pointNum2 * sizeof(double));
        for (int k = 0; k < pointNum2; ++k)
        {
            padx[k] = pointList2[2 * k];
            pady[k] = pointList2[2 * k + 1];
        }
        ClipRing2.setPoints(pointNum2, padx, pady);
        free(padx);
        free(pady);
    }

    ClipRing2.closeRings();
    pClipPolygon2.addRing(&ClipRing2);
    pClipPolygon2.closeRings();*/
}

bool GeometryUtils::IsPointOnPoly(const float& x, const float& y, const OGRPolygon& ClipPolygon2)
{
    OGRPoint point;
    point.setX((double)x);
    point.setY((double)y);

    if (!point.IsValid())
    {
        return false;
    }

    if (point.Intersects(&ClipPolygon2) || point.Within(&ClipPolygon2))
    {
        return  true;
    }

    return false;
}

bool GeometryUtils::IsLineOnPoly(const float* pointList1, const int pointNum1, const OGRPolygon& ClipPolygon2, const int vacuateFactor, const bool bVacuate)
{
   /* OGRLineString ClipRing1;
    if (bVacuate)
    {
        //boost::timer t2;
        //��ϡ
        VacuateNTHPointAlgModel nthpModel;
        nthpModel.SetN(vacuateFactor);

        std::vector<double> outputVec;
        VacuateAlgUtils::Vacuate(&nthpModel, (float*)pointList1, pointNum1 * 2, outputVec);
      *//*  for (size_t i = 0; i < outputVec.size()/2; ++i)
        {
            ClipRing1.addPoint(outputVec[2 * i], outputVec[2 * i + 1]);
        }*//*

        double* padx = (double*)malloc(outputVec.size()/2 * sizeof(double));
        double* pady = (double*)malloc(outputVec.size()/2 * sizeof(double));
        for (unsigned int k = 0; k < outputVec.size()/2; ++k)
        {
            padx[k] = outputVec[2 * k];
            pady[k] = outputVec[2 * k + 1];
        }
        ClipRing1.setPoints((int)(outputVec.size() / 2), padx, pady);
        free(padx);
        free(pady);

        //std::cout << "line ��ϡ, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;
    } 
    else
    {
        *//*for (size_t i = 0; i < pointNum1; ++i)
        {
            ClipRing1.addPoint(pointList1[2 * i], pointList1[2 * i + 1]);
        }*//*
        //boost::timer t2;
        double* padx = (double*)malloc(pointNum1 * sizeof(double));
        double* pady = (double*)malloc(pointNum1 * sizeof(double));
        for (int k = 0; k < pointNum1; ++k)
        {
            padx[k] = pointList1[2 * k];
            pady[k] = pointList1[2 * k + 1];
        }
        ClipRing1.setPoints(pointNum1, padx, pady);
        free(padx);
        free(pady);

        //std::cout << "line ����ϡ, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;
    }
   
    ClipRing1.closeRings();

    //boost::timer t2;

    if (ClipPolygon2.Intersects(&ClipRing1) || ClipPolygon2.Contains(&ClipRing1))
    {
        return true;
    }
    */
    //std::cout << "line Intersects, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;

    return false;
}

bool GeometryUtils::IsPolyOnPoly(const float* pointList1, const int pointNum1, const OGRPolygon& ClipPolygon2, const int vacuateFactor, const bool bVacuate)
{
//    OGRPolygon ClipPolygon1;
//    OGRLinearRing ClipRing1;
//    if (bVacuate)
//    {
//        //boost::timer t2;
//        //��ϡ
//        VacuateNTHPointAlgModel nthpModel;
//        nthpModel.SetN(vacuateFactor);
//
//        std::vector<double> outputVec;
//        VacuateAlgUtils::Vacuate(&nthpModel, (float*)pointList1, pointNum1 * 2, outputVec);
//
//    /*    for (size_t i = 0; i < outputVec.size()/2; ++i)
//        {
//            ClipRing1.addPoint(outputVec[2 * i], outputVec[2 * i + 1]);
//        }
//        */
//        double* padx = (double*)malloc(outputVec.size()/2 * sizeof(double));
//        double* pady = (double*)malloc(outputVec.size()/2 * sizeof(double));
//        for (unsigned int k = 0; k < outputVec.size()/2; ++k)
//        {
//            padx[k] = outputVec[2 * k];
//            pady[k] = outputVec[2 * k + 1];
//        }
//        ClipRing1.setPoints((int)(outputVec.size() / 2), padx, pady);
//        free(padx);
//        free(pady);
//
//        //std::cout << "poly ��ϡ, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;
//    }
//    else
//    {
//        //boost::timer t2;
//        double* padx = (double*)malloc(pointNum1 * sizeof(double));
//        double* pady = (double*)malloc(pointNum1 * sizeof(double));
//        for (int k = 0; k < pointNum1; ++k)
//        {
//            padx[k] = pointList1[2 * k];
//            pady[k] = pointList1[2 * k + 1];
//        }
//        ClipRing1.setPoints(pointNum1, padx, pady);
//        free(padx);
//        free(pady);
//        //for (size_t i = 0; i < pointNum1; ++i)
//        //{
//        //    ClipRing1.addPoint(pointList1[2 * i], pointList1[2 * i + 1]);
//        //}
//
//        //std::cout << "poly ����ϡ, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;
//    }
//
//    if (ClipRing1.IsEmpty())
//    {
//        std::cout << "ClipRing1.IsEmpty()" << std::endl;
//        ClipRing1.closeRings();
//        return false;
//    }
//
//    //boost::timer t2;
//    ClipRing1.closeRings();
//    ClipPolygon1.addRing(&ClipRing1);
//    ClipPolygon1.closeRings();
//
//    if (ClipPolygon2.Intersects(&ClipPolygon1) || ClipPolygon2.Contains(&ClipPolygon1) || ClipPolygon1.Contains(&ClipPolygon2))
//    {
//        return true;
//    }

    //std::cout << "poly Intersects, ����ʱ��: " << t2.elapsed() * 1000 << "ms" << std::endl;

    return false;
}


template<typename T1>
bool GeometryUtils::IsPointOnPoint(const T1 x, const T1 y, const T1 x1, const T1 y1, const T1 radius)
{
	if (x >= x1 - radius && y >= y1 - radius && x <= x1 + radius && y <= y1 + radius)
	{
		return true;
	}

	return false;
}

template<typename T1, typename T2>
bool GeometryUtils::IsPointOnLine(const T1 x, const T1 y, const T2* pointList, const size_t pointNum, const T2 lineWidth)
{
	float tmp;
	/*if (lineWidth < 1.0)
	{
		tmp = 1.0;
	}*/
	//else
	{
		tmp = (float)lineWidth;
	}

	for (int i = 0, j = 1; j < pointNum; i++, j++)
	{
		if ((PointLineLength((float)x, (float)y, (float)pointList[i * 2], (float)pointList[i * 2 + 1], (float)pointList[j * 2], (float)pointList[j * 2 + 1])) <= (tmp))
		{
			return true;
		}
		else
		{
			continue;
		}
	}

	return false;
}

template<typename T1, typename T2>
bool GeometryUtils::IsPointOnPoly(const T1 x, const T1 y, const T2* pointList, const size_t pointNum)
{
	size_t i, j;
	i = j = 0;
	int c = 0;
	for (i = 0, j = size_t(pointNum - 1); i < pointNum; j = i++)
	{
		if ((pointList[j * 2 + 1] - pointList[i * 2 + 1]) == 0)
		{
			continue;
		}

		if ((((pointList[i * 2 + 1] <= y) && (y < pointList[j * 2 + 1]) ||
			  ((pointList[j * 2 + 1] <= y) && (y < pointList[i * 2 + 1]))) &&
			 (x < (pointList[j * 2] - pointList[i * 2]) * (y - pointList[i * 2 + 1]) / (pointList[j * 2 + 1] - pointList[i * 2 + 1]) + pointList[i * 2])))
		{
			c = !c;
		}
	}

	if (c != 0)
	{
		return true;
	}

	return false;
}


template<typename T1>
bool GeometryUtils::IsLineOnPoly(const T1* pointList1, const size_t pointNum1, const T1* pointList2, const size_t pointNum2)
{
	//for (int i = 0; i < pointNum1; ++i)
	//{
	//	if (IsPointOnPoly(pointList1[2 * i], pointList1[2 * i + 1], pointList2, pointNum2))
	//	{
	//		return true;
	//	}
	//}

	OGRLineString ClipRing1;
	for (size_t i = 0; i < pointNum1; ++i)
	{
		ClipRing1.addPoint(pointList1[2 * i], pointList1[2 * i + 1]);
	}
	ClipRing1.closeRings();

	OGRPolygon ClipPolygon2;
	OGRLinearRing ClipRing2;
	for (size_t i = 0; i < pointNum2; ++i)
	{
		ClipRing2.addPoint(pointList2[2 * i], pointList2[2 * i + 1]);
	}
	ClipRing2.closeRings();
	ClipPolygon2.addRing(&ClipRing2);
	ClipPolygon2.closeRings();

	if (ClipPolygon2.Intersects(&ClipRing1) || ClipPolygon2.Contains(&ClipRing1))
	{
		return true;
	}

	return false;
}

template<typename T2>
bool GeometryUtils::IsPolyOnPoly(const T2* pointList1, const size_t pointNum1, const T2* pointList2, const size_t pointNum2)
{/*
	for (int i = 0; i < pointNum1; ++i)
	{
		if (IsPointOnPoly(pointList1[2 * i], pointList1[2 * i + 1], pointList2, pointNum2))
		{
			return true;
		}
	}

	return false;*/

	//common::Rectangle<T2> list1Rect;
	//list1Rect.g_left = 9999.0;
	//list1Rect.g_right = 0.0;
	//list1Rect.g_top = 9999.0;
	//list1Rect.g_bottom = 0.0;
	//for (size_t i = 0; i < pointNum1; ++i)
	//{
	//    list1Rect.g_left = std::min(list1Rect.g_left, pointList1[2 * i]);
	//    list1Rect.g_right = std::max(list1Rect.g_right, pointList1[2 * i]);

	//    list1Rect.g_top = std::min(list1Rect.g_top, pointList1[2 * i + 1]);
	//    list1Rect.g_bottom = std::max(list1Rect.g_bottom, pointList1[2 * i + 1]);
	//}

	//common::Rectangle<T2> list2Rect;
	//list2Rect.g_left = 9999.0;
	//list2Rect.g_right = 0.0;
	//list2Rect.g_top = 9999.0;
	//list2Rect.g_bottom = 0.0;
	//for (size_t i = 0; i < pointNum2; ++i)
	//{
	//    list2Rect.g_left = std::min(list2Rect.g_left, pointList2[2 * i]);
	//    list2Rect.g_right = std::max(list2Rect.g_right, pointList2[2 * i]);

	//    list2Rect.g_top = std::min(list2Rect.g_top, pointList2[2 * i + 1]);
	//    list2Rect.g_bottom = std::max(list2Rect.g_bottom, pointList2[2 * i + 1]);
	//}

	//if (list2Rect.InterSects(list1Rect) || list2Rect.Contain(list1Rect) || list1Rect.Contain(list2Rect))
	//{
	//    return true;
	//}

	OGRPolygon ClipPolygon1;
	OGRLinearRing ClipRing1;
	for (size_t i = 0; i < pointNum1; ++i)
	{
		ClipRing1.addPoint(pointList1[2 * i], pointList1[2 * i + 1]);
	}
	ClipRing1.closeRings();
	ClipPolygon1.addRing(&ClipRing1);
	ClipPolygon1.closeRings();

	OGRPolygon ClipPolygon2;
	OGRLinearRing ClipRing2;
	for (size_t i = 0; i < pointNum2; ++i)
	{
		ClipRing2.addPoint(pointList2[2 * i], pointList2[2 * i + 1]);
	}
	ClipRing2.closeRings();
	ClipPolygon2.addRing(&ClipRing2);
	ClipPolygon2.closeRings();

	if (ClipPolygon1.Intersects(&ClipPolygon2) || ClipPolygon1.Contains(&ClipPolygon2) || ClipPolygon2.Contains(&ClipPolygon1))
	{
		return true;
	}

	return false;

}

/*
//�� ���ཻ
bool GeometryUtils::IsPolyInsectPoly(const T2* pointList1, const size_t pointNum1, const T2* pointList2, const size_t pointNum2)
{
	unsigned char  inPont = 0;
	unsigned char  outPoint = 0;
	for (int i = 0; i < pointNum1; ++i)
	{

		if (IsPointOnPoly(pointList1[2 * i]), pointList1(2 * i + 1), pointList2, pointNum2)
		{
			inPont = 1;
		}
		else
		{
			outPont = 1;
		}
		if ( inPoint && outPoint)
		{
		return true;
		}
	}
	return false;
}
*/

/*
bool VectorPyramidFile::IsPointOnPoly(const float x, const float y, const float* pointList, const size_t pointNum)
{
	size_t i, j;
	i = j = 0;
	int c = 0;
	for (i=0; i< size_t(pointNum - 1); j = i++)
	{
		if ((pointList[i * 2 + 1] < y && pointList[j * 2 + 1]>=y
		||   pointList[j * 2 + 1]< y && pointList[i * 2 + 1] >=y)
		&&  (pointList[i * 2]<=x || pointList[j * 2]<=x))
		{
				c ^= (pointList[i * 2]+(y-pointList[i * 2 + 1])/(pointList[j * 2 + 1]-pointList[i * 2 + 1])*(pointList[j * 2]-pointList[i * 2 ])<x);
		}
		j=i;
	}

	if (c != 0)
	{
	return true;
	}

	return false;
}

*/

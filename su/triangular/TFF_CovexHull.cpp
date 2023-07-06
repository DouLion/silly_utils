#pragma once

#include "TFF_CovexHull.h"

bool sortAngle(PointAndAngle A, PointAndAngle B)
{
	return (A.angle < B.angle) || (A.angle <= B.angle && A.angle >= B.angle && A.len > B.len);
}

covexHull::covexHull(void)
{

}


void covexHull::addPoint(const vertex& point)
{

	m_points.push_back(point);

}

inline vector<vertex>::iterator covexHull::findLeftPoint()
{

	//最左边的点，就是x的值最小的点

	std::vector<vertex>::iterator ret = m_points.begin();

	for (std::vector<vertex>::iterator it = m_points.begin(); it != m_points.end(); ++it)
	{
		if (it->GetX() < ret->GetX())
		{
			ret = it;
		}
	}
	return ret;
}


//
//void covexHull::draw(CClientDC & dc)
//
//{
//
//	//先绘制所有的点
//
//	for(vector<vertex>::iterator it = m_points.begin() ; it != m_points.end() ; ++it)
//
//	{
//
//		dc.Ellipse(it->GetX()-3, it->GetY()-3, it->GetX()+3, it->GetY()+3);
//
//	}
//
//	//绘制简单多边形
//
//	{
//
//		deque<PointAndAngle>::iterator it =  m_pointAndAngle.begin();
//
//		if(it != m_pointAndAngle.end())//防止列表为空
//
//			dc.MoveTo(it->point.GetX(),it->point.GetY());
//
//		for(; it!= m_pointAndAngle.end() ; ++it)
//
//		{
//
//			dc.LineTo(it->point.GetX(),it->point.GetY());
//
//		}
//
//		if(m_pointAndAngle.size() != 0)//防止列表为空
//
//			dc.LineTo(m_pointAndAngle.begin()->point.GetX(),m_pointAndAngle.begin()->point.GetY());
//
//	}
//
//	//绘制凸多边形
//
//	{
//
//		CPen * oldPen;
//
//		CPen * newPen = new CPen(PS_SOLID,1,RGB(255,0,0));//更改画笔颜色
//
//		oldPen = dc.SelectObject(newPen);
//
//
//
//		deque<vertex>::iterator it =  m_stack.begin();
//
//		if(it != m_stack.end())
//
//			dc.MoveTo(it->GetX(),it->GetY());
//
//		for(; it!= m_stack.end() ; ++it)
//
//		{
//
//			dc.LineTo(it->GetX(),it->GetY());
//
//		}
//
//		if(m_stack.size() != 0)
//
//			dc.LineTo(m_stack.begin()->GetX(),m_stack.begin()->GetY());
//
//
//
//		dc.SelectObject(&oldPen);
//
//		delete newPen;
//
//	}
//
//
//
//}



void covexHull::compute()
{
	computeSimplePolygon();//先计算简单多边形
	computeCovexHull();//计算凸多边形
	GetBoundary();
}


void covexHull::computeSimplePolygon()
{

	m_pointAndAngle.clear();

	vector<vertex>::iterator it = findLeftPoint();//先找到最左侧的点

	vertex point(*it);//将这个点保存下来

	m_points.erase(it);//将最左侧的点从列表中删除（因为这个点自身求角度无意义）

	PointAndAngle paa;
	for (vector<vertex>::iterator i = m_points.begin(); i != m_points.end(); ++i)//计算所有点与最左侧点的角度
	{

		paa.point = *i;
		paa.len = sqrt(pow(i->GetX() - point.GetX(), 2.0) + pow(i->GetY() - point.GetY(), 2.0));

		if (i->GetX() - point.GetX() == 0)//要先判断除数为的情况

		{

			if (i->GetY() > point.GetY())

				paa.angle = 90.0 / 360.0 * atan(1.0) * 4;//PI = atan(1.0)*4

			else

				paa.angle = -90.0 / 360.0 * atan(1.0) * 4;
		}

		else

			paa.angle = atan(double(double(i->GetY() - point.GetY()) / double(i->GetX() - point.GetX())));//计算角度

		m_pointAndAngle.push_back(paa);//放入简单多变行容器

	}


	std::sort(m_pointAndAngle.begin(), m_pointAndAngle.end(), sortAngle);//按照角度从小到大排序

	paa.point = point;

	m_pointAndAngle.push_front(paa);//最后将最左侧的点放入集合

	m_points.push_back(point);//将最左侧点放入点集合



}


double covexHull::computeS(const vertex& p1, const vertex& p2, const vertex& p3) const
{

	return (p3.GetX() - p1.GetX()) * (-p2.GetY() + p1.GetY()) - (-p3.GetY() + p1.GetY()) * (p2.GetX() - p1.GetX());//计算S，注意实际坐标系与屏幕坐标系之间的转换

}


double covexHull::GetDistance(vertex& A, vertex& B)
{
	return sqrt(pow(A.GetX() - B.GetX(), 2) + pow(A.GetY() - B.GetY(), 2));
}

void covexHull::GetBoundary()
{
	int pointNum = (int)m_points.size();
	int i, j, len, index;
	bool bSkip = false;
	bool bAdd = false;
	deque<vertex>::iterator it = m_stack.begin();
	for (j = 0; j < pointNum; j++)
	{
		bSkip = false;
		bAdd = false;
		index = -1;
		len = (int)m_stack.size();
		it = m_stack.begin();
		for (i = 0; i < len; i++)
		{
			if (m_points[j].GetIndex() == m_stack[i].GetIndex() || m_points[j].GetIndex() == m_stack[(i + 1) % len].GetIndex())
			{
				bSkip = true;
				break;
			}
			if (GetDistance(m_points[j], m_stack[i]) + GetDistance(m_points[j], m_stack[(i + 1) % len]) <= GetDistance(m_stack[i], m_stack[(i + 1) % len]))
			{
				bAdd = true;
				index = i + 1;
			}
		}
		if (bSkip)
		{
			continue;
		}
		if (bAdd)
		{
			m_stack.insert(it + index, m_points[j]);
		}
	}

}

void covexHull::computeCovexHull()
{

	m_stack.clear();


	if (m_pointAndAngle.size() < 3)//当小于个点，就不用计算了
		return;

	m_stack.push_front(m_pointAndAngle.at(0).point);//先将前两个点放入栈中
	m_stack.push_front(m_pointAndAngle.at(1).point);

	deque<PointAndAngle>::iterator it = m_pointAndAngle.begin() + 2;//迭代器先要移动两个位置（因为那两个位置已经放入栈中了）

	for (; it != m_pointAndAngle.end();)//迭代求解
	{
		/*	if(fabs((it->point.GetX() - 124.116)*(it->point.GetY() - 42.583) )< 0.00000001)
			{
				int cc = 0;
				cc =1;
			}*/
		if (computeS(m_stack.at(1), m_stack.at(0), it->point) >= 0)//当S大于，此时点在直线的右侧
		{
			if (computeS(m_stack.back(), m_stack.front(), it->point) >= 0)//当S大于，将点压入栈中，否则不压入（不进栈，迭代器移动，相当于reject）
			{
				m_stack.push_front(it->point);
			}
			++it;//迭代器移动
		}
		else//S小于说明点在直线左侧，当前栈顶肯定不是凸点
		{
			m_stack.pop_front();//弹出栈顶
			if (m_stack.size() < 2)//栈内元素太少，将当前点直接填入栈中
			{
				m_stack.push_front(it->point);
			}
			//注意这里迭代器并没有移动
		}

	}


}


covexHull::~covexHull(void)
{

}



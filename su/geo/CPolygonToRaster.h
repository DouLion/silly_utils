#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <set>
#include <map>
#include <string>
#include "math.h"

#define EPS 1E-10

namespace CVectorToRaster {

	inline char toupper(char c)	{
		if (c >= 'a' && c <= 'z')//是小写字母，执行转换。
			c -= 'a' - 'A';//转成大写。
		return c;
	}

	std::string to_upper(std::string& input)	{
		std::string OutPut = input;
		char* p = (char*)OutPut.c_str();
		while (*p)
		{
			*p++ = toupper(*p);
		}
		return OutPut;
	}

	template<typename T1, typename T2>
	static double min(T1 x, T2 y) {
		if (x <= y) {
			return x;
		}
		return y;
	}

	template<typename T1, typename T2>
	static double max(T1 x, T2 y) {
		if (x >= y) {
			return x;
		}
		return y;
	}
	template<typename T1, typename T2>
	//点在面内
	static int PointRgn(T1 x, T1 y, int Numble, T2* PointList)
	{
		int i, j;
		int c = 0;
		for (i = 0, j = Numble - 1; i < Numble; j = i++)
		{
			if ((((PointList[i * 2 + 1] <= y) && (y < PointList[j * 2 + 1]) ||
				((PointList[j * 2 + 1] <= y) && (y < PointList[i * 2 + 1]))) &&
				(x < (PointList[j * 2] - PointList[i * 2]) * (y - PointList[i * 2 + 1]) / (PointList[j * 2 + 1] - PointList[i * 2 + 1]) + PointList[i * 2])))
			{
				c = !c;
			}
		}
		return c;
	}

	template<typename T1, typename T2>
	static int PointInRgn(T1 x, T1 y, std::vector<T2>& PointList) {
		if (PointList.size() < 6) {
			return 0;
		}
		return PointRgn(x, y, PointList.size()/2, &(PointList[0]));
	}

	class Range {
	public:
		Range() {
			Clear();
		};
		~Range() {
		};

		void Clear() {
			MinX = MinY = 1E30;
			MaxX = MaxY = -1E30;
		}

		double MinX;
		double MaxX;
		double MinY;
		double MaxY;

		template<typename T>
		void UpdateRange(std::vector<T>& vVertex) {
			if (vVertex.empty()) {
				return;
			}
			UpdateRange(&(vVertex[0]), vVertex.size()/2);
		}

		template<typename T>
		void UpdateRange(T* pData, int Num) {
			if (pData == nullptr || Num <= 0) {
				return;
			}
			for (int i = 0; i < Num; i++) {
				if (MinX > pData[i * 2]) {
					MinX = pData[i * 2];
				}
				if (MaxX < pData[i * 2]) {
					MaxX = pData[i * 2];
				}
				if (MinY > pData[i * 2 + 1]) {
					MinY = pData[i * 2 + 1];
				}
				if (MaxY < pData[i * 2 + 1]) {
					MaxY = pData[i * 2 + 1];
				}
			}
		}

		void UpdateRange(Range& Bound) {
			if (MinX > Bound.MinX) {
				MinX = Bound.MinX;
			}
			if (MaxX < Bound.MaxX) {
				MaxX = Bound.MaxX;
			}
			if (MinY > Bound.MinY) {
				MinY = Bound.MinY;
			}
			if (MaxY < Bound.MaxY) {
				MaxY = Bound.MaxY;
			}
		}

	};

	//点
	class Point {
	public:
		Point() {
		};
		Point(double X, double Y) {
			x = X;
			y = Y;
		};
		~Point() {
		};
		double x;
		double y;

		Point operator + (Point p) { return Point(x + p.x, y + p.y); }
		Point operator - (Point p) { return Point(x - p.x, y - p.y); }
		Point operator * (double a) { return Point(a * x, a * y); }
		Point operator / (double a) { return Point(x / a, y / a); }

		double abs() { return sqrt(norm()); }
		double norm() { return x * x + y * y; }

		bool operator < (const Point& p) const {
			return x != p.x ? x < p.x : y < p.y;
		}

		bool operator == (const Point& p) const {
			return fabs(x - p.x) < EPS && fabs(y - p.y) < EPS;
		}

	};

	typedef Point Vector;//Vector类，向量 

	//线段
	class LineSegment {
	public:
		LineSegment() {
		};
		LineSegment(Point p1, Point p2) {
			BegP = p1;
			EndP = p2;
		};
		~LineSegment() {
		};
		Point BegP;
		Point EndP;

		static double dot(Vector a, Vector b) {//内积 
			return a.x * b.x + a.y * b.y;
		}
		static double cross(Vector a, Vector b) {//外积 
			return a.x * b.y - a.y * b.x;
		}

		//计算交点
		static bool getCrossPoint(LineSegment& s1, LineSegment& s2, Point& crossPoint) {
			Vector base = s2.EndP - s2.BegP;
			double d1 = abs(cross(base, s1.BegP - s2.BegP));
			double d2 = abs(cross(base, s1.EndP - s2.BegP));
			double t = d1 / (d1 + d2);
			crossPoint = s1.BegP + (s1.EndP - s1.BegP) * t;
			if (t < 0 || t > 1) {
				return false;
			}
			return true;
		}

		bool getCrossPoint(LineSegment& Segment, Point& crossPoint) {
			return getCrossPoint(*this, Segment, crossPoint);
		}

		bool getCrossPoint(Point p1, Point p2, Point& crossPoint) {
			LineSegment Segment(p1, p2);
			return getCrossPoint(*this, Segment, crossPoint);
		}

		bool getCrossPoint(double x1, double y1, double x2, double y2,  Point& crossPoint) {
			LineSegment Segment(Point(x1, y1), Point(x2, y2));
			return getCrossPoint(*this, Segment, crossPoint);
		}

	};

	class Geometry {
	public:
		Geometry() {
			GeoType = 0;
		}
		~Geometry() {}
		int GeoType;
	};

	class Polygon : public Geometry {
	public:
		Polygon() {
			GeoType = 1;
		};
		~Polygon() {
		};

		//外边界 x, y, x, y.....
		std::vector<double> vVertex;
		//洞 x, y, x, y.....
		std::vector < std::vector<double>> vHoles;
		//外包围框
		Range Bound;

		Range GetBound() {
			return Bound;
		}

		//获取外边界
		std::vector<double>* GetVertex() {
			return &vVertex;
		}

		int HoleNumber() {
			return vHoles.size();
		}

		//获取第k个洞
		std::vector<double>* GetHole(int k) {

			if (k < vHoles.size() && k >= 0) {
				return &(vHoles[k]);
			}
			return nullptr;
		}

		void Clear() {
			vVertex.clear();
			vHoles.clear();
			Bound.Clear();
		}

		//闭合面
		template<typename T>
		void CloseRing(std::vector<T>& vVertex) {
			if (vVertex.size() < 4) {
				return;
			}
			int Num = vVertex.size() / 2;

			Vector v(vVertex[Num * 2 - 2] - vVertex[0], vVertex[Num * 2 - 1] - vVertex[1]);
			if (v.abs() > EPS) {
				vVertex.push_back(vVertex[0]);
				vVertex.push_back(vVertex[1]);
			}
		}

		//判断点在面内
		bool InRegion(double x, double y) {
			for (auto hole : vHoles) {
				if (PointInRgn(x, y, hole)) {
					return false;
				}
			}
			return PointInRgn(x, y, vVertex);
		}

		//添加多边形
		template<typename T>
		void AddVertex(std::vector<T>& vRingVertex) {
			vVertex.clear();
			vVertex.insert(vVertex.end(), vRingVertex.begin(), vRingVertex.end());
			CloseRing(vVertex);
			Bound.UpdateRange(vVertex);
		}

		//添加多边形
		template<typename T>
		void AddVertex(T* pData, int Num) {
			if (nullptr == pData || Num <= 0) {
				return;
			}
			vVertex.clear();
			vVertex.resize(Num, 0);
			for (int i = 0; i < Num*2; i++) {
				vVertex[i] = pData[i];
			}
			CloseRing(vVertex);
			Bound.UpdateRange(vVertex);
		}

		//添加洞
		template<typename T>
		void AddHole(std::vector<T>& vHoleVertex) {
			std::vector<double> vData;
			vData.insert(vData.end(), vHoleVertex.begin(), vHoleVertex.end());
			CloseRing(vData);
			vHoles.push_back(vData);
		}

		//添加洞
		template<typename T>
		void AddHole(T* pData, int Num) {
			if (nullptr == pData || Num <= 0) {
				return;
			}
			std::vector<double> vData;
			vData.resize(Num, 0);
			for (int i = 0; i < Num*2; i++) {
				vData[i] = pData[i];
			}
			CloseRing(vData);
			vHoles.push_back(vData);
		}

		int GetVertexNum() {
			return vVertex.size() / 2;
		}

		int GetHoleVertexNum(int HoleIndex) {
			if (HoleIndex < 0 || HoleIndex >= vHoles.size()) {
				return -1;
			}
			return vHoles[HoleIndex].size() / 2;
		}

		double GetX(int k) {
			if (k * 2 < vVertex.size() && k >= 0) {
				return vVertex.at(k * 2);
			}
			return -1;
		}

		double GetX(int HoleIndex, int k) {
			if (HoleIndex < 0	|| HoleIndex >= vHoles.size()) {
				return -1;
			}
			if (k * 2 < vHoles[HoleIndex].size() && k >= 0) {
				return vHoles[HoleIndex].at(k * 2);
			}
			return -1;
		}

		double GetY(int k) {
			if (k * 2+1 < vVertex.size()) {
				return vVertex.at(k * 2+1);
			}
			return -1;
		}

		double GetY(int HoleIndex, int k) {
			if (HoleIndex < 0 || HoleIndex >= vHoles.size()) {
				return -1;
			}
			if (k * 2+1 < vHoles[HoleIndex].size() && k >= 0) {
				return vHoles[HoleIndex].at(k * 2+1);
			}
			return -1;
		}

		void GetCrossPoints(double Y, std::vector<double>& vLinePoints, std::set<double>& vXcoords) {
			LineSegment line(Point(Bound.MinX, Y), Point(Bound.MaxX, Y));
			Point CrossPoint;
			double x1, y1, x2, y2;
			for (int i = 0; i < vLinePoints.size() / 2 - 1; i++) {
				y1 = vLinePoints[i * 2 + 1];
				y2 = vLinePoints[i * 2 + 3];
				if ((y1 - Y) * (y2 - Y) > 0) {
					continue;
				}
				x1 = vLinePoints[i * 2];
				x2 = vLinePoints[i * 2 + 2];
				LineSegment theLine(Point(x1, y1), Point(x2, y2));
				if (line.getCrossPoint(theLine, CrossPoint)) {
					vXcoords.insert(CrossPoint.x);
				}
			}
		}

		bool GetCrossPoints(double Y, std::set<double>& vXcoords) {
			if (vVertex.size() < 6 || Y < Bound.MinY || Y > Bound.MaxY) {
				return false;
			}
			vXcoords.clear();
			GetCrossPoints(Y, vVertex, vXcoords);
			for (auto hole : vHoles) {
				GetCrossPoints(Y, hole, vXcoords);
			}
			return !vXcoords.empty();
		}

		bool GetSegments(double Y, std::vector<LineSegment>& vSegment) {
			std::set<double> vXcoords;
			if (!GetCrossPoints(Y, vXcoords)) {
				return false;
			}

			std::vector<double> vXPoints;
			vXPoints.insert(vXPoints.end(), vXcoords.begin(), vXcoords.end());
			
			vSegment.clear();
			bool lastState = false;
			for (int i = 0; i < vXPoints.size() - 1; i++) {
				double X = (vXPoints[i] + vXPoints[i + 1]) / 2;
				if (InRegion(X, Y)) {
					vSegment.push_back(LineSegment(Point(vXPoints[i], Y), Point(vXPoints[i+1], Y)));
					lastState = true;
				}
				else  {
					if (!lastState) {
						vSegment.push_back(LineSegment(Point(vXPoints[i], Y), Point(vXPoints[i], Y)));
					}
					lastState = false;
				}
			}

			return !vSegment.empty();
		}
	};
	
	class MultiPolygon : public Geometry {
	public:
		MultiPolygon() {
			GeoType = 2;
		};
		~MultiPolygon() {
		};

		std::vector<Polygon> vMPoly;
		//外包围框
		Range Bound;

		void Clear() {
			vMPoly.clear();
			Bound.Clear();
		}

		void AddPolygon(Polygon& poly) {
			vMPoly.push_back(poly);
			Bound.UpdateRange(poly.Bound);
		}

		//获取第k个多边形
		Polygon* GetPolygon(int k) {
			if (k >= 0 && k < vMPoly.size()) {
				return &(vMPoly[k]);
			}
			return nullptr;
		}

		//判断点在面内
		bool InRegion(double x, double y) {
			for (auto poly : vMPoly) {
				if (poly.InRegion(x, y)) {
					return true;
				}
			}
			return false;
		}
	};

	class ZipInfo {
	public:
		char ID[32];
		char Name[32];
		int Offset;
		int BufferSize;
	};

	class ZipRasterData {
	public: 
		ZipRasterData() {
			CellSize = 0.0001;
			BegX = 0;
			BegY = 0;
			rows = 0;
			cols = 0;
		};
		~ZipRasterData() {
		};

		double BegX;
		double BegY;
		double CellSize;
		int rows;
		int cols;

		std::vector<std::vector<int>> vBegAndEndCols;

		long GetBufferSize() {
			long Size = 0;
			Size += sizeof(BegX);
			Size += sizeof(BegY);
			Size += sizeof(CellSize);
			Size += sizeof(int);

			for (auto iter : vBegAndEndCols) {
				Size += sizeof(int);
				Size += iter.size() * sizeof(int);
			}
			return Size;
		}

		std::vector<unsigned char> vBuffer;

		unsigned char* Packet() {
			int size = GetBufferSize();
			if (size <= 0) {
				return nullptr;
			}
			vBuffer.resize(size, 0);

			unsigned char* pCur = &(vBuffer[0]);
			memcpy(pCur, &(BegX), sizeof(BegX));
			pCur += sizeof(BegX);
			memcpy(pCur, &(BegY), sizeof(BegY));
			pCur += sizeof(BegY);
			memcpy(pCur, &(CellSize), sizeof(CellSize));
			pCur += sizeof(CellSize);
			size = vBegAndEndCols.size();
			memcpy(pCur, &(size), sizeof(size));
			pCur += sizeof(size);
			for (auto iter : vBegAndEndCols) {
				size = iter.size();
				memcpy(pCur, &(size), sizeof(size));
				pCur += sizeof(size);
				if (size > 0) {
					memcpy(pCur, &(iter[0]), sizeof(int)*size);
					pCur += sizeof(int)*size;
				}
			}
			return pCur;
		}


		bool unPacket(void* pData)
		{
			if (pData == nullptr) {
				return false;
			}
			unsigned char* pCur = (unsigned char*)pData;
			memcpy(&(BegX), pCur, sizeof(BegX));
			pCur += sizeof(BegX);
			memcpy(&(BegY), pCur, sizeof(BegY));
			pCur += sizeof(BegY);
			memcpy(&(CellSize), pCur, sizeof(CellSize));
			pCur += sizeof(CellSize);
			int Num;

			memcpy(&(rows), pCur, sizeof(rows));
			pCur += sizeof(rows);
			int MaxCol = 0;
			vBegAndEndCols.resize(rows);
			for (int i = 0; i < rows; i++) {
				memcpy(&(cols), pCur, sizeof(cols));
				pCur += sizeof(cols);
				for (int j = 0; j < cols; j++) {
					memcpy(&(Num), pCur, sizeof(Num));
					pCur += sizeof(Num);
					vBegAndEndCols[i].push_back(Num);
					MaxCol = max(MaxCol, Num);
				}

			}
			cols = MaxCol;

			return true;

		}

		void* GetBuffer(long& Bufferlen) {
			if (vBuffer.empty()) {
				Packet();
			}
			Bufferlen = vBuffer.size();
			if (Bufferlen <= 0) {
				return nullptr;
			}
			return(&vBuffer[0]);
		}

		bool ZipGeometry(Geometry* pGeo, double GridSize) {
			bool bResult = false;
			if (pGeo == nullptr) {
				return false;
			}
			switch (pGeo->GeoType) {
			case 1: bResult = ZipPolygon((Polygon*)pGeo, GridSize); break;
			case 2: bResult = ZipMultiPolygon((MultiPolygon*)pGeo, GridSize); break;
			default: break;
			}
			return bResult;
		}

		bool ZipPolygon(Polygon* pPoly, double GridSize) {
			if (pPoly == nullptr) {
				return false;
			}
			BegX = pPoly->Bound.MinX;
			BegY = pPoly->Bound.MinY;
			CellSize = fabs(GridSize);
			if (CellSize <= 0) {
				CellSize = 0.001;
			}

			cols = floor(fabs(pPoly->Bound.MaxX - pPoly->Bound.MinX)/ CellSize + 0.5) + 1;
			rows = floor(fabs(pPoly->Bound.MaxY - pPoly->Bound.MinY) / CellSize + 0.5) + 1;


			vBegAndEndCols.clear();
			vBegAndEndCols.resize(rows);
			for (int i = 0; i < rows; i++) {
				std::set<double> vXcoords;
				pPoly->GetCrossPoints(BegY + i*CellSize, vXcoords);
				for (auto val : vXcoords) {
					vBegAndEndCols[i].push_back(floor(fabs(val - pPoly->Bound.MinY) / CellSize + 0.5));
				}
			}

			return true;
		}

		bool ZipMultiPolygon(MultiPolygon* pMPoly, double GridSize) {
			if (pMPoly == nullptr) {
				return false;
			}
			BegX = pMPoly->Bound.MinX;
			BegY = pMPoly->Bound.MinY;
			CellSize = fabs(GridSize);
			if (CellSize <= 0) {
				CellSize = 0.001;
			}

			cols = floor(fabs(pMPoly->Bound.MaxX - pMPoly->Bound.MinX) / CellSize + 0.5) + 1;
			rows = floor(fabs(pMPoly->Bound.MaxY - pMPoly->Bound.MinY) / CellSize + 0.5) + 1;

			vBegAndEndCols.clear();
			vBegAndEndCols.resize(rows);

			for (auto Poly : pMPoly->vMPoly) {
				for (int i = 0; i < rows; i++) {
					std::set<double> vXcoords;
					if (Poly.GetCrossPoints(BegY + i * CellSize, vXcoords)) {
						for (auto val : vXcoords) {
							vBegAndEndCols[i].push_back(floor(fabs(val - pMPoly->Bound.MinY) / CellSize + 0.5));
						}
					}

				}
			}
			return true;
		}						
	};

	class CVectorRasterData
	{
	public:
		CVectorRasterData() {
			m_FileHead = nullptr;
			m_FileData = nullptr;
			m_IsOpen = false;
			m_ObjNum = 0;
			sprintf(m_Version, "%s", "CVRD1.0");
			sprintf(m_PrjInfo, "%s", "CGCS2000");
		}
		~CVectorRasterData() {

		}
		std::map<std::string, ZipInfo> m_mapInfo;
		FILE* m_FileHead;
		FILE* m_FileData;
		bool m_IsOpen;

		char m_Version[16];
		char m_PrjInfo[1024];
		int m_ObjNum;

		bool Open(const char* FilePath, const char* FileName, const char* Mode) {
			char Path[1024];
			sprintf(Path, "%s\\%s.idx", FilePath, FileName);
			m_FileHead = fopen(Path, Mode);
			if (m_FileHead == nullptr) {
				m_FileHead = nullptr;
				return false;
			}
			sprintf(Path, "%s\\%s.dat", FilePath, FileName);
			m_FileData = fopen(Path, Mode);
			if (m_FileData == nullptr) {
				fclose(m_FileHead);
				m_FileHead = nullptr;
				m_FileData = nullptr;
				return false;
			}

			fseek(m_FileHead, 0, SEEK_END);
			long size = ftell(m_FileHead);
			fseek(m_FileHead, 0, SEEK_SET);
			if (size < sizeof(m_Version) + sizeof(m_PrjInfo) + sizeof(m_ObjNum)) {
				fwrite(m_Version, sizeof(m_Version), 1, m_FileHead);
				fwrite(m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
				fwrite(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
			}
			else {
				fread(m_Version, sizeof(m_Version), 1, m_FileHead);
				fread(m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
				fread(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
			}
			if (strcmp(m_Version, "CVRD1.0") != 0) {
				Close();
				return false;
			}

			for (int i = 0; i < m_ObjNum; i++) {
				ZipInfo Info;
				fread(&Info.ID, sizeof(Info.ID), 1, m_FileHead);
				fread(&Info.Name, sizeof(Info.Name), 1, m_FileHead);
				fread(&Info.Offset, sizeof(Info.Offset), 1, m_FileHead);
				fread(&Info.BufferSize, sizeof(Info.BufferSize), 1, m_FileHead);
				std::string strKey = Info.ID;
				strKey = to_upper(strKey);
				m_mapInfo[strKey] = Info;
			}
			m_IsOpen = true;
			return true;
		}

		bool Close() {
			if (m_IsOpen) {
				fclose(m_FileHead);
				fclose(m_FileData);
				m_IsOpen = false;
				return true;
			}
			return false;
		}

		bool Flush() {
			if (m_IsOpen) {
				fflush(m_FileHead);
				fflush(m_FileData);
				return true;
			}
			return false;
		}

		std::map<std::string, ZipInfo>* GetObjList() {
			return &m_mapInfo;
		}

		bool GetObjInfo(const char* ID, ZipRasterData& data) {
			std::string strKey = ID;
			strKey = to_upper(strKey);

			auto iter = m_mapInfo.find(strKey);
			if (iter == m_mapInfo.end()) {
				return false;
			}
			bool bResult = false;
			fseek(m_FileData, iter->second.Offset, SEEK_SET);
			unsigned char* pBuffer = new unsigned char[iter->second.BufferSize];
			fread(pBuffer, iter->second.BufferSize, 1, m_FileData);
			bResult = data.unPacket(pBuffer);
			delete[] pBuffer;
			return bResult;
		}

		bool WriteObjNum() {
			if (!m_IsOpen) {
				return false;
			}
			fseek(m_FileHead, sizeof(m_Version) + sizeof(m_PrjInfo), SEEK_SET);
			fwrite(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
			return true;
		}

		bool WriteVersion() {
			if (!m_IsOpen) {
				return false;
			}
			fseek(m_FileHead, 0, SEEK_SET);
			fwrite(&m_Version, sizeof(m_Version), 1, m_FileHead);
			return true;
		}

		bool WritePrjInfo() {
			if (!m_IsOpen) {
				return false;
			}
			fseek(m_FileHead, sizeof(m_Version), SEEK_SET);
			fwrite(&m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
			return true;
		}

		bool WriteObjInfo(const char* ID, const char* Name, Geometry* pGeo, double GridSize) {
			if (!m_IsOpen) {
				return false;
			}

			ZipInfo Info;
			memset(&Info, 0, sizeof(ZipInfo));
			memcpy(Info.ID, ID, min(strlen(ID), sizeof(Info.ID) - 1));
			memcpy(Info.Name, Name, min(strlen(Name), sizeof(Info.Name) - 1));
			Info.Offset = ftell(m_FileData);

			ZipRasterData data;
			data.ZipGeometry(pGeo, GridSize);			
			long BufferSize = 0;
			unsigned char* pBuffer = (unsigned char*)data.GetBuffer(BufferSize);
			fseek(m_FileData, 0, SEEK_END);
			fwrite(pBuffer, BufferSize, 1, m_FileData);

			Info.BufferSize = BufferSize;
			fseek(m_FileHead, 0, SEEK_END);

			fwrite(&Info.ID, sizeof(Info.ID), 1, m_FileHead);
			fwrite(&Info.Name, sizeof(Info.Name), 1, m_FileHead);
			fwrite(&Info.Offset, sizeof(Info.Offset), 1, m_FileHead);
			fwrite(&Info.BufferSize, sizeof(Info.BufferSize), 1, m_FileHead);

			m_ObjNum = m_ObjNum + 1;
			WriteObjNum();

			std::string strKey = Info.ID;
			strKey = to_upper(strKey);
			m_mapInfo[strKey] = Info;
		}

	};
}



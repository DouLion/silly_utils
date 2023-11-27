/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: zhao yan wei
 * @date: 2023/11/03
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>
#include "math.h"

#include <marco.h>

#define EPS 1E-10

namespace CVectorToRaster {

//    template<typename T1, typename T2>
//    static double min(T1 x, T2 y) {
//        if (x <= y) {
//            return x;
//        }
//        return y;
//    }
//
//    template<typename T1, typename T2>
//    static double max(T1 x, T2 y) {
//        if (x >= y) {
//            return x;
//        }
//        return y;
//    }
    class V2RTools {
    public:
        template<typename T1, typename T2>
        //点在面内
        static int PointRgn(T1 x, T1 y, int Numble, T2 *PointList) {
            int i, j;
            int c = 0;
            for (i = 0, j = Numble - 1; i < Numble; j = i++) {
                if ((((PointList[i * 2 + 1] <= y) && (y < PointList[j * 2 + 1]) ||
                      ((PointList[j * 2 + 1] <= y) && (y < PointList[i * 2 + 1]))) &&
                     (x < (PointList[j * 2] - PointList[i * 2]) * (y - PointList[i * 2 + 1]) /
                          (PointList[j * 2 + 1] - PointList[i * 2 + 1]) + PointList[i * 2]))) {
                    c = !c;
                }
            }
            return c;
        }

        template<typename T1, typename T2>
        static int PointInRgn(T1 x, T1 y, std::vector<T2> &PointList) {
            if (PointList.size() < 6) {
                return 0;
            }
            return PointRgn(x, y, PointList.size() / 2, &(PointList[0]));
        }
    };

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
        void UpdateRange(std::vector<T> &vVertex) {
            if (vVertex.empty()) {
                return;
            }
            UpdateRange(&(vVertex[0]), vVertex.size() / 2);
        }

        template<typename T>
        void UpdateRange(T *pData, int Num) {
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

        void UpdateRange(Range &Bound) {
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

        Point operator+(Point p) { return Point(x + p.x, y + p.y); }

        Point operator-(Point p) { return Point(x - p.x, y - p.y); }

        Point operator*(double a) { return Point(a * x, a * y); }

        Point operator/(double a) { return Point(x / a, y / a); }

        double abs() { return sqrt(norm()); }

        double norm() { return x * x + y * y; }

        bool operator<(const Point &p) const {
            return x != p.x ? x < p.x : y < p.y;
        }

        bool operator==(const Point &p) const {
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
        static bool getCrossPoint(LineSegment &s1, LineSegment &s2, Point &crossPoint) {
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

        bool getCrossPoint(LineSegment &Segment, Point &crossPoint) {
            return getCrossPoint(*this, Segment, crossPoint);
        }

        bool getCrossPoint(Point p1, Point p2, Point &crossPoint) {
            LineSegment Segment(p1, p2);
            return getCrossPoint(*this, Segment, crossPoint);
        }

        bool getCrossPoint(double x1, double y1, double x2, double y2, Point &crossPoint) {
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
        std::vector<std::vector<double>> vHoles;
        //外包围框
        Range Bound;

        Range GetBound() {
            return Bound;
        }

        //获取外边界
        std::vector<double> *GetVertex() {
            return &vVertex;
        }

        int HoleNumber() {
            return vHoles.size();
        }

        //获取第k个洞
        std::vector<double> *GetHole(int k) {

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
        void CloseRing(std::vector<T> &vVertex) {
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
            for (auto hole: vHoles) {
                if (V2RTools::PointInRgn(x, y, hole)) {
                    return false;
                }
            }
            return V2RTools::PointInRgn(x, y, vVertex);
        }

        //添加多边形
        template<typename T>
        void AddVertex(std::vector<T> &vRingVertex) {
            vVertex.clear();
            vVertex.insert(vVertex.end(), vRingVertex.begin(), vRingVertex.end());
            CloseRing(vVertex);
            Bound.UpdateRange(vVertex);
        }

        //添加多边形
        template<typename T>
        void AddVertex(T *pData, int Num) {
            if (nullptr == pData || Num <= 0) {
                return;
            }
            vVertex.clear();
            vVertex.resize(Num, 0);
            for (int i = 0; i < Num * 2; i++) {
                vVertex[i] = pData[i];
            }
            CloseRing(vVertex);
            Bound.UpdateRange(vVertex);
        }

        //添加洞
        template<typename T>
        void AddHole(std::vector<T> &vHoleVertex) {
            std::vector<double> vData;
            vData.insert(vData.end(), vHoleVertex.begin(), vHoleVertex.end());
            CloseRing(vData);
            vHoles.push_back(vData);
        }

        //添加洞
        template<typename T>
        void AddHole(T *pData, int Num) {
            if (nullptr == pData || Num <= 0) {
                return;
            }
            std::vector<double> vData;
            vData.resize(Num, 0);
            for (int i = 0; i < Num * 2; i++) {
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
            if (HoleIndex < 0 || HoleIndex >= vHoles.size()) {
                return -1;
            }
            if (k * 2 < vHoles[HoleIndex].size() && k >= 0) {
                return vHoles[HoleIndex].at(k * 2);
            }
            return -1;
        }

        double GetY(int k) {
            if (k * 2 + 1 < vVertex.size()) {
                return vVertex.at(k * 2 + 1);
            }
            return -1;
        }

        double GetY(int HoleIndex, int k) {
            if (HoleIndex < 0 || HoleIndex >= vHoles.size()) {
                return -1;
            }
            if (k * 2 + 1 < vHoles[HoleIndex].size() && k >= 0) {
                return vHoles[HoleIndex].at(k * 2 + 1);
            }
            return -1;
        }

        void GetCrossPoints(double Y, std::vector<double> &vLinePoints, std::set<double> &vXcoords);

        bool GetCrossPoints(double Y, std::set<double> &vXcoords);

        bool GetSegments(double Y, std::vector<LineSegment> &vSegment);
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

        void AddPolygon(Polygon &poly) {
            vMPoly.push_back(poly);
            Bound.UpdateRange(poly.Bound);
        }

        //获取第k个多边形
        Polygon *GetPolygon(int k) {
            if (k >= 0 && k < vMPoly.size()) {
                return &(vMPoly[k]);
            }
            return nullptr;
        }

        //判断点在面内
        bool InRegion(double x, double y) {
            for (auto poly: vMPoly) {
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

        long GetBufferSize();

        std::vector<unsigned char> vBuffer;

        unsigned char *Packet();

        bool unPacket(void *pData);


        void *GetBuffer(long &Bufferlen);

        bool ZipGeometry(Geometry *pGeo, double GridSize);

        bool ZipPolygon(Polygon *pPoly, double GridSize);

        bool ZipMultiPolygon(MultiPolygon *pMPoly, double GridSize);
    };

    class CVectorRasterData {
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
        FILE *m_FileHead;
        FILE *m_FileData;
        bool m_IsOpen;

        char m_Version[16];
        char m_PrjInfo[1024];
        int m_ObjNum;

        bool Open(const char *FilePath, const char *FileName, const char *Mode);

        bool Close();

        bool Flush();

        std::map<std::string, ZipInfo> *GetObjList() {
            return &m_mapInfo;
        }

        bool GetObjInfo(const char *ID, ZipRasterData &data);

        bool WriteObjNum();

        bool WriteVersion();

        bool WritePrjInfo();

        bool WriteObjInfo(const char *ID, const char *Name, Geometry *pGeo, double GridSize);

    };
}



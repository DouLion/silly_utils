#include "CPolygonToRaster.h"
#include <cstring>

static char toupper(char c)
{
    if (c >= 'a' && c <= 'z')  // 是小写字母，执行转换。
        c -= 'a' - 'A';        // 转成大写。
    return c;
}

static std::string to_upper(std::string &input)
{
    std::string OutPut = input;
    char *p = (char *)OutPut.c_str();
    while (*p)
    {
        *p++ = toupper(*p);
    }
    return OutPut;
}

///  DEMO

// #include <iostream>
//
//
//  void TestWrite() {
//	CVectorToRaster::CVectorRasterData dataSet;
//	if (!dataSet.Open(".", "test", "wb")) {
//		std::cout << "Failed to Open File\n";
//	}
//
//	CVectorToRaster::Polygon Poly;
//	std::vector<double> vVextex;
//	vVextex.push_back(0);
//	vVextex.push_back(0);
//
//	vVextex.push_back(0);
//	vVextex.push_back(100);
//
//	vVextex.push_back(100);
//	vVextex.push_back(0);
//
//	Poly.AddVertex(vVextex);
//
//	std::string strID = "Test001";
//	std::string strName = "三角形";
//
//	double GridSize = 0.5;
//	dataSet.WriteObjInfo(strID.c_str(), strName.c_str(), &Poly, GridSize);
//
//	dataSet.Close();
// }
//
//  void TestRead() {
//	CVectorToRaster::CVectorRasterData dataSet;
//	if (!dataSet.Open(".", "test", "rb")) {
//		std::cout << "Failed to Open File\n";
//	}
//
//	std::map<std::string, CVectorToRaster::ZipInfo>* mapInfo = dataSet.GetObjList();
//	for (auto iter : *mapInfo) {
//		CVectorToRaster::ZipRasterData data;
//		if (dataSet.GetObjInfo(iter.first.c_str(), data)) {
//			double X, BegY, EndY;
//			for (int i = 0; i < data.vBegAndEndCols.size(); i++) {
//				X = data.BegY + i * data.CellSize;  //纬度
//				for (int j = 0; j < data.vBegAndEndCols[i].size()/2; j++) {
//					BegY = data.BegX + data.vBegAndEndCols[i].at(j * 2) * data.CellSize; // 开始经度
//					EndY = data.BegX + data.vBegAndEndCols[i].at(j * 2+1) * data.CellSize; // 结束经度
//				}
//			}
//			std::cout << "Todo " << iter.first << std::endl;
//		}
//	}
//
//	dataSet.Close();
// }
//

long CVectorToRaster::ZipRasterData::GetBufferSize()
{
    long Size = 0;
    Size += sizeof(BegX);
    Size += sizeof(BegY);
    Size += sizeof(CellSize);
    Size += sizeof(int);

    for (auto iter : vBegAndEndCols)
    {
        Size += sizeof(int);
        Size += iter.size() * sizeof(int);
    }
    return Size;
}

unsigned char *CVectorToRaster::ZipRasterData::Packet()
{
    int size = GetBufferSize();
    if (size <= 0)
    {
        return nullptr;
    }
    vBuffer.resize(size, 0);

    unsigned char *pCur = &(vBuffer[0]);
    memcpy(pCur, &(BegX), sizeof(BegX));
    pCur += sizeof(BegX);
    memcpy(pCur, &(BegY), sizeof(BegY));
    pCur += sizeof(BegY);
    memcpy(pCur, &(CellSize), sizeof(CellSize));
    pCur += sizeof(CellSize);
    size = vBegAndEndCols.size();
    memcpy(pCur, &(size), sizeof(size));
    pCur += sizeof(size);
    for (auto iter : vBegAndEndCols)
    {
        size = iter.size();
        memcpy(pCur, &(size), sizeof(size));
        pCur += sizeof(size);
        if (size > 0)
        {
            memcpy(pCur, &(iter[0]), sizeof(int) * size);
            pCur += sizeof(int) * size;
        }
    }
    return pCur;
}

bool CVectorToRaster::ZipRasterData::unPacket(void *pData)
{
    if (pData == nullptr)
    {
        return false;
    }
    unsigned char *pCur = (unsigned char *)pData;
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
    for (int i = 0; i < rows; i++)
    {
        memcpy(&(cols), pCur, sizeof(cols));
        pCur += sizeof(cols);
        for (int j = 0; j < cols; j++)
        {
            memcpy(&(Num), pCur, sizeof(Num));
            pCur += sizeof(Num);
            vBegAndEndCols[i].push_back(Num);
            MaxCol = SU_MAX(MaxCol, Num);
        }
    }
    cols = MaxCol;

    return true;
}

void *CVectorToRaster::ZipRasterData::GetBuffer(long &Bufferlen)
{
    if (vBuffer.empty())
    {
        Packet();
    }
    Bufferlen = vBuffer.size();
    if (Bufferlen <= 0)
    {
        return nullptr;
    }
    return (&vBuffer[0]);
}

bool CVectorToRaster::ZipRasterData::ZipGeometry(CVectorToRaster::Geometry *pGeo, double GridSize)
{
    bool bResult = false;
    if (pGeo == nullptr)
    {
        return false;
    }
    switch (pGeo->GeoType)
    {
        case 1:
            bResult = ZipPolygon((Polygon *)pGeo, GridSize);
            break;
        case 2:
            bResult = ZipMultiPolygon((MultiPolygon *)pGeo, GridSize);
            break;
        default:
            break;
    }
    return bResult;
}

bool CVectorToRaster::ZipRasterData::ZipPolygon(CVectorToRaster::Polygon *pPoly, double GridSize)
{
    if (pPoly == nullptr)
    {
        return false;
    }
    BegX = pPoly->Bound.MinX;
    BegY = pPoly->Bound.MinY;
    CellSize = fabs(GridSize);
    if (CellSize <= 0)
    {
        CellSize = 0.001;
    }

    cols = floor(fabs(pPoly->Bound.MaxX - pPoly->Bound.MinX) / CellSize + 0.5) + 1;
    rows = floor(fabs(pPoly->Bound.MaxY - pPoly->Bound.MinY) / CellSize + 0.5) + 1;

    vBegAndEndCols.clear();
    vBegAndEndCols.resize(rows);
    for (int i = 0; i < rows; i++)
    {
        std::set<double> vXcoords;
        pPoly->GetCrossPoints(BegY + i * CellSize, vXcoords);
        for (auto val : vXcoords)
        {
            vBegAndEndCols[i].push_back(floor(fabs(val - pPoly->Bound.MinX) / CellSize + 0.5));
        }
    }

    return true;
}

bool CVectorToRaster::ZipRasterData::ZipMultiPolygon(CVectorToRaster::MultiPolygon *pMPoly, double GridSize)
{
    if (pMPoly == nullptr)
    {
        return false;
    }
    BegX = pMPoly->Bound.MinX;
    BegY = pMPoly->Bound.MinY;
    CellSize = fabs(GridSize);
    if (CellSize <= 0)
    {
        CellSize = 0.001;
    }

    cols = floor(fabs(pMPoly->Bound.MaxX - pMPoly->Bound.MinX) / CellSize + 0.5) + 1;
    rows = floor(fabs(pMPoly->Bound.MaxY - pMPoly->Bound.MinY) / CellSize + 0.5) + 1;

    vBegAndEndCols.clear();
    vBegAndEndCols.resize(rows);

    for (auto Poly : pMPoly->vMPoly)
    {
        for (int i = 0; i < rows; i++)
        {
            std::set<double> vXcoords;
            if (Poly.GetCrossPoints(BegY + i * CellSize, vXcoords))
            {
                for (auto val : vXcoords)
                {
                    vBegAndEndCols[i].push_back(floor(fabs(val - pMPoly->Bound.MinX) / CellSize + 0.5));
                }
            }
        }
    }
    return true;
}

bool CVectorToRaster::CVectorRasterData::Open(const char *FilePath, const char *FileName, const char *Mode)
{
    char Path[1024];
    sprintf(Path, "%s\\%s.idx", FilePath, FileName);
    m_FileHead = fopen(Path, Mode);
    if (m_FileHead == nullptr)
    {
        m_FileHead = nullptr;
        return false;
    }
    sprintf(Path, "%s\\%s.dat", FilePath, FileName);
    m_FileData = fopen(Path, Mode);
    if (m_FileData == nullptr)
    {
        fclose(m_FileHead);
        m_FileHead = nullptr;
        m_FileData = nullptr;
        return false;
    }

    fseek(m_FileHead, 0, SEEK_END);
    long size = ftell(m_FileHead);
    fseek(m_FileHead, 0, SEEK_SET);
    if (size < sizeof(m_Version) + sizeof(m_PrjInfo) + sizeof(m_ObjNum))
    {
        fwrite(m_Version, sizeof(m_Version), 1, m_FileHead);
        fwrite(m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
        fwrite(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
    }
    else
    {
        fread(m_Version, sizeof(m_Version), 1, m_FileHead);
        fread(m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
        fread(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
    }
    if (strcmp(m_Version, "CVRD1.0") != 0)
    {
        Close();
        return false;
    }

    for (int i = 0; i < m_ObjNum; i++)
    {
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

bool CVectorToRaster::CVectorRasterData::Close()
{
    if (m_IsOpen)
    {
        fclose(m_FileHead);
        fclose(m_FileData);
        m_IsOpen = false;
        return true;
    }
    return false;
}

bool CVectorToRaster::CVectorRasterData::Flush()
{
    if (m_IsOpen)
    {
        fflush(m_FileHead);
        fflush(m_FileData);
        return true;
    }
    return false;
}

bool CVectorToRaster::CVectorRasterData::GetObjInfo(const char *ID, CVectorToRaster::ZipRasterData &data)
{
    std::string strKey = ID;
    strKey = to_upper(strKey);

    auto iter = m_mapInfo.find(strKey);
    if (iter == m_mapInfo.end())
    {
        return false;
    }
    bool bResult = false;
    fseek(m_FileData, iter->second.Offset, SEEK_SET);
    unsigned char *pBuffer = new unsigned char[iter->second.BufferSize];
    fread(pBuffer, iter->second.BufferSize, 1, m_FileData);
    bResult = data.unPacket(pBuffer);
    delete[] pBuffer;
    return bResult;
}

bool CVectorToRaster::CVectorRasterData::WriteObjNum()
{
    if (!m_IsOpen)
    {
        return false;
    }
    fseek(m_FileHead, sizeof(m_Version) + sizeof(m_PrjInfo), SEEK_SET);
    fwrite(&m_ObjNum, sizeof(m_ObjNum), 1, m_FileHead);
    return true;
}

bool CVectorToRaster::CVectorRasterData::WriteVersion()
{
    if (!m_IsOpen)
    {
        return false;
    }
    fseek(m_FileHead, 0, SEEK_SET);
    fwrite(&m_Version, sizeof(m_Version), 1, m_FileHead);
    return true;
}

bool CVectorToRaster::CVectorRasterData::WritePrjInfo()
{
    if (!m_IsOpen)
    {
        return false;
    }
    fseek(m_FileHead, sizeof(m_Version), SEEK_SET);
    fwrite(&m_PrjInfo, sizeof(m_PrjInfo), 1, m_FileHead);
    return true;
}

bool CVectorToRaster::CVectorRasterData::WriteObjInfo(const char *ID, const char *Name, CVectorToRaster::Geometry *pGeo, double GridSize)
{
    if (!m_IsOpen)
    {
        return false;
    }

    ZipInfo Info;
    memset(&Info, 0, sizeof(ZipInfo));
    memcpy(Info.ID, ID, SU_MIN(strlen(ID), sizeof(Info.ID) - 1));
    memcpy(Info.Name, Name, SU_MIN(strlen(Name), sizeof(Info.Name) - 1));
    Info.Offset = ftell(m_FileData);

    ZipRasterData data;
    data.ZipGeometry(pGeo, GridSize);
    long BufferSize = 0;
    unsigned char *pBuffer = (unsigned char *)data.GetBuffer(BufferSize);
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
    return true;
}

bool CVectorToRaster::Polygon::GetSegments(double Y, std::vector<LineSegment> &vSegment)
{
    std::set<double> vXcoords;
    if (!GetCrossPoints(Y, vXcoords))
    {
        return false;
    }

    std::vector<double> vXPoints;
    vXPoints.insert(vXPoints.end(), vXcoords.begin(), vXcoords.end());

    vSegment.clear();
    bool lastState = false;
    for (int i = 0; i < vXPoints.size() - 1; i++)
    {
        double X = (vXPoints[i] + vXPoints[i + 1]) / 2;
        if (InRegion(X, Y))
        {
            vSegment.push_back(LineSegment(Point(vXPoints[i], Y), Point(vXPoints[i + 1], Y)));
            lastState = true;
        }
        else
        {
            if (!lastState)
            {
                vSegment.push_back(LineSegment(Point(vXPoints[i], Y), Point(vXPoints[i], Y)));
            }
            lastState = false;
        }
    }

    return !vSegment.empty();
}

bool CVectorToRaster::Polygon::GetCrossPoints(double Y, std::set<double> &vXcoords)
{
    if (vVertex.size() < 6 || Y < Bound.MinY || Y > Bound.MaxY)
    {
        return false;
    }
    vXcoords.clear();
    GetCrossPoints(Y, vVertex, vXcoords);
    for (auto hole : vHoles)
    {
        GetCrossPoints(Y, hole, vXcoords);
    }
    return !vXcoords.empty();
}

void CVectorToRaster::Polygon::GetCrossPoints(double Y, std::vector<double> &vLinePoints, std::set<double> &vXcoords)
{
    LineSegment line(Point(Bound.MinX, Y), Point(Bound.MaxX, Y));
    Point CrossPoint;
    double x1, y1, x2, y2;
    for (int i = 0; i < vLinePoints.size() / 2 - 1; i++)
    {
        y1 = vLinePoints[i * 2 + 1];
        y2 = vLinePoints[i * 2 + 3];
        if ((y1 - Y) * (y2 - Y) > 0)
        {
            continue;
        }
        x1 = vLinePoints[i * 2];
        x2 = vLinePoints[i * 2 + 2];
        LineSegment theLine(Point(x1, y1), Point(x2, y2));
        if (line.getCrossPoint(theLine, CrossPoint))
        {
            vXcoords.insert(CrossPoint.x);
        }
    }
}

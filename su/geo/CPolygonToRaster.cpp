//#include "CPolygonToRaster.h"
//#include <iostream>
//
//
//void TestWrite() {
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
//}
//
//void TestRead() {
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
//}
//


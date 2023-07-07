#include <iostream>
#include <otl/otl_tools.h>
#include <compress/libzippp.h>
#include <files/TFF_FileUtils.h>
#include <assert.h>
#include <bzlib.h>

using namespace libzippp;
int main()
{
	char* content = nullptr;
	int len = 0;
	FileUtils::ReadAll("E:/SL_CHINA_20230607_0000_1hourAcc.LatLon", &content, len);
	// char buffer[] = "hello world!1234567890hello world!1234567890678ty7t32148776416754327854763214354343231";   //原始数据
	// = ff.size();       //原始数据长度

	unsigned int destLen = len * 1.1 + 600;  //压缩后的数据长度，原始数组长度*1.01+600，可以多给一些，保持内存充足。
	char* dest = (char*)malloc(destLen);   //接收压缩过的数据
	memset(dest, 0, destLen);
	int err;        //错误码
//压缩
	err = BZ2_bzBuffToBuffCompress(
			dest,       //目标buffer（压缩后的数据）
			&destLen,   //目标buffer长度。函数内部压缩完之后，会设置为实际的长度。
			content,     //原始数据
			len,        //原始数据长度
			1,          //blockSize100k 字典大小，范围1-9，实际尺寸是100k*N
			0,          //verbosity 控制台输出压缩信息，范围0-4，数字越小输出越少
			30          //默认值30，库内部排序算法相关
	);

	std::cout << strlen(dest) << std::endl;
	return 0;

	return 0;
}
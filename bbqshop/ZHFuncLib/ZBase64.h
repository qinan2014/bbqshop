#ifndef Z_BASE_64_H  
#define Z_BASE_64_H  

#include <iostream>
class __declspec(dllexport) ZBase64
{
public:
	ZBase64(void);
	~ZBase64(void);

	/*编码
    DataByte
        [in]输入的数据长度,以字节为单位
    */
    std::string Encode(const char* Data,int DataByte);
    /*解码
    DataByte
        [in]输入的数据长度,以字节为单位
    OutByte
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算
        输出数据的长度
    */
    std::string Decode(const char* Data,int DataByte,int& OutByte);
};

#endif  
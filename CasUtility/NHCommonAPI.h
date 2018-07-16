/*
* Copyright (c) 
* All rights reserved.
*
* 文件名称：NHCommonAPI.h
* 文件标识：
* 描述：通用接口函数
*
* 版本：1.0
* 作者：Castle
* 日期：2010-09-03
*
*/

#pragma once



#define SAFEFREE(x) if (x){delete x;x = NULL;}
#define SAFEFREE_ARRAY(x) if (x){delete[] x;x = NULL;}
#include <xstring>
#include <vector>
using namespace std;


class CASUTILITY_EXPORT CNHCommonAPI
{
public:
	CNHCommonAPI(void);
	virtual ~CNHCommonAPI(void);

	// 禁用拷贝构造函数和拷贝赋值函数
private:
	CNHCommonAPI(const CNHCommonAPI &);
	CNHCommonAPI & operator =(const CNHCommonAPI &);

public:
	// 获取文件路径
	// (in)chFileFolder:文件夹
	// (in)chFileName:文件名
	// (out)chFilePath:文件路径
	// (in)bCreateFile:当文件不存在时，是否创建文件
	// (return)错误代码:
	// 0x00:正常，
	// 非0x00:异常(0x01:创建文件夹失败,0x02:文件不存在,0x03:创建文件失败)
	// 注意: 文件夹名和文件名前边不需要加"\\"
	// 例如: <L"Config"> 和 <L"CasInfo.ini">
	static DWORD GetFilePathEx(const wchar_t wchFileFolder[MAX_PATH], const wchar_t wchFileName[MAX_PATH], wchar_t wchFilePath[MAX_PATH], const bool bCreateFile = false);

	// 字符编码转换函数
	static wchar_t * ANSIToUnicode(const char *str);
	static char * UnicodeToANSI(const wchar_t *str);
	static wchar_t * UTF8ToUnicode(const char *str);
	static char * UnicodeToUTF8(const wchar_t *str);
	static char * ANSIToUTF8(const char *str);
	static char * UTF8ToANSI(const char *str);

	// 四舍五入
	static int round(const float f);
	static int round(const double d);
	//Castle判断是否数字
	static bool IsNum(const wchar_t *str);

	// 获取文件路径
	// 当前目录，CD=Current Directory
	// 上级目录，HLD=Higher Level Directory
	// 下级目录，LLD=Lower Level Directory
	// 参数：
	// pwchFileFolder:文件夹名
	// pwchFileName:文件名
	// pwchFilePath:文件路径
	// bCreateFile:当文件不存在时，是否创建文件
	// 返回：错误代码:
	// 0x00:正常，
	// 非0x00:异常(0x01:创建文件夹失败,0x02:文件不存在,0x03:创建文件失败)
	// 注意: 文件夹名和文件名前边不需要加"\\"
	// 例如: <L"Config"> 和 <L"CasInfo.ini">
	static DWORD GetCDFilePath(const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile = false);
	static DWORD GetHLDFilePath(const wchar_t *const pwchFileFolder, const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile = false);
	static DWORD GetLLDFilePath(const wchar_t *const pwchFileFolder, const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile = false);

	// 创建Unicode文件
	// 参数：
	// pwchFilePath:文件路径
	// 返回：错误代码:
	// 0x00:正常，
	// 非0x00:异常
	static DWORD CreateUnicodeFile(const wchar_t *const pwchFilePath);
	static void getMACAddress(char *const strMAC);

	static std::string base64_encode(BYTE const* buf, unsigned int bufLen);
	static std::vector<BYTE> base64_decode(std::string const& encoded_string);
};

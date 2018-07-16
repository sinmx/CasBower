#include "StdAfx.h"
#include "NHCommonAPI.h"

#include <cassert>
#include <io.h>
#include <cstdio>
#include <malloc.h>
#include <cstdlib>
#include <windows.h>
#include <iphlpapi.h>
#include <shlobj.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

CNHCommonAPI::CNHCommonAPI(void)
{
}

CNHCommonAPI::~CNHCommonAPI(void)
{
}

DWORD CNHCommonAPI::GetFilePathEx(const wchar_t wchFileFolder[MAX_PATH], const wchar_t wchFileName[MAX_PATH], wchar_t wchFilePath[MAX_PATH], const bool bCreateFile/* = false*/)
{
	assert(NULL != wchFileFolder);
	assert(NULL != wchFileName);
	assert(NULL != wchFilePath);

	// 获取exe(dll)文件绝对路径
	GetModuleFileName(NULL, wchFilePath, MAX_PATH);
	// 向上2层
	for(int i=(int)(wcslen(wchFilePath)-1); i>=0; i--)
	{
		if(L'\\' != wchFilePath[i])
		{
			wchFilePath[i] = L'\0';
		}
		else
		{
			wchFilePath[i] = L'\0';
			break;
		}
	}
	for(int i=(int)(wcslen(wchFilePath)-1); i>=0; i--)
	{
		if(L'\\' != wchFilePath[i])
		{
			wchFilePath[i] = L'\0';
		}
		else
		{
			wchFilePath[i] = L'\0';
			break;
		}
	}

	// 文件夹路径
	wcscat_s(wchFilePath, MAX_PATH, L"\\");
	wcscat_s(wchFilePath, MAX_PATH, wchFileFolder);

	// 判断文件夹是否存在
	if (_waccess(wchFilePath, 00))
	{
		// 文件夹不存在
		// 创建文件夹
		if (!CreateDirectory(wchFilePath, NULL))
		{ 
			// 创建文件夹失败,返回
			return 0x01;
		}
	}

	// 文件路径
	wcscat_s(wchFilePath, MAX_PATH, L"\\");
	wcscat_s(wchFilePath, MAX_PATH, wchFileName);

	// 判断文件是否存在
	if (_waccess(wchFilePath, 00))
	{
		// 文件不存在
		if (bCreateFile)
		{
			// 创建Unicode文件
			if (0x00 != CreateUnicodeFile(wchFilePath))
			{
				// 创建文件失败
				return 0x03;
			}
		}
		else
		{
			// 文件不存在
			return 0x02;
		}
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(wchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(wchFilePath, dwAttributes);

	return 0x00;
}

wchar_t * CNHCommonAPI::ANSIToUnicode(const char *str)
{
	// 支持(str==NULL)的情况

	int textlen(0);
	wchar_t *result(NULL);
	textlen = MultiByteToWideChar(CP_ACP, 0, str,-1, NULL, 0); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result, 0, (textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result,textlen); 
	return result;
}

char * CNHCommonAPI::UnicodeToANSI(const wchar_t *str)
{
	// 支持(str==NULL)的情况

	char* result(NULL);
	int textlen(0);
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char)*(textlen+1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

wchar_t * CNHCommonAPI::UTF8ToUnicode(const char *str)
{
	// 支持(str==NULL)的情况

	int textlen(0);
	wchar_t *result(NULL);
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result, 0, (textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen); 
	return result;
}

char * CNHCommonAPI::UnicodeToUTF8(const wchar_t *str)
{
	// 支持(str==NULL)的情况

	char *result(NULL);
	int textlen(0);
	textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char)*(textlen+1));
	WideCharToMultiByte(CP_UTF8, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

char * CNHCommonAPI::ANSIToUTF8(const char *str)
{
	// 支持(str==NULL)的情况

	char *result(NULL);

	wchar_t * pwchUnicode = ANSIToUnicode(str);

	result = UnicodeToUTF8(pwchUnicode);

	if (NULL != pwchUnicode)
	{
		free(pwchUnicode);
		pwchUnicode = NULL;
	}

	return result;
}

char * CNHCommonAPI::UTF8ToANSI(const char *str)
{
	// 支持(str==NULL)的情况

	char *result(NULL);

	wchar_t * pwchUnicode = UTF8ToUnicode(str);

	result = UnicodeToANSI(pwchUnicode);

	if (NULL != pwchUnicode)
	{
		free(pwchUnicode);
		pwchUnicode = NULL;
	}

	return result;
}

int CNHCommonAPI::round(const float f)
{
	int n(0);
	if (f > 0.0f)
	{
		// 正数
		n = static_cast<int>(f + 0.5f);
	}
	else if (f < 0.0f)
	{
		// 负数
		n = static_cast<int>(f - 0.5f);
	}
	else
	{
		// 零
		n = 0;
	}

	return n;
}

bool CNHCommonAPI::IsNum(const wchar_t *str)
{
	int nDos(0);
	for (int i=0; i<wcslen(str); i++)
	{
		if (str[i] == 0x2E)
		{
			nDos++;
		}
		else if (str[i]>=0x30 && str[i]<=0x39)
		{

		}
		else
		{
			return false;
		}
	}

	if (nDos > 1)
	{
		return false;
	}

	return true;
}

int CNHCommonAPI::round(const double d)
{
	int n(0);
	if (d > 0.0)
	{
		// 正数
		n = static_cast<int>(d + 0.5);
	}
	else if (d < 0.0)
	{
		// 负数
		n = static_cast<int>(d - 0.5);
	}
	else
	{
		// 零
		n = 0;
	}

	return n;
}

DWORD CNHCommonAPI::GetCDFilePath(const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile)
{
	assert(NULL != pwchFileName);
	assert(NULL != pwchFilePath);

	GetModuleFileNameW(NULL, pwchFilePath, MAX_PATH);
	// 向上1层
	wchar_t *pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileName);

	// 判断文件是否存在
	if (_waccess(pwchFilePath, 00))
	{
		// 文件不存在
		if (bCreateFile)
		{
			// 创建Unicode文件
			if (0x00 != CreateUnicodeFile(pwchFilePath))
			{
				// 创建文件失败
				return 0x03;
			}
		}
		else
		{
			// 文件不存在
			return 0x02;
		}
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(pwchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(pwchFilePath, dwAttributes);

	return 0x00;
}

DWORD CNHCommonAPI::GetHLDFilePath(const wchar_t *const pwchFileFolder, const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile)
{
	assert(NULL != pwchFileFolder);
	assert(NULL != pwchFileName);
	assert(NULL != pwchFilePath);

	GetModuleFileNameW(NULL, pwchFilePath, MAX_PATH);
	// 向上1层
	wchar_t *pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';
	// 向上1层
	pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';

	// 文件夹路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileFolder);
	wchar_t wchFolderPath[MAX_PATH];
	ZeroMemory(wchFolderPath, sizeof(wchFolderPath));
	wcsncpy_s(wchFolderPath, _countof(wchFolderPath), pwchFilePath, _TRUNCATE);

	// 判断文件夹是否存在
	if (_waccess(wchFolderPath, 00))
	{
		// 文件夹不存在
		// 创建文件夹
		if (!CreateDirectory(wchFolderPath, NULL))
		{ 
			// 创建文件夹失败,返回
			return 0x01;
		}
	}

	// 文件路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileName);

	// 判断文件是否存在
	if (_waccess(pwchFilePath, 00))
	{
		// 文件不存在
		if (bCreateFile)
		{
			// 创建Unicode文件
			if (0x00 != CreateUnicodeFile(pwchFilePath))
			{
				// 创建文件失败
				return 0x03;
			}
		}
		else
		{
			// 文件不存在
			return 0x02;
		}
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(pwchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(pwchFilePath, dwAttributes);

	return 0x00;
}

DWORD CNHCommonAPI::GetLLDFilePath(const wchar_t *const pwchFileFolder, const wchar_t *const pwchFileName, wchar_t *const pwchFilePath, const bool bCreateFile)
{
	assert(NULL != pwchFileFolder);
	assert(NULL != pwchFileName);
	assert(NULL != pwchFilePath);

	GetModuleFileNameW(NULL, pwchFilePath, MAX_PATH);
	// 向上1层
	wchar_t *pwchDest = wcsrchr(pwchFilePath, L'\\');
	*pwchDest = L'\0';

	// 文件夹路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileFolder);
	wchar_t wchFolderPath[MAX_PATH];
	ZeroMemory(wchFolderPath, sizeof(wchFolderPath));
	wcsncpy_s(wchFolderPath, _countof(wchFolderPath), pwchFilePath, _TRUNCATE);

	// 判断文件夹是否存在
	if (_waccess(wchFolderPath, 00))
	{
		// 文件夹不存在
		// 创建文件夹
		if (!CreateDirectory(wchFolderPath, NULL))
		{ 
			// 创建文件夹失败,返回
			return 0x01;
		}
	}

	// 文件路径
	wcscat_s(pwchFilePath, MAX_PATH, L"\\");
	wcscat_s(pwchFilePath, MAX_PATH, pwchFileName);

	// 判断文件是否存在
	if (_waccess(pwchFilePath, 00))
	{
		// 文件不存在
		if (bCreateFile)
		{
			// 创建Unicode文件
			if (0x00 != CreateUnicodeFile(pwchFilePath))
			{
				// 创建文件失败
				return 0x03;
			}
		}
		else
		{
			// 文件不存在
			return 0x02;
		}
	}

	// 去除文件只读属性
	DWORD dwAttributes(0);
	dwAttributes = ::GetFileAttributes(pwchFilePath);
	dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
	::SetFileAttributes(pwchFilePath, dwAttributes);

	return 0x00;
}

void CNHCommonAPI::getMACAddress(char *const strMAC)
{
	IP_ADAPTER_INFO adapter[5];  //Maximum 5 adapters
	DWORD buflen=sizeof(adapter);
	DWORD status=GetAdaptersInfo(adapter,&buflen);
	BYTE s[8];
	if(status == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO painfo=adapter;
		memcpy(s, painfo->Address, 6);
		sprintf_s(strMAC, MAX_PATH, "%02X%02X%02X%02X%02X%02X",s[0],s[1],s[2],s[3],s[4],s[5]);
	} 
}

DWORD CNHCommonAPI::CreateUnicodeFile(const wchar_t *const pwchFilePath)
{
	DWORD dwReturn(0x00);
	try
	{
		// 创建Unicode文件
		FILE *fp(NULL);
		_wfopen_s(&fp, pwchFilePath, L"r");
		if (fp == NULL)
		{
			if (0 == _wfopen_s(&fp, pwchFilePath, L"w+b"))
			{
				wchar_t wchUnicode(wchar_t(0XFEFF));
				fputwc(wchUnicode, fp);
			}
			else
			{
				throw;
			}
		}
		fclose(fp);

		dwReturn = 0x00;
	}
	catch (...)
	{
		// 创建文件失败

		dwReturn = 0x01;
	}

	return dwReturn;
}

//单字符转unicode宽字符
wstring MutiToUnicode(const string& str, int nType)
{
	int  unicodeLen = ::MultiByteToWideChar(nType, 0, str.c_str(), -1, NULL, 0);
	wchar_t * pUnicode = new  wchar_t[unicodeLen+1]();
	::MultiByteToWideChar(nType, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring rt = pUnicode;
	SAFEFREE_ARRAY(pUnicode);
	return rt;
}
//unicode宽字符转单字符
string UnicodeToMuti(const wstring& wstr, int nType)
{
	int nLen = WideCharToMultiByte(nType, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char * pMuti = new  char[nLen+1]();
	WideCharToMultiByte(nType, 0, wstr.c_str(), -1, pMuti, nLen, NULL, NULL);
	string strText = pMuti;
	SAFEFREE_ARRAY(pMuti);
	return strText;
}

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(BYTE c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CNHCommonAPI::base64_encode(BYTE const* buf, unsigned int bufLen) {
	std::string ret;
	int i = 0;
	int j = 0;
	BYTE char_array_3[3];
	BYTE char_array_4[4];

	while (bufLen--) {
		char_array_3[i++] = *(buf++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

std::vector<BYTE> CNHCommonAPI::base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	BYTE char_array_4[4], char_array_3[3];
	std::vector<BYTE> ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}
#include "StdAfx.h"
#include "UsbDev.h"


CUsbDev::CUsbDev(void)
{
}


CUsbDev::~CUsbDev(void)
{
}


DWORD CUsbDev::GetUsbID(wchar_t* const pwchUSBSN)
{
	DWORD dw(0x00);
	wchar_t wchFilePath[MAX_PATH]={0};

	// 获取exe(dll)文件绝对路径
	GetModuleFileName(NULL, wchFilePath, MAX_PATH);

	if(!::IsCharAlphaW(wchFilePath[0]))
	{
		//驱动器参数无效
		dw = 0x01;
	}
	else
	{
		/**********获取驱动器名字、序列号和文件系统部分**********/
		wchar_t wchRootPathName[50] = {0};
		_snwprintf_s(wchRootPathName, _countof(wchRootPathName), _TRUNCATE, L"%c:\\", wchFilePath[0]);
		LPCTSTR lpRootPathName = wchRootPathName;
		LPTSTR lpVolumeNameBuffer = new wchar_t[_MAX_FNAME];
		DWORD nVolumeNameSize = _MAX_FNAME;
		DWORD nVolumeSerialNumber = 0;//便于驱动器无效时做判断
		DWORD nMaximumComponentLength;
		DWORD nFileSystemFlags;
		LPTSTR lpFileSystemNameBuffer = new wchar_t[20];//文件系统(NTFS,FAT)多大有定义好的宏吗
		DWORD nFileSystemNameSize = 20;
		GetVolumeInformationW(
			lpRootPathName,
			lpVolumeNameBuffer,
			nVolumeNameSize,
			&nVolumeSerialNumber, 
			&nMaximumComponentLength,
			&nFileSystemFlags,
			lpFileSystemNameBuffer,
			nFileSystemNameSize);

		delete[] lpVolumeNameBuffer;
		delete[] lpFileSystemNameBuffer;

		// 可移动磁盘
		if (DRIVE_REMOVABLE == GetDriveTypeW(lpRootPathName))
		{
			if (nVolumeSerialNumber != 0)
			{
				_snwprintf_s(pwchUSBSN, 50, _TRUNCATE, L"%X", nVolumeSerialNumber);
			}
			else
			{
				dw = 0x02;
			}
		}
		else
		{
			dw = 0x03;
		}
	}

	return dw;

}
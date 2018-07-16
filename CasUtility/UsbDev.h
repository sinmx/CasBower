#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <xstring>
#include <WinIoCtl.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#pragma comment( lib, "setupapi.lib" )

#define GDI_VOLUMENAME	 0
#define GDI_VOLUMESERIALNUMBER	1
#define GDI_VOLUMEFILESYSTEM	2
#define GDI_VOLUMETYPE	 3
#define GDI_VOLUMESIZE	 4
#define GDI_VOLUMEFREESIZE	 5



class CUsbDev
{
public:
	CUsbDev(void);
	~CUsbDev(void);
	
	static DWORD GetUsbID(wchar_t* const pwchUSBSN);
};


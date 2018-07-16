#pragma once

// 函数执行结果
#define SEN_OPEN_SUCCEED						0x00  // 打开串口成功
#define SEN_OPEN_FAIL							0x01  // 打开串口失败
#define SEN_CLOSE_SUCCEED						0x00  // 关闭串口成功
#define SEN_CLOSE_FAIL_WIN32_API				0x01  // 关闭串口失败：执行win32 API(CloseHandle)失败
#define SEN_CLOSE_FAIL_NO_OPEN_PORT				0x02  // 关闭串口失败：没有可以关闭的串口
#define SEN_WR_SUCCEED							0x00  // 写读成功
#define SEN_WR_PORT_NOT_OPEN					0x01  // 写读失败：串口未打开
#define SEN_WR_READ_SPECIFIED_DATA_FAIL			0x02  // 写读失败：读指定数据失败
#define SEN_WR_WRITE_SPECIFIED_DATA_FAIL		0x03  // 写读失败：写指定数据失败
#define SEN_WR_EQUIP_RETURE_0X15				0x04  // 写读失败：仪器返回错误代码（0x15）
#define SEN_WR_CHECK_SUM_FAIL					0x05  // 写读失败：读出数据校验和失败
#define SEN_WR_UNKNOWN_ERROR					0xFF  // 写读失败：未知错误

#define PLC_PROT_ERROR	0
//打开阀门==闭合
#define PLC_PROT_BIHE	1
//关闭阀门==断开
#define PLC_PROT_DUAN	2
#define PLC_SET_OK		3
#define PLC_SET_WRONG	4

#define PLC_OPEN_ERROR	-1

#	ifdef BUILD_CASCOM
#	define CASCOM_EXPORTS __declspec(dllexport)
#else
#	define CASCOM_EXPORTS __declspec(dllimport)
#endif

class CASCOM_EXPORTS CasT
{
public:
	CasT(void);
	~CasT(void);

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 串口操作start
	// 打开串口
	// 打开串口前，自动识别是否已经存在已经打开的串口资源，如果有，在打开之前，先关闭以前打开的串口资源
	// in bPort：串口号(1~255)
	// in nBaudRate：波特率 (110、300、600、1200、2400、4800、9600、14400、19200、38400、56000、57600、115200、128000、256000)
	// 返回打开结果：
	// ANA_OPEN_SUCCEED
	// ANA_OPEN_FAIL
	DWORD Open(const BYTE bPort, const int nBaudRate = CBR_9600, const int nCheck=NOPARITY);

	// 关闭串口
	// 自动识别是否有可以不关闭的串口资源
	// 返回关闭结果：
	// ANA_CLOSE_SUCCEED
	// ANA_CLOSE_FAIL_WIN32API
	// ANA_CLOSE_FAIL_NOOPENPORT
	DWORD Close(void);

	// 判断串口是或打开
	bool IsOpen(void) const;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 串口操作end

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 仪器操作start 
	// 1取实时数据
	// (out)pfResult,结果
	// 返回函数执行结果：
	// SEN_WR_SUCCEED
	// SEN_WR_PORT_NOT_OPEN
	// SEN_WR_READ_SPECIFIED_DATA_FAIL
	// SEN_WR_EQUIP_RETURE_0X15
	// SEN_WR_CHECK_SUM_FAIL
	//粉剂
	DWORD GetData(const short sType, float *const pfResult = NULL);
	//助剂
	DWORD GetData(float *const pfResult = NULL);
	// 清零
	DWORD SetZero(void);
	// 设置SN数据
	// SEN_WR_SUCCEED
	// SEN_WR_PORT_NOT_OPEN
	// SEN_WR_READ_SPECIFIED_DATA_FAIL
	// SEN_WR_EQUIP_RETURE_0X15
	// SEN_WR_CHECK_SUM_FAIL
	DWORD SetData(const float fData);
	void GetSum(int *pnData, int nLength, char *chSendBuf); //校验和计算和发送数组的计算
	int CheckOpen(char *pchPort);
	bool ForceBit(char *pchPort, bool bOn);

	bool GetAllCyStatus(BYTE *const pbStatus, const DWORD dwReadLength);

	//Flow*************
	//读瞬时流量
	bool GetFlowRTRateData(float *const pfData);	
	//读累积流量
	bool GetFlowAllRateData(float *const pfData);
	//读报警状态
	bool GetFlowAlarm(UINT *const pType);

	bool PLCSet(const char *pchPort, bool bOn);

	int PLCRead(const char *pchPort);


	//读瞬时流速
	bool GetFlowRTSpeedData(float *const pfData);
	//读瞬时流量单位
	bool GetFlowRTRateUnit(UINT *const pType);
	//读总量流量单位
	bool GetFlowAllRateUnit(UINT *const pType);

protected:
	// 写读串口数据
	// 写入相应的命令，读取对应的返回数据
	// 并根据需要，分别对写入和读取的数据进行校验和验证
	// (in)pbWriteBuffer：要写入的数据内容指针，可以为空指针，如果为空指针，则不进行写数据操作
	// (in)dwWriteLength：要写入的数据长度（字节数），当pbWriteBuffer为空指针时，这个参数被忽略
	// (out)pbReadBuffer：要读取的数据内容指针，不能为空指针
	// (in)dwReadLength：要读取的数据长度（字节数）
	// (in)bNeedWriteChkSum：true，需要写校验，false：不需要，当pbWriteBuffer为空指针时，这个参数被忽略
	// (in)bNeedReadChkSum：true，需要读校验，false：不需要
	// (in)bPurgeComm,是否重置串口操作,true,重置串口操作(执行PurgeComm函数),false,不重置串口操作(不执行PurgeComm函数)
	// 返回，函数执行结果：
	// ANA_WR_SUCCEED
	// ANA_WR_PORT_NOT_OPEN
	// ANA_WR_READ_SPECIFIED_DATA_FAIL
	// ANA_WR_EQUIP_RETURE_0X15
	// ANA_WR_CHECK_SUM_FAIL
	DWORD WriteAndReadData(BYTE *const pbWriteBuffer, 
		const DWORD dwWriteLength, 
		BYTE *const pbReadBuffer, 
		const DWORD dwReadLength, 
		const bool bNeedWriteChkSum=true, 
		const bool bNeedReadChkSum=true, 
		const bool bPurgeComm=true);

	// 写串口数据
	// 写入相应的命令
	// (in)pbWriteBuffer：要写入的数据内容指针，可以为空指针，如果为空指针，则不进行写数据操作
	// (in)dwWriteLength：要写入的数据长度（字节数），当pbWriteBuffer为空指针时，这个参数被忽略
	// (in)bNeedWriteChkSum：true，需要写校验，false：不需要，当pbWriteBuffer为空指针时，这个参数被忽略
	// (in)bPurgeComm,是否重置串口操作,true,重置串口操作(执行PurgeComm函数),false,不重置串口操作(不执行PurgeComm函数)
	// 返回，函数执行结果：
	// SEN_WR_SUCCEED
	// SEN_WR_PORT_NOT_OPEN
	// SEN_WR_WRITE_SPECIFIED_DATA_FAIL
	DWORD WriteData(BYTE *const pbWriteBuffer,
		const DWORD dwWriteLength,
		const bool bNeedWriteChkSum = false,
		const bool bPurgeComm = true);

	// 读串口数据
	// 读取对应的返回数据
	// (out)pbReadBuffer：要读取的数据内容指针，不能为空指针
	// (in)dwReadLength：要读取的数据长度（字节数）
	// (in)bDecryption：true，需要解密，false：不需要
	// (in)bNeedReadChkSum：true，需要读校验，false：不需要
	// 返回，函数执行结果：
	// SEN_WR_SUCCEED
	// SEN_WR_PORT_NOT_OPEN
	// SEN_WR_READ_SPECIFIED_DATA_FAIL
	// SEN_WR_CHECK_SUM_FAIL
	DWORD ReadData(BYTE *const pbReadBuffer,
		const DWORD dwReadLength,
		const bool bDecryption = true,
		const bool bNeedReadChkSum = true);

	// 计算前n位的校验和
	// 相加，然后取最低字节位
	// (in)p：要进行校验的数据内容指针
	// (in)un：要进行校验的数据长度
	// 返回校验和
	BYTE CheckSum(const BYTE *const p, const UINT un);

	//Flow******************
	//CRC
	UINT FlowCheck(BYTE * puchMsg, UINT usDataLen);
	float Data2Float(BYTE FloatByte1, BYTE FloatByte2, BYTE FloatByte3, BYTE FloatByte4);

	bool GetFlowAllRateData1(long *const plData);
	bool GetFlowAllRateData2(float *const pfData);
protected:
	// 串口句柄
	HANDLE m_hComm;

private:
	//客户信息临界保护量
	CRITICAL_SECTION m_sCriticalSection;

};


#ifndef CASDEF_H
#define CASDEF_H

// 宏定义
#ifndef ERROR_MAX
#define ERROR_MAX 1024 // 出错最大输出字符数
#endif

#ifndef SQLMAX
#define SQLMAX 1024 // sql 语句最大输入字符数
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 键盘映射消息定义begin
#define WM_HOTKEY_VK_F2				WM_USER+0x0701
#define WM_HOTKEY_VK_F3				WM_USER+0x0702
#define WM_HOTKEY_VK_F4				WM_USER+0x0703
#define WM_HOTKEY_VK_F5				WM_USER+0x0704
#define WM_HOTKEY_VK_F6				WM_USER+0x0705
#define WM_HOTKEY_VK_F7				WM_USER+0x0706
#define WM_HOTKEY_VK_F8				WM_USER+0x0707
#define WM_HOTKEY_VK_F9				WM_USER+0x0708
#define WM_HOTKEY_VK_F10			WM_USER+0x0709
#define WM_HOTKEY_VK_F11			WM_USER+0x070A
#define WM_HOTKEY_VK_F12			WM_USER+0x070B
#define WM_HOTKEY_VK_OEM_PLUS		WM_USER+0x070C
#define WM_HOTKEY_VK_OEM_MINUS		WM_USER+0x070D
#define WM_HOTKEY_VK_OEM_5			WM_USER+0x070E
#define WM_HOTKEY_VK_OEM_3			WM_USER+0x070F     //For the US standard keyboard, the '`~' key
#define WM_HOTKEY_VK_ALT_S			WM_USER+0x0710
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 键盘映射消息定义end


// 定义角色类型
enum NHRoleType
{
	ADMIN = 0,		// 管理员	0
	OPERATOR,		// 操作员	1
	BILLER,			// 开单员	2
	SHROFF,			// 仓库员	3
	ASSESSOR,		// 称料员	4
};

//////////////////////////////////////////////////////////////////
#define NH_INT_TO_WCHART 12
#define NH_TIME_TO_WCHART 30
#define NH_BOOL_TO_WCHART 2
#define NH_FLOAT_TO_WCHART 20
//////////////////////////////////////////////////////////////////

// 用户信息<UserInfo>
typedef struct UseInfo
{
	wchar_t wchAutoID[NH_INT_TO_WCHART];				// 自动编号
	wchar_t wchName[50];								// 用户名
	wchar_t wchPassword[50];							// 密码
	wchar_t wchLicenseSn[50];						// 许可证编号
	wchar_t wchLicenseStartDate[NH_TIME_TO_WCHART];	// 许可证开始日期
	wchar_t wchLicenseEndDate[NH_TIME_TO_WCHART];	// 许可证结束日期
	wchar_t wchRole[NH_INT_TO_WCHART];				// 角色
	wchar_t wchRemark[1024];							// 备注
	wchar_t wchReserved1[50];						// 保留字段,以后备用
	wchar_t wchReserved2[50];						// 保留字段,以后备用
	wchar_t wchReserved3[50];						// 保留字段,以后备用

	UseInfo(){ZeroMemory(this, sizeof(UseInfo));}
}USERINFO,*PUSERINFO;

typedef struct CompanyInfo
{
	wchar_t wchName[1024];								// 用户名
	wchar_t wchEName[1024];								// 用户名
	wchar_t wchAddress[1024];							
	wchar_t wchTel[1024];								
	wchar_t wchFax[1024];								
	wchar_t wchEmail[1024];															
	wchar_t wchRemark[1024];							// 备注
	wchar_t wchReserved1[50];						// 保留字段,以后备用
	wchar_t wchReserved2[50];						// 保留字段,以后备用
	wchar_t wchReserved3[50];						// 保留字段,以后备用

	CompanyInfo(){ZeroMemory(this, sizeof(CompanyInfo));}
}COMPANYINFO,*PCOMPANYINFO;
#endif
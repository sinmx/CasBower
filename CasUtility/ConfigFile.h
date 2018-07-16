#ifndef _CCONFIGFILE_H_INC_
#define _CCONFIGFILE_H_INC_

#pragma once

#if defined(__cplusplus) && !defined(CINTERFACE)
#include <objbase.h>
#ifndef PURE
#	define PURE =0
#endif
#endif

typedef unsigned int	UINT;
typedef unsigned long   DWORD;
typedef int             BOOL;
typedef DWORD ULONG;
typedef char CHAR;

#ifndef _WCHAR_DEFINED
#define _WCHAR_DEFINED
typedef wchar_t WCHAR;
#endif // !_WCHAR_DEFINED
typedef const CHAR * LPCSTR;
typedef /* [string] */ const WCHAR *LPCWSTR;

#ifndef __ISectionA_FWD_DEFINED__
#define __ISectionA_FWD_DEFINED__
typedef interface ISectionA ISectionA;
typedef interface IConfigFileA IConfigFileA;
typedef interface ISectionFileA ISectionFileA;
#endif 	/* __ISectionA_FWD_DEFINED__ */

#if defined(__cplusplus)
struct ISectionA{
	virtual DWORD STDMETHODCALLTYPE GetName(CHAR *lpName,DWORD nSize) PURE;
	virtual DWORD STDMETHODCALLTYPE GetItemCount() PURE;
	virtual BOOL  STDMETHODCALLTYPE GetItemStringA(int iItem,CHAR *pKey,int nKeySize,CHAR *pValue,int nValueSize) PURE;
	virtual DWORD STDMETHODCALLTYPE GetString(LPCSTR pszKeyName,LPCSTR lpDefault,CHAR *lpReturnedString,DWORD nSize) PURE;
	virtual	LPCSTR STDMETHODCALLTYPE GetStringPtrA(LPCSTR pszKeyName,DWORD *pnLength) PURE;
	virtual	LPCWSTR STDMETHODCALLTYPE GetStringPtrW(LPCWSTR pszKeyName,DWORD *pnLength) PURE;
	virtual UINT STDMETHODCALLTYPE GetInt(LPCSTR lpKeyName,int nDefault) PURE;
	virtual	BOOL STDMETHODCALLTYPE GetBool(LPCSTR pszKeyName,BOOL bDefault) PURE;
	virtual BOOL STDMETHODCALLTYPE WriteString(LPCSTR lpKeyName,LPCSTR pszValue) PURE;
	virtual BOOL STDMETHODCALLTYPE WriteInt(LPCSTR lpKeyName,int nValue) PURE;
	virtual BOOL STDMETHODCALLTYPE WriteBool(LPCSTR lpKeyName,BOOL bValue) PURE; 
};

struct ISectionFileA:public ISectionA {
	virtual BOOL	STDMETHODCALLTYPE Flush() PURE;
	virtual ULONG	STDMETHODCALLTYPE AddRef() PURE;
	virtual ULONG	STDMETHODCALLTYPE Release() PURE;
};

struct IConfigFileA {
	virtual DWORD   STDMETHODCALLTYPE GetSectionCount() PURE;
	virtual	ISectionA *	STDMETHODCALLTYPE GetSection(LPCSTR pszSection) PURE;
	virtual	ISectionA *	STDMETHODCALLTYPE GetSectionByIndex(int iIndex) PURE;
	virtual DWORD	STDMETHODCALLTYPE GetString(LPCSTR pszSection,LPCSTR lpKeyName,LPCSTR lpDefault,CHAR *lpReturnedString,DWORD nSize) PURE;
	virtual int		STDMETHODCALLTYPE GetInt(LPCSTR pszSection,LPCSTR lpKeyName,int lpDefault) PURE;
	virtual	BOOL	STDMETHODCALLTYPE GetBool(LPCSTR pszSection,LPCSTR pszKeyName,BOOL bDefault) PURE;
	virtual BOOL	STDMETHODCALLTYPE WriteString(LPCSTR pszSection,LPCSTR lpKeyName,LPCSTR pszValue) PURE;
	virtual BOOL	STDMETHODCALLTYPE WriteInt(LPCSTR pszSection,LPCSTR lpKeyName,int nValue) PURE;
	virtual BOOL	STDMETHODCALLTYPE WriteBool(LPCSTR pszSection,LPCSTR lpKeyName,BOOL bValue) PURE;
	virtual BOOL	STDMETHODCALLTYPE Flush() PURE;
	virtual ULONG	STDMETHODCALLTYPE AddRef() PURE;
	virtual ULONG	STDMETHODCALLTYPE Release() PURE;
};

#else 
/* C style interface */

typedef struct ISectionAVtbl
{
	DWORD (STDMETHODCALLTYPE *GetName)(ISectionA * _pThis,CHAR *lpName,DWORD nSize);
	DWORD (STDMETHODCALLTYPE *GetItemCount) (ISectionA * _pThis) ;
	BOOL  (STDMETHODCALLTYPE *GetItemStringA) (ISectionA * _pThis,int iItem,CHAR *pKey,int nKeySize,CHAR *pValue,int nValueSize) ;
	DWORD (STDMETHODCALLTYPE *GetString) (ISectionA * _pThis,LPCSTR pszKeyName,LPCSTR lpDefault,CHAR *lpReturnedString,DWORD nSize) ;
	LPCSTR (STDMETHODCALLTYPE *GetStringPtrA) (ISectionA * _pThis,LPCSTR pszKeyName,DWORD *pnLength) ;
	LPCWSTR (STDMETHODCALLTYPE *GetStringPtrW) (ISectionA * _pThis,LPCWSTR pszKeyName,DWORD *pnLength) ;
	UINT (STDMETHODCALLTYPE *GetInt) (ISectionA * _pThis,LPCSTR lpKeyName,int nDefault) ;
	BOOL (STDMETHODCALLTYPE *GetBool) (ISectionA * _pThis,LPCSTR pszKeyName,BOOL bDefault) ;
	BOOL (STDMETHODCALLTYPE *WriteString) (ISectionA * _pThis,LPCSTR lpKeyName,LPCSTR pszValue) ;
	BOOL (STDMETHODCALLTYPE *WriteInt) (ISectionA * _pThis,LPCSTR lpKeyName,int nValue) ;
	BOOL (STDMETHODCALLTYPE *WriteBool) (ISectionA * _pThis,LPCSTR lpKeyName,BOOL bValue) ; 
} ISectionAVtbl;

interface ISectionA
{
	CONST_VTBL struct ISectionAVtbl *lpVtbl;
};

#define ISectionA_GetName(This,lpName,nSize) \
	(This)->lpVtbl->GetName(This,lpName,nSize)
#define ISectionA_GetItemCount(This) \
	(This)->lpVtbl->GetItemCount(This)
#define ISectionA_GetItemStringA(This,iItem,pKey,nKeySize,pValue,nValueSize) \
	(This)->lpVtbl->GetItemStringA(This,iItem,pKey,nKeySize,pValue,nValueSize)
#define ISectionA_GetString(This,pszKeyName,lpDefault,lpReturnedString,nSize) \
	(This)->lpVtbl->GetString(This,pszKeyName,lpDefault,lpReturnedString,nSize)
#define ISectionA_GetStringPtrA(This,pszKeyName,pnLength) \
	(This)->lpVtbl->GetStringPtrA(This,pszKeyName,pnLength)
#define ISectionA_GetStringPtrW(This,pszKeyName,pnLength) \
	(This)->lpVtbl->GetStringPtrW(This,pszKeyName,pnLength)
#define ISectionA_GetInt(This,lpKeyName,nDefault) \
	(This)->lpVtbl->GetInt(This,lpKeyName,nDefault)
#define ISectionA_GetBool(This,pszKeyName,bDefault) \
	(This)->lpVtbl->GetBool(This,pszKeyName,bDefault)
#define ISectionA_WriteString(This,lpKeyName,pszValue) \
	(This)->lpVtbl->WriteString(This,lpKeyName,pszValue)
#define ISectionA_WriteInt(This,lpKeyName,nValue) \
	(This)->lpVtbl->WriteInt(This,lpKeyName,nValue)
#define ISectionA_WriteBool(This,lpKeyName,bValue) \
	(This)->lpVtbl->WriteBool(This,lpKeyName,bValue)

typedef struct ISectionFileAVtbl
{
	DWORD (STDMETHODCALLTYPE *GetName)(ISectionA * _pThis,CHAR *lpName,DWORD nSize);
	DWORD (STDMETHODCALLTYPE *GetItemCount) (ISectionFileA * _pThis) ;
	BOOL  (STDMETHODCALLTYPE *GetItemStringA) (ISectionFileA * _pThis,int iItem,CHAR *pKey,int nKeySize,CHAR *pValue,int nValueSize) ;
	DWORD (STDMETHODCALLTYPE *GetString) (ISectionFileA * _pThis,LPCSTR pszKeyName,LPCSTR lpDefault,CHAR *lpReturnedString,DWORD nSize) ;
	LPCSTR (STDMETHODCALLTYPE *GetStringPtrA) (ISectionFileA * _pThis,LPCSTR pszKeyName,DWORD *pnLength) ;
	LPCWSTR (STDMETHODCALLTYPE *GetStringPtrW) (ISectionFileA * _pThis,LPCWSTR pszKeyName,DWORD *pnLength) ;
	UINT (STDMETHODCALLTYPE *GetInt) (ISectionFileA * _pThis,LPCSTR lpKeyName,int nDefault) ;
	BOOL (STDMETHODCALLTYPE *GetBool) (ISectionFileA * _pThis,LPCSTR pszKeyName,BOOL bDefault) ;
	BOOL (STDMETHODCALLTYPE *WriteString) (ISectionFileA * _pThis,LPCSTR lpKeyName,LPCSTR pszValue) ;
	BOOL (STDMETHODCALLTYPE *WriteInt) (ISectionFileA * _pThis,LPCSTR lpKeyName,int nValue) ;
	BOOL (STDMETHODCALLTYPE *WriteBool) (ISectionFileA * _pThis,LPCSTR lpKeyName,BOOL bValue) ;
	BOOL	(STDMETHODCALLTYPE *Flush)(ISectionFileA * _pThis) ;
	ULONG	(STDMETHODCALLTYPE *AddRef)(ISectionFileA * _pThis) ;
	ULONG	(STDMETHODCALLTYPE *Release)(ISectionFileA * _pThis) ;
} ISectionFileAVtbl;

interface ISectionFileA
{
	CONST_VTBL struct ISectionFileAVtbl *lpVtbl;
};

#define ISectionFileA_GetName(This,lpName,nSize) \
	(This)->lpVtbl->GetName(This,lpName,nSize)
#define ISectionFileA_GetItemCount(This) \
	(This)->lpVtbl->GetItemCount(This)
#define ISectionFileA_GetItemStringA(This,iItem,pKey,nKeySize,pValue,nValueSize) \
	(This)->lpVtbl->GetItemStringA(This,iItem,pKey,nKeySize,pValue,nValueSize)
#define ISectionFileA_GetString(This,pszKeyName,lpDefault,lpReturnedString,nSize) \
	(This)->lpVtbl->GetString(This,pszKeyName,lpDefault,lpReturnedString,nSize)
#define ISectionFileA_GetStringPtrA(This,pszKeyName,pnLength) \
	(This)->lpVtbl->GetStringPtrA(This,pszKeyName,pnLength)
#define ISectionFileA_GetStringPtrW(This,pszKeyName,pnLength) \
	(This)->lpVtbl->GetStringPtrW(This,pszKeyName,pnLength)
#define ISectionFileA_GetInt(This,lpKeyName,nDefault) \
	(This)->lpVtbl->GetInt(This,lpKeyName,nDefault)
#define ISectionFileA_GetBool(This,pszKeyName,bDefault) \
	(This)->lpVtbl->GetBool(This,pszKeyName,bDefault)
#define ISectionFileA_WriteString(This,lpKeyName,pszValue) \
	(This)->lpVtbl->WriteString(This,lpKeyName,pszValue)
#define ISectionFileA_WriteInt(This,lpKeyName,nValue) \
	(This)->lpVtbl->WriteInt(This,lpKeyName,nValue)
#define ISectionFileA_WriteBool(This,lpKeyName,bValue) \
	(This)->lpVtbl->WriteBool(This,lpKeyName,bValue)
#define ISectionFileA_AddRef(This)	\
	( (This)->lpVtbl -> AddRef(This) ) 
#define ISectionFileA_Release(This)	\
	( (This)->lpVtbl -> Release(This) ) 
#define ISectionFileA_Flush(This)	\
	( (This)->lpVtbl -> Flush(This) ) 

typedef struct IConfigFileAVtbl {
	DWORD   (STDMETHODCALLTYPE *GetSectionCount)(IConfigFileA *_pThis) ;
	ISectionA *	(STDMETHODCALLTYPE *GetSection)(IConfigFileA *_pThis,LPCSTR pszSection) ;
	ISectionA *	(STDMETHODCALLTYPE *GetSectionByIndex)(IConfigFileA *_pThis,int iIndex);
	DWORD	(STDMETHODCALLTYPE *GetString)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR lpKeyName,LPCSTR lpDefault,CHAR *lpReturnedString,DWORD nSize) ;
	int		(STDMETHODCALLTYPE *GetInt)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR lpKeyName,int lpDefault) ;
	BOOL	(STDMETHODCALLTYPE *GetBool)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR pszKeyName,BOOL bDefault) ;
	BOOL	(STDMETHODCALLTYPE *WriteString)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR lpKeyName,LPCSTR pszValue) ;
	BOOL	(STDMETHODCALLTYPE *WriteInt)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR lpKeyName,int nValue) ;
	BOOL	(STDMETHODCALLTYPE *WriteBool)(IConfigFileA *_pThis,LPCSTR pszSection,LPCSTR lpKeyName,BOOL bValue) ;
	BOOL	(STDMETHODCALLTYPE *Flush)(IConfigFileA *_pThis) ;
	ULONG	(STDMETHODCALLTYPE *AddRef)(IConfigFileA *_pThis) ;
	ULONG	(STDMETHODCALLTYPE *Release)(IConfigFileA *_pThis) ;
}IConfigFileAVtbl;

interface IConfigFileA
{
	CONST_VTBL struct IConfigFileAVtbl *lpVtbl;
};

#define IConfigFileA_GetSectionCount(This) \
	(This)->lpVtbl->GetSectionCount(This)
#define IConfigFileA_GetSection(This,pszSection) \
	(This)->lpVtbl->GetSection(This,iIndex)
#define IConfigFileA_GetSectionByIndex(This,pszSection) \
	(This)->lpVtbl->GetSectionByIndex(This,pszSection)
#define IConfigFileA_GetString(This,pszSection,pszKeyName,lpDefault,lpReturnedString,nSize) \
	(This)->lpVtbl->GetString(This,pszSection,pszKeyName,lpDefault,lpReturnedString,nSize)
#define IConfigFileA_GetInt(This,pszSection,lpKeyName,nDefault) \
	(This)->lpVtbl->GetInt(This,pszSection,lpKeyName,nDefault)
#define IConfigFileA_GetBool(This,pszSection,pszKeyName,bDefault) \
	(This)->lpVtbl->GetBool(This,pszSection,pszKeyName,bDefault)
#define IConfigFileA_WriteString(This,pszSection,lpKeyName,pszValue) \
	(This)->lpVtbl->WriteString(This,pszSection,lpKeyName,pszValue)
#define IConfigFileA_WriteInt(This,pszSection,lpKeyName,nValue) \
	(This)->lpVtbl->WriteInt(This,pszSection,lpKeyName,nValue)
#define IConfigFileA_WriteBool(This,pszSection,lpKeyName,bValue) \
	(This)->lpVtbl->WriteBool(This,pszSection,lpKeyName,bValue)
#define IConfigFileA_AddRef(This)	\
	( (This)->lpVtbl -> AddRef(This) ) 
#define IConfigFileA_Release(This)	\
	( (This)->lpVtbl -> Release(This) ) 
#define IConfigFileA_Flush(This)	\
	( (This)->lpVtbl -> Flush(This) ) 

#endif //__cplusplus

#if defined(__cplusplus)
extern "C"{
#endif //#if defined(__cplusplus)

	IConfigFileA * WINAPI OpenConfigFileW(LPCWSTR lpszIniFile);
	IConfigFileA * WINAPI OpenConfigFileA(LPCSTR lpszIniFile);

	ISectionFileA * WINAPI OpenSectionFileW(LPCWSTR lpszIniFile,LPCWSTR pszSection,BOOL bCreate);
	ISectionFileA * WINAPI OpenSectionFileA(LPCSTR lpszIniFile,LPCSTR pszSection,BOOL bCreate);

#if defined(__cplusplus)
}
#endif //#if defined(__cplusplus)

#ifdef UNICODE
#define OpenConfigFile OpenConfigFileW
#define OpenSectionFile OpenSectionFileW
#else
#define OpenConfigFile OpenConfigFileA
#define OpenSectionFile OpenSectionFileA
#endif

#if defined(__cplusplus)
#define ReleaseConfigFile(f) f->Release()
#else
#define ReleaseConfigFile(f) (f)->lpVtbl->Release(f)
#endif

#endif //_CCONFIGFILE_H_INC_
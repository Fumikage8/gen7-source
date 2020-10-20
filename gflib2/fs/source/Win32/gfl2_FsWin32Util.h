#if !defined(GFL2_FSWIN32UTIL_H_INCLUDED)
#define GFL2_FSWIN32UTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsWin32Util.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   Win32ファイルユーティリティ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>
#include "../internal/gfl2_FsUtilBase.h"


#if   defined(GF_PLATFORM_WIN32)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Win32Util
 @brief     Win32ファイルユーティリティ
 */
//==============================================================================
class Win32Util : public UtilBase
{
  GFL_FORBID_COPY_AND_ASSIGN(Win32Util);

public:
  //! @brief  コンストラクタ
  Win32Util(void);
  //! @brief  デストラクタ
  virtual ~Win32Util();

public:
  static Result IsPcAccessEnableFlag(bool* enableFlag);
  static Result GetProcessEnvironmentVariable(u32* pEnvVarLen, char* pEnvVar, u32 envVarBufSize, const char* envName);
  static Result GetCurrentWorkDirectory(char* path, u32 pathBufSize);
  static Result SetCurrentWorkDirectory(const char* path, char* prevPath, u32 prevPathBufSize);
  static Result CheckFileExist(bool* pExist, const char* fileName, b32 shareMode = false ); // shareMode=trueの場合、共有モードでファイルを開く。=エクセルで開いている際も開ける
  static Result GetFileSize(size_t* pFileSize, const char* fileName, b32 shareMode = false ); // shareMode=trueの場合、共有モードでファイルを開く。=エクセルで開いている際も開ける
  static Result ReadFile(void** ppBuf, size_t* pBufSize, size_t* pFileSize, const char* fileName, gfl2::heap::HeapBase* heapForBuf, u32 align, b32 shareMode = false ); // shareMode=trueの場合、共有モードでファイルを開く。=エクセルで開いている際も開ける
  static Result ReadFile(size_t* pFileSize, void* pBuf, size_t bufSize, const char* fileName, b32 shareMode = false ); // shareMode=trueの場合、共有モードでファイルを開く。=エクセルで開いている際も開ける
  static Result WriteFile(const char* fileName, const void* pBuf, size_t bufSize);
  static Result WriteFileByAddMode(const char* fileName, const void* pBuf, size_t bufSize); // 追記モード

  static Result GetFullPathFileName(
      u32* fullNameLen, char* fullName, u32 fullNameBufSize,
      AsyncFileManager* manager,
      const char* fileName
  );
  static Result CreateDir(const char* fullName);

};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32


#endif  // GFL2_FSWIN32UTIL_H_INCLUDED

#if !defined(GFL2_FSCTRHIOUTIL_H_INCLUDED)
#define GFL2_FSCTRHIOUTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrHioUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.16
 @brief   CTRのHIOファイルユーティリティ
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


#if   defined(GF_PLATFORM_CTR)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrHioUtil
 @brief     CTRのHIOファイルユーティリティ
 */
//==============================================================================
class CtrHioUtil : public UtilBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrHioUtil);

public:
  //! @brief  コンストラクタ
  CtrHioUtil(void);
  //! @brief  デストラクタ
  virtual ~CtrHioUtil();

public:
  static Result IsPcAccessEnableFlag(bool* enableFlag);
  static Result GetProcessEnvironmentVariable(u32* pEnvVarLen, char* pEnvVar, u32 envVarBufSize, const char* envName);
  static Result GetCurrentWorkDirectory(char* path, u32 pathBufSize);
  static Result SetCurrentWorkDirectory(const char* path, char* prevPath, u32 prevPathBufSize);
  static Result CheckFileExist(bool* pExist, const char* fileName, b32 shareMode = false );       // @note shareModeはwin32版に合わせているだけで未使用
  static Result GetFileSize(size_t* pFileSize, const char* fileName, b32 shareMode = false );     // @note shareModeはwin32版に合わせているだけで未使用
  static Result ReadFile(void** ppBuf, size_t* pBufSize, size_t* pFileSize, const char* fileName, gfl2::heap::HeapBase* heapForBuf, u32 align, b32 shareMode = false ); // @note shareModeはwin32版に合わせているだけで未使用
  static Result ReadFile(size_t* pFileSize, void* pBuf, size_t bufSize, const char* fileName, b32 shareMode = false ); // @note shareModeはwin32版に合わせているだけで未使用
  static Result WriteFile(const char* fileName, const void* pBuf, size_t bufSize);
  static Result WriteFileByAddMode(const char* fileName, const void* pBuf, size_t bufSize); // 追記モード
  static Result GetFullPathFileName(
      u32* fullNameLen, char* fullName, u32 fullNameBufSize,
      AsyncFileManager* manager,
      const char* fileName
  );
  static Result CreateDir(const char* fullName);

#if GFL_FS_HIO_ENABLE
private:
  //! @brief  ファイルを読み込みモードで開きサイズを取得する。ファイルの位置は先頭ではなくなっているしファイルは閉じていないので注意して下さい。
  //! @return  戻り値resのres.IsSuccess()が真なら、openFlagは必ず真になっている。
  //!          res.IsSuccess()が偽のときは、openFlagは真のときも偽のときもあり得る。
  static Result openFileReadModeAndGetFileSize(nn::hio::HostFile* file, bool* openFlag, size_t* pFileSize, const char* fileName);
  //! @brief  ファイルを読み込みモードで開きサイズを取得しファイルの位置を先頭にしておく。ファイルは閉じていないので注意して下さい。
  static Result openFileReadModeAndGetFileSizeAndSeekBegin(nn::hio::HostFile* file, bool* openFlag, size_t* pFileSize, const char* fileName);
  //! @brief  ファイルを閉じる。
  static Result closeFile(nn::hio::HostFile* file);
  //! @brief  ディレクトリをつくる
  static Result createDir(const char* fullName);
#endif
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRHIOUTIL_H_INCLUDED

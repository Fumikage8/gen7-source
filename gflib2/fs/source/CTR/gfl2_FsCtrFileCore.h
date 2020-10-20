#if !defined(GFL2_FSCTRFILECORE_H_INCLUDED)
#define GFL2_FSCTRFILECORE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrFileCore.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTRファイル核
 @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
             他でも使う場合は必ずエラー処理を見直して下さい。
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
#include <fs/include/internal/gfl2_FsFileBase.h>


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE  // @attention CTRのSDカード(デバッグ用)以外でも使う場合は、エラー処理を見直した後にこの#ifを外して下さい。


// nnのインクルード
#include <nn.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrFileCore
 @brief     CTRファイル核
 @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
             他でも使う場合は必ずエラー処理を見直して下さい。
 */
//==============================================================================
class CtrFileCore : public FileCoreBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrFileCore);

public:
  //! @brief  コンストラクタ
  CtrFileCore(void);
  //! @brief  デストラクタ
  virtual ~CtrFileCore();
  
protected:
  virtual Result openForDevice(const char* fullName, Define::OpenMode::Tag openMode);
  virtual Result closeForDevice(Define::OpenMode::Tag openMode);
  virtual Result getFileSizeForDevice(size_t* pFileSize) const;
  virtual Result readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
  virtual Result writeForDevice(const void* pBuf, size_t bufSize);
  virtual Result seekForDevice(size_t offset, Define::Origin::Tag origin);
  virtual Result tellForDevice(size_t* pos) const;

private:
  Result createDir(const char* fullName);

private:
  nn::fs::FileStream m_file;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_SDMC_ENABLE
#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRFILECORE_H_INCLUDED


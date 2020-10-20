#if !defined(GFL2_FSCTRHIOFILECORE_H_INCLUDED)
#define GFL2_FSCTRHIOFILECORE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrHioFileCore.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   CTRのHIOファイル核
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
#if GFL_FS_HIO_ENABLE


// nnのインクルード
#include <nn.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrHioFileCore
 @brief     CTRのHIOファイル核
 */
//==============================================================================
class CtrHioFileCore : public FileCoreBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrHioFileCore);

public:
  //! @brief  コンストラクタ
  CtrHioFileCore(void);
  //! @brief  デストラクタ
  virtual ~CtrHioFileCore();
  
protected:
  virtual Result openForDevice(const char* fullName, Define::OpenMode::Tag openMode);
  virtual Result closeForDevice(Define::OpenMode::Tag openMode);
  virtual Result getFileSizeForDevice(size_t* pFileSize) const;
  virtual Result readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
  virtual Result writeForDevice(const void* pBuf, size_t bufSize);
  virtual Result seekForDevice(size_t offset, Define::Origin::Tag origin);
  virtual Result tellForDevice(size_t* pos) const;

private:
  nn::hio::HostFile m_file;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_HIO_ENABLE
#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRHIOFILECORE_H_INCLUDED


#if !defined(GFL2_FSCTRROMFILECORE_H_INCLUDED)
#define GFL2_FSCTRROMFILECORE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrRomFileCore.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTRのromファイル核
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


// nnのインクルード
#include <nn.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrRomFileCore
 @brief     CTRromファイル核
 */
//==============================================================================
class CtrRomFileCore : public FileCoreBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrRomFileCore);

public:
  //! @brief  コンストラクタ
  CtrRomFileCore(void);
  //! @brief  デストラクタ
  virtual ~CtrRomFileCore();
  
protected:
  virtual Result openForDevice(const char* fullName, Define::OpenMode::Tag openMode);
  virtual Result closeForDevice(Define::OpenMode::Tag openMode);
  virtual Result getFileSizeForDevice(size_t* pFileSize) const;
  virtual Result readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize);
  virtual Result writeForDevice(const void* pBuf, size_t bufSize);
  virtual Result seekForDevice(size_t offset, Define::Origin::Tag origin);
  virtual Result tellForDevice(size_t* pos) const;

private:
  nn::fs::FileInputStream m_file;
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRROMFILECORE_H_INCLUDED


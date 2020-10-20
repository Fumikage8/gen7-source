#if !defined(GFL2_FSCTRHIODEVICE_H_INCLUDED)
#define GFL2_FSCTRHIODEVICE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrHioDevice.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   CTRのHIOデバイス
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
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrHioDevice
 @brief     CTRのHIOデバイス
 */
//==============================================================================
class CtrHioDevice : public DeviceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrHioDevice);

  friend class System;

private:
  //! @brief  コンストラクタ
  CtrHioDevice(void);
  //! @brief  デストラクタ
  virtual ~CtrHioDevice();

public:
  //! @brief  デバイスタイプを取得する
  virtual Define::DeviceType::Tag GetDeviceType(void) const;

  //! @brief  マウント
  Result Mount(gfl2::heap::HeapBase* heap, const char* path);

  //! @brief  アンマウント
  virtual Result Unmount(void);

  //! @brief  フルパスファイル名を得る
  virtual Result GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const;

private:
  char* m_path;     // 終端の'\0'の分までメモリを確保しているので、(m_pathLen +1)ある。終端は'\0'になっている。区切りは/になっている。末尾には/が付いている。
  u32   m_pathLen;  // 終端の'\0'は含まない長さ
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_HIO_ENABLE
#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRHIODEVICE_H_INCLUDED


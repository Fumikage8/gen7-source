#if !defined(GFL2_FSCTRSDMCDEVICE_H_INCLUDED)
#define GFL2_FSCTRSDMCDEVICE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCtrSdmcDevice.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.27
 @brief   CTRのSDカード(デバッグ用)デバイス
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
#if GFL_FS_SDMC_ENABLE


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     CtrSdmcDevice
 @brief     CTRのSDカード(デバッグ用)デバイス
 */
//==============================================================================
class CtrSdmcDevice : public DeviceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(CtrSdmcDevice);

  friend class System;

private:
  //! @brief  コンストラクタ
  CtrSdmcDevice(void);
  //! @brief  デストラクタ
  virtual ~CtrSdmcDevice();

public:
  //! @brief  デバイスタイプを取得する
  virtual Define::DeviceType::Tag GetDeviceType(void) const;

  //! @brief  マウント
  //! @param[in] archiveName  アーカイブ名。終端は'\0'になっていること。
  //                          例：archiveName="sdmc:" ("/"はどこにも入りません)
  Result Mount(gfl2::heap::HeapBase* heap, const char* archiveName);

  //! @brief  アンマウント
  virtual Result Unmount(void);

  //! @brief  フルパスファイル名を得る
  virtual Result GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const;

public:
  //! @brief  アーカイブ名を得る
  //! @param[out]  archiveNameLen   アーカイブ名の長さを入れて返す。終端の'\0'は含まない長さ。
  //! @param[out]  archiveName      アーカイブ名へのポインタを入れて返す。呼び出し元は見るだけで変更はしないように。
  Result GetArchiveName(u32* archiveNameLen, const char** archiveName) const;

private:
  char* m_archiveName;     // 終端の'\0'の分までメモリを確保しているので、(m_archiveNameLen +1)ある。終端は'\0'になっている。"sdmc:"とか。
  u32   m_archiveNameLen;  // 終端の'\0'は含まない長さ
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_SDMC_ENABLE
#endif  // GF_PLATFORM_CTR


#endif  // GFL2_FSCTRSDMCDEVICE_H_INCLUDED


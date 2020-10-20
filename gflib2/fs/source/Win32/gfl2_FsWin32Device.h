#if !defined(GFL2_FSWIN32DEVICE_H_INCLUDED)
#define GFL2_FSWIN32DEVICE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsWin32Device.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   WIN32デバイス
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


#if   defined(GF_PLATFORM_WIN32)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Win32Device
 @brief     WIN32デバイス
 */
//==============================================================================
class Win32Device : public DeviceBase
{
  GFL_FORBID_COPY_AND_ASSIGN(Win32Device);

  friend class System;

private:
  //! @brief  コンストラクタ
  Win32Device(void);
  //! @brief  デストラクタ
  virtual ~Win32Device();

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


#endif  // GF_PLATFORM_WIN32


#endif  // GFL2_FSWIN32DEVICE_H_INCLUDED


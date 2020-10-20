//==============================================================================
/**
 @file    gfl2_FsCtrHioDevice.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   CTRのHIOデバイス
 */
//==============================================================================


// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrHioDevice.h"


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE

  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrHioDevice::CtrHioDevice(void)
  : DeviceBase(),
    m_path(NULL),
    m_pathLen(0)
{}
CtrHioDevice::~CtrHioDevice()
{
  this->Unmount();
}

Define::DeviceType::Tag CtrHioDevice::GetDeviceType(void) const
{
  return Define::DeviceType::CTR_HIO_DEVICE;
}

Result CtrHioDevice::Mount(gfl2::heap::HeapBase* heap, const char* path)
{
  // nn::fs::hio::MountHioArchive
  // を使っていない理由は
  // HIOのときはWin32版と同じ挙動にしたかったから。
  // 即ち、garcファイルだけは短縮名でないファイルを読みに行き、
  // それ以外はromfilesフォルダ内のファイルを読むようにしている。
  // インテリジェントシステムズ様製のisfs_HioArchive.h(is::fs::hio)
  // を使っていない理由も、上記と同様。
  // テストしたわけではない。もしかしたら「..」を使えばromfilesより上のフォルダを読みに行けるかもしれない。

  Result res;

  if(!m_mountFlag)
  {
    res |= Str::CreatePath(heap, &m_pathLen, &m_path, path);
    if(res.IsSuccess())
    {
      m_mountFlag = true;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }

  return res;
}

Result CtrHioDevice::Unmount(void)
{
  Result res;

  if(m_mountFlag)
  {
    m_mountFlag = false;

    GflHeapSafeFreeMemory(m_path);
    m_pathLen = 0;
  }

  return res;
}

Result CtrHioDevice::GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const
{
  Result res;
  res |= Str::GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, 0, NULL, m_pathLen, m_path, fileName);
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_HIO_ENABLE
#endif  // GF_PLATFORM_CTR


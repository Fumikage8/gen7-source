//==============================================================================
/**
 @file    gfl2_FsWin32Device.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   WIN32デバイス
 */
//==============================================================================


// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsWin32Device.h"


#if   defined(GF_PLATFORM_WIN32)

  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Win32Device::Win32Device(void)
  : DeviceBase(),
    m_path(NULL),
    m_pathLen(0)
{}
Win32Device::~Win32Device()
{
  this->Unmount();
}

Define::DeviceType::Tag Win32Device::GetDeviceType(void) const
{
  return Define::DeviceType::WIN32_DEVICE;
}

Result Win32Device::Mount(gfl2::heap::HeapBase* heap, const char* path)
{
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

Result Win32Device::Unmount(void)
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

Result Win32Device::GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const
{
  Result res;
  res |= Str::GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, 0, NULL, m_pathLen, m_path, fileName);
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32


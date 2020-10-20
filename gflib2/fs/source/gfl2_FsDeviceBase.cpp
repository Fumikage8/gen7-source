//==============================================================================
/**
 @file    gfl2_FsDeviceBase.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   デバイス(ボリューム、イメージ)
 */
//==============================================================================


// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsDeviceBase.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


DeviceBase::DeviceBase(void)
  : m_mountFlag(false)
{}
DeviceBase::~DeviceBase()
{}

Define::DeviceType::Tag DeviceBase::GetDeviceType(void) const
{
  return Define::DeviceType::DEVICE_BASE;
}

//Result DeviceBase::Mount(gfl2::heap::HeapBase* heap, const char* path)
//{
//  Result res;
//  return res;
//}
Result DeviceBase::Unmount(void)
{
  Result res;
  return res;
}

Result DeviceBase::GetFullPathFileName(u32* /*fullNameLen*/, char* /*fullName*/, u32 /*fullNameBufSize*/, const char* /*fileName*/) const
{
  Result res;
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl)


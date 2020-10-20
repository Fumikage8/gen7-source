//==============================================================================
/**
 @file    gfl2_FsCtrSdmcPathDevice.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.27
 @brief   CTRのSDカード(デバッグ用)デバイス(パスだけ)
 */
//==============================================================================


// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrSdmcDevice.h"
#include "gfl2_FsCtrSdmcPathDevice.h"



#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE

  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrSdmcPathDevice::CtrSdmcPathDevice(void)
  : DeviceBase(),
    m_path(NULL),
    m_pathLen(0),
    m_ctrSdmcDevice(NULL)
{}
CtrSdmcPathDevice::~CtrSdmcPathDevice()
{
  this->Unmount();
}

Define::DeviceType::Tag CtrSdmcPathDevice::GetDeviceType(void) const
{
  return Define::DeviceType::CTR_SDMC_DEVICE;  // CTR_SDMC_DEVICEでよい
}

Result CtrSdmcPathDevice::Mount(gfl2::heap::HeapBase* heap, const char* path, const CtrSdmcDevice* ctrSdmcDevice)
{
  Result res;

  if(!m_mountFlag)
  {
    if(ctrSdmcDevice)
    {
      res |= Str::CreatePath(heap, &m_pathLen, &m_path, path);
      if(res.IsSuccess())
      {
        m_mountFlag = true;
        m_ctrSdmcDevice = ctrSdmcDevice;
      }
    }
    else
    {
      res |= Result::Bit::MOUNT_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }

  return res;
}

Result CtrSdmcPathDevice::Unmount(void)
{
  Result res;

  if(m_mountFlag)
  {
    m_mountFlag = false;

    GflHeapSafeFreeMemory(m_path);
    m_pathLen = 0;

    m_ctrSdmcDevice = NULL;
  }

  return res;
}

Result CtrSdmcPathDevice::GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const
{
  Result res;
  u32 archiveNameLen = 0;
  const char* archiveName = NULL;
  res |= m_ctrSdmcDevice->GetArchiveName(&archiveNameLen, &archiveName);
  if(res.IsSuccess())
  {
    res |= Str::GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, archiveNameLen, archiveName, m_pathLen, m_path, fileName);
  }
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_SDMC_ENABLE
#endif  // GF_PLATFORM_CTR


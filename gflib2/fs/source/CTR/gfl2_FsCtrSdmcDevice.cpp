//==============================================================================
/**
 @file    gfl2_FsCtrSdmcDevice.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.27
 @brief   CTRのSDカード(デバッグ用)デバイス
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrSdmcDevice.h"


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE

  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrSdmcDevice::CtrSdmcDevice(void)
  : DeviceBase(),
    m_archiveName(NULL),
    m_archiveNameLen(0)
{}
CtrSdmcDevice::~CtrSdmcDevice()
{
  this->Unmount();
}

Define::DeviceType::Tag CtrSdmcDevice::GetDeviceType(void) const
{
  return Define::DeviceType::CTR_SDMC_DEVICE;
}

Result CtrSdmcDevice::Mount(gfl2::heap::HeapBase* heap, const char* archiveName)
{
  Result res;

  if(!m_mountFlag)
  {
    res |= Str::CreateName(heap, &m_archiveNameLen, &m_archiveName, archiveName);
    if(res.IsSuccess())
    {
      nn::Result nnRes = nn::fs::MountSdmc(m_archiveName);

      if(!nnRes.IsSuccess())
      {
        GFL_PRINT("nn::fs::MountSdmc Error: ");
        if     ( nnRes <= nn::fs::ResultMediaNotFound() )      GFL_PRINT("ResultMediaNotFound");
        else if( nnRes <= nn::fs::ResultAlreadyExists() )      GFL_PRINT("ResultAlreadyExists");
        else if( nnRes <= nn::fs::ResultArchiveInvalidated() ) GFL_PRINT("ResultArchiveInvalidated");
        else if( nnRes <= nn::fs::ResultMediaAccessError() )   GFL_PRINT("ResultMediaAccessErrorn");
        else if( nnRes <= nn::fs::ResultBadFormat() )          GFL_PRINT("ResultBadFormat");
        else                                                   GFL_PRINT("Unknown");
        GFL_PRINT("\n");
      }

      if(nnRes.IsSuccess())
      {
        m_mountFlag = true;
      }
      else
      {
        res |= Result::Bit::MOUNT_FAIL;

        GflHeapSafeFreeMemory(m_archiveName);
        m_archiveNameLen = 0;
      }
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }

  return res;
}

Result CtrSdmcDevice::Unmount(void)
{
  Result res;

  if(m_mountFlag)
  {
    nn::Result nnRes = nn::fs::Unmount(m_archiveName);  // @note (1)セーブデータを二重化している際はCommitSaveDataを行うこと。(2)ファイル・ディレクトリを開いているときはFinalizeすること。(3)書き込みしているときはFlushすること。

    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::UNMOUNT_FAIL;
    }

    m_mountFlag = false;

    GflHeapSafeFreeMemory(m_archiveName);
    m_archiveNameLen = 0;
  }

  return res;
}

Result CtrSdmcDevice::GetArchiveName(u32* archiveNameLen, const char** archiveName) const
{
  Result res;
  if(m_mountFlag)
  {
    *archiveNameLen = m_archiveNameLen;
    *archiveName    = m_archiveName;
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MOUNT;
  }
  return res;
}

Result CtrSdmcDevice::GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const
{
  Result res;
  res |= Str::GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, m_archiveNameLen, m_archiveName, 0, NULL, fileName);
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_SDMC_ENABLE
#endif  // GF_PLATFORM_CTR


//==============================================================================
/**
 @file    gfl2_FsWin32FileCore.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   Win32ファイル核
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsWin32FileCore.h"
#include "../gfl2_FsDeviceBase.h"
#include "gfl2_FsWin32Func.h"


#if   defined(GF_PLATFORM_WIN32)


#include <Windows.h>  // MultiByteToWideChar
#include <stdio.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Win32FileCore::Win32FileCore(void)
  : FileCoreBase(),
    m_pFile(NULL)
{}
Win32FileCore::~Win32FileCore()
{}

Result Win32FileCore::openForDevice(const char* fullName, Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    static const char* OPEN_MODE_NAME[Define::OpenMode::NUM] =
    {
      "rb",
      "wb",
    };

    if(openMode == Define::OpenMode::WRITE)
    {
      res |= Win32Func::CreateDir(fullName);
    }
 
    fopen_s(&m_pFile, fullName, OPEN_MODE_NAME[openMode]);
    if(!m_pFile)
    {
      res |= Result::Bit::FILE_OPEN_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::closeForDevice(Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    fclose(m_pFile);
    m_pFile = NULL;
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::getFileSizeForDevice(size_t* pFileSize) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    size_t pos = ftell(m_pFile);
    fseek(m_pFile, 0, SEEK_END);
    *pFileSize = ftell(m_pFile);
    fseek(m_pFile, static_cast<long>(pos), SEEK_SET);
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    if(readSize <= bufSize)
    {
      size_t curPos = ftell(m_pFile);
      fseek(m_pFile, 0, SEEK_END);
      size_t endPos = ftell(m_pFile);
      size_t remainSize = endPos - curPos;
      fseek(m_pFile, static_cast<long>(curPos), SEEK_SET);
     
      size_t readCount = fread(pBuf, readSize, 1, m_pFile);  // size_t readCount = fread_s(pBuf, bufSize, readSize, 1, m_pFile);
      if(readCount < 1)
      {
        res |= Result::Bit::FILE_READ_FAIL;
      }
      if(res.IsSuccess())
      {
        if( pRealReadSize )
        {
          if(readSize <= remainSize)
          {
            *pRealReadSize = readSize;
          }
          else
          {
            *pRealReadSize = remainSize;
          }
        }
      }
    }
    else
    {
      res |= Result::Bit::BUF_SIZE_TOO_SMALL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::writeForDevice(const void* pBuf, size_t bufSize)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    size_t writeCount = fwrite(pBuf, bufSize, 1, m_pFile);
    if(writeCount < 1)
    {
      res |= Result::Bit::FILE_WRITE_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::seekForDevice(size_t offset, Define::Origin::Tag origin)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    static const s32 ORIGIN_VAL[Define::Origin::NUM] =
    {
      SEEK_SET,
      SEEK_CUR,
      SEEK_END,
    };

    fseek(m_pFile, static_cast<long>(offset), ORIGIN_VAL[origin]);
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result Win32FileCore::tellForDevice(size_t* pos) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
    *pos = ftell(m_pFile);
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32


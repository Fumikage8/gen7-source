//==============================================================================
/**
 @file    gfl2_FsCtrRomFileCore.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTRのromファイル核
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrRomFileCore.h"
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrRomFileCore::CtrRomFileCore(void)
  : FileCoreBase(),
    m_file()
{}
CtrRomFileCore::~CtrRomFileCore()
{}

Result CtrRomFileCore::openForDevice(const char* fullName, Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    if(openMode == Define::OpenMode::READ)
    {
#if GFL_DEBUG
      nn::Result nnRes = m_file.TryInitialize(fullName);
      if(!nnRes.IsSuccess())
      {
        res |= Result::Bit::FILE_OPEN_FAIL;
        Result::PrintNnResult(nnRes);
      }
#else
      m_file.Initialize(fullName);
      // ファイル・ディレクトリ操作時のエラーハンドリング
      // rom アーカイブ
      // 非 Try 系の API であれば、内部でエラーハンドリングが行われるため、Result を受け取る必要がありません。
#endif
    }
    else
    {
      res |= Result::Bit::FILE_WRITE_DIRECTORY_CREATE_NOT_ENABLE;
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。romはWriteできない。
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrRomFileCore::closeForDevice(Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    m_file.Finalize();  // romはWriteできないので、フラッシュの必要もない。
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrRomFileCore::getFileSizeForDevice(size_t* pFileSize) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    s64 fileSize = 0;
#if GFL_DEBUG
    nn::Result nnRes = m_file.TryGetSize(&fileSize);
    if(nnRes.IsSuccess())
    {
      *pFileSize = static_cast<size_t>(fileSize);
    }
    else
    {
      res |= Result::Bit::FILE_GET_SIZE_FAIL;
      Result::PrintNnResult(nnRes);
    }
#else
    fileSize = m_file.GetSize();
    *pFileSize = static_cast<size_t>(fileSize);
#endif
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrRomFileCore::readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    if(readSize <= bufSize)
    {
      s32 realReadSize = 0;
#if GFL_DEBUG
      nn::Result nnRes = m_file.TryRead(&realReadSize, pBuf, readSize);
      if(nnRes.IsSuccess())
      {
        if( pRealReadSize )
        {
          *pRealReadSize = static_cast<size_t>(realReadSize);
        }
      }
      else 
      {
        res |= Result::Bit::FILE_READ_FAIL;
        Result::PrintNnResult(nnRes);
      }
#else
      realReadSize = m_file.Read(pBuf, readSize);
      if( pRealReadSize )
      {
        *pRealReadSize = static_cast<size_t>(realReadSize);
      }
#endif
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

Result CtrRomFileCore::writeForDevice(const void* pBuf, size_t bufSize)
{
  Result res;
  res |= Result::Bit::FILE_WRITE_DIRECTORY_CREATE_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。romはWriteできない。
  return res;
}

Result CtrRomFileCore::seekForDevice(size_t offset, Define::Origin::Tag origin)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    static const nn::fs::PositionBase ORIGIN_VAL[Define::Origin::NUM] =
    {
      nn::fs::POSITION_BASE_BEGIN,
      nn::fs::POSITION_BASE_CURRENT,
      nn::fs::POSITION_BASE_END,
    };
    
    s64 l_pos = static_cast<s64>(offset);
#if GFL_DEBUG
    nn::Result nnRes = m_file.TrySeek(l_pos, ORIGIN_VAL[origin]);
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
      Result::PrintNnResult(nnRes);
    } 
#else
    m_file.Seek(l_pos, ORIGIN_VAL[origin]);
#endif
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrRomFileCore::tellForDevice(size_t* pos) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
    s64 l_pos = 0;
#if GFL_DEBUG
    nn::Result nnRes = m_file.TryGetPosition(&l_pos);
    if(nnRes.IsSuccess())
    {
      *pos = static_cast<size_t>(l_pos);
    }
    else
    {
      res |= Result::Bit::FILE_TELL_FAIL;
      Result::PrintNnResult(nnRes);
    } 
#else
    l_pos = m_file.GetPosition();
    *pos = static_cast<size_t>(l_pos);
#endif
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

#endif  // GF_PLATFORM_CTR


//==============================================================================
/**
 @file    gfl2_FsCtrHioFileCore.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   CTRのHIOファイル核
 */
//==============================================================================


// fsのインクルード
#include "gfl2_FsCtrHioFileCore.h"
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrHioFileCore::CtrHioFileCore(void)
  : FileCoreBase(),
    m_file()
{}
CtrHioFileCore::~CtrHioFileCore()
{}

Result CtrHioFileCore::openForDevice(const char* fullName, Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    static bit32 OPEN_MODE_VAL[Define::Define::OpenMode::NUM] =
    {
      nn::hio::HostFile::ACCESS_MODE_READ,
      nn::hio::HostFile::ACCESS_MODE_WRITE,
    };
    static nn::hio::HostFile::OpenDisp OPEN_DISP_VAL[Define::Define::OpenMode::NUM] =
    {
      nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING,
      nn::hio::HostFile::OPEN_DISP_CREATE_NEW,
    };
 
    nn::Result nnRes = m_file.Open(fullName, OPEN_MODE_VAL[openMode], OPEN_DISP_VAL[openMode]);
    if(!nnRes.IsSuccess())
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

Result CtrHioFileCore::closeForDevice(Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    nn::Result nnRes = m_file.Close();
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::FILE_CLOSE_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrHioFileCore::getFileSizeForDevice(size_t* pFileSize) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    nn::Result nnRes;
    s64 currPos = 0;
    nnRes = ((const_cast<CtrHioFileCore*>(this))->m_file).Seek(&currPos, 0, nn::hio::HostFile::SEEK_TYPE_CURRENT);  // constメンバ関数から非constメンバ関数を強引に呼ぶ
    if(nnRes.IsSuccess())
    {
      s64 fileSize = 0;
      nnRes = ((const_cast<CtrHioFileCore*>(this))->m_file).Seek(&fileSize, 0, nn::hio::HostFile::SEEK_TYPE_END);
      if(nnRes.IsSuccess())
      {
        *pFileSize = static_cast<size_t>(fileSize);
        
        s64 dummyPos = 0;
        nnRes = ((const_cast<CtrHioFileCore*>(this))->m_file).Seek(&dummyPos, currPos, nn::hio::HostFile::SEEK_TYPE_SET);
        if(!nnRes.IsSuccess())
        {
          res |= Result::Bit::FILE_SEEK_FAIL;
        }
      }
      else
      {
        res |= Result::Bit::FILE_SEEK_FAIL;
      }
    }
    else 
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrHioFileCore::readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    if(readSize <= bufSize)
    {
      size_t realReadSize = 0;
      nn::Result nnRes = m_file.Read(&realReadSize, pBuf, readSize);
      if(nnRes.IsSuccess())
      {
        if( pRealReadSize )
        {
          *pRealReadSize = realReadSize;
        }
      }
      else 
      {
        res |= Result::Bit::FILE_READ_FAIL;
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

Result CtrHioFileCore::writeForDevice(const void* pBuf, size_t bufSize)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    size_t realWriteSize = 0;
    nn::Result nnRes = m_file.Write(&realWriteSize, pBuf, bufSize);
    if(nnRes.IsSuccess())
    {
      if(realWriteSize != bufSize)
      {
        res |= Result::Bit::FILE_WRITE_FAIL;
      }
    }
    else 
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

Result CtrHioFileCore::seekForDevice(size_t offset, Define::Origin::Tag origin)
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    static const nn::hio::HostFile::SeekType ORIGIN_VAL[Define::Origin::NUM] =
    {
      nn::hio::HostFile::SEEK_TYPE_SET,
      nn::hio::HostFile::SEEK_TYPE_CURRENT,
      nn::hio::HostFile::SEEK_TYPE_END,
    };
    
    s64 l_pos = static_cast<s64>(offset);
    s64 dummyPos = 0;
    nn::Result nnRes = m_file.Seek(&dummyPos, l_pos, ORIGIN_VAL[origin]);
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
    } 
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrHioFileCore::tellForDevice(size_t* pos) const
{
  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
    s64 currPos = 0;
    nn::Result nnRes = ((const_cast<CtrHioFileCore*>(this))->m_file).Seek(&currPos, 0, nn::hio::HostFile::SEEK_TYPE_CURRENT);  // constメンバ関数から非constメンバ関数を強引に呼ぶ
    if(nnRes.IsSuccess())
    {
      *pos = static_cast<size_t>(currPos);
    }
    else
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
    } 
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL_FS_HIO_ENABLE
#endif  // GF_PLATFORM_CTR


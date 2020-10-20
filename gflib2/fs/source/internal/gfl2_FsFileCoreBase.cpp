//==============================================================================
/**
 @file    gfl2_FsFileCoreBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイル核
 */
//==============================================================================


// fsのインクルード
#include <fs/include/internal/gfl2_FsFileCoreBase.h>
#include <fs/include/gfl2_FsStr.h>
#include "../gfl2_FsDeviceBase.h"
#include "gfl2_FsCalc.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


FileCoreBase::FileCoreBase(void)
  : m_device(NULL),
    m_fileName(NULL),
    m_fileNameLen(0),
    m_openMode(Define::OpenMode::READ),
    m_openFlag(false)
{}
FileCoreBase::~FileCoreBase()
{
  this->Close();
}

Result FileCoreBase::Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  Result res;

  if(!m_openFlag)
  {
    res |= Str::CreateFileName(heap, &m_fileNameLen, &m_fileName, NULL, fileName);

    if(res.IsSuccess())
    {
      res |= this->open(device, m_fileName, openMode);
    }
    if(!res.IsSuccess())
    {
      GflHeapSafeFreeMemory(m_fileName);
      m_fileNameLen = 0;
    }
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_ALREADY;
  }

  return res;
}

Result FileCoreBase::Open(const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  Result res;

  if(!m_openFlag)
  {
    res |= this->open(device, fileName, openMode);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_ALREADY;
  }

  return res;
}

Result FileCoreBase::Close(void)
{
  Result res;

  if(m_openFlag)
  {
    res |= this->closeForDevice(m_openMode);

    m_openFlag = false;
 
    GflHeapSafeFreeMemory(m_fileName);
    m_fileNameLen = 0;
    
    m_device = NULL;
  }

  return res;
}

Result FileCoreBase::CheckFileNameIsSame(bool* sameFlag, const char* fileName) const
{
  Result res;
  *sameFlag = false;

  if(m_openFlag)
  {
    if(m_fileName)
    {
      *sameFlag = Str::CheckFileNameIsSame(m_fileName, fileName);
    }
    else
    {
      res |= Result::Bit::FILE_NAME_NOT_KEEP;
    }
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

  return res;
}

Result FileCoreBase::GetFileSize(size_t* pFileSize) const
{
  Result res;

  if(m_openFlag)
  {
    res |= this->getFileSizeForDevice(pFileSize);
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

  return res;
}

Result FileCoreBase::Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t readSize, gfl2::heap::HeapBase* heap, u32 align)
{
  Result res;
 
  // 2のべき乗チェック
  if( !( Calc::IsPowerOf2(align) ) )
  {
    res |= Result::Bit::ALIGN_INCORRECT;
  }
  else
  {
    if( align == 4096 )
    {
      *ppBuf = GflHeapAllocMemoryAlign4096(heap, readSize );  // *ppBuf = GflHeapAllocMemoryAlign(heap, static_cast<const int>(readSize), static_cast<const int>(align));
    }
    else
    {
      *ppBuf = GflHeapAllocMemoryAlign(heap, readSize, align);  // *ppBuf = GflHeapAllocMemoryAlign(heap, static_cast<const int>(readSize), static_cast<const int>(align));
    }
    if(*ppBuf)
    {
      size_t bufSize = Calc::AlignVal(readSize, align);
      res |= this->Read(pRealReadSize, *ppBuf, readSize, readSize);  // ユーザはreadSizeだけ確保するつもりでいるので、*ppBufのサイズはbufSizeではなくreadSizeとして渡すことにする。(readSize<=bufSize)は真のはず。
      if(res.IsSuccess())
      {
        if(pBufSize)
        {
          *pBufSize = bufSize;
        }
      }
      else
      {
        GflHeapSafeFreeMemory(*ppBuf);
      }
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }

 return res;
}

Result FileCoreBase::Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t pos, size_t readSize, gfl2::heap::HeapBase* heap, u32 align)
{
  Result res;
  res |= this->Seek(pos, Define::Origin::BEGIN);
  if(res.IsSuccess())
  {
    res |= this->Read(ppBuf, pBufSize, pRealReadSize, readSize, heap, align);
  }
  return res;
}

Result FileCoreBase::Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  Result res;
 
  if(m_openFlag)
  {
    res |= this->readForDevice(pRealReadSize, pBuf, bufSize, readSize);
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

 return res;
}

Result FileCoreBase::Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t pos, size_t readSize)
{
  Result res;
  res |= this->Seek(pos, Define::Origin::BEGIN);
  if(res.IsSuccess())
  {
    res |= this->Read(pRealReadSize, pBuf, bufSize, readSize);
  }
  return res;
}

Result FileCoreBase::Write(const void* pBuf, size_t bufSize)
{
  Result res;
 
  if(m_openFlag)
  {
    res |= this->writeForDevice(pBuf, bufSize);
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

 return res;
}

Result FileCoreBase::Seek(size_t offset, Define::Origin::Tag origin)
{
  Result res;
 
  if(m_openFlag)
  {
    res |= this->seekForDevice(offset, origin);
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

 return res;
}

Result FileCoreBase::Tell(size_t* pos) const
{
  Result res;
 
  if(m_openFlag)
  {
    res |= this->tellForDevice(pos);
  }
  else
  {
    res |= Result::Bit::FILE_NOT_OPEN;
  }

 return res;
}

Result FileCoreBase::open(const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  Result res;

  m_device = device;  // ForDevice関数内で使用されるので、ここで一旦設定する。
  m_openMode = openMode;

  char fullName[Define::FILE_NAME_FULL_LEN_MAX];
  u32  fullNameLen = 0;
  res = m_device->GetFullPathFileName(&fullNameLen, fullName, Define::FILE_NAME_FULL_LEN_MAX, fileName);

  if(res.IsSuccess())
  {
    res |= openForDevice(fullName, m_openMode);

    if(res.IsSuccess())
    {
      m_openFlag = true;
    }
    else
    {
      m_device = NULL;
    }
  }
  else
  {
    m_device = NULL;
  }

  return res;
}

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


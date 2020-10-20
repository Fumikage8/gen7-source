//==============================================================================
/**
 @file    gfl2_FsFileBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイル基底クラス
 */
//==============================================================================


// fsのインクルード
#include <fs/include/internal/gfl2_FsFileBase.h>
#include "../gfl2_FsDeviceBase.h"
#include "../Win32/gfl2_FsWin32FileCore.h"
#include "../CTR/gfl2_FsCtrFileCore.h"
#include "../CTR/gfl2_FsCtrRomFileCore.h"
#include "../CTR/gfl2_FsCtrHioFileCore.h"



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


FileBase::FileBase(void)
  : m_fileCore(NULL)
{}
FileBase::~FileBase()
{
  this->Close();
}

Result FileBase::Close(void)
{
  return this->close();
}

Result FileBase::CheckFileNameIsSame(bool* sameFlag, const char* fileName) const
{
  Result res;

  if(m_fileCore)
  {
    res |= m_fileCore->CheckFileNameIsSame(sameFlag, fileName);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }
  
  return res;
}

Result FileBase::GetFileSize(size_t* pFileSize) const
{
  Result res;

  if(m_fileCore)
  {
    res |= m_fileCore->GetFileSize(pFileSize);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t readSize, gfl2::heap::HeapBase* heap, u32 align)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Read(ppBuf, pBufSize, pRealReadSize, readSize, heap, align);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Read(void** ppBuf, size_t* pBufSize, size_t* pRealReadSize, size_t pos, size_t readSize, gfl2::heap::HeapBase* heap, u32 align)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Read(ppBuf, pBufSize, pRealReadSize, pos, readSize, heap, align);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Read(pRealReadSize, pBuf, bufSize, readSize);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Read(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t pos, size_t readSize)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Read(pRealReadSize, pBuf, bufSize, pos, readSize);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Seek(size_t offset, Define::Origin::Tag origin)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Seek(offset, origin);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::Tell(size_t* pos) const
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Tell(pos);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  Result res;
  
  if(device->GetDeviceType() == Define::DeviceType::WIN32_DEVICE)
  {
#if   defined(GF_PLATFORM_WIN32)
    m_fileCore = GFL_NEW(heap) Win32FileCore();
#endif  // GF_PLATFORM_WIN32
  }
  else if(device->GetDeviceType() == Define::DeviceType::CTR_ROM_DEVICE)
  {
#if   defined(GF_PLATFORM_CTR)
    m_fileCore = GFL_NEW(heap) CtrRomFileCore();
#endif  // GF_PLATFORM_CTR
  }
  else if(device->GetDeviceType() == Define::DeviceType::CTR_HIO_DEVICE)
  {
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
    m_fileCore = GFL_NEW(heap) CtrHioFileCore();
#endif
#endif  // GF_PLATFORM_CTR
  }
  else if(device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE
    m_fileCore = GFL_NEW(heap) CtrFileCore();
#endif
#endif  // GF_PLATFORM_CTR
  }
  else
  {
    res |= Result::Bit::FILE_CORE_CREATE_FAIL;
  }

  if(m_fileCore)
  {
    res |= m_fileCore->Open(heap, device, fileName, openMode);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

Result FileBase::close(void)
{
  Result res;
 
  if(m_fileCore)
  {
    res |= m_fileCore->Close();
    GFL_SAFE_DELETE(m_fileCore);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}

  
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


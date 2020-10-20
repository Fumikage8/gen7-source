//==============================================================================
/**
 @file    gfl2_FsFile.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   通常ファイル
 */
//==============================================================================


// fsのインクルード
#include <fs/include/internal/gfl2_FsFile.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


File::File(void)
  : FileBase()
{
}
File::~File()
{
  this->Close();
}

Result File::Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  return this->open(heap, device, fileName, openMode);
}

Result File::Close(void)
{
  return this->close();
}
  
Result File::Write(const void* pBuf, size_t bufSize)
{
  Result res;
 
  if(m_fileCore)
  {
    res = m_fileCore->Write(pBuf, bufSize);
  }
  else
  {
    res |= Result::Bit::FILE_CORE_NOT_EXIST;
  }

  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


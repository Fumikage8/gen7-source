//==============================================================================
/**
 @file    gfl2_FsDirectSdmcFile.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.30
 @brief   DirectSdmc用のファイル
          Win32ではPCが、CTRではSDカード(デバッグ用)がデバイスになります。
 */
//==============================================================================


// fsのインクルード
#include <fs/include/gfl2_FsDirectSdmcFile.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


DirectSdmcFile::DirectSdmcFile(void)
  : File()
{}
DirectSdmcFile::~DirectSdmcFile()
{
  this->Close();
}

Result DirectSdmcFile::Open(gfl2::heap::HeapBase* heap, const DeviceBase* device, const char* fileName, Define::OpenMode::Tag openMode)
{
  return File::Open(heap, device, fileName, openMode);
}
Result DirectSdmcFile::Close(void)
{
  return File::Close();
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


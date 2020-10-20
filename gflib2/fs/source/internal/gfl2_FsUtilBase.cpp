//==============================================================================
/**
 @file    gfl2_FsUtilBase.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.17
 @brief   ファイルユーティリティ基底クラス
 */
//==============================================================================


// fsのインクルード
#include "gfl2_FsUtilBase.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


UtilBase::UtilBase(void)
{}
UtilBase::~UtilBase()
{}

Result UtilBase::IsPcAccessEnableFlag(bool* /*enableFlag*/)
{
  Result res;
  return res;
}

Result UtilBase::GetProcessEnvironmentVariable(u32* /*pEnvVarLen*/, char* /*pEnvVar*/, u32 /*envVarBufSize*/, const char* /*envName*/)
{
  Result res;
  return res;
}

Result UtilBase::GetCurrentWorkDirectory(char* /*path*/, u32 /*pathBufSize*/)
{
  Result res;
  return res;
}

Result UtilBase::SetCurrentWorkDirectory(const char* /*path*/, char* /*prevPath*/, u32 /*prevPathBufSize*/)
{
  Result res;
  return res;
}

Result UtilBase::CheckFileExist(bool* /*pExist*/, const char* /*fileName*/)
{
  Result res;
  return res;
}

Result UtilBase::GetFileSize(size_t* /*pFileSize*/, const char* /*fileName*/)
{
  Result res;
  return res;
}

Result UtilBase::ReadFile(void** /*ppBuf*/, size_t* /*pBufSize*/, size_t* /*pFileSize*/, const char* /*fileName*/, gfl2::heap::HeapBase* /*heapForBuf*/, u32 /*align*/)
{
  Result res;
  return res;
}

Result UtilBase::ReadFile(size_t* /*pFileSize*/, void* /*pBuf*/, size_t /*bufSize*/, const char* /*fileName*/)
{
  Result res;
  return res;
}

Result UtilBase::WriteFile(const char* /*fileName*/, const void* /*pBuf*/, size_t /*bufSize*/)
{
  Result res;
  return res;
}

Result UtilBase::GetFullPathFileName(
    u32* /*fullNameLen*/, char* /*fullName*/, u32 /*fullNameBufSize*/,
    AsyncFileManager* /*manager*/,
    const char* /*fileName*/
)
{
  Result res;
  return res;
}

Result UtilBase::CreateDir(const char* /*fullName*/)
{
  Result res;
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


//==============================================================================
/**
 @file    gfl2_FsCtrHioUtil.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.16
 @brief   CTRのHIOファイルユーティリティ
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


// fsのインクルード
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/internal/gfl2_FsFile.h>
#include <fs/include/gfl2_FsStr.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "gfl2_FsCtrHioUtil.h"
#include "../internal/gfl2_FsCalc.h"
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrHioUtil::CtrHioUtil(void)
{}
CtrHioUtil::~CtrHioUtil()
{}

Result CtrHioUtil::IsPcAccessEnableFlag(bool* enableFlag)
{
  Result res;
  *enableFlag = false;

#if GFL_FS_HIO_ENABLE
  res |= System::GetHioEnableFlag(enableFlag);
#endif
  
  return res;
}

Result CtrHioUtil::GetProcessEnvironmentVariable(u32* pEnvVarLen, char* pEnvVar, u32 envVarBufSize, const char* envName)
{
#if GFL_FS_HIO_ENABLE
  Result res;

  s32 envVarLen = 0;
  nn::Result nnRes = nn::hio::GetEnvironmentVariable(&envVarLen, pEnvVar, static_cast<s32>(envVarBufSize), envName);
  if(nnRes.IsSuccess())
  {
    if(pEnvVarLen) *pEnvVarLen = Str::GetStrLen(pEnvVar);
  }
  else
  {
    res |= Result::Bit::ENVIRONMENT_VARIABLE_GET_FAIL;
  }
  
  return res;
#else
  if(pEnvVarLen) *pEnvVarLen = 0;
  GFL_UNUSED(pEnvVar);
  GFL_UNUSED(envVarBufSize);
  GFL_UNUSED(envName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::GetCurrentWorkDirectory(char* path, u32 pathBufSize)
{
#if GFL_FS_HIO_ENABLE
  Result res;

  if(path)
  {
    s32 pathLen;
    nn::Result nnRes = nn::hio::GetCurrentDirectory(&pathLen, path, static_cast<s32>(pathBufSize));
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::CURRENT_DIRECTORY_GET_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::CURRENT_DIRECTORY_GET_FAIL;
  }

  return res;
#else
  GFL_UNUSED(path);
  GFL_UNUSED(pathBufSize);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::SetCurrentWorkDirectory(const char* path, char* prevPath, u32 prevPathBufSize)
{
#if GFL_FS_HIO_ENABLE
  Result res;

  if(prevPath)
  {
    res |= GetCurrentWorkDirectory(prevPath, prevPathBufSize);
  }

  if(res.IsSuccess())
  {
    nn::Result nnRes = nn::hio::SetCurrentDirectory(path);
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::CURRENT_DIRECTORY_SET_FAIL;
    }
  }

  return res;
#else
  GFL_UNUSED(path);
  GFL_UNUSED(prevPath);
  GFL_UNUSED(prevPathBufSize);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::CheckFileExist(bool* pExist, const char* fileName, b32 shareMode )
{
#if GFL_FS_HIO_ENABLE
  Result res;
  nn::hio::HostFile file;
  nn::Result nnRes = file.Exists(pExist, fileName);
  if(!nnRes.IsSuccess())
  {
    res |= Result::Bit::FILE_EXIST_CHECK_FAIL;
  }
  return res;
#else
  *pExist = false;
  GFL_UNUSED(fileName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::GetFileSize(size_t* pFileSize, const char* fileName, b32 shareMode )
{
#if GFL_FS_HIO_ENABLE
  Result res;

  nn::hio::HostFile file;
  bool openFlag = false;
  res |= openFileReadModeAndGetFileSize(&file, &openFlag, pFileSize, fileName);

  if(openFlag)
  {
    res |= closeFile(&file);
  }

  return res;
#else
  *pFileSize = 0;
  GFL_UNUSED(fileName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::ReadFile(void** ppBuf, size_t* pBufSize, size_t* pFileSize, const char* fileName, gfl2::heap::HeapBase* heapForBuf, u32 align, b32 shareMode )
{
#if GFL_FS_HIO_ENABLE
  Result res;

  // 2のべき乗チェック
  if( !( Calc::IsPowerOf2(align) ) )
  {
    res |= Result::Bit::ALIGN_INCORRECT;
  }
  else
  {
    nn::hio::HostFile file;
    bool openFlag = false;
    size_t fileSize = 0;
    res |= openFileReadModeAndGetFileSizeAndSeekBegin(&file, &openFlag, &fileSize, fileName);
 
    if(res.IsSuccess())
    {
      *ppBuf = GflHeapAllocMemoryAlign(heapForBuf, fileSize, align);
      size_t bufSize = Calc::AlignVal(fileSize, align);

      size_t realReadSize = 0;
      nn::Result nnRes = file.Read(&realReadSize, *ppBuf, fileSize);
      if(nnRes.IsSuccess() && realReadSize==fileSize)  // ファイル丸ごと読み込みだからrealReadSize==fileSizeとならねばならない。
      {
        if(pFileSize) *pFileSize = fileSize;
        if(pBufSize) *pBufSize = bufSize;
      }
      else
      {
        GflHeapSafeFreeMemory(*ppBuf);
        res |= Result::Bit::FILE_READ_FAIL;
      }
    }

    if(openFlag)
    {
      res |= closeFile(&file);
    }
  }

  return res;
#else
  *ppBuf = NULL;
  if(pFileSize) *pFileSize = 0;
  if(pBufSize) *pBufSize = 0;
  GFL_UNUSED(fileName);
  GFL_UNUSED(heapForBuf);
  GFL_UNUSED(align);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::ReadFile(size_t* pFileSize, void* pBuf, size_t bufSize, const char* fileName, b32 shareMode )
{
#if GFL_FS_HIO_ENABLE
  Result res;

  nn::hio::HostFile file;
  bool openFlag = false;
  size_t fileSize = 0;
  res |= openFileReadModeAndGetFileSizeAndSeekBegin(&file, &openFlag, &fileSize, fileName);

  if(res.IsSuccess())
  {
    if( fileSize <= bufSize )
    {
      size_t realReadSize = 0;
      nn::Result nnRes = file.Read(&realReadSize, pBuf, fileSize);
      if(nnRes.IsSuccess() && realReadSize==fileSize)  // ファイル丸ごと読み込みだからrealReadSize==fileSizeとならねばならない。
      {
        if(pFileSize) *pFileSize = fileSize;
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

  if(openFlag)
  {
    res |= closeFile(&file);
  }

  return res;
#else
  if(pFileSize) *pFileSize = 0;
  GFL_UNUSED(pBuf);
  GFL_UNUSED(bufSize);
  GFL_UNUSED(fileName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::WriteFile(const char* fileName, const void* pBuf, size_t bufSize)
{
#if GFL_FS_HIO_ENABLE
  Result res;
  nn::Result nnRes;

  res |= createDir(fileName);

  nn::hio::HostFile file;
  //nnRes = file.Open(fileName, nn::hio::HostFile::ACCESS_MODE_WRITE, nn::hio::HostFile::OPEN_DISP_CREATE_NEW);  // OPEN_DISP_CREATE_NEWだとファイルが既に存在するときオープンできないようなので(エラー内容までは確認していない)コメントアウト。
  nnRes = file.Open(fileName, nn::hio::HostFile::ACCESS_MODE_WRITE, nn::hio::HostFile::OPEN_DISP_CREATE_ALWAYS);
  
  if(nnRes.IsSuccess())
  {
    size_t realWriteSize = 0;
    nnRes = file.Write(&realWriteSize, pBuf, bufSize);
    if(!(nnRes.IsSuccess() && realWriteSize==bufSize))  // ファイル丸ごと書き込みだからrealWriteSize==fileSizeとならねばならない。
    {
      res |= Result::Bit::FILE_WRITE_FAIL;
    }

    res |= closeFile(&file);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }
 
  return res;
#else
  GFL_UNUSED(fileName);
  GFL_UNUSED(pBuf);
  GFL_UNUSED(bufSize);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::WriteFileByAddMode(const char* fileName, const void* pBuf, size_t bufSize)
{
#if GFL_FS_HIO_ENABLE
  Result res;
  nn::Result nnRes;

  res |= createDir(fileName);

  nn::hio::HostFile file;
  bool bExist = false;
  CheckFileExist(&bExist, fileName);

  if (bExist)
  {
    nnRes = file.Open(fileName, nn::hio::HostFile::ACCESS_MODE_READ_WRITE, nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING);
  }
  else
  {
    nnRes = file.Open(fileName, nn::hio::HostFile::ACCESS_MODE_READ_WRITE, nn::hio::HostFile::OPEN_DISP_CREATE_NEW);
  }
  
  if(nnRes.IsSuccess())
  {
    // 末尾へ
    file.Seek(0, nn::hio::HostFile::SEEK_TYPE_END);

    size_t realWriteSize = 0;
    nnRes = file.Write(&realWriteSize, pBuf, bufSize);
    if(!(nnRes.IsSuccess() && realWriteSize==bufSize))  // ファイル丸ごと書き込みだからrealWriteSize==fileSizeとならねばならない。
    {
      //res |= Result::Bit::FILE_WRITE_FAIL;
    }

    res |= closeFile(&file);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }
 
  return res;
#else
  GFL_UNUSED(fileName);
  GFL_UNUSED(pBuf);
  GFL_UNUSED(bufSize);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}



Result CtrHioUtil::GetFullPathFileName(
    u32* fullNameLen, char* fullName, u32 fullNameBufSize,
    AsyncFileManager* manager,
    const char* fileName
)
{
#if GFL_FS_HIO_ENABLE
  Result res;
  const DeviceBase* device = manager->GetPcDevice();
  res |= device->GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, fileName);
  return res;
#else
  GFL_UNUSED(fullNameLen);
  GFL_UNUSED(fullName);
  GFL_UNUSED(fullNameBufSize);
  GFL_UNUSED(manager);
  GFL_UNUSED(fileName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

Result CtrHioUtil::CreateDir(const char* fullName)
{
#if GFL_FS_HIO_ENABLE
  return createDir(fullName);
#else
  GFL_UNUSED(fullName);
  Result res;
  res |= Result::Bit::HIO_NOT_ENABLE;
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  return res;
#endif
}

#if GFL_FS_HIO_ENABLE
Result CtrHioUtil::openFileReadModeAndGetFileSize(nn::hio::HostFile* file, bool* openFlag, size_t* pFileSize, const char* fileName)
{
  Result res;
  nn::Result nnRes;

  nnRes = file->Open(fileName, nn::hio::HostFile::ACCESS_MODE_READ, nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING);

  if(nnRes.IsSuccess())
  {
    *openFlag = true;

    s64 pos;
    nnRes = file->Seek(&pos, 0, nn::hio::HostFile::SEEK_TYPE_END);
    if(nnRes.IsSuccess())
    {
      *pFileSize = static_cast<size_t>(pos);
    }
    else
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
    }
  }
  else
  {
    *openFlag = false;
    
    res |= Result::Bit::FILE_OPEN_FAIL;
  }

  return res;
}

Result CtrHioUtil::openFileReadModeAndGetFileSizeAndSeekBegin(nn::hio::HostFile* file, bool* openFlag, size_t* pFileSize, const char* fileName)
{
  Result res;
  res |= openFileReadModeAndGetFileSize(file, openFlag, pFileSize, fileName);
  if(res.IsSuccess())
  {
    s64 pos;
    nn::Result nnRes = file->Seek(&pos, 0, nn::hio::HostFile::SEEK_TYPE_SET);
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
    }
  }
  return res;
}

Result CtrHioUtil::closeFile(nn::hio::HostFile* file)
{
  Result res;
  nn::Result nnRes = file->Close();
  if(!nnRes.IsSuccess())
  {
    res |= Result::Bit::FILE_CLOSE_FAIL;
  }
  return res;
}

Result CtrHioUtil::createDir(const char* fullName)
{
  // CtrFileCore::createDirと似ているので、まとめられるならまとめたいなあ。
  
  // @attention  CTRのHIO(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;

  char path[Define::FILE_NAME_FULL_LEN_MAX];
  res |= Str::CopySafely(path, Define::FILE_NAME_FULL_LEN_MAX, fullName);
  if(res.IsSuccess())
  {
    u32 slashCount = 0;
    u32 pathLen = 0;
    u32 endlessLoopCount = 0;
    for ever
    {
      pathLen = Str::GetPathLenBySlash(path, slashCount);
      if(pathLen > 0)
      {
        char replacedChar = path[pathLen];
        path[pathLen] = '\0';
        {
          nn::Result nnRes = nn::hio::CreateDirectory(path);
          if(!nnRes.IsSuccess())
          {
            // 既存フォルダをつくろうとしてエラーになっているかもしれないので、このエラーは無視する
            
            // @attention  CTRのHIO(デバッグ用)で起きたエラーなので、気にしない。
            //             CTRのHIO(デバッグ用)以外でも使う場合は、エラー処理を見直して下さい。
          }
        }
        path[pathLen] = replacedChar;
        ++slashCount;
      }
      else
      {
        break;
      }

      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
  }

  return res;
}
#endif


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_CTR


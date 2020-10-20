//==============================================================================
/**
 @file    gfl2_FsWin32Util.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   Win32ファイルユーティリティ
 */
//==============================================================================


// fsのインクルード
#include <fs/include/internal/gfl2_FsFile.h> 
#include <fs/include/gfl2_FsStr.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "gfl2_FsWin32Util.h"
#include "../internal/gfl2_FsCalc.h"
#include "../gfl2_FsDeviceBase.h"
#include "gfl2_FsWin32Func.h"

#if   defined(GF_PLATFORM_WIN32)


#include <Windows.h>  // MultiByteToWideChar, SetCurrentDirectory
#include <Share.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


Win32Util::Win32Util(void)
{}
Win32Util::~Win32Util()
{}

Result Win32Util::IsPcAccessEnableFlag(bool* enableFlag)
{
  Result res;
  *enableFlag = true;
  return res;
}

Result Win32Util::GetProcessEnvironmentVariable(u32* pEnvVarLen, char* pEnvVar, u32 envVarBufSize, const char* envName)
{
  Result res;

  size_t envVarLen = 0;
  ::getenv_s(&envVarLen, pEnvVar, static_cast<size_t>(envVarBufSize), envName);
  if(pEnvVarLen)
  {
    *pEnvVarLen = Str::GetStrLen(pEnvVar);
  }

  return res;
}

Result Win32Util::GetCurrentWorkDirectory(char* path, u32 pathBufSize)
{
  Result res;

  if(path)
  {
    TCHAR pathWide[Define::FILE_NAME_FULL_LEN_MAX];
    u32 r0 = ::GetCurrentDirectory(Define::FILE_NAME_FULL_LEN_MAX, pathWide);
    if(r0 == 0)
    {
      res |= Result::Bit::CURRENT_DIRECTORY_GET_FAIL;
    }
    else
    {
      s32 r1 = ::WideCharToMultiByte(CP_ACP, 0, pathWide, -1, path, static_cast<s32>(pathBufSize), NULL, NULL);
      // ワイド文字からマルチバイト文字への変換を行う際はC言語標準ライブラリのwcstombs_sがある。使うには#include <stdlib.h>が必要。
      if(r1 == 0)
      {
        res |= Result::Bit::WIDE_CHAR_TO_MULTI_BYTE_CHAR_FAIL;
      }
    }
  }
  else
  {
    res |= Result::Bit::CURRENT_DIRECTORY_GET_FAIL;
  }

  return res;
}

Result Win32Util::SetCurrentWorkDirectory(const char* path, char* prevPath, u32 prevPathBufSize)
{
  Result res;

  if(prevPath)
  {
    res |= GetCurrentWorkDirectory(prevPath, prevPathBufSize);
  }

  if(res.IsSuccess())
  {
    TCHAR pathWide[Define::FILE_NAME_FULL_LEN_MAX];
    s32 r0 = ::MultiByteToWideChar(CP_ACP, 0, path, -1, pathWide, Define::FILE_NAME_FULL_LEN_MAX);
    if(r0 == 0)
    {
      res |= Result::Bit::MULTI_BYTE_CHAR_TO_WIDE_CHAR_FAIL;
    }
    else
    {
      BOOL r1 = ::SetCurrentDirectory(pathWide);  // Win32の関数で戻り値はBOOL型。
      if(r1 == 0)
      {
        res |= Result::Bit::CURRENT_DIRECTORY_SET_FAIL;
      }
    }
  }

  return res;
}

Result Win32Util::CheckFileExist(bool* pExist, const char* fileName, b32 shareMode )
{
  Result res;

  FILE* l_pFile = NULL;

  if( shareMode )
  {
    l_pFile = _fsopen( fileName, "rb", _SH_DENYNO );
  }
  else
  {
    fopen_s(&l_pFile, fileName, "rb");
  }

  if(l_pFile)
  {
    fclose(l_pFile);
    *pExist = true;
  }
  else
  {
    *pExist = false;
  }

  return res;
}

Result Win32Util::GetFileSize(size_t* pFileSize, const char* fileName,  b32 shareMode )
{
  Result res;

  FILE* l_pFile = NULL;
  if( shareMode )
  {
    l_pFile = _fsopen( fileName, "rb", _SH_DENYNO );
  }
  else
  {
    fopen_s(&l_pFile, fileName, "rb");
  }

  if(l_pFile)
  {
    fseek(l_pFile, 0, SEEK_END);
    *pFileSize = ftell(l_pFile);

    fclose(l_pFile);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }

  return res;
}

Result Win32Util::ReadFile(void** ppBuf, size_t* pBufSize, size_t* pFileSize, const char* fileName, gfl2::heap::HeapBase* heapForBuf, u32 align, b32 shareMode )
{
  Result res;

  // 2のべき乗チェック
  if( !( Calc::IsPowerOf2(align) ) )
  {
    res |= Result::Bit::ALIGN_INCORRECT;
  }
  else
  {
    FILE* l_pFile = NULL;
    if( shareMode )
    {
      l_pFile = _fsopen( fileName, "rb", _SH_DENYNO );
    }
    else
    {
      fopen_s(&l_pFile, fileName, "rb");
    }

    if(l_pFile)
    {
      fseek(l_pFile, 0, SEEK_END);
      size_t fileSize = ftell(l_pFile);
     
      *ppBuf = GflHeapAllocMemoryAlign(heapForBuf, fileSize, align);
      size_t bufSize = Calc::AlignVal(fileSize, align);
     
      fseek(l_pFile, 0, SEEK_SET);
      size_t readCount = fread(*ppBuf, fileSize, 1, l_pFile);  // fread_s
     
      if(readCount < 1)
      {
        GflHeapSafeFreeMemory(*ppBuf);
        res |= Result::Bit::FILE_READ_FAIL;
      }
      else
      {
        if(pFileSize) *pFileSize = fileSize;
        if(pBufSize) *pBufSize = bufSize;
      }
      
      fclose(l_pFile);
    }
    else
    {
      res |= Result::Bit::FILE_OPEN_FAIL;
    }
  }

  return res;
}

Result Win32Util::ReadFile(size_t* pFileSize, void* pBuf, size_t bufSize, const char* fileName, b32 shareMode )
{
  Result res;

  FILE* l_pFile = NULL;
  if( shareMode )
  {
    l_pFile = _fsopen( fileName, "rb", _SH_DENYNO );
  }
  else
  {
    fopen_s(&l_pFile, fileName, "rb");
  }

  if(l_pFile)
  {
    fseek(l_pFile, 0, SEEK_END);
    size_t fileSize = ftell(l_pFile);
 
    if( fileSize <= bufSize )
    {
      fseek(l_pFile, 0, SEEK_SET);
      size_t readCount = fread(pBuf, fileSize, 1, l_pFile);  // fread_s
 
      if(readCount < 1)
      {
        res |= Result::Bit::FILE_READ_FAIL;
      }
      else
      {
        if(pFileSize) *pFileSize = fileSize;
      }
    }
    else
    {
      res |= Result::Bit::BUF_SIZE_TOO_SMALL;
    }
 
    fclose(l_pFile);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }

  return res;
}

Result Win32Util::WriteFile(const char* fileName, const void* pBuf, size_t bufSize)
{
  Result res;

  res |= Win32Func::CreateDir(fileName);

  FILE* l_pFile = NULL;
  fopen_s(&l_pFile, fileName, "wb");

  if(l_pFile)
  {
    size_t writeCount = fwrite(pBuf, bufSize, 1, l_pFile);
    if(writeCount < 1)
    {
      res |= Result::Bit::FILE_WRITE_FAIL;
    }
    
    fclose(l_pFile);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }
 
  return res;
}

Result Win32Util::WriteFileByAddMode(const char* fileName, const void* pBuf, size_t bufSize)
{
  Result res;

  res |= Win32Func::CreateDir(fileName);

  FILE* l_pFile = NULL;
  fopen_s(&l_pFile, fileName, "ab");

  if(l_pFile)
  {
    size_t writeCount = fwrite(pBuf, bufSize, 1, l_pFile);
    if(writeCount < 1)
    {
      res |= Result::Bit::FILE_WRITE_FAIL;
    }
    
    fclose(l_pFile);
  }
  else
  {
    res |= Result::Bit::FILE_OPEN_FAIL;
  }
 
  return res;
}

Result Win32Util::GetFullPathFileName(
    u32* fullNameLen, char* fullName, u32 fullNameBufSize,
    AsyncFileManager* manager,
    const char* fileName
)
{
  Result res;
  const DeviceBase* device = manager->GetPcDevice();
  res |= device->GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, fileName);
  return res;
}

Result Win32Util::CreateDir(const char* fullName)
{
  return Win32Func::CreateDir(fullName);
}

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_WIN32


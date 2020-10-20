//==============================================================================
/**
 @file    gfl2_FsCtrFileCore.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTRファイル核
 @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
             他でも使う場合は必ずエラー処理を見直して下さい。
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrFileCore.h"
#include "../gfl2_FsDeviceBase.h"


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE  // @attention CTRのSDカード(デバッグ用)以外でも使う場合は、エラー処理を見直した後にこの#ifを外して下さい。


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrFileCore::CtrFileCore(void)
  : FileCoreBase(),
    m_file()
{}
CtrFileCore::~CtrFileCore()
{}

Result CtrFileCore::openForDevice(const char* fullName, Define::OpenMode::Tag openMode)
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    static bit32 OPEN_MODE_VAL[Define::Define::OpenMode::NUM] =
    {
      nn::fs::OPEN_MODE_READ,
      nn::fs::OPEN_MODE_WRITE | nn::fs::OPEN_MODE_CREATE,
    };

    if(openMode == Define::OpenMode::WRITE)
    {
      res |= this->createDir(fullName);
    }

    {
      nn::Result nnRes = m_file.TryInitialize(fullName, OPEN_MODE_VAL[openMode]);
      if(!nnRes.IsSuccess())
      {
        res |= Result::Bit::FILE_OPEN_FAIL;
        Result::PrintNnResult(nnRes);
      }
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::closeForDevice(Define::OpenMode::Tag openMode)
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    if(openMode == Define::OpenMode::WRITE)
    {
      nn::Result nnRes = m_file.TryFlush();  // Writeのときだけフラッシュ
      if(!nnRes.IsSuccess())
      {
        res |= Result::Bit::FILE_FLUSH_FAIL;
        Result::PrintNnResult(nnRes);
      }
    }
    m_file.Finalize();  // フラッシュが終わっていないとパニックになる
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::getFileSizeForDevice(size_t* pFileSize) const
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    s64 fileSize = 0;
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
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::readForDevice(size_t* pRealReadSize, void* pBuf, size_t bufSize, size_t readSize)
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    if(readSize <= bufSize)
    {
      s32 realReadSize = 0;
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

Result CtrFileCore::writeForDevice(const void* pBuf, size_t bufSize)
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    s32 realWriteSize = 0;
    bool flushFlag = false;  // フラッシュを毎回行うと遅くなりそうなのでfalseにし、close前にフラッシュを行うことにする。
    nn::Result nnRes = m_file.TryWrite(&realWriteSize, pBuf, bufSize, flushFlag);
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
      Result::PrintNnResult(nnRes);
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::seekForDevice(size_t offset, Define::Origin::Tag origin)
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    static const nn::fs::PositionBase ORIGIN_VAL[Define::Origin::NUM] =
    {
      nn::fs::POSITION_BASE_BEGIN,
      nn::fs::POSITION_BASE_CURRENT,
      nn::fs::POSITION_BASE_END,
    };
    
    s64 l_pos = static_cast<s64>(offset);
    nn::Result nnRes = m_file.TrySeek(l_pos, ORIGIN_VAL[origin]);
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::FILE_SEEK_FAIL;
      Result::PrintNnResult(nnRes);
    } 
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::tellForDevice(size_t* pos) const
{
  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
  //             他でも使う場合は必ずエラー処理を見直して下さい。

  Result res;
  
  if(m_device->GetDeviceType() == Define::DeviceType::CTR_SDMC_DEVICE)
  {
    s64 l_pos = 0;
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
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MATCH;
  }

  return res;
}

Result CtrFileCore::createDir(const char* fullName)
{
  // CtrHioUtil::createDirと似ているので、まとめられるならまとめたいなあ。

  // @attention  CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理が不十分かもしれません。
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
          nn::Result nnRes = nn::fs::TryCreateDirectory(path);
              // TryCreateDirectoryをテストして分かったこと
              //     sdmc:/aaa/bbb/cccという書き方でOK。\区切りは試していないので分からない。
              //     存在しないフォルダを含んだパスを指定すると、目的のフォルダがつくられない。確認していないが多分エラーになっている。
              //     既に存在するフォルダをつくろうとしたときにエラーが起きているかどうかは、確認していないので分からない。
          if(!nnRes.IsSuccess())
          {
            // 既存フォルダをつくろうとしてエラーになっているかもしれないので、このエラーは無視する
            
            // @attention  CTRのSDカード(デバッグ用)で起きたエラーなので、気にしない。
            //             CTRのSDカード(デバッグ用)以外でも使う場合は、エラー処理を見直して下さい。
            Result::PrintNnResult(nnRes);
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


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_FS_SDMC_ENABLE
#endif  // GF_PLATFORM_CTR


//==============================================================================
/**
 @file    gfl2_FsCtrRomDevice.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   CTR(3DS)のrom
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// fsのインクルード
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/gfl2_FsStr.h>
#include "gfl2_FsCtrRomDevice.h"


#if   defined(GF_PLATFORM_CTR)


// nnのインクルード
#include <nn.h>

  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


CtrRomDevice::CtrRomDevice(void)
  : DeviceBase(),
    m_archiveName(NULL),
    m_archiveNameLen(0),
    m_path(NULL),
    m_pathLen(0),
    m_maxFile(NULL),
    m_maxDirectory(NULL),
    m_useCache(false),
    m_workingMemory(NULL),
    m_workingMemorySize(0)
{}
CtrRomDevice::~CtrRomDevice()
{
  this->Unmount();
}

Define::DeviceType::Tag CtrRomDevice::GetDeviceType(void) const
{
  return Define::DeviceType::CTR_ROM_DEVICE;
}

Result CtrRomDevice::Mount(gfl2::heap::HeapBase* heap, const char* archiveName, const char* path, size_t maxFile, size_t maxDirectory, bool useCache)
{
  Result res;

  if(!m_mountFlag)
  {
    res |= Str::CreateName(heap, &m_archiveNameLen, &m_archiveName, archiveName);
    if(res.IsSuccess())
    {
      res |= Str::CreatePath(heap, &m_pathLen, &m_path, path);
    }
    if(res.IsSuccess())
    {
      m_maxFile      = maxFile;
      m_maxDirectory = maxDirectory;
      m_useCache     = useCache;
      m_workingMemorySize = nn::fs::GetRomRequiredMemorySize(m_maxFile, m_maxDirectory, m_useCache);  // CTR-SDK 1.2、2.1以降なら、エラーハンドリングしなくてよい
      m_workingMemory = GflHeapAllocMemoryAlign(heap, m_workingMemorySize, 4);

      /*nn::Result nnRes =*/ nn::fs::MountRom(m_archiveName, m_maxFile, m_maxDirectory, m_workingMemory, m_workingMemorySize, m_useCache);  // CTR-SDK 1.2、2.1以降なら、エラーハンドリングしなくてよい
                                                                                                                                        // @attention 必ずrom2:（パッチ）の実験をしておくこと！

      m_mountFlag = true;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }

  return res;
}

Result CtrRomDevice::Unmount(void)
{
  Result res;

  if(m_mountFlag)
  {
    if( System::GetAsyncFileManagerForCtrRomDevice() )
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT。CTRのromにアクセスしている可能性のある非同期ファイルマネージャがまだ残っている。

      res |= System::DestroyAsyncFileManagerForCtrRomDevice();
      GFL_ASSERT(res.IsSuccess());  // 開発中に気付かせるASSERT。非同期ファイルマネージャの破棄に失敗した。
    }

    nn::Result nnRes = nn::fs::Unmount(m_archiveName);
        // @note
        // (1)セーブデータを二重化している際はCommitSaveDataを行うこと。 → セーブデータではなくromなので大丈夫。
        // (2)ファイル・ディレクトリを開いているときはFinalizeすること。 → 非同期ファイルマネージャを終了させたので大丈夫。
        // (3)書き込みしているときはFlushすること。                      → CTRのromファイルにはWriteをさせていないので大丈夫。
    
    if(!nnRes.IsSuccess())
    {
      if(nnRes <= nn::fs::ResultNotFound())
      {
        // 指定したアーカイブはマウントされていません。
        // 製品では発生しないようにするべきエラーです。
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }

      NN_ERR_THROW_FATAL_ALL(nnRes);
      // [質問] FSのエラーハンドリングについて
      // ys5034 2011-01-05 21:04:40 
      // アプリケーション側の不具合でなければ、特に対応は必須ではありませんが、
      // NN_ERR_THROW_FATALではエラーレベルによってはメッセージ表示されないため、
      // NN_ERR_THROW_FATAL_ALLをお使いください。
      
      res |= Result::Bit::UNMOUNT_FAIL;
    }

    m_mountFlag = false;

    GflHeapSafeFreeMemory(m_workingMemory);
    m_maxFile      = 0;
    m_maxDirectory = 0;
    m_useCache     = false;
    m_workingMemorySize = 0;

    GflHeapSafeFreeMemory(m_path);
    m_pathLen = 0;
    GflHeapSafeFreeMemory(m_archiveName);
    m_archiveNameLen = 0;
  }

  return res;
}

Result CtrRomDevice::GetFullPathFileName(u32* fullNameLen, char* fullName, u32 fullNameBufSize, const char* fileName) const
{
  Result res;
  res |= Str::GetFullPathFileName(fullNameLen, fullName, fullNameBufSize, m_archiveNameLen, m_archiveName, m_pathLen, m_path, fileName);
  return res;
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GF_PLATFORM_CTR


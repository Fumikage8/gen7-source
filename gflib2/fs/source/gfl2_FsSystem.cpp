//==============================================================================
/**
 @file    gfl2_FsSystem.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステム
 */
//==============================================================================


// gfl2のインクルード
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_Threadstatic.h>

// fsのインクルード
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "gfl2_FsDeviceBase.h"
#include "internal/gfl2_FsCalc.h"
#include <fs/include/gfl2_FsDirectSdmcFile.h>




#if   defined(GF_PLATFORM_WIN32)

// fsのインクルード
  #include "Win32/gfl2_FsWin32Device.h"

#elif defined(GF_PLATFORM_CTR)

  // nnのインクルード
  #include <nn.h>

  // fsのインクルード
  #include "CTR/gfl2_FsCtrRomDevice.h"

#if GFL_FS_HIO_ENABLE
  // CTR(3DS)のHIO
  #include "CTR/gfl2_FsCtrHioDevice.h"
#endif
#if GFL_FS_SDMC_ENABLE
  // CTR(3DS)のSDカード(デバッグ用)
  #include "CTR/gfl2_FsCtrSdmcDevice.h"
  #include "CTR/gfl2_FsCtrSdmcPathDevice.h"
#endif

#endif  // GF_PLATFORM_




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


////////////////////////////////////////////////////////////////////////////////
bool System::m_onceInitializeFlag = false;
bool System::m_initializeFlag     = false;

#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
void* System::m_hioBuf = NULL;
#endif
#endif  // GF_PLATFORM_CTR

#if GFL_FS_DEBUG_LOG
gfl2::heap::HeapBase* System::m_fsDebugLogHeap = NULL;
#endif


////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_WIN32)
Win32Device*      System::m_win32RomDevice                    = NULL;
AsyncFileManager* System::m_asyncFileManagerForWin32RomDevice = NULL;
////////////////////////////////////////////////////////////////////////////////
#elif defined(GF_PLATFORM_CTR)
CtrRomDevice*     System::m_ctrRomDevice                    = NULL;
AsyncFileManager* System::m_asyncFileManagerForCtrRomDevice = NULL;
#endif  // GF_PLATFORM_

////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
CtrHioDevice*     System::m_ctrHioRomDevice                   = NULL;
#endif
#endif  // GF_PLATFORM_CTR

#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE
CtrSdmcDevice*    System::m_ctrSdmcDevice                     = NULL;
#endif
#endif  // GF_PLATFORM_CTR

////////////////////////////////////////////////////////////////////////////////
#if GFL_FS_GASC_ENABLE
#if   defined(GF_PLATFORM_WIN32)
Win32Device*   System::m_win32ArcSrcDevice      = NULL;
#elif defined(GF_PLATFORM_CTR)
CtrHioDevice*  System::m_ctrArcSrcDevice        = NULL;
#endif  // GF_PLATFORM_
#endif

////////////////////////////////////////////////////////////////////////////////
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
DeviceBase* System::m_directSdmcDevice = NULL;
#endif




////////////////////////////////////////////////////////////////////////////////
System::System(void)
{}
System::~System()
{
#if   defined(GF_PLATFORM_CTR)
  UnmountCtrDirectSdmcDevice();
  TerminateCtrSdmcDevice();
  
  DestroyAsyncFileManagerForCtrRomDevice();

  UnmountCtrRomDevice();
  UnmountCtrHioRomDevice();
  UnmountCtrArcSrcDevice();

#elif defined(GF_PLATFORM_WIN32)
  UnmountWin32DirectSdmcDevice();

  DestroyAsyncFileManagerForWin32RomDevice();

  UnmountWin32RomDevice();
  UnmountWin32ArcSrcDevice();
#endif  // GF_PLATFORM_

  Terminate();
}

Result System::Initialize(gfl2::heap::HeapBase* devHeapForHio, gfl2::heap::HeapBase* heapForFsDebugLog)
{
  Result res;

  if(!m_onceInitializeFlag)
  {
#if   defined(GF_PLATFORM_CTR)
    nn::fs::Initialize();
#endif  // GF_PLATFORM_CTR

    m_onceInitializeFlag = true;
  }

  if(!m_initializeFlag)
  {
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
    if(devHeapForHio)
    {
      m_hioBuf = GflHeapAllocMemory(devHeapForHio, nn::hio::WORKMEMORY_SIZE);
      nn::Result nnRes = nn::hio::Initialize(m_hioBuf);
      if(nnRes.IsSuccess())
      {
        s32 hioVersion = nn::hio::GetVersion();
        GFL_PRINT("HostIO Library Version: %d\n", hioVersion);
      }
      else
      {
        res |= Result::Bit::HIO_INITIALIZE_FAIL;
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
      }
    }
#endif
#endif  // GF_PLATFORM_CTR

#if GFL_FS_DEBUG_LOG
    m_fsDebugLogHeap = heapForFsDebugLog;
#endif
    
    m_initializeFlag = true;
  }

  return res;
}
Result System::Terminate(void)
{
  Result res;

  if(m_initializeFlag)
  {
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
    if(m_hioBuf)
    {
      nn::Result nnRes = nn::hio::Finalize();
      if(!nnRes.IsSuccess())
      {
        res |= Result::Bit::HIO_TERMINATE_FAIL;
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
      }
      GflHeapSafeFreeMemory(m_hioBuf);
    }
#endif
#endif  // GF_PLATFORM_CTR

    m_initializeFlag = false;
  }

  return res;
}

AsyncFileManager* System::GetAsyncFileManagerForRomDevice(void)
{
#if   defined(GF_PLATFORM_WIN32)
  return GetAsyncFileManagerForWin32RomDevice();
#elif defined(GF_PLATFORM_CTR)
  return GetAsyncFileManagerForCtrRomDevice();
#endif  // GF_PLATFORM_
}

const DeviceBase* System::GetArcSrcDevice(void)
{
#if GFL_FS_GASC_ENABLE
#if   defined(GF_PLATFORM_WIN32)
  return getWin32ArcSrcDevice();
#elif defined(GF_PLATFORM_CTR)
  return getCtrArcSrcDevice();
#endif  // GF_PLATFORM_
#else
  return NULL;
#endif
}

const DeviceBase* System::GetDirectSdmcDevice(void)
{
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  return m_directSdmcDevice;
#else
  return NULL;
#endif
}

Result System::GetHioEnableFlag(bool* enableFlag)
{
  Result res;
  *enableFlag = false;

#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  if(m_hioBuf)
  {
    nn::Result nnRes = nn::hio::GetHioDaemonStatus(enableFlag);  // @note この関数が呼ばれる度に、ホスト側でのHostIOデーモンの起動状態を確認するのは、負荷が大きいか？
                                                                 //       負荷が大きいなら、Initialize後に1回だけ確認して結果を覚えておき、この関数ではその覚えている結果を返すだけにしよう。
    if(!nnRes.IsSuccess())
    {
      res |= Result::Bit::HIO_GET_DAEMON_STATUS_FAIL;
      *enableFlag = false;
    }
  }
#endif
#endif  // GF_PLATFORM_CTR

  return res;
}

Result System::GetSdmcEnableFlag(bool* enableFlag)
{
  Result res;
  *enableFlag = false;

#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE
  if(m_ctrSdmcDevice)
  {
    *enableFlag = true;
  }
#endif
#endif  // GF_PLATFORM_CTR

  return res;
}


DirectSdmcFile* System::CreateDirectSdmcFile(gfl2::heap::HeapBase* heap, const char* fileName, Define::OpenMode::Tag openMode)
{
  DirectSdmcFile* file = NULL;
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  Result res;
  if(m_directSdmcDevice)
  {
    file = GFL_NEW(heap) DirectSdmcFile;
    if(file)
    {
      res |= file->Open(heap, m_directSdmcDevice, fileName, openMode);
      if(!res.IsSuccess())
      {
        GFL_SAFE_DELETE(file);
        file = NULL;
      }
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::DEVICE_NOT_MOUNT;
  }
#endif
  return file;
}
void System::DestroyDirectSdmcFile(DirectSdmcFile* file)
{
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  if(file)
  {
    Result res;
    res |= file->Close();
    GFL_SAFE_DELETE(file);
  }
#endif
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_WIN32)

Result System::MountWin32RomDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;
  
  if(m_win32RomDevice == NULL)
  {
    m_win32RomDevice = GFL_NEW(heap) Win32Device();
    if(m_win32RomDevice)
    {
      res |= m_win32RomDevice->Mount(heap, path);
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
  
  return res;
}
Result System::UnmountWin32RomDevice(void)
{
  Result res;
  if(m_win32RomDevice)
  {
    res |= m_win32RomDevice->Unmount();
    GFL_SAFE_DELETE(m_win32RomDevice);
  }
  return res;
}
const DeviceBase* System::GetWin32RomDevice(void)
{
  return m_win32RomDevice;
}


Result System::CreateAsyncFileManagerForWin32RomDevice(gfl2::heap::HeapBase* heap, int fileThreadPrio, int otherThreadPrio, u32 fsDebugLogArcFileInfoElemNumMax)
{
  Result res;
  
  if(m_asyncFileManagerForWin32RomDevice == NULL)
  {
    DeviceBase* arcSrcDevice = NULL;
#if GFL_FS_GASC_ENABLE
    arcSrcDevice = m_win32ArcSrcDevice;
#endif
    gfl2::heap::HeapBase* fsDebugLogHeap = NULL;
#if GFL_FS_DEBUG_LOG
    fsDebugLogHeap = m_fsDebugLogHeap;
#endif
    m_asyncFileManagerForWin32RomDevice = GFL_NEW(heap) AsyncFileManager(heap, m_win32RomDevice, NULL, arcSrcDevice, fileThreadPrio, otherThreadPrio, fsDebugLogHeap, fsDebugLogArcFileInfoElemNumMax);
    if(m_asyncFileManagerForWin32RomDevice == NULL)
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MANAGER_ALREADY_EXIST;
  }
  
  return res;
}
Result System::DestroyAsyncFileManagerForWin32RomDevice(void)
{
  Result res;
  if(m_asyncFileManagerForWin32RomDevice)
  {
    m_asyncFileManagerForWin32RomDevice->CancelAllReq();
    m_asyncFileManagerForWin32RomDevice->RequestEnd();
    u32 endlessLoopCount = 0;
    while(!m_asyncFileManagerForWin32RomDevice->HasEnded())
    {
      Calc::SleepInLoop();

      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX_LONG)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
    GFL_SAFE_DELETE(m_asyncFileManagerForWin32RomDevice);
  }
  return res;
}
AsyncFileManager* System::GetAsyncFileManagerForWin32RomDevice(void)
{
  return m_asyncFileManagerForWin32RomDevice;
}

////////////////////////////////////////////////////////////////////////////////
Result System::MountWin32ArcSrcDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;
  
#if GFL_FS_GASC_ENABLE
  if(m_win32ArcSrcDevice == NULL)
  {
    m_win32ArcSrcDevice = GFL_NEW(heap) Win32Device();
    if(m_win32ArcSrcDevice)
    {
      res |= m_win32ArcSrcDevice->Mount(heap, path);
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#endif

  return res;
}
Result System::UnmountWin32ArcSrcDevice(void)
{
  Result res;
#if GFL_FS_GASC_ENABLE
  if(m_win32ArcSrcDevice)
  {
    res |= m_win32ArcSrcDevice->Unmount();
    GFL_SAFE_DELETE(m_win32ArcSrcDevice);
  }
#endif
  return res;
}
const DeviceBase* System::getWin32ArcSrcDevice(void)
{
#if GFL_FS_GASC_ENABLE
  return m_win32ArcSrcDevice;
#else
  return NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////
Result System::MountWin32DirectSdmcDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  if(!m_directSdmcDevice)
  {
    Win32Device* l_device = GFL_NEW(heap) Win32Device();
    if(l_device)
    {
      res |= l_device->Mount(heap, path);
      if(res.IsSuccess())
      {
        m_directSdmcDevice = l_device;
      }
      else
      {
        GFL_SAFE_DELETE(l_device);
      }
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#endif
  return res;
}
Result System::UnmountWin32DirectSdmcDevice(void)
{
  Result res;
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  if(m_directSdmcDevice)
  {
    res |= m_directSdmcDevice->Unmount();
    GFL_SAFE_DELETE(m_directSdmcDevice);
  }
#endif
  return res;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#elif defined(GF_PLATFORM_CTR)

Result System::MountCtrRomDevice(gfl2::heap::HeapBase* heap, const char* archiveName, const char* path, size_t maxFile, size_t maxDirectory, bool useCache)
{
  Result res;
  
  if(m_ctrRomDevice == NULL)
  {
    m_ctrRomDevice = GFL_NEW(heap) CtrRomDevice();
    if(m_ctrRomDevice)
    {
      res |= m_ctrRomDevice->Mount(heap, archiveName, path, maxFile, maxDirectory, useCache);
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
  
  return res;
}
Result System::UnmountCtrRomDevice(void)
{
  Result res;
  if(m_ctrRomDevice)
  {
    res |= m_ctrRomDevice->Unmount();
    GFL_SAFE_DELETE(m_ctrRomDevice);
  }
  return res;
}
const DeviceBase* System::GetCtrRomDevice(void)
{
  return m_ctrRomDevice;
}


Result System::CreateAsyncFileManagerForCtrRomDevice(gfl2::heap::HeapBase* heap, int fileThreadPrio, int otherThreadPrio, u32 fsDebugLogArcFileInfoElemNumMax)
{
  Result res;
  
  if(m_asyncFileManagerForCtrRomDevice == NULL)
  {
    DeviceBase* hioRomDevice = NULL;
    DeviceBase* arcSrcDevice = NULL;
#if GFL_FS_HIO_ENABLE
    hioRomDevice = m_ctrHioRomDevice;
#endif
#if GFL_FS_GASC_ENABLE
    arcSrcDevice = m_ctrArcSrcDevice;
#endif
    gfl2::heap::HeapBase* fsDebugLogHeap = NULL;
#if GFL_FS_DEBUG_LOG
    fsDebugLogHeap = m_fsDebugLogHeap;
#endif
    m_asyncFileManagerForCtrRomDevice = GFL_NEW(heap) AsyncFileManager(heap, m_ctrRomDevice, hioRomDevice, arcSrcDevice, fileThreadPrio, otherThreadPrio, fsDebugLogHeap, fsDebugLogArcFileInfoElemNumMax);
    if(m_asyncFileManagerForCtrRomDevice == NULL)
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MANAGER_ALREADY_EXIST;
  }
  
  return res;
}
Result System::DestroyAsyncFileManagerForCtrRomDevice(void)
{
  Result res;
  if(m_asyncFileManagerForCtrRomDevice)
  {
    m_asyncFileManagerForCtrRomDevice->CancelAllReq();
    m_asyncFileManagerForCtrRomDevice->RequestEnd();
    u32 endlessLoopCount = 0;
    while(!m_asyncFileManagerForCtrRomDevice->HasEnded())
    {
      Calc::SleepInLoop();

      if(++endlessLoopCount >= Define::ENDLESS_LOOP_COUNT_MAX_LONG)
      {
        GFL_ASSERT(0);  // 開発中に無限ループに気付かせるためのASSERT
        break;
      }
    }
    GFL_SAFE_DELETE(m_asyncFileManagerForCtrRomDevice);
  }
  return res;
}
AsyncFileManager* System::GetAsyncFileManagerForCtrRomDevice(void)
{
  return m_asyncFileManagerForCtrRomDevice;
}

////////////////////////////////////////////////////////////////////////////////
Result System::MountCtrHioRomDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;

#if GFL_FS_HIO_ENABLE
  if(m_ctrHioRomDevice == NULL)
  {
    m_ctrHioRomDevice = GFL_NEW(heap) CtrHioDevice();
    if(m_ctrHioRomDevice)
    {
      res |= m_ctrHioRomDevice->Mount(heap, path);
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#endif

  return res;
}
Result System::UnmountCtrHioRomDevice(void)
{
  Result res;
#if GFL_FS_HIO_ENABLE
  if(m_ctrHioRomDevice)
  {
    res |= m_ctrHioRomDevice->Unmount();
    GFL_SAFE_DELETE(m_ctrHioRomDevice);
  }
#endif
  return res;
}
const DeviceBase* System::GetCtrHioRomDevice(void)
{
#if GFL_FS_HIO_ENABLE
  return m_ctrHioRomDevice;
#else
  return NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////
Result System::MountCtrArcSrcDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;
  
#if GFL_FS_GASC_ENABLE
  if(m_ctrArcSrcDevice == NULL)
  {
    m_ctrArcSrcDevice = GFL_NEW(heap) CtrHioDevice();
    if(m_ctrArcSrcDevice)
    {
      res |= m_ctrArcSrcDevice->Mount(heap, path);
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#endif

  return res;
}
Result System::UnmountCtrArcSrcDevice(void)
{
  Result res;
#if GFL_FS_GASC_ENABLE
  if(m_ctrArcSrcDevice)
  {
    res |= m_ctrArcSrcDevice->Unmount();
    GFL_SAFE_DELETE(m_ctrArcSrcDevice);
  }
#endif
  return res;
}
const DeviceBase* System::getCtrArcSrcDevice(void)
{
#if GFL_FS_GASC_ENABLE
  return m_ctrArcSrcDevice;
#else
  return NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////
Result System::InitializeCtrSdmcDevice(gfl2::heap::HeapBase* heap, const char* archiveName)
{
  Result res;
  
#if GFL_FS_SDMC_ENABLE
  if(m_ctrSdmcDevice == NULL)
  {
    m_ctrSdmcDevice = GFL_NEW(heap) CtrSdmcDevice();
    if(m_ctrSdmcDevice)
    {
      res |= m_ctrSdmcDevice->Mount(heap, archiveName);
      if(!res.IsSuccess())
      {
        GFL_SAFE_DELETE(m_ctrSdmcDevice);
      }
    }
    else
    {
      res |= Result::Bit::HEAP_ALLOC_FAIL;
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#endif

  return res;
}
Result System::TerminateCtrSdmcDevice(void)
{
  Result res;
#if GFL_FS_SDMC_ENABLE
  if(m_ctrSdmcDevice)
  {
    res |= m_ctrSdmcDevice->Unmount();
    GFL_SAFE_DELETE(m_ctrSdmcDevice);
  }
#endif
  return res;
}

////////////////////////////////////////////////////////////////////////////////
Result System::MountCtrDirectSdmcDevice(gfl2::heap::HeapBase* heap, const char* path)
{
  Result res;
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
#if GFL_FS_SDMC_ENABLE
  if(!m_directSdmcDevice)
  {
    if(res.IsSuccess())
    {
      bool enableFlag = false;
      res |= GetSdmcEnableFlag(&enableFlag);
      if(res.IsSuccess() && enableFlag)
      {
        CtrSdmcPathDevice* l_device = GFL_NEW(heap) CtrSdmcPathDevice();
        if(l_device)
        {
          res |= l_device->Mount(heap, path, m_ctrSdmcDevice);
          if(res.IsSuccess())
          {
            m_directSdmcDevice = l_device;
          }
          else
          {
            GFL_SAFE_DELETE(l_device);
          }
        }
        else
        {
          res |= Result::Bit::HEAP_ALLOC_FAIL;
        }
      }
      else
      {
        res |= Result::Bit::SDMC_NOT_ENABLE;
      }
    }
  }
  else
  {
    res |= Result::Bit::MOUNT_ALREADY;
  }
#else  // GFL_FS_SDMC_ENABLE
  res |= Result::Bit::SDMC_NOT_ENABLE;
#endif  // GFL_FS_SDMC_ENABLE
#endif
  return res;
}
Result System::UnmountCtrDirectSdmcDevice(void)
{
  Result res;
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  if(m_directSdmcDevice)
  {
    res |= m_directSdmcDevice->Unmount();
    GFL_SAFE_DELETE(m_directSdmcDevice);
  }
#endif
  return res;
}


#endif  // GF_PLATFORM_



////////////////////////////////////////////////////////////////////////////////
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


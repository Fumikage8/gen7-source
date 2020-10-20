//===================================================================
/**
 * @file    MainHeap.hpp
 * @brief   nijiアプリケーション メインフレームのHEAP初期化
 * @author  GAME FREAK Inc.
 * @date    15.01.31
 *
 * Main.cppからinclude
*/
//===================================================================

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include "HeapDefine.h"
#include "HeapSize.h"


namespace Main{

#if defined(GF_PLATFORM_WIN32)

#include <stdlib.h>

static void HeapCreate(void * systemHeapAdrs,void * deviceHeapAdrs, size_t systemHeapSize, size_t devieHeapSize, int scale);
static void HeapFree(void);

//--------------------------------------------
//
/**

@class HeapObject

静的なクラスのコンストラクタ呼び出し順序は、下記のルールに従う
（１）同じコンパイル単位（*.objファイル）内では、宣言された順番
（２）別のコンパイル単位では、リンカの振る舞いによるため、未定義

new/deleteをオーバーライドする際に、コンストラクタ内でnewを使われると
heapシステムが起動していないため問題が起きる。
それを防ぐため、すべての静的オブジェクトの中で真っ先にHeapObjectの
コンストラクタが動くように配置している。

*/
//--------------------------------------------
class HeapObject
{
public:
  static const int HEAP_SCALE_SIZE = 5; // Win32はこの値分メモリを多く取る。

public:
  HeapObject()
  {

    m_SystemHeapTotalSize = System::HeapSize::HEAP_MEMORY;
    m_DeviceHeapTotalSize = System::HeapSize::DEVICE_MEMORY+System::HeapSize::DEBUG_DEVICE;
    m_SystemHeapSize = m_SystemHeapTotalSize-System::HeapSize::RESIDENT;
    m_DeviceHeapSize = m_DeviceHeapTotalSize-System::HeapSize::RESIDENT_DEVICE;

    m_Win32WorkHeapSize = System::HeapSize::WIN32_WORK_MEMORY;


    m_addr_heap = malloc(m_SystemHeapTotalSize * HEAP_SCALE_SIZE);
    m_addr_dev  = malloc(m_DeviceHeapTotalSize * HEAP_SCALE_SIZE);
    m_addr_win32_work = malloc( m_Win32WorkHeapSize );
    HeapCreate(m_addr_heap, m_addr_dev, m_SystemHeapTotalSize * HEAP_SCALE_SIZE, m_DeviceHeapTotalSize * HEAP_SCALE_SIZE, HEAP_SCALE_SIZE);
  }
  ~HeapObject()
  {
    HeapFree();
    free(m_addr_win32_work);
    free(m_addr_heap);
    free(m_addr_dev);
    m_addr_win32_work = NULL;
    m_addr_heap = NULL;
    m_addr_dev = NULL;
  }

  void* GetSystemHeapInfo(size_t* size)
  {
    *size = m_SystemHeapSize * HEAP_SCALE_SIZE;
    return m_addr_heap;
  }
  void* GetDeviceHeapInfo(size_t* size)
  {
    *size = m_DeviceHeapSize * HEAP_SCALE_SIZE;
    return m_addr_dev;
  }
  void* GetResidentHeapInfo(size_t* size)
  {
    *size = System::HeapSize::RESIDENT * HEAP_SCALE_SIZE;
    return reinterpret_cast<u8*>(m_addr_heap)+(m_SystemHeapSize * HEAP_SCALE_SIZE);
  }
  void* GetResidentDeviceHeapInfo(size_t* size)
  {
    *size = System::HeapSize::RESIDENT_DEVICE * HEAP_SCALE_SIZE;
    return reinterpret_cast<u8*>(m_addr_dev)+(m_DeviceHeapSize * HEAP_SCALE_SIZE);
  }
  void* GetWin32WorkHeapInfo(size_t* size)
  {
    *size = m_Win32WorkHeapSize;
    return m_addr_win32_work;
  }

private:


  void * m_addr_heap;
  void * m_addr_dev;
  void * m_addr_win32_work;
  size_t  m_SystemHeapTotalSize;
  size_t  m_DeviceHeapTotalSize;
  size_t  m_SystemHeapSize;
  size_t  m_DeviceHeapSize;
  size_t  m_Win32WorkHeapSize;
};

HeapObject		g_heapSys;		// メモリ管理用・静的オブジェクトの先頭に置くこと！！

void HeapCreate(void * systemHeapAdrs,void * deviceHeapAdrs, size_t systemHeapSize, size_t devieHeapSize, int scale)
{
  size_t  win32WorkHeapSize = 0;
  void*   win32WorkHeapAdrs = g_heapSys.GetWin32WorkHeapInfo(&win32WorkHeapSize);
  
  //-----------------------------------------------------------------------------
  /**
  *    ヒープの初期化
  */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Initialize( systemHeapAdrs, systemHeapSize , HEAPID_CHILD_MAX, HEAPID_LOCAL_MAX );
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
  systemHeap->SetCreateHeapScale( scale );

#if PM_DEBUG
  gfl2::heap::GflHeapBase::SetDebugFreeMemClear(gfl2::heap::GflHeapBase::DEBUG_FREE_MEMC_FF);
#endif  

  if ( gfl2::heap::Manager::IsExistHeapId( gfl2::heap::HEAPID_SYSTEM ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] SystemHeap = NULL!!\n");
  }
 
  gfl2::heap::HeapBase* deviceHeap = GFL_CREATE_HEAP_BUFFER( deviceHeapAdrs, HEAPID_DEVICE , devieHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  deviceHeap->SetCreateHeapScale(scale);
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEVICE ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] DeviceHeap = NULL!!\n");
  }
#if GFL_DEBUG_PRINT
	gfl2::heap::HeapBase* win32WorkHeap = GFL_CREATE_HEAP_BUFFER( win32WorkHeapAdrs, HEAPID_WIN32_WORK, win32WorkHeapSize, gfl2::heap::HEAP_TYPE_EXP );
#else
  GFL_CREATE_HEAP_BUFFER( win32WorkHeapAdrs, HEAPID_WIN32_WORK, win32WorkHeapSize, gfl2::heap::HEAP_TYPE_EXP );
#endif
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_WIN32_WORK ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] Win32WorkHeap = NULL!!\n");
  }

  GFL_PRINT("[GFL_USE] SystemHeapSize    =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", systemHeapSize, systemHeapAdrs, systemHeap, systemHeap->GetTotalAllocatableSize());
  GFL_PRINT("[GFL_USE] SysDeviceHeapSize =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", devieHeapSize, deviceHeapAdrs, deviceHeap, deviceHeap->GetTotalAllocatableSize() );
  GFL_PRINT("[GFL_USE] Win32WorkHeapSize =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", win32WorkHeapSize, win32WorkHeapAdrs, win32WorkHeap, win32WorkHeap->GetTotalAllocatableSize());  
}

void HeapFree(void)
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM ));


  //-----------------------------------------------------------------------------
  /**
  *    ヒープの破棄
  */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Finalize();
}

//--------------------------------------------
/*
  HEAPID_RESIDENT, HEAPID_RESIDENT_DEVICEの初期化
  ソフトリセットで消すために、Systemとは初期化タイミングをずらす

  GflUseモジュール初期化のタイミングで呼び出す
*/
//--------------------------------------------
void ResidentHeapCreate(void)
{
  size_t  residentHeapSize = 0;
  void*   residentHeapAdrs = g_heapSys.GetResidentHeapInfo(&residentHeapSize);
  size_t  residentDeviceHeapSize = 0;
  void*   residentDeviceHeapAdrs = g_heapSys.GetResidentDeviceHeapInfo(&residentDeviceHeapSize);
  
  gfl2::heap::HeapBase* residentHeap = GFL_CREATE_HEAP_BUFFER( residentHeapAdrs, HEAPID_RESIDENT , residentHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  residentHeap->SetCreateHeapScale( HeapObject::HEAP_SCALE_SIZE );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_RESIDENT ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] ResidentHeap = NULL!!\n");
  }
  gfl2::heap::HeapBase* residentDeviceHeap = GFL_CREATE_HEAP_BUFFER( residentDeviceHeapAdrs, HEAPID_RESIDENT_DEVICE , residentDeviceHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  residentDeviceHeap->SetCreateHeapScale( HeapObject::HEAP_SCALE_SIZE );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_RESIDENT_DEVICE ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] ResidentDeviceHeap = NULL!!\n");
  }
  GFL_PRINT("[GFL_USE] ResidentHeapSize =   %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", residentHeapSize, residentHeapAdrs, residentHeap, residentHeap->GetTotalAllocatableSize());
  GFL_PRINT("[GFL_USE] ResDeviceHeapSize =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", residentDeviceHeapSize, residentDeviceHeapAdrs, residentDeviceHeap, residentDeviceHeap->GetTotalAllocatableSize());  
}

void ResidentHeapFree()
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT ));  
}

#endif //defined(GF_PLATFORM_WIN32)

#if defined(GF_PLATFORM_CTR)

void HeapCreate(void)
{
  //-----------------------------------------------------------------------------
  /**
  *    ヒープの初期化
  */
  //-----------------------------------------------------------------------------
  size_t  systemHeapTotalSize = nn::os::GetHeapSize();
  size_t  systemHeapSize = (systemHeapTotalSize - System::HeapSize::RESIDENT);
  void*   systemHeapAdrs = reinterpret_cast<void*>(nn::os::GetHeapAddress());

  size_t  devieHeapTotalSize = nn::os::GetDeviceMemorySize();
  size_t  devieHeapSize = (devieHeapTotalSize - System::HeapSize::RESIDENT_DEVICE);
  void*   deviceHeapAdrs = reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress());

  gfl2::heap::Manager::Initialize( systemHeapAdrs, (int)systemHeapSize, HEAPID_CHILD_MAX, HEAPID_LOCAL_MAX );
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );

#if PM_DEBUG
  gfl2::heap::GflHeapBase::SetDebugFreeMemClear(gfl2::heap::GflHeapBase::DEBUG_FREE_MEMC_FF);
#endif  
  // for debug
  if ( gfl2::heap::Manager::IsExistHeapId( gfl2::heap::HEAPID_SYSTEM ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] SystemHeap = NULL!!\n");
  }
 
  gfl2::heap::HeapBase* deviceHeap = GFL_CREATE_HEAP_BUFFER( deviceHeapAdrs, HEAPID_DEVICE , devieHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEVICE ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] DeviceHeap = NULL!!\n");
  }

  GFL_PRINT("[GFL_USE] SystemHeapSize    =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", systemHeapSize, systemHeapAdrs, systemHeap, systemHeap->GetTotalAllocatableSize());
  GFL_PRINT("[GFL_USE] SysDeviceHeapSize =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", devieHeapSize, deviceHeapAdrs, deviceHeap, deviceHeap->GetTotalAllocatableSize() );
}

void HeapFree(void)
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM ));

  //-----------------------------------------------------------------------------
  /**
  *    ヒープの破棄
  */
  //-----------------------------------------------------------------------------
  gfl2::heap::Manager::Finalize();
}


//--------------------------------------------
/*
  HEAPID_RESIDENT, HEAPID_RESIDENT_DEVICEの初期化
  ソフトリセットで消すために、Systemとは初期化タイミングをずらす

  GflUseモジュール初期化のタイミングで呼び出す
*/
//--------------------------------------------
void ResidentHeapCreate(void)
{
  size_t  systemHeapTotalSize = nn::os::GetHeapSize();
  size_t  systemHeapSize = (systemHeapTotalSize - System::HeapSize::RESIDENT);
  size_t  devieHeapTotalSize = nn::os::GetDeviceMemorySize();
  size_t  devieHeapSize = (devieHeapTotalSize - System::HeapSize::RESIDENT_DEVICE);

  size_t  residentHeapSize = System::HeapSize::RESIDENT;
  void*   residentHeapAdrs = reinterpret_cast<void*>(nn::os::GetHeapAddress()+systemHeapSize);
  size_t  residentDeviceHeapSize = System::HeapSize::RESIDENT_DEVICE;
  void*   residentDeviceHeapAdrs = reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()+devieHeapSize);

  gfl2::heap::HeapBase* residentHeap = GFL_CREATE_HEAP_BUFFER( residentHeapAdrs, HEAPID_RESIDENT , residentHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_RESIDENT ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] ResidentHeap = NULL!!\n");
  }
  gfl2::heap::HeapBase* residentDeviceHeap = GFL_CREATE_HEAP_BUFFER( residentDeviceHeapAdrs, HEAPID_RESIDENT_DEVICE , residentDeviceHeapSize, gfl2::heap::HEAP_TYPE_EXP );
  if ( gfl2::heap::Manager::IsExistHeapId( HEAPID_RESIDENT_DEVICE ) == false )
  {
    GFL_ASSERT_STOP_MSG( 0, "[GFL_USE] ResidentDeviceHeap = NULL!!\n");
  }
  GFL_PRINT("[GFL_USE] ResidentHeapSize =   %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", residentHeapSize, residentHeapAdrs, residentHeap, residentHeap->GetTotalAllocatableSize());
  GFL_PRINT("[GFL_USE] ResDeviceHeapSize =  %08x, Adrs=%p, Handle=%p, Allocatable=%08x\n", residentDeviceHeapSize, residentDeviceHeapAdrs, residentDeviceHeap, residentDeviceHeap->GetTotalAllocatableSize());  
}

void ResidentHeapFree()
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE ));
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT ));  
}

#endif //defined(GF_PLATFORM_CTR)


} //namespace Main

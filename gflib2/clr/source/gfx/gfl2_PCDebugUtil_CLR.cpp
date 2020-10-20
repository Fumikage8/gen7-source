#include <clr/include/gfx/gfl2_PCDebugUtil_CLR.h>
#include <gfx/include/gfl2_PCDebugUtil.h>
#include <windows.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace gfx {

  static gfl2::heap::HeapBase* GetScreenShotHeap()
  {
    if (!gfl2::heap::Manager::IsExistHeapId(gfl2::heap::HEAPID_SYSTEM))
    {
      u32 bufSize = 0x6400000;
      void* pBuf = malloc(bufSize);
      gfl2::heap::Manager::Initialize(pBuf, bufSize, 1);
    }
    return gfl2::heap::Manager::GetDefaultNewUseHeap();
  }

  void PCDebugUtil::PrintScreen(System::String^ fileName, gfl2::clr::gfx::DisplayDeviceHandle^ hDisplay)
  {
    IntPtr ptr;
    const char* fileName_ = NULL;

    ptr = Marshal::StringToHGlobalAnsi( fileName );
    fileName_ = (const char*)ptr.ToPointer();

    gfl2::gfx::DisplayDeviceHandle hDisplay_ = hDisplay->GetNative();

    gfl2::heap::HeapBase* pHeap = GetScreenShotHeap();

    gfl2::gfx::PCDebugUtil::PrintScreen(fileName_, hDisplay_, pHeap);
  }

  void PCDebugUtil::PrintScreenAsTga(System::String^ fileName, gfl2::clr::gfx::DisplayDeviceHandle^ hDisplay)
  {
    IntPtr ptr;
    const char* fileName_ = NULL;

    ptr = Marshal::StringToHGlobalAnsi( fileName );
    fileName_ = (const char*)ptr.ToPointer();

    gfl2::gfx::DisplayDeviceHandle hDisplay_ = hDisplay->GetNative();

    gfl2::heap::HeapBase* pHeap = GetScreenShotHeap();

    gfl2::gfx::PCDebugUtil::PrintScreenAsTga(fileName_, hDisplay_, pHeap);
  }
}}}
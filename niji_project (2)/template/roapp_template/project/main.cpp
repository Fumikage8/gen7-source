//===================================================================
/**
 * @file    main.cpp
 * @brief   アプリケーション サンプルスケルトン
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================



#ifdef GF_PLATFORM_CTR

#include <nn/fnd.h>
#include <nn/os.h>
#include <nn/fs.h>
#include <nn/ro.h>
#include <nn/applet.h>
#include <nn/dbg.h>

//----------------------------------------------------------------------------
/**
 *  nnMain()
 */
//----------------------------------------------------------------------------
extern "C" void nnMain()
{
#if 0
  // ホームメニューからも実行できるよう nn::applet::Enable() のみ呼ぶ
  // ホームメニュー遷移や電源ボタンクリック、スリープには非対応
  nn::applet::Enable();

  NN_LOG("App: nnMain\n");
  NN_LOG("App: initialize\n");

  nn::fs::Initialize();

  const size_t ROMFS_BUFFER_SIZE = 1024 * 64;
  static char buffer[ROMFS_BUFFER_SIZE];
  NN_UTIL_PANIC_IF_FAILED(
    nn::fs::MountRom(16, 16, buffer, ROMFS_BUFFER_SIZE));

  nn::fs::FileInputStream f;

  nn::os::MemoryBlock mb;
  mb.Initialize(8 * 1024 * 1024);
  bit8* pHeap = reinterpret_cast<bit8*>(mb.GetAddress());

  NN_LOG("App: load crs\n");
  s64 crsSize;
  {
    f.Initialize(L"rom:/static.crs");
    crsSize = f.GetSize();
    f.Read(pHeap, crsSize);
    f.Finalize();
  }
  void* pRs = pHeap;
  pHeap += nn::math::RoundUp(crsSize, 0x1000);

  NN_LOG("App: load crr\n");
  s64 crrSize;
  {
     f.Initialize(L"rom:/.crr/static.crr");
     crrSize = f.GetSize();
     f.Read(pHeap, crrSize);
     f.Finalize();
  }
  void* pRr = pHeap;
  pHeap += crrSize;
#if 0
  NN_LOG("App: load Module\n", pHeap);
  s64 cro1Size;
  {
     f.Initialize(L"rom:/Module.cro");
     cro1Size = f.GetSize();
     f.Read(pHeap, cro1Size);
     f.Finalize();
  }
  void* pRo1 = pHeap;
  pHeap += nn::math::RoundUp(cro1Size, 0x1000);
  nn::ro::SizeInfo sizeInfo1;
  nn::ro::GetSizeInfo(&sizeInfo1, pRo1);
  void* pBuffer1 = pHeap;
  pHeap += nn::math::RoundUp(sizeInfo1.bufferSize, 0x1000);
#endif

  NN_LOG("App:    ro initialize\n");
  nn::ro::Initialize(pRs, crsSize);

  NN_LOG("App:    register crr\n");
  nn::ro::RegistrationList* pRl = nn::ro::RegisterList(pRr, crrSize);
  NN_NULL_ASSERT(pRl);
  #if 0
  {
    NN_LOG("App:    ro load\n");
    nn::ro::Module* pModule1 = nn::ro::LoadModule(pRo1, cro1Size, pBuffer1, sizeInfo1.bufferSize);
    NN_NULL_ASSERT(pModule1);

    NN_LOG("App:    ro initialize\n");
    pModule1->DoInitialize();

    NN_LOG("App:    call\n");
    Test();

    NN_LOG("App:    ro unload Module\n");
    pModule1->DoFinalize();
    pModule1->Unload();
  }
  #endif

  NN_LOG("App:    unregister crr\n");
  pRl->Unregister();
  NN_LOG("App:    ro finalize\n");
  nn::ro::Finalize();

  NN_LOG("App: end\n");
#endif
}

#endif //#ifdef GF_PLATFORM_CTR

#ifdef GF_PLATFORM_WIN32

#include <stdio.h>

extern "C" void main()
{

}

#endif //GF_PLATFORM_WIN32




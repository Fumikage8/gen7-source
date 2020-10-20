#if PM_DEBUG

#if defined(GF_PLATFORM_CTR)

#include "System/include/RomProtect.h"

#include <GameSys/include/GameManager.h>
#include <base/include/gfl2_Singleton.h>


namespace System{
  const char* RomProtect::FILE_PATH = "sdmc:/20150610.lzh";

  bool RomProtect::ExistTheFile(gfl2::heap::HeapBase* pHeap)
  {
    nn::fs::FileInputStream *fs = GFL_NEW(pHeap) nn::fs::FileInputStream();
    nn::Result result = fs->TryInitialize( FILE_PATH );
    GFL_DELETE fs;
    return result.IsSuccess();
  }

  void RomProtect::KeyCheckLoop(gfl2::heap::HeapBase* pHeap)
  {
    nn::os::TargetHardware hardware = nn::os::GetRunningTargetHardware();
    
    if(hardware != nn::os::TARGET_HARDWARE_PRODUCT && hardware != nn::os::TARGET_HARDWARE_SNAKE_PRODUCT)
    {
      //開発実機で無い場合は何もしない
      return;
    }

#if GFL_FS_SDMC_ENABLE
    GFL_PRINT("File Check.\n");
    if(this->ExistTheFile(pHeap) == false)
    {
      GFL_PRINT("No File. Good bye.\n");
      while(1){} // 無限ループ
    }
#endif
    gfl2::ui::DeviceManager* pUiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    gfl2::ui::Button* pButton = pUiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

    nn::fnd::DateTime startTime;
    bool pressed = false;

    u32 errorCount = 0;
    u32 prevKey = 0;
        
    while (true)
    {
      if (pressed) // 指定のボタンが押されている
      {
#if GFL_FS_SDMC_ENABLE
        nn::fnd::TimeSpan time = nn::fnd::DateTime::GetNow() - startTime;
#endif
        if(time.GetSeconds() >= DOWN_SECOND)
        {
          break;
        }
      }

      if (errorCount >= MISS_COUNT) // 失敗回数が一定数を超えたらさようなら
      {
        // ファイル削除
        nn::fs::TryDeleteFile(FILE_PATH);

        GFL_PRINT("Good bye.\n");
        while(1){} // 無限ループ
      }

      u32 key = pButton->GetHold();
      if( key ==  CORRECT_KEY )
      {
        if (pressed == false)
        {
          startTime = nn::fnd::DateTime::GetNow();
          pressed = true;
        }
      }
      else
      {
        pressed = false;
        if (key != 0)
        {
          if (prevKey != key)
          {
            prevKey = key;
            ++errorCount;
            GFL_PRINT("Error Count %d\n", errorCount);
          }
        }
      }

      pUiDeviceManager->Main();
    }
  }

} // System


#endif // GF_PLATFORM_CTR

#endif // PM_DEBUG
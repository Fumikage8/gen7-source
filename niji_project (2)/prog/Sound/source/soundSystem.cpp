#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/SoundLocal.h"

#include "Sound/include/HioSoundEditor.h"

namespace Sound {


  PMSoundSystem* s_PMSoundSystem = NULL; 

#if PM_DEBUG
  PMSoundSystem* GetPMSoundSystem( void )
  {
    return s_PMSoundSystem;
  }
#endif


  //---------------------------------------------------------------------------
  /**
   * @brief サウンド準備
   * @param heap         サウンドシステムに使用するヒープ
   * @param device_heap  サウンドシステムに使用するデバイスヒープ
   */
  //---------------------------------------------------------------------------
  void Initialize_SYSTEM( 
      gfl2::heap::HeapBase* heap,
      gfl2::heap::HeapBase* device_heap )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_PMSoundSystem == NULL )
    {
      s_PMSoundSystem = GFL_NEW( heap ) Sound::PMSoundSystem( heap, device_heap );
    } 
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief サウンド後片付け
   */
  //---------------------------------------------------------------------------
  void Finalize_SYSTEM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    GFL_SAFE_DELETE( s_PMSoundSystem );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief サウンド更新
   */
  //---------------------------------------------------------------------------
  void Update_SYSTEM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->Update();
  }


}

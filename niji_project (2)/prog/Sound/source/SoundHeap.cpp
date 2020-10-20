//=============================================================================
/**
 * @brief 
 * @file sound_HEAP.cpp
 * @author obata
 * @date 2012.02.21
 */
//=============================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/SELevelManager.h"

namespace Sound {


  enum SELevel
  {
    SELV_NONE,        // SEデータなし
    SELV_SYS_GLOBAL,  // システムの常駐SEあり
    SELV_FIELD_GLOBAL,// フィールドの常駐SEあり
    SELV_APP_GLOBAL,  // アプリケーションの常駐SEあり
  };

  //---------------------------------------------------------------------------
  /**
   * @brief SEのサウンドデータをヒープに積む
   * @param soundID  読み込むサウンドデータIDの中間ID
   */
  //---------------------------------------------------------------------------
  void LoadAndPushSESoundData( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->LoadAndPushSESoundData( soundID );
  } 
  // 非同期版
  b32 LoadAndPushSESoundDataAsync( gfl2::heap::HeapBase* thread_heap, u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->LoadAndPushSESoundDataAsync( thread_heap, soundID );
  }
  b32 WaitLoadAndPushSESoundDataAsync(void)
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->WaitLoadAndPushSESoundDataAsync();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 1階層下にあるSEのサウンドデータをヒープから削除する
   * @param soundID  削除するサウンドデータIDの中間ID
   */
  //---------------------------------------------------------------------------
  void PopAndUnloadSESoundData( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->PopAndUnloadSESoundData( soundID );
  } 
  
  //---------------------------------------------------------------------------
  /**
   * @brief 全階層にあるSEのサウンドデータをヒープから削除する
   *
   * @note  BTS4850 SoftwareResetから呼ばれるSoundSystemのFinalize用
   */
  //---------------------------------------------------------------------------
  void PopAndUnloadSESoundDataAll( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->PopAndUnloadSESoundDataAll();
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief アプリケーション常駐のサウンドを積む
   */
  //---------------------------------------------------------------------------
  void LoadAndPushAppGlobalSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    const SELevelManager* level_manager = s_PMSoundSystem->GetSELevelManager();
    u32 current_level = level_manager->GetCurrentLevel();
    GFL_ASSERT_STOP( current_level == SELV_FIELD_GLOBAL );

    // アプリ常駐SEを積む
    if( current_level == SELV_FIELD_GLOBAL ) {
      s_PMSoundSystem->LoadAndPushSESoundData( GROUP_APP_GLOBAL );
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief フィールドの常駐SEをヒープから削除する
   */
  //---------------------------------------------------------------------------
  void PopAndUnloadAppGlobalSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    const SELevelManager* level_manager = s_PMSoundSystem->GetSELevelManager();
    u32 current_level = level_manager->GetCurrentLevel();

    // アプリ常駐SEを削除
    if( current_level == SELV_APP_GLOBAL ) {
      s_PMSoundSystem->PopAndUnloadSESoundData( GROUP_APP_GLOBAL );
    }
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief バトルの常駐SEをヒープに積む
   */
  //---------------------------------------------------------------------------
  void LoadAndPushBattleGlobalSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    const SELevelManager* level_manager = s_PMSoundSystem->GetSELevelManager();
    u32 current_level = level_manager->GetCurrentLevel();

    // バトル常駐SEを積む
    if( current_level == SELV_APP_GLOBAL ) {
      s_PMSoundSystem->PopAndUnloadSESoundData( GROUP_APP_GLOBAL );
      s_PMSoundSystem->LoadAndPushSESoundData( GROUP_BATTLE_GLOBAL );
    }else if( current_level == SELV_FIELD_GLOBAL ){
      s_PMSoundSystem->LoadAndPushSESoundData( GROUP_BATTLE_GLOBAL );
    }
    else{
      GFL_ASSERT_STOP( (current_level == SELV_APP_GLOBAL) || (current_level == SELV_FIELD_GLOBAL) );
    }
  }

  void LoadAndPushBattleGlobalSEAsync( gfl2::heap::HeapBase * pThreadHeap )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    const SELevelManager* level_manager = s_PMSoundSystem->GetSELevelManager();
    u32 current_level = level_manager->GetCurrentLevel();

    // バトル常駐SEを積む
    if( current_level == SELV_APP_GLOBAL ) {
      s_PMSoundSystem->PopAndUnloadSESoundData( GROUP_APP_GLOBAL );
      //s_PMSoundSystem->LoadAndPushSESoundData( GROUP_BATTLE_GLOBAL );
      s_PMSoundSystem->LoadAndPushSESoundDataAsync( pThreadHeap, GROUP_BATTLE_GLOBAL );
    }else if( current_level == SELV_FIELD_GLOBAL ){
     // s_PMSoundSystem->LoadAndPushSESoundData( GROUP_BATTLE_GLOBAL );
       s_PMSoundSystem->LoadAndPushSESoundDataAsync( pThreadHeap, GROUP_BATTLE_GLOBAL );
    }
    else{
      GFL_ASSERT_STOP( (current_level == SELV_APP_GLOBAL) || (current_level == SELV_FIELD_GLOBAL) );
    }
  }
  b32 WaitLoadAndPushBattleGlobalSEAsync(void)
  {
    return WaitLoadAndPushSESoundDataAsync();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief バトルの常駐SEをヒープから削除する
   */
  //---------------------------------------------------------------------------
  void PopAndUnloadBattleGlobalSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    const SELevelManager* level_manager = s_PMSoundSystem->GetSELevelManager();
    u32 current_level = level_manager->GetCurrentLevel();
    GFL_ASSERT_STOP( current_level == SELV_APP_GLOBAL );

    // バトル常駐SEを破棄する
    if( current_level == SELV_APP_GLOBAL ) {
      s_PMSoundSystem->PopAndUnloadSESoundData( GROUP_BATTLE_GLOBAL );
      s_PMSoundSystem->LoadAndPushSESoundData( GROUP_APP_GLOBAL );
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 外部管理のサウンドヒープにサウンドデータを読み込む
   * @param sound_heap サウンドデータを配置するサウンドヒープ
   * @param soundID 読む込むサウンドデータの中間ID
   * @return 読み込みに成功したか？
   */
  //---------------------------------------------------------------------------
  b32 LoadSoundDataToExternalHeap( gfl2::sound::SoundHeap* sound_heap, u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->LoadSoundDataToExternalHeap( sound_heap, soundID );
  }


} // namespace Sound

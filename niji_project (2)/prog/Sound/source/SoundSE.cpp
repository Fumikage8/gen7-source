//=============================================================================
/**
 * @brief 
 * @file sound_SE.cpp
 * @author obata
 * @date 2011.09.15
 */
//=============================================================================

#include <debug/include/gfl2_DebugPrint.h>

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/EnvSEManager.h"

namespace Sound {

  s32 s_SEControlID[ PMSoundSystem::SE_HANDLE_NUM ]; 
  b32 s_IsInitializedSE = false;
  u8 GetUnusedSoundHandleIndex( void );
  void PlaySECore( u8, u32, u32, s32, u32 );
  PMSoundSystem::HandleIndex GetSoundHandle( u8 );
  b32 IsControlTarget( u8, u32, s32 );

  //---------------------------------------------------------------------------
  /**
   * @brief 使用準備
   * @param heap         
   * @param device_heap  
   */
  //---------------------------------------------------------------------------
  void Initialize_SE( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap )
  {
    SOUND_UPDATE_CRITICALSECTION;

    GFL_UNUSED(heap);
    GFL_UNUSED(device_heap);
    DEBUG_SOUND_OFF_RETNONE;
    for( u32 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) {
      s_SEControlID[i] = NOT_CONTROL;
    }
    s_IsInitializedSE = true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 後片付け
   */
  //---------------------------------------------------------------------------
  void Finalize_SE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_IsInitializedSE = false;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEを再生する
   * @param soundID       再生するSE
   * @param fadein_frame  フェードインにかけるフレーム数
   * @param controlID     SEの管理ID
   * @param playerID      再生に使用するプレイヤーのID
   *
   * @note
   * 同じSEが複数同時に再生される && それぞれのSEを識別して操作したい 場合は
   * controlID を指定してください.
   * 以降, soundID と controlID の組み合わせで識別します.
   * 
   * @note 
   * playerID に DEFAULT_PLAYER を指定した場合,
   * サウンドデータに設定されたプレイヤーで再生します.
   */
  //---------------------------------------------------------------------------
  void PlaySE( u32 soundID, u32 fadein_frame, s32 controlID, u32 playerID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    u8 handle_index = GetUnusedSoundHandleIndex();
    PlaySECore( handle_index, soundID, fadein_frame, controlID, playerID );
  }

  /**
   * @brief 使用中でないSE用サウンドハンドルのインデックスを取得する
   * @note 空きがない場合は 0 を返す.
   */
  u8 GetUnusedSoundHandleIndex( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return 0; // 初期化チェック

    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
      if( !s_PMSoundSystem->IsSEPlaying( handle ) ) {
        return i;
      }
    }

//    PMSoundSystem::HandleIndex handle = GetSoundHandle( 0 );
    GFL_PRINT( "WARNING!! SEの制御権を失いました( handle index =0 )\n" );
    return 0;
  }

  /**
   * @brief サウンドハンドルを取得する
   * @param index  SEハンドルのインデックス
   */
  PMSoundSystem::HandleIndex GetSoundHandle( u8 index )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return static_cast<PMSoundSystem::HandleIndex>( PMSoundSystem::SHANDLE_SE_00 ); // 初期化チェック

    if( PMSoundSystem::SE_HANDLE_NUM <= index ) {
      GFL_ASSERT(0);
      index = 0;
    }
    return static_cast<PMSoundSystem::HandleIndex>( PMSoundSystem::SHANDLE_SE_00 + index );
  } 

  /**
   * @brief SE再生処理
   * @param handle_index  使用するSEハンドルのインデックス
   * @param soundID       再生するSE
   * @param fadein_frame  フェードインにかけるフレーム数
   * @param controlID     SEの管理ID
   * @param playerID      再生に使用するプレイヤーのID
   */
  void PlaySECore( 
      u8 handle_index, u32 soundID, u32 fadein_frame, s32 controlID, u32 playerID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    GFL_ASSERT( handle_index < PMSoundSystem::SE_HANDLE_NUM ); 
    PMSoundSystem::HandleIndex handle = GetSoundHandle( handle_index );
    if( playerID == DEFAULT_PLAYER ) {
      s_PMSoundSystem->PlaySE( handle, soundID, fadein_frame );
      s_SEControlID[ handle_index ] = controlID;
    }
    else {
      nw::snd::SoundStartable::StartInfo info;
      info.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
      info.playerId = playerID;
      s_PMSoundSystem->PlaySE( handle, soundID, fadein_frame, &info );
      s_SEControlID[ handle_index ] = controlID;
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 詳細なパラメータを指定してSEを再生する
   * @param soundID       再生するSE
   * @param info       SEに適用する再生パラメータ
   * @param controlID  SEの管理ID
   * @param playerID   再生に使用するプレイヤーのID
   *
   * @note
   * この関数から返った後は info を破棄しても大丈夫です.
   */
  //---------------------------------------------------------------------------
  void PlaySE( 
      u32 soundID, const PlayInfo& info, s32 controlID, u32 playerID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    PlaySE( soundID, info.fadein_frame, controlID, playerID );

    if( info.effect_enable & EFFECT_BIT_VOLUME ) {
      ChangeSEVolume( soundID, info.volume, info.change_volume_frame, controlID );        
    }

    if( info.effect_enable & EFFECT_BIT_PITCH ) {
      ChangeSEPitch( soundID, info.pitch, controlID );
    }

    if( info.effect_enable & EFFECT_BIT_PAN ) {
      ChangeSEPan( soundID, info.pan, controlID );
    }

    if( info.effect_enable & EFFECT_BIT_SURROUND_PAN ) {
      ChangeSESurroundPan( soundID, info.surround_pan, controlID );
    }

    if( info.effect_enable & EFFECT_BIT_TEMPO ) {
      ChangeSETempo( soundID, info.tempo, controlID );
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEが再生中かどうかを調べる
   * @param soundID       チェックするSE
   * @param controlID  SEの管理ID
   * @retval true   再生中
   * @retval false  再生していない
   *
   * @note 
   * ( 終わることのない )ループ音を無限に待つのを防ぐために soundID を指定します.
   */
  //---------------------------------------------------------------------------
  b32 IsSEPlaying( u32 soundID, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return false; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief 操作対象のハンドルかどうかを調べる
   * @param handle_index  チェックするSE用ハンドルのインデックス
   * @param soundID       操作対象のSE
   * @param controlID     操作対象の管理ID
   */
  b32 IsControlTarget( u8 handle_index, u32 soundID, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return false; // 初期化チェック
    if( ( s_SEControlID[ handle_index ] != NOT_CONTROL ) && 
        ( s_SEControlID[ handle_index ] != controlID ) ) {
      return false;
    }
    PMSoundSystem::HandleIndex handle = GetSoundHandle( handle_index );
    return s_PMSoundSystem->IsSEPlaying( handle, soundID );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEの再生が終了したかどうかを調べる
   * @param soundID       チェックするSE
   * @param controlID  SEの管理ID
   * @retval true   終了した or 再生していない
   * @retval false  再生中
   *
   * @note 
   * ( 終わることのない )ループ音を無限に待つのを防ぐために soundID を指定します.
   */
  //---------------------------------------------------------------------------
  b32 IsSEFinished( u32 soundID, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    return !IsSEPlaying( soundID, controlID );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEを停止する
   * @param soundID        停止するSE
   * @parma fadeout_frame  フェードアウトにかけるフレーム数
   * @param controlID      SEの管理ID
   */
  //---------------------------------------------------------------------------
  void StopSE( u32 soundID, u32 fadeout_frame, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->StopSE( handle, fadeout_frame );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 全てのSEを停止する
   */
  //---------------------------------------------------------------------------
  void StopAllSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    s_PMSoundSystem->StopAllSE();
    ResetEnvSE(); //環境音も止める
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 環境音の再生 
   */
  //---------------------------------------------------------------------------
  void PlayEnvSE( u32 soundID, u32 fadein_frame, f32 volume )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
 
    if( s_PMSoundSystem->GetEnvSEManager()->Entry( soundID, volume ) ){
      PlayInfo  info( EFFECT_BIT_VOLUME, fadein_frame );
      info.volume = volume;

      PlaySE( soundID, info );
    }
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 環境音のボリューム操作
   */
  //---------------------------------------------------------------------------
  void ChangeEnvSEVolume( u32 soundID, f32 volume, u32 change_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    
    if( s_PMSoundSystem->GetEnvSEManager()->ChangeVolume( soundID, volume ) ){
      ChangeSEVolume( soundID, volume, change_frame );        
    }
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 環境音を止める
   */
  //---------------------------------------------------------------------------
  void StopEnvSE( u32 soundID, u32 fadeout_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    
    s_PMSoundSystem->GetEnvSEManager()->Release( soundID );
  
    if(IsSEPlaying( soundID )){
      Sound::StopSE( soundID, fadeout_frame );
    }
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 全ての環境音を止める(ポーズ等のステータスはリセットしない)
   */
  //---------------------------------------------------------------------------
  void StopEnvSEAll( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    
    StopPlayer( PLAYER_SE_ENV, BGM_FADE_VERY_FAST );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief すべての環境音を止めてステータスもリセットする
   */
  //---------------------------------------------------------------------------
  void ResetEnvSE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    
    s_PMSoundSystem->GetEnvSEManager()->ResetAll();
    StopPlayer( PLAYER_SE_ENV, BGM_FADE_VERY_FAST );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 環境音をポーズする
   */
  //---------------------------------------------------------------------------
  void PauseEnvSE( EnvSEPauseState state )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック

    s_PMSoundSystem->GetEnvSEManager()->SetPause(state);
    StopPlayer( PLAYER_SE_ENV, BGM_FADE_VERY_FAST );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 環境音のポーズを解除する
   */
  //---------------------------------------------------------------------------
  void ReplayEnvSE( EnvSEPauseState state )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    
    {
      EnvSEManager::PlayState replay[EnvSEManager::MAX_PLAY_NUM];
      int ct = s_PMSoundSystem->GetEnvSEManager()->ResetPause(state, replay);

      for( int i = 0; i < ct; ++i ){
        PlayInfo  info( EFFECT_BIT_VOLUME, BGM_FADE_MIDDLE );
        info.volume = replay[i].volume;
        PlaySE( replay[i].soundItemID, info );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEの音量を変更する
   * @param soundID       音量を変更するSE
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   * @param controlID     SEの管理ID
   */
  //---------------------------------------------------------------------------
  void ChangeSEVolume( 
      u32 soundID, f32 volume, u32 change_frame, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSEVolume( handle, volume, change_frame );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのピッチを変更する
   * @param soundID    ピッチを変更するSE
   * @param value      周波数比率
   * @param controlID  SEの管理ID
   * @note 
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //---------------------------------------------------------------------------
  void ChangeSEPitch( u32 soundID, f32 value, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSEPitch( handle, value );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEの水平方向パンを変更する
   * @param soundID       パンを変更するSE
   * @param value      パン
   * @param controlID  SEの管理ID
   * @note 
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeSEPan( u32 soundID, f32 value, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSEPan( handle, value );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEの垂直方向パンを変更する
   * @param soundID       パンを変更するSE
   * @param value      パン
   * @param controlID  SEの管理ID
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeSESurroundPan( u32 soundID, f32 value, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSESurroundPan( handle, value );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのテンポを変更する
   * @param soundID       テンポを変更するSE
   * @param value      テンポを倍率で指定( 1.0fで元に戻る )
   * @param controlID  SEの管理ID
   */
  //---------------------------------------------------------------------------
  void ChangeSETempo( u32 soundID, f32 value, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSETempoRatio( handle, value );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのトランスポーズを変更する
   * @param soundID       テンポを変更するSE
   * @param value      テンポを倍率で指定( 1.0fで元に戻る )
   * @param controlID  SEの管理ID
   */
  //---------------------------------------------------------------------------
  void ChangeSETranspose( u32 soundID, s8 transposeVal, s32 controlID, u32 transBit)
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return ; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) 
    {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        s_PMSoundSystem->ChangeSETranspose( handle, transposeVal, transBit );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのトラック変数を変更する
   * @param soundID  トラック変数を変更するSE
   * @param trackNo  トラックNo
   * @param varNo    変数番号(0-15)
   * @param value    値
   * @param controlID     SEの管理ID
   */
  //---------------------------------------------------------------------------
  void ChangeSETrackVariable( 
      u32 soundID, int trackNo, int varNo, s16 value, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        return s_PMSoundSystem->ChangeSETrackVariable( handle, trackNo, varNo, value );
      }
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのトラック変数を取得する
   * @param soundID  トラック変数を変更するSE
   * @param trackNo  トラックNo
   * @param varNo    変数番号(0-15)
   * @param controlID     SEの管理ID
   * @retval value    書き込まれた変数値
   */
  //---------------------------------------------------------------------------
  s16 GetSETrackVariable( 
      u32 soundID, int trackNo, int varNo, s32 controlID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    if(!s_IsInitializedSE) return 0; // 初期化チェック
    for( u8 i=0; i<PMSoundSystem::SE_HANDLE_NUM; i++ ) {
      if( IsControlTarget( i, soundID, controlID ) ) {
        PMSoundSystem::HandleIndex handle = GetSoundHandle( i );
        return s_PMSoundSystem->GetSETrackVariable( handle, trackNo, varNo );
      }
    }
    return 0;
  }


} // namespace Sound

//=============================================================================
/**
 * @brief  サウンドプレイヤ操作関数
 * @file   sound_PLAYER.cpp
 * @author obata
 * @date   2012.02.18
 */
//=============================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"

namespace Sound {

  // プレイヤのフェード情報
  struct FadeStatus
  {
    u32 playerID;      // 操作対象のプレイヤID
    b32 isRunning;    // フェード中か？
    u32 currentFrame;  // フェードフレームカウンタ
    u32 endFrame;      // フェード終了フレーム
    f32 startVolume;   // フェード開始時の音量
    f32 endVolume;     // フェード終了時の音量
  };

  FadeStatus s_BGMPlayerFadeStatus[ SND_PLAYER_IDX_MAX ];

  void UpdatePlayerVolumeFade( FadeStatus* );


  //---------------------------------------------------------------------------
  /**
   * @brief 使用準備
   */
  //---------------------------------------------------------------------------
  void Initialize_PLAYER( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    for( u32 i=0; i<SND_PLAYER_IDX_MAX; i++ )
    {
      s_BGMPlayerFadeStatus[i].isRunning    = false;
      s_BGMPlayerFadeStatus[i].currentFrame = 0;
      s_BGMPlayerFadeStatus[i].endFrame     = 0;
      s_BGMPlayerFadeStatus[i].startVolume  = 0.0f;
      s_BGMPlayerFadeStatus[i].endVolume    = 0.0f;
    }
    //enum SoundPlayerIndexの並び順と揃えておく
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_STRM].playerID      = PLAYER_STRM;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_BGM].playerID       = PLAYER_BGM;
    
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_1].playerID      = PLAYER_SE_1;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_2].playerID      = PLAYER_SE_2;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_3].playerID      = PLAYER_SE_3;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_4].playerID      = PLAYER_SE_4;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_ENV].playerID    = PLAYER_SE_ENV;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_FT].playerID     = PLAYER_SE_FT;
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_GLOBAL].playerID = PLAYER_SE_GLOBAL;
    
    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_PV].playerID        = PLAYER_PV;

    s_BGMPlayerFadeStatus[SND_PLAYER_IDX_SE_WEATHER].playerID = PLAYER_SE_WEATHER;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 後片付け
   */
  //---------------------------------------------------------------------------
  void Finalize_PLAYER( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 更新処理
   */
  //---------------------------------------------------------------------------
  void Update_PLAYER( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    for( u32 i=0; i<SND_PLAYER_IDX_MAX; i++ )
    {
      UpdatePlayerVolumeFade( &s_BGMPlayerFadeStatus[i] );
    }
  }

  /**
   * @brief プレイヤのボリュームフェードを更新する
   * @param fadeStatus 更新するフェード情報
   */
  void UpdatePlayerVolumeFade( FadeStatus* fadeStatus )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( !fadeStatus->isRunning ) {
      return;
    }

    if( fadeStatus->currentFrame < fadeStatus->endFrame )
    {
      fadeStatus->currentFrame++;
      f32 ratio = static_cast<f32>(fadeStatus->currentFrame) / static_cast<f32>(fadeStatus->endFrame);
      f32 v0 = fadeStatus->startVolume;
      f32 v1 = fadeStatus->endVolume;
      f32 volume = v0 + ( ( v1 - v0 ) * ratio );
      SetPlayerVolume( fadeStatus->playerID, volume );
    }

    if( fadeStatus->endFrame <= fadeStatus->currentFrame )
    {
      fadeStatus->isRunning = false;
    }
  }


  //---------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中のサウンドの個数を取得する
   * @param middleID プレイヤを表す中間ID
   */
  //---------------------------------------------------------------------------
  u32 GetPlayingCount( u32 middleID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0);
    return s_PMSoundSystem->GetPlayerPlayingCount( middleID );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーの音量を取得する
   * @param middleID プレイヤを表す中間ID
   */
  //---------------------------------------------------------------------------
  f32 GetPlayerVolume( u32 middleID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0.0f);
    return s_PMSoundSystem->GetPlayerVolume( middleID );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief プレイヤボリュームを変更する
   * @param middleID プレイヤを表す中間ID
   * @param volume   音量の倍率
   */
  //---------------------------------------------------------------------------
  void SetPlayerVolume( u32 middleID, f32 volume )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetPlayerVolume( middleID, volume );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中の全てのサウンドを停止する
   * @param middleID       プレイヤを表す中間ID
   * @param fade_out_frame フェードアウトにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void StopPlayer( u32 middleID, u32 fade_out_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->StopPlayerSound( middleID, fade_out_frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中の全てのサウンドを一時停止する
   * @param middleID   プレイヤを表す中間ID
   * @param fade_frame フェードアウトにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void PausePlayer( u32 middleID, u32 fade_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->PausePlayerSound( middleID, true, fade_frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーの一時停止を解除する
   * @param middleID   プレイヤを表す中間ID
   * @param fade_frame フェードアウトにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ReleasePlayer( u32 middleID, u32 fade_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->PausePlayerSound( middleID, false, fade_frame );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 指定Indexのサウンドプレイヤーのボリュームを変更する
   * @param volume 音量の倍率
   * @param frame  音量変化にかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ChangePlayerVolume( SoundPlayerIndex playerIdx, f32 volume, u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    u32 playerID = s_BGMPlayerFadeStatus[playerIdx].playerID;
    if( frame == 0 )
    {
      SetPlayerVolume( playerID, volume );
      s_BGMPlayerFadeStatus[playerIdx].isRunning = false;
    }
    else
    {
      s_BGMPlayerFadeStatus[playerIdx].isRunning    = true;
      s_BGMPlayerFadeStatus[playerIdx].currentFrame = 0;
      s_BGMPlayerFadeStatus[playerIdx].endFrame     = frame;
      s_BGMPlayerFadeStatus[playerIdx].startVolume  = GetPlayerVolume( playerID );
      s_BGMPlayerFadeStatus[playerIdx].endVolume    = volume;
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief　指定プレイヤーにフェードリクエストがあるか？
   * 
   * @fix GFNMCat[5590]反転世界イベント終了後にアサート
   * 環境音ボリュームチェンジリセットをフレーム指定で行うと、フリームーブ時のチェックにひっかっかる
   */
  //---------------------------------------------------------------------------
  bool IsRunningPlayerVolumeChange( SoundPlayerIndex playerIdx )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_BGMPlayerFadeStatus[playerIdx].isRunning;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMのプレイヤボリュームを変更する
   * @param volume 音量の倍率
   * @param frame  音量変化にかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ChangeBGMPlayerVolume( f32 volume, u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    ChangePlayerVolume( SND_PLAYER_IDX_STRM, volume, frame );
    ChangePlayerVolume( SND_PLAYER_IDX_BGM, volume, frame );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief BGMとSEのプレイヤボリュームを変更する
   * @param volume 音量の倍率
   * @param frame  音量変化にかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ChangeAllPlayerVolume( f32 volume, u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    for( int i = 0;i < SND_PLAYER_IDX_MAX; i++ ){
      ChangePlayerVolume( static_cast<SoundPlayerIndex>(i), volume, frame );
    }
  }

} // namespace Sound 

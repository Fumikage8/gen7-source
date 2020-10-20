//=============================================================================
/**
 * @brief 
 * @file  sound_BGM.cpp
 * @author obata
 * @date   2011.06.29
 */
//=============================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/BGMManager.h"
#include "Sound/include/PMSoundSystem.h"

namespace Sound { 

  /// BGM退避数
  enum {
    PUSH_COUNT_NONE,    //< 退避なし
    PUSH_COUNT_BASE,    //< ベースBGM退避中(フィールドでのME、バトルBGM再生中など)
    PUSH_COUNT_BATTLE,  //< バトルBGM退避中(バトル中のMEなど)
    PUSH_COUNT_OVER,    //< 退避数オーバー
  };

  BGMManager* s_BGMManager = NULL;

  //---------------------------------------------------------------------------
  /**
   * @brief 使用準備
   * @param heap         
   * @param device_heap  
   */
  //---------------------------------------------------------------------------
  void Initialize_BGM( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    if( s_BGMManager == NULL )
    {
      s_BGMManager = GFL_NEW( heap ) BGMManager( heap, s_PMSoundSystem );
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 後片付け
   */
  //---------------------------------------------------------------------------
  void Finalize_BGM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    GFL_SAFE_DELETE( s_BGMManager );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 更新処理
   */
  //---------------------------------------------------------------------------
  void Update_BGM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_BGMManager->Update();
  }
  
  //=============================================================================
  // 基本のBGM再生〜停止関数
  //=============================================================================

  //---------------------------------------------------------------------------
  /**
   * @brief BGMを開始する
   * @param soundID       再生するBGMのID
   * @param fadein_frame  フェードインにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void StartBGMReq( u32 soundID, FadeFrame fadein_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_PREPARE_START( soundID );
    s_BGMManager->AddCommand_PREPARE_WAIT();
    s_BGMManager->AddCommand_PREPARE_PLAY();
    if( static_cast<int>(fadein_frame) > 0 ){
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( fadein_frame );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief BGMを開始する  StartBGMReq()に移行し削除予定
   * @param soundID       再生するBGMのID
   * @param fadein_frame  フェードインにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void StartBGM( u32 soundID, u32 fadein_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    StartBGMReq( soundID, static_cast<FadeFrame>(fadein_frame) );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @biref BGMを変更する
   * @param soundID  次のBGM
   */
  //---------------------------------------------------------------------------
  static void ChangeBGMReqCore( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame, u32 groupID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    BGM_PRINT( "BGMReq: ChangeBGMReqCore %08x %d", soundID, groupID);
    if( s_BGMManager->GetLastPlayedSoundID() != soundID ) 
    {
      BGM_PRINT( " Exec" );
      s_BGMManager->AddCommand_BLOCK_START( groupID, soundID );
      s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
      s_BGMManager->AddCommand_SKIP( soundID );
      s_BGMManager->AddCommand_PREPARE_START( soundID );
      s_BGMManager->AddCommand_PREPARE_WAIT();
      s_BGMManager->AddCommand_PREPARE_PLAY();
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_SKIP_END();
      s_BGMManager->AddCommand_SKIP( soundID );
      s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
      s_BGMManager->AddCommand_SKIP_END();
      s_BGMManager->AddCommand_BLOCK_END();
    }
    BGM_PRINT( "\n" );
  }
  void ChangeBGMReq( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    ChangeBGMReqCore( soundID, fadeoutFrame, fadeinFrame, BGMManager::COMMAND_BLOCK_GROUP_NONE );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードアウトしてポーズで止める
   *
   * @note  StopBGMとの差異は、StopBGMが完全にBGMを停止するのに対して
   *        EndBGMでは停止させずにポーズ(状態が保存されている)にとどめます
   *
   *        StopBGM()ではなく、BGMの停止には原則こちらを利用してください。
   *        EndBGMReq()の後でStartBGMReq()が使えます。
   *        基本、StopBGM()を利用してBGMを完全停止させる必要はありません。
   *
   *        PauseBGMとFadeOutBGMとほぼ同義ですが、違いはこちらがコマンド実行を一度キューに預けることです。
   *        PauseBGMやFadeOutBGMのようにポーズが即時実行されず、他の〜Req()系関数によって先にキューに
   *        積まれたコマンドを消化してから実行されます。
   */
  //---------------------------------------------------------------------------
  void EndBGMReq( FadeFrame fadeoutFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: EndBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードアウトさせる【リクエスト系 原則こちらを利用】
   * @param frame  フェードアウトにかけるフレーム数
   *
   * @note ノイズ防止のため、frameには 10frame = xy_snd::BGM_FADE_VERY_FAST以上の値を渡してください
   *       10frame未満の短いフレーム数でフェードアウトをかけると、ノイズが発生する場合があります
   */
  //---------------------------------------------------------------------------
  void FadeOutBGMReq( FadeFrame fadeoutFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: FadeOutBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードインさせる【リクエスト系 原則こちらを利用】
   * @param frame  フェードインにかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void FadeInBGMReq( FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: FadeInBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードアウトさせてPush【リクエスト系 原則こちらを利用】
   * @param frame  フェードアウトにかけるフレーム数
   *
   * @note ノイズ防止のため、frameには 10frame = xy_snd::BGM_FADE_VERY_FAST以上の値を渡してください
   *       10frame未満の短いフレーム数でフェードアウトをかけると、ノイズが発生する場合があります
   *
   *       PopBGMReq()と対
   */
  //---------------------------------------------------------------------------
  void PushBGMReq( FadeFrame fadeoutFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: PushBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_PUSH();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMをPopしてフェードインさせる【リクエスト系 原則こちらを利用】
   * @param frame  フェードインにかけるフレーム数
   *
   * @note  PushBGMReq()と対
   *        fadeinFrame = BGM_FADE_NONEを指定するとPopだけ行い再生復帰しないので注意
   */
  //---------------------------------------------------------------------------
  void PopBGMReq( FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: PopBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_POP( fadeinFrame );  //元の曲の途中から
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 元のBGMを鳴らさずにPopのみ行う【リクエスト系 原則こちらを利用】
   * @param frame  フェードインにかけるフレーム数
   *
   * @note  PushBGMReq()と対
   */
  //---------------------------------------------------------------------------
  void PopBGMOnlyReq(void)
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_POP( 0 );
    s_BGMManager->AddCommand_BLOCK_END();
  }
   
  //---------------------------------------------------------------------------
  /**
   * @brief BGMの非同期読み込みを開始する
   * @param soundID  非同期読み込みを行うBGMID
   * @param startMilliSec  再生開始位置(ミリ秒)
   * @note PreparePlayBGMReqと対で使ってください
   */
  //---------------------------------------------------------------------------
  void PrepareBGMReq( u32 soundID , u32 startMilliSec )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: PrepareBGMReq %08x\n", soundID );
    s_BGMManager->AddCommand_BLOCK_START();
		s_BGMManager->AddCommand_PREPARE_START( soundID , startMilliSec );
		s_BGMManager->AddCommand_PREPARE_WAIT();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの非同期読み込みを行ったBGMを再生する
   * @param fadeinFrame  デフォルトはNONE
   * @note PrepareBGMReqと対で使ってください
   */
  //---------------------------------------------------------------------------
  void PreparePlayBGMReq( FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: PreparePlayBGMReq \n" );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_PREPARE_PLAY();
    if( static_cast<int>(fadeinFrame) > 0 ){
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  }
 
  //=============================================================================
  // BGMの一時停止と再開 〜Req()系関数との併用時はコマンドが実行されるタイミングに気を付けてください
  //=============================================================================

  //---------------------------------------------------------------------------
  /**
   * @brief BGMを一時停止する
   * @param fadeout_frame  フェードアウトにかけるフレーム数
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              FadeOutReq()の中身がPauseBGMなのでそちらを用いること
   */
  //---------------------------------------------------------------------------
  void PauseBGM( u32 fadeout_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->PauseBGM( fadeout_frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 一時停止中のBGMを再開する
   * @param fadein_frame  フェードインにかけるフレーム数
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              FadeInReq()の中身がRestartBGMなのでそちらを用いること
   */
  //---------------------------------------------------------------------------
  void RestartBGM( u32 fadein_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->PauseReleaseBGM( fadein_frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードアウトさせる
   * @param frame  フェードアウトにかけるフレーム数
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              基本はFadeOutReq()を用いること
   */
  //---------------------------------------------------------------------------
  void FadeOutBGM( u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->PauseBGM( frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMをフェードインさせる
   * @param frame  フェードインにかけるフレーム数
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              基本はFadeInReq()を用いること
   */
  //---------------------------------------------------------------------------
  void FadeInBGM( u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->PauseReleaseBGM( frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMがフェード中か？
   * @retval true   フェード中
   * @retval false  フェード中でない
   */
  //--------------------------------------------------------------------------
  b32 IsBGMFade( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);

    return s_PMSoundSystem->IsBGMFade();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMが再生中かどうかを調べる
   * @retval true   再生している
   * @retval false  再生していない
   */
  //---------------------------------------------------------------------------
  b32 IsBGMPlaying( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);//タイトルが通るようにtrue
    return s_PMSoundSystem->IsBGMPlaying();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 任意のBGMが再生中かどうかを調べる
   * @param soundID  チェックするBGMの中間ID
   * @retval true   再生している
   * @retval false  再生していない
   */
  //---------------------------------------------------------------------------
  b32 IsBGMPlaying( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true); //タイトルが通るようにtrue
    return s_PMSoundSystem->IsBGMPlaying( soundID );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 処理が終了していないコマンドを持っているか？
   * @retval true  終了していないコマンドがある
   * @retval false 全てのコマンドが終了した
   */
  //---------------------------------------------------------------------------
  b32 HaveCommand( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_BGMManager->HaveCommand();
  }
  
  //=============================================================================
  // BGMのPushPopと完全停止。直接呼出しは原則禁止。利用したい場合は確認してください
  //
  // 〜Req()系関数との併用時はコマンドが実行されるタイミングに気を付けてください
  //=============================================================================

  //--------------------------------------------------------------------------
  /**
   * @brief 再生中のBGMを退避する
   * @retval true   退避に成功
   * @retval false  退避に失敗
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              基本はPushReq() を用いること
   */
  //--------------------------------------------------------------------------
  b32 PushBGM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->PushBGM();
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 退避したBGMを復元する
   * @param fadein_frame BGMのフェードインにかけるフレーム数
   * @retval true   復元に成功
   * @retval false  復元に失敗
   *
   * 原則利用禁止:どうしても利用したいケースではサウンド担当に確認
   *              基本はPopReq() or PopOnlyReq()を用いること
   */
  //--------------------------------------------------------------------------
  b32 PopBGM( u32 fadein_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->PopBGM( fadein_frame );
  }

  b32 PopBGMOnly( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    return s_PMSoundSystem->PopBGMOnly();
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの退避数を取得する
   *
   * @note  Reqが積まれて未消化の場合があるので、厳密に取得したい場合には
   *        xy_snd::HaveCommand()でコマンド消化を待ってから呼び出す必要があります
   */
  //--------------------------------------------------------------------------
  u32 GetPushedBGMCount( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(0);
    return s_PMSoundSystem->GetPushedBGMCount();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief BGMを完全停止する
   * @param fadeout_frame  フェードアウトにかけるフレーム数
   *
   * @note  特に理由がなければ、BGMの停止処理にはEndBGMReq()を利用してください
   *        この関数の利用箇所は最終的にリストアップします
   */
  //---------------------------------------------------------------------------
  void StopBGM( u32 fadeout_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    s_PMSoundSystem->StopBGM( fadeout_frame );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief 全てのBGMを停止する 注意コマンド!
   *
   * @note  すべてのBGMを強制的に終了。PushPop状態もリセットします。
   *        コマンドキューも状態に関わらずクリアされる特別仕様のため、
   *        安全な利用の為に、使用する前に一度ご相談ください。by iwasawa
   */
  //---------------------------------------------------------------------------
  void StopAllBGM( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    int push_count = s_PMSoundSystem->GetPushedBGMCount();
    GFL_PRINT("StopAllBGM pushed=%d\n",push_count);
    // BGM を停止
    while( 0 < push_count-- ) 
    {
      s_PMSoundSystem->StopBGM(0);
      s_PMSoundSystem->PopBGMOnly(); //BTS6242 Homeボタン押し状態で再生リクエストをかけてはいけない
    }
    s_PMSoundSystem->StopBGM(0);

    //ハンドルで止められなかった時のための保険
    s_PMSoundSystem->StopPlayerSound( PLAYER_STRM, 0 );
    s_PMSoundSystem->StopPlayerSound( PLAYER_BGM, 0 );

    // 管理情報をクリア
    s_BGMManager->ClearManageState();
  }
  
  //=============================================================================
  // BGMに対するエフェクト処理系
  // 〜Req()系関数との併用時はコマンドが実行されるタイミングに気を付けてください
  //=============================================================================

  //---------------------------------------------------------------------------
  /**
   * @brief BGMのテンポを変更する
   * @param tempo  テンポを倍率で指定( 1.0fで元に戻る )
   *
   * @attention 
   * シーケンスサウンドに対してのみ有効です.
   * シーケンスサウンド以外の場合, 何も変化しません.
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTempo( f32 tempo )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTempoRatio( tempo );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの音量を変更する
   * @param volume  音量の倍率( 1.0fで等倍 )
   * @param frame   音量変化にかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ChangeBGMVolume( f32 volume, u32 frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMVolume( volume, frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの音量を変更する
   * @param track_bit     対象トラックのビットフラグ
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTrackVolume( u32 track_bit, f32 volume, u32 change_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTrackVolume( track_bit, volume, change_frame );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMのピッチを変更する
   * @param value  周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //---------------------------------------------------------------------------
  void ChangeBGMPitch( f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMPitch( value );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMのピッチを12平均律に即した音程に変更する
   * @param level 12平均律のレベル
   * @note
   * level ==  2*12   2オクターブ高い音程
   * level ==  1*12   1オクターブ高い音程
   * level ==  0      通常の音程
   * level == -1*12  2オクターブ高い音程
   * level == -2*12  1オクターブ高い音程
   */
  //---------------------------------------------------------------------------
  void ChangeBGMPitchByEqualTemperament( s8 level )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMPitchByEqualTemperament( level );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックのピッチを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTrackPitch( u32 track_bit, f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTrackPitch( track_bit, value );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックのピッチを12平均律に即した音程に変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param level 12平均律のレベル
   * @note
   * level ==  2*12   2オクターブ高い音程
   * level ==  1*12   1オクターブ高い音程
   * level ==  0      通常の音程
   * level == -1*12  2オクターブ高い音程
   * level == -2*12  1オクターブ高い音程
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTrackPitchByEqualTemperament( u32 track_bit, s8 level )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTrackPitchByEqualTemperament( track_bit, level );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの水平方向パンを変更する
   * @param value  パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeBGMPan( f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMPan( value );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの水平方向パンを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTrackPan( u32 track_bit, f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTrackPan( track_bit, value );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの垂直方向パンを変更する
   * @param value  パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeBGMSurroundPan( f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMSurroundPan( value );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの垂直方向パンを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //---------------------------------------------------------------------------
  void ChangeBGMTrackSurroundPan( u32 track_bit, f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->ChangeBGMTrackSurroundPan( track_bit, value );
  }

  
  //=============================================================================
  // バトルBGM制御用関数群
  //=============================================================================

  //---------------------------------------------------------------------------
  /**
   * @brief バトルBGMを開始する(視線曲再生も共用)
   * @param soundID  戦闘曲
   */
  //---------------------------------------------------------------------------
  void StartBattleBGMReq( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadein_frame /*= Sound::BGM_FADE_NONE*/  )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartBattleBGMReq %08x\n", soundID );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    
    // ベースBGM再生中はPush有り 視線曲再生中は既にPush済みなのでここではやらない
    if( s_BGMManager->GetLastPushedCount() < PUSH_COUNT_BASE ){
       s_BGMManager->AddCommand_PUSH();
    }
		s_BGMManager->AddCommand_PREPARE_START( soundID );
		s_BGMManager->AddCommand_PREPARE_WAIT();
		s_BGMManager->AddCommand_PREPARE_PLAY();
    if( fadein_frame != BGM_FADE_NONE){
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( fadein_frame );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @biref バトル中のBGMを変更する
   * @param soundID  次のBGM
   */
  //---------------------------------------------------------------------------
  void ChangeBattleBGMReq( u32 soundID, FadeFrame fadeout_frame /*= BGM_FADE_MIDDLE*/, FadeFrame fadein_frame /*= BGM_FADE_NONE*/ )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    ChangeBGMReqCore( soundID, fadeout_frame, fadein_frame, BGMManager::COMMAND_BLOCK_GROUP_BATTLE );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief バトルBGMを終了し, 元のBGMに復帰する
   *
   * @note  new_BGMがSOUND_ITEM_ID_NONEの場合はStartBattleBGMReq再生時点の曲を途中から再生
   *        有効なBGMIDを指定した場合、Pop処理後に新しいBGMを流す
   */ 
  //---------------------------------------------------------------------------
  void EndBattleBGMReq( u32 new_BGM )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: EndBattleBGMReq %08x\n", new_BGM );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( BGM_FADE_VERY_FAST );
    s_BGMManager->AddCommand_FADE_WAIT();
    if( PUSH_COUNT_NONE < s_BGMManager->GetLastPushedCount() ) 
    {
      if( new_BGM == SOUND_ITEM_ID_NONE ){
        s_BGMManager->AddCommand_POP( BGM_FADE_MIDDLE );  //元の曲の途中から
      }else{
        s_BGMManager->AddCommand_POP( 0 );
      }
    }
    if( new_BGM != SOUND_ITEM_ID_NONE ){
      s_BGMManager->AddCommand_PREPARE_START( new_BGM );
      s_BGMManager->AddCommand_PREPARE_WAIT();
      s_BGMManager->AddCommand_PREPARE_PLAY();
      //PushPopで元曲に戻らないパターンは通常FadeIn無しにしているが、バトル終了後なのでFadeInを入れる
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( BGM_FADE_MIDDLE );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  } 
  
  //---------------------------------------------------------------------------
  /**
   * @brief バトルBGMを終了し, SilentBGMに復帰させる(全滅時用)
   */ 
  //---------------------------------------------------------------------------
  void EndBattleBGMReqForGameOver( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: EndBattleBGMReqForGameOver\n" );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( BGM_FADE_VERY_FAST );
    s_BGMManager->AddCommand_FADE_WAIT();
    if( PUSH_COUNT_NONE < s_BGMManager->GetLastPushedCount() ){
      s_BGMManager->AddCommand_POP( 0 );
    }
		s_BGMManager->AddCommand_PREPARE_START( STRM_BGM_SILENCE );
  	s_BGMManager->AddCommand_PREPARE_WAIT();
	 	s_BGMManager->AddCommand_PREPARE_PLAY();
    s_BGMManager->AddCommand_BLOCK_END();
  } 
  
  //=============================================================================
  // アプリBGM制御用関数群
  //
  // StartAppBGMReqでフィールドBGM(ベースBGM)の状態を保存してアプリ用BGMに切り替え、
  // EndAppBGMReq()で元の曲を途中から再開します
  //
  // 内部でStart/EndでPushPopが発生するので、使い方に注意してください
  //
  //=============================================================================
  
  //---------------------------------------------------------------------------
  /**
   * @brief フィールドBGMを終了し, アプリ用BGMを開始する
   * @param soundID  再生するアプリ画面用BGM
   */
  //---------------------------------------------------------------------------
  void StartAppBGMReq( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;

    BGM_PRINT( "BGMReq: StartAppBGMReq %08x\n", soundID );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( BGM_FADE_VERY_FAST );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_PUSH();
		s_BGMManager->AddCommand_PREPARE_START( soundID );
		s_BGMManager->AddCommand_PREPARE_WAIT();
		s_BGMManager->AddCommand_PREPARE_PLAY();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief アプリ用BGMを変更する
   * @param soundID  新しいアプリ用BGM
   * @param fadeoutFrame  デフォルトはVERY_FAST
   * @param fadeinFrame   デフォルトはNONE
   *
   * @note  現曲のフェードアウトから次曲の再生〜フェードインまでシームレスに実行
   *        StartAppBGMReq()とEndAppBGMReq()の間でのみ使用可
   *
   *        現曲のフェードアウト完了後、次曲の再生を特定のタイミングまで待ちたい場合は
   *        FadeOutAppBGMReq()とFadeInNextAppBGMReq()を対で用いること
   */
  //---------------------------------------------------------------------------
  void ChangeAppBGMReq( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    ChangeBGMReqCore( soundID, fadeoutFrame, fadeinFrame, BGMManager::COMMAND_BLOCK_GROUP_APP );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief アプリ用BGMを変更する 同一曲のリクエストの対するスキップ無し
   * @param soundID  新しいアプリ用BGM
   * @param fadeoutFrame  デフォルトはVERY_FAST
   * @param fadeinFrame   デフォルトはNONE
   *
   * @note  利用許可制。ご利用前に必ずサウンド班とサウンドプログラマーへの確認を取ってください
   *        ChangeAppBGMReq()との違いは、グループ単位でのスキップ処理のみにする&&同一曲リクエストをスキップしない
   *
   *        現曲のフェードアウトから次曲の再生〜フェードインまでシームレスに実行
   *        StartAppBGMReq()とEndAppBGMReq()の間でのみ使用可
   *
   *        現曲のフェードアウト完了後、次曲の再生を特定のタイミングまで待ちたい場合は
   *        FadeOutAppBGMReq()とFadeInNextAppBGMReq()を対で用いること
   */
  //---------------------------------------------------------------------------
  void ChangeAppBGMReqForce( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: ChangeAppBGMReqForce %08x\n", soundID );
    s_BGMManager->AddCommand_BLOCK_START( BGMManager::COMMAND_BLOCK_GROUP_APP, soundID, false );
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_PREPARE_START( soundID );
    s_BGMManager->AddCommand_PREPARE_WAIT();
    s_BGMManager->AddCommand_PREPARE_PLAY();
    if( static_cast<int>(fadeinFrame) > 0 ){
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  } 
  
  //---------------------------------------------------------------------------
  /**
   * @brief アプリ用BGMを変更する前のフェードアウト
   *
   * @param fadeoutFrame  デフォルトはVERY_FAST
   *
   * @note  現在のアプリBGMをフェードアウト
   *        StartAppBGMReq()とEndAppBGMReq()の間でのみ使用可
   *
   *        FadeInNextAppBGMReq()と対で使うこと。
   *        サウンドシステムのキューがコマンド実行を管理するので、FadeInNextAppBGMReq()を呼ぶ際のウェイト処理は必要ない
   *
   *        現曲のフェードアウトから次曲の再生〜フェードインまでシームレスに実行したい場合は
   *        ChangeAppBGMReq()を用いる
   */
  //---------------------------------------------------------------------------
  void FadeOutAppBGMReq( FadeFrame fadeoutFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: FadeOutAppBGMReq\n" );
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief アプリ用BGMを変更する 前BGMのフェードアウト後に次のBGMの再生リクエスト
   *
   * @param fadeinFrame  デフォルトはNONE
   *
   * @note  新しいアプリBGMを再生開始してフェードイン
   *        StartAppBGMReq()とEndAppBGMReq()の間でのみ使用可
   *
   *        FadeOutAppBGMReq()と対で使うこと
   *
   *        現曲のフェードアウトから次曲の再生〜フェードインまでシームレスに実行したい場合は
   *        ChangeAppBGMReq()を用いる
   */
  //---------------------------------------------------------------------------
  void FadeInNextAppBGMReq( u32 soundID, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: FadeInNextAppBGMReq %08x", soundID);
    if( s_BGMManager->GetLastPlayedSoundID() != soundID ) 
    {
      BGM_PRINT( " Exec" );
      s_BGMManager->AddCommand_BLOCK_START();
      s_BGMManager->AddCommand_SKIP( soundID );
      s_BGMManager->AddCommand_PREPARE_START( soundID );
      s_BGMManager->AddCommand_PREPARE_WAIT();
      s_BGMManager->AddCommand_PREPARE_PLAY();
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_SKIP_END();
      s_BGMManager->AddCommand_SKIP( soundID );
      s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
      s_BGMManager->AddCommand_SKIP_END();
      s_BGMManager->AddCommand_BLOCK_END();
    }
    BGM_PRINT( "\n" );
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief アプリ用BGMを終了し, フィールドBGMを再開する
   */
  //---------------------------------------------------------------------------
  void EndAppBGMReq( FadeFrame fadeoutFrame /*= BGM_FADE_VERY_FAST*/, FadeFrame fadeinFrame /*= BGM_FADE_MIDDLE*/ )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: EndAppBGMReq\n");
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_POP( fadeinFrame );
    s_BGMManager->AddCommand_BLOCK_END();
  }
  
  //---------------------------------------------------------------------------
  /**
   * @brief Sangoイントロデモ専用BGMチェンジコマンド
   * @param soundID       次に鳴らすBGM
   * @param fadeoutFrame  デフォルトはVERY_FAST
   * @param fadeinFrame   デフォルトはNONE
   *
   * @note  利用許可制。ご利用前に必ずサウンド班とサウンドプログラマーへの確認を取ってください
   *
   *        現曲のフェードアウトから次曲を現曲と同じサンプル位置から続き再生〜フェードインまでシームレスに実行
   */
  //---------------------------------------------------------------------------
  void ChangeBGMContinueReq( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: ChangeIntroDemoBGMReq %08x\n", soundID );
    s_BGMManager->AddCommand_BLOCK_START( BGMManager::COMMAND_BLOCK_GROUP_APP, soundID, false );
    s_BGMManager->AddCommand_FADE_OUT( fadeoutFrame );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_PREPARE_START_CONTINUE( soundID );
    s_BGMManager->AddCommand_PREPARE_WAIT();
    s_BGMManager->AddCommand_PREPARE_PLAY();
    if( static_cast<int>(fadeinFrame) > 0 ){
      s_BGMManager->AddCommand_PAUSE();
      s_BGMManager->AddCommand_FADE_IN( fadeinFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
    }
    s_BGMManager->AddCommand_BLOCK_END();
  } 

  //=============================================================================
  // フィールドマップBGM制御用関数群 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
  //=============================================================================
  //---------------------------------------------------------------------------
  /**
   * @brief 現在のBGMをフェードアウトし, 次のマップのBGMの再生準備を開始する
   * @param soundID  次のマップのBGM
   * 
   * @note 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
   */
  //---------------------------------------------------------------------------
  void PrepareNextMapBGMReq( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: PrepareNextMapBGMReq");
    if( s_BGMManager->GetLastPlayedSoundID() != soundID ) 
    {
      BGM_PRINT( " Exec" );
      s_BGMManager->AddCommand_BLOCK_START();
      s_BGMManager->AddCommand_FADE_OUT( BGM_FADE_MIDDLE );
      s_BGMManager->AddCommand_FADE_WAIT();
      s_BGMManager->AddCommand_PREPARE_START( soundID );
      s_BGMManager->AddCommand_PREPARE_WAIT();
      s_BGMManager->AddCommand_BLOCK_END();
    }
    BGM_PRINT( "\n" );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief PrepareNextMapBGMReq() で準備したBGMの再生を開始する
   *
   * @note 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
   */
  //---------------------------------------------------------------------------
  void PreparePlayNextMapBGMReq( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: PreparePlayNextMapBGMReq");
    if( s_BGMManager->GetLastPreparedSoundID() != SMID_NULL )
    {
      BGM_PRINT( " Exec" );
      s_BGMManager->AddCommand_BLOCK_START();
      s_BGMManager->AddCommand_PREPARE_PLAY();
      s_BGMManager->AddCommand_BLOCK_END();
    }
    BGM_PRINT( "\n" );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief フィールドでのBGM変更処理
   * @param soundID  次のBGM
   * @param fadeout_frame  フェードアウトフレーム(デフォルトはBGM_FADE_MIDDLE)
   * @param fadein_frame   フェードインフレーム(デフォルトはBGM_FADE_NONE)
   *
   * @note 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
   *
   * @note  ゾーンチェンジ時や自機形態変化に伴うBGM変更に用いる。
   *        リクエストが重なった場合にスキップされることがある
   */
  //---------------------------------------------------------------------------
  void ChangeFieldBGMReq( u32 soundID, FadeFrame fadeoutFrame, FadeFrame fadeinFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    ChangeBGMReqCore( soundID, fadeoutFrame, fadeinFrame, BGMManager::COMMAND_BLOCK_GROUP_FIELD );
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief イベントBGMを開始する
   * @param soundID    開始するBGM
   * @param fadeFrame  フェード時間(デフォルトではBGM_FADE_VERY_FAST)
   *
   * @note 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
   */
  //---------------------------------------------------------------------------
  void StartFieldEventBGM( u32 soundID, FadeFrame fadeFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

  	DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartFieldEventBGM %08x", soundID);
    if( s_BGMManager->GetLastPlayedSoundID() != soundID ) 
    {
      BGM_PRINT( " Exec" );
      s_BGMManager->AddCommand_BLOCK_START();
      s_BGMManager->AddCommand_FADE_OUT( fadeFrame );
      s_BGMManager->AddCommand_FADE_WAIT();
			s_BGMManager->AddCommand_PREPARE_START( soundID );
			s_BGMManager->AddCommand_PREPARE_WAIT();
			s_BGMManager->AddCommand_PREPARE_PLAY();
      s_BGMManager->AddCommand_BLOCK_END();
    }
    BGM_PRINT( "\n" );
  }
  
  //=============================================================================
  // ME制御用関数群 フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
  //=============================================================================

  //---------------------------------------------------------------------------
  /**
   * @brief MEの再生を開始する
   * @param soundID  再生するME
   *
   * @note フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
   */
  //---------------------------------------------------------------------------
  void StartME( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartME %08x\n", soundID );
    
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_FADE_OUT( BGM_FADE_VERY_FAST );
    s_BGMManager->AddCommand_FADE_WAIT();
    s_BGMManager->AddCommand_PUSH();
		s_BGMManager->AddCommand_PREPARE_START( soundID );
		s_BGMManager->AddCommand_PREPARE_WAIT();
		s_BGMManager->AddCommand_PREPARE_PLAY();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief MEの再生を終了し, 元のBGMに復帰する
   *
   * @note フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
   */
  //---------------------------------------------------------------------------
  void StartBaseBGMFromME( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartBaseBGMFromME\n");

    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_POP( BGM_FADE_VERY_FAST );
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief ME再生開始時のBGMを破棄して, 新しいBGMで復帰する
   * @param new_BGM  新しいBGM
   *
   * @note フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
   */
  //---------------------------------------------------------------------------
  void StartNewBGMFromME( u32 new_BGM )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    BGM_PRINT( "BGMReq: StartNewBGMFromME %08x\n", new_BGM);
    
    s_BGMManager->AddCommand_BLOCK_START();
    s_BGMManager->AddCommand_POP( 0 );
    s_BGMManager->AddCommand_PREPARE_START( new_BGM );
    s_BGMManager->AddCommand_PREPARE_WAIT();
    s_BGMManager->AddCommand_PREPARE_PLAY();
    s_BGMManager->AddCommand_BLOCK_END();
  }

  //---------------------------------------------------------------------------
  /**
   * @brief MEが再生中かどうかを調べる
   * @param soundID  調べるME
   * @retval true   終了した
   * @retval false  再生中
   *
   * @note 
   * StartME()で登録したBGMリクエストが反映される前に
   * この関数が呼ばれると正しい結果を取得することができない.
   * ( まだMEを再生していない状態を「終了した」状態と勘違いしてしまうため )
   * ので, 未処理のBGMのリクエストが存在している間はME再生中扱いとする.
   *
   * 厳密に対処するには
   * MEリクエスト発行〜ME再生開始 の間の状態を認識する必要がある.
   * リクエストにコールバックを登録する？
   * また, 状態を管理するのは役割ではない.
   *
   * @note フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
   */
  //---------------------------------------------------------------------------
  b32 IsMEFinished( u32 soundID )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    if( s_BGMManager->HaveCommand() ) {
      return false;  // @note 参照
    }
    return !( s_PMSoundSystem->IsBGMPlaying( soundID ) );
  }
  
  //---------------------------------------------------------------------------
  // その他
  //--------------------------------------------------------------------------- 
  /**
   * @brief 現在なっているストリームサウンドの再生位置を取得する
   * @return 再生位置（サンプル数） ハンドルが無効の場合は負の値
   */
  long GetStreamSoundPlaySamplePosition( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(-1);
    return s_PMSoundSystem->GetStreamSoundPlaySamplePosition();
  }
  
  /**
   * @brief サンプリングレートを取得する
   *
   * @return  サンプリングレート  ハンドルが無効の場合は負の値
   */
  int GetStreamSoundSampleRate( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(-1);
    return s_PMSoundSystem->GetStreamSoundSampleRate();
  }

  /**
   * @brief ストリームデータのループフラグを取得する
   *
   * @return  ストリームデータのループフラグを取得する
   * @note ハンドルが無効の場合は負の値 有効ならtrue 無効ならfalse
   */
  int GetStreamSoundLoopFlag( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(-1);
    return s_PMSoundSystem->GetStreamSoundLoopFlag();
  }

  /**
   * @brief ストリームデータのループ開始位置を取得する
   *
   * @return  ストリームデータのループ開始位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  int GetStreamSoundLoopStartPos( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(-1);
    return s_PMSoundSystem->GetStreamSoundLoopStartPos();
  }

  /**
   * @brief ストリームデータのループ終了位置を取得する
   *
   * @return  ストリームデータのループ終了位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  int GetStreamSoundLoopEndPos( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(-1);
    return s_PMSoundSystem->GetStreamSoundLoopEndPos();
  }

#if PM_DEBUG
  b32 IsBGMControlEnable( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    return s_PMSoundSystem->IsBGMControlEnable();
  }
  void SetBGMControlEnable( b32 enable )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    s_PMSoundSystem->SetBGMControlEnable( enable );
  }
#endif



} // namespace Sound

//=============================================================================
/**
 * @brief
 * @file    Sound.h
 * @author  Miyuki Iwasawa
 * @date    2015.03.12
 */
//=============================================================================
#ifndef __H_SOUND_H__
#define __H_SOUND_H__
#pragma once

#include <debug/include/gfl2_Assert.h>
#include <sound/include/gfl2_SoundSystem.h>
#include <pml/include/pmlib.h>
#include <niji_conv_header/poke_lib/monsno_def.h>

#include "niji_conv_header/sound/SoundMiddleID.h"
#include "niji_conv_header/sound/VoiceID.h"
#include "niji_conv_header/sound/SoundDefine.h"

#if PM_DEBUG

#define DEBUG_SOUND_OFF (0)     // サウンドOFF　デバッグ機能　1でON

#else

#define DEBUG_SOUND_OFF (0)

#endif

//---------------------------------------------------------------------------
//サウンドOFFデバッグ機能ON・OFF
//---------------------------------------------------------------------------
#if DEBUG_SOUND_OFF

#define DEBUG_SOUND_OFF_RETNONE if(!Sound::IsInitialized()) return
#define DEBUG_SOUND_OFF_RET(ret) if(!Sound::IsInitialized()) return (ret)

#else // DEBUG_SOUND_OFF

#define DEBUG_SOUND_OFF_RETNONE ((void)0)
#define DEBUG_SOUND_OFF_RET(ret) ((void)0)

#endif // DEBUG_SOUND_OFF

//サウンドのアップデートをスレッドで行う設定
#define SOUND_UPDATE_THREAD_ENABLE 1
#if SOUND_UPDATE_THREAD_ENABLE
#define SOUND_UPDATE_CRITICALSECTION gfl2::thread::ScopedLock scopedLock(Sound::GetCriticalSection())
#else
#define SOUND_UPDATE_CRITICALSECTION ((void)0)
#endif

namespace gfl2{
  namespace fs
  {
    class AsyncFileManager;
  }
}

namespace Sound {

  // BGMボリューム
  static const f32 BGM_VOLUME_NORMAL = 1.0f;  // 通常の音量
  static const f32 BGM_VOLUME_APP    = 0.7f;  // アプリ時の音量
  static const f32 BGM_VOLUME_ZUKAN  = 0.5f;  // 図鑑アプリ時の音量
  static const f32 BGM_VOLUME_VCHAT  = 0.3f;  // ボイスチャット時の音量
  static const f32 BGM_VOLUME_VCHAT_FIELD  = 0.0f;  // フィールドボイスチャット時の音量
  static const f32 BGM_VOLUME_KAWAIGARI =  0.3f;  // 可愛がり時の音量

  // デフォルトのプレイヤーを指す定数
  static const u32 DEFAULT_PLAYER = 0;
  static const u32 SOUND_ITEM_ID_NONE = 0xFFFFFFFF;

  // SEを管理しないことを表すID
  const s32 NOT_CONTROL = -1;

  // SEに適用するエフェクトを示すビットフラグ
  enum EffectBit {
    EFFECT_BIT_VOLUME       = 1 << 0,   // 音量
    EFFECT_BIT_PITCH        = 1 << 1,   // ピッチ
    EFFECT_BIT_PAN          = 1 << 2,   // 水平方向のパン
    EFFECT_BIT_SURROUND_PAN = 1 << 3,   // 垂直方向のパン
    EFFECT_BIT_TEMPO        = 1 << 4,   // テンポ
  };

  // SEに適用するパラメータ
  struct PlayInfo
  {
    u16 fadein_frame;   //再生開始時のフェードインフレーム(デフォルトは0)
    //エフェクト指定パラメータ
    u16 effect_enable;   // 下記の引数が有効かどうかを EFFECT_BIT_XXXX の論理和で指定
    f32 volume;         // 音量          ( effect_enable & EFFECT_BIT_VOLUME ) != 0 の場合にのみ有効
    f32 pitch;          // ピッチ        ( effect_enable & EFFECT_BIT_PITCH ) != 0 の場合にのみ有効
    f32 pan;            // 水平方向のパン( effect_enable & EFFECT_BIT_PAN ) != 0 の場合にのみ有効
    f32 surround_pan;   // 垂直方向のパン( effect_enable & EFFECT_BIT_SURROUND_PAN ) != 0 の場合にのみ有効
    f32 tempo;          // テンポ        ( effect_enable & EFFECT_BIT_TEMPO ) != 0 の場合にのみ有効
    u32 change_volume_frame;  // 音量変化にかけるフレーム値 ( effect_enable & EFFECT_BIT_VOLUME ) != 0 の場合にのみ有効 未指定なら0frame

    PlayInfo( u16 effect_enable = 0, u16 fadein_frame = 0 ) :
      fadein_frame(fadein_frame),
      effect_enable(effect_enable),
      volume(1.0f),
      pitch(1.0f),
      pan(0.0f),
      surround_pan(1.0f),
      tempo(1.0f),
      change_volume_frame(0) {}
  };

  // 3Dサウンドアクターに適用するエフェクトを示すビットフラグ
  enum EffectBit3D {
    EFFECT_BIT_VOLUME_3D      = 1 << 0,   // 音量
    EFFECT_BIT_PITCH_3D       = 1 << 1,   // ピッチ
    EFFECT_BIT_PAN_3D         = 1 << 2,   // 水平方向のパン
    EFFECT_BIT_POS_3D         = 1 << 3,   // ポジション
    EFFECT_BIT_VELOCITY_3D    = 1 << 4,   // ベロシティ
  };

  // 3Dサウンドアクターに適用するパラメータ
  struct PlayInfo3D
  {
    u8  effect_enable3D;    // 下記の引数が有効かどうかを EFFECT_BIT_XXXX_3D の論理和で指定
    f32 volume3D;           // 音量          ( effect_3d_enable & EFFECT_BIT_VOLUME_3D ) != 0 の場合にのみ有効
    f32 pitch3D;            // ピッチ        ( effect_3d_enable & EFFECT_BIT_PITCH_3D ) != 0 の場合にのみ有効
    f32 pan3D;              // パン　　　     ( effect_3d_enable & EFFECT_BIT_PAN_3D ) != 0 の場合にのみ有効
    gfl2::math::VEC3 pos3D;  // ポジション     ( effect_3d_enable & EFFECT_BIT_POSITION_3D ) != 0 の場合にのみ有効
    gfl2::math::VEC3 velocity3D; // ベロシティ ( effect_3d_enable & EFFECT_BIT_VELOCITY_3D ) != 0 の場合にのみ有効
  };

  // SEに適用するエフェクトを示すビットフラグ
  enum VoiceEffectBit {
    VEFFECT_BIT_VOLUME       = 1 << 0,   // 音量
    VEFFECT_BIT_PITCH        = 1 << 1,   // ピッチ
    VEFFECT_BIT_PAN          = 1 << 2,   // 水平方向のパン
    VEFFECT_BIT_SURROUND_PAN = 1 << 3,   // 垂直方向のパン(現状、通常モードのみ有効)
    VEFFECT_BIT_TEMPO        = 1 << 4,   // テンポ(現状、通常モードのみ有効)
    VEFFECT_BIT_CHANGE_VOLUME_FRAME = 1 << 5, // 音量変化にかけるフレーム値(EFFECT_BIT_VOLUME有効時のみ反映、現状、通常モードのみ有効)
    VEFFECT_BIT_POS          = 1 << 6,   // ポジション(3Dモードのみ有効)
    VEFFECT_BIT_VELOCITY     = 1 << 7,   // ベロシティ(3Dモードのみ有効)
  };

  // VOICEに適用するパラメータ
  struct VoicePlayInfo
  {
    u32 effect_enable;   // 下記の引数が有効かどうかを EFFECT_BIT_XXXX の論理和で指定
    f32 volume;         // 音量          ( effect_enable & EFFECT_BIT_VOLUME ) != 0 の場合にのみ有効
    f32 pitch;          // ピッチ        ( effect_enable & EFFECT_BIT_PITCH ) != 0 の場合にのみ有効
    f32 pan;            // 水平方向のパン( effect_enable & EFFECT_BIT_PAN ) != 0 の場合にのみ有効
    f32 surround_pan;   // 垂直方向のパン( effect_enable & EFFECT_BIT_SURROUND_PAN ) != 0 の場合にのみ有効
    f32 tempo;          // テンポ        ( effect_enable & EFFECT_BIT_TEMPO ) != 0 の場合にのみ有効
    f32 change_volume_frame;  // 音量変化にかけるフレーム値
                              // ( effect_enable & EFFECT_BIT_VOLUME ) != 0 &&
                              // ( effect_enable & EFFECT_BIT_VOLUME_CHANGE_FRAME )!=0 の場合にのみ有効
    gfl2::math::VEC3 pos3D;  // ポジション     ( effect_3d_enable & EFFECT_BIT_POSITION_3D ) != 0 の場合にのみ有効
    gfl2::math::VEC3 velocity3D; // ベロシティ ( effect_3d_enable & EFFECT_BIT_VELOCITY_3D ) != 0 の場合にのみ有効
  };

  enum VoiceType {
    VOICE_TYPE_DEFAULT,   ///< 通常
    VOICE_TYPE_BATTLE,    ///< バトル(ピカチュウのみに有効なデータ。他は通常と同じ)
    VOICE_TYPE_DOWN,      ///< ダウン時
    VOICE_TYPE_SP00,      ///< 特殊 3匹選択時のみ使用
//    VOICE_TYPE_SP01,      ///< 特殊 廃止された 121227 iwasawa
    VOICE_TYPE_NUM,
  };


  extern b32 IsInitialized( void );
  extern gfl2::thread::CriticalSection& GetCriticalSection( void );

  extern void Initialize( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
  extern void Finalize( void );
  extern void FinalizeForGameReset( gfl2::fs::AsyncFileManager* pFileReadManager ); //BTS6040 ゲームリセット時用 いつ呼ばれても大丈夫にするため、再生の後処理とかを全部やってからFinalize
  extern void Update( void );
#if SOUND_UPDATE_THREAD_ENABLE
  extern void UpdateThreadSync( void );
#endif

#if PM_DEBUG
  class PMSoundSystem;
  extern PMSoundSystem* GetPMSoundSystem( void );
#endif

  //---------------------------------------------------------------------------
  // サウンドモード変更
  //--------------------------------------------------------------------------
  extern void SetOutputModeByConfig( void );
  extern gfl2::sound::SoundSystem::OutputMode GetOutputMode( void );
  extern void SetOutputModeMono( void );
  extern void SetOutputModeStereo( void );
  extern void SetOutputModeSurround( void );

  //--------------------------------------------------------------------------
  // マスターボリューム変更
  //		マスターボリュームスイッチフラグ on = true. off = false
  //--------------------------------------------------------------------------
  extern void SoundMasterVolumeSW(b32 OnOff);

  //---------------------------------------------------------------------------
  // 基本のBGM再生〜終了 〜Req()系 コマンドキューを介してリクエストを実行する
  // 各関数の詳細な説明は src/sound/sound_BGM.cpp を参照してください
  //---------------------------------------------------------------------------
  extern void StartBGMReq( u32 soundID, FadeFrame fadein_frame = BGM_FADE_NONE );                    // BGM再生をリクエスト
  extern void StartBGM( u32 soundID, u32 fadein_frame = 0 );  // BGM再生をリクエスト StartBGMReq()に移行し削除予定
  extern void ChangeBGMReq( u32 soundID, FadeFrame fadeoutFrame = BGM_FADE_MIDDLE, FadeFrame fadeinFrame = BGM_FADE_NONE ); //BGM変更をリクエスト
  extern void EndBGMReq( FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST );        // BGM停止をリクエスト
  extern void FadeOutBGMReq( FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST ); //BGMのフェードアウトをリクエスト
  extern void FadeInBGMReq( FadeFrame fadeinFrame );  //フェードアウトしたBGMを再開させる
  extern void PushBGMReq( FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST);    // 再生中のBGMを退避する
  extern void PopBGMReq( FadeFrame fadeinFrame );                          // 退避したBGMを復元する(BGM_FADE_NONEを指定すると再生復帰しないので注意)
  extern void PopBGMOnlyReq( void );																			 // 退避したBGMを復元する(再生復帰なし)
  extern void PrepareBGMReq( u32 soundID , u32 startMilliSec = 0 ); ///< BGMの非同期読み込みを開始する
  extern void PreparePlayBGMReq( FadeFrame fadeinFrame = BGM_FADE_NONE ); ///< BGMの非同期読み込みを行ったBGMを再生する

  //---------------------------------------------------------------------------
  // BGM 即時実行系 コマンドキューを介さずにダイレクトに実行する
  // 各関数の詳細な説明は src/sound/sound_BGM.cpp を参照してください
  //
  // 原則利用禁止：各コマンドに対応したReq系関数があるので、そちらを用いてください
  //---------------------------------------------------------------------------
  extern void PauseBGM( u32 frame = BGM_FADE_VERY_FAST );                 // BGMを一時停止する
  extern void RestartBGM( u32 frame = BGM_FADE_NONE );                    // 一時停止中のBGMを再開する
  extern void FadeOutBGM( u32 frame = BGM_FADE_VERY_FAST );               // BGMをフェードアウトさせる
  extern void FadeInBGM( u32 frame );                                     // BGMをフェードインさせる

  //---------------------------------------------------------------------------
  // BGM 即時実行系 コマンドキューを介さずにダイレクトに実行する
  // 各関数の詳細な説明は src/sound/sound_BGM.cpp を参照してください
  //---------------------------------------------------------------------------
  extern b32 IsBGMFade( void );                                          // BGMがフェード中か？
  extern b32 IsBGMPlaying( void );                                       // BGMが再生中かどうかを調べる
  extern b32 IsBGMPlaying( u32 soundID );                                // 任意のBGMが再生中かどうかを調べる
  extern b32 HaveCommand( void );                                        // 終了していないコマンドがあるか？

  //---------------------------------------------------------------------------
  // 直接呼出しが推奨されないBGMダイレクト実行系
  // 各関数の詳細な説明は src/sound/sound_BGM.cpp を参照してください
  //
  // 扱いが特殊なので、安全のために使用したい場合は事前に確認を取ってください
  // StopBGM,PushBGM,PopBGM,PopBGMOnlyにはReq系関数を用意してあるので、原則そちらを利用すること
  //---------------------------------------------------------------------------
  extern void StopBGM( u32 fadeout_frame = BGM_FADE_VERY_FAST );          // BGMを完全停止する
  extern void StopAllBGM( void );                                         // 全てのBGMを完全停止する
  extern b32 PushBGM( void );                                            // 再生中のBGMを退避する
  extern b32 PopBGM( u32 frame );                                        // 退避したBGMを復元する
  extern b32 PopBGMOnly( void );																					// 退避したBGMを復元する(再生復帰なし)
  extern u32 GetPushedBGMCount( void );                                   // BGMの退避数を取得する

  //=============================================================================
  // BGMエフェクト系  ダイレクト実行
  //=============================================================================
  extern void ChangeBGMVolume( f32 volume, u32 frame );                         // 音量を変更する
  extern void ChangeBGMTempo( f32 tempo );                                      // テンポを変更する
  extern void ChangeBGMPitch( f32 value );                                      // ピッチを変更する
  extern void ChangeBGMPitchByEqualTemperament( s8 level );                     // ピッチを12平均律に即した音程に変更する
  extern void ChangeBGMPan( f32 value );                                        // 水平方向パンを変更する
  extern void ChangeBGMSurroundPan( f32 value );                                // 垂直方向パンを変更する
  extern void ChangeBGMTrackVolume( u32 track_bit, f32 volume, u32 frame );     // 特定トラックの音量を変更する
  extern void ChangeBGMTrackPitch( u32 track_bit, f32 value );                  // 特定トラックのピッチを変更する
  extern void ChangeBGMTrackPitchByEqualTemperament( u32 track_bit, s8 level ); // 特定トラックのピッチを12平均律に即した音程に変更する
  extern void ChangeBGMTrackPan( u32 track_bit, f32 value );                    // 特定トラックの水平方向パンを変更する
  extern void ChangeBGMTrackSurroundPan( u32 track_bit, f32 value );            // 特定トラックの垂直方向パンを変更する
#if PM_DEBUG
  b32 IsBGMControlEnable( void ); // BGM 操作が有効か？
  void SetBGMControlEnable( b32 enable ); // BGM 操作が可能かどうかをセットする
#endif

  //=============================================================================
  // シチュエーション別BGM操作
  // コマンドキューを介してリクエストを実行する
  //=============================================================================

  //---------------------------------------------------------------------------
  // BGM操作 バトル用
  // コマンドキューを介してリクエストを実行する
  //---------------------------------------------------------------------------
  extern void StartBattleBGMReq( u32 soundID, FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST, FadeFrame fadein_frame = BGM_FADE_NONE  );  // バトルBGMを開始する(視線曲再生も共用)
  extern void ChangeBattleBGMReq( u32 soundID, FadeFrame fadeout_frame = BGM_FADE_MIDDLE, FadeFrame fadein_frame = BGM_FADE_NONE );  // バトル中のBGMを変更する
  extern void EndBattleBGMReq( u32 new_BGM = SOUND_ITEM_ID_NONE );  // バトルBGMを終了し元のBGMに復帰する(基本はStartBattleBGMReq再生時点の曲に戻る)
  extern void EndBattleBGMReqForGameOver( void ); // バトルBGMを終了しSilentBGMに復帰する(全滅時用)

  //---------------------------------------------------------------------------
  // BGM操作 フィールド用 直接呼出し禁止 FieldSoundモジュールを介して呼び出すこと！
  // コマンドキューを介してリクエストを実行する
  //---------------------------------------------------------------------------
  extern void PrepareNextMapBGMReq( u32 soundID );  // 現在のBGMをフェードアウトし, 次のマップのBGMの再生準備を開始する
  extern void PreparePlayNextMapBGMReq( void );     // PrepareNExtMapBGMReq() で準備したBGMの再生を開始する
  extern void ChangeFieldBGMReq( u32 soundID, FadeFrame fadeout_frame = BGM_FADE_MIDDLE, FadeFrame fadein_frame = BGM_FADE_NONE );  // フィールドでのBGM変更処理(スキップ条件がフィールド専用)
  extern void StartFieldEventBGMReq( u32 soundID, FadeFrame fadeFrame = BGM_FADE_VERY_FAST ); // イベントBGMを開始する(直接呼出し禁止 FieldSoundモジュール経由で呼ぶこと！)

  //---------------------------------------------------------------------------
  // BGM操作 アプリ用
  //
  // StartAppBGMReqでフィールドBGM(ベースBGM)の状態を保存してアプリ用BGMに切り替え、
  // EndAppBGMReq()で元の曲を途中から再開します
  //
  // 内部でStart/EndでPushPopが発生するので、使い方に注意してください
  //
  // コマンドキューを介してリクエストを実行します
  //---------------------------------------------------------------------------
  extern void StartAppBGMReq( u32 soundID );    // フィールドBGMを終了し, アプリ用BGMを開始する
  extern void ChangeAppBGMReq( u32 soundID, FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST, FadeFrame fadeinFrame = BGM_FADE_NONE );   // アプリ用BGMを変更する
  extern void FadeOutAppBGMReq( FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST );   // アプリ用BGMを変更のためにフェードアウト
  extern void FadeInNextAppBGMReq( u32 soundID, FadeFrame fadeinFrame = BGM_FADE_NONE );   // アプリ用BGMを変更する
  extern void EndAppBGMReq( FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST, FadeFrame fadeinFrame = BGM_FADE_MIDDLE );             // アプリ用BGMを終了し, フィールドBGMを再開する

  //利用許可制！
  extern void ChangeAppBGMReqForce( u32 soundID, FadeFrame fadeoutFrame = BGM_FADE_VERY_FAST, FadeFrame fadeinFrame = BGM_FADE_NONE );
  
  //利用許可制！
  extern void ChangeBGMContinueReq( u32 soundID, FadeFrame fadeoutFrame = BGM_FADE_MIDDLE, FadeFrame fadeinFrame = BGM_FADE_NONE ); //BGM変更をリクエスト(続きから再生版)

  //---------------------------------------------------------------------------
  // ME操作系
  // フィールドマップ上で用いる時は FieldSoundモジュールでパックされている関数を用いること
  //
  // IsMEFinished()以外はコマンドキューを介してリクエストを実行します
  //---------------------------------------------------------------------------
  extern void StartME( u32 soundID );            // MEの再生を開始する
  extern b32 IsMEFinished( u32 soundID );       // MEが再生中かどうかを調べる
  extern void StartBaseBGMFromME( void );        // MEの再生を終了し, 元のBGMに復帰する
  extern void StartNewBGMFromME( u32 soundID );  // ME再生開始時のBGMを破棄して, 新しいBGMで復帰する

  //---------------------------------------------------------------------------
  // SE
  //---------------------------------------------------------------------------
  // SEを再生する
  extern void PlaySE( u32 soundID, u32 fadein_frame = 0, s32 controlID = NOT_CONTROL, u32 playerID = DEFAULT_PLAYER );
  // 詳細なパラメータを指定してSEを再生する
  extern void PlaySE( u32 soundID, const PlayInfo& info, s32 controlID = NOT_CONTROL, u32 playerID = DEFAULT_PLAYER );
  extern b32 IsSEPlaying( u32 soundID, s32 controlID = NOT_CONTROL );                    // SEが再生中かどうかを調べる
  extern b32 IsSEFinished( u32 soundID, s32 controlID = NOT_CONTROL );                   // SEの再生が終了したかどうかを調べる
  extern void StopSE( u32 soundID, u32 fadeout_frame = 0, s32 controlID = NOT_CONTROL );  // SEを停止する
  extern void StopAllSE( void );                                                          // 全てのSEを停止する
  extern void ChangeSEVolume( u32 soundID, f32 volume, u32 change_frame = 0, s32 controlID = NOT_CONTROL ); // SEの音量を変更する
  extern void ChangeSEPitch( u32 soundID, f32 value, s32 controlID = NOT_CONTROL );       // SEのピッチを変更する
  extern void ChangeSEPan( u32 soundID, f32 value, s32 controlID = NOT_CONTROL );         // SEの水平方向パンを変更する
  extern void ChangeSESurroundPan( u32 soundID, f32 value, s32 controlID = NOT_CONTROL ); // SEの垂直方向パンを変更する
  extern void ChangeSETempo( u32 soundID, f32 value, s32 controlID = NOT_CONTROL );       // SEのテンポを変更する
  extern void ChangeSETranspose( u32 soundID, s8 value, s32 controlID = NOT_CONTROL, u32 bit = 0xffffffff );  // SEのトランスポーズする
  extern void ChangeSETrackVariable( u32 soundID, int trackNo, int varNo, s16 value, s32 controlID = NOT_CONTROL );   //SEのトラック変数を変更する
  extern s16  GetSETrackVariable( u32 soundID, int trackNo, int varNo, s32 controlID = NOT_CONTROL );   //SEのトラック変数を取得する

  //---------------------------------------------------------------------------
  // 環境音
  //---------------------------------------------------------------------------
  extern void PlayEnvSE( u32 soundID, u32 fadein_frame = 0, f32 volume = 1.0f );  //環境音の再生を始める
  extern void ChangeEnvSEVolume( u32 soundID, f32 volume, u32 change_frame = 0 ); //環境音のボリュームを変える
  extern void StopEnvSE( u32 soundID, u32 fadeout_frame = 0 ); //環境音を止める
  extern void StopEnvSEAll( void ); //全ての環境音を止める
  extern void ResetEnvSE( void ); //すべての環境音を止めてステータスもリセットする
  extern void PauseEnvSE( EnvSEPauseState type );   //すべての環境音を一時停止する
  extern void ReplayEnvSE( EnvSEPauseState type );  //すべての環境音の一時停止を解放する

  //-------------------------------------------------------------------------
  // 3Dサウンド
  //-------------------------------------------------------------------------
  extern void Set3DParam( const gfl2::sound::SoundSystem::Sound3DParam& param );
  extern void Get3DParam( gfl2::sound::SoundSystem::Sound3DParam* buffer );

  extern void SetListenerParam( const gfl2::sound::SoundSystem::ListenerParam& param );
  extern void GetListenerParam( gfl2::sound::SoundSystem::ListenerParam* buffer );
  extern void SetListenerMatrix( const gfl2::math::MTX34& matrix );
  extern void SetListenerPos( const gfl2::math::VEC3& pos, const gfl2::math::VEC3& up, const gfl2::math::VEC3& dir );
  extern void SetListenerVelocity( const gfl2::math::VEC3& velocity );

  extern b32 Play3DActor( u8 actor_index, u32 soundID , const nw::snd::SoundStartable::StartInfo* startInfo = NULL );
  extern void Stop3DActor( u8 actor_index, s32 fadeFrames );
  extern void Pause3DActor( u8 actor_index, b32 pauseOn, s32 fadeFrames );
  extern void Set3DActorVolume( u8 actor_index, f32 volume );
  extern f32 Get3DActorVolume( u8 actor_index );
  extern void Set3DActorPitch( u8 actor_index, f32 pitch );
  extern f32 Get3DActorPitch( u8 actor_index );
  extern void Set3DActorPan( u8 actor_index, f32 pan );
  extern f32 Get3DActorPan( u8 actor_index );
  extern void Set3DActorPos( u8 actor_index, const gfl2::math::VEC3& pos );
  extern void Get3DActorPos( u8 actor_index, gfl2::math::VEC3* pos );
  extern void Set3DActorVelocity( u8 actor_index, const gfl2::math::VEC3& velocity );

  //---------------------------------------------------------------------------
  // なきごえ
  //---------------------------------------------------------------------------
  /**
   * @brief プレーヤーの追加と削除
   * 削除の際は追加分全て削除するので注意
   * (現状の用途として複雑な追加状態はないと考え、きちんと管理されたシステムにしていないため)
   */
  extern void CreateAdditionalVoicePlayer( u32 num, gfl2::heap::HeapBase* device_heap);
  extern b32 IsPlayingAdditionalVoicePlayer( void );
  extern void DeleteAdditionalVoicePlayer( void );

  // 鳴き声を再生する
  extern void PlayVoice(u8 voice_index,
                        MonsNo monsno, FormNo formno,
                        VoiceType voice_type = VOICE_TYPE_DEFAULT,
                        b32 act3D = false, u32 userId = 0 );
  extern void PlayVoice(u8 voice_index,
                        const pml::pokepara::CoreParam* param,
                        VoiceType voice_type = VOICE_TYPE_DEFAULT,
                        b32 act3D = false, u32 userId = 0 );
  extern void PlayVoice(u8 voice_index, VoiceID voiceID, b32 act3D = false, u32 userId = 0 );

  // 鳴き声を3Dアクターで再生する
  inline void PlayVoice3DActor( u8 voice_index, MonsNo monsno, FormNo formno, VoiceType voice_type = VOICE_TYPE_DEFAULT )
                { PlayVoice(voice_index, monsno, formno, voice_type, true ); }
  inline void PlayVoice3DActor( u8 voice_index, const pml::pokepara::PokemonParam* pokeparam, VoiceType voice_type = VOICE_TYPE_DEFAULT )
                { PlayVoice(voice_index, pokeparam, voice_type, true ); }
  inline void PlayVoice3DActor( u8 voice_index, VoiceID voiceID )
                { PlayVoice(voice_index, voiceID, true ); }

  // 鳴き声が終了したか？
  extern b32 IsVoiceFinished( u8 voice_index );
  inline b32 IsVoice3DActorFinished( u8 voice_index ){ return IsVoiceFinished(voice_index); }
  extern u32 GetVoiceUserId( u8 voice_index );
  
  // 鳴き声を停止する
  extern void StopVoice(u8 voice_index, s32 fadeFrame = 0); //fadeFrameはGFBTS2505対策で必ず0になるので指定の意味はない 130514)
  inline void StopVoice3DActor( u8 voice_index ){ StopVoice(voice_index, 0); }

  //全ての鳴き声を停止させる(BTS6040,BTS6242,BTS6269,BTS6274対策 ゲームリセット用)
  extern b32 ResetVoiceReq( gfl2::fs::AsyncFileManager* pFileReadManager );

  // 鳴き声にエフェクトを適用する
  extern void ChangeVoiceEffect(u8 voice_index, const VoicePlayInfo& vEffectInfo);
  inline void ChangeVoiceEffect( u8 voice_index, const PlayInfo& effectInfo )
  {
    VoicePlayInfo vi = {0};

    vi.effect_enable = 0;

    if(effectInfo.effect_enable & EFFECT_BIT_VOLUME){
      vi.effect_enable |= VEFFECT_BIT_VOLUME;
      vi.volume = effectInfo.volume;
      if( effectInfo.change_volume_frame != 0 ){
        vi.effect_enable |= VEFFECT_BIT_CHANGE_VOLUME_FRAME;
        vi.change_volume_frame = effectInfo.change_volume_frame;
      }
    }
    if(effectInfo.effect_enable & EFFECT_BIT_PITCH){ vi.effect_enable |= VEFFECT_BIT_PITCH; }
    if(effectInfo.effect_enable & EFFECT_BIT_PAN){ vi.effect_enable |= VEFFECT_BIT_PAN; }
    if(effectInfo.effect_enable & EFFECT_BIT_SURROUND_PAN){ vi.effect_enable |= VEFFECT_BIT_SURROUND_PAN; }
    if(effectInfo.effect_enable & EFFECT_BIT_TEMPO){ vi.effect_enable |= VEFFECT_BIT_TEMPO; }

    vi.pitch = effectInfo.pitch;
    vi.pan = effectInfo.pan;
    vi.surround_pan = effectInfo.surround_pan;
    vi.tempo = effectInfo.tempo;

    ChangeVoiceEffect(voice_index, vi);
  }
  inline void ChangeVoice3DActorEffect( u8 voice_index, const PlayInfo3D& effectInfo3D )
  {
    VoicePlayInfo vi = {0};

    vi.effect_enable = 0;
    if(effectInfo3D.effect_enable3D & EFFECT_BIT_VOLUME_3D){ vi.effect_enable |= VEFFECT_BIT_VOLUME; }
    if(effectInfo3D.effect_enable3D & EFFECT_BIT_PITCH_3D){ vi.effect_enable |= VEFFECT_BIT_PITCH; }
    if(effectInfo3D.effect_enable3D & EFFECT_BIT_PAN_3D){ vi.effect_enable |= VEFFECT_BIT_PAN; }
    if(effectInfo3D.effect_enable3D & EFFECT_BIT_POS_3D){ vi.effect_enable |= VEFFECT_BIT_POS; }
    if(effectInfo3D.effect_enable3D & EFFECT_BIT_VELOCITY_3D){ vi.effect_enable |= VEFFECT_BIT_VELOCITY; }

    vi.volume = effectInfo3D.volume3D;
    vi.pitch = effectInfo3D.pitch3D;
    vi.pan = effectInfo3D.pan3D;
    vi.pos3D = effectInfo3D.pos3D;
    vi.velocity3D = effectInfo3D.velocity3D;

    ChangeVoiceEffect(voice_index, vi);
  }

  // 鳴き声のボリュームを操作する
  extern void ChangeVoiceVolume( u8 voice_index, const f32 value, const u32 change_frame = 0 );
  inline void ChangeVoice3DActorVolume( u8 voice_index, const f32 value ){ ChangeVoiceVolume(voice_index, value, 0); }

  // 鳴き声のピッチを操作する
  extern void ChangeVoicePitch( u8 voice_index, const f32 value );
  inline void ChangeVoice3DActorPitch( u8 voice_index, const f32 value ){ ChangeVoicePitch(voice_index, value); }

  // 鳴き声の水平方向パンを操作する
  extern void ChangeVoicePan( u8 voice_index, const f32 value );
  inline void ChangeVoice3DActorPan( u8 voice_index, const f32 value ){ ChangeVoicePan(voice_index, value); }

  // 鳴き声の垂直方向パンを操作する
  extern void ChangeVoiceSurroundPan( u8 voice_index, const f32 value );

  // 鳴き声のテンポを操作する
  extern void ChangeVoiceTempo( u8 voice_index, const f32 value );

  // 3Dアクター再生の鳴き声のポジションを操作する
  extern void ChangeVoice3DPos( u8 voice_index, const gfl2::math::VEC3& pos );
  inline void ChangeVoice3DActorPos( u8 voice_index, const gfl2::math::VEC3& pos ){ ChangeVoice3DPos(voice_index, pos); }

  // 3Dアクター再生の鳴き声のベロシティを操作する
  extern void ChangeVoice3DVelocity( u8 voice_index, const gfl2::math::VEC3& velocity );
  inline void ChangeVoice3DActorVelocity( u8 voice_index, const gfl2::math::VEC3& velocity ){ ChangeVoice3DVelocity(voice_index, velocity); }

  // 鳴き声のセンド量を
  extern b32 SetVoiceFxSend( u8 voice_index, nw::snd::AuxBus busID, float send );

  //---------------------------------------------------------------------------
  // プレイヤ
  //---------------------------------------------------------------------------
  extern u32 GetPlayingCount( u32 soundID );                  // 指定したプレイヤで再生中のサウンドの個数を取得する
  extern f32 GetPlayerVolume( u32 soundID );                  // プレイヤボリュームを取得する
  extern void SetPlayerVolume( u32 soundID, f32 volume );     // プレイヤボリュームを変更する
  extern void StopPlayer( u32 soundID, u32 fo_frame );        // 指定したプレイヤで再生中の全てのサウンドを停止する
  extern void PausePlayer( u32 soundID, u32 fo_frame );       // 指定したプレイヤで再生中の全てのサウンドを一時停止する
  extern void ReleasePlayer( u32 soundID, u32 fi_frame );     // 指定したプレイヤの一時停止を解除する

  //プレイヤーボリュームの変更 ※変更したら必ず1.0に戻すこと
  extern void ChangePlayerVolume( SoundPlayerIndex playerIdx, f32 volume, u32 frame );  //指定した管理Indexのプレイヤーボリュームを変更する
  extern void ChangeBGMPlayerVolume( f32 volume, u32 frame ); // BGMのプレイヤボリュームを変更する
  extern void ChangeAllPlayerVolume( f32 volume, u32 frame ); // BGM/SE全プレイヤーのボリュームを変更する
  //---------------------------------------------------------------------------
  /**
   * @brief　指定プレイヤーにフェードリクエストがあるか？
   * 
   * @fix GFNMCat[5590]反転世界イベント終了後にアサート
   * 環境音ボリュームチェンジリセットをフレーム指定で行うと、フリームーブ時のチェックにひっかっかる
   */
  //---------------------------------------------------------------------------
  extern bool IsRunningPlayerVolumeChange( SoundPlayerIndex playerIdx );

  //---------------------------------------------------------------------------
  // 汎用サウンドデータ
  //---------------------------------------------------------------------------
  /**
   * @brief 外部管理のサウンドヒープにサウンドデータを読み込む
   * @param sound_heap サウンドデータを配置するサウンドヒープ
   * @param soundID 読む込むサウンドデータの中間ID
   * @return 読み込みに成功したか？
   */
  extern b32 LoadSoundDataToExternalHeap( gfl2::sound::SoundHeap* sound_heap, u32 soundID );

  //---------------------------------------------------------------------------
  // SEサウンドデータ
  //---------------------------------------------------------------------------
  /**
   * @brief SEのサウンドデータを, システム管理のサウンドヒープに積む
   * @param soundID  読み込むサウンドデータIDの中間ID
   */
  extern void LoadAndPushSESoundData( u32 soundID );
  // 非同期版
  extern b32 LoadAndPushSESoundDataAsync( gfl2::heap::HeapBase* thread_heap, u32 soundID );
  extern b32 WaitLoadAndPushSESoundDataAsync(void);

  /**
   * @brief 1階層下にあるSEのサウンドデータを, システム管理のサウンドヒープから削除する
   * @param soundID  削除するサウンドデータIDの中間ID
   */
  extern void PopAndUnloadSESoundData( u32 soundID );

  /**
   * @brief 全階層にあるSEのサウンドデータを, システム管理のサウンドヒープから削除する
   *
   * @note  BTS4850 SoftwareResetから呼ばれるSoundSystemのFinalize用
   */
  extern void PopAndUnloadSESoundDataAll( void );

  //---------------------------------------------------------------------------
  // アプリケーション常駐サウンドデータ
  //---------------------------------------------------------------------------
  /**
   * @brief アプリケーション常駐のサウンドを積む
   */
  extern void LoadAndPushAppGlobalSE( void );

  /**
   * @brief アプリケーションの常駐SEをヒープから削除する
   */
  extern void PopAndUnloadAppGlobalSE( void );

  //---------------------------------------------------------------------------
  // バトル常駐サウンドデータ
  //---------------------------------------------------------------------------
  /**
   * @brief バトルの常駐SEを, システム管理のサウンドヒープに積む
   */
  extern void LoadAndPushBattleGlobalSE( void );

  /**
   * @brief バトル常駐SEの非同期処理リクエスト（読み込み＋サウンドヒープに積む処理）
   * @param pThreadHeap   スレッド生成に利用するヒープ
   */
  extern void LoadAndPushBattleGlobalSEAsync( gfl2::heap::HeapBase * pThreadHeap );
  /**
   * @brief バトル常駐SEの非同期処理待ち（読み込み＋サウンドヒープに積む処理）
   */
  extern b32 WaitLoadAndPushBattleGlobalSEAsync(void);

  /**
   * @brief システム管理のサウンドヒープに積んだバトルの常駐SEをポップする
   */
  extern void PopAndUnloadBattleGlobalSE( void );

  //---------------------------------------------------------------------------
  // エフェクトの追加
  //  XYでは下記になっている
  //        BUS_A→リバーブ専用
  //        BUS_B→鳴き声の波形を読み取るクラス専用
  //---------------------------------------------------------------------------
  /**
   * @brief BUS_Aにリバーブエフェクトを追加する
   *
   * @param reverb_buffer_heap  リバーブバッファー用ヒープ
   * @param param               設定構造体
   *
   * @return true追加できた falseできなかった
   */
#if defined(GF_PLATFORM_CTR)
  extern b32 AppendReverbEffect( gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param );
#elif defined(GF_PLATFORM_WIN32)
  extern b32 AppendReverbEffect( gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverbStd::ReverbStdParam & param );
#endif

  /**
   * @brief BUS_Aのリバーブエフェクトを破棄する
   *
   * @param fadeTimes フェードタイム（ミリ秒）GFBTS2506対策でfadeTimesは強制的に0になる
   *
   * 詳細は GFBTSとPMSoundSystem::RemoveReverbEffect()の関数コメントを参照のこと
   */
  extern void RemoveReverbEffect( int fadeTimes );
  /**
   * @brief BUS_Bに鳴き声用の波形読み取りエフェクトを追加する
   *
   * @param effect  読み取りエフェクト
   *
   * @return true追加できた falseできなかった
   */
  extern b32 AppendEffect( nw::snd::FxBase *effect );
  /**
   * @brief BUS_Bのエフェクトを破棄する
   *
   * @param fadeTimes フェードタイム（ミリ秒）
   */
  extern void RemoveEffect( int fadeTimes );

  //---------------------------------------------------------------------------
  // その他
  //---------------------------------------------------------------------------
  /**
   * @brief 現在なっているストリームサウンドの再生位置を取得する
   * @return 再生位置（サンプル数） ハンドルが無効の場合は負の値
   */
  extern long GetStreamSoundPlaySamplePosition( void );
  /**
   * @brief サンプリングレートを取得する
   *
   * @return  サンプリングレート  ハンドルが無効の場合は負の値
   */
  extern int GetStreamSoundSampleRate( void );

  /**
   * @brief ストリームデータのループフラグを取得する
   *
   * @return  ストリームデータのループフラグを取得する
   * @note ハンドルが無効の場合は負の値 有効ならtrue 無効ならfalse
   */
  extern int GetStreamSoundLoopFlag( void );

  /**
   * @brief ストリームデータのループ開始位置を取得する
   *
   * @return  ストリームデータのループ開始位置を取得する
   * @note ハンドルが無効の場合は負の値
   */
  extern int GetStreamSoundLoopStartPos( void );


  /**
   * @brief ストリームデータのループ終了位置を取得する
   *
   * @return  ストリームデータのループ終了位置を取得する
   * @note ハンドルが無効の場合は負の値
   */
  extern int GetStreamSoundLoopEndPos( void );

} // namespace Sound


#endif // __H_SOUND_H__

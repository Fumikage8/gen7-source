//=============================================================================
/**
 * @file  PMSoundSystem.h
 * @brief ポケモンサウンド制御クラス
 * @author Miyuki Iwasawa
 * @date   2015.03.13
 *
 * @note
 * サウンドハンドルを管理し, ハンドルの概念をユーザから隠蔽する.
 * SoundLevelManagerを介したBGM操作を行い, BGMの階層構造を隠蔽する.
 */
//=============================================================================
#ifndef __PMSOUNDSYSTEM_H__
#define __PMSOUNDSYSTEM_H__
#pragma once

#include <sound/include/gfl2_SoundSystem.h>

#include "base/include/gfl2_Singleton.h"
#include "heap/include/gfl2_Heap.h"


//初期化タイミングだとAssertコールバックが未定義でデバッグ版だと停止しないので無限ループを仕込んでいる 
#define SOUND_SYSTEM_INITIALIZE_ASSERT(msg) GFL_ASSERT_STOP_MSG(0,msg); for(;;){}

namespace Sound {

  class BGMLevelManager;
  class SELevelManager;
  class EnvSEManager;


  class PMSoundSystem : public gfl2::base::Singleton<PMSoundSystem>
  {
    GFL_FORBID_COPY_AND_ASSIGN( PMSoundSystem );

    public:

#if PM_DEBUG
    // ROMアクセス頻度検出用デバッグ機能
    static int m_bRomAccessCheckMode;

    enum
    {
      DEBUG_ROMACCESS_MODE_IDLE,
      DEBUG_ROMACCESS_MODE_PRINT,
      DEBUG_ROMACCESS_MODE_ASSERT,
      DEBUG_ROMACCESS_MODE_MAX,
    };

#endif


    // サウンドハンドル
    enum HandleIndex 
    {
      SHANDLE_BGM_LV1,   // BGM 第一階層
      SHANDLE_BGM_LV2,   // BGM 第二階層
      SHANDLE_BGM_LV3,   // BGM 第三階層
      SHANDLE_BGM_LV4,   // BGM 第四階層
      SHANDLE_SE_00,     // SE
      SHANDLE_SE_01,     // SE
      SHANDLE_SE_02,     // SE
      SHANDLE_SE_03,     // SE
      SHANDLE_SE_04,     // SE
      SHANDLE_SE_05,     // SE
      SHANDLE_SE_06,     // SE
      SHANDLE_SE_07,     // SE
      SHANDLE_SE_08,     // SE
      SHANDLE_SE_09,     // SE
      SHANDLE_SE_10,     // SE
      SHANDLE_VOICE_00,  // 鳴き声
      SHANDLE_VOICE_01,  // 鳴き声
      SHANDLE_VOICE_02,  // 鳴き声
      SHANDLE_VOICE_03,  // 鳴き声
      SHANDLE_VOICE_04,  // 鳴き声
      SHANDLE_VOICE_05,  // 鳴き声
      SHANDLE_VOICE_06,  // 鳴き声
      SHANDLE_VOICE_07,  // 鳴き声
      SHANDLE_VOICE_08,  // 鳴き声
      SHANDLE_VOICE_09,  // 鳴き声
      SHANDLE_VOICE_10,  // 鳴き声
      SHANDLE_VOICE_11,  // 鳴き声
      SHANDLE_NUM,

      SHANDLE_SE_HEAD    = SHANDLE_SE_00,
      SHANDLE_SE_TAIL    = SHANDLE_SE_10,
      SE_HANDLE_NUM      = SHANDLE_SE_TAIL - SHANDLE_SE_HEAD + 1,

      SHANDLE_VOICE_HEAD = SHANDLE_VOICE_00,
      SHANDLE_VOICE_TAIL = SHANDLE_VOICE_11,
      VOICE_HANDLE_NUM   = SHANDLE_VOICE_TAIL - SHANDLE_VOICE_HEAD + 1,
      VOICE_HANDLE_COMMON_NUM = 2,  //ゲーム内常駐するボイスハンドル数

      SOUND_3D_ACTOR_NUM = 50,  //6, 通常SE用
      SOUND_3D_ACTOR_VOICE_NUM = VOICE_HANDLE_NUM, //ポケモンボイス専用
      SOUND_3D_ACTOR_TOTAL_NUM = SOUND_3D_ACTOR_NUM+SOUND_3D_ACTOR_VOICE_NUM, //アクタートータル数
      SOUND_3D_ACTOR_INDEX_VOICE_START = SOUND_3D_ACTOR_NUM, //ボイス用アクタースタートindex
    };


    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ・基本動作
    //-------------------------------------------------------------------------
    PMSoundSystem( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap );
    ~PMSoundSystem();
    void Update( void ); 

    //-------------------------------------------------------------------------
    // サウンドデータの読み込み
    //-------------------------------------------------------------------------
    void LoadAndPushSESoundData( u32 soundID );  // SEのサウンドデータをシステム管理のサウンドヒープに積む
    void PopAndUnloadSESoundData( u32 soundID ); // 最後に積んだSEのサウンドデータをシステム管理のサウンドヒープから削除する
    void PopAndUnloadSESoundDataAll( void ); // 全SEのサウンドデータをシステム管理のサウンドヒープから削除する BTS4850
    b32 LoadSoundDataToExternalHeap( gfl2::sound::SoundHeap* sound_heap, u32 soundID ); // 外部管理のサウンドヒープにサウンドデータを読み込む

    // 非同期版
    b32 LoadAndPushSESoundDataAsync( gfl2::heap::HeapBase * thread_heap, u32 soundID );  // SEのサウンドデータをシステム管理のサウンドヒープに積む
    b32 WaitLoadAndPushSESoundDataAsync(void);

    //-------------------------------------------------------------------------
    // 外部読み込み　サウンドデータの設定
    //-------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドデータマネージャへサウンドデータを設定する。
     *
     *	@param    fileID    セットするデータのファイルIDです。
     *	@param    address   アドレス
     */
    //-----------------------------------------------------------------------------
    void SetSoundFileAddress( u32 fileID, const void * address );
     

    //-------------------------------------------------------------------------
    // プレイヤ
    //-------------------------------------------------------------------------
    u32 GetPlayerPlayingCount( u32 soundID ) const;          // 指定したプレイヤで再生中のサウンドの個数を取得する
    f32 GetPlayerVolume( u32 soundID ) const;                // プレイヤボリューム取得する
    void SetPlayerVolume( u32 soundID, f32 volume );         // プレイヤボリュームを変更する
    void StopPlayerSound( u32 soundID, u32 fade_out_frame ); // 指定したプレイヤで再生中の全てのサウンドを停止する
    void PausePlayerSound( u32 soundID, b32 pause_on, u32 fade_frame ); // 指定したプレイヤで再生中の全てのサウンドを一時停止・再開する
		//--------------------------------------------------------------------------
		// サウンドモード変更
		//--------------------------------------------------------------------------
    gfl2::sound::SoundSystem::OutputMode GetOutputMode( void );
		void SetOutputModeMono( void );
		void SetOutputModeStereo( void );
		void SetOutputModeSurround( void );

		//--------------------------------------------------------------------------
		// マスターボリューム変更
		//		マスターボリュームスイッチフラグ on = true. off = false
		//--------------------------------------------------------------------------
		void SoundMasterVolumeSW(b32 OnOff);

    //-------------------------------------------------------------------------
    // BGM
    //-------------------------------------------------------------------------
    void PlayBGM( u32 soundID, u32 fadein_frame ); // BGMを再生する
    void StopBGM( u32 fadeout_frame );             // BGMを停止する
    void PrepareBGM( u32 soundID , u32 startMilliSec = 0 );                // BGMの再生準備をする
    b32 IsBGMPrepared( void ) const;              // BGMの再生準備が完了しているかどうかを調べる
    void PlayPreparedBGM( u32 fadein_frame );      // PrepareBGM()で準備したBGMを再生する
    b32 IsBGMPlaying( void ) const;               // BGMが再生中かどうかを調べる
    b32 IsBGMPlaying( u32 soundID ) const;        // 任意のBGMが再生中かどうかを調べる
    b32 IsBGMValid() const;                       // カレントBGMが有効(PlayもしくはPause状態)かどうか？
    void PauseBGM( u32 fadeout_frame );            // BGMを一時停止する
    void PauseReleaseBGM( u32 fadein_frame );      // BGMの一時停止を解除する
    b32 IsPauseBGM( void ) const;                 // BGMが一時停止中かどうかを調べる
    b32 IsBGMFade( void ) const;                  // BGMがフェード中かどうかを調べる
    void ChangeBGMVolume( f32 volume, u32 change_frame ); // BGMの音量を変更する
    void ChangeBGMPitch( f32 value );                     // BGMのピッチを変更する
    void ChangeBGMPitchByEqualTemperament( s8 level );    // BGMのピッチを12平均律に即した音程に変更する
    void ChangeBGMPan( f32 value );                       // BGMの水平方向パンを変更する
    void ChangeBGMTrackPan( u32 track_bit, f32 value );   // BGMの特定トラックの水平方向パンを変更する
    void ChangeBGMSurroundPan( f32 value );               // BGMの垂直方向パンを変更する
    void ChangeBGMTrackVolume( u32 track_bit, f32 volume, u32 change_frame ); // BGMの特定トラックの音量を変更する
    void ChangeBGMTrackSurroundPan( u32 track_bit, f32 value );               // BGMの特定トラックの垂直方向パンを変更する
    void ChangeBGMTrackPitch( u32 track_bit, f32 value );                     // BGMの特定トラックのピッチを変更する
    void ChangeBGMTrackPitchByEqualTemperament( u32 track_bit, s8 level );    // BGMの特定トラックのピッチを12平均律に即した音程に変更する
    void ChangeBGMTempoRatio( f32 value ); // BGMのテンポを変更する
    b32 PushBGM( void );                  // 再生中のBGMを退避する
    b32 PopBGM( u32 fadein_frame );       // 退避したBGMを復元する
    b32 PopBGMOnly( void );							 // 退避したBGMを復元する(再生再開しない)
    b32 PrepareBGMContinue( u32 soundID ); // 現在再生中の曲位置から次の曲の再生準備をする
    u32 GetPushedBGMCount( void ) const;   // 退避中のBGMの数を取得する
    u32 GetMaxPushBGMCount( void ) const;  // 退避可能なBGMの最大数を取得する

    //-------------------------------------------------------------------------
    // SE
    //-------------------------------------------------------------------------
     // SEを再生する
    void PlaySE(
        HandleIndex handle, 
        u32 soundID,
        u32 fadein_frame = 0,
        nw::snd::SoundStartable::StartInfo* start_info = NULL );
    void PlaySEVolume( HandleIndex handle, u32 soundID, f32 volume );     // SEを再生する( 音量指定あり )
    b32 IsSEPlaying( HandleIndex handle ) const;                         // 何でもいいので再生中かどうかを調べる
    b32 IsSEPlaying( HandleIndex handle, u32 soundID ) const;            // 指定したSEが再生中かどうかを調べる
    void StopSE( HandleIndex handle, u32 fadeout_frame = 0 );             // SEを停止する
    void StopAllSE( void );                                               // 全てのSEを停止する
    void ChangeSEVolume( HandleIndex handle, f32 volume, u32 frame = 0 ); // SEの音量を変更する
    void ChangeSEPitch( HandleIndex handle, f32 value );                  // SEのピッチを変更する
    void ChangeSEPan( HandleIndex handle, f32 value );                    // SEの水平方向パンを変更する
    void ChangeSESurroundPan( HandleIndex handle, f32 value );            // SEの垂直方向パンを変更する
    void ChangeSETempoRatio( HandleIndex handle, f32 value );             // SEのテンポを変更する
		void ChangeSETranspose( HandleIndex handle, s8 value, u32 bit );			// SEのトランスポーズ
    void ChangeSETrackVariable( HandleIndex handle, int trackNo, int varNo, s16 value );			// SEのトラック変数書き込み
    s16  GetSETrackVariable( HandleIndex handle, int trackNo, int varNo );			// SEのトラック変数取得
    EnvSEManager*   GetEnvSEManager( void ) const;
    SELevelManager* GetSELevelManager( void ) const;

    //-------------------------------------------------------------------------
    // VOICE
    //-------------------------------------------------------------------------
    void PlayVoice( HandleIndex handle, u32 soundID );                   // VOICEを再生する
    void PlayVoiceVolume( HandleIndex handle, u32 soundID, f32 volume ); // VOICEを再生する( 音量指定あり )
    b32 IsVoicePlaying( HandleIndex handle, u32 soundID ) const;        // VOICEが再生中かどうかを調べる
    void StopVoice( HandleIndex handle, u32 soundID, u32 frame = 0 );    // VOICEを停止する
    void StopAllVoice( void );                                           // 全てのVoiceを停止する
    void ChangeVoiceVolume( HandleIndex handle, f32 volume, u32 change_frame = 0 ); // Voiceの音量を変更する
    void ChangeVoicePitch( HandleIndex handle, f32 value );       // VOICEのピッチを変更する
    void ChangeVoicePan( HandleIndex handle, f32 value );         // VOICEの水平方向パンを変更する
    void ChangeVoiceSurroundPan( HandleIndex handle, f32 value ); // VOICEの垂直方向パンを変更する
    void ChangeVoiceTempoRatio( HandleIndex handle, f32 value );  // VOICEのテンポを変更する

    //-------------------------------------------------------------------------
    // 3Dサウンド
    //-------------------------------------------------------------------------
    void Set3DParam( const gfl2::sound::SoundSystem::Sound3DParam& param );
    void Get3DParam( gfl2::sound::SoundSystem::Sound3DParam* buffer ) const;

    void SetListenerParam( const gfl2::sound::SoundSystem::ListenerParam& param );
    void GetListenerParam( gfl2::sound::SoundSystem::ListenerParam* buffer ) const;
    void SetListenerMatrix( const gfl2::math::MTX34& matrix );
    void SetListenerPos( const gfl2::math::VEC3& pos, const gfl2::math::VEC3& up, const gfl2::math::VEC3& dir );
    void SetListenerVelocity( const gfl2::math::VEC3& velocity );

    b32 Play3DActor( u8 actor_index, u32 soundID, const nw::snd::SoundStartable::StartInfo* start_info = NULL );
    b32 PlayVoice3DActor( u8 actor_index, HandleIndex handle, u32 soundID );
    b32 Prepare3DActor( u8 actor_index, u32 soundID, const nw::snd::SoundStartable::StartInfo* start_info = NULL );
    void PlayPrepared3DActor( u8 actor_index );
    b32 IsPrepared3DActor( u8 actor_index );

    void Stop3DActor( u8 actor_index, s32 fadeFrames );
    u32 Get3DActorPlayingCount( u8 actor_index, u8 actorPlayerIdx ) const;
    u32 Get3DActorPlayingCount( u8 actor_index ) const;
    b32 IsPlaying3DActor( u8 actor_index, u8 actorPlayerIdx ) const;
    b32 IsPlaying3DActor( u8 actor_index ) const;
    
    void Pause3DActor( u8 actor_index, b32 pauseOn, s32 fadeFrames );
    void Set3DActorVolume( u8 actor_index, f32 volume );
    f32 Get3DActorVolume( u8 actor_index ) const;
    void Set3DActorPitch( u8 actor_index, f32 pitch );
    f32 Get3DActorPitch( u8 actor_index ) const;
    void Set3DActorPan( u8 actor_index, f32 pan );
    f32 Get3DActorPan( u8 actor_index ) const;
    void Set3DActorPos( u8 actor_index, const gfl2::math::VEC3& pos );
    void Get3DActorPos( u8 actor_index, gfl2::math::VEC3* pos ) const;
    void Set3DActorVelocity( u8 actor_index, const gfl2::math::VEC3& velocity );

    //-------------------------------------------------------------------------
    // エフェクト
    //-------------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
    b32 AppendReverbEffect( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param );
#elif defined(GF_PLATFORM_WIN32)
    b32 AppendReverbEffect( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverbStd::ReverbStdParam & param );
#endif
    void RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes ); //GFBTS2506 対策でfadeTimesは内部で強制的に0になる
    b32 AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect );
    void RemoveEffect( nw::snd::AuxBus busID, int fadeTimes );



    private:

    static const u32 BGM_HEAPSIZE = 128;
    static const u32 SE_HEAPSIZE = 430 * 1024;/*450 * 1024*/  //mimijiで変更。常駐SEには手を入れない前提で２０Kカット

    b32                    m_BGMControlEnable;  // BGM 操作が有効かどうか
    gfl2::sound::SoundSystem*  m_soundSystem;
    gfl2::sound::SoundHeap*    m_soundHeap[2];  // BGM, SE の2つ
    BGMLevelManager*        m_BGMLevelManager;
    SELevelManager*         m_SELevelManager;
    EnvSEManager*           m_EnvSEManager;

#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection m_CSectionForSoundControl;
#endif

    void CreateSoundSystem( gfl2::heap::HeapBase*, gfl2::heap::HeapBase* );
    void CreateSoundHeap( gfl2::heap::HeapBase*, gfl2::heap::HeapBase* );
    void CreateSoundLevelManager( gfl2::heap::HeapBase* );
    void PlayVoiceCore( HandleIndex , u32 , f32 );
    b32 IsSEHandle( HandleIndex ) const;
    b32 IsVoiceHandle( HandleIndex ) const;

#if PM_DEBUG
    // ROMアクセス頻度検出用デバッグ機能
    static b32 m_bRomAccessCheckStart;
    static s64 m_RomAccessStartTime;
    static s64 m_RomBeforeAccessTime;
#endif


    public:

     // サウンドシステムを取得する
    gfl2::sound::SoundSystem* GetSoundSystem( void );
     //現在なっているサウンドハンドルを返す
    nw::snd::SoundHandle* GetSoundHandleOnCurrent( void );

  //--------------------------------------------------------------------------
  /**
   * @brief 現在なっているストリームサウンドの再生位置を取得する
   * @return 再生位置（サンプル数） 失敗した場合は負の値
   */
  //--------------------------------------------------------------------------
  int GetStreamSoundPlaySamplePosition( void );

  //--------------------------------------------------------------------------
  /**
   * @brief サンプリングレートを取得する
   * @return 再生位置（サンプル数） 失敗した場合は負の値
   */
  //--------------------------------------------------------------------------
  int GetStreamSoundSampleRate( void );

  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループフラグを取得する
   *
   * @return  ストリームデータのループフラグを取得する
   * @note ハンドルが無効の場合は負の値 有効ならtrue 無効ならfalse
   */
  //--------------------------------------------------------------------------
  b32 GetStreamSoundLoopFlag( void );

  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループ開始位置を取得する
   *
   * @return  ストリームデータのループ開始位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  //--------------------------------------------------------------------------
  int GetStreamSoundLoopStartPos( void );

  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループ終了位置を取得する
   *
   * @return  ストリームデータのループ終了位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  //--------------------------------------------------------------------------
  int GetStreamSoundLoopEndPos( void );

#if PM_DEBUG 
    void SetBGMControlEnable( b32 enable ); // BGM 操作が可能かどうかをセットする
    b32 IsBGMControlEnable( void );         // BGM 操作が有効か？

    void DebugCheckRomAccess(HandleIndex handle, u32 middleID) const;

    void DumpHeapInfo(void);
#endif

  };



} // namespace Sound


#endif // __PMSOUNDSYSTEM_H__

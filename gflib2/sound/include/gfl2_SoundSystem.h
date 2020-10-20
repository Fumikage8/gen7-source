//=============================================================================
/**
 * @file gfl2_SoundSystem.h
 * @brief サウンドシステム
 * @author Miyuki Iwasawa
 * @date 2015.02.19
 */
//=============================================================================
#ifndef __GFL_SOUNDSYSTEM_H__
#define __GFL_SOUNDSYSTEM_H__
#pragma once

#include <nw/snd.h>

#include "heap/include/gfl2_Heap.h"
#include "math/include/gfl2_Math.h"
#include "base/include/gfl2_Singleton.h"


namespace gfl2 {
  namespace sound {

    class SoundHeap;
    class SoundDataManagerThread;

    class SoundSystem : public gfl2::base::Singleton<SoundSystem> 
    {
      GFL_FORBID_COPY_AND_ASSIGN( SoundSystem ); 
      public: 
      // サウンドIDの無効値
      static const u32 INVALID_SOUND_ID = 0xffffffff; 

      // サウンドアーカイブの読み込み方
      enum SoundArchiveType {
        SOUND_ARCHIVE_TYPE_ROM,     // ROM/FS上のアーカイブを使用する
        SOUND_ARCHIVE_TYPE_MEMORY,  // メモリ上のアーカイブを使用する
      }; 

      // 初期化パラメータ
      struct InitParam 
      {
        gfl2::heap::HeapBase* pAppHeap;            // システムが使用するアプリケーションヒープ
        gfl2::heap::HeapBase* pDevHeap;            // システムが使用するデバイスヒープ
        u32                  soundHeapSize;       // サウンドヒープのサイズ
        u8                   soundHandleNum;      // 管理するサウンドハンドルの数
        u8                   sound3DActorNum;     // 3Dアクターの数
        SoundArchiveType     soundArchiveType;    // サウンドアーカイブがどこにあるのか
        const char*          bcsarFilePath;       // CTRサウンドアーカイブファイルのパス( SOUND_ARCHIVE_TYPE_HIO の場合は絶対パスを指定 )
        const char*          bfsarFilePath;       // Cafeサウンドアーカイブファイルのパス( SOUND_ARCHIVE_TYPE_HIO の場合は絶対パスを指定 )
        const void*          bcsarData;           // サウンドアーカイブのデータ( SOUND_ARCHIVE_TYPE_MEMORY を指定した場合のみ有効 )
        bool                 aacUse;              // AAC サウンドを使用するか？
        char*                aacDspComponentPath; // AAC デコード用 DSP コンポーネントのパス
        s32                  soundThreadPriority; // サウンドスレッドのプライオリティ
        s32                  soundDataLoadThreadPriority; // サウンドデータロードスレッドのプライオリティ

        InitParam( void ) :
          pAppHeap( NULL ), 
          pDevHeap( NULL ),
          soundHeapSize( 0 ),
          soundHandleNum( 0 ),
          sound3DActorNum( 0 ), 
          soundArchiveType( SOUND_ARCHIVE_TYPE_ROM ),
          bcsarFilePath( NULL ),
          bcsarData( NULL ),
          aacUse( false ),
          aacDspComponentPath( NULL ),
          soundThreadPriority( nw::snd::SoundSystem::SoundSystemParam::DEFAULT_SOUND_THREAD_PRIORITY ),
          soundDataLoadThreadPriority( nw::snd::SoundSystem::SoundSystemParam::DEFAULT_TASK_THREAD_PRIORITY )
        {}
      }; 

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param const InitParam& initParam 初期化パラメータ
       */
      //-----------------------------------------------------------------------
      SoundSystem( const InitParam& initParam );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~SoundSystem(); 

      //-----------------------------------------------------------------------
      /**
       * @brief フレーム処理( 毎フレーム呼んでください )
       */
      //-----------------------------------------------------------------------
      void Update( void );

      // ロード対象を選択するためのフラグ
      enum LoadFlags{
        LOAD_SEQ        = nw::snd::SoundDataManager::LOAD_SEQ,  // シーケンスサウンド
        LOAD_WAVE_SOUND = nw::snd::SoundDataManager::LOAD_WSD,  // ウェーブサウンド
        LOAD_BANK       = nw::snd::SoundDataManager::LOAD_BANK, // バンク
        LOAD_WAVE_ARC   = nw::snd::SoundDataManager::LOAD_WARC, // 波形アーカイブ
        LOAD_ALL        = nw::snd::SoundDataManager::LOAD_ALL,  // 関連する全てのファイル
      }; 

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする
       *
       * @param itemID    ロードするデータのアイテムID
       * @param loadFlag  ロードするデータを論理和で指定( gfl2::sound::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundData( u32 itemID, LoadFlags loadFlag = LOAD_ALL );

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドアーカイブ中のデータを指定したサウンドヒープにロードする
       *
       * @param[out] dest      読み込んだデータを配置するサウンドヒープ
       * @param[in]  itemID    ロードするデータのアイテムID
       * @param[in]  loadFlag  ロードするデータを論理和で指定( gfl2::sound::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundData( SoundHeap* dest, u32 itemID, LoadFlags loadFlag = LOAD_ALL );
      
      //----------------------------------------------------------------------------
      /**
       * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする 非同期版
       *
       * @param thread_heap スレッドヒープ
       * @param priority    スレッド優先順位
       * @param itemID      ロードするデータのアイテムID
       * @param loadFlag    ロードするデータを論理和で指定( gfl2::sound::SoundSystem::LOAD_xxxx )
       *
       *	@retval true    読み開始
       *	@retval false   読み開始失敗
       */
      //-----------------------------------------------------------------------------
      bool LoadSoundDataAsync( gfl2::heap::HeapBase * thread_heap, s32 priority, u32 itemID, LoadFlags loadFlag = LOAD_ALL );

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする 非同期版
       *
       * @param[out] dest      読み込んだデータを配置するサウンドヒープ
       * @param[in]  thread_heap スレッドヒープ
       * @param[in]  priority    スレッド優先順位
       * @param[in]  itemID    ロードするデータのアイテムID
       * @param[in]  loadFlag  ロードするデータを論理和で指定( gfl2::sound::SoundSystem::LOAD_xxxx )
       */
      //-----------------------------------------------------------------------
      bool LoadSoundDataAsync( SoundHeap* dest, gfl2::heap::HeapBase * thread_heap, s32 priority, u32 itemID, LoadFlags loadFlag = LOAD_ALL );



      //----------------------------------------------------------------------------
      /**
       *	@brief  サウンドアーカイブ中のデータを指定したサウンドヒープにロードする　非同期　待ち
       *
       *	@retval true  ロード完了
       *	@retval false ロード中
       */
      //-----------------------------------------------------------------------------
      bool WaitLoadSoundDataAsync(void);

      //----------------------------------------------------------------------------
      /**
       *	@brief  サウンドアーカイブ　非同期読み込みの結果を取得
       *
       *	@retval true  成功
       *	@retval false 失敗
       */
      //-----------------------------------------------------------------------------
      bool GetLoadSoundDataAsyncResult(void) const;


      //----------------------------------------------------------------------------
      /**
       *	@brief  サウンドデータマネージャへサウンドデータを設定する。
       *
       *	@param    fileID    セットするデータのファイルIDです。
       *	@param    address   アドレス
       */
      //-----------------------------------------------------------------------------
      void SetSoundFileAddress( u32 fileID, const void * address );

      //----------------------------------------------------------------------------
      /**
       *	@brief  サウンドデータマネージャへサウンドグループファイルを登録する。
       *
       *	@param    groupFile グループファイル
       *	@param    size      サイズ
       */
      //-----------------------------------------------------------------------------
      void SetSoundFileAddressInGroupFile( const void * groupFile, size_t size );

      //----------------------------------------------------------------------------
      /**
       *	@brief  サウンドデータマネージャからサウンドグループファイルを登録解除する
       *
       *	@param    groupFile グループファイル
       *	@param    size      サイズ
       */
      //-----------------------------------------------------------------------------
      void ClearSoundFileAddressInGroupFile( const void * groupFile, size_t size );


      //-----------------------------------------------------------------------
      /**
       * @brief 再生する
       *
       * @param handleIdx    再生後の制御に使用するハンドルのインデックス
       * @param itemID       再生するサウンドのアイテムNo.
       * @param fadeInFrames フェードインにかけるフレーム数
       * @param startInfo    再生開始詳細設定構造体
       *
       * @return 再生が成功した場合 true
       */
      //-----------------------------------------------------------------------
      bool Play( u8 handleIdx, u32 itemID, s32 fadeInFrames = 0, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );

      //-----------------------------------------------------------------------
      /**
       * @brief 再生の準備を開始する
       *
       * @param handleIdx   再生後の制御に使用するハンドルのインデックス
       * @param itemID      再生するサウンドのアイテムNo.
       * @param startInfo   再生開始詳細設定構造体
       *
       * @retval true   成功
       * @retval false  失敗
       */
      //-----------------------------------------------------------------------
      bool PrepareSound( u8 handleIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );

      //-----------------------------------------------------------------------
      /**
       * @brief PrepareSound()で開始した再生準備が完了したかどうかを調べる
       *
       * @param handleIdx  再生するサウンドハンドルのインデックス
       *
       * @retval true   再生準備が完了
       * @retval false  再生準備が未完了
       */
      //-----------------------------------------------------------------------
      bool IsPrepared( u8 handleIdx );

      //-----------------------------------------------------------------------
      /**
       * @brief PrepareSound()で準備したサウンドを再生する
       *
       * @param handleIdx     再生するサウンドハンドルのインデックス
       * @param fadeInFrames  フェードインにかけるフレーム数
       *
       * @note 再生準備が完了していない場合, 準備完了を待ってから再生を開始します.
       */
      //-----------------------------------------------------------------------
      void PlayPrepared( u8 handleIdx, s32 fadeInFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief 停止する
       *
       * @param handleIdx     制御を行うハンドルのインデックス
       * @param fadeOutFrames フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void Stop( u8 handleIdx, s32 fadeOutFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief 全てのサウンドを停止する
       * @param fadeOutFrames フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void StopAll( s32 fadeOutFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief 一時停止 or 再開 する
       *
       * @param handleIdx  制御を行うハンドルのインデックス
       * @param pauseOn    true:一時停止 false:再開
       * @param fadeFrames フェードイン, フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void Pause( u8 handleIdx, bool pauseOn, s32 fadeFrames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief 一時停止中かどうかを調べる
       *
       * @param handleIdx 制御を行うハンドルのインデックス
       *
       * @retval true   一時停止中
       * @retval false  一時停止中でない
       */
      //-----------------------------------------------------------------------
      bool IsPause( u8 handleIdx ) const; 

      //-----------------------------------------------------------------------
      /**
       * @brief 再生, 停止時における
       *        フェードイン, フェードアウト完了までの残りフレーム数を取得する
       *
       * @param handleIdx 制御を行うハンドルのインデックス
       */
      //-----------------------------------------------------------------------
      s32 GetRemainingFadeFrames( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 一時停止時における
       *        フェードイン, フェードアウト完了までの残りフレーム数を取得する
       *
       * @param handleIdx 制御を行うハンドルのインデックス
       */
      //-----------------------------------------------------------------------
      s32 GetRemainingPauseFadeFrames( u8 handleIdx ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief ハンドルにサウンドが関連付けられているかどうかを調べる
       *
       * @param handleIdx  調べるハンドルのインデックス
       *
       * @retval true    関連付けられている
       * @retval false   関連付けられていない
       */
      //-----------------------------------------------------------------------
      bool IsAttachedSound( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ハンドル関連付けられているサウンドのIDを取得する
       *
       * @param handleIdx  調べるハンドルのインデックス
       *
       * @return 指定したハンドルに関連付けられているサウンドID
       * @retval INVALID_SOUND_ID  サウンドが関連付けられていない場合
       */
      //-----------------------------------------------------------------------
      u32 GetAttachedSound( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief ストリームサウンドの再生位置を取得する
       *
       * @param handleIdx  ストリームサウンド再生中のハンドルのインデックス
       *
       * @return ストリームデータの再生位置( サンプル数 )
       * @retval 負数  ハンドルが無効の場合
       */
      //-----------------------------------------------------------------------
      s32 GetStreamSamplePosition( u8 handleIdx ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief 音量を変更する
       *
       * @param handleIdx 制御を行うハンドルのインデックス
       * @param volume    設定する音量の倍率 ( 0.0 〜 )
       * @param frames    音量の変化にかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void SetVolume( u8 handleIdx, float volume, s32 frames = 0 );

      //-----------------------------------------------------------------------
      /**
       * @brief 音程を変更する
       *
       * @param handleIdx 制御を行うハンドルのインデックス
       * @param pitch     周波数比率
       *
       * @note 周波数比率 
       *   0.5 = 1 オクターブ 低
       *   1.0 = 等倍 ( デフォルト値 )
       *   2.0 = 1 オクターブ 高
       */
      //-----------------------------------------------------------------------
      void SetPitch( u8 handleIdx, float pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief 12平均律に即した音程に変更する
       *
       * @param u8 handleIdx  制御を行うハンドルのインデックス
       * @param s8 level      12平均律のレベル
       *
       * @note levelの指定例
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2オクターブ高い音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1オクターブ高い音程
       * EQUAL_TEMPERAMENT_LEVEL_UNISON             通常の音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1オクターブ低い音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2オクターブ低い音程
       */
      //-----------------------------------------------------------------------
      void SetPitchByEqualTemperament( u8 handleIdx, s8 level ); 
      static const s8 ONE_OCTAVE_STEP_COUNT = 12;
      static const s8 EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH = ONE_OCTAVE_STEP_COUNT;  // 1オクターブ高くなる周波数比
      static const s8 EQUAL_TEMPERAMENT_LEVEL_UNISON = 0;                               // 等倍の周波数比
      static const s8 EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW = -ONE_OCTAVE_STEP_COUNT;  // 1オクターブ低くなる周波数比

      //-----------------------------------------------------------------------
      /**
       * @brief パン ( 左右の定位 ) を変更する
       *
       * @param handleIdx  制御を行うハンドルのインデックス
       * @param pan        パンの相対値
       *
       * @note
       * pan が  0.0 なら, データ通りの方向から聞こえる.
       * pan が -1.0 なら, 正面から聞こえていた音が左から聞こえるようになる.
       * pan が +1.0 なら, 正面から聞こえていた音が右から聞こえるようになる.
       */
      //-----------------------------------------------------------------------
      void SetPan( u8 handleIdx, float pan );

      //-----------------------------------------------------------------------
      /**
       * @brief サラウンドパン ( 前後の定位 ) を変更する
       *
       * @param handleIdx  制御を行うハンドルのインデックス
       * @param pan        パンの相対値
       *
       * @note
       * pan が  0.0 なら, データ通りの方向から聞こえる.
       * pan が +1.0 なら, 最前方から聞こえていた音が中央から聞こえるようになる.
       * pan が +2.0 なら, 最前方から聞こえていた音が最後方から聞こえるようになる.
       */
      //-----------------------------------------------------------------------
      void SetSurroundPan( u8 handleIdx, float pan );

      //-----------------------------------------------------------------------
      /**
       * @brief テンポを変更する
       *
       * @param handleIdx   制御を行うハンドルのインデックス
       * @param tempoRatio  テンポの倍率
       *
       * @attention シーケンスサウンドに対してのみ有効です。
       */
      //-----------------------------------------------------------------------
      void SetTempoRatio( u8 handleIdx, f32 tempoRatio );

	    //-------------------------------------------------------------------------
		  /**
	     * @brief トランスポーズ
	     *
	     * @param handleIdx    制御を行うハンドルのインデックス
	     * @param tempoRatio  テンポの倍率
	     *
	     * @attention シーケンスサウンドに対してのみ有効です。
	     */
			//-------------------------------------------------------------------------
			void SetTranspose( u8 handleIdx, s8 transposeValue, u32 transposeTrackBit );



      //-----------------------------------------------------------------------
      /**
       * @brief 指定したプレイヤーで再生中のサウンドの個数を取得する
       *
       * @param playerID プレイヤーのアイテムID
       */
      //-----------------------------------------------------------------------
      u32 GetPlayingSoundCount( u32 playerID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したプレイヤーの音量を取得する
       *
       * @param playerID プレイヤーのアイテムID
       */
      //-----------------------------------------------------------------------
      f32 GetPlayerVolume( u32 playerID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したプレイヤーの音量を変更する
       *
       * @param playerID プレイヤーのアイテムID
       * @param volume    設定する音量[0.0, 2.0]
       */
      //-----------------------------------------------------------------------
      void SetPlayerVolume( u32 playerID, f32 volume );

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したプレイヤーで再生中の全てのサウンドを停止する
       *
       * @param playerID      プレイヤーのアイテムID
       * @param fadeOutFrame  フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void StopPlayerSound( u32 playerID, u32 fadeOutFrame );

      //-----------------------------------------------------------------------
      /**
       * @brief 指定したプレイヤーで再生中の全てのサウンドを一時停止・再開する
       *
       * @param playerID   プレイヤーのアイテムID
       * @param pauseOn    true:一時停止, false:再開
       * @param fadeFrame  フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void PausePlayerSound( u32 playerID, bool pauseOn, u32 fadeFrame );




      //-----------------------------------------------------------------------
      /**
       * @brief サウンドヒープの状態を保存する
       *
       * @return 保存後の階層レベル ( 保存に失敗した場合は -1 )
       */
      //-----------------------------------------------------------------------
      s32 SaveHeapState( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 保存したサウンドヒープの状態を復元する
       *
       * @param level 復元する階層レベル
       */
      //-----------------------------------------------------------------------
      void LoadHeapState( s32 level );

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドヒープをクリアする
       */
      //-----------------------------------------------------------------------
      void ClearHeap( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 現在のサウンドヒープの階層レベルを取得する
       */
      //-----------------------------------------------------------------------
      s32 GetCurrentHeapLevel( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドヒープのサイズを取得する
       */
      //-----------------------------------------------------------------------
      size_t GetHeapSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドヒープの空き容量を取得する
       */
      //-----------------------------------------------------------------------
      size_t GetHeapFreeSize( void ) const;




      //-----------------------------------------------------------------------
      /**
       * @brief マスターボリュームを 設定 する
       *
       * @param volume    音量の倍率 ( 0.0 〜 )
       * @param fadeTimes 音量変化にかける時間[ミリ秒]
       */
      //-----------------------------------------------------------------------
      void SetMasterVolume( float volume, s32 fadeTimes );

      //-----------------------------------------------------------------------
      /**
       * @brief マスターボリュームを 取得 する
       *
       * @return マスターボリュームの倍率 ( 0.0 〜 )
       */
      //-----------------------------------------------------------------------
      float GetMasterVolume( void ) const;




      // 出力モード
      enum OutputMode {
        OUTPUT_MODE_MONO     = nw::snd::OUTPUT_MODE_MONO,     // モノラルモード ( パン無効 )
        OUTPUT_MODE_STEREO   = nw::snd::OUTPUT_MODE_STEREO,   // ステレオモード
        OUTPUT_MODE_SURROUND = nw::snd::OUTPUT_MODE_SURROUND, // サラウンドモード
      };

      //-----------------------------------------------------------------------
      /**
       * @brief サウンド出力モードを設定する
       *
       * @param mode 設定する出力モード
       */
      //-----------------------------------------------------------------------
      void SetOutputMode( OutputMode mode );

      //-----------------------------------------------------------------------
      /**
       * @brief サウンド出力モードを取得する
       */
      //-----------------------------------------------------------------------
      OutputMode GetOutputMode( void ) const; 

      // シーケンスのミュート状態
      enum SeqMute {
        SEQ_MUTE_OFF     = nw::snd::SEQ_MUTE_OFF,     // シーケンストラックをミュート状態を解除する.
        SEQ_MUTE_NO_STOP = nw::snd::SEQ_MUTE_NO_STOP, // シーケンストラックをミュート状態にする. 発音中の音に変化はなし.
        SEQ_MUTE_RELEASE = nw::snd::SEQ_MUTE_RELEASE, // シーケンストラックをミュート状態にする. 発音中の音を緩やかに停止させる.
        SEQ_MUTE_STOP    = nw::snd::SEQ_MUTE_STOP,    // シーケンストラックをミュート状態にする. 発音中の音を直ちに停止する.
      };

      //---------------------------------------------------------------------------
      /**
       * @brief SEのトラック変数を変更する
       * @param handle  再生に使用するSEハンドルのインデックス
       * @param trackNo  トラックNo
       * @param varNo    変数番号(0-15)
       * @param value    書き込む変数値
       */
      //---------------------------------------------------------------------------
      void SetTrackVariable( u8 handleIdx, int trackNo, int varNo, s16 value );

      //---------------------------------------------------------------------------
      /**
       * @brief SEのトラック変数を取得
       * @param handle  再生に使用するSEハンドルのインデックス
       * @param trackNo  トラックNo
       * @param varNo    変数番号(0-15)
       */
      //---------------------------------------------------------------------------
      s16 GetTrackVariable( u8 handleIdx, int trackNo, int varNo);

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとのミュート状態を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param mute         ミュート状態
       */
      //-----------------------------------------------------------------------
      void SetTrackMute( u8 handleIdx, u32 trackBitFlag, SeqMute mute );

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとのサイレント状態を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param silenceFlag  サイレント状態にする場合 true, 解除する場合 false
       * @param fadeTimes    フェード時間 (ミリ秒)
       */
      //-----------------------------------------------------------------------
      void SetTrackSilence( u8 handleIdx, u32 trackBitFlag, bool silenceFlag, s32 fadeTimes ); 

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとの音量を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param volume       音量の倍率 ( デフォルトは 1.0 )
       * @param frame        音量変化にかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void SetTrackVolume( u8 handleIdx, u32 trackBitFlag, f32 volume, s32 frame );

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとの音程を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param pitch        周波数比率
       *
       * @note 周波数比率 
       *   0.5 = 1 オクターブ 低
       *   1.0 = 等倍 ( デフォルト値 )
       *   2.0 = 1 オクターブ 高
       */
      //-----------------------------------------------------------------------
      void SetTrackPitch( u8 handleIdx, u32 trackBitFlag, f32 pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとの音程を12平均律に即した音程に変更する
       *
       * @param handleIdx     制御を行うハンドルのインデックス
       * @param trackBitFlag  設定するトラックのビットフラグ
       * @param level         12平均律のレベル
       *
       * @note levelの指定例
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2オクターブ高い音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1オクターブ高い音程
       * EQUAL_TEMPERAMENT_LEVEL_UNISON             通常の音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1オクターブ低い音程
       * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2オクターブ低い音程
       */
      //-----------------------------------------------------------------------
      void SetTrackPitchByEqualTemperament( u8 handleIdx, u32 trackBitFlag, s8 level ); 

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとのパン ( 左右の定位 ) を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param pan          パンの値
       *
       * @note パンの値
       *   -1.0 = 左
       *    0.0 = 中
       *    1.0 = 右
       */
      //-----------------------------------------------------------------------
      void SetTrackPan( u8 handleIdx, u32 trackBitFlag, f32 pan );

      //-----------------------------------------------------------------------
      /**
       * @brief トラックのサラウンドパン ( 前後の定位 ) を変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param pan          パンの値
       u*/
      //-----------------------------------------------------------------------
      void SetTrackSurroundPan( u8 handleIdx, u32 trackBitFlag, f32 pan ); 

      //-----------------------------------------------------------------------
      /**
       * @brief トラックごとのバンクを変更する
       *
       * @param handleIdx    制御を行うハンドルのインデックス
       * @param trackBitFlag 設定するトラックのビットフラグ
       * @param bankIdx      設定するバンクのインデックス ( 0 〜 3 )
       */
      //-----------------------------------------------------------------------
      void SetTrackBankIndex( u8 handleIdx, u32 trackBitFlag, s32 bankIdx );




      // 3D サウンドパラメータ
      struct Sound3DParam 
      {
        s32 maxPriorityReduction; // 最大プライオリティ減少量
        f32 panRange;             // パンの変化幅 ( 0.0 〜 1.0 )
        f32 sonicVelocity;        // 音速 ( ドップラー効果の計算に使用 ) [*/frame]
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dサウンドのパラメータを設定する
       *
       * @param param 設定する3Dサウンドパラメータ
       */
      //-----------------------------------------------------------------------
      void Set3DParam( const Sound3DParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief 現在設定されている3Dサウンドのパラメータを取得する
       *
       * @param[out] pOutParam パラメータの格納先
       */
      //-----------------------------------------------------------------------
      void Get3DParam( Sound3DParam* pOutParam ) const;




      // 3D サウンド リスナーパラメータ
      struct ListenerParam 
      {
        f32 interiorSize;      // インテリアサイズ
        f32 maxVolumeDistance; // 音量が最大になる範囲
        f32 unitDistance;      // 音量が減衰する単位距離
      };

      //-----------------------------------------------------------------------
      /**
       * @brief 現在設定されているリスナーのパラメータを設定する
       *
       * @param param 設定するリスナーパラメータ
       */
      //-----------------------------------------------------------------------
      void SetListenerParam( const ListenerParam& param );

      //-----------------------------------------------------------------------
      /**
       * @brief リスナーのパラメータを取得する
       * 
       * @param[out] pOutParam パラメータの格納先
       */
      //-----------------------------------------------------------------------
      void GetListenerParam( ListenerParam* pOutParam ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief リスナー行列を設定する
       *
       * @param matrix 設定するリスナー行列
       */
      //-----------------------------------------------------------------------
      void SetListenerMatrix( const gfl2::math::MTX34& matrix );

      //-----------------------------------------------------------------------
      /**
       * @brief リスナーの座標を変更する
       *
       * @param pos リスナーの座標
       * @param up  リスナーの上方ベクトル
       * @param dir リスナーの向きベクトル
       */
      //-----------------------------------------------------------------------
      void SetListenerPos( 
          const gfl2::math::VEC3& pos,
          const gfl2::math::VEC3& up,
          const gfl2::math::VEC3& dir );

      //-----------------------------------------------------------------------
      /**
       * @brief リスナーの速度を設定する( ドップラー効果に反映されます )
       *
       * @param velocity リスナーの移動速度
       *
       * @note
       * SetListenerMatrix(), SetListenerPos() 内で自動的に計算されるため, 
       * 通常はアプリケーション側で設定する必要はありません.
       *
       * @note
       * リスナー座標が一瞬で遠くに飛ぶような場合に限り,
       * 速度が正しく計算できないため
       * アプリケーション側で上書きする必要があります.
       */
      //-----------------------------------------------------------------------
      void SetListenerVelocity( const gfl2::math::VEC3& velocity );


      //-----------------------------------------------------------------------
      /**
       * @brief 3Dサウンドを再生する
       *
       * @param actorIdx    再生後の制御を行う3Dアクターのインデックス
       * @param itemID      再生するサウンドのアイテムID
       * @param startInfo   詳細な再生パラメータ
       *
       * @return 再生が成功した場合 true
       */
      //-----------------------------------------------------------------------
      bool Play3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 3Dアクターの再生準備開始
       *
       * @param actorIdx    再生後の制御を行う3Dアクターのインデックス
       * @param itemID      再生するサウンドのアイテムID
       * @param startInfo   詳細な再生パラメータ(NULLまたは省略可)
       *
       * @return すでに再生準備ができている場合 true
       *
       * @note
       * この関数でTrueが返った、もしくは、
       * 呼び出し後にIsPrepared3DActor()でTrueが返るまで待ってからPlayPrepared3DActor()で再生を開始すること
       *
       */
      //-------------------------------------------------------------------------
      bool Prepare3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo = NULL );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 3Dアクターの再生準備ができているか？
       *
       * @param actorIdx    3Dアクターのインデックス
       *
       * @return すでに再生準備ができている場合 true
       */
      //-------------------------------------------------------------------------
      bool IsPrepared3DActor( u8 actorIdx );
    
      //-------------------------------------------------------------------------
      /**
       * @brief 再生準備ができている3Dアクターの再生を開始する
       *
       * @param actorIdx    3Dアクターのインデックス
       *
       * @note
       * Prepar3DActor()呼び出しでTrueが返った、もしくは
       * Prepar3DActor()呼び出し後、IsPrepaed3DActor()で待ち、Trueが返るまで待ってから呼び出すこと
       */
      //-------------------------------------------------------------------------
      void PlayPrepared3DActor( u8 actorIdx );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターで再生中の全てのサウンドを停止する
       *
       * @param actorIdx   制御を行う3Dアクターのインデックス
       * @param fadeFrames フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void Stop3DActor( u8 actorIdx, s32 fadeFrames );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターで再生中の全てのサウンドを一時停止・再開する
       *
       * @param actorIdx   制御を行う3Dアクターのインデックス
       * @param pauseOn    true :一時停止, false:再開
       * @param fadeFrames フェードイン・フェードアウトにかけるフレーム数
       */
      //-----------------------------------------------------------------------
      void Pause3DActor( u8 actorIdx, bool pauseOn, s32 fadeFrames );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの音量を変更する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       * @param volume   設定する音量の倍率( 0.0 〜 1.0 )
       */
      //-----------------------------------------------------------------------
      void Set3DActorVolume( u8 actorIdx, f32 volume );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの音量を取得する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       *
       * @return 指定した3Dアクターに設定されている音量の倍率
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorVolume( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの音程を変更する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       * @param pitch    周波数比率
       *
       * @note 周波数比率 
       *   0.5 = 1 オクターブ 低
       *   1.0 = 等倍( デフォルト値 )
       *   2.0 = 1 オクターブ 高
       */
      //-----------------------------------------------------------------------
      void Set3DActorPitch( u8 actorIdx, f32 pitch );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの音程を取得する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       *
       * @return 指定した3Dアクターに設定されている周波数比率
       *
       * @note周波数比率 
       *   0.5 = 1 オクターブ 低
       *   1.0 = 等倍 ( デフォルト値 )
       *   2.0 = 1 オクターブ 高
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorPitch( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターのパン ( 左右の定位 ) を変更する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       * @param pan      パンの値
       *
       * @note パンの値
       *   -1.0 = 左
       *    0.0 = 中
       *    1.0 = 右
       */
      //-----------------------------------------------------------------------
      void Set3DActorPan( u8 actorIdx, f32 pan );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターのパン( 左右の定位 )を取得する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       *
       * @return 指定した3Dアクターに設定されているパンの値
       *
       * @note パンの値
       *   -1.0 = 左
       *    0.0 = 中
       *    1.0 = 右
       */
      //-----------------------------------------------------------------------
      f32 Get3DActorPan( u8 actorIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの座標を変更する
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       * @param pos      設定する座標
       */
      //-----------------------------------------------------------------------
      void Set3DActorPos( u8 actorIdx, const gfl2::math::VEC3& pos );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの座標を取得する
       *
       * @param[in]  actorIdx 制御を行う3Dアクターのインデックス
       * @param[out] pos      取得した座標の格納先
       */
      //-----------------------------------------------------------------------
      void Get3DActorPos( u8 actorIdx, gfl2::math::VEC3* pos ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターの速度を設定する( ドップラー効果に反映されます )
       *
       * @param actorIdx 制御を行う3Dアクターのインデックス
       * @param pos      設定する座標
       *
       * @note
       * Set3DActorPos() で座標を設定した際に
       * 登録時の差分から自動的に計算されるため,
       * 通常はアプリケーション側で設定する必要はありません.
       *
       * @note
       * アクター座標が一瞬で遠くに飛ぶような場合に限り, 
       * 速度が正しく計算できないため
       * アプリケーション側で上書きする必要があります.
       */
      //-----------------------------------------------------------------------
      void Set3DActorVelocity( u8 actorIdx, const gfl2::math::VEC3& velocity );

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターで再生中のサウンド数を取得する
       * @param actorIdx        制御を行う3Dアクターのインデックス
       * @param actorPlayerIdx  対象のアクタープレイヤーのインデックス
       */
      //-----------------------------------------------------------------------
      u32 Get3DActorPlayingSoundCount( u8 actorIdx, u8 actorPlayerIdx ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 3Dアクターが全アクタープレイヤーで再生中のサウンド数を取得する
       * @param actorIdx  制御を行う3Dアクターのインデックス
       */
      //-----------------------------------------------------------------------
      u32 Get3DActorPlayingSoundCount( u8 actorIdx ) const;



      //-----------------------------------------------------------------------
      /**
       * @brief サウンドに設定されているユーザパラメータを取得
       *
       * @param itemID  取得するアイテムID
       */
      //-----------------------------------------------------------------------
      u32 GetSoundUserParam( u32 itemID );

      //-----------------------------------------------------------------------
      /*
       * @brief シーケンス変数を取得
       *
       * @param[in]  handleIdx 値を取得するサウンドハンドルインデックス
       * @param[in]  varNo     変数ナンバー(0~15)
       * @param[out] var       取得した値を格納するワークへのポインタ
       * 
       * @retval  true:取得成功　false:取得失敗
       */
      //-----------------------------------------------------------------------
      bool ReadVariable( u8 handleIdx, int varNo, s16* var );

      //-----------------------------------------------------------------------
      /*
       * @brief シーケンス変数に格納
       *
       * @param[in] handleIdx 値を格納するサウンドハンドルインデックス
       * @param[in] varNo     変数ナンバー(0~15)
       * @param[in] var       格納する値
       *
       * @retval  true:格納成功　false:格納失敗
       */
      //-----------------------------------------------------------------------
      bool WriteVariable( u8 handleIdx, int varNo, s16 var );

      //-----------------------------------------------------------------------
      /*
       * @brief シーケンスグローバル変数を取得
       *
       * @param[in]  handleIdx 値を取得するサウンドハンドルインデックス
       * @param[in]  varNo     変数ナンバー(0~15)
       * @param[out] var       取得した値を格納するワークへのポインタ
       * 
       * @retval  true:取得成功　false:取得失敗
       */
      //-----------------------------------------------------------------------
      bool ReadGlobalVariable( u8 handleIdx, int varNo, s16* var );

      //-----------------------------------------------------------------------
      /*
       * @brief シーケンスグローバル変数に格納
       *
       * @param[in] handleIdx 値を格納するサウンドハンドルインデックス
       * @param[in] varNo     変数ナンバー(0~15)
       * @param[in] var       格納する値
       *
       * @retval  true:格納成功　false:格納失敗
       */
      //-----------------------------------------------------------------------
      bool WriteGlobalVariable( u8 handleIdx, int varNo, s16 var );

      //-----------------------------------------------------------------------
      /*
       * @brief サウンドタイプ取得
       *
       * @param[in] handleIdx 取得するサウンドハンドルインデックス
       *
       * @retval  nw::snd::SoundArchive::SoundType
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive::SoundType GetSoundType( u8 handleIdx ) const;

      //-----------------------------------------------------------------------
      /*
       * @brief サウンドタイプ取得
       *
       * @param[in] itemID  調べるサウンドのID
       *
       * @retval  nw::snd::SoundArchive::SoundType
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive::SoundType GetSoundTypeOfItem( u32 itemID ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief エフェクトをAUXバスに追加します。
       *
       * @param busID バスID（A,Bあります）
       * @param effect  エフェクトの抽象クラスを継承したもの
       *
       * @return true追加に成功 false追加に失敗
       */
      //-----------------------------------------------------------------------
      bool AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect );
      
      //-----------------------------------------------------------------------
      /**
       * @brief エフェクトをAUXバスから削除します。
       *
       * @param busID バスID（A,Bあります）
       * @param フェード時間（ミリ秒）
       */
      //-----------------------------------------------------------------------
      void RemoveEffect( nw::snd::AuxBus busID, int fadeTimes );

      //-----------------------------------------------------------------------
      /**
       * @brief リバーブエフェクトをAUXバスに追加します。
       *
       * @param busID バスID（A,Bあります）
       * @param reverb_buffer_heap  リバーブ用のバッファ取得用ヒープ
       * @param param パラム
       *
       * @return true追加に成功 false追加に失敗
       */
      //-----------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
      bool AppendReverbEffectCTR( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param );
#elif defined(GF_PLATFORM_WIN32)
      bool AppendReverbEffectWin32( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverbStd::ReverbStdParam & param );
#endif
      //-----------------------------------------------------------------------
      /**
       * @brief エフェクトをAUXバスから削除します。
       *        RemoveEffectとの違いは、存在していればリバーブ用バッファを解放することです
       *
       * @param busID バスID（A,Bあります）
       * @param フェード時間（ミリ秒）
       */
      //-----------------------------------------------------------------------
      void RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes );

      protected:
      gfl2::heap::HeapBase* m_pAppHeap;
      gfl2::heap::HeapBase* m_pDevHeap;

      nw::snd::SoundArchive* m_pSoundArchive;
#if defined(GF_PLATFORM_CTR)
      nw::snd::RomSoundArchive m_romSoundArchive; 
#elif defined(GF_PLATFORM_WIN32)
      nw::snd::FsSoundArchive  m_fsSoundArchive; 
#endif
      nw::snd::MemorySoundArchive   m_memSoundArchive;
      nw::snd::SoundDataManager     m_soundDataManager;
      nw::snd::SoundArchivePlayer   m_soundArchivePlayer;
      gfl2::sound::SoundHeap*       m_soundHeap;

      gfl2::sound::SoundDataManagerThread*  m_pSoundDataManagerThread;
      bool                                  m_SoundDataManagerThreadResult;

#if defined(GF_PLATFORM_CTR)
      nn::os::CriticalSection               m_criticalSectionForSoundDataManagerLoad;
#endif
      nw::snd::SoundHandle* m_pSoundHandles;
      u8 m_soundHandleNum;

      nw::snd::Sound3DManager m_sound3DManager;
      nw::snd::Sound3DListener m_sound3DListener;
      nw::snd::Sound3DActor* m_pSound3DActors;
      nw::snd::SoundHandle* m_pSound3DHandles;
      u8 m_sound3DActorNum;
      
#if defined(GF_PLATFORM_CTR)
      nw::snd::FxReverb m_fxReverbCTR; 
#elif defined(GF_PLATFORM_WIN32)
      nw::snd::FxReverbStd m_fxReverbWin32; 
#endif
     
      void* m_pMemoryForSoundReverb;

      void* m_pMemoryForDspComponent;
      void* m_pMemoryForAacLibrary;
      void* m_pMemoryForSoundSystem;
      void* m_pMemoryForSoundArchiveHeader;
      void* m_pMemoryForSoundDataManager;
      void* m_pMemoryForSoundArchivePlayer;
      void* m_pMemoryForStreamBuffer;
      void* m_pMemoryForCacheBuffer;
      void* m_pMemoryForSoundHeap; 
      void* m_pMemoryForSound3DManager;

      bool m_isAacLibraryInitialized;  // AAC ライブラリを初期化したか？

      // 12平均律での周波数比配列
      static const u8 EQUAL_TEMPERAMENT_LEVEL_NUM = ONE_OCTAVE_STEP_COUNT + 1;
      static const f32 m_highFrequencyRatioForEqualTemperament[ EQUAL_TEMPERAMENT_LEVEL_NUM ];  
      static const f32 m_lowFrequencyRatioForEqualTemperament[ EQUAL_TEMPERAMENT_LEVEL_NUM ];  

      protected:
      // スレッド読み込みを行うので、外部公開しない。
      nw::snd::SoundDataManager* GetSoundDataManager( void );

      private:

#if defined(GF_PLATFORM_CTR)
      void InitDspComponent( const InitParam& );
      void InitSDKLibrary( void );
#endif //defined(GF_PLATFORM_CTR)

      void InitSoundSystem( const InitParam& );
      void InitSoundArchive( const InitParam& );
      void InitRomSoundArchive( const InitParam& );
      void InitMemorySoundArchive( const InitParam& );
      void InitSoundDataManager( void );
      void InitSoundArchivePlayer( void );
      void InitSoundHeap( const InitParam& );
      void CreateSoundHandles( const InitParam& );
      void InitSound3DManager( void );
      void InitSound3DListener( void );
      void CreateSound3DActorsAndHandles( const InitParam& );
      void InitSound3DActors( void ); 
      void DeleteSoundHandles( void );
      void DeleteSound3DActorsAndHandles( void ); 
      nw::snd::SoundArchive::SoundType GetAttachedSoundType( const nw::snd::SoundHandle* ) const; 
      f32 CalcPitchByEqualTemperament( s8 ) const;

      public:
      //-----------------------------------------------------------------------
      /**
       * @brief 各種メンバを取得する
       */
      //-----------------------------------------------------------------------
      nw::snd::SoundArchive* GetSoundArchive( void );
#if defined(GF_PLATFORM_CTR)
      nw::snd::RomSoundArchive* GetRomSoundArchive( void );
#endif //defined(GF_PLATFORM_CTR)
      nw::snd::SoundArchivePlayer* GetSoundArchivePlayer( void );
      nw::snd::SoundHandle* GetSoundHandle( u8 handleIdx );
      nw::snd::SoundHeap* GetSoundHeap( void );

      //-----------------------------------------------------------------------
      /**
       * @brief サウンドヒープの内容を出力する
       */
      //-----------------------------------------------------------------------
      void DumpSoundHeap( void );
    };


  } //namespace sound
} //namespace gfl2

#endif //__GFL_SOUNDSYSTEM_H__

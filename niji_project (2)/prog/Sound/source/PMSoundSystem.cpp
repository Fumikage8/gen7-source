//============================================================================
/**
 * @file PMSoundSystem.cpp
 * @brief ポケモンサウンド制御クラス
 * @author obata_toshihiro
 * @date 2011.06.28
 */
//============================================================================

#include <nw/snd.h>
#include "macro/include/gfl2_Macros.h"

#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/SELevelManager.h"
#include "Sound/include/BGMLevelManager.h"
#include "Sound/include/SoundMiddleIDConverter.h"
#include "Sound/include/EnvSEManager.h"
#include "Sound/include/Sound.h"

#if defined(GF_PLATFORM_WIN32)
  #include <windows.h>
#endif

#if PM_DEBUG
#include <system/include/Timer/gfl2_PerformanceCounter.h> //
#endif

// Singletonインスタンスの実体を定義
SINGLETON_INSTANCE_DEFINITION(Sound::PMSoundSystem)
template class gfl2::base::SingletonAccessor<Sound::PMSoundSystem>;


// 特定の人の環境ではサウンドアーカイブをHIOで読み込む
#if defined( DEBUG_ONLY_FOR_hio_sound )
#define HIO_SOUNDARC 1  // PC からロードする
#else
#define HIO_SOUNDARC 0  // ROM からロードする
#endif


namespace Sound {

  enum{
#if defined(GF_PLATFORM_CTR)
    SOUND_THREAD_PRIORITY = nw::snd::SoundSystem::SoundSystemParam::DEFAULT_SOUND_THREAD_PRIORITY,
    SOUND_LOAD_THREAD_PRIORITY = nw::snd::SoundSystem::SoundSystemParam::DEFAULT_TASK_THREAD_PRIORITY
#elif defined(GF_PLATFORM_WIN32)
    SOUND_THREAD_PRIORITY = THREAD_PRIORITY_HIGHEST,
    SOUND_LOAD_THREAD_PRIORITY = THREAD_PRIORITY_HIGHEST
#endif
  };

#if PM_DEBUG
  // ROMアクセス頻度検出用デバッグ機能
  int PMSoundSystem::m_bRomAccessCheckMode = 1;
  b32 PMSoundSystem::m_bRomAccessCheckStart = false;
  s64 PMSoundSystem::m_RomAccessStartTime = 0;
  s64 PMSoundSystem::m_RomBeforeAccessTime = 0;
#endif // PM_DEBUG

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap         インスタンスの生成に使用するヒープ
   * @param device_heap  インスタンスの生成に使用するデバイスヒープ
   */
  //---------------------------------------------------------------------------
  PMSoundSystem::PMSoundSystem(
      gfl2::heap::HeapBase* heap,
      gfl2::heap::HeapBase* device_heap ) :
    m_BGMControlEnable( true ),
    m_soundSystem( NULL ),
    m_SELevelManager( NULL ),
#if defined(GF_PLATFORM_CTR)
    m_EnvSEManager( NULL ),
    m_CSectionForSoundControl()
#elif defined(GF_PLATFORM_WIN32)
    m_EnvSEManager( NULL )
#endif
  { 
    this->CreateSoundSystem( heap, device_heap );
    this->CreateSoundHeap( heap, device_heap );
    this->CreateSoundLevelManager( heap );
    m_EnvSEManager = GFL_NEW( heap ) EnvSEManager();

#if defined(GF_PLATFORM_CTR)
    m_CSectionForSoundControl.Initialize();
#endif
    }

  /**
   * @brief サウンドシステムを生成する
   * @param heap         システムに使用するヒープ
   * @param device_heap  システムに使用するデバイスヒープ
   */
  void PMSoundSystem::CreateSoundSystem(
      gfl2::heap::HeapBase* heap, 
      gfl2::heap::HeapBase* device_heap )
  {
    gfl2::sound::SoundSystem::InitParam system_param;
    system_param.pAppHeap        = heap;
    system_param.pDevHeap        = device_heap;
    system_param.soundHeapSize   = 0;  // システム管理のサウンドヒープは使用しない
    system_param.soundHandleNum  = SHANDLE_NUM;
    system_param.sound3DActorNum = SOUND_3D_ACTOR_TOTAL_NUM;
    system_param.aacUse = false;  // AAC利用 
    //system_param.aacDspComponentPath = "rom:/sound/dspaudio.cdc";  // AAC関連
    system_param.soundThreadPriority = SOUND_THREAD_PRIORITY;  // サウンドスレッドプライオリティ設定
    system_param.soundDataLoadThreadPriority = SOUND_LOAD_THREAD_PRIORITY;  // サウンドロードスレッドプライオリティ設定

    system_param.soundArchiveType = gfl2::sound::SoundSystem::SOUND_ARCHIVE_TYPE_ROM;

#if defined(GF_PLATFORM_CTR)
    system_param.bcsarFilePath = "rom:/data/sound/niji_sound.bcsar";
#elif defined(GF_PLATFORM_WIN32)
	  static const int cFileNameLen = 512;
    char buf[cFileNameLen];
	  if ( GetEnvironmentVariableA("NIJI_PROJ_ROMFS_WIN32_DIR", buf, cFileNameLen) > 0 )
    {
		  nw::ut::snprintf(buf, cFileNameLen, "%sdata/sound/niji_sound.bfsar", buf );
			NW_LOG("buf(%s)\n", buf);
		}
    system_param.bfsarFilePath = buf;
#endif

    m_soundSystem = GFL_NEW( heap ) gfl2::sound::SoundSystem( system_param );

    if( m_soundSystem == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("SoundSystem Initialize Failed\n"); //システム初期化失敗
    }
    m_soundSystem->SetOutputMode(gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND);
    m_soundSystem->SetMasterVolume(BGM_VOLUME_NORMAL, 0);
  }

  /**
   * @brief サウンドシステムを生成する
   * @param heap         管理領域に使用するヒープ
   * @param device_heap  サウンドヒープとして使用するデバイスヒープ
   */
  void PMSoundSystem::CreateSoundHeap(
      gfl2::heap::HeapBase* heap,
      gfl2::heap::HeapBase* device_heap )
  {
    m_soundHeap[0] = GFL_NEW( heap ) gfl2::sound::SoundHeap( heap, device_heap, BGM_HEAPSIZE );  
    if( m_soundHeap[0] == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("BGMHeap Initialize Failed\n"); //システム初期化失敗
    }
    m_soundHeap[1] = GFL_NEW( heap ) gfl2::sound::SoundHeap( heap, device_heap, SE_HEAPSIZE );
    if( m_soundHeap[1] == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("SEHeap Initialize Failed\n"); //システム初期化失敗
    }
  }

  /**
   * @brief 階層管理システムを作成する
   * @param heap  インスタンスの生成に使用するヒープ
   */
  void PMSoundSystem::CreateSoundLevelManager( gfl2::heap::HeapBase* heap )
  {
    m_SELevelManager = GFL_NEW( heap )
      SELevelManager( heap, m_soundSystem, m_soundHeap[1] );

    if( m_SELevelManager == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("SELevelManager Initialize Failed\n"); //システム初期化失敗
    }

    BGMLevelManager::Settings setting;
    setting.soundHandle[0] = SHANDLE_BGM_LV1;
    setting.soundHandle[1] = SHANDLE_BGM_LV2;
    setting.soundHandle[2] = SHANDLE_BGM_LV3;
    setting.soundHandle[3] = SHANDLE_BGM_LV4;
    m_BGMLevelManager = GFL_NEW( heap )
      BGMLevelManager( heap, m_soundSystem, m_soundHeap[0], setting );

    if( m_BGMLevelManager == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("BGMLevelManager Initialize Failed\n"); //システム初期化失敗
    }
  } 

  //--------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------
  PMSoundSystem::~PMSoundSystem()
  {
    GFL_DELETE m_EnvSEManager;
    GFL_DELETE m_SELevelManager;
    GFL_DELETE m_BGMLevelManager;
    GFL_DELETE m_soundHeap[0];
    GFL_DELETE m_soundHeap[1];
    GFL_DELETE m_soundSystem;
  } 

  //--------------------------------------------------------------------------
  /**
   * @brief 毎フレームの更新処理
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::Update( void )
  {
    m_soundSystem->Update();
  } 





  // サウンドヒープ

  //--------------------------------------------------------------------------
  /**
   * @brief 外部管理のサウンドヒープにサウンドデータを読み込む
   * @param sound_heap サウンドデータを配置するサウンドヒープ
   * @param middleID   読む込むサウンドデータの中間ID
   * @return 読み込みに成功したか？
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::LoadSoundDataToExternalHeap( 
      gfl2::sound::SoundHeap* sound_heap,
      u32 middleID )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    b32 result = m_soundSystem->LoadSoundData( sound_heap, sound_itemID, gfl2::sound::SoundSystem::LOAD_ALL );
#if PM_DEBUG
    if( !result ) {
      GFL_PRINT( "WARNING!! サウンドデータ( 0x%x )の読み込みに失敗しました。\n", middleID );
    }
#endif
    return result;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  SEのサウンドデータをシステム管理のサウンドヒープに積む
   *
   *	@param	thread_heap   スレッドヒープ
   *	@param	soundID       サウンドID
   *
   *	@retval true  読み込み開始
   *	@retval false 読み込み開始失敗　（前の読み込み中）
   */
  //-----------------------------------------------------------------------------
  b32 PMSoundSystem::LoadAndPushSESoundDataAsync( gfl2::heap::HeapBase * thread_heap, u32 middleID )  
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    return m_SELevelManager->LoadSEDataAsync( thread_heap, sound_itemID );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  SEのサウンドデータの非同期読み込み完了待ち
   *
   *	@retval true    完了
   *	@retval false   途中
   */
  //-----------------------------------------------------------------------------
  b32 PMSoundSystem::WaitLoadAndPushSESoundDataAsync(void)
  {
    SELevelManager::Result result = m_SELevelManager->WaitLoadSeDataAsync();
    if( result != SELevelManager::LOADING ){
      if( result == SELevelManager::LOAD_FINISH ){
        m_SELevelManager->PushSEData(); 
      }
      return true;
    }
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  サウンドデータマネージャへサウンドデータを設定する。
   *
   *	@param    fileID    セットするデータのファイルIDです。
   *	@param    address   アドレス
   */
  //-----------------------------------------------------------------------------
  void PMSoundSystem::SetSoundFileAddress( u32 fileID, const void * address )
  {
    m_soundSystem->SetSoundFileAddress( fileID, address );
  }




  // プレイヤ

  //--------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中のサウンドの個数を取得する
   * @param middleID プレイヤを表す中間ID
   */
  //--------------------------------------------------------------------------
  u32 PMSoundSystem::GetPlayerPlayingCount( u32 middleID ) const
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    return m_soundSystem->GetPlayingSoundCount( sound_itemID );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーの音量を取得する
   * @param middleID プレイヤを表す中間ID
   */
  //--------------------------------------------------------------------------
  f32 PMSoundSystem::GetPlayerVolume( u32 middleID ) const
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    return m_soundSystem->GetPlayerVolume( sound_itemID );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief プレイヤボリュームを変更する
   * @param middleID プレイヤを表す中間ID
   * @param volume   音量の倍率
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::SetPlayerVolume( u32 middleID, f32 volume )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    m_soundSystem->SetPlayerVolume( sound_itemID, volume );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中の全てのサウンドを停止する
   * @param middleID       プレイヤを表す中間ID
   * @param fade_out_frame フェードアウトにかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopPlayerSound( u32 middleID, u32 fade_out_frame )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    m_soundSystem->StopPlayerSound( sound_itemID, fade_out_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 指定したプレイヤーで再生中の全てのサウンドを一時停止・再開する
   * @param middleID   プレイヤを表す中間ID
   * @param pause_on   true なら一時停止, false なら再開
   * @param fade_frame フェードアウトにかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PausePlayerSound( u32 middleID, b32 pause_on, u32 fade_frame )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    m_soundSystem->PausePlayerSound( sound_itemID, GFL_BOOL_CAST(pause_on), fade_frame );
  }




  //--------------------------------------------------------------------------
  /**
   * @brief サウンドモード変更
   */
  //--------------------------------------------------------------------------
  gfl2::sound::SoundSystem::OutputMode PMSoundSystem::GetOutputMode( void )
  { return m_soundSystem->GetOutputMode(); }

  void PMSoundSystem::SetOutputModeMono( void )
	{ m_soundSystem->SetOutputMode(gfl2::sound::SoundSystem::OUTPUT_MODE_MONO); }

  void PMSoundSystem::SetOutputModeStereo( void )
	{ m_soundSystem->SetOutputMode(gfl2::sound::SoundSystem::OUTPUT_MODE_STEREO); }

  void PMSoundSystem::SetOutputModeSurround( void )
	{ m_soundSystem->SetOutputMode(gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND); }


  //--------------------------------------------------------------------------
  /**
   * @brief マスターボリューム変更
   * @param middleID   マスターボリュームスイッチフラグ on = true. off = false
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::SoundMasterVolumeSW(b32 OnOff)
	{
		if(OnOff != false){
			m_soundSystem->SetMasterVolume(BGM_VOLUME_NORMAL, 0);
		} else {
			m_soundSystem->SetMasterVolume(0, 0);
		}
	}


  // BGM

  //--------------------------------------------------------------------------
  /**
   * @brief BGMを再生する
   * @param middleID  再生するBGMの中間ID
   * @param fadein_frame  フェードインフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlayBGM( u32 middleID, u32 fadein_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    this->PrepareBGM( middleID );

    this->PlayPreparedBGM( fadein_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの再生準備をする
   * @param middleID  再生するBGMの中間ID
   * @param startMilliSec  再生開始位置(ミリ秒)
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PrepareBGM( u32 middleID , u32 startMilliSec )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif
    this->StopBGM(0);
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_BGMLevelManager->LoadBGMData( sound_itemID );
    if( startMilliSec != 0 )
    {
      nw::snd::SoundStartable::StartInfo startInfo;
      startInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_START_OFFSET;
      startInfo.startOffset = startMilliSec;
      startInfo.startOffsetType = nw::snd::SoundStartable::StartInfo::START_OFFSET_TYPE_MILLISEC;
      m_soundSystem->PrepareSound( sound_handle_index, sound_itemID , &startInfo );
    }
    else
    {
      m_soundSystem->PrepareSound( sound_handle_index, sound_itemID );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの再生準備が完了しているかどうかを調べる
   * @retval true   完了
   * @retval false  未完了
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsBGMPrepared( void ) const
  {
    if( !m_BGMControlEnable ) {
      return true;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    return m_soundSystem->IsPrepared( sound_handle_index );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief PrepareBGM()で準備したBGMを再生する
   * @param fadein_frame  フェードインフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlayPreparedBGM( u32 fadein_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->PlayPrepared( sound_handle, fadein_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMを停止する
   * @param fadeout_frame  フェードアウトフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopBGM( u32 fadeout_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif
    u32 sound_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    u32 sound_itemID = m_BGMLevelManager->GetSoundItemIDOnCurrentLevel();
    m_soundSystem->Stop( sound_handle, fadeout_frame );
    m_BGMLevelManager->UnloadBGMData( sound_itemID );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMが再生中かどうかを調べる
   * @retval true   再生している
   * @retval false  再生していない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsBGMPlaying( void ) const
  {
    u32 count = m_soundSystem->GetPlayingSoundCount( MiddleIDConverter::CalcSoundItemID( PLAYER_BGM ) );
    count += m_soundSystem->GetPlayingSoundCount( MiddleIDConverter::CalcSoundItemID( PLAYER_STRM ) );
    return ( 0 < count );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 任意のBGMが再生中かどうかを調べる
   * @param middleID チェックするBGMの中間ラベル
   * @retval true   再生している
   * @retval false  再生していない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsBGMPlaying( u32 middleID ) const
  {
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    u32 playing_itemID = m_soundSystem->GetAttachedSound( sound_handle_index );
    u32 check_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    return ( playing_itemID == check_itemID );  // サウンドアイテムIDを比べる
  }
  
  //--------------------------------------------------------------------------
  /**
   * @brief  カレントBGMが有効か(Pause状態は有効とみなされる)どうかを調べる
   * @retval true   有効
   * @retval false  無効
   *
   * IsBGMPlaying()との差は、Pause状態でtrueを返すかどうか？
   * IsBGMPlaying()はPause状態の場合falseを返すが
   * IsBGMActive()ではPause状態でもtrueを返す、つまり完全停止状態でなければtrueを返す
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsBGMValid() const
  {
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    return m_soundSystem->IsAttachedSound( sound_handle_index );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 再生中のBGMを退避する
   * @retval true   退避に成功
   * @retval false  退避に失敗
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::PushBGM( void )
  {
    if( !m_BGMControlEnable ) {
      return true;
    }
    if( this->IsBGMValid() == false ){
      GFL_ASSERT_MSG( 0, "退避する有効なBGMがない。BGM完全停止状態でPush処理は呼べません\n");	//@check iwasawa ケア済み
      return false;
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif
    nw::snd::SoundStartable::StartInfo start_info;
    start_info.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_START_OFFSET;
    start_info.startOffsetType = nw::snd::SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE;
    start_info.startOffset = -1;
    start_info.playerId = PLAYER_STRM;

    // 再生中のBGMがストリームサウンドなら途中再生情報を保存し, 再生を停止する
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {
      s32 sample_pos = m_soundSystem->GetStreamSamplePosition( current_handle );
      start_info.startOffset = sample_pos;
      m_soundSystem->Stop( current_handle );
    }

    return m_BGMLevelManager->PushBGMData( &start_info );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 退避したBGMを復元する
   * @param fadein_frame フェードインにかけるフレーム数
   * @retval true   復元に成功
   * @retval false  復元に失敗
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::PopBGM( u32 fadein_frame )
  {
    if( !m_BGMControlEnable ) {
      return true;
    }
    if( this->GetPushedBGMCount( ) <= 0 ){
      GFL_PRINT( "WARNING!! 復帰すべき BGM がありません。\n" );
      GFL_ASSERT(0); //@check iwasawa ケア済み
      return false; //製品版では退避しているBGMがなければ何もしない
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif

    u32 item = 0;
    u32 handle = 0;
    nw::snd::SoundStartable::StartInfo start_info;
    b32 pop_result = m_BGMLevelManager->PopBGMData( &item, &handle, &start_info );

    // 復元した階層がストリームサウンドなら, 停止した位置から再生する
    nw::snd::SoundArchive::SoundType sound_type = m_soundSystem->GetSoundTypeOfItem( item );
    if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {
			// fadein_frameが短すぎる場合、直後にBGM切り替え操作をすると
			// ライブラリ内のサウンドスレッドでフリーズ（コンフリクトが原因？）するので注意
			// その場合↓のPopBGMOnlyを使用する
      GFL_PRINT("#PopOffset %d\n",start_info.startOffset);
			m_soundSystem->Play( handle, item, fadein_frame, &start_info );
    }

    return pop_result;
  }

  b32 PMSoundSystem::PopBGMOnly( void )
  {
    if( !m_BGMControlEnable ) {
      return true;
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif

    u32 item = 0;
    u32 handle = 0;
    nw::snd::SoundStartable::StartInfo start_info;
    b32 pop_result = m_BGMLevelManager->PopBGMData( &item, &handle, &start_info );

    return pop_result;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 現在再生中のBGMと同じ位置から次のBGMの再生準備をする
   * @retval true   退避に成功
   * @retval false  退避に失敗
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::PrepareBGMContinue( u32 middleID )
  {
    if( !m_BGMControlEnable ) {
      return true;
    }
    if( this->IsBGMValid() == false ){
      GFL_ASSERT_MSG( 0, "有効なBGMがない\n");	//@check iwasawa ケア済み
      //次のBGMを頭から再生準備
      this->PrepareBGM( middleID );
      return false;
    }
#if defined(GF_PLATFORM_CTR)
    nn::os::CriticalSection::ScopedLock scope_lock( m_CSectionForSoundControl );
#endif

    nw::snd::SoundStartable::StartInfo start_info;
    start_info.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_START_OFFSET;
    start_info.startOffsetType = nw::snd::SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE;
    start_info.startOffset = -1;

    // 再生中のBGMがストリームサウンドなら途中再生情報を取得し、再生を停止する
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {
      s32 sample_pos = m_soundSystem->GetStreamSamplePosition( current_handle );
      start_info.startOffset = sample_pos;  //オフセットを設定
    }else{
      start_info.startOffset = 0;   //オフセットを設定
    }
    //現曲をストップ
    this->StopBGM( 0 );
    
    { //次曲のPrepareを開始
      u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
      m_BGMLevelManager->LoadBGMData( sound_itemID );
      m_soundSystem->PrepareSound( current_handle, sound_itemID, &start_info );
    }
    
    return true;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 退避中のBGMの数を取得する
   */
  //--------------------------------------------------------------------------
  u32 PMSoundSystem::GetPushedBGMCount( void ) const
  {
    return m_BGMLevelManager->GetCurrentLevel();
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 退避可能なBGMの最大数を取得する
   */
  //--------------------------------------------------------------------------
  u32 PMSoundSystem::GetMaxPushBGMCount( void ) const
  {
    return BGM_PUSH_COUNT_MAX;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMを一時停止する
   * @param fadeout_frame  フェードアウトフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PauseBGM( u32 fadeout_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->Pause( sound_handle_index, true, fadeout_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの一時停止を解除する
   * @param fadein_frame  フェードインフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PauseReleaseBGM( u32 fadein_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->Pause( sound_handle_index, false, fadein_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMが一時停止中かどうかを調べる
   * @retval true   一時停止中
   * @retval false  再生中
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsPauseBGM( void ) const
  {
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    return m_soundSystem->IsPause( sound_handle_index );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMがフェード中かどうかを調べる
   * @retval true   フェード中
   * @retval false  フェードしていない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsBGMFade( void ) const
  {
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    if( ( 0 < m_soundSystem->GetRemainingFadeFrames( sound_handle_index ) ) ||
        ( 0 < m_soundSystem->GetRemainingPauseFadeFrames( sound_handle_index ) ) ) 
    {
      return true;
    }
    return false;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの音量を変更する
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMVolume( f32 volume, u32 change_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetVolume( sound_handle_index, volume, change_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの音量を変更する
   * @param track_bit     対象トラックのビットフラグ
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTrackVolume( u32 track_bit, f32 volume, u32 change_frame )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTrackVolume( sound_handle_index, track_bit, volume, change_frame );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMのピッチを変更する
   * @param value  周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMPitch( f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetPitch( sound_handle_index, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMのピッチを12平均律に即した音程に変更する
   * @param level 12平均律のレベル
   * @note
   * level == 2*12   2オクターブ高い音程
   * level == 1*12   1オクターブ高い音程
   * level == 0      通常の音程
   * level == -1*12  2オクターブ高い音程
   * level == -2*12  1オクターブ高い音程
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMPitchByEqualTemperament( s8 level )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetPitchByEqualTemperament( sound_handle_index, level );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの特定のトラックのピッチを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTrackPitch( u32 track_bit, f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTrackPitch( sound_handle_index, track_bit, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックのピッチを12平均律に即した音程に変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param level 12平均律のレベル
   * @note
   * level == 2*12   2オクターブ高い音程
   * level == 1*12   1オクターブ高い音程
   * level == 0      通常の音程
   * level == -1*12  2オクターブ高い音程
   * level == -2*12  1オクターブ高い音程
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTrackPitchByEqualTemperament( u32 track_bit, s8 level )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTrackPitchByEqualTemperament( sound_handle_index, track_bit, level );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの水平方向パンを変更する
   * @param value  パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMPan( f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetPan( sound_handle_index, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの水平方向パンを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTrackPan( u32 track_bit, f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTrackPan( sound_handle_index, track_bit, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの垂直方向パンを変更する
   * @param value  パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMSurroundPan( f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetSurroundPan( sound_handle_index, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMの特定トラックの垂直方向パンを変更する
   * @param track_bit 対象トラックのビットフラグ
   * @param value     パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTrackSurroundPan( u32 track_bit, f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTrackSurroundPan( sound_handle_index, track_bit, value );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief BGMのテンポを変更する
   * @param value  テンポを倍率で指定( 1.0fで元に戻る )
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeBGMTempoRatio( f32 value )
  {
    if( !m_BGMControlEnable ) {
      return;
    }
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    m_soundSystem->SetTempoRatio( sound_handle_index, value );
  }






  // SE
  
  //--------------------------------------------------------------------------
  /**
   * @brief EnvSE(環境音)マネージャを取得する
   */
  //--------------------------------------------------------------------------
  EnvSEManager* PMSoundSystem::GetEnvSEManager( void ) const
  {
    return m_EnvSEManager;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEの階層マネージャを取得する
   */
  //--------------------------------------------------------------------------
  SELevelManager* PMSoundSystem::GetSELevelManager( void ) const
  {
    return m_SELevelManager;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEのサウンドデータをヒープに積む
   * @param middleID  読み込むサウンドデータIDの中間ID
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::LoadAndPushSESoundData( u32 middleID )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    m_SELevelManager->LoadSEData( sound_itemID );
    m_SELevelManager->PushSEData();
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 1階層下にあるSEのサウンドデータをヒープから削除する
   * @param middleID  削除するサウンドデータIDの中間ID
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PopAndUnloadSESoundData( u32 middleID )
  {
    u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    m_SELevelManager->PopSEData();
    m_SELevelManager->UnloadSEData( sound_itemID );
  }
  
  //--------------------------------------------------------------------------
  /**
   * @brief 全階層のSEのサウンドデータをヒープから削除する
   *
   * @note  BTS4850 SoftwareResetから呼ばれるSoundSystemのFinalize用
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PopAndUnloadSESoundDataAll( void )
  {
    u32 level = m_SELevelManager->GetCurrentLevel();
    u32 pop_sound_itemID = 0;
    
    //積まれている分だけ破棄
    for( u32 i = 0; i < level; ++i )
    {
      if( m_SELevelManager->PopSEData( &pop_sound_itemID ) != false ){
        m_SELevelManager->UnloadSEData( pop_sound_itemID );
      }
    }
  }


  //--------------------------------------------------------------------------
  /**
   * @birief SEを再生する
   * @param handle        再生に使用するSEハンドルのインデックス
   * @param middleID          再生するSE
   * @param fadein_frame  フェードインにかけるフレーム数
   * @param start_info    再生パラメータ
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlaySE(
      HandleIndex handle, u32 middleID, u32 fadein_frame,
      nw::snd::SoundStartable::StartInfo* start_info )
  {
    if( this->IsSEHandle( handle ) ) 
    {
      if( start_info ) {
        start_info->playerId = MiddleIDConverter::CalcSoundItemID( start_info->playerId );
      }
      u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      m_soundSystem->Play( handle, sound_itemID, fadein_frame, start_info );

#if PM_DEBUG
      DebugCheckRomAccess( handle, middleID );
#endif // PM_DEBUG
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief ボリューム指定をしてSEを再生する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param middleID    再生するSE
   * @param volume  ボリューム( 倍率 )
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlaySEVolume( HandleIndex handle, u32 middleID, f32 volume )
  {
    if( this->IsSEHandle( handle ) ) {
      u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      m_soundSystem->Play( handle, sound_itemID );
      m_soundSystem->SetVolume( handle, volume );
    }
  }

  /**
   * @brief SE用ハンドルかどうかを調べる
   * @param handle チェックするハンドル
   * @retval true   SEのハンドル
   * @retval false  SE以外のハンドル
   */
  b32 PMSoundSystem::IsSEHandle( HandleIndex handle ) const
  {
    return( ( SHANDLE_SE_HEAD <= handle ) && ( handle <= SHANDLE_SE_TAIL ) );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEを停止する
   * @param handle         停止するSEハンドルのインデックス
   * @param fadeout_frame  フェードアウトにかけるフレーム数
   *
   * @note
   * サウンドハンドルと指定されたSEとの関連付けが無効になっている場合, 停止できない.
   * 停止したいSEと, 同時に再生されるSEはハンドルを分ける必要がある.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopSE( HandleIndex handle, u32 fadeout_frame )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->Stop( handle, fadeout_frame );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 全てのSEを停止する
   *
   * @note
   * サウンドハンドルとの関連付けが無効になったSEが存在するかもしれないため
   * プレイヤー単位で停止させる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopAllSE( void )
  {
    u32 PLAYER_ID[] = {
      PLAYER_SE_1,
      PLAYER_SE_2,
      PLAYER_SE_3,
      PLAYER_SE_4,
      PLAYER_SE_ENV,
      PLAYER_SE_FT,
      PLAYER_SE_GLOBAL,
    };

    for( u32 i=0; i<GFL_NELEMS(PLAYER_ID); i++ )
    {
      u32 itemID = MiddleIDConverter::CalcSoundItemID( PLAYER_ID[i] );
      m_soundSystem->StopPlayerSound( itemID, 0 );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 何でもいいので再生中かどうかを調べる
   * @param handle  再生に使用するSEハンドルのインデックス
   * @retval true   再生中
   * @retval false  再生していない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsSEPlaying( HandleIndex handle ) const
  {
    if( this->IsSEHandle( handle ) ) {
      u32 attached_itemID = m_soundSystem->GetAttachedSound( handle );
      return ( attached_itemID != gfl2::sound::SoundSystem::INVALID_SOUND_ID );
    }
    return false;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 指定したSEを再生しているかどうかを調べる
   * @param handle  停止させるSEハンドルのインデックス
   * @param middleID    調べるSE
   * @retval true   再生している
   * @retval false  再生していない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsSEPlaying( HandleIndex handle, u32 middleID ) const
  {
    if( this->IsSEHandle( handle ) ) {
      u32 attached_itemID = m_soundSystem->GetAttachedSound( handle );
      u32 check_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      return ( attached_itemID == check_itemID );
    }
    return false;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEの音量を変更する
   * @param handle        再生に使用するSEハンドルのインデックス
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSEVolume(
      HandleIndex handle, f32 volume, u32 change_frame )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetVolume( handle, volume, change_frame );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEのピッチを変更する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param value   周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSEPitch( HandleIndex handle, f32 value )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetPitch( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEの水平方向パンを変更する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param value   パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSEPan( HandleIndex handle, f32 value )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetPan( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEの垂直方向パンを変更する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param value   パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSESurroundPan( HandleIndex handle, f32 value )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetSurroundPan( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEのテンポを変更する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param value   テンポを倍率で指定( 1.0fで元に戻る )
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSETempoRatio( HandleIndex handle, f32 value )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetTempoRatio( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief SEのトランスポーズ
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param value   トランスポーズ値指定
   * @param bit			対象トラック指定(bit)
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeSETranspose( HandleIndex handle, s8 value, u32 bit )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetTranspose( handle, value, bit );
    }
  }


  //---------------------------------------------------------------------------
  /**
   * @brief SEのトラック変数を変更する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param trackNo  トラックNo
   * @param varNo    変数番号(0-15)
   * @param value    書き込む変数値
   */
  //---------------------------------------------------------------------------
  void PMSoundSystem::ChangeSETrackVariable( 
      HandleIndex handle, int trackNo, int varNo, s16 value )
  {
    if( this->IsSEHandle( handle ) ) {
      m_soundSystem->SetTrackVariable( handle, trackNo, varNo, value );
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief SEのトラック変数を取得する
   * @param handle  再生に使用するSEハンドルのインデックス
   * @param trackNo  トラックNo
   * @param varNo    変数番号(0-15)
   * @retval s16    変数値
   */
  //---------------------------------------------------------------------------
  s16 PMSoundSystem::GetSETrackVariable( 
      HandleIndex handle, int trackNo, int varNo )
  {
    if( this->IsSEHandle( handle ) ) {
      return m_soundSystem->GetTrackVariable( handle, trackNo, varNo );
    }
    return 0;
  }

  //--------------------------------------------------------------------------
  void PMSoundSystem::Set3DParam( const gfl2::sound::SoundSystem::Sound3DParam& param )
  {
    m_soundSystem->Set3DParam( param );
  }

  void PMSoundSystem::Get3DParam( gfl2::sound::SoundSystem::Sound3DParam* pOutParam ) const
  {
    m_soundSystem->Get3DParam( pOutParam );
  }

  void PMSoundSystem::SetListenerParam( const gfl2::sound::SoundSystem::ListenerParam& param )
  {
    m_soundSystem->SetListenerParam( param );
  }

  void PMSoundSystem::GetListenerParam( gfl2::sound::SoundSystem::ListenerParam* pOutParam ) const
  {
    m_soundSystem->GetListenerParam( pOutParam );
  }

  void PMSoundSystem::SetListenerMatrix( const gfl2::math::MTX34& matrix )
  {
    m_soundSystem->SetListenerMatrix( matrix );
  }

  void PMSoundSystem::SetListenerPos( const gfl2::math::VEC3& pos, const gfl2::math::VEC3& up, const gfl2::math::VEC3& dir )
  {
    m_soundSystem->SetListenerPos( pos, up, dir );
  }

  void PMSoundSystem::SetListenerVelocity( const gfl2::math::VEC3& velocity )
  {
    m_soundSystem->SetListenerVelocity( velocity );
  }

  b32 PMSoundSystem::Play3DActor( u8 actor_index, u32 middleID, const nw::snd::SoundStartable::StartInfo* start_info )
  {
    u32 itemID = MiddleIDConverter::CalcSoundItemID( middleID );

    if( start_info )
    {
      nw::snd::SoundStartable::StartInfo true_info = *start_info;
      if( true_info.enableFlag & nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID ) {
        true_info.playerId = MiddleIDConverter::CalcSoundItemID( true_info.playerId );
      }
      return m_soundSystem->Play3DActor( actor_index, itemID, &true_info );
    }
    else
    {
      return m_soundSystem->Play3DActor( actor_index, itemID );
    }
  }

  b32 PMSoundSystem::PlayVoice3DActor( u8 actor_index, HandleIndex handle, u32 middleID )
  {
    if( this->IsVoiceHandle( handle ) ) {
      u32 itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      return m_soundSystem->Play3DActor( actor_index, itemID );
    }
    return false;
  }
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
  b32 PMSoundSystem::Prepare3DActor( u8 actor_index, u32 middleID, const nw::snd::SoundStartable::StartInfo* start_info )
  {
    u32 itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    return m_soundSystem->Prepare3DActor( actor_index, itemID, start_info );
  }
 
  //-------------------------------------------------------------------------
  /**
   * @brief 3Dアクターの再生準備ができているか？
   *
   * @param actorIdx    3Dアクターのインデックス
   *
   * @return すでに再生準備ができている場合 true
   */
  //-------------------------------------------------------------------------
  b32 PMSoundSystem::IsPrepared3DActor( u8 actor_index )
  {
    return m_soundSystem->IsPrepared3DActor( actor_index );
  }
  
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
  void PMSoundSystem::PlayPrepared3DActor( u8 actor_index )
  {
    m_soundSystem->PlayPrepared3DActor( actor_index );
  }

  void PMSoundSystem::Stop3DActor( u8 actor_index, s32 fadeFrames )
  {
    m_soundSystem->Stop3DActor( actor_index, fadeFrames );
  }

  //-----------------------------------------------------------------------
  /**
   * @brief 3Dアクターで再生中のサウンド数を取得する
   * @param actorIdx        制御を行う3Dアクターのインデックス
   * @param actorPlayerIdx  対象のアクタープレイヤーのインデックス
   * 
   * @node  return>0 なら再生中
   */
  //-----------------------------------------------------------------------  
  u32 PMSoundSystem::Get3DActorPlayingCount( u8 actor_index, u8 actorPlayerIdx ) const
  {
    if( actorPlayerIdx >= nw::snd::Sound3DActor::ACTOR_PLAYER_COUNT ){
      GFL_ASSERT_MSG( 0, "actorPlayerIdx >= nw::snd::Sound3DActor::ACTOR_PLAYER_COUNT" );  //@check iwasawa ケア済み
      actorPlayerIdx = 0;
    }
    return m_soundSystem->Get3DActorPlayingSoundCount( actor_index, actorPlayerIdx );
  }


  //-----------------------------------------------------------------------
  /**
   * @brief 3Dアクターが全アクタープレイヤーで再生中のサウンド数を取得する
   * @param actorIdx        制御を行う3Dアクターのインデックス
   * @param actorPlayerIdx  対象のアクタープレイヤーのインデックス
   * 
   * @node  return>0 ならアクターが持つ4つのうちいずれかのプレイヤーが再生中
   */
  //-----------------------------------------------------------------------  
  u32 PMSoundSystem::Get3DActorPlayingCount( u8 actor_index ) const
  {
    return m_soundSystem->Get3DActorPlayingSoundCount( actor_index );
  }

  //-----------------------------------------------------------------------
  /**
   * @brief 3Dアクターの指定プレイヤーが再生中かを返す
   * @param actorIdx        制御を行う3Dアクターのインデックス
   * @param actorPlayerIdx  対象のアクタープレイヤーのインデックス
   * 
   * @retval  true  再生中
   * @retval  false 再生していない
   */
  //-----------------------------------------------------------------------  
  b32 PMSoundSystem::IsPlaying3DActor( u8 actor_index, u8 actorPlayerIdx ) const
  {
    if( actorPlayerIdx >= nw::snd::Sound3DActor::ACTOR_PLAYER_COUNT ){
      GFL_ASSERT_MSG( 0, "actorPlayerIdx >= nw::snd::Sound3DActor::ACTOR_PLAYER_COUNT" );  //@check iwasawa ケア済み
      actorPlayerIdx = 0;
    }
    return(this->Get3DActorPlayingCount( actor_index, actorPlayerIdx ) > 0);
  }

  //-----------------------------------------------------------------------
  /**
   * @brief 3Dアクターの全プレイヤーのいずれかが再生中かを返す
   * @param actorIdx        制御を行う3Dアクターのインデックス
   * 
   * @retval  true  4つあるプレイヤーのいずれかが再生中
   * @retval  false 全プレイヤーが再生していない
   */
  //-----------------------------------------------------------------------  
  b32 PMSoundSystem::IsPlaying3DActor( u8 actor_index ) const
  {
    return(this->Get3DActorPlayingCount( actor_index ) > 0);
  }

  void PMSoundSystem::Pause3DActor( u8 actor_index, b32 pauseOn, s32 fadeFrames )
  {
    m_soundSystem->Pause3DActor( actor_index, GFL_BOOL_CAST(pauseOn), fadeFrames );
  }

  void PMSoundSystem::Set3DActorVolume( u8 actor_index, f32 volume )
  {
    m_soundSystem->Set3DActorVolume( actor_index, volume );
  }

  f32 PMSoundSystem::Get3DActorVolume( u8 actor_index ) const
  {
    return m_soundSystem->Get3DActorVolume( actor_index );
  }

  void PMSoundSystem::Set3DActorPitch( u8 actor_index, f32 pitch )
  {
    m_soundSystem->Set3DActorPitch( actor_index, pitch );
  }

  f32 PMSoundSystem::Get3DActorPitch( u8 actor_index ) const
  {
    return m_soundSystem->Get3DActorPitch( actor_index );
  }

  void PMSoundSystem::Set3DActorPan( u8 actor_index, f32 pan )
  {
    m_soundSystem->Set3DActorPan( actor_index, pan );
  }

  f32 PMSoundSystem::Get3DActorPan( u8 actor_index ) const
  {
    return m_soundSystem->Get3DActorPan( actor_index );
  }

  void PMSoundSystem::Set3DActorPos( u8 actor_index, const gfl2::math::VEC3& pos )
  {
    m_soundSystem->Set3DActorPos( actor_index, pos );
  }

  void PMSoundSystem::Get3DActorPos( u8 actor_index, gfl2::math::VEC3* pos ) const
  {
    m_soundSystem->Get3DActorPos( actor_index, pos );
  }

  void PMSoundSystem::Set3DActorVelocity( u8 actor_index, const gfl2::math::VEC3& velocity )
  {
    m_soundSystem->Set3DActorVelocity( actor_index, velocity );
  }
  
#if defined(GF_PLATFORM_CTR)
  b32 PMSoundSystem::AppendReverbEffect( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param )
  {
    return m_soundSystem->AppendReverbEffectCTR( busID, reverb_buffer_heap, param );
  }
#elif defined(GF_PLATFORM_WIN32)
  b32 PMSoundSystem::AppendReverbEffect( nw::snd::AuxBus busID, gfl2::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverbStd::ReverbStdParam & param )
  {
    return m_soundSystem->AppendReverbEffectWin32( busID, reverb_buffer_heap, param );
  }
#endif

  /*
   *  GFBTS2506 130514 by iwasawa 
   *  SoundSystem::RemoveReverbEffect()内でfadeTimesを待たずに
   *  リバーブ用バッファSoundSystem::m_pMemoryForSoundReverbを解放してしまう潜在バグがある
   *
   *  アッパーでは修正可能だが、kujiraでは時期を鑑みFadeTimesを必ず0で運用することで
   *  問題の顕在化するのを防ぐ対処を行う
   */
  void PMSoundSystem::RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes )
  {
    GFL_UNUSED(fadeTimes);
    m_soundSystem->RemoveReverbEffect( busID, /*fadeTimes*/0 ); //GFBTS2506 対策で強制的に0にする
  }
  b32 PMSoundSystem::AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect )
  {
    return m_soundSystem->AppendEffect( busID, effect );
  }
  void PMSoundSystem::RemoveEffect( nw::snd::AuxBus busID, int fadeTimes )
  {
    m_soundSystem->RemoveEffect( busID, fadeTimes );
  }





  // Voice

  //--------------------------------------------------------------------------
  /**
   * @birief Voiceを再生する
   * @param handle   再生に使用するVoiceハンドルのインデックス
   * @param middleID 再生するVoiceの中間ID
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlayVoice( HandleIndex handle, u32 middleID )
  {
    this->PlayVoiceCore( handle, middleID, 1.0f );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief ボリューム指定をしてVoiceを再生する
   * @param handle   再生に使用するVoiceハンドルのインデックス
   * @param middleID 再生するVoiceの中間ID
   * @param volume   ボリューム( 倍率 )
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::PlayVoiceVolume( HandleIndex handle, u32 middleID, f32 volume )
  {
    this->PlayVoiceCore( handle, middleID, volume );
  }

  /**
   * @brief Voiceを再生する
   * @param handle   再生に使用するVoiceハンドルのインデックス
   * @param middleID 再生するVoiceの中間ID
   * @param volume   ボリューム( 倍率 )
   */
  void PMSoundSystem::PlayVoiceCore( HandleIndex handle, u32 middleID, f32 volume )
  {
    if( this->IsVoiceHandle( handle ) ) {
      u32 sound_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      m_soundSystem->Play( handle, sound_itemID );
      m_soundSystem->SetVolume( handle, volume );
    }
  }

  /**
   * @brief VOICE用ハンドルかどうかを調べる
   * @param handle チェックするハンドル
   * @retval true   VOICEのハンドル
   * @retval false  VOICE以外のハンドル
   */
  b32 PMSoundSystem::IsVoiceHandle( HandleIndex handle ) const
  {
    return( ( SHANDLE_VOICE_HEAD <= handle ) && ( handle <= SHANDLE_VOICE_TAIL ) );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceを停止する
   * @param handle   停止させるVoiceハンドルのインデックス
   * @param middleID 停止させるVoice
   *
   * @note
   * サウンドハンドルと指定されたVoiceとの関連付けが無効になっている場合, 停止できない.
   * 停止したいVoiceと, 同時に再生されるVoiceはハンドルを分ける必要がある.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopVoice( HandleIndex handle, u32 middleID, u32 frame )
  {
    if( this->IsVoiceHandle( handle ) ) {
      u32 attatched_itemID = m_soundSystem->GetAttachedSound( handle );
      u32 check_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      if( attatched_itemID == check_itemID ) {
        m_soundSystem->Stop( handle, frame );
      }
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 全てのVoiceを停止する
   *
   * @note
   * サウンドハンドルとの関連付けが無効になったVoiceが存在するかもしれないため
   * プレイヤー単位で停止させる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::StopAllVoice( void )
  {
    u32 PLAYER_ID[] = {
      PLAYER_PV,
    };

    for( u32 i=0; i<GFL_NELEMS(PLAYER_ID); i++ )
    {
      u32 itemID = MiddleIDConverter::CalcSoundItemID( PLAYER_ID[i] );
      m_soundSystem->StopPlayerSound( itemID, 0 );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceを再生しているかどうかを調べる
   * @param handle   停止させるVoiceハンドルのインデックス
   * @param middleID 調べるVoiceの中間ID
   * @retval true   再生している
   * @retval false  再生していない
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::IsVoicePlaying( HandleIndex handle, u32 middleID ) const
  {
    if( this->IsVoiceHandle( handle ) ) {
      u32 check_itemID = MiddleIDConverter::CalcSoundItemID( middleID );
      u32 attatched_itemID = m_soundSystem->GetAttachedSound( handle );
      return ( attatched_itemID == check_itemID );
    }
    return false;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceの音量を変更する
   * @param handle        再生に使用するVoiceハンドルのインデックス
   * @param volume        音量の倍率( 1.0fで等倍 )
   * @param change_frame  音量変化にかけるフレーム数
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeVoiceVolume(
      HandleIndex handle, f32 volume, u32 change_frame )
  {
    if( this->IsVoiceHandle( handle ) ) {
      m_soundSystem->SetVolume( handle, volume, change_frame );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceのピッチを変更する
   * @param handle  再生に使用するVoiceハンドルのインデックス
   * @param value   周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeVoicePitch( HandleIndex handle, f32 value )
  {
    if( this->IsVoiceHandle( handle ) ) {
      m_soundSystem->SetPitch( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceの水平方向パンを変更する
   * @param handle  再生に使用するVoiceハンドルのインデックス
   * @param value   パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeVoicePan( HandleIndex handle, f32 value )
  {
    if( this->IsVoiceHandle( handle ) ) {
      m_soundSystem->SetPan( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceの垂直方向パンを変更する
   * @param handle  再生に使用するVoiceハンドルのインデックス
   * @param value   パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeVoiceSurroundPan( HandleIndex handle, f32 value )
  {
    if( this->IsVoiceHandle( handle ) ) {
      m_soundSystem->SetSurroundPan( handle, value );
    }
  }

  //--------------------------------------------------------------------------
  /**
   * @brief Voiceのテンポを変更する
   * @param handle  再生に使用するVoiceハンドルのインデックス
   * @param value   テンポを倍率で指定( 1.0fで元に戻る )
   */
  //--------------------------------------------------------------------------
  void PMSoundSystem::ChangeVoiceTempoRatio( HandleIndex handle, f32 value )
  {
    if( this->IsVoiceHandle( handle ) ) {
      m_soundSystem->SetTempoRatio( handle, value );
    }
  }






  /**
   * @brief サウンドシステムを取得する
   */
  gfl2::sound::SoundSystem* PMSoundSystem::GetSoundSystem( void )
  {
    return m_soundSystem;
  }
  

  /**
   * @brief 現在なっているサウンドハンドルを返す
   */
  nw::snd::SoundHandle* PMSoundSystem::GetSoundHandleOnCurrent( void )
  {
    u32 sound_handle_index = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    return this->GetSoundSystem()->GetSoundHandle( sound_handle_index );
  }

  //--------------------------------------------------------------------------
  /**
   * @brief 現在なっているストリームサウンドの再生位置を取得する
   * @return 再生位置（サンプル数） 失敗した場合は負の値
   */
  //--------------------------------------------------------------------------
  int PMSoundSystem::GetStreamSoundPlaySamplePosition( void )
  {
    if( !m_BGMControlEnable ) {
      return -1;
    }
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM )
    {
      nw::snd::SoundHandle* handle = GetSoundHandleOnCurrent();
      if( handle )
      {
        nw::snd::StreamSoundHandle stream_handle( handle );
        if( stream_handle.IsAttachedSound() )
        {
          return stream_handle.GetPlaySamplePosition();
        }
      }
    }
    return -1;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief サンプリングレートを取得する
   * @return 再生位置（サンプル数） 失敗した場合は負の値
   */
  //--------------------------------------------------------------------------
  int PMSoundSystem::GetStreamSoundSampleRate( void )
  {
    if( !m_BGMControlEnable ) {
      return -1;
    }
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM )
    {
      nw::snd::SoundHandle* handle = GetSoundHandleOnCurrent();
      if( handle )
      {
        nw::snd::StreamSoundHandle stream_handle( handle );

        if( stream_handle.IsAttachedSound() )
        {
          if( stream_handle.IsPrepared() )
          {
            nw::snd::StreamDataInfo data_info;
            if( stream_handle.ReadStreamDataInfo( &data_info ) )
            {
              return data_info.sampleRate;
            }
          }
        }
      }
    }
    return -1;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループフラグを取得する
   *
   * @return  ストリームデータのループフラグを取得する
   * @note ハンドルが無効の場合は負の値 有効ならtrue 無効ならfalse
   */
  //--------------------------------------------------------------------------
  b32 PMSoundSystem::GetStreamSoundLoopFlag( void )
  {
    if( !m_BGMControlEnable ) {
      return -1;
    }
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM )
    {
      nw::snd::SoundHandle* handle = GetSoundHandleOnCurrent();
      if( handle )
      {
        nw::snd::StreamSoundHandle stream_handle( handle );

        if( stream_handle.IsAttachedSound() )
        {
          if( stream_handle.IsPrepared() )
          {
            nw::snd::StreamDataInfo data_info;
            if( stream_handle.ReadStreamDataInfo( &data_info ) )
            {
              return data_info.loopFlag;
            }
          }
        }
      }
    }
    return -1;
  }

  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループ開始位置を取得する
   *
   * @return  ストリームデータのループ開始位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  //--------------------------------------------------------------------------
  int PMSoundSystem::GetStreamSoundLoopStartPos( void )
  {
    if( !m_BGMControlEnable ) {
      return -1;
    }
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM )
    {
      nw::snd::SoundHandle* handle = GetSoundHandleOnCurrent();
      if( handle )
      {
        nw::snd::StreamSoundHandle stream_handle( handle );

        if( stream_handle.IsAttachedSound() )
        {
          if( stream_handle.IsPrepared() )
          {
            nw::snd::StreamDataInfo data_info;
            if( stream_handle.ReadStreamDataInfo( &data_info ) )
            {
              return data_info.loopStart;
            }
          }
        }
      }
    }
    return -1;
  }


  //--------------------------------------------------------------------------
  /**
   * @brief ストリームデータのループ終了位置を取得する
   *
   * @return  ストリームデータのループ終了位置を取得する  
   * @note ハンドルが無効の場合は負の値
   */
  //--------------------------------------------------------------------------
  int PMSoundSystem::GetStreamSoundLoopEndPos( void )
  {
    if( !m_BGMControlEnable ) {
      return -1;
    }
    u32 current_handle = m_BGMLevelManager->GetSoundHandleIndexOnCurrentLevel();
    nw::snd::SoundArchive::SoundType current_sound_type = m_soundSystem->GetSoundType( current_handle );
    if( current_sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM )
    {
      nw::snd::SoundHandle* handle = GetSoundHandleOnCurrent();
      if( handle )
      {
        nw::snd::StreamSoundHandle stream_handle( handle );

        if( stream_handle.IsAttachedSound() )
        {
          if( stream_handle.IsPrepared() )
          {
            nw::snd::StreamDataInfo data_info;
            if( stream_handle.ReadStreamDataInfo( &data_info ) )
            {
              return data_info.loopEnd;
            }
          }
        }
      }
    }
    return -1;
  }


#if PM_DEBUG
  /**
   * @brief BGM 操作が可能かどうかをセットする
   * @param enable  true なら操作可能
   */
  void PMSoundSystem::SetBGMControlEnable( b32 enable )
  {
    m_BGMControlEnable = enable;
  }

  /**
   * @brief BGM 操作が有効かどうかを調べる
   * @retval true   有効
   * @retval false  無効
   */
  b32 PMSoundSystem::IsBGMControlEnable( void )
  {
    return m_BGMControlEnable;
  }

  /**
   *  @brief ROMAccess頻度のチェックを行なう。
   */
  void PMSoundSystem::DebugCheckRomAccess( HandleIndex handle, u32 middleID ) const
  {
    if( m_bRomAccessCheckMode > DEBUG_ROMACCESS_MODE_IDLE )
    {
      const nw::snd::SoundHandle* cp_handle =  m_soundSystem->GetSoundHandle( handle );

      // 非常駐SE条件
      if( (cp_handle != NULL) && !cp_handle->IsPrepared() )
      {
        // 計測開始条件
        if( !m_bRomAccessCheckStart )
        {
          m_RomAccessStartTime  = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();
          m_RomBeforeAccessTime = m_RomAccessStartTime;
          m_bRomAccessCheckStart = true;
        }
        // 計測中
        else
        {
          s64 now = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();
          s64 diff = now - m_RomBeforeAccessTime;

          // 計測継続条件
          if( (diff >= 0) && (diff < (1000 * 2)) )
          {
            TOMOYA_PRINT( "diff %d now %d before %d\n", (s32)(diff), (s32)(now), (s32)(m_RomBeforeAccessTime) );
            m_RomBeforeAccessTime = now;

            // 警告表示条件
            if( (now - m_RomAccessStartTime) >= (1000 * 5) )
            {
              TOMOYA_PRINT( "diff5 %d now %d startTime %d\n", (s32)(now - m_RomAccessStartTime), (s32)(now), (s32)(m_RomAccessStartTime) );
              switch(m_bRomAccessCheckMode)
              {
              case DEBUG_ROMACCESS_MODE_PRINT:
                GFL_RELEASE_PRINT( "2秒以内に1回以上の頻度で5秒にわたって非常駐SEの読み込みを行なっています SE[%d]\n", middleID );
                break;

              case DEBUG_ROMACCESS_MODE_ASSERT:
                GFL_ASSERT_MSG( 0, "2秒以内に1回以上の頻度で5秒にわたって非常駐SEの読み込みを行なっています SE[%d]\n", middleID );
                break;

              default:
                // 禁止
                GFL_ASSERT(0);
                break;
              }
            }
          }
          // 計測終了条件
          else
          {
            // 再計測開始
            m_RomAccessStartTime  = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();
            m_RomBeforeAccessTime = m_RomAccessStartTime;
            m_bRomAccessCheckStart = true;
          }
        }
      }
    }
  }

  void PMSoundSystem::DumpHeapInfo(void)
  {
    GFL_PRINT("BgmHeapSize = 0x%x / 0x%x\n", m_soundHeap[0]->GetNwSoundHeap()->GetFreeSize(), m_soundHeap[0]->GetNwSoundHeap()->GetSize());
    GFL_PRINT("SeHeapSize = 0x%x / 0x%x\n", m_soundHeap[1]->GetNwSoundHeap()->GetFreeSize(), m_soundHeap[1]->GetNwSoundHeap()->GetSize());
  }

#endif



} // namespace xy_snd


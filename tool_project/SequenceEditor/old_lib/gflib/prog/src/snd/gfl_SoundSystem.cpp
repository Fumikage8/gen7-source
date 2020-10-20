//=============================================================================
/**
 * @file gfl_SoundSystem.cpp
 * @brief サウンドシステム
 * @author obata_toshihiro
 * @date 2010.10.29
 */
//=============================================================================
#include <nw.h>
#ifdef GFL_SND_ENABLE_AAC
#include <nw/snd/aac.h>
#include <nn/aacdec.h>
#endif
#if GFL_HIO_ENABLE
#include <nw/snd/snd_HioSoundArchive.h>
#endif
#include "gfl_debug.h"
#include "gfl_Math.h"
#include "gfl_Heap.h"
#include "snd/gfl_SoundSystem.h"
#include "snd/gfl_SoundHeap.h"
#include "ro/gfl_RoManager.h"
#include "gfl_SoundDataManagerThread.h"

#define GFL_SOUNDSYSTEM_HEAP_CHECK 0
#define GFL_SOUNDSYSTEM_PLAYER_HEAP_SIZE_CHECK 0


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::snd::SoundSystem);
template class gfl::base::SingletonAccessor<gfl::snd::SoundSystem>;
#endif


namespace gfl {
  namespace snd {


    // 12平均律で音程を1オクターブ上げるための周波数比
    const f32 SoundSystem::m_highFrequencyRatioForEqualTemperament[ SoundSystem::EQUAL_TEMPERAMENT_LEVEL_NUM ] =
    {
      1.000000f,
      1.059463f,
      1.122462f, 
      1.189207f,
      1.259921f,
      1.334840f,
      1.414214f,
      1.498307f, 
      1.587401f,
      1.681793f,
      1.781797f,
      1.887749f,
      2.000000f,
    };

    // 12平均律で音程を1オクターブ下げるための周波数比
    const f32 SoundSystem::m_lowFrequencyRatioForEqualTemperament[ SoundSystem::EQUAL_TEMPERAMENT_LEVEL_NUM ] =
    {
      1.000000f,
      0.943874f, 
      0.890899f,
      0.840896f, 
      0.793701f,
      0.749153f,
      0.707107f,
      0.667420f,
      0.629961f, 
      0.594604f,
      0.561231f,
      0.529732f,
      0.500000f,
    };


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param const InitParam& initParam 初期化パラメータ
     */
    //-------------------------------------------------------------------------
    SoundSystem::SoundSystem( const InitParam& initParam ) : 
      m_pAppHeap( initParam.pAppHeap ),
      m_pDevHeap( initParam.pDevHeap ),
      m_pSoundArchive( NULL ),
      m_romSoundArchive(),
      m_memSoundArchive(),
      m_soundDataManager(),
      m_soundArchivePlayer(),
      m_soundHeap( NULL ),
      m_pSoundDataManagerThread(NULL),
      m_SoundDataManagerThreadResult(false),
      m_criticalSectionForSoundDataManagerLoad(),
      m_pSoundHandles( NULL ),
      m_soundHandleNum( 0 ),
      m_sound3DManager(),
      m_sound3DListener(),
      m_pSound3DActors( NULL ),
      m_pSound3DHandles( NULL ),
      m_sound3DActorNum( 0 ),
      m_pMemoryForSoundReverb( NULL ),
      m_pMemoryForDspComponent( NULL ),
      m_pMemoryForAacLibrary( NULL ),
      m_pMemoryForSoundSystem( NULL ),
      m_pMemoryForSoundArchiveHeader( NULL ),
      m_pMemoryForSoundDataManager( NULL ),
      m_pMemoryForSoundArchivePlayer( NULL ),
      m_pMemoryForStreamBuffer( NULL ), 
      m_pMemoryForSoundHeap( NULL ),
      m_pMemoryForSound3DManager( NULL ),
      m_isAacLibraryInitialized( false )
#if GFL_HIO_ENABLE
      ,m_hioSoundArchive()
#endif
    {
      this->InitDspComponent( initParam );
      this->InitSDKLibrary();
      this->InitSoundSystem( initParam );
      this->InitSoundArchive( initParam );
      this->InitSoundDataManager();
      this->InitSoundArchivePlayer();
      this->InitSoundHeap( initParam );
      this->CreateSoundHandles( initParam );
      this->InitSound3DManager();
      this->InitSound3DListener();
      this->CreateSound3DActorsAndHandles( initParam );
      this->InitSound3DActors();
    }

    /**
     * @brief DSP コンポーネントを初期化する
     */
    void SoundSystem::InitDspComponent( const InitParam& initParam )
    {
      nn::Result result;

      result = nn::dsp::Initialize();
      GFL_ASSERT( result.IsSuccess() ); //@check

#ifdef GFL_SND_ENABLE_AAC

      if( initParam.aacUse )
      {
        // AAC デコード用コンポーネントをロード
        nn::fs::FileReader reader( initParam.aacDspComponentPath );
        size_t size = reader.GetSize();
        m_pMemoryForDspComponent = GflHeapAllocMemory( m_pDevHeap, size );
        s32 read_size = reader.Read( m_pMemoryForDspComponent, size );
        reader.Finalize();
        result = nn::dsp::LoadComponent( static_cast<u8*>(m_pMemoryForDspComponent), size );
        GFL_ASSERT( result.IsSuccess() ); //@check
#if GFL_SOUNDSYSTEM_HEAP_CHECK
        GFL_PRINT( "DspComponent\t%d\n", size );
#endif
      }
      else
      {
        // デフォルトコンポーネントをロード
        result = nn::dsp::LoadDefaultComponent();
        GFL_ASSERT( result.IsSuccess() ); //@check
      }

#else // GFL_SND_ENABLE_AAC

      {
        // デフォルトコンポーネントをロード
        result = nn::dsp::LoadDefaultComponent();
        GFL_ASSERT( result.IsSuccess() ); //@check
      }
#endif
    }

    /**
     * @brief CTR_Library を初期化する
     */
    void SoundSystem::InitSDKLibrary( void )
    {
      nn::Result result = nn::snd::Initialize();
      GFL_ASSERT( result.IsSuccess() ); //@check
    }

    /**
     * @brief サウンドシステムを初期化する
     */
    void SoundSystem::InitSoundSystem( const InitParam& initParam )
    {
      nw::snd::SoundSystem::SoundSystemParam param;
      param.soundThreadPriority = initParam.soundThreadPriority;
      param.taskThreadPriority = initParam.soundDataLoadThreadPriority;

      size_t work_mem_size = nw::snd::SoundSystem::GetRequiredMemSize( param );
      m_pMemoryForSoundSystem = GflHeapAllocMemory( m_pDevHeap, work_mem_size ); 

      nw::snd::SoundSystem::Initialize( 
          param,
          reinterpret_cast<uptr>(m_pMemoryForSoundSystem),
          work_mem_size );

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "SoundSystem\t%d\n", work_mem_size );
#endif

#ifdef GFL_SND_ENABLE_AAC
      // AAC ライブラリの初期化
      if( initParam.aacUse ) 
      {
        size_t aac_dec_buffer_size = nw::snd::aac::GetAacDecBufferSize();
        m_pMemoryForAacLibrary = GflHeapAllocMemoryAlign( m_pDevHeap, aac_dec_buffer_size, 32 );
        nw::snd::aac::Initialize( m_pMemoryForAacLibrary, aac_dec_buffer_size );
        m_isAacLibraryInitialized = true;
#if GFL_SOUNDSYSTEM_HEAP_CHECK
        GFL_PRINT( "AacLibrary\t%d\n", aac_dec_buffer_size );
#endif
      } 
#endif // GFL_SND_ENABLE_AAC
    }

    /**
     * @brief サウンドアーカイブを初期化する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::InitSoundArchive( const InitParam& initParam ) 
    {
      switch( initParam.soundArchiveType ) 
      {
      default:
        GFL_ASSERT(0); //@check
        // fall through

      case SOUND_ARCHIVE_TYPE_ROM:
        this->InitRomSoundArchive( initParam );
        m_pSoundArchive = &m_romSoundArchive;
        break;

      case SOUND_ARCHIVE_TYPE_MEMORY:
        this->InitMemorySoundArchive( initParam );
        m_pSoundArchive = &m_memSoundArchive;
        break;

#if GFL_HIO_ENABLE
      case SOUND_ARCHIVE_TYPE_HIO:
        this->InitHioSoundArchive( initParam );
        m_pSoundArchive = &m_hioSoundArchive;
        break;
#endif
      }
#if GFL_SOUNDSYSTEM_PLAYER_HEAP_SIZE_CHECK
      {
        u32 player_count = m_pSoundArchive->GetPlayerCount();
        for( u32 i=0; i<player_count; i++ ) {
          nw::snd::SoundArchive::PlayerInfo info;
          u32 playerID = m_pSoundArchive->GetPlayerIdFromIndex( i );
          m_pSoundArchive->ReadPlayerInfo( playerID, &info );
          GFL_PRINT("PlayerInfo %d, id=%d, heap=%d max=%d\n", i, playerID, info.playerHeapSize, info.playableSoundMax);
        }
      }
#endif //GFL_SOUNDSYSTEM_PLAYER_HEAP_SIZE_CHECK
    }

    /**
     * @brief ROMサウンドアーカイブを初期化する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::InitRomSoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_ROM ); //@check

      // サウンドアーカイブファイルを開く
      bool open_succeeded = m_romSoundArchive.Open( initParam.bcsarFilePath );
      GFL_ASSERT_STOP( open_succeeded );  //@check

      // サウンドアーカイブのヘッダを読み込む
      size_t header_size = m_romSoundArchive.GetHeaderSize();
      m_pMemoryForSoundArchiveHeader = GflHeapAllocMemory( m_pDevHeap, header_size );
      bool load_succeeded = m_romSoundArchive.LoadHeader( m_pMemoryForSoundArchiveHeader, header_size );
      GFL_ASSERT_STOP( load_succeeded ); //@check

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "RomSoundArchive\t%d\n", header_size );
#endif
    } 

#if GFL_HIO_ENABLE
    /**
     * @brief HIOサウンドアーカイブを初期化する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::InitHioSoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_HIO ); //@check

      // サウンドアーカイブファイルを開く
      bool open_succeeded = m_hioSoundArchive.Open( initParam.bcsarFilePath );
      GFL_ASSERT_STOP( open_succeeded );  //@check

      // サウンドアーカイブのヘッダを読み込む
      size_t header_size = m_hioSoundArchive.GetHeaderSize();
      m_pMemoryForSoundArchiveHeader = GflHeapAllocMemory( m_pDevHeap, header_size );
      bool load_succeeded = m_hioSoundArchive.LoadHeader( m_pMemoryForSoundArchiveHeader, header_size );
      GFL_ASSERT_STOP( load_succeeded ); //@check
    }
#endif

    /**
     * @brief MEMORYサウンドアーカイブを初期化する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::InitMemorySoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_MEMORY ); //@check
      m_memSoundArchive.Initialize( initParam.bcsarData );
    } 

    /**
     * @brief サウンドデータマネージャを初期化する
     */
    void SoundSystem::InitSoundDataManager( void )
    {
      size_t buffer_size = m_soundDataManager.GetRequiredMemSize( m_pSoundArchive );
      m_pMemoryForSoundDataManager = GflHeapAllocMemory( m_pDevHeap, buffer_size );

      bool suceeded = m_soundDataManager.Initialize( 
          m_pSoundArchive, 
          m_pMemoryForSoundDataManager, 
          buffer_size );

      // 排他制御
      m_criticalSectionForSoundDataManagerLoad.Initialize();

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "SoundDataManager\t%d\n", buffer_size );
#endif
    }

    /**
     * @brief サウンドアーカイブプレイヤーを初期化する
     */
    void SoundSystem::InitSoundArchivePlayer( void )
    {
      size_t player_buffer_size = m_soundArchivePlayer.GetRequiredMemSize( m_pSoundArchive );
      size_t stream_buffer_size = m_soundArchivePlayer.GetRequiredStreamBufferSize( m_pSoundArchive );
      size_t cache_buffer_size = m_soundArchivePlayer.GetRequiredStreamCacheSize( m_pSoundArchive,0x8000 ); //24kb/1ch分のキャッシュ

      m_pMemoryForSoundArchivePlayer = GflHeapAllocMemoryAlign( m_pDevHeap, player_buffer_size, 32 );
      m_pMemoryForStreamBuffer = GflHeapAllocMemoryAlign( m_pDevHeap, stream_buffer_size, 32 ); // アプリケーションヒープはNG
      m_pMemoryForCacheBuffer = GflHeapAllocMemoryAlign( m_pDevHeap, cache_buffer_size, 32 ); // アプリケーションヒープはNG


      nw::snd::SoundArchivePlayer::InitializeParam mmx;

      mmx.soundArchive = m_pSoundArchive;
      mmx.soundDataManager = &m_soundDataManager;
      mmx.setupBuffer = m_pMemoryForSoundArchivePlayer;
      mmx.setupBufferSize = player_buffer_size;
      mmx.streamBuffer = m_pMemoryForStreamBuffer;
      mmx.streamBufferSize = stream_buffer_size;
      mmx.streamCacheBuffer = m_pMemoryForCacheBuffer;
      mmx.streamCacheSize = cache_buffer_size;
      mmx.userParamSizePerSound = 0;

      m_soundArchivePlayer.Initialize( mmx );
//          m_pSoundArchive, 
  //        &m_soundDataManager,
    //      m_pMemoryForSoundArchivePlayer, player_buffer_size,
      //    m_pMemoryForStreamBuffer, stream_buffer_size );

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "SoundArchivePlayer\t%d\n", player_buffer_size );
      GFL_PRINT( "StreamBuffer\t%d\n", stream_buffer_size );
#endif
    }

    /**
     * @brief サウンドヒープを初期化する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::InitSoundHeap( const InitParam& initParam )
    { 
      if( 0 < initParam.soundHeapSize ) {
        m_soundHeap = GFL_NEW( m_pAppHeap ) gfl::snd::SoundHeap( m_pAppHeap, m_pDevHeap, initParam.soundHeapSize );
      }

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "SoundHeap\t%d\n", initParam.soundHeapSize );
#endif
    }

    /**
     * @brief サウンドハンドルを作成する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::CreateSoundHandles( const InitParam& initParam )
    { 
      GFL_ASSERT( !m_pSoundHandles ); //@check

      m_soundHandleNum = initParam.soundHandleNum;
      m_pSoundHandles = GFL_NEW(m_pAppHeap) nw::snd::SoundHandle[ initParam.soundHandleNum ];
    }

    /**
     * @brief サウンド3Dマネージャーを初期化する
     */
    void SoundSystem::InitSound3DManager( void )
    {
      size_t setup_size = m_sound3DManager.GetRequiredMemSize( m_pSoundArchive );
      m_pMemoryForSound3DManager = GflHeapAllocMemory( m_pDevHeap, setup_size );
      m_sound3DManager.Initialize( m_pSoundArchive, m_pMemoryForSound3DManager, setup_size );

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "Sound3DManager\t%d\n", setup_size );
#endif
    }

    /**
     * @brief 3D サウンドリスナーを初期化する
     */
    void SoundSystem::InitSound3DListener( void )
    {
      // リスナー行列を設定
      gfl::math::MTX34 listener_matrix;
      const gfl::math::VEC3 pos( 0.0f, 0.0f, 0.0f );   // リスナーの位置
      const gfl::math::VEC3 up( 0.0f, 1.0f, 0.0f );  // リスナーの上方ベクトル
      const gfl::math::VEC3 dir( 0.0f, 0.0f, 1.0f ); // リスナーの向きベクトル
      this->SetListenerPos( pos, up, dir );

      // マネージャに登録
      m_sound3DManager.AddListener( &m_sound3DListener ); 
    }

    /**
     * @brief 3Dアクターと, 3Dアクター用のサウンドハンドルを作成する
     * @param initParam システムの初期化パラメータ
     */
    void SoundSystem::CreateSound3DActorsAndHandles( const InitParam& initParam )
    {
      GFL_ASSERT( !m_pSound3DActors ); //@check
      GFL_ASSERT( !m_pSound3DHandles ); //@check

      m_sound3DActorNum = initParam.sound3DActorNum;
      m_pSound3DActors = GFL_NEW(m_pAppHeap) nw::snd::Sound3DActor[ initParam.sound3DActorNum ];
      m_pSound3DHandles = GFL_NEW(m_pAppHeap) nw::snd::SoundHandle[ initParam.sound3DActorNum ];
    }

    /**
     * @brief 3Dアクターを初期化する
     */
    void SoundSystem::InitSound3DActors( void )
    {
      GFL_ASSERT( m_pSound3DActors ); //@check

      for( u32 i=0; i<m_sound3DActorNum; i++ ) {
        m_pSound3DActors[i].Initialize( m_soundArchivePlayer, m_sound3DManager );
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    SoundSystem::~SoundSystem()
    { 
      this->DeleteSoundHandles();
      this->DeleteSound3DActorsAndHandles();
      m_sound3DManager.Finalize();
      
      GFL_SAFE_DELETE( m_soundHeap );
      m_soundArchivePlayer.Finalize();
      m_soundDataManager.Finalize();
      m_romSoundArchive.Close();
      m_memSoundArchive.Finalize();

      // 排他制御破棄
      m_criticalSectionForSoundDataManagerLoad.Finalize();

      
#if GFL_HIO_ENABLE
      m_hioSoundArchive.Close();
#endif

#ifdef GFL_SND_ENABLE_AAC
      if( m_isAacLibraryInitialized ) {
        nw::snd::aac::Finalize();
        nn::aacdec::Finalize();
      }
#endif
      
      nw::snd::SoundSystem::Finalize();
      nn::snd::Finalize();
      nn::dsp::UnloadComponent();
      nn::dsp::Finalize();

      GflHeapSafeFreeMemory( m_pMemoryForDspComponent );
      GflHeapSafeFreeMemory( m_pMemoryForAacLibrary );
      GflHeapSafeFreeMemory( m_pMemoryForSoundSystem );
      GflHeapSafeFreeMemory( m_pMemoryForSoundArchiveHeader );
      GflHeapSafeFreeMemory( m_pMemoryForSoundDataManager );
      GflHeapSafeFreeMemory( m_pMemoryForSoundArchivePlayer );
      GflHeapSafeFreeMemory( m_pMemoryForStreamBuffer );
      GflHeapSafeFreeMemory( m_pMemoryForCacheBuffer );
      GflHeapSafeFreeMemory( m_pMemoryForSoundHeap );
      GflHeapSafeFreeMemory( m_pMemoryForSound3DManager );
    }

    /**
     * @brief サウンドハンドルを破棄する
     */
    void SoundSystem::DeleteSoundHandles( void )
    {
      GFL_DELETE_ARRAY m_pSoundHandles;
      m_pSoundHandles = NULL;
      m_soundHandleNum = 0;
    }

    /**
     * @brief 3Dアクターと, 3Dアクター用のサウンドハンドルを破棄する
     */
    void SoundSystem::DeleteSound3DActorsAndHandles( void )
    {
      GFL_DELETE_ARRAY m_pSound3DActors;
      GFL_DELETE_ARRAY m_pSound3DHandles;
      m_pSound3DActors = NULL;
      m_pSound3DHandles = NULL;
      m_sound3DActorNum = 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief フレーム処理( 毎フレーム呼んでください )
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Update( void )
    {
      m_soundArchivePlayer.Update();
    }



    //-------------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする
     *
     * @param itemID    ロードするデータのアイテムID
     * @param loadFlag  ロードするデータを論理和で指定( gfl::snd::SoundSystem::LOAD_xxxx )
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::LoadSoundData( u32 itemID, u32 loadFlag )
    {
      GFL_ASSERT_MSG( m_soundHeap, "ERROR!! サウンドヒープが作成されていません\n" ); //@check
      if( m_soundHeap ) {
        return this->LoadSoundData( m_soundHeap, itemID, loadFlag );
      }
      return false;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブ中のデータを指定したサウンドヒープにロードする
     *
     * @param[out] dest      読み込んだデータを配置するサウンドヒープ
     * @param[in]  itemID    ロードするデータのアイテムID
     * @param[in]  loadFlag  ロードするデータを論理和で指定( gfl::snd::SoundSystem::LOAD_xxxx )
     */
    //-----------------------------------------------------------------------
    bool SoundSystem::LoadSoundData( SoundHeap* dest, u32 itemID, u32 loadFlag )
    {
      bool result = false;
      m_criticalSectionForSoundDataManagerLoad.Enter();
      {
        nw::snd::SoundHeap* nw_sound_heap = dest->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          result = m_soundDataManager.LoadData( itemID, nw_sound_heap, loadFlag );
        }

        if( !result ) {
          GFL_PRINT( "WARNING!! サウンドデータ(0x%x)のロードに失敗しました\n", itemID );
        }
#if GFL_SOUNDSYSTEM_HEAP_CHECK
        else {
          GFL_PRINT( "サウンドデータ(0x%x)をロードしました\n", itemID );
          GFL_PRINT( "SoundHeapSize = 0x%x / 0x%x\n", nw_sound_heap->GetFreeSize(), nw_sound_heap->GetSize() );
        }
#endif
      }
      m_criticalSectionForSoundDataManagerLoad.Leave();
      return result;
    }

    
    //----------------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする 非同期版
     *
     * @param thread_heap スレッドヒープ
     * @param priority    スレッド優先順位
     * @param itemID      ロードするデータのアイテムID
     * @param loadFlag    ロードするデータを論理和で指定( gfl::snd::SoundSystem::LOAD_xxxx )
     *
     *	@retval true    読み開始
     *	@retval false   読み開始失敗
     */
    //-----------------------------------------------------------------------------
    bool SoundSystem::LoadSoundDataAsync( gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag )
    {
      GFL_ASSERT_MSG( m_soundHeap, "ERROR!! サウンドヒープが作成されていません\n" ); //@check
      if( m_soundHeap ) {
        return this->LoadSoundDataAsync( m_soundHeap, thread_heap, priority, itemID, loadFlag );
      }
      return false;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブ中のデータをシステム内部ヒープにロードする 非同期版
     *
     * @param[out] dest      読み込んだデータを配置するサウンドヒープ
     * @param[in]  thread_heap スレッドヒープ
     * @param[in]  priority    スレッド優先順位
     * @param[in]  itemID    ロードするデータのアイテムID
     * @param[in]  loadFlag  ロードするデータを論理和で指定( gfl::snd::SoundSystem::LOAD_xxxx )
     */
    //-----------------------------------------------------------------------
    bool SoundSystem::LoadSoundDataAsync( SoundHeap* dest, gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag )
    {
      // まだ読み込み中
      if( m_pSoundDataManagerThread ){
        return false;
      }
      {
        // スレッドを起動
        SoundDataManagerThread::Description desc;
        desc.m_pSoundSystem    = this;
        desc.m_pDest    = dest;
        desc.m_pResult  = &m_SoundDataManagerThreadResult;
        desc.m_itemID   = itemID;
        desc.m_loadFlag = loadFlag;

        m_pSoundDataManagerThread = GFL_NEW_LOW( thread_heap ) SoundDataManagerThread( thread_heap->GetLowerHandle(), desc );

        // スレッド開始
        m_pSoundDataManagerThread->Start( priority );
      }
      return true;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドアーカイブ中のデータを指定したサウンドヒープにロードする　非同期　待ち
     *
     *	@retval true  ロード完了
     *	@retval false ロード中
     */
    //-----------------------------------------------------------------------------
    bool SoundSystem::WaitLoadSoundDataAsync(void)
    {
      bool result = true;
      if( m_pSoundDataManagerThread ) {
        if( m_pSoundDataManagerThread->IsAlive() ){
          return false;
        }
        else{
          // 終了
          GFL_DELETE m_pSoundDataManagerThread;
          m_pSoundDataManagerThread = NULL;
        }
      }
      return result;
    }


    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドアーカイブ　非同期読み込みの結果を取得
     *
     *	@retval true  成功
     *	@retval false 失敗
     */
    //-----------------------------------------------------------------------------
    bool SoundSystem::GetLoadSoundDataAsyncResult(void) const
    {
      return m_SoundDataManagerThreadResult;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドデータマネージャへサウンドデータを設定する。
     *
     *	@param    fileID    セットするデータのファイルIDです。
     *	@param    address   アドレス
     */
    //-----------------------------------------------------------------------------
    void SoundSystem::SetSoundFileAddress( u32 fileID, const void * address )
    {
      nw::snd::SoundSystem::LockSoundThread();
      m_criticalSectionForSoundDataManagerLoad.Enter();
      m_soundDataManager.SetFileAddress( fileID, address );
      m_criticalSectionForSoundDataManagerLoad.Leave();
      nw::snd::SoundSystem::UnlockSoundThread();
    } 

    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドデータマネージャへサウンドグループファイルを登録する。
     *
     *	@param    groupFile グループファイル
     *	@param    size      サイズ
     */
    //-----------------------------------------------------------------------------
    void SoundSystem::SetSoundFileAddressInGroupFile( const void * groupFile, size_t size )
    {
      nw::snd::SoundSystem::LockSoundThread();
      m_criticalSectionForSoundDataManagerLoad.Enter();
      m_soundDataManager.SetFileAddressInGroupFile( groupFile, size );
      m_criticalSectionForSoundDataManagerLoad.Leave();
      nw::snd::SoundSystem::UnlockSoundThread();
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  サウンドデータマネージャからサウンドグループファイルを登録解除する
     *
     *	@param    groupFile グループファイル
     *	@param    size      サイズ
     */
    //-----------------------------------------------------------------------------
    void SoundSystem::ClearSoundFileAddressInGroupFile( const void * groupFile, size_t size )
    {
      nw::snd::SoundSystem::LockSoundThread();
      m_criticalSectionForSoundDataManagerLoad.Enter();
      m_soundDataManager.ClearFileAddressInGroupFile( groupFile, size );
      m_criticalSectionForSoundDataManagerLoad.Leave();
      nw::snd::SoundSystem::UnlockSoundThread();
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 指定したプレイヤーで再生中のサウンドの個数を取得する
     * @param playerID  プレイヤーのアイテムID
     */
    //-------------------------------------------------------------------------
    u32 SoundSystem::GetPlayingSoundCount( u32 playerID ) const
    {
      nw::snd::SoundArchivePlayer* sound_archive_player = const_cast<nw::snd::SoundArchivePlayer*>( &m_soundArchivePlayer );
      const nw::snd::SoundPlayer& player = sound_archive_player->GetSoundPlayer( playerID );
      return player.GetPlayingSoundCount();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したプレイヤーの音量を取得する
     * @param playerID  プレイヤーのアイテムID
     */
    //-------------------------------------------------------------------------
    f32 SoundSystem::GetPlayerVolume( u32 playerID ) const
    {
      nw::snd::SoundArchivePlayer* sound_archive_player = const_cast<nw::snd::SoundArchivePlayer*>( &m_soundArchivePlayer );
      const nw::snd::SoundPlayer& player = sound_archive_player->GetSoundPlayer( playerID );
      return player.GetVolume();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したプレイヤーの音量を変更する
     * @param playerID  プレイヤーのアイテムID
     * @param volume    設定する音量[0.0, 2.0]
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetPlayerVolume( u32 playerID, f32 volume )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.SetVolume( volume );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したプレイヤーで再生中の全てのサウンドを停止する
     * @param playerID      プレイヤーのアイテムID
     * @param fadeOutFrame  フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::StopPlayerSound( u32 playerID, u32 fadeOutFrame )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.StopAllSound( fadeOutFrame );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したプレイヤーで再生中の全てのサウンドを一時停止・再開する
     * @param playerID   プレイヤーのアイテムID
     * @param pauseOn    true:一時停止, false:再開
     * @param fade_frame フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::PausePlayerSound( u32 playerID, bool pause_on, u32 fade_frame )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.PauseAllSound( pause_on, fade_frame );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 再生する
     *
     * @param handleIdx    再生後の制御に使用するハンドルのインデックス
     * @param itemID       再生するサウンドのアイテムNo.
     * @param fadeInFrames フェードインにかけるフレーム数
     *
     * @return 再生が成功した場合 true
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::Play( u8 handleIdx, u32 itemID, s32 fadeInFrames, const nw::snd::SoundStartable::StartInfo* startInfo )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 
      nw::snd::SoundStartable::StartResult result;
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ];
      result = m_soundArchivePlayer.StartSound( handle, itemID, startInfo );
      handle->FadeIn( fadeInFrames );
      return result.IsSuccess();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 再生の準備を開始する
     *
     * @param handleIdx    再生後の制御に使用するハンドルのインデックス
     * @param itemID       再生するサウンドのアイテムNo.
     *
     * @retval true   成功
     * @retval false  失敗
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::PrepareSound( u8 handleIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 
      nw::snd::SoundStartable::StartResult result;
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ];
      result = m_soundArchivePlayer.PrepareSound( handle, itemID, startInfo );
      return result.IsSuccess();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief PrepareSound()で開始した再生準備が完了したかどうかを調べる
     *
     * @param handleIdx  再生するサウンドハンドルのインデックス
     *
     * @retval true   再生準備が完了
     * @retval false  再生準備が未完了
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::IsPrepared( u8 handleIdx )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) {  // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.IsPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {  // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        return stream_handle.IsPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_WAVE ) {  // ウェーブサウンド
        nw::snd::WaveSoundHandle wave_handle( handle );
        return wave_handle.IsPrepared();
      }
      return false;
    }

    /**
     * @brief サウンドハンドルに関連付けられているサウンドの種類を調べる
     *
     * @param handle 対象のサウンドハンドル
     */
    nw::snd::SoundArchive::SoundType SoundSystem::GetAttachedSoundType( const nw::snd::SoundHandle* handle ) const
    {
      if( !handle->IsAttachedSound() ) {  // サウンドが関連付けられていない
        return nw::snd::SoundArchive::SOUND_TYPE_INVALID;
      } 
      u32 soundID = handle->GetId();
      return this->GetSoundTypeOfItem( soundID );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief PrepareSound()で準備したサウンドを再生する
     *
     * @param handleIdx     再生するサウンドハンドルのインデックス
     * @param fadeInFrames  フェードインにかけるフレーム数
     *
     * @note 再生準備が完了していない場合, 準備完了を待ってから再生を開始します.
     */
    //-------------------------------------------------------------------------
    void SoundSystem::PlayPrepared( u8 handleIdx, s32 fadeInFrames )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) {  // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.StartPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {  // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.StartPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_WAVE ) {  // ウェーブサウンド
        nw::snd::WaveSoundHandle wave_handle( handle );
        wave_handle.StartPrepared();
      }

      handle->FadeIn( fadeInFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 停止する
     *
     * @param handleIdx     制御を行うハンドルのインデックス
     * @param fadeOutFrames フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Stop( u8 handleIdx, s32 fadeOutFrames )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->Stop( fadeOutFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのサウンドを停止する
     * @param fadeOutFrames フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::StopAll( s32 fadeOutFrames )
    {
      u32 player_count = m_pSoundArchive->GetPlayerCount();
      for( u32 i=0; i<player_count; i++ ) {
        u32 playerID = m_pSoundArchive->GetPlayerIdFromIndex( i );
        this->StopPlayerSound( playerID, fadeOutFrames );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 一時停止 or 再開 する
     *
     * @param handleIdx  制御を行うハンドルのインデックス
     * @param pauseOn    true:一時停止, false:再開
     * @param fadeFrames フェードイン, フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Pause( u8 handleIdx, bool pauseOn, s32 fadeFrames )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->Pause( pauseOn, fadeFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 一時停止中かどうかを調べる
     *
     * @param handleIdx 制御を行うハンドルのインデックス
     *
     * @retval true   一時停止中
     * @retval false  一時停止中でない
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::IsPause( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return handle->IsPause();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 再生, 停止時における
     *        フェードイン, フェードアウト完了までの残りフレーム数を取得する
     *
     * @param u8 handleIdx 制御を行うハンドルのインデックス
     */
    //-------------------------------------------------------------------------
    s32 SoundSystem::GetRemainingFadeFrames( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return 0;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return handle->GetRemainingFadeFrames();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 一時停止時における
     *        フェードイン, フェードアウト完了までの残りフレーム数を取得する
     *
     * @param u8 handleIdx 制御を行うハンドルのインデックス
     */
    //-------------------------------------------------------------------------
    s32 SoundSystem::GetRemainingPauseFadeFrames( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return 0;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return handle->GetRemainingPauseFadeFrames();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ハンドルにサウンドが関連付けられているかどうかを調べる
     *
     * @param handleIdx  調べるハンドルのインデックス
     *
     * @retval true    関連付けられている
     * @retval false   関連付けられていない
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::IsAttachedSound( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      }
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return handle->IsAttachedSound();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ハンドル関連付けられているサウンドのIDを取得する
     *
     * @param handleIdx  調べるハンドルのインデックス
     *
     * @return 指定したハンドルに関連付けられているサウンドID
     * @retval INVALID_SOUND_ID  サウンドが関連付けられていない場合
     */
    //-------------------------------------------------------------------------
    u32 SoundSystem::GetAttachedSound( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return 0;
      }
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return handle->GetId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ストリームサウンドの再生位置を取得する
     *
     * @param handleIdx  ストリームサウンド再生中のハンドルのインデックス
     *
     * @return ストリームデータの再生位置( サンプル数 )
     * @retval 負数  ハンドルが無効の場合
     */
    //-------------------------------------------------------------------------
    s32 SoundSystem::GetStreamSamplePosition( u8 handleIdx ) const
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return -1;
      }
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        return stream_handle.GetPlaySamplePosition();
      }
      return -1;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 音量を変更する
     *
     * @param handleIdx 制御を行うハンドルのインデックス
     * @param volume    設定する音量の倍率 ( 0.0 ～ )
     * @param frames    音量の変化にかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetVolume( u8 handleIdx, float volume, s32 frames )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->SetVolume( volume, frames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとの音量を変更する
     *
     * @param u8  handleIdx    制御を行うハンドルのインデックス
     * @param u32 trackBitFlag 設定するトラックのビットフラグ
     * @param f32 volume       音量の倍率 ( デフォルトは 1.0 )
     * @param s32 frame        音量変化にかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackVolume( u8 handleIdx, u32 trackBitFlag, f32 volume, s32 frame )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackVolume( trackBitFlag, volume );
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackVolume( trackBitFlag, volume, frame );
      }
    }




    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::SetPitch( u8 handleIdx, float pitch )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->SetPitch( pitch );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 12平均律に即した音程に変更する
     *
     * @param handleIdx  制御を行うハンドルのインデックス
     * @param level      12平均律のレベル
     *
     * @note levelの指定例
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2オクターブ高い音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1オクターブ高い音程
     * EQUAL_TEMPERAMENT_LEVEL_UNISON             通常の音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1オクターブ低い音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2オクターブ低い音程
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetPitchByEqualTemperament( u8 handleIdx, s8 level )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      f32 pitch = this->CalcPitchByEqualTemperament( level );
      this->SetPitch( handleIdx, pitch );
    }

    /**
     * @brief 12平均律に即した音程の周波数比率を計算する
     *
     * @param level 12平均律のレベル
     *
     * @note levelの指定例
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2オクターブ高い音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1オクターブ高い音程
     * EQUAL_TEMPERAMENT_LEVEL_UNISON             通常の音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1オクターブ低い音程
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2オクターブ低い音程
     */
    f32 SoundSystem::CalcPitchByEqualTemperament( s8 level ) const
    {
      const f32* frequency_ratio = ( level < 0 ) ? m_lowFrequencyRatioForEqualTemperament : m_highFrequencyRatioForEqualTemperament;
      u8 abs_level = gfl::math::Abs( level );
      u8 octave_count = abs_level / ONE_OCTAVE_STEP_COUNT;
      f32 pitch = frequency_ratio[ abs_level % ONE_OCTAVE_STEP_COUNT ];
      for( u32 i=0; i<octave_count; i++ ) {
        pitch *= frequency_ratio[ ONE_OCTAVE_STEP_COUNT ];
      }
      return pitch;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとの音程を変更する
     *
     * @param u8  handleIdx    制御を行うハンドルのインデックス
     * @param u32 trackBitFlag 設定するトラックのビットフラグ
     * @param f32 pitch        周波数比率
     *
     * ※周波数比率 
     *   0.5 = 1 オクターブ 低
     *   1.0 = 等倍 ( デフォルト値 )
     *   2.0 = 1 オクターブ 高
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackPitch( u8 handleIdx, u32 trackBitFlag, f32 pitch )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackPitch( trackBitFlag, pitch );
      }
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackPitchByEqualTemperament( u8 handleIdx, u32 trackBitFlag, s8 level )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      f32 pitch = this->CalcPitchByEqualTemperament( level );
      this->SetTrackPitch( handleIdx, trackBitFlag, pitch );
    }




    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::SetPan( u8 handleIdx, float pan )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->SetPan( pan );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとのパン ( 左右の定位 ) を変更する
     *
     * @param u8    handleIdx    制御を行うハンドルのインデックス
     * @param u32   trackBitFlag 設定するトラックのビットフラグ
     * @param float pan          パンの値
     *
     * ※パンの値
     *   -1.0 = 左
     *    0.0 = 中
     *    1.0 = 右
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SoundSystem::SetTrackPan( u8 handleIdx, u32 trackBitFlag, f32 pan )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackPan( trackBitFlag, pan );
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackPan( trackBitFlag, pan );
      }
    }




    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::SetSurroundPan( u8 handleIdx, float pan )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      handle->SetSurroundPan( pan );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トラックのサラウンドパン ( 前後の定位 ) を変更する
     *
     * @param u8    handleIdx    制御を行うハンドルのインデックス
     * @param u32   trackBitFlag 設定するトラックのビットフラグ
     * @param float pan          パンの値
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackSurroundPan( u8 handleIdx, u32 trackBitFlag, f32 pan )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // ストリームサウンド
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackSurroundPan( trackBitFlag, pan );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief テンポを変更する
     *
     * @param handleIdx    制御を行うハンドルのインデックス
     * @param tempoRatio  テンポの倍率
     *
     * @attention シーケンスサウンドに対してのみ有効です。
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTempoRatio( u8 handleIdx, f32 tempoRatio )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTempoRatio( tempoRatio );
      }
    }

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
    void SoundSystem::SetTranspose( u8 handleIdx, s8 transposeValue, u32 transposeTrackBit )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
			if(transposeValue < nw::snd::SequenceSoundHandle::TRANSPOSE_MIN)
			{ transposeValue = nw::snd::SequenceSoundHandle::TRANSPOSE_MIN; }
			if(transposeValue > nw::snd::SequenceSoundHandle::TRANSPOSE_MAX)
			{ transposeValue = nw::snd::SequenceSoundHandle::TRANSPOSE_MAX; }

      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackTranspose( transposeTrackBit, transposeValue );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとのミュート状態を変更する
     *
     * @param handleIdx    制御を行うハンドルのインデックス
     * @param trackBitFlag 設定するトラックのビットフラグ
     * @param mute         ミュート状態
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackMute( u8 handleIdx, u32 trackBitFlag, SeqMute mute )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackMute( trackBitFlag, mute );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとのサイレント状態を変更する
     *
     * @param handleIdx    制御を行うハンドルのインデックス
     * @param trackBitFlag 設定するトラックのビットフラグ
     * @param silenceFlag  サイレント状態にする場合 true, 解除する場合 false
     * @param fadeTimes    フェード時間 (ミリ秒)
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackSilence( u8 handleIdx, u32 trackBitFlag, bool silenceFlag, s32 fadeTimes )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackSilence( trackBitFlag, silenceFlag, fadeTimes );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief トラックごとのバンクを変更する
     *
     * @param u8  handleIdx    制御を行うハンドルのインデックス
     * @param u32 trackBitFlag 設定するトラックのビットフラグ
     * @param s32 bankIdx      設定するバンクのインデックス ( 0 ～ 3 )
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetTrackBankIndex( u8 handleIdx, u32 trackBitFlag, s32 bankIdx )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return;
      } 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackBankIndex( trackBitFlag, bankIdx );
      }
    }




    //-------------------------------------------------------------------------
    /*
     * @brief サウンドタイプ取得
     *
     * @param[in] u32 itemID  調べるサウンドのID
     *
     * @retval  nw::snd::SoundArchive::SoundType
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchive::SoundType SoundSystem::GetSoundTypeOfItem( u32 itemID ) const
    {
      return m_pSoundArchive->GetSoundType( itemID );
    }
    

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
    bool SoundSystem::AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect )
    {
      return nw::snd::SoundSystem::AppendEffect( busID , effect );
    }
    

    //-----------------------------------------------------------------------
    /**
     * @brief エフェクトをAUXバスから削除します。
     *
     * @param busID バスID（A,Bあります）
     * @param フェード時間（ミリ秒）
     */
    //-----------------------------------------------------------------------
    void SoundSystem::RemoveEffect( nw::snd::AuxBus busID, int fadeTimes )
    {
      nw::snd::SoundSystem::ClearEffect( busID , fadeTimes );
    }
    
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
    bool SoundSystem::AppendReverbEffect( nw::snd::AuxBus busID, gfl::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param )
    {
      if( m_pMemoryForSoundReverb != NULL )
      {
        GFL_ASSERT_MSG(0,"リバーブを２重にかけることはできません\n"); //@check
        return false;
      }

      m_fxReverb.SetParam( param );
      
      // ディレイの初期化（パラメータ設定後のみ使用可能）
      size_t size = m_fxReverb.GetRequiredMemSize();
      
      // エフェクトの仕様する領域を確保、割り当て
      m_pMemoryForSoundReverb = GflHeapAllocMemory( reverb_buffer_heap, size );
      m_fxReverb.AssignWorkBuffer( reinterpret_cast<uptr>(m_pMemoryForSoundReverb), size );
      return nw::snd::SoundSystem::AppendEffect( busID , &m_fxReverb );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief エフェクトをAUXバスから削除します。
     *        RemoveEffectとの違いは、存在していればリバーブ用バッファを解放することです
     *
     * @param busID バスID（A,Bあります）
     * @param フェード時間（ミリ秒）
     */
    //-----------------------------------------------------------------------
    void SoundSystem::RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes )
    {
      this->RemoveEffect( busID, fadeTimes );
      GflHeapSafeFreeMemory(m_pMemoryForSoundReverb);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープの状態を保存する
     *
     * @return 保存後の階層レベル( 保存に失敗した場合は -1 )
     */
    //-------------------------------------------------------------------------
    s32 SoundSystem::SaveHeapState( void )
    { 
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          return nw_sound_heap->SaveState();
        }
      }
      return -1;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 保存したサウンドヒープの状態を復元する
     *
     * @param level 復元する階層レベル( 現在の階層レベル以下の値のみ有効 )
     */
    //-------------------------------------------------------------------------
    void SoundSystem::LoadHeapState( s32 level )
    { 
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          nw_sound_heap->LoadState( level );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープをクリアする
     */
    //-------------------------------------------------------------------------
    void SoundSystem::ClearHeap( void )
    {
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          nw_sound_heap->Clear();
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のサウンドヒープの階層レベルを取得する
     */
    //-------------------------------------------------------------------------
    s32 SoundSystem::GetCurrentHeapLevel( void ) const
    {
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          return nw_sound_heap->GetCurrentLevel();
        }
      }
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープのサイズを取得する
     */
    //-------------------------------------------------------------------------
    size_t SoundSystem::GetHeapSize( void ) const
    {
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          return nw_sound_heap->GetSize();
        }
      }
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープの空き容量を取得する
     */
    //-------------------------------------------------------------------------
    size_t SoundSystem::GetHeapFreeSize( void ) const
    {
      GFL_ASSERT( m_soundHeap ); //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          return nw_sound_heap->GetFreeSize();
        }
      }
      return 0;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief マスターボリュームを 設定 する
     *
     * @param volume    音量の倍率( 0.0 ～ )
     * @param fadeTimes 音量変化にかける時間[ミリ秒]
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetMasterVolume( float volume, s32 fadeTimes )
    {
      nw::snd::SoundSystem::SetMasterVolume( volume, fadeTimes );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief マスターボリュームを 取得 する
     *
     * @return マスターボリュームの倍率( 0.0 ～ )
     */
    //-------------------------------------------------------------------------
    float SoundSystem::GetMasterVolume( void ) const
    {
      return nw::snd::SoundSystem::GetMasterVolume();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief サウンド出力モードを設定する
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetOutputMode( SoundSystem::OutputMode mode )
    {
      nw::snd::SoundSystem::SetOutputMode( static_cast<nw::snd::OutputMode>(mode) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンド出力モードを取得する
     */
    //-------------------------------------------------------------------------
    SoundSystem::OutputMode SoundSystem::GetOutputMode( void ) const
    {
      return static_cast<OutputMode>(nw::snd::SoundSystem::GetOutputMode());
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 3D サウンドのパラメータを設定する
     *
     * @param param 設定する3Dサウンドパラメータ
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DParam( const Sound3DParam& param )
    {
      m_sound3DManager.SetMaxPriorityReduction( param.maxPriorityReduction );
      m_sound3DManager.SetPanRange( param.panRange );
      m_sound3DManager.SetSonicVelocity( param.sonicVelocity );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在設定されている 3D サウンドのパラメータを取得する
     *
     * @param[out] pOutParam パラメータの格納先
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Get3DParam( Sound3DParam* pOutParam ) const
    {
      pOutParam->maxPriorityReduction = m_sound3DManager.GetMaxPriorityReduction();
      pOutParam->panRange = m_sound3DManager.GetPanRange();
      pOutParam->sonicVelocity = m_sound3DManager.GetSonicVelocity();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リスナーのパラメータを設定する
     *
     * @param param 設定するリスナーパラメータ
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerParam( const ListenerParam& param )
    {
      m_sound3DListener.SetInteriorSize( param.interiorSize );
      m_sound3DListener.SetMaxVolumeDistance( param.maxVolumeDistance );
      m_sound3DListener.SetUnitDistance( param.unitDistance );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リスナーのパラメータを取得する
     *
     * @param[out] pOutParam パラメータの格納先
     */
    //-------------------------------------------------------------------------
    void SoundSystem::GetListenerParam( ListenerParam* pOutParam ) const
    {
      pOutParam->interiorSize = m_sound3DListener.GetInteriorSize();
      pOutParam->maxVolumeDistance = m_sound3DListener.GetMaxVolumeDistance();
      pOutParam->unitDistance = m_sound3DListener.GetUnitDistance();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リスナー行列を設定する
     *
     * @param matrix 設定するリスナー行列
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerMatrix( const gfl::math::MTX34& matrix )
    {
      m_sound3DListener.SetMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief リスナーの座標を変更する
     *
     * @param pos リスナーの座標
     * @param up  リスナーの上方ベクトル
     * @param dir リスナーの向きベクトル
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerPos( 
        const gfl::math::VEC3& pos,
        const gfl::math::VEC3& up,
        const gfl::math::VEC3& dir )
    {
      gfl::math::MTX34 matrix;
      gfl::math::VEC3 target = pos + dir;
      gfl::math::MTX34LookAt( &matrix, &pos, &up, &target ); 
      this->SetListenerMatrix( matrix );
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerVelocity( const gfl::math::VEC3& velocity )
    {
      m_sound3DListener.SetVelocity( velocity );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 3Dサウンドを再生する
     *
     * @param actorIdx    再生後の制御を行う3Dアクターのインデックス
     * @param itemID      再生するサウンドのアイテムID
     * @param startInfo   詳細な再生パラメータ
     *
     * @return 再生が成功した場合 true
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::Play3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ];
      nw::snd::SoundHandle* handle = &m_pSound3DHandles[ actorIdx ];
      nw::snd::SoundStartable::StartResult result;
      actor->Finalize();
      actor->Initialize( m_soundArchivePlayer, m_sound3DManager );
      result = actor->StartSound( handle, itemID, startInfo );
      return result.IsSuccess();
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
    bool SoundSystem::Prepare3DActor( u8 actorIdx, u32 itemID, const nw::snd::SoundStartable::StartInfo* startInfo )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ];
      nw::snd::SoundHandle* handle = &m_pSound3DHandles[ actorIdx ];
      nw::snd::SoundStartable::StartResult result;
      actor->Finalize();
      actor->Initialize( m_soundArchivePlayer, m_sound3DManager );
      result = actor->PrepareSound( handle, itemID, startInfo );
      GFL_PRINT("3DActorPrepare id=%d attach=%d\n", actorIdx, handle->IsAttachedSound() );
      return result.IsSuccess();
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
    bool SoundSystem::IsPrepared3DActor( u8 actorIdx )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      }
      nw::snd::SoundHandle* handle = &m_pSound3DHandles[ actorIdx ];
      GFL_PRINT("3DActorIsPrepare id=%d attach=%d\n", actorIdx, handle->IsAttachedSound() );
      return handle->IsPrepared();
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
    void SoundSystem::PlayPrepared3DActor( u8 actorIdx )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::SoundHandle* handle = &m_pSound3DHandles[ actorIdx ];
      GFL_PRINT("3DActorPlay id=%d attach=%d\n", actorIdx, handle->IsAttachedSound() );
      handle->StartPrepared();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターで再生中の全てのサウンドを停止する
     *
     * @param actorIdx   制御を行う3Dアクターのインデックス
     * @param fadeFrames フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Stop3DActor( u8 actorIdx, s32 fadeFrames )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->StopAllSound( fadeFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターで再生中の全てのサウンドを一時停止・再開する
     *
     * @param actorIdx   制御を行う3Dアクターのインデックス
     * @param pauseOn    true なら一時停止, false なら再開
     * @param fadeFrames フェードイン・フェードアウトにかけるフレーム数
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Pause3DActor( u8 actorIdx, bool pauseOn, s32 fadeFrames )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->PauseAllSound( pauseOn, fadeFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの音量を変更する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     * @param volume   設定する音量の倍率( 0.0 ～ 1.0 )
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DActorVolume( u8 actorIdx, f32 volume )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->SetVolume( volume );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの音量を取得する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     *
     * @return 指定した3Dアクターに設定されている音量の倍率
     */
    //-------------------------------------------------------------------------
    f32 SoundSystem::Get3DActorVolume( u8 actorIdx ) const
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return 0.0f;
      }
      const nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      return actor->GetVolume();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの音程を変更する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     * @param pitch    周波数比率
     *
     * @note 周波数比率 
     *   0.5 = 1 オクターブ 低
     *   1.0 = 等倍 ( デフォルト値 )
     *   2.0 = 1 オクターブ 高
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DActorPitch( u8 actorIdx, f32 pitch )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->SetPitch( pitch );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの音程を取得する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     *
     * @return 指定した3Dアクターに設定されている周波数比率
     *
     * @note 周波数比率 
     *   0.5 = 1 オクターブ 低
     *   1.0 = 等倍( デフォルト値 )
     *   2.0 = 1 オクターブ 高
     */
    //-------------------------------------------------------------------------
    f32 SoundSystem::Get3DActorPitch( u8 actorIdx ) const
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return 0.0f;
      }
      const nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      return actor->GetPitch();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターのパン( 左右の定位 )を変更する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     * @param pan      パンの値
     *
     * @note パンの値
     *   -1.0 = 左
     *    0.0 = 中
     *    1.0 = 右
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DActorPan( u8 actorIdx, f32 pan )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->SetPan( pan );
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    f32 SoundSystem::Get3DActorPan( u8 actorIdx ) const
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return 0.0f;
      }
      const nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      return actor->GetPan();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの座標を変更する
     *
     * @param actorIdx 制御を行う3Dアクターのインデックス
     * @param pos      設定する座標
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DActorPos( u8 actorIdx, const gfl::math::VEC3& pos )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->SetPosition( pos );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 3Dアクターの座標を取得する
     *
     * @param[in]  actorIdx 制御を行う3Dアクターのインデックス
     * @param[out] pos      取得した座標の格納先
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Get3DActorPos( u8 actorIdx, gfl::math::VEC3* pos ) const
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@fix
        *pos = gfl::math::VEC3::Zero();
        return;
      }
      const nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      const nw::math::VEC3& actor_pos = actor->GetPosition();
      pos->x = actor_pos.x;
      pos->y = actor_pos.y;
      pos->z = actor_pos.z;
    }

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void SoundSystem::Set3DActorVelocity( u8 actorIdx, const gfl::math::VEC3& velocity )
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      actor->SetVelocity( velocity );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 3Dアクターで再生中のサウンド数を取得する
     * @param actorIdx        制御を行う3Dアクターのインデックス
     * @param actorPlayerIdx  対象のアクタープレイヤーのインデックス
     */
    //-----------------------------------------------------------------------
    u32 SoundSystem::Get3DActorPlayingSoundCount( u8 actorIdx, u8 actorPlayerIdx ) const
    {
      if( m_sound3DActorNum <= actorIdx ) {
        GFL_ASSERT(0); //@check
        return 0;
      }
      nw::snd::Sound3DActor* actor = &m_pSound3DActors[ actorIdx ]; 
      return actor->GetPlayingSoundCount( actorPlayerIdx );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief 3Dアクターが全アクタープレイヤーで再生中のサウンド数を取得する
     * @param actorIdx  制御を行う3Dアクターのインデックス
     */
    //-----------------------------------------------------------------------
    u32 SoundSystem::Get3DActorPlayingSoundCount( u8 actorIdx ) const
    {
      u32 sum = 0;
      for( u32 i=0; i<4; i++ ) {
        sum += this->Get3DActorPlayingSoundCount( actorIdx, i );
      }
#if 0
      if(m_pSound3DHandles[actorIdx].IsAttachedSound()){
        GFL_PRINT("3DActorPlayingSoundCt id=%d ct=%d\n", actorIdx, sum);
      }
#endif
      return sum;
    }



    //-------------------------------------------------------------------------
    /**
     * @brief サウンドに設定されているユーザパラメータを取得
     *
     * @param itemID  取得するアイテムID
     */
    //-------------------------------------------------------------------------
    u32 SoundSystem::GetSoundUserParam( u32 itemID )
    { 
      return m_pSoundArchive->GetSoundUserParam( itemID );
    }

    //-------------------------------------------------------------------------
    /*
     * @brief シーケンス変数を取得
     *
     * @param[in]   handleIdx 値を取得するサウンドハンドルインデックス
     * @param[in]   varNo     変数ナンバー(0~15)
     * @param[out]  var       取得した値を格納するワークへのポインタ
     * 
     * @retval  true:取得成功　false:取得失敗
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::ReadVariable( u8 handleIdx, int varNo, s16* var )
    { 
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 

      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.ReadVariable( varNo, var );
      }
      else
      { 
        //シーケンスサウンド以外ではこの関数を呼べない
        GFL_ASSERT( 0 );  //@check 不正な種類のサウンド
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief シーケンス変数に格納
     *
     * @param[in] handleIdx 値を格納するサウンドハンドルインデックス
     * @param[in] varNo     変数ナンバー(0~15)
     * @param[in] var       格納する値
     *
     * @retval  true:格納成功　false:格納失敗
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::WriteVariable( u8 handleIdx, int varNo, s16 var )
    { 
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0); //@check
        return false;
      } 

      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.WriteVariable( varNo, var );
      }
      else
      { 
        // シーケンスサウンド以外ではこの関数を呼べない
        GFL_ASSERT( 0 );  //@check 不正な種類のサウンド
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief シーケンスグローバル変数を取得
     *
     * @param[in]   handleIdx 値を取得するサウンドハンドルインデックス
     * @param[in]   varNo     変数ナンバー(0~15)
     * @param[out]  var       取得した値を格納するワークへのポインタ
     * 
     * @retval  true:取得成功　false:取得失敗
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::ReadGlobalVariable( u8 handleIdx, int varNo, s16* var )
    { 
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0);  //@check
        return false;
      } 

      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {
        // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.ReadGlobalVariable( varNo, var );
      }
      else
      { 
        // シーケンスサウンド以外ではこの関数を呼べない
        GFL_ASSERT( 0 );  //@check 不正な種類のサウンド
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief シーケンスグローバル変数に格納
     *
     * @param[in] handleIdx 値を格納するサウンドハンドルインデックス
     * @param[in] varNo     変数ナンバー(0~15)
     * @param[in] var       格納する値
     *
     * @retval  true:格納成功　false:格納失敗
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::WriteGlobalVariable( u8 handleIdx, int varNo, s16 var )
    { 
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0);  //@check
        return false;
      } 

      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      nw::snd::SoundArchive::SoundType sound_type = this->GetAttachedSoundType( handle );
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check 不正な種類のサウンド

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // シーケンスサウンド
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.WriteGlobalVariable( varNo, var );
      }
      else
      { 
        // シーケンスサウンド以外ではこの関数を呼べない
        GFL_ASSERT( 0 );  //@check 不正な種類のサウンド
        return false;
      }
    }




    //-------------------------------------------------------------------------
    /*
     * @brief サウンドタイプ取得
     *
     * @param[in] handleIdx 取得するサウンドハンドルインデックス
     *
     * @retval  nw::snd::SoundArchive::SoundType
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchive::SoundType SoundSystem::GetSoundType( u8 handleIdx ) const
    { 
      nw::snd::SoundHandle* handle = &m_pSoundHandles[ handleIdx ]; 
      return this->GetAttachedSoundType( handle );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブを取得する
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchive* SoundSystem::GetSoundArchive( void )
    {
      return m_pSoundArchive;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドアーカイブプレイヤーを取得する
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchivePlayer* SoundSystem::GetSoundArchivePlayer( void )
    { 
      return &m_soundArchivePlayer;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドハンドルを取得する
     * @param handleIdx  取得するハンドルのインデックス
     */
    //--------------------------------------------------------------------------
    nw::snd::SoundHandle* SoundSystem::GetSoundHandle( u8 handleIdx )
    {
      if( m_soundHandleNum <= handleIdx ) {
        GFL_ASSERT(0);  //@check
        return NULL;
      } 
      return &m_pSoundHandles[ handleIdx ]; 
    }
    //--------------------------------------------------------------------------
    /**
     * @brief サウンドデータマネージャを取得する
     */
    //--------------------------------------------------------------------------
    nw::snd::SoundDataManager* SoundSystem::GetSoundDataManager( void )
    {
      return &m_soundDataManager;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープを取得する
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundHeap* SoundSystem::GetSoundHeap( void )
    {
      GFL_ASSERT( m_soundHeap );  //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          return nw_sound_heap;
        }
      }
      return NULL;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief サウンドヒープの内容を出力する
     */
    //-------------------------------------------------------------------------
    void SoundSystem::DumpSoundHeap( void ) 
    { 
      GFL_ASSERT( m_soundHeap );  //@check
      if( m_soundHeap ) {
        nw::snd::SoundHeap* nw_sound_heap = m_soundHeap->GetNwSoundHeap();
        if( nw_sound_heap->IsValid() ) {
          GFL_PRINT( "CurrentHeapLevel = %d\n", this->GetCurrentHeapLevel() );
          GFL_PRINT( "FreeSize = 0x%x / 0x%x\n", this->GetHeapFreeSize(), this->GetHeapSize() );
          nw_sound_heap->Dump( m_soundDataManager, *m_pSoundArchive ); 
        }
      }
    }


  } //namespace snd
} //namespace gfl

//=============================================================================
/**
 * @file gfl_SoundSystem.cpp
 * @brief �T�E���h�V�X�e��
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
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::snd::SoundSystem);
template class gfl::base::SingletonAccessor<gfl::snd::SoundSystem>;
#endif


namespace gfl {
  namespace snd {


    // 12���ϗ��ŉ�����1�I�N�^�[�u�グ�邽�߂̎��g����
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

    // 12���ϗ��ŉ�����1�I�N�^�[�u�����邽�߂̎��g����
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
     * @brief �R���X�g���N�^
     *
     * @param const InitParam& initParam �������p�����[�^
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
     * @brief DSP �R���|�[�l���g������������
     */
    void SoundSystem::InitDspComponent( const InitParam& initParam )
    {
      nn::Result result;

      result = nn::dsp::Initialize();
      GFL_ASSERT( result.IsSuccess() ); //@check

#ifdef GFL_SND_ENABLE_AAC

      if( initParam.aacUse )
      {
        // AAC �f�R�[�h�p�R���|�[�l���g�����[�h
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
        // �f�t�H���g�R���|�[�l���g�����[�h
        result = nn::dsp::LoadDefaultComponent();
        GFL_ASSERT( result.IsSuccess() ); //@check
      }

#else // GFL_SND_ENABLE_AAC

      {
        // �f�t�H���g�R���|�[�l���g�����[�h
        result = nn::dsp::LoadDefaultComponent();
        GFL_ASSERT( result.IsSuccess() ); //@check
      }
#endif
    }

    /**
     * @brief CTR_Library ������������
     */
    void SoundSystem::InitSDKLibrary( void )
    {
      nn::Result result = nn::snd::Initialize();
      GFL_ASSERT( result.IsSuccess() ); //@check
    }

    /**
     * @brief �T�E���h�V�X�e��������������
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
      // AAC ���C�u�����̏�����
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
     * @brief �T�E���h�A�[�J�C�u������������
     * @param initParam �V�X�e���̏������p�����[�^
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
     * @brief ROM�T�E���h�A�[�J�C�u������������
     * @param initParam �V�X�e���̏������p�����[�^
     */
    void SoundSystem::InitRomSoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_ROM ); //@check

      // �T�E���h�A�[�J�C�u�t�@�C�����J��
      bool open_succeeded = m_romSoundArchive.Open( initParam.bcsarFilePath );
      GFL_ASSERT_STOP( open_succeeded );  //@check

      // �T�E���h�A�[�J�C�u�̃w�b�_��ǂݍ���
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
     * @brief HIO�T�E���h�A�[�J�C�u������������
     * @param initParam �V�X�e���̏������p�����[�^
     */
    void SoundSystem::InitHioSoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_HIO ); //@check

      // �T�E���h�A�[�J�C�u�t�@�C�����J��
      bool open_succeeded = m_hioSoundArchive.Open( initParam.bcsarFilePath );
      GFL_ASSERT_STOP( open_succeeded );  //@check

      // �T�E���h�A�[�J�C�u�̃w�b�_��ǂݍ���
      size_t header_size = m_hioSoundArchive.GetHeaderSize();
      m_pMemoryForSoundArchiveHeader = GflHeapAllocMemory( m_pDevHeap, header_size );
      bool load_succeeded = m_hioSoundArchive.LoadHeader( m_pMemoryForSoundArchiveHeader, header_size );
      GFL_ASSERT_STOP( load_succeeded ); //@check
    }
#endif

    /**
     * @brief MEMORY�T�E���h�A�[�J�C�u������������
     * @param initParam �V�X�e���̏������p�����[�^
     */
    void SoundSystem::InitMemorySoundArchive( const InitParam& initParam )
    {
      GFL_ASSERT( initParam.soundArchiveType == SOUND_ARCHIVE_TYPE_MEMORY ); //@check
      m_memSoundArchive.Initialize( initParam.bcsarData );
    } 

    /**
     * @brief �T�E���h�f�[�^�}�l�[�W��������������
     */
    void SoundSystem::InitSoundDataManager( void )
    {
      size_t buffer_size = m_soundDataManager.GetRequiredMemSize( m_pSoundArchive );
      m_pMemoryForSoundDataManager = GflHeapAllocMemory( m_pDevHeap, buffer_size );

      bool suceeded = m_soundDataManager.Initialize( 
          m_pSoundArchive, 
          m_pMemoryForSoundDataManager, 
          buffer_size );

      // �r������
      m_criticalSectionForSoundDataManagerLoad.Initialize();

#if GFL_SOUNDSYSTEM_HEAP_CHECK
      GFL_PRINT( "SoundDataManager\t%d\n", buffer_size );
#endif
    }

    /**
     * @brief �T�E���h�A�[�J�C�u�v���C���[������������
     */
    void SoundSystem::InitSoundArchivePlayer( void )
    {
      size_t player_buffer_size = m_soundArchivePlayer.GetRequiredMemSize( m_pSoundArchive );
      size_t stream_buffer_size = m_soundArchivePlayer.GetRequiredStreamBufferSize( m_pSoundArchive );
      size_t cache_buffer_size = m_soundArchivePlayer.GetRequiredStreamCacheSize( m_pSoundArchive,0x8000 ); //24kb/1ch���̃L���b�V��

      m_pMemoryForSoundArchivePlayer = GflHeapAllocMemoryAlign( m_pDevHeap, player_buffer_size, 32 );
      m_pMemoryForStreamBuffer = GflHeapAllocMemoryAlign( m_pDevHeap, stream_buffer_size, 32 ); // �A�v���P�[�V�����q�[�v��NG
      m_pMemoryForCacheBuffer = GflHeapAllocMemoryAlign( m_pDevHeap, cache_buffer_size, 32 ); // �A�v���P�[�V�����q�[�v��NG


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
     * @brief �T�E���h�q�[�v������������
     * @param initParam �V�X�e���̏������p�����[�^
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
     * @brief �T�E���h�n���h�����쐬����
     * @param initParam �V�X�e���̏������p�����[�^
     */
    void SoundSystem::CreateSoundHandles( const InitParam& initParam )
    { 
      GFL_ASSERT( !m_pSoundHandles ); //@check

      m_soundHandleNum = initParam.soundHandleNum;
      m_pSoundHandles = GFL_NEW(m_pAppHeap) nw::snd::SoundHandle[ initParam.soundHandleNum ];
    }

    /**
     * @brief �T�E���h3D�}�l�[�W���[������������
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
     * @brief 3D �T�E���h���X�i�[������������
     */
    void SoundSystem::InitSound3DListener( void )
    {
      // ���X�i�[�s���ݒ�
      gfl::math::MTX34 listener_matrix;
      const gfl::math::VEC3 pos( 0.0f, 0.0f, 0.0f );   // ���X�i�[�̈ʒu
      const gfl::math::VEC3 up( 0.0f, 1.0f, 0.0f );  // ���X�i�[�̏���x�N�g��
      const gfl::math::VEC3 dir( 0.0f, 0.0f, 1.0f ); // ���X�i�[�̌����x�N�g��
      this->SetListenerPos( pos, up, dir );

      // �}�l�[�W���ɓo�^
      m_sound3DManager.AddListener( &m_sound3DListener ); 
    }

    /**
     * @brief 3D�A�N�^�[��, 3D�A�N�^�[�p�̃T�E���h�n���h�����쐬����
     * @param initParam �V�X�e���̏������p�����[�^
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
     * @brief 3D�A�N�^�[������������
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
     * @brief �f�X�g���N�^
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

      // �r������j��
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
     * @brief �T�E���h�n���h����j������
     */
    void SoundSystem::DeleteSoundHandles( void )
    {
      GFL_DELETE_ARRAY m_pSoundHandles;
      m_pSoundHandles = NULL;
      m_soundHandleNum = 0;
    }

    /**
     * @brief 3D�A�N�^�[��, 3D�A�N�^�[�p�̃T�E���h�n���h����j������
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
     * @brief �t���[������( ���t���[���Ă�ł������� )
     */
    //-------------------------------------------------------------------------
    void SoundSystem::Update( void )
    {
      m_soundArchivePlayer.Update();
    }



    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h����
     *
     * @param itemID    ���[�h����f�[�^�̃A�C�e��ID
     * @param loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
     */
    //-------------------------------------------------------------------------
    bool SoundSystem::LoadSoundData( u32 itemID, u32 loadFlag )
    {
      GFL_ASSERT_MSG( m_soundHeap, "ERROR!! �T�E���h�q�[�v���쐬����Ă��܂���\n" ); //@check
      if( m_soundHeap ) {
        return this->LoadSoundData( m_soundHeap, itemID, loadFlag );
      }
      return false;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �T�E���h�A�[�J�C�u���̃f�[�^���w�肵���T�E���h�q�[�v�Ƀ��[�h����
     *
     * @param[out] dest      �ǂݍ��񂾃f�[�^��z�u����T�E���h�q�[�v
     * @param[in]  itemID    ���[�h����f�[�^�̃A�C�e��ID
     * @param[in]  loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
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
          GFL_PRINT( "WARNING!! �T�E���h�f�[�^(0x%x)�̃��[�h�Ɏ��s���܂���\n", itemID );
        }
#if GFL_SOUNDSYSTEM_HEAP_CHECK
        else {
          GFL_PRINT( "�T�E���h�f�[�^(0x%x)�����[�h���܂���\n", itemID );
          GFL_PRINT( "SoundHeapSize = 0x%x / 0x%x\n", nw_sound_heap->GetFreeSize(), nw_sound_heap->GetSize() );
        }
#endif
      }
      m_criticalSectionForSoundDataManagerLoad.Leave();
      return result;
    }

    
    //----------------------------------------------------------------------------
    /**
     * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h���� �񓯊���
     *
     * @param thread_heap �X���b�h�q�[�v
     * @param priority    �X���b�h�D�揇��
     * @param itemID      ���[�h����f�[�^�̃A�C�e��ID
     * @param loadFlag    ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
     *
     *	@retval true    �ǂ݊J�n
     *	@retval false   �ǂ݊J�n���s
     */
    //-----------------------------------------------------------------------------
    bool SoundSystem::LoadSoundDataAsync( gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag )
    {
      GFL_ASSERT_MSG( m_soundHeap, "ERROR!! �T�E���h�q�[�v���쐬����Ă��܂���\n" ); //@check
      if( m_soundHeap ) {
        return this->LoadSoundDataAsync( m_soundHeap, thread_heap, priority, itemID, loadFlag );
      }
      return false;
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �T�E���h�A�[�J�C�u���̃f�[�^���V�X�e�������q�[�v�Ƀ��[�h���� �񓯊���
     *
     * @param[out] dest      �ǂݍ��񂾃f�[�^��z�u����T�E���h�q�[�v
     * @param[in]  thread_heap �X���b�h�q�[�v
     * @param[in]  priority    �X���b�h�D�揇��
     * @param[in]  itemID    ���[�h����f�[�^�̃A�C�e��ID
     * @param[in]  loadFlag  ���[�h����f�[�^��_���a�Ŏw��( gfl::snd::SoundSystem::LOAD_xxxx )
     */
    //-----------------------------------------------------------------------
    bool SoundSystem::LoadSoundDataAsync( SoundHeap* dest, gfl::heap::HeapBase * thread_heap, gfl::base::Thread::Priority priority, u32 itemID, u32 loadFlag )
    {
      // �܂��ǂݍ��ݒ�
      if( m_pSoundDataManagerThread ){
        return false;
      }
      {
        // �X���b�h���N��
        SoundDataManagerThread::Description desc;
        desc.m_pSoundSystem    = this;
        desc.m_pDest    = dest;
        desc.m_pResult  = &m_SoundDataManagerThreadResult;
        desc.m_itemID   = itemID;
        desc.m_loadFlag = loadFlag;

        m_pSoundDataManagerThread = GFL_NEW_LOW( thread_heap ) SoundDataManagerThread( thread_heap->GetLowerHandle(), desc );

        // �X���b�h�J�n
        m_pSoundDataManagerThread->Start( priority );
      }
      return true;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �T�E���h�A�[�J�C�u���̃f�[�^���w�肵���T�E���h�q�[�v�Ƀ��[�h����@�񓯊��@�҂�
     *
     *	@retval true  ���[�h����
     *	@retval false ���[�h��
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
          // �I��
          GFL_DELETE m_pSoundDataManagerThread;
          m_pSoundDataManagerThread = NULL;
        }
      }
      return result;
    }


    //----------------------------------------------------------------------------
    /**
     *	@brief  �T�E���h�A�[�J�C�u�@�񓯊��ǂݍ��݂̌��ʂ��擾
     *
     *	@retval true  ����
     *	@retval false ���s
     */
    //-----------------------------------------------------------------------------
    bool SoundSystem::GetLoadSoundDataAsyncResult(void) const
    {
      return m_SoundDataManagerThreadResult;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  �T�E���h�f�[�^�}�l�[�W���փT�E���h�f�[�^��ݒ肷��B
     *
     *	@param    fileID    �Z�b�g����f�[�^�̃t�@�C��ID�ł��B
     *	@param    address   �A�h���X
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
     *	@brief  �T�E���h�f�[�^�}�l�[�W���փT�E���h�O���[�v�t�@�C����o�^����B
     *
     *	@param    groupFile �O���[�v�t�@�C��
     *	@param    size      �T�C�Y
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
     *	@brief  �T�E���h�f�[�^�}�l�[�W������T�E���h�O���[�v�t�@�C����o�^��������
     *
     *	@param    groupFile �O���[�v�t�@�C��
     *	@param    size      �T�C�Y
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
     * @brief �w�肵���v���C���[�ōĐ����̃T�E���h�̌����擾����
     * @param playerID  �v���C���[�̃A�C�e��ID
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
     * @brief �w�肵���v���C���[�̉��ʂ��擾����
     * @param playerID  �v���C���[�̃A�C�e��ID
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
     * @brief �w�肵���v���C���[�̉��ʂ�ύX����
     * @param playerID  �v���C���[�̃A�C�e��ID
     * @param volume    �ݒ肷�鉹��[0.0, 2.0]
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetPlayerVolume( u32 playerID, f32 volume )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.SetVolume( volume );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���v���C���[�ōĐ����̑S�ẴT�E���h���~����
     * @param playerID      �v���C���[�̃A�C�e��ID
     * @param fadeOutFrame  �t�F�[�h�A�E�g�ɂ�����t���[����
     */
    //-------------------------------------------------------------------------
    void SoundSystem::StopPlayerSound( u32 playerID, u32 fadeOutFrame )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.StopAllSound( fadeOutFrame );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �w�肵���v���C���[�ōĐ����̑S�ẴT�E���h���ꎞ��~�E�ĊJ����
     * @param playerID   �v���C���[�̃A�C�e��ID
     * @param pauseOn    true:�ꎞ��~, false:�ĊJ
     * @param fade_frame �t�F�[�h�A�E�g�ɂ�����t���[����
     */
    //-------------------------------------------------------------------------
    void SoundSystem::PausePlayerSound( u32 playerID, bool pause_on, u32 fade_frame )
    {
      nw::snd::SoundPlayer& player = m_soundArchivePlayer.GetSoundPlayer( playerID );
      player.PauseAllSound( pause_on, fade_frame );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �Đ�����
     *
     * @param handleIdx    �Đ���̐���Ɏg�p����n���h���̃C���f�b�N�X
     * @param itemID       �Đ�����T�E���h�̃A�C�e��No.
     * @param fadeInFrames �t�F�[�h�C���ɂ�����t���[����
     *
     * @return �Đ������������ꍇ true
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
     * @brief �Đ��̏������J�n����
     *
     * @param handleIdx    �Đ���̐���Ɏg�p����n���h���̃C���f�b�N�X
     * @param itemID       �Đ�����T�E���h�̃A�C�e��No.
     *
     * @retval true   ����
     * @retval false  ���s
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
     * @brief PrepareSound()�ŊJ�n�����Đ������������������ǂ����𒲂ׂ�
     *
     * @param handleIdx  �Đ�����T�E���h�n���h���̃C���f�b�N�X
     *
     * @retval true   �Đ�����������
     * @retval false  �Đ�������������
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) {  // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.IsPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {  // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        return stream_handle.IsPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_WAVE ) {  // �E�F�[�u�T�E���h
        nw::snd::WaveSoundHandle wave_handle( handle );
        return wave_handle.IsPrepared();
      }
      return false;
    }

    /**
     * @brief �T�E���h�n���h���Ɋ֘A�t�����Ă���T�E���h�̎�ނ𒲂ׂ�
     *
     * @param handle �Ώۂ̃T�E���h�n���h��
     */
    nw::snd::SoundArchive::SoundType SoundSystem::GetAttachedSoundType( const nw::snd::SoundHandle* handle ) const
    {
      if( !handle->IsAttachedSound() ) {  // �T�E���h���֘A�t�����Ă��Ȃ�
        return nw::snd::SoundArchive::SOUND_TYPE_INVALID;
      } 
      u32 soundID = handle->GetId();
      return this->GetSoundTypeOfItem( soundID );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief PrepareSound()�ŏ��������T�E���h���Đ�����
     *
     * @param handleIdx     �Đ�����T�E���h�n���h���̃C���f�b�N�X
     * @param fadeInFrames  �t�F�[�h�C���ɂ�����t���[����
     *
     * @note �Đ��������������Ă��Ȃ��ꍇ, ����������҂��Ă���Đ����J�n���܂�.
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) {  // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.StartPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) {  // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.StartPrepared();
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_WAVE ) {  // �E�F�[�u�T�E���h
        nw::snd::WaveSoundHandle wave_handle( handle );
        wave_handle.StartPrepared();
      }

      handle->FadeIn( fadeInFrames );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ��~����
     *
     * @param handleIdx     ������s���n���h���̃C���f�b�N�X
     * @param fadeOutFrames �t�F�[�h�A�E�g�ɂ�����t���[����
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
     * @brief �S�ẴT�E���h���~����
     * @param fadeOutFrames �t�F�[�h�A�E�g�ɂ�����t���[����
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
     * @brief �ꎞ��~ or �ĊJ ����
     *
     * @param handleIdx  ������s���n���h���̃C���f�b�N�X
     * @param pauseOn    true:�ꎞ��~, false:�ĊJ
     * @param fadeFrames �t�F�[�h�C��, �t�F�[�h�A�E�g�ɂ�����t���[����
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
     * @brief �ꎞ��~�����ǂ����𒲂ׂ�
     *
     * @param handleIdx ������s���n���h���̃C���f�b�N�X
     *
     * @retval true   �ꎞ��~��
     * @retval false  �ꎞ��~���łȂ�
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
     * @brief �Đ�, ��~���ɂ�����
     *        �t�F�[�h�C��, �t�F�[�h�A�E�g�����܂ł̎c��t���[�������擾����
     *
     * @param u8 handleIdx ������s���n���h���̃C���f�b�N�X
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
     * @brief �ꎞ��~���ɂ�����
     *        �t�F�[�h�C��, �t�F�[�h�A�E�g�����܂ł̎c��t���[�������擾����
     *
     * @param u8 handleIdx ������s���n���h���̃C���f�b�N�X
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
     * @brief �n���h���ɃT�E���h���֘A�t�����Ă��邩�ǂ����𒲂ׂ�
     *
     * @param handleIdx  ���ׂ�n���h���̃C���f�b�N�X
     *
     * @retval true    �֘A�t�����Ă���
     * @retval false   �֘A�t�����Ă��Ȃ�
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
     * @brief �n���h���֘A�t�����Ă���T�E���h��ID���擾����
     *
     * @param handleIdx  ���ׂ�n���h���̃C���f�b�N�X
     *
     * @return �w�肵���n���h���Ɋ֘A�t�����Ă���T�E���hID
     * @retval INVALID_SOUND_ID  �T�E���h���֘A�t�����Ă��Ȃ��ꍇ
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
     * @brief �X�g���[���T�E���h�̍Đ��ʒu���擾����
     *
     * @param handleIdx  �X�g���[���T�E���h�Đ����̃n���h���̃C���f�b�N�X
     *
     * @return �X�g���[���f�[�^�̍Đ��ʒu( �T���v���� )
     * @retval ����  �n���h���������̏ꍇ
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
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        return stream_handle.GetPlaySamplePosition();
      }
      return -1;
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ���ʂ�ύX����
     *
     * @param handleIdx ������s���n���h���̃C���f�b�N�X
     * @param volume    �ݒ肷�鉹�ʂ̔{�� ( 0.0 �` )
     * @param frames    ���ʂ̕ω��ɂ�����t���[����
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
     * @brief �g���b�N���Ƃ̉��ʂ�ύX����
     *
     * @param u8  handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param u32 trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param f32 volume       ���ʂ̔{�� ( �f�t�H���g�� 1.0 )
     * @param s32 frame        ���ʕω��ɂ�����t���[����
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

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackVolume( trackBitFlag, volume );
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackVolume( trackBitFlag, volume, frame );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ������ύX����
     *
     * @param handleIdx ������s���n���h���̃C���f�b�N�X
     * @param pitch     ���g���䗦
     *
     * @note ���g���䗦 
     *   0.5 = 1 �I�N�^�[�u ��
     *   1.0 = ���{ ( �f�t�H���g�l )
     *   2.0 = 1 �I�N�^�[�u ��
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
     * @brief 12���ϗ��ɑ����������ɕύX����
     *
     * @param handleIdx  ������s���n���h���̃C���f�b�N�X
     * @param level      12���ϗ��̃��x��
     *
     * @note level�̎w���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_UNISON             �ʏ�̉���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1�I�N�^�[�u�Ⴂ����
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2�I�N�^�[�u�Ⴂ����
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
     * @brief 12���ϗ��ɑ����������̎��g���䗦���v�Z����
     *
     * @param level 12���ϗ��̃��x��
     *
     * @note level�̎w���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_UNISON             �ʏ�̉���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1�I�N�^�[�u�Ⴂ����
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2�I�N�^�[�u�Ⴂ����
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
     * @brief �g���b�N���Ƃ̉�����ύX����
     *
     * @param u8  handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param u32 trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param f32 pitch        ���g���䗦
     *
     * �����g���䗦 
     *   0.5 = 1 �I�N�^�[�u ��
     *   1.0 = ���{ ( �f�t�H���g�l )
     *   2.0 = 1 �I�N�^�[�u ��
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

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackPitch( trackBitFlag, pitch );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �g���b�N���Ƃ̉�����12���ϗ��ɑ����������ɕύX����
     *
     * @param handleIdx     ������s���n���h���̃C���f�b�N�X
     * @param trackBitFlag  �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param level         12���ϗ��̃��x��
     *
     * @note level�̎w���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH*2  2�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_HIGH    1�I�N�^�[�u��������
     * EQUAL_TEMPERAMENT_LEVEL_UNISON             �ʏ�̉���
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW     1�I�N�^�[�u�Ⴂ����
     * EQUAL_TEMPERAMENT_LEVEL_ONE_OCTAVE_LOW*2   2�I�N�^�[�u�Ⴂ����
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
     * @brief �p�� ( ���E�̒�� ) ��ύX����
     *
     * @param handleIdx  ������s���n���h���̃C���f�b�N�X
     * @param pan        �p���̑��Βl
     *
     * @note
     * pan ��  0.0 �Ȃ�, �f�[�^�ʂ�̕������畷������.
     * pan �� -1.0 �Ȃ�, ���ʂ��畷�����Ă������������畷������悤�ɂȂ�.
     * pan �� +1.0 �Ȃ�, ���ʂ��畷�����Ă��������E���畷������悤�ɂȂ�.
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
     * @brief �g���b�N���Ƃ̃p�� ( ���E�̒�� ) ��ύX����
     *
     * @param u8    handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param u32   trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param float pan          �p���̒l
     *
     * ���p���̒l
     *   -1.0 = ��
     *    0.0 = ��
     *    1.0 = �E
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

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackPan( trackBitFlag, pan );
      }
      else if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackPan( trackBitFlag, pan );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �T���E���h�p�� ( �O��̒�� ) ��ύX����
     *
     * @param handleIdx  ������s���n���h���̃C���f�b�N�X
     * @param pan        �p���̑��Βl
     *
     * @note
     * pan ��  0.0 �Ȃ�, �f�[�^�ʂ�̕������畷������.
     * pan �� +1.0 �Ȃ�, �őO�����畷�����Ă��������������畷������悤�ɂȂ�.
     * pan �� +2.0 �Ȃ�, �őO�����畷�����Ă��������Ō�����畷������悤�ɂȂ�.
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
     * @brief �g���b�N�̃T���E���h�p�� ( �O��̒�� ) ��ύX����
     *
     * @param u8    handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param u32   trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param float pan          �p���̒l
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

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_STRM ) { // �X�g���[���T�E���h
        nw::snd::StreamSoundHandle stream_handle( handle );
        stream_handle.SetTrackSurroundPan( trackBitFlag, pan );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �e���|��ύX����
     *
     * @param handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param tempoRatio  �e���|�̔{��
     *
     * @attention �V�[�P���X�T�E���h�ɑ΂��Ă̂ݗL���ł��B
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
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTempoRatio( tempoRatio );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �g�����X�|�[�Y
     *
     * @param handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param tempoRatio  �e���|�̔{��
     *
     * @attention �V�[�P���X�T�E���h�ɑ΂��Ă̂ݗL���ł��B
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
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackTranspose( transposeTrackBit, transposeValue );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �g���b�N���Ƃ̃~���[�g��Ԃ�ύX����
     *
     * @param handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param mute         �~���[�g���
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
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackMute( trackBitFlag, mute );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �g���b�N���Ƃ̃T�C�����g��Ԃ�ύX����
     *
     * @param handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param silenceFlag  �T�C�����g��Ԃɂ���ꍇ true, ��������ꍇ false
     * @param fadeTimes    �t�F�[�h���� (�~���b)
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
      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackSilence( trackBitFlag, silenceFlag, fadeTimes );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �g���b�N���Ƃ̃o���N��ύX����
     *
     * @param u8  handleIdx    ������s���n���h���̃C���f�b�N�X
     * @param u32 trackBitFlag �ݒ肷��g���b�N�̃r�b�g�t���O
     * @param s32 bankIdx      �ݒ肷��o���N�̃C���f�b�N�X ( 0 �` 3 )
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

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) { // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        seq_handle.SetTrackBankIndex( trackBitFlag, bankIdx );
      }
    }




    //-------------------------------------------------------------------------
    /*
     * @brief �T�E���h�^�C�v�擾
     *
     * @param[in] u32 itemID  ���ׂ�T�E���h��ID
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
     * @brief �G�t�F�N�g��AUX�o�X�ɒǉ����܂��B
     *
     * @param busID �o�XID�iA,B����܂��j
     * @param effect  �G�t�F�N�g�̒��ۃN���X���p����������
     *
     * @return true�ǉ��ɐ��� false�ǉ��Ɏ��s
     */
    //-----------------------------------------------------------------------
    bool SoundSystem::AppendEffect( nw::snd::AuxBus busID, nw::snd::FxBase *effect )
    {
      return nw::snd::SoundSystem::AppendEffect( busID , effect );
    }
    

    //-----------------------------------------------------------------------
    /**
     * @brief �G�t�F�N�g��AUX�o�X����폜���܂��B
     *
     * @param busID �o�XID�iA,B����܂��j
     * @param �t�F�[�h���ԁi�~���b�j
     */
    //-----------------------------------------------------------------------
    void SoundSystem::RemoveEffect( nw::snd::AuxBus busID, int fadeTimes )
    {
      nw::snd::SoundSystem::ClearEffect( busID , fadeTimes );
    }
    
    //-----------------------------------------------------------------------
    /**
     * @brief ���o�[�u�G�t�F�N�g��AUX�o�X�ɒǉ����܂��B
     *
     * @param busID �o�XID�iA,B����܂��j
     * @param reverb_buffer_heap  ���o�[�u�p�̃o�b�t�@�擾�p�q�[�v
     * @param param �p����
     *
     * @return true�ǉ��ɐ��� false�ǉ��Ɏ��s
     */
    //-----------------------------------------------------------------------
    bool SoundSystem::AppendReverbEffect( nw::snd::AuxBus busID, gfl::heap::HeapBase* reverb_buffer_heap, const nw::snd::FxReverb::Param & param )
    {
      if( m_pMemoryForSoundReverb != NULL )
      {
        GFL_ASSERT_MSG(0,"���o�[�u���Q�d�ɂ����邱�Ƃ͂ł��܂���\n"); //@check
        return false;
      }

      m_fxReverb.SetParam( param );
      
      // �f�B���C�̏������i�p�����[�^�ݒ��̂ݎg�p�\�j
      size_t size = m_fxReverb.GetRequiredMemSize();
      
      // �G�t�F�N�g�̎d�l����̈���m�ہA���蓖��
      m_pMemoryForSoundReverb = GflHeapAllocMemory( reverb_buffer_heap, size );
      m_fxReverb.AssignWorkBuffer( reinterpret_cast<uptr>(m_pMemoryForSoundReverb), size );
      return nw::snd::SoundSystem::AppendEffect( busID , &m_fxReverb );
    }

    //-----------------------------------------------------------------------
    /**
     * @brief �G�t�F�N�g��AUX�o�X����폜���܂��B
     *        RemoveEffect�Ƃ̈Ⴂ�́A���݂��Ă���΃��o�[�u�p�o�b�t�@��������邱�Ƃł�
     *
     * @param busID �o�XID�iA,B����܂��j
     * @param �t�F�[�h���ԁi�~���b�j
     */
    //-----------------------------------------------------------------------
    void SoundSystem::RemoveReverbEffect( nw::snd::AuxBus busID, int fadeTimes )
    {
      this->RemoveEffect( busID, fadeTimes );
      GflHeapSafeFreeMemory(m_pMemoryForSoundReverb);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�q�[�v�̏�Ԃ�ۑ�����
     *
     * @return �ۑ���̊K�w���x��( �ۑ��Ɏ��s�����ꍇ�� -1 )
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
     * @brief �ۑ������T�E���h�q�[�v�̏�Ԃ𕜌�����
     *
     * @param level ��������K�w���x��( ���݂̊K�w���x���ȉ��̒l�̂ݗL�� )
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
     * @brief �T�E���h�q�[�v���N���A����
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
     * @brief ���݂̃T�E���h�q�[�v�̊K�w���x�����擾����
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
     * @brief �T�E���h�q�[�v�̃T�C�Y���擾����
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
     * @brief �T�E���h�q�[�v�̋󂫗e�ʂ��擾����
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
     * @brief �}�X�^�[�{�����[���� �ݒ� ����
     *
     * @param volume    ���ʂ̔{��( 0.0 �` )
     * @param fadeTimes ���ʕω��ɂ����鎞��[�~���b]
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetMasterVolume( float volume, s32 fadeTimes )
    {
      nw::snd::SoundSystem::SetMasterVolume( volume, fadeTimes );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �}�X�^�[�{�����[���� �擾 ����
     *
     * @return �}�X�^�[�{�����[���̔{��( 0.0 �` )
     */
    //-------------------------------------------------------------------------
    float SoundSystem::GetMasterVolume( void ) const
    {
      return nw::snd::SoundSystem::GetMasterVolume();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�o�̓��[�h��ݒ肷��
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetOutputMode( SoundSystem::OutputMode mode )
    {
      nw::snd::SoundSystem::SetOutputMode( static_cast<nw::snd::OutputMode>(mode) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�o�̓��[�h���擾����
     */
    //-------------------------------------------------------------------------
    SoundSystem::OutputMode SoundSystem::GetOutputMode( void ) const
    {
      return static_cast<OutputMode>(nw::snd::SoundSystem::GetOutputMode());
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 3D �T�E���h�̃p�����[�^��ݒ肷��
     *
     * @param param �ݒ肷��3D�T�E���h�p�����[�^
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
     * @brief ���ݐݒ肳��Ă��� 3D �T�E���h�̃p�����[�^���擾����
     *
     * @param[out] pOutParam �p�����[�^�̊i�[��
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
     * @brief ���X�i�[�̃p�����[�^��ݒ肷��
     *
     * @param param �ݒ肷�郊�X�i�[�p�����[�^
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
     * @brief ���X�i�[�̃p�����[�^���擾����
     *
     * @param[out] pOutParam �p�����[�^�̊i�[��
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
     * @brief ���X�i�[�s���ݒ肷��
     *
     * @param matrix �ݒ肷�郊�X�i�[�s��
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerMatrix( const gfl::math::MTX34& matrix )
    {
      m_sound3DListener.SetMatrix( matrix );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���X�i�[�̍��W��ύX����
     *
     * @param pos ���X�i�[�̍��W
     * @param up  ���X�i�[�̏���x�N�g��
     * @param dir ���X�i�[�̌����x�N�g��
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
     * @brief ���X�i�[�̑��x��ݒ肷��( �h�b�v���[���ʂɔ��f����܂� )
     *
     * @param velocity ���X�i�[�̈ړ����x
     *
     * @note
     * SetListenerMatrix(), SetListenerPos() ���Ŏ����I�Ɍv�Z����邽��, 
     * �ʏ�̓A�v���P�[�V�������Őݒ肷��K�v�͂���܂���.
     *
     * @note
     * ���X�i�[���W����u�ŉ����ɔ�Ԃ悤�ȏꍇ�Ɍ���,
     * ���x���������v�Z�ł��Ȃ�����
     * �A�v���P�[�V�������ŏ㏑������K�v������܂�.
     */
    //-------------------------------------------------------------------------
    void SoundSystem::SetListenerVelocity( const gfl::math::VEC3& velocity )
    {
      m_sound3DListener.SetVelocity( velocity );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 3D�T�E���h���Đ�����
     *
     * @param actorIdx    �Đ���̐�����s��3D�A�N�^�[�̃C���f�b�N�X
     * @param itemID      �Đ�����T�E���h�̃A�C�e��ID
     * @param startInfo   �ڍׂȍĐ��p�����[�^
     *
     * @return �Đ������������ꍇ true
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
     * @brief 3D�A�N�^�[�̍Đ������J�n
     *
     * @param actorIdx    �Đ���̐�����s��3D�A�N�^�[�̃C���f�b�N�X
     * @param itemID      �Đ�����T�E���h�̃A�C�e��ID
     * @param startInfo   �ڍׂȍĐ��p�����[�^(NULL�܂��͏ȗ���)
     *
     * @return ���łɍĐ��������ł��Ă���ꍇ true
     *
     * @note
     * ���̊֐���True���Ԃ����A�������́A
     * �Ăяo�����IsPrepared3DActor()��True���Ԃ�܂ő҂��Ă���PlayPrepared3DActor()�ōĐ����J�n���邱��
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
     * @brief 3D�A�N�^�[�̍Đ��������ł��Ă��邩�H
     *
     * @param actorIdx    3D�A�N�^�[�̃C���f�b�N�X
     *
     * @return ���łɍĐ��������ł��Ă���ꍇ true
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
     * @brief �Đ��������ł��Ă���3D�A�N�^�[�̍Đ����J�n����
     *
     * @param actorIdx    3D�A�N�^�[�̃C���f�b�N�X
     *
     * @note
     * Prepar3DActor()�Ăяo����True���Ԃ����A��������
     * Prepar3DActor()�Ăяo����AIsPrepaed3DActor()�ő҂��ATrue���Ԃ�܂ő҂��Ă���Ăяo������
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
     * @brief 3D�A�N�^�[�ōĐ����̑S�ẴT�E���h���~����
     *
     * @param actorIdx   ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param fadeFrames �t�F�[�h�A�E�g�ɂ�����t���[����
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
     * @brief 3D�A�N�^�[�ōĐ����̑S�ẴT�E���h���ꎞ��~�E�ĊJ����
     *
     * @param actorIdx   ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param pauseOn    true �Ȃ�ꎞ��~, false �Ȃ�ĊJ
     * @param fadeFrames �t�F�[�h�C���E�t�F�[�h�A�E�g�ɂ�����t���[����
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
     * @brief 3D�A�N�^�[�̉��ʂ�ύX����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param volume   �ݒ肷�鉹�ʂ̔{��( 0.0 �` 1.0 )
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
     * @brief 3D�A�N�^�[�̉��ʂ��擾����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     *
     * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă��鉹�ʂ̔{��
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
     * @brief 3D�A�N�^�[�̉�����ύX����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param pitch    ���g���䗦
     *
     * @note ���g���䗦 
     *   0.5 = 1 �I�N�^�[�u ��
     *   1.0 = ���{ ( �f�t�H���g�l )
     *   2.0 = 1 �I�N�^�[�u ��
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
     * @brief 3D�A�N�^�[�̉������擾����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     *
     * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă�����g���䗦
     *
     * @note ���g���䗦 
     *   0.5 = 1 �I�N�^�[�u ��
     *   1.0 = ���{( �f�t�H���g�l )
     *   2.0 = 1 �I�N�^�[�u ��
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
     * @brief 3D�A�N�^�[�̃p��( ���E�̒�� )��ύX����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param pan      �p���̒l
     *
     * @note �p���̒l
     *   -1.0 = ��
     *    0.0 = ��
     *    1.0 = �E
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
     * @brief 3D�A�N�^�[�̃p��( ���E�̒�� )���擾����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     *
     * @return �w�肵��3D�A�N�^�[�ɐݒ肳��Ă���p���̒l
     *
     * @note �p���̒l
     *   -1.0 = ��
     *    0.0 = ��
     *    1.0 = �E
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
     * @brief 3D�A�N�^�[�̍��W��ύX����
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param pos      �ݒ肷����W
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
     * @brief 3D�A�N�^�[�̍��W���擾����
     *
     * @param[in]  actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param[out] pos      �擾�������W�̊i�[��
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
     * @brief 3D�A�N�^�[�̑��x��ݒ肷��( �h�b�v���[���ʂɔ��f����܂� )
     *
     * @param actorIdx ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param pos      �ݒ肷����W
     *
     * @note
     * Set3DActorPos() �ō��W��ݒ肵���ۂ�
     * �o�^���̍������玩���I�Ɍv�Z����邽��,
     * �ʏ�̓A�v���P�[�V�������Őݒ肷��K�v�͂���܂���.
     *
     * @note
     * �A�N�^�[���W����u�ŉ����ɔ�Ԃ悤�ȏꍇ�Ɍ���, 
     * ���x���������v�Z�ł��Ȃ�����
     * �A�v���P�[�V�������ŏ㏑������K�v������܂�.
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
     * @brief 3D�A�N�^�[�ōĐ����̃T�E���h�����擾����
     * @param actorIdx        ������s��3D�A�N�^�[�̃C���f�b�N�X
     * @param actorPlayerIdx  �Ώۂ̃A�N�^�[�v���C���[�̃C���f�b�N�X
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
     * @brief 3D�A�N�^�[���S�A�N�^�[�v���C���[�ōĐ����̃T�E���h�����擾����
     * @param actorIdx  ������s��3D�A�N�^�[�̃C���f�b�N�X
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
     * @brief �T�E���h�ɐݒ肳��Ă��郆�[�U�p�����[�^���擾
     *
     * @param itemID  �擾����A�C�e��ID
     */
    //-------------------------------------------------------------------------
    u32 SoundSystem::GetSoundUserParam( u32 itemID )
    { 
      return m_pSoundArchive->GetSoundUserParam( itemID );
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �V�[�P���X�ϐ����擾
     *
     * @param[in]   handleIdx �l���擾����T�E���h�n���h���C���f�b�N�X
     * @param[in]   varNo     �ϐ��i���o�[(0~15)
     * @param[out]  var       �擾�����l���i�[���郏�[�N�ւ̃|�C���^
     * 
     * @retval  true:�擾�����@false:�擾���s
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.ReadVariable( varNo, var );
      }
      else
      { 
        //�V�[�P���X�T�E���h�ȊO�ł͂��̊֐����ĂׂȂ�
        GFL_ASSERT( 0 );  //@check �s���Ȏ�ނ̃T�E���h
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �V�[�P���X�ϐ��Ɋi�[
     *
     * @param[in] handleIdx �l���i�[����T�E���h�n���h���C���f�b�N�X
     * @param[in] varNo     �ϐ��i���o�[(0~15)
     * @param[in] var       �i�[����l
     *
     * @retval  true:�i�[�����@false:�i�[���s
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.WriteVariable( varNo, var );
      }
      else
      { 
        // �V�[�P���X�T�E���h�ȊO�ł͂��̊֐����ĂׂȂ�
        GFL_ASSERT( 0 );  //@check �s���Ȏ�ނ̃T�E���h
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �V�[�P���X�O���[�o���ϐ����擾
     *
     * @param[in]   handleIdx �l���擾����T�E���h�n���h���C���f�b�N�X
     * @param[in]   varNo     �ϐ��i���o�[(0~15)
     * @param[out]  var       �擾�����l���i�[���郏�[�N�ւ̃|�C���^
     * 
     * @retval  true:�擾�����@false:�擾���s
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {
        // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.ReadGlobalVariable( varNo, var );
      }
      else
      { 
        // �V�[�P���X�T�E���h�ȊO�ł͂��̊֐����ĂׂȂ�
        GFL_ASSERT( 0 );  //@check �s���Ȏ�ނ̃T�E���h
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /*
     * @brief �V�[�P���X�O���[�o���ϐ��Ɋi�[
     *
     * @param[in] handleIdx �l���i�[����T�E���h�n���h���C���f�b�N�X
     * @param[in] varNo     �ϐ��i���o�[(0~15)
     * @param[in] var       �i�[����l
     *
     * @retval  true:�i�[�����@false:�i�[���s
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
      GFL_ASSERT( sound_type != nw::snd::SoundArchive::SOUND_TYPE_INVALID );  //@check �s���Ȏ�ނ̃T�E���h

      if( sound_type == nw::snd::SoundArchive::SOUND_TYPE_SEQ ) 
      {  
        // �V�[�P���X�T�E���h
        nw::snd::SequenceSoundHandle seq_handle( handle );
        return seq_handle.WriteGlobalVariable( varNo, var );
      }
      else
      { 
        // �V�[�P���X�T�E���h�ȊO�ł͂��̊֐����ĂׂȂ�
        GFL_ASSERT( 0 );  //@check �s���Ȏ�ނ̃T�E���h
        return false;
      }
    }




    //-------------------------------------------------------------------------
    /*
     * @brief �T�E���h�^�C�v�擾
     *
     * @param[in] handleIdx �擾����T�E���h�n���h���C���f�b�N�X
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
     * @brief �T�E���h�A�[�J�C�u���擾����
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchive* SoundSystem::GetSoundArchive( void )
    {
      return m_pSoundArchive;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�A�[�J�C�u�v���C���[���擾����
     */
    //-------------------------------------------------------------------------
    nw::snd::SoundArchivePlayer* SoundSystem::GetSoundArchivePlayer( void )
    { 
      return &m_soundArchivePlayer;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�n���h�����擾����
     * @param handleIdx  �擾����n���h���̃C���f�b�N�X
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
     * @brief �T�E���h�f�[�^�}�l�[�W�����擾����
     */
    //--------------------------------------------------------------------------
    nw::snd::SoundDataManager* SoundSystem::GetSoundDataManager( void )
    {
      return &m_soundDataManager;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief �T�E���h�q�[�v���擾����
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
     * @brief �T�E���h�q�[�v�̓��e���o�͂���
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

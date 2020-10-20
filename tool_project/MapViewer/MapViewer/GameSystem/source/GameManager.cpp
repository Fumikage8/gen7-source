//===================================================================
/**
 * @file    GameManager.cpp
 * @brief   �Q�[���őS�̂�ʂ��ĊǗ����K�v�ȃN���X�̃C���X�^���X�����N���X
 * @author  k.ohno
 * @date    10.12.22
*/
//===================================================================

#include <heap/include/gfl2_Heap.h>
///#include <debug/include/gfl2_DebugWinSystem.h>
///#include <gfx/include/gfl2_GFGL.h>  //GPU�v���p@saito�s�v
//#include <ro/gfl_RoManager.h>

#include "../../System/include/HeapDefine.h"
#include "../../System/include/HeapSize.h"
#include "System/include/GflvUse.h"
#include "GameSystem/include/GameManager.h"
#include "GameSystem/include/GameProcManager.h"
#include "GameSystem/include/GameEventManager.h"
#include "GameSystem/include/GameData.h"
///#include "Print/include/GrammaticalAttributeProvider.h"@saito�s�v
///#include "Debug/DebugWin/include/DebugWinSystem.h"@saito
///#include "FieldScript/include/FieldScriptSystem.h"@saito�s�v
///#include "Sound/include/Sound.h"@saito�s�v

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(GameSystem::GameManager);
template class gfl2::base::SingletonAccessor<GameSystem::GameManager>;

namespace GameSystem 
{
  // �}�C�N���b���擾
  u64 GetMicroSecond( void )
  {
#if defined(GF_PLATFORM_WIN32)
#include <windows.h>
    LARGE_INTEGER tick;
    LARGE_INTEGER frequency;
    if( ::QueryPerformanceCounter(&tick) == FALSE || ::QueryPerformanceFrequency(&frequency) == FALSE )
    {
      return 0ULL;
    }

    return 1000000ULL * (u64)tick.QuadPart  / (u64)frequency.QuadPart ;
#endif
#if defined(GF_PLATFORM_CTR)
    nn::os::Tick now_tick = nn::os::Tick::GetSystemCurrent();
    return static_cast<s32>(now_tick.ToTimeSpan().GetMicroSeconds());
#endif // GF_PLATFORM_CTR
    return 0;
  }

  // ���������񐔂��擾
  // win32�ł�1�b/60��1��Ƃ����[�������񐔂�Ԃ�
  s32 GetVSyncCount( void )
  {
#if defined(GF_PLATFORM_WIN32)
    return GetMicroSecond() * 60 / 1000000;
#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
    return nngxCheckVSync(NN_GX_DISPLAY_BOTH);
#endif // GF_PLATFORM_CTR

  }

  // ���������҂�
  // win32�ł�1�b/60��1��Ƃ����[�������^�C�~���O�܂ő҂�
  void VSyncWait( void )
  {
#if defined(GF_PLATFORM_WIN32)

    s32 nowCount( GetVSyncCount() );
    while( nowCount == GetVSyncCount() )
    {
      Sleep(1);
    }

#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
#endif // GF_PLATFORM_CTR

  }

  // ���������҂�
  // �X���[�v�ő҂�
  // @param nowCount      ���݂̃J�E���^
  // @param milliseconds  1�X���[�v������̑҂�����(�~���b)
  void VSyncWaitBySleep( s32 nowCount, s64 milliseconds )
  {
    while( nowCount == GetVSyncCount() )
    {
#if defined(GF_PLATFORM_WIN32)
      Sleep( milliseconds );
#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( milliseconds ) );
#endif // GF_PLATFORM_CTR
    }
  }

  // �o�b�t�@�N���A�ݒ�A�����l
  static const GameManager::BUFFER_CLEAR_SETTING sc_BufferClearDefaultSettingTable[ gfl2::gfx::CtrDisplayNo::NumberOf ] = 
  {
    // LEFT,           //!< 3DS�̍���� 400x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
    // RIGHT,          //!< 3DS�̉E��� 400x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
    // DOWN,           //!< 3DS�̉���� 320x240
    {
      gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
      1.0f, 
      255.0f,
      GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
    },
  };

//----------------------------------------------------------------------------
/**
 *  @brief    �R���X�g���N�^���f�X�g���N�^
 *  @param    sHeap �V�X�e���q�[�v
 *  @param    dHeap �f�o�C�X�q�[�v
 */
//-----------------------------------------------------------------------------
GameManager::GameManager(gfl2::heap::HeapBase *sHeap, gfl2::heap::HeapBase *dHeap) :
  m_ReqFrameMode(GameSystem::FRAMEMODE_MAX),
  m_FrameMode(GameSystem::FRAMEMODE_60),
  m_FrameCount(0),
  m_VSyncCount( 0 ),
  m_parallaxDepthRange(1.0f/*gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE*/),
  m_fatalEvent(false)
{
  for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
  {
    m_BufferClearSettingTable[ i ] = sc_BufferClearDefaultSettingTable[ i ];
  }

  m_systemHeap = sHeap;
  m_deviceHeap = dHeap;

  m_procCellHeap = GFL_CREATE_HEAP(m_systemHeap, HEAPID_PROCCELL, System::HeapSize::PROCCELL, gfl2::heap::HEAP_TYPE_EXP, false);

  //UI�f�o�C�X�}�l�[�W���̃t���[�����[�g�����ݒ�
  gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();
  if( m_FrameMode == GameSystem::FRAMEMODE_60 )
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );
  }
  else
  {
    p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );
  }

  p_devman->SynchronizeInputUpdateForAllDevice();

#if GFL_DEBUG
  mDebugKeyDisable = false;
#endif

	mEndRequest = false;
}


//----------------------------------------------------------------------------
/**
 *  @brief    �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
GameManager::~GameManager()
{
  if( m_procCellHeap != NULL )
  {
    gfl2::heap::Manager::DeleteHeap(m_procCellHeap);
    m_procCellHeap = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief    ������
 */
//-----------------------------------------------------------------------------
void GameManager::Initialize(void)
{
  this->CreateProcManager();
/**
  // ���@�Ή����W���[��
  m_pGrammaticalAttributeProvider = GFL_NEW(m_systemHeap) print::GrammaticalAttributeProvider();
  m_pGrammaticalAttributeProvider->Initialize(m_systemHeap);
  
  m_FieldScriptSystem = GFL_NEW(m_systemHeap) Field::FieldScript::FieldScriptSystem( this, m_systemHeap );
*///@saito�s�v
	m_GameData = GFL_NEW(m_systemHeap) GameData( m_systemHeap, m_deviceHeap, this->GetUiDeviceManager());
  this->ClearFrameControl();
  this->SetDefaultRenderingParameter();
  m_pFieldSystem = NULL;
}


//----------------------------------------------------------------------------
/**
 *  @brief �v���Z�X�}�l�[�W���𐶐�����
 */
//-----------------------------------------------------------------------------
void GameManager::CreateProcManager( void )
{
  GFL_ASSERT( m_procManager == NULL ); //@check
  m_procManager = GFL_NEW(m_systemHeap) GameSystem::GameProcManager(m_procCellHeap, this);
  m_GameEventManager = GFL_NEW(m_systemHeap) GameSystem::GameEventManager(this);
}

//----------------------------------------------------------------------------
/**
 *  @brief    ���C������
 *  @return   �������I������ꍇfalse ���s��true
 */
//-----------------------------------------------------------------------------
bool GameManager::Process(void)
{
  this->ChangeFrameControl();                // ����t���[�����[�h�̕ύX

  bool result_proc = this->DoFrameAction();  // 1�t���[���ōs���鏈��

  // �C�x���g���AProc�������Ȃ�����Q�[���I��
  return ( result_proc | m_GameEventManager->IsExists() );
}


//----------------------------------------------------------------------------
/**
 *  @brief 1�t���[���ōs�����������s����
 *
 *  @retval true    �r��
 *  @retval false   �I���i�v���Z�X���Ȃ��Ȃ����j
 */
//-----------------------------------------------------------------------------
bool GameManager::DoFrameAction( void )
{
  bool result_proc   = true;
  bool isUpdateFrame = this->UpdateFrameControl();
#if 0
  if( isUpdateFrame )
  {
#if GFL_DEBUG
    // R�{ L + X�@�ŁA�L�[������ԂŁA�f�o�b�O�E�B���h�E���I�[�v��
    this->GetUiDeviceManager()->SetDebugKeyDisable( false );  // �L�[�L��

    //@todo xy_system::GflUse::isCreateGraphicsSystem();���`�F�b�N���Ă������Ȃ��H
    //if( !IsDebugMode() )
    {
      bool is_before_open = gfl2::debug::DebugWin_IsOpen();

      // �f�o�b�O�L�[�@���O�v���C���ł��A�I���W�i���̃C���v�b�gON
      DebugUiDeviceSoftwareControlEnabel(false);
      {

        gfl2::debug::DebugWin_Update();
      }
      DebugUiDeviceSoftwareControlEnabel(true);
      // �f�o�b�O�L�[�@���O�v���C���ł��A�I���W�i���̃C���v�b�gOFF

      // ������OPEN�ɂȂ����B
      if( gfl2::debug::DebugWin_IsOpen() && !is_before_open )
      {
        //@todo ���O�Đ��̎d�g��(�I�����W����Debug�@�\(�L��)�ɂ��邪�Aniji�Ōʎ������邩�H

        // L��������Ă�����A�L�[������Ԃ�
        if( this->GetUiDeviceManager()->GetButton(0)->IsHold( gfl2::ui::BUTTON_L ) )
        {
          mDebugKeyDisable = true;
        }
      }

    }
#endif
  }
#endif //@saito_del

    // @todo �f�o�b�O�E�B���h�E�֘A����
    // �f�o�C�X�̍X�V���J�b�g
#if GFL_DEBUG
  if(this->IsDebugKeyDisable())
  {
    this->GetUiDeviceManager()->SetDebugKeyDisable( true );
  }
#endif // GFL_DEBUG
  if( isUpdateFrame )
  {
    {
      //xy_debug::PauseSystem_Update();

      if( !isPause() )
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);@saito�s�v

        // �C�x���g�X�V
        this->UpdateEventProcess();

        // 1/60�v���Z�X����
        result_proc = this->UpdateProcManager();

        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);@saito�s�v
      }
    }
    // �T�E���h�V�X�e�����C��
    ///Sound::Update();@saito�s�v
  }
  else
  {
    // 2/60�v���Z�X�����B �u1/30���쎞�v�݂̂�����ɏ���������܂��B
    {
      if( !isPause() )
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);@saito�s�v
        result_proc = this->NextFrameUpdateProcManager();
        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);@saito�s�v
      }
    }
  }

  // �`��
  if( m_FrameMode == GameSystem::FRAMEMODE_60 || !isUpdateFrame )
  {

    if( 1 /*mGameData->GetIsSkipDraw() == false*/ ) //@todo
    {
      //������Draw�̌v�����s���ƁA�֐�����VSyncWait���Ă΂�Ă���̂ō��邩������Ɉړ��B
      this->DrawProcManager();

      //@todo �z���g�͂�����GPU/Draw�̃I�[�o�[�`�F�b�N�������肷��
    }
#if 0
    else
    {
      mGameData->SetIsSkipDraw(false);
    }
#endif
  }

#if GFL_DEBUG
  // �����ŁA�f�o�b�O�L�[��������������B
  // �������Ȃ��ƁA�f�o�b�O�E�B���h�E���������t���[���ŁA
  // �A�v���P�[�V�������L�[���͂��Ă��܂��B
  ///if( !gfl2::debug::DebugWin_IsOpen() ){ mDebugKeyDisable = false; }
#endif // GFL_DEBUG

  return result_proc;
}


//----------------------------------------------------------------------------
/**
 *  @brief �v���Z�X�}�l�[�W�����X�V����
 *
 *  @retval true    �r��
 *  @retval false   �I���i�v���Z�X���Ȃ��Ȃ����j
 */
//-----------------------------------------------------------------------------
bool GameManager::UpdateProcManager( void )
{
  if( m_procManager ){
    const gfl2::proc::MainStatus ret = m_procManager->Main();
    if(gfl2::proc::MAINSTATUS_NULL == ret ){
      //�Q�[���I��
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief �v���Z�X�}�l�[�W���� 1/30�t���[�����쎞��2/60 �X�V����
 *
 *  @retval true    �r��
 *  @retval false   �I���i�v���Z�X���Ȃ��Ȃ����j
 */
//-----------------------------------------------------------------------------
bool GameManager::NextFrameUpdateProcManager( void )
{
  if( m_procManager ){
    const gfl2::proc::MainStatus ret = m_procManager->NextFrameMain();
    if(gfl2::proc::MAINSTATUS_NULL == ret){
      //�Q�[���I��
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �`�揈��(1/60���쎞�j
 */
//-----------------------------------------------------------------------------
void GameManager::DrawProcManager( void )
{
  if( m_procManager )
  {
#if 0
#if GFL_DEBUG
    if( !IsDebugMode() )
#endif
#endif
    {
      {
        ///Debug::DebugWin::TickStart(Debug::DebugWin::DEBUGWIN_TICK_DRAW);@saito�s�v
        // �o�^����PROC�ɂ��A�f�B�X�v���C�o�b�t�@�Ɉˑ����Ȃ��`�揈�������s
        if( m_procManager->GetDrawFlag() )
        {
          m_procManager->PreDraw();
        }

        for( u32 i = 0; i < gfl2::gfx::CtrDisplayNo::NumberOf; i++ )
        {
#if defined(GF_PLATFORM_WIN32)

          // PC�ł͉E��ʂ̕`����X�L�b�v
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT )
          {
            continue;
          }

#endif //GF_PLATFORM_WIN32
#if defined(GF_PLATFORM_CTR)

          // ���̎�OFF���͉E��ʂ̕`����X�L�b�v
          // @todo ���̎��ɂȂ����u�ԂɃ`��������������A�Ώ����@�͂܂����܂��ĂȂ� 150223 ikuta_junya
          if( i == gfl2::gfx::CtrDisplayNo::RIGHT && nn::gx::CTR::IsStereoVisionAllowed() == false )
          {
            continue;
          }

#endif // GF_PLATFORM_CTR

          // �f�B�X�v���C�ݒ�
          gfl2::gfx::GFGL::SetActiveDisplay( System::GflvUse::m_HandleList[i] );
          
          // �����_�[�^�[�Q�b�g�ݒ�
          gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflvUse::m_HandleList[ i ] );
          gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflvUse::m_HandleList[ i ] );
          gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
		      gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );

          // �����_�[�^�[�Q�b�g�N���A����
          // @note BeginScene()������RenderTarget�y�сADepthStencil���Q�Ƃ���BSet����Ă��Ȃ��ꍇ�̓A�N�Z�X��O���������܂� 150224 ikuta_junya
          if( gfl2::gfx::GFGL::BeginScene() )
          {
            const BUFFER_CLEAR_SETTING& rSetting = m_BufferClearSettingTable[ i ];

            // �r���[�|�[�g�ݒ�
            u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflvUse::m_HandleList[ i ] );
            u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflvUse::m_HandleList[ i ] );

            if ( displayWidth && displayHeight )
            {
              gfl2::gfx::GFGL::SetViewport( 0, 0, displayWidth, displayHeight );
              gfl2::gfx::GFGL::SetScissor( 0, 0, displayWidth, displayHeight );
            }
            else
            {
              const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
              gfl2::gfx::GFGL::SetViewport( 0, 0, rDesc.width, rDesc.height);
              gfl2::gfx::GFGL::SetScissor( 0, 0, rDesc.width, rDesc.height);
            }

            // �J���[���[�x�X�e���V��
            if( rSetting.clearTarget == ( BUFFER_CLEAR_TARGET_BIT_COLOR | BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL ) )
            {
              gfl2::gfx::GFGL::ClearRenderTargetDepthStencil( pBackBuffer, rSetting.color, pDepthBuffer, rSetting.depth, rSetting.stencil );
            }
            else 
            {
              // �J���[
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_COLOR )
              {
                gfl2::gfx::GFGL::ClearRenderTarget( pBackBuffer, rSetting.color );
              }

              // �[�x�X�e���V��
              if( rSetting.clearTarget & BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL )
              {
                gfl2::gfx::GFGL::ClearDepthStencil( pDepthBuffer, rSetting.depth, rSetting.stencil );
              }
            }
            gfl2::gfx::GFGL::EndScene();
          }

          // �o�^����PROC�ɂ��A�t���[���o�b�t�@�ւ̕`������s���܂�
          if( m_procManager->GetDrawFlag() )
          {
            m_procManager->Draw( static_cast< gfl2::gfx::CtrDisplayNo >( i ) );
            ///gfl2::debug::DebugWin_Draw(static_cast< gfl2::gfx::CtrDisplayNo >( i ) , System::GflvUse::m_HandleList[i]);
            
            gfl2::gfx::GFGL::TransferDisplay();	// @todo �{����if( m_procManager->GetDrawFlag() )�̊O�����A�����`�悹���ɂɎ��s����ƃn���O���Ă��܂��H 
          }
          // @todo Fade�`�揈��
          // @todo Debug�`�揈��
          
          // �f�B�X�v���C�o�b�t�@�ɓ]�� @todo �����GameManager
        }

        // @todo GPU�v�� ikuta_junya 150215

        //gfl2::grp::Fade_Update();
        //xy_system::GflUse::GetHomeNix()->Update();
        //xy_system::GflUse::GetHomeNix()->Draw();
        // grpSys->EndRendering(m_parallaxDepthRange * (nn::gx::IsStereoVisionAllowed() ? 1.0f:0.0f));

        ///Debug::DebugWin::TickEnd(Debug::DebugWin::DEBUGWIN_TICK_DRAW);@saito�s�v
        // @todo ����if���͖{���͕s�v�����A����ł�Draw�֐������s����Ȃ��Ƃ��Ɏ~�܂��Ă��܂�
        if( m_procManager->GetDrawFlag() )
        {
          // 30�t���[�����[�h�ŁA�O�񂩂瓯������ĂȂ��Ȃ瓯��������
          if( m_FrameMode == GameSystem::FRAMEMODE_30 && m_VSyncCount == GetVSyncCount() )
          {
            // VSyncWait���s���O�ɕς���Ă��܂��\��������̂ŁASleep�ő҂B
            VSyncWaitBySleep( m_VSyncCount, 1 );
          }

          // PresentPath
          gfl2::gfx::GFGL::Finish();
          gfl2::gfx::GFGL::SwapDisplay();

#if defined(GF_PLATFORM_WIN32)
          // win32�ł͐���������؂��Ă���̂ŁA���O��1�b/60��1��Ƃ����[�������^�C�~���O�܂ő҂��܂� @todo �� 150219 ikuta_junya
          VSyncWait();
#endif

          m_VSyncCount = GetVSyncCount();
        }

     }

      // 1�b������̕`��񐔂��v�����g
#if GFL_DEBUG
      static u64 s_BeforeTime = 0;
      static u32 s_FpsCount = 0;

      // FPS�J�E���g�F1�b��1��A�Ăяo���񐔂����Z�b�g
      u64 now = GetMicroSecond();
      if( now - s_BeforeTime >= 1000000 )
      {
        s_BeforeTime = now;
        //GFL_PRINT("FPS: %d \n", s_FpsCount );
        s_FpsCount = 0;
      }

      s_FpsCount++;
#endif
    }
  ///Debug::DebugWin::TickCalc(Debug::DebugWin::DEBUGWIN_TICK_GPU,gfl2::gfx::GFGL::GetTotalGPUTimeMicroSecond());@saito�s�v
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �C�x���g����
 */
//-----------------------------------------------------------------------------
void GameManager::UpdateEventProcess( void )
{
  /**
  // NULL���ǂ��������`�F�b�N
  if( m_GameEventManager->isExists() == false )
  {
    // �C�x���g���s�O�@�C�x���g���m����
    m_procManager->EventCheck();
  }

  // �C�x���g����
  m_GameEventManager->Main();
  *///@saito hock
}


//----------------------------------------------------------------------------
/**
 *  @brief    �J������
 */
//-----------------------------------------------------------------------------
void GameManager::Finalize(void)
{
/**  
  GFL_SAFE_DELETE( m_FieldScriptSystem );
  m_pGrammaticalAttributeProvider->Finalize();
  GFL_SAFE_DELETE( m_pGrammaticalAttributeProvider );
  
*///@saito�s�v
	GFL_SAFE_DELETE( m_GameData );
  this->DeleteProcManager();
}


//----------------------------------------------------------------------------
/**
 *  @brief �v���Z�X�}�l�[�W����j������
 */
//-----------------------------------------------------------------------------
void GameManager::DeleteProcManager( void )
{
  GFL_SAFE_DELETE( m_GameEventManager );
  GFL_SAFE_DELETE( m_procManager );
}


//----------------------------------------------------------------------------
/**
 *  @brief    ����t���[���ύX���N�G�X�g
 */
//-----------------------------------------------------------------------------
void GameManager::RequestFrameMode( GameSystem::FrameMode mode )
{
  this->RequestFrameControl(mode);
}


//----------------------------------------------------------------------------
/**
 *  @brief    �����_�����O�̃p�����[�^�Ƀf�t�H���g�l��ݒ肷��B
 */
//-----------------------------------------------------------------------------
void GameManager::SetDefaultRenderingParameter( void )
{
  m_parallaxDepthRange = 1.0f; //gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ONE; @todo
}


//----------------------------------------------------------------------------
/**
 *  @brief    �����_�����O�̃p�����[�^��ݒ肷��B
 *            �ݒ肵�����܂ł��̒l���g�p���ꑱ���܂��B
 *
 *  @param    parallaxDepthRange  ���̎��̎����̕␳�l
 *                                 (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                  gfl2::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 */
//-----------------------------------------------------------------------------
void GameManager::SetRenderingParameter( const f32 parallaxDepthRange )
{
  m_parallaxDepthRange = parallaxDepthRange;
}


//----------------------------------------------------------------------------
/**
 *  @brief    �����_�����O�̃p�����[�^���擾����B
 *
 *  @param[out]    parallaxDepthRange  ���̎��̎����̕␳�l
 *                                      (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
 *                                       gfl2::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
 */
//-----------------------------------------------------------------------------
void GameManager::GetRenderingParameter( f32* parallaxDepthRange ) const
{
  if( parallaxDepthRange )
  {
    *parallaxDepthRange = m_parallaxDepthRange;
  }
}


// @fix SBTS[2501] : �y�\���F����̃f�����ɗ��̎���ON�ɂ���ƉE�ڗp�f���ɕs�����������z
//----------------------------------------------------------------------------
/**
 *	@brief  �E�ڂ������_�����O����邩�H
 *
 *	@retval true  �����_�����O�����
 *	@retval false �����_�����O����Ȃ�
 */
//-----------------------------------------------------------------------------
bool GameManager::IsUpperRightRendering( void ) const
{
  return false;
#if 0
  // ���������Ȃ��ꍇ�́A�E�ڂ������_�����O���Ă��Ȃ��B
  if( (m_parallaxDepthRange * (nn::gx::IsStereoVisionAllowed() ? 1.0f:0.0f)) == gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_ZERO ){
    return false;
  }
  return true;
#endif
}


//-----------------------------------------------------------------------------
/**
 * @brief �t�@�C���ǂݍ��݃}�l�[�W�����擾����
 */
//-----------------------------------------------------------------------------
gfl2::fs::AsyncFileManager* GameManager::GetAsyncFileManager( void ) const
{
  return System::GflvUse::m_Singlton.m_romAsyncFileManager;
}


//-----------------------------------------------------------------------------
/**
 * @brief UI �f�o�C�X�}�l�[�W�����擾����
 */
//-----------------------------------------------------------------------------
gfl2::ui::DeviceManager* GameManager::GetUiDeviceManager( void ) const
{
  return System::GflvUse::m_Singlton.m_pUiDeviceManager;
}

#if 0
//-----------------------------------------------------------------------------
/**
 * @brief �O���t�B�b�N�X�V�X�e�����擾����
 */
//-----------------------------------------------------------------------------
gfl2::grp::GraphicsSystem* GameManager::GetGraphicsSystem( void ) const
{
  return xy_system::GflvUse::m_Singlton.m_pGraphicsSystem;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�X�V�X�e�����f�t�H���g�̏�Ԃɖ߂��B
 */
//-----------------------------------------------------------------------------
void GameManager::SetupDefaultGraphicsSystem( void )
{
  xy_system::GflvUse::m_Singlton.m_pGraphicsSystem->ChangeSetUp( &xy_system::GflvUse::m_DefaultGraphicsSetUp );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�X�V�X�e���̃Z�b�g�A�b�v���Ƀf�t�H���g�l��^����B
 */
//-----------------------------------------------------------------------------
void GameManager::SetDefaultGraphicsSystemSetUp( gfl2::grp::GraphicsSystemSetUp* setup ) const
{
  *setup = xy_system::GflvUse::m_DefaultGraphicsSetUp;
}
#endif

//-----------------------------------------------------------------------------
/**
 * @brief ���ݓ��삵�Ă���v���Z�X�����邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool GameManager::IsProcessExists( void ) const
{
  if( m_procManager->GetBaseProcess() == NULL ){
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���ݓ��삵�Ă���GameEvent�����邩�`�F�b�N
 */
//-----------------------------------------------------------------------------
bool GameManager::IsEventExists( void ) const
{
  return m_GameEventManager->IsExists();
}

// �o�b�t�@�N���A�ݒ�

/**
*	@brief  �w���ʂ̃N���A�ݒ��ݒ肷��
*
*  @param  display   �Ώۉ��
*  @param  rSetting  �N���A�ݒ�
*
*  @return ����
*/
void GameManager::SetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, const BUFFER_CLEAR_SETTING& rSetting )
{
  if( display >= gfl2::gfx::CtrDisplayNo::NumberOf )
  {
    return;
  }

  m_BufferClearSettingTable[ display ] = rSetting;
}

/**
*	@brief  �w���ʂ̃N���A�ݒ���擾����
*
*  @param  display   �Ώۉ��
*  @param  pOut      �i�[��
*
*  @return ����
*/
b32 GameManager::GetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, BUFFER_CLEAR_SETTING* pOut ) const
{
  if( display >= gfl2::gfx::CtrDisplayNo::NumberOf )
  {
    return false;
  }

  *pOut = m_BufferClearSettingTable[ display ];
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t���[���Ǘ��̏�����
 */
//-----------------------------------------------------------------------------
void GameManager::ClearFrameControl( void )
{
  m_ReqFrameMode= GameSystem::FRAMEMODE_MAX;
  m_FrameMode   = GameSystem::FRAMEMODE_60;
  m_FrameCount  = 0;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���̃��[�h�`�F�b�N
 *
 *  @param  mode  ���ׂ������[�h
 *
 *  @retval true  ���̃��[�h
 *  @retval false �Ⴄ���[�h
 */
//-----------------------------------------------------------------------------
bool GameManager::IsFrameControl( GameSystem::FrameMode mode ) const   //
{
  return ( m_FrameMode == mode );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t���[���R���g���[���X�V
 *
 *  @retval true    1�t���[���ڂ̏������s���i�C�x���g���C���ƃv���Z�X���C���j
 *  @retval false   2�t���[���ڂ̏������s���i�v���Z�X�̃l�N�X�g�t���[�����C���j
 */
//-----------------------------------------------------------------------------
bool GameManager::UpdateFrameControl( void )
{
  if( m_FrameMode == GameSystem::FRAMEMODE_60 ){
    return true;
  }
  m_FrameCount ^= 1;

  return m_FrameCount;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t���[���̍X�V����
 */
//-----------------------------------------------------------------------------
void GameManager::ChangeFrameControl( void )
{
  // ���N�G�X�g��������΁A�������Ȃ��B
  if( m_ReqFrameMode == GameSystem::FRAMEMODE_MAX ){
    return ;
  }

  // �ύX
  if( m_FrameMode != m_ReqFrameMode ){
    
    gfl2::ui::DeviceManager* p_devman = this->GetUiDeviceManager();

    if( m_ReqFrameMode == GameSystem::FRAMEMODE_60 ){

      // �L�[����
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_60 );

    }
    // 1/30�t���[������
    else{

      // �L�[����
      p_devman->SetFrameRateForAllDevice( gfl2::ui::Device::FRAME_RATE_30 );
      p_devman->SynchronizeInputUpdateForAllDevice();
    }

    // Top�t���[������J�n
    m_FrameCount = 0;
    m_FrameMode  = m_ReqFrameMode;
  }

  m_ReqFrameMode = GameSystem::FRAMEMODE_MAX;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t���[���R���g���[���̃��[�h�ύX���N�G�X�g�ݒ�
 *
 *  @param  mode  �ύX�������[�h
 */
//-----------------------------------------------------------------------------
void GameManager::RequestFrameControl( GameSystem::FrameMode mode )
{
  if( mode < GameSystem::FRAMEMODE_MAX ){
    m_ReqFrameMode = mode;
  }else{
    GFL_ASSERT( mode < GameSystem::FRAMEMODE_MAX ); //@check
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief   �~�߂邩�ǂ���
 */
//-----------------------------------------------------------------------------
bool GameManager::isPause( void )
{
#if 0
  if( xy_debug::PauseSystem_IsPause() ){
    return true;
  }
#endif
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @brief UI���O�Đ��̈ꎞ��~�ݒ�
 */
//-----------------------------------------------------------------------------
void GameManager::DebugUiDeviceSoftwareControlEnabel( bool isEnable )
{
  /**
   * @brief �\���L�[���擾����
   * @param keyID  �擾����\���L�[��ID
   */
  gfl2::ui::VectorDevice* pCross = GetUiDeviceManager()->GetCrossKey( 0 );
  pCross->SetSoftwareControlEnable(isEnable);

  /**
   * @brief �A�i���O�X�e�B�b�N���擾����
   * @param stickID  �擾����A�i���O�X�e�B�b�N��ID
   */
  gfl2::ui::VectorDevice* pStick = GetUiDeviceManager()->GetStick( 0 );
  pStick->SetSoftwareControlEnable(isEnable);

  /**
   * @brief �{�^�����擾����
   * @param buttonID  �擾����{�^����ID
   */
  gfl2::ui::Button* pButton = GetUiDeviceManager()->GetButton( 0 );
  pButton->SetSoftwareControlEnable(isEnable);

  /**
   * @brief �^�b�`�p�l�����擾����
   * @param tpID  �擾����^�b�`�p�l����ID
   */
  gfl2::ui::TouchPanel* pTouch = GetUiDeviceManager()->GetTouchPanel( 0 );
  pTouch->SetSoftwareControlEnable(isEnable);
}


} // namespace GameSystem


//===================================================================
/**
 * @file    CommonApplication.cpp
 * @brief   �R�����A�v���P�[�V����
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

/**
#include "../../System/include/MyRenderingPipeLine.h"
*///@saito
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "System/include/GflvUse.h"
///#include "System/include/PmlUse.h"@saito�s�v
#include "GameSystem/include/GameManager.h"
///#include "Sound/include/Sound.h"@saito�s�v

#include "IApplication.h"
#include "CommonApplication.hpp"
// niji
#include "GameSystem/include/GameProcManager.h"

#include "ViewerProc/include/ViewerProc.h"//@saito add
/**
#include "test/FieldTest/include/FieldTestProc.h"   // @todo �^�C�g�������������܃^�C�g���������o����܂ŉ��Ώ� 150207 ikuta_junya

#include <App/AppTemplate/include/AppTemplate.h>  // ���C�A�E�g�`��e�X�g  @todo kawada_koji_20150209

// @todo Layout��GARC�ǂݍ��݃e�X�g�p	fukushima_yuya
#include <Test/LayoutArcTest/include/LayoutArcTest.h>
// @todo DrawUtilText�̃e�X�g�p fukushima_yuya
#include <Test/DrawUtilTextTest/include/DrawUtilTextProc.h>

// @todo �e�X�g�\���R�[�h�̓e�X�g�v���Z�X�Ɉ����z���\��  150207 ikuta_junya
*///@saito�s�v
///#include <Debug/Launcher/include/LauncherProc.h>@saito�s�v
///#include <Debug/StartMenu/include/StartLauncher.h>@saito�s�v
#include "DebugWin/include/DebugWinSystem.h"

#if 1 // @todo GflUse�ł�����
#include "../Print/include/SystemFont.h"
#endif


namespace Main{

CommonApplication *g_pApp = NULL;


CommonApplication::CommonApplication() :
  Main::Framework::IApplication(),
  mpGameManager(NULL)
{
}

CommonApplication::~CommonApplication()
{
}

bool CommonApplication::OnInit()
{
  System::GflvUse::Initialize();

  // win32�ł̓t���[�����[�g���Œ艻���邽�߁A����������؂� @todo �� 150219 ikuta_junya
#if defined(GF_PLATFORM_WIN32)
#include <GL/glew.h>
  // �g���֐����󂯎��֐��|�C���^
  BOOL (WINAPI *wglSwapIntervalEXT)(int) = NULL;

  if( strstr( (char*)glGetString( GL_EXTENSIONS ), "WGL_EXT_swap_control") == 0)
  {
    // WGL_EXT_swap_control���T�|�[�g���Ă��Ȃ�
  }
  else
  {
    // API���擾���Ď��s
    wglSwapIntervalEXT = (BOOL (WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT");
    if( wglSwapIntervalEXT )
    {
      wglSwapIntervalEXT( 0 );
    }
  }
#endif

  //�풓�V�X�e���pheap�擾
  gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
  gfl2::heap::HeapBase *deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  //PokeLib������
  ///System::PmlUse::Initialize(systemHeap);@saito�s�v
  //�Q�[���}�l�[�W���[������
  mpGameManager = GFL_NEW(systemHeap) GameSystem::GameManager( systemHeap, deviceHeap );
  mpGameManager->Initialize();

  //�f�o�b�O�E�B���h�E������
  ///Debug::DebugWin::InitializeSystem( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEVICE ) );

  //Proc������
  GameSystem::GameProcManager* pProcManager = mpGameManager->GetProcManager();

#if defined(GF_PLATFORM_CTR)
    // ���̎��ݒ�
	gfl2::gfx::GFGL::SetStereoModeEnable(true);
#endif

#if 1 // @todo GflUse�ł�����
	m_pMemAllocator = GFL_NEW(systemHeap) gfl2::heap::NwAllocator( systemHeap );
	m_pDevAllocator = GFL_NEW(deviceHeap) gfl2::heap::NwAllocator( deviceHeap );


  // font
  {
// 	  m_pMemAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pMemHeap );
//	  m_pDevAllocator = GFL_NEW( pMemHeap ) gfl2::heap::NwAllocator( pDevHeap );
  //  m_pRenderingPipeLine = GFL_NEW( systemHeap ) gfl2::util::UtilTextRenderingPipeLine( 512, 256 );
  //  gfl2::util::DrawUtilText::Initialize( m_pMemAllocator, m_pDevAllocator, m_pRenderingPipeLine->GetUtilTextRenderPath() );
	 // gfl2::util::DrawUtilText::SetFont( print::SystemFont_GetFont() );
  }


  // LytSys������
  ///gfl2::lyt::LytSys::InitializeSystem( m_pMemAllocator, m_pDevAllocator );
  //�t�H���g�ݒ�
  ///view_print::SystemFont_SetLytSystemDefaultFont();
#endif

  // @todo �v���Z�X�N���A�^�C�g���������o����܂ŉ��Ώ�(kujira/sango�ł͌���I���C�x���g���R�[�����Ă���)  150207 ikuta_junya
//  NIJI_PROC_CALL< GameSys::GameProcManager, Test::FieldTest::FieldTestProc >( pProcManager );
  
  // @todo 150217 ikuta_junya �`��p�C�v���C���ڐA�ɔ����A���������܂���
  // mpGameManager->SetLayoutRenderPath( m_pRenderingPipeLine->GetLayoutRenderPath() );


  //{  // ���C�A�E�g�`��e�X�g  @todo kawada_koji_20150209
  //    NIJI_PROC_CALL< GameSys::GameProcManager, app::app_template::AppTemplateProc >( pProcManager );
  //}
  /**
#if PM_DEBUG
  { // �����`���[
    debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
    debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( pProcManager, &m_pLauncherParam );
    launcher->CreateStartListData<debug::startmenu::StartLauncher>();
  }
#endif  // PM_DEBUG
  *///@saito�s�v

  ///@saito add���̃v���b�N��ݒ�
  NIJI_PROC_CALL< GameSystem::GameProcManager, Field::ViewerProc >( pProcManager );

  // @todo Layout��GARC�ǂݍ��݃e�X�g�p	fukushima_yuya
  //NIJI_PROC_CALL< GameSys::GameProcManager, Test::LytArc::LayoutArcProc >( pProcManager );

  {
  // @todo DrawUtilText�̃e�X�g�p
  //    NIJI_PROC_CALL< GameSys::GameProcManager, Test::UtilText::DrawUtilTextProc >( pProcManager );
  }

	return true;
}

bool CommonApplication::OnUpdate()
{
  // @todo GameProc�����݂���ƁAOnDraw�Ă΂�Ȃ��Ȃ��Ă��܂��̂ŁA��U�R�����g�A�E�g���܂��� 150207
  // while( true )
  {
    System::GflvUse::Update();

    //if(!GFL_SINGLETON_INSTANCE(xynet::XyNetErrManager)->IsFatalError())
    {
      // Game�}�l�[�W���X�V
      if(!mpGameManager->Process())
      {
        return false;
      }
    }
    // VSync����
    //xy_system::GflUse::VSync();
  }
  return true;
}
bool CommonApplication::OnDraw()
{
  // @note �`��p�C�v���C����GameManager�ɈڐA���܂��� 150217 ikuta_junya
  //@saito_todo�`��p�C�v���C�����쐬
	return true;
}

bool CommonApplication::OnTerm()
{
#if 1 // @todo GflUse�ł�����
  ///gfl2::lyt::LytSys::FinalizeSystem();
  GFL_SAFE_DELETE( m_pDevAllocator );
  GFL_SAFE_DELETE( m_pMemAllocator );
#endif
  ///Sound::FinalizeForGameReset(mpGameManager->GetAsyncFileManager());@saito�s�v
  //�f�o�b�O�E�B���h�E�I��
  ///Debug::DebugWin::FinalizeSystem();
  mpGameManager->Finalize();
  GFL_SAFE_DELETE(mpGameManager);
  ///System::PmlUse::Finalize();@saito�s�v
  System::GflvUse::Finalize();
  return true;
}

void CommonApplication::EndRequest()
{
	mpGameManager->EndRequest();
}

Main::Framework::IApplication* CreateApplication(void)
{
	if( g_pApp == NULL ) {
    gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );    
		g_pApp = GFL_NEW(systemHeap) CommonApplication();
	}
	return g_pApp;
}

//=======================================================
/*
 */
//=======================================================
void DeleteApplication(void)
{
	GFL_DELETE g_pApp;
}


} //namespace Main

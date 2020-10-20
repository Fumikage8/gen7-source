//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   CaptureTutorialMain.cpp
 *  @author ito_isao
 *
 */
//====================================================================== 
#include "CaptureTutorialMain.h"
#include "CaptureTutorialScene.h"
#include "CaptureTutorialViewer.h"

// nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  //------------------------------------------------------------------------------
  // 定数定義
  //------------------------------------------------------------------------------
  //  4M
  #define CAPTURE_DEMO_APP_HEAP_SIZE ( 1024 * 1024 * 1 )
  //  32M
  #define CAPTURE_DEMP_DEV_HEAP_SIZE ( 1024 * 1024 * 35 )

  const s32 CaptureTutorialMain::FADE_TIME   = 5;
  const s32 CaptureTutorialMain::RECORD_MAX  = 11;

  //------------------------------------------------------------------------------
  // 親となるシーン
  //------------------------------------------------------------------------------
  CaptureTutorialMain::CaptureTutorialMain( GameSys::GameManager* pGameManager , GameSys::GameProc* pProc, CaptureTutorialViewer::TutorialType type)
  {
    GFL_ASSERT( pGameManager );
    GFL_ASSERT( pProc );

    _Clear();

    m_TutorialType = type;

    m_owner = this;
  
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    //  ヒープの残り数を表示
    {
      u32 maxSize = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE )->GetTotalAllocatableSize();
      GFL_PRINT("max dev allocatable %d\n", maxSize);

      maxSize = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP )->GetTotalAllocatableSize();
      GFL_PRINT("max heap allocatable %d\n", maxSize);
    }
#endif

    // 捕獲チュートリル用の専用ヒープ作成
    {
      gfl2::heap::HeapBase* pSysHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      gfl2::heap::HeapBase* pDevHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_utilheap.LocalHeapCreate( pSysHeap, pDevHeap, CAPTURE_DEMO_APP_HEAP_SIZE, CAPTURE_DEMP_DEV_HEAP_SIZE );
    }

    m_pGameManager  = pGameManager;
    m_pProc = pProc;
  }

  CaptureTutorialMain::~CaptureTutorialMain()
  {
    DeleteViewer();

    Exit();
  }

  // バトル描画システムの作成
  void CaptureTutorialMain::CreateViewer()
  {
    GFL_ASSERT( m_pCore == NULL );

    m_pCore = GFL_NEW( m_utilheap.GetSystemHeap() ) CaptureTutorialViewer();
    m_pCore->SetUtilHeap( &m_utilheap );
    m_pCore->SetBattleProc( m_pProc );

    //戻り値を見る必要があるので外で待ちますが、1回動かしておかないとまずい
    m_pCore->Initialize( m_utilheap.GetDeviceHeap(), m_TutorialType );
  }

  //  バトル描画システムの削除
  void  CaptureTutorialMain::DeleteViewer()
  {
    GFL_SAFE_DELETE( m_pCore );
  }

  //------------------------------------------------------------------------------
  void CaptureTutorialMain::Enter()
  {
    GFL_PRINT("enter:CaptureTutorialMain\n");

    // 初期のシーンを生成
    SetNext<CaptureTutorialInitScene>();
  }

  void CaptureTutorialMain::Exit()
  {
    GFL_PRINT("exit:CaptureTutorialMain\n");
    if( m_pCurrent != NULL )
    {
      m_pCurrent->Exit();
      GFL_SAFE_DELETE( m_pCurrent );
    }
  }

  //------------------------------------------------------------------------------
  bool CaptureTutorialMain::Update()
  {
    if( m_pNext != NULL )
    {
      if( m_pCurrent != NULL )
      {
        m_pCurrent->Exit();
        GFL_SAFE_DELETE( m_pCurrent );
      }

      m_pCurrent = m_pNext;
      m_pCurrent->Enter();

      m_pNext = NULL;

      return true;
    }
  
    if( m_pCurrent != NULL )
    {
      m_pCurrent->Update();
    }

    //  バトルビューアを更新
    if( ( m_pCore != NULL ) && ( m_bViewerUpdate == true ) )
    {
      m_pCore->Update();
    }

    if( IsFinished() )
    {
      return false;
    }
  
    return true;
  }

  void CaptureTutorialMain::PreDraw()
  {
    if( m_pCore != NULL )
    {
      m_pCore->PreDraw();
    }
  }

  //------------------------------------------------------------------------------
  // 描画
  //------------------------------------------------------------------------------
  void CaptureTutorialMain::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( ( m_pCore != NULL ) && ( m_bViewerDraw == true ) )
    {
      m_pCore->Draw( displayNo );
    }
  }

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

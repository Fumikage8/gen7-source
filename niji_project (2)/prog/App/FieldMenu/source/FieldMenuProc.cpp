#if PM_DEBUG
//=============================================================================
/**
 * @file    FieldMenuProc.cpp
 * @brief   フィールドメニューProc
 * @author  Hiroyuki Nakamura
 * @date    2015.05.18
 */
//=============================================================================

// module
#include "App/FieldMenu/include/FieldMenuProc.h"
#include "App/FieldMenu/include/FieldMenuFrame.h"
// niji
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)


static const u32 MANAGER_HEAP_SIZE = 0x1000;    //!< マネージャヒープサイズ
//static const u32 LOCAL_HEAP_SIZE        = 0x50000;
//static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x180000;
//static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x500000;

enum
{
  SEQ_MENU_MAIN = 0,  //!< シーケンス：メイン
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuProc::FieldMenuProc(void)
//  : m_param( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuProc::~FieldMenuProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   外部設定パラメータをセット
 * @date    2015.05.18
 *
 * @param   param   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
/*
void FieldMenuProc::Setup( APP_PARAM * param )
{
  m_param = param;
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.05.18
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuProc::InitFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    CreateHeap();
    CreateRenderManager();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    m_pRenderingManager->Update();
    if( m_pRenderingManager->IsCreated() == false )
    {
      break;
    }
    CreateFrame();
  	return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   Proc終了処理
 * @date    2015.05.18
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuProc::EndFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    if( DeleteFrame() != false )
    {
      AddSubSeq();
    }
    break;

  case 1:
    if( DeleteRenderManager() == false )
    {
      break;
    }
    DeleteHeap();
  	return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   Procアップデート処理
 * @date    2015.05.18
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case SEQ_MENU_MAIN:
    return Update_MenuMain( pManager );
  }
	return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   Proc描画下準備
 * @date    2015.07.09
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.05.18
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::CreateHeap(void)
{
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate(
    heap,
    dev_heap,
    System::HeapSize::HEAP_FIELD_SUBSCREEN_SIZE,
    System::HeapSize::HEAP_FIELD_SUBSCREEN_DEVICE_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::DeleteHeap(void)
{
  m_heap.Delete();
  GFL_DELETE_HEAP( m_pManagerHeap );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateRenderManager
 * @brief   描画マネージャ生成
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::CreateRenderManager(void)
{
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }

  m_pRenderingManager->StartAsyncCreate(
    desc,
    &m_heap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRenderManager
 * @brief   描画マネージャ削除
 * @date    2015.07.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuProc::DeleteRenderManager(void)
{
  if( m_pRenderingManager->CanDestroy() == false )
  {
    return false;
  }
  GFL_DELETE m_pRenderingManager;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFrame
 * @brief   フレーム生成
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuProc::CreateFrame(void)
{
  // フレーム生成
  int mode = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuOpen();
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pMenuFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, FieldMenuFrame >( m_pFrameManager );
  m_pMenuFrame->Setup( &m_heap, m_pRenderingManager, static_cast<App::FieldMenu::FieldMenuFrame::StartType>(mode) );
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
#if PM_DEBUG
  m_pMenuFrame->DebugSetup();
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFrame
 * @brief   フレーム削除
 * @date    2015.07.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuProc::DeleteFrame(void)
{
  if( m_pFrameManager->End() != false )
  {
    return false;
  }
  GFL_DELETE m_pFrameManager;
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    Update_MenuMain
 * @brief   アップデート：メイン処理
 * @date    2015.05.29
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuProc::Update_MenuMain( gfl2::proc::Manager * pManager )
{
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

  switch( m_pMenuFrame->GetRequest() )
  {
  case Field::SubScreen::OUT_REQ_ID_NONE:
    break;

  case Field::SubScreen::OUT_REQ_ID_MENU_OPEN:
    
    
    m_pMenuFrame->SetRequest( Field::SubScreen::IN_REQ_ID_MENU_OPEN );
    break;

  case Field::SubScreen::OUT_REQ_ID_MENU_CLOSE:
    break;

  case Field::SubScreen::OUT_REQ_ID_POKELIST:
  case Field::SubScreen::OUT_REQ_ID_BAG:
  case Field::SubScreen::OUT_REQ_ID_ZUKAN:
  case Field::SubScreen::OUT_REQ_ID_TR_CARD:
  case Field::SubScreen::OUT_REQ_ID_CONFIG:
  case Field::SubScreen::OUT_REQ_ID_REPORT:
    break;

  case Field::SubScreen::OUT_REQ_ID_POKEFINDER:
    break;
  }

  m_pRenderingManager->Update();

  return gfl2::proc::RES_CONTINUE;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // PM_DEBUG

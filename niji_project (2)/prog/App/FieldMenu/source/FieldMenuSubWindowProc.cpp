//=============================================================================
/**
 * @file    FieldMenuSubWindowProc.cpp
 * @brief   フィールドメニューProc
 * @author  Hiroyuki Nakamura
 * @date    2015.06.04
 */
//=============================================================================

// gfl2
#include <Fade/include/gfl2_FadeManager.h>
// module
#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "App/FieldMenu/include/FieldMenuFrame.h"
// niji
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

/*
  system:  400k = 0x064000
  device: 1536k = 0x180000
*/

static const u32 MANAGER_HEAP_SIZE = 0x1000;  //!< マネージャ用ヒープサイズ

//! メインシーケンス
enum
{
  SEQ_MENU_MAIN = 0,      //!< メイン
  SEQ_REMAKE_FRAME_INIT,  //!< フレーム再生成初期化
  SEQ_REMAKE_FRAME_MAIN,  //!< フレーム再生成メイン
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuSubWindowProc::FieldMenuSubWindowProc(void)
  : m_pManagerHeap( NULL )
  , m_pRenderingManager( NULL )
  , m_pFrameManager( NULL )
  , m_pMenuFrame( NULL )
  , m_frameRemakeReq( false )
  , m_isFrameRemake( false )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuSubWindowProc::~FieldMenuSubWindowProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.06.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::InitFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case 0:
    CreateHeap();
    CreateRenderManager();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
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
 * @date    2015.06.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::EndFunc( gfl2::proc::Manager * pManager )
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
 * @date    2015.06.04
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  if( m_frameRemakeReq != false )
  {
    m_frameRemakeReq = false;
    // 再生成中は何もしない
    if( m_isFrameRemake == false )
    {
      m_isFrameRemake = true;
      SetSubSeq( SEQ_REMAKE_FRAME_INIT );
    }
  }

  switch( GetSubSeq() )
  {
  case SEQ_MENU_MAIN:
    return Update_MenuMain( pManager );

  case SEQ_REMAKE_FRAME_INIT:
    if( Update_RemakeFrameInit( pManager ) == gfl2::proc::RES_CONTINUE )
    {
      break;
    }
    SetSubSeq( SEQ_REMAKE_FRAME_MAIN );
    /* FALL THROUGH */
  case SEQ_REMAKE_FRAME_MAIN:
    return Update_RemakeFrameMain( pManager );
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
void FieldMenuSubWindowProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  m_pRenderingManager->PreDraw();
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.06.04
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFrameManager->Draw( displayNo );
  m_pRenderingManager->Draw( displayNo );
}

//-----------------------------------------------------------------------------
/**
 * @func    Resume
 * @brief   フィールド復帰　再稼働
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::Resume(void)
{
  m_pMenuFrame->SetRequest( Field::SubScreen::IN_REQ_ID_INPUT_ENABLE );   // 下画面入力有効
}

//-----------------------------------------------------------------------------
/**
 * @func    Suspend
 * @brief   フィールド削除　一時停止
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::Suspend(void)
{
  m_pMenuFrame->SetRequest( Field::SubScreen::IN_REQ_ID_INPUT_DESABLE );  // 下画面入力無効
}

//-----------------------------------------------------------------------------
/**
 * @func    GetEventRequestID
 * @brief	  起動リクエストを返す
 * @date    2015.09.03
 *
 * @return  リクエストID
 */
//-----------------------------------------------------------------------------
Field::SubScreen::FieldMenuOutRequestID FieldMenuSubWindowProc::GetEventRequestID(void)
{
  return m_pMenuFrame->GetRequest();
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ生成
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::CreateHeap(void)
{
  Field::FieldResident * field_resident = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldResident();
  gfl2::heap::HeapBase * heap     = field_resident->GetFieldSubScreenHeap();
  gfl2::heap::HeapBase * dev_heap = field_resident->GetFieldSubScreenDeviceHeap();

  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

  // app::util::Heap作成
  gfl2::heap::NwAllocator * allocator     = GFL_NEW( heap ) gfl2::heap::NwAllocator( heap );
  gfl2::heap::NwAllocator * dev_allocator = GFL_NEW( heap ) gfl2::heap::NwAllocator( dev_heap );

  gfl2::util::GLHeapAllocator * gl_allocator     = GFL_NEW( heap ) gfl2::util::GLHeapAllocator( heap );
  gfl2::util::GLHeapAllocator * dev_gl_allocator = GFL_NEW( heap ) gfl2::util::GLHeapAllocator( dev_heap );

  m_heap.Create(
    heap,
    dev_heap,
    allocator,
    dev_allocator,
    gl_allocator,
    dev_gl_allocator );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::DeleteHeap(void)
{
  {
    gfl2::heap::NwAllocator * allocator;

    allocator = m_heap.GetSystemAllocator();
    GFL_DELETE allocator;

    allocator = m_heap.GetDeviceAllocator();
    GFL_DELETE allocator;
  }
  {
    gfl2::util::GLHeapAllocator * allocator;

    allocator = m_heap.GetGLHeapAllocator();
    GFL_DELETE allocator;

    allocator = m_heap.GetDevGLHeapAllocator();
    GFL_DELETE allocator;
  }
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
void FieldMenuSubWindowProc::CreateRenderManager(void)
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
bool FieldMenuSubWindowProc::DeleteRenderManager(void)
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
void FieldMenuSubWindowProc::CreateFrame(void)
{
  int mode = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuOpen();
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pMenuFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, FieldMenuFrame >( m_pFrameManager );
  m_pMenuFrame->Setup( &m_heap, m_pRenderingManager, static_cast<App::FieldMenu::FieldMenuFrame::StartType>(mode) );
//  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(false);
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
bool FieldMenuSubWindowProc::DeleteFrame(void)
{
  if( m_pFrameManager == NULL )
  {
    return true;
  }
  if( m_pFrameManager->End() != false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pFrameManager );
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    Update_MenuMain
 * @brief   アップデート：メイン処理
 * @date    2015.06.04
 *
 * @param   pManager  プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::Update_MenuMain( gfl2::proc::Manager * pManager )
{
  if( IsReqEnd() )
  {
    SetRequest( Field::SubScreen::IN_REQ_ID_MENU_END );
//    return gfl2::proc::RES_FINISH;
  }

  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }

/*
  switch( m_pMenuFrame->GetRequest() )
  {
  case Field::SubScreen::OUT_REQ_ID_NONE:
    break;

  case Field::SubScreen::OUT_REQ_ID_MENU_OPEN:
//    m_pMenuFrame->SetRequest( Field::SubScreen::IN_REQ_ID_MENU_OPEN );
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
*/

  m_pRenderingManager->Update();

  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_RemakeFrameInit
 * @brief   アップデート：フレーム再生成初期化
 * @date    2015.09.28
 *
 * @param   pManager  プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::Update_RemakeFrameInit( gfl2::proc::Manager * pManager )
{
  SetRequest( Field::SubScreen::IN_REQ_ID_MENU_END );
  if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
  {
    return gfl2::proc::RES_FINISH;
  }
  m_pRenderingManager->Update();
  return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_RemakeFrameMain
 * @brief   アップデート：フレーム再生成メイン
 * @date    2015.09.28
 *
 * @param   pManager  プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result FieldMenuSubWindowProc::Update_RemakeFrameMain( gfl2::proc::Manager * pManager )
{
  if( DeleteFrame() != false )
  {
    if( IsReqEnd() )
    {
      return gfl2::proc::RES_FINISH;
    }
    CreateFrame();
    m_isFrameRemake = false;
    SetSubSeq( SEQ_MENU_MAIN );
  }
  return gfl2::proc::RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetRequest
 * @brief    FieldMenuFrameへ変更の指示を出す
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::SetRequest( Field::SubScreen::FieldMenuInRequestID id )
{
  if(m_pMenuFrame){
    m_pMenuFrame->SetRequest(id);
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRequest
 * @brief   FieldMenuFrameから変更の指示を得る
 */
//-----------------------------------------------------------------------------
Field::SubScreen::FieldMenuOutRequestID FieldMenuSubWindowProc::GetRequest(void)
{
  if(m_pMenuFrame){
    return m_pMenuFrame->GetRequest();
  }
  return Field::SubScreen::OUT_REQ_ID_NONE;
}


//-----------------------------------------------------------------------------
/**
 * @func    RemakeFrame
 * @brief   フレーム再生成リクエスト
 * @date    2015.09.10
 *
 * @retval  true  = リクエスト成功
 * @retval  false = リクエスト失敗
 *
 * @note  終了リクエストが発行されている場合は失敗します
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::RemakeFrame(void)
{
  if( IsReqEnd() )
  {
    return false;
  }
  m_frameRemakeReq = true;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFrameRemake
 * @brief   フレーム再生成中か
 * @date    2015.09.10
 *
 * @retval  true  = 再生成中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::IsFrameRemake(void)
{
  return m_isFrameRemake;
}


//-----------------------------------------------------------------------------
/**
 * @func    GetRideID
 * @brief   選択されたライドポケモンのIDを取得
 * @date    2015.09.16
 *
 * @return  ライドポケモンのID
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RideID FieldMenuSubWindowProc::GetRideID(void)
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->GetRideID();
  }
  return Savedata::FieldMenu::RIDE_ID_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetFieldRideID
 * @brief   フィールドのライドポケモンのIDを取得
 * @date    2015.09.16
 *
 * @param   id  下画面のライドポケモンID
 *
 * @return  フィールドのライドポケモンのID
 */
//-----------------------------------------------------------------------------
Field::RIDE_POKEMON_ID FieldMenuSubWindowProc::GetFieldRideID( Savedata::FieldMenu::RideID id )
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->GetFieldRideID( id );
  }
  return Field::RIDE_NO_POKEMON;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCreate
 * @brief   フィールドメニューの生成が終了したか
 * @date    2015.11.04
 *
 * @retval  true  = した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::IsCreate(void)
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->IsCreate();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomEvent
 * @brief   ロトムメッセージ表示イベントを開始
 * @date    2015.11.11
 *
 * @param   msg_id  メッセージID
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::SetRotomEvent( u32 msg_id )
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->SetRotomEvent( msg_id );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomEvent
 * @brief   ロトムメッセージ表示イベント中か
 * @date    2015.11.11
 *
 * @retval  true  = イベント中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::IsRotomEvent(void)
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->IsRotomEvent();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomPosEvent
 * @brief   ロトムメッセージ表示リクエスト（POSイベント用）
 * @date    2016.01.15
 *
 * @param   msg_id  メッセージID
 */
//-----------------------------------------------------------------------------
void FieldMenuSubWindowProc::SetRotomPosEvent( u32 msg_id )
{
  if( m_pMenuFrame )
  {
    m_pMenuFrame->SetRotomPosEvent( msg_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMenuDrawCreate
 * @brief   MenuDrawクラスの生成が完了したか
 * @date    2017.04.05
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 *
 * @note
 *    FieldStaffListクラス生成時に
 *    メッセージのアーカイブへの読み込みが重なってしまうため、
 *    メッセージを読み込んでいるMenuDrawクラスの完成をチェックする
 */
//-----------------------------------------------------------------------------
bool FieldMenuSubWindowProc::IsMenuDrawCreate(void) const
{
  if( m_pMenuFrame )
  {
    return m_pMenuFrame->IsMenuDrawCreate();
  }
  return false;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

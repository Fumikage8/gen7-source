//=============================================================================
/**
 * @file    FieldMenuFrame.cpp
 * @brief   フィールドメニューフレーム
 * @author  Hiroyuki Nakamura
 * @date    2015.05.18
 */
//=============================================================================

// module
#include "App/FieldMenu/include/FieldMenuFrame.h"
#include "MenuDefine.h"
#include "MenuBase.h"
#include "MenuDraw.h"
#include "NaviDraw.h"
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "App/FieldMenu/include/RideDrawParam.h"
#include "RideDraw.h"
#include "ReportDraw.h"
#include "QuickMatchDraw.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "Sound/include/Sound.h"
#include "System/include/GflUse.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/message.gaix"
#include "niji_conv_header/field/flagwork/FlagDefine.h"

#include "NetStatic/NetEvent/include/QuickMatchEvent.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! メインシーケンス
enum MainSeq
{
  SEQ_INIT = 0,
  SEQ_START,
  SEQ_MAIN,
  SEQ_OPEN_MENU,
  SEQ_CLOSE_MENU,

  SEQ_CHANGE_MENU_PAGE,
  SEQ_SLIDE_MENU_PAGE,

  SEQ_MENU_ICON_CATCH,

  SEQ_INIT_RIDE_LIST,
  SEQ_EXIT_RIDE_LIST,

  SEQ_INIT_REPORT,
  SEQ_MAIN_REPORT,
  SEQ_EXIT_REPORT,

  SEQ_INIT_QUICKMATCH,
  SEQ_EXIT_QUICKMATCH,

  SEQ_ROTOM_EVENT,

  SEQ_END
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuFrame::FieldMenuFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
  , m_startType( START_TYPE_NORMAL )
  , m_dispMode( DISP_MODE_FRAME_OFF )
  , m_gameManager( NULL )
  , m_saveFieldMenu( NULL )
  , m_saveMisc( NULL )
  , m_playData( NULL )
  , m_eventWork( NULL )
  , m_inRequestID( Field::SubScreen::IN_REQ_ID_NONE )
  , m_outRequestID( Field::SubScreen::OUT_REQ_ID_NONE )
  , m_createSeq( 0 )
  , m_subSeq( 0 )
  , m_moduleSeq( 0 )
  , m_setupFieldMenuSeq( 0 )
  , m_pMenuBase( NULL )
  , m_isMenuBaseCreate( false )
  , m_pMenuDraw( NULL )
  , m_isMenuDrawCreate( false )
  , m_pNaviDraw( NULL )
  , m_isNaviDrawCreate( false )
  , m_cursorPos( 0 )
  , m_catchScrollCount( 0 )
  , m_isMenuOpen( false )
  , m_cpyIsMenuBaseInput( false )
  , m_cpyIsMenuDrawInput( false )
  , m_cpyIsNaviDrawInput( false )
  , m_isPassive( false )
  , m_pRideDraw( NULL )
  , m_rideID( Savedata::FieldMenu::RIDE_ID_NONE )
  , m_isRideCollisionUpdate( false )
  , m_pReportDraw( NULL )
  , m_pQuickMatch( NULL )
  , m_isQuickMatchExit( false )
  , m_isCreate( false )
  , m_isRotomEvent( false )
  , m_isRotomEventStartOpen( false )
  , m_isEndEnable( false )
  , m_pRotomMsgHeap( NULL )
  , m_pMenuBaseHeap( NULL )
  , m_isStartGetRequest( false )
#if PM_DEBUG
  , m_debugModeFlag( false )
  , m_debugSysHeapSizeLow( 0xffffffff )
  , m_debugDevHeapSizeLow( 0xffffffff )
#endif  // PM_DEBUG
{
  GetGameData();
  m_isPassive = !m_playData->GetEndInputMode();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenuFrame::~FieldMenuFrame()
{
  DeleteRotomMessageHeap();
  DeleteMenuBaseHeap();
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.05.18
 *
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 * @param   type        開始タイプ
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man, StartType type )
{
  m_heap             = heap;
  m_renderingManager = render_man;
  m_startType        = type;

  if( m_startType == START_TYPE_MENU_OPEN )
  {
    m_isMenuOpen = true;
  }
  else
  {
    m_isMenuOpen = false;
  }

  CreateMenuBaseHeap();
  CreateRotomMessageHeap();
}

#if PM_DEBUG
void FieldMenuFrame::DebugSetup(void)
{
  m_debugModeFlag = true;
}
#endif  // PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.05.18
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result FieldMenuFrame::InitFunc(void)
{
  if( m_startType == START_TYPE_MENU_OPEN )
  {
    if( CreateMenuBaseEz() == false )
    {
      return applib::frame::RES_CONTINUE;
    }
    return applib::frame::RES_FINISH;
  }

  if( CreateMenuBase() != false )
  {
    if( m_startType == START_TYPE_NORMAL )
    {
      m_isCreate = true;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.05.18
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result FieldMenuFrame::EndFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    if( DeleteRideDraw() == false )
    {
      break;
    }
    if( DeleteQuickMatchDraw() == false )
    {
      break;
    }
    if( DeleteNaviDraw() == false )
    {
      break;
    }
    if( DeleteMenuDraw() == false )
    {
      break;
    }
    if( DeleteMenuBase() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    m_playData->SetCursorPos( m_cursorPos );
    m_playData->SetEndInputMode( !m_isPassive );
    return applib::frame::RES_FINISH;

  default:
    GFL_ASSERT( 0 );
    break;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.05.18
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result FieldMenuFrame::UpdateFunc(void)
{
#if PM_DEBUG
  if( DebugSetHeapSizeLow() != false )
  {
    DebugPrintHeapSizeLow( "UpdateFunc" );
  }
#endif  // PM_DEBUG

  m_outRequestID = Field::SubScreen::OUT_REQ_ID_NONE;   // メニューが発行したリクエストをクリア

  if( CheckRequest() == false )
  {
    SetTimeRotomColor();
    m_pMenuBase->UpdateTree();
    if( m_pNaviDraw != NULL )
    {
      m_pNaviDraw->MoveMapCenter();
    }
  }

  switch( GetSubSeq() )
  {
  case SEQ_INIT:
    // 通常
    if( m_startType == START_TYPE_NORMAL )
    {
      Update_SeqInit();
    }
    // メニューを開いた状態
    else if( m_startType == START_TYPE_MENU_OPEN )
    {
      Update_SeqInitMenu();
    }
    // ライド選択画面
    else
    {
      Update_SeqInitRide();
    }
    break;
  case SEQ_START:
    Update_SeqStart();
    break;
  case SEQ_MAIN:
    Update_SeqMain();
    break;
  case SEQ_OPEN_MENU:
    Update_SeqOpenMenu();
    break;
  case SEQ_CLOSE_MENU:
    Update_SeqCloseMenu();
    break;
  case SEQ_CHANGE_MENU_PAGE:
    Update_SeqChangeMenuPage();
    break;
  case SEQ_SLIDE_MENU_PAGE:
    Update_SeqSlideMenuPage();
    break;
  case SEQ_MENU_ICON_CATCH:
    Update_SeqMenuIconCatch();
    break;

  case SEQ_INIT_RIDE_LIST:
    Update_SeqInitRideList();
    break;
  case SEQ_EXIT_RIDE_LIST:
    Update_SeqExitRideList();
    break;

  case SEQ_INIT_REPORT:
    Update_SeqInitReport();
    break;
  case SEQ_MAIN_REPORT:
    Update_SeqMainReport();
    break;
  case SEQ_EXIT_REPORT:
    Update_SeqExitReport();
    break;

  case SEQ_INIT_QUICKMATCH:
    Update_SeqInitQuickMatch();
    break;
  case SEQ_EXIT_QUICKMATCH:
    Update_SeqExitQuickMatch();
    break;

  case SEQ_ROTOM_EVENT:
    Update_SeqRotomEvent();
    break;

  case SEQ_END:
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.05.18
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pMenuBase->DrawTree( no );
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    IsCreate
 * @brief   最低限の生成が完了したか
 * @date    2015.11.04
 *
 * @retval  true  = 完了した
 * @retval  false = それ以外
 *
 * @note  trueを返すまでフェードイン禁止。
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::IsCreate(void)
{
  return m_isCreate;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequest
 * @brief   下画面にリクエストを発行
 * @date    2015.05.25
 *
 * @param   id  リクエストID
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetRequest( Field::SubScreen::FieldMenuInRequestID id )
{
  m_inRequestID = id;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRequest
 * @brief   下画面からのリクエストを取得
 * @date    2015.05.25
 *
 * @return  リクエストID
 */
//-----------------------------------------------------------------------------
Field::SubScreen::FieldMenuOutRequestID FieldMenuFrame::GetRequest(void)
{
  m_isStartGetRequest = true;
  return m_outRequestID;
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetRequest
 * @brief   下画面からのリクエストを削除
 * @date    2015.05.25
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ResetRequest(void)
{
  m_outRequestID = Field::SubScreen::OUT_REQ_ID_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckRequest
 * @brief   外部からのリクエストをチェック
 * @date    2015.06.09
 *
 * @retval  true  = リクエストあり
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CheckRequest(void)
{
  switch( m_inRequestID )
  {
  case Field::SubScreen::IN_REQ_ID_MENU_END:    // モジュール終了
    if( IsSetEnd() == false )
    {
      break;
    }
    SetEnd();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    return true;

  case Field::SubScreen::IN_REQ_ID_MENU_OPEN:     // メニューを開く
    PlaySE( SEQ_SE_OPEN1 );
    OpenMenu();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
#if PM_DEBUG
    DebugPrintHeapSizeLow( "MENU_OPEN" );
#endif  // PM_DEBUG
    break;

  case Field::SubScreen::IN_REQ_ID_INPUT_DESABLE: // 入力を無効にする
    SetInputEnable( false );
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_INPUT_ENABLE:  // 入力を有効にする
    SetInputEnable( true );
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_RIDE_OPEN:     // ライドポケモンのリストを開く
    PlaySE( SEQ_SE_DECIDE1 );
    OpenRideList();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_REPORT_OPEN:   // レポート画面を開く
    OpenReport();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_QUICKMATCH_OPEN: // クイックマッチ下画面を開く
    OpenQuickMatch();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_INPUT_ENABLE_MENU_OPEN:  // 入力を有効にしてメニューを開く
    SetInputEnable( true );
    OpenMenu();
    PlaySE( SEQ_SE_OPEN1 );
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_CALL_APP:  // アプリが呼ばれた
    // 入力停止（パッシブ表示にはしない）
    if( m_pMenuBase != NULL && m_isMenuBaseCreate != false )
    {
      m_pMenuBase->SetInputEnabled( false );
    }
    if( m_pMenuDraw != NULL && m_isMenuDrawCreate != false )
    {
      m_pMenuDraw->SetInputEnabled( false );
    }
    if( m_pNaviDraw != NULL && m_isNaviDrawCreate != false )
    {
      m_pNaviDraw->SetInputEnabled( false );
    }
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_START_ROTOPON:   // ロトポン開始
    StartRotopon();
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;

  case Field::SubScreen::IN_REQ_ID_ROTOPON_ITEM_EVENT_RETURN: // ロトポン道具入手スクリプト終了
    m_pMenuBase->SetRotoponItemScript( false );
    m_inRequestID = Field::SubScreen::IN_REQ_ID_NONE;
    break;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDispMask
 * @brief   マスク表示切り替え
 * @date    2015.11.09
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetDispMask( bool flg )
{
  if( m_isPassive == false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pMenuBase->SetVisiblePassivePane( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputEnable
 * @brief   下画面全体の入力制御切り替え
 * @date    2015.06.09
 *
 * @param   flg   true = 許可, false = 不許可
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetInputEnable( bool flg )
{
  if( flg == false )
  {
    // すでに止めている場合は何もしない
    if( m_isPassive == false )
    {
      if( m_pMenuBase != NULL && m_isMenuBaseCreate != false )
      {
        m_cpyIsMenuBaseInput = m_pMenuBase->IsInputEnabled();
        m_pMenuBase->SetInputEnabled( false );
        m_pMenuBase->SetVisiblePassivePane( true );
      }
      if( m_pMenuDraw != NULL && m_isMenuDrawCreate != false )
      {
        m_cpyIsMenuDrawInput = m_pMenuDraw->IsInputEnabled();
        m_pMenuDraw->SetInputEnabled( false );
        m_pMenuDraw->ForceEndCatchMenuIcon();
        m_pMenuDraw->ForceEndSlideMenuPageScroll();
      }
      if( m_pNaviDraw != NULL && m_isNaviDrawCreate != false )
      {
        m_cpyIsNaviDrawInput = m_pNaviDraw->IsInputEnabled();
        m_pNaviDraw->SetInputEnabled( false );
      }
      m_isPassive = true;
    }
  }
  else
  {
    // 止めてない場合は復帰しない
    if( m_isPassive != false )
    {
      if( m_pMenuBase != NULL && m_isMenuBaseCreate != false )
      {
        m_pMenuBase->SetInputEnabled( m_cpyIsMenuBaseInput );
        m_pMenuBase->SetVisiblePassivePane( false );
      }
      if( m_pMenuDraw != NULL && m_isMenuDrawCreate != false )
      {
        m_pMenuDraw->SetInputEnabled( m_cpyIsMenuDrawInput );
      }
      if( m_pNaviDraw != NULL && m_isNaviDrawCreate != false )
      {
        m_pNaviDraw->SetInputEnabled( m_cpyIsNaviDrawInput );
      }
      m_isPassive = false;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuBaseInputEnable
 * @brief   MenuBaseクラスの入力制御切り替え
 * @date    2015.06.19
 *
 * @param   flg   true = 許可, false = 不許可
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetMenuBaseInputEnable( bool flg )
{
  if( m_isPassive == false )
  {
    m_pMenuBase->SetInputEnabled( flg );
  }
  else
  {
    m_cpyIsMenuBaseInput = flg;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuDrawInputEnable
 * @brief   MenuDrawクラスの入力制御切り替え
 * @date    2015.06.19
 *
 * @param   flg   true = 許可, false = 不許可
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetMenuDrawInputEnable( bool flg )
{
  if( m_isPassive == false )
  {
    m_pMenuDraw->SetInputEnabled( flg );
  }
  else
  {
    m_cpyIsMenuDrawInput = flg;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNaviDrawInputEnable
 * @brief   NaviDrawクラスの入力制御切り替え
 * @date    2015.06.19
 *
 * @param   flg   true = 許可, false = 不許可
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetNaviDrawInputEnable( bool flg )
{
  if( m_isPassive == false && m_pNaviDraw != NULL )
  {
    m_pNaviDraw->SetInputEnabled( flg );
  }
  else
  {
    m_cpyIsNaviDrawInput = flg;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PlaySE
 * @brief   SE再生
 * @date    2015.06.23
 *
 * @parma   id  SEのID
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::PlaySE( u32 id )
{
  if( m_isPassive == false )
  {
    Sound::PlaySE( id );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateRotomMessageHeap
 * @brief   ロトムメッセージ用のヒープを生成
 * @date    2016.01.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CreateRotomMessageHeap(void)
{
  m_pRotomMsgHeap = GFL_NEW(m_heap->GetSystemHeap()) app::util::Heap();
  m_pRotomMsgHeap->LocalHeapCreate( m_heap->GetDeviceHeap(), m_heap->GetDeviceHeap(), ROTOM_MSG_HEAP_SIZE, ROTOM_MSG_DEVICE_HEAP_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRotomMessageHeap
 * @brief   ロトムメッセージ用のヒープを削除
 * @date    2016.01.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::DeleteRotomMessageHeap(void)
{
  if( m_pRotomMsgHeap != NULL )
  {
    m_pRotomMsgHeap->Delete();
    GFL_SAFE_DELETE( m_pRotomMsgHeap );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuBaseHeap
 * @brief   MenuBaseクラス用のヒープを生成
 * @date    2016.04.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CreateMenuBaseHeap(void)
{
  m_pMenuBaseHeap = GFL_NEW(m_heap->GetSystemHeap()) app::util::Heap();
  m_pMenuBaseHeap->LocalHeapCreate( m_heap->GetDeviceHeap(), m_heap->GetDeviceHeap(), MENUBASE_HEAP_SIZE, MENUBASE_DEVICE_HEAP_SIZE );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuBaseHeap
 * @brief   MenuBaseクラス用のヒープを削除
 * @date    2016.04.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::DeleteMenuBaseHeap(void)
{
  if( m_pMenuBaseHeap != NULL )
  {
    m_pMenuBaseHeap->Delete();
    GFL_SAFE_DELETE( m_pMenuBaseHeap );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.05.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::GetGameData(void)
{
  m_gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  GameSys::GameData * gd = m_gameManager->GetGameData();
  m_saveFieldMenu = gd->GetFieldMenu();
  m_saveMisc      = gd->GetMisc();
  m_playData      = gd->GetFieldMenuPlayData();
  m_eventWork     = gd->GetEventWork();

  m_cursorPos = m_playData->GetCursorPos();

  Field::ZoneID zoneID = gd->GetStartLocation()->GetZoneID();
  void* pZoneData = NULL;
  if( zoneID != Field::ZONEID_ERROR)
  {
    pZoneData = gd->GetFieldZoneDataLoader()->GetZoneData( zoneID);
  }
  Field::ZoneDataAccessor zoneDataAccessor( pZoneData);

  // 表示モードの設定
  m_dispMode = DISP_MODE_FRAME_OFF;
  if( (pZoneData != NULL) && (zoneDataAccessor.IsSubscreenSleep() != false) )
  {
    m_dispMode = DISP_MODE_SLEEP;
    m_saveFieldMenu->SetVisibleRotom( false );  // @fix NMCat[3392]: スリープモード時はロトムメッセージを強制OFF
  }
  else if( m_eventWork->CheckEventFlag(SYS_FLAG_BOUKEN_NAVI_GET) != false )
  {
    m_dispMode = DISP_MODE_NORMAL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    AddSubModuleView
 * @brief   登録順を一定にしてViewを登録
 * @date    2015.07.15
 *
 * @param   view  登録するView
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::AddSubModuleView( app::ui::UIView * view )
{
  bool rm_draw = false;

  if( m_pMenuDraw != NULL )
  {
    if( m_pMenuBase->IsExistSubView( m_pMenuDraw ) != false )
    {
      m_pMenuDraw->RemoveFromSuperView();
      rm_draw = true;
    }
  }

  m_pMenuBase->AddSubView( view );

  if( rm_draw != false )
  {
    m_pMenuBase->AddSubView( m_pMenuDraw );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuBaseEz
 * @brief   MenuBaseクラス生成（簡易版）
 * @date    2016.04.19
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateMenuBaseEz(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pMenuBase = GFL_NEW(m_heap->GetSystemHeap()) MenuBase( m_pMenuBaseHeap, m_renderingManager, &m_dispMode, &m_isPassive );
    m_pMenuBase->OpenLayoutFile();
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuBase->IsOpenLayoutFile() == false )
    {
      break;
    }
    m_createSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMenuBaseSync
 * @brief   MenuBaseクラス生成（同期版）
 * @date    2016.04.21
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::InitMenuBaseSync(void)
{
  if( m_isMenuBaseCreate == false )
  {
    m_pMenuBase->SetFrameListener( this );
    if( m_pMenuBase->InitializeSync() == false )
    {
      GFL_ASSERT( 0 );
    }
    m_pMenuBase->CreateRotomMsgWindow( m_pRotomMsgHeap );
    m_isMenuBaseCreate = true;

    if( m_dispMode == DISP_MODE_SLEEP )
    {
      m_pMenuBase->SetRotomSleepAnime();
    }
    else if( m_dispMode == DISP_MODE_NORMAL )
    {
      // 進化のお願いが発生している場合
      if( m_playData->IsRotomRequestEvolution() != false )
      {
        m_pMenuBase->SetRequestRotomMessageEvolution();
      }
      // ゲーム開始時のロトムメッセージが表示されている場合
      if( m_playData->IsEndContinueRotomMessage() != false )
      {
        m_pMenuBase->SetRequestRotomMessageReport();
      }
    }

    SetTimeRotomColor();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuBase
 * @brief   MenuBaseクラス生成
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateMenuBase(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pMenuBase = GFL_NEW(m_heap->GetSystemHeap()) MenuBase( m_pMenuBaseHeap, m_renderingManager, &m_dispMode, &m_isPassive );
    m_pMenuBase->SetFrameListener( this );
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuBase->Initialize() == false )
    {
      break;
    }
    m_pMenuBase->CreateRotomMsgWindow( m_pRotomMsgHeap );
    if( m_dispMode == DISP_MODE_NORMAL )
    {
      // 進化のお願いが発生している場合
      if( m_playData->IsRotomRequestEvolution() != false )
      {
        m_pMenuBase->SetRequestRotomMessageEvolution();
      }
      // ゲーム開始時のロトムメッセージが表示されている場合
      if( m_playData->IsEndContinueRotomMessage() != false )
      {
        m_pMenuBase->SetRequestRotomMessageReport();
      }
    }

    SetTimeRotomColor();

    m_isMenuBaseCreate = true;
    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuBase
 * @brief   MenuBaseクラス削除
 * @date    2016.01.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteMenuBase(void)
{
  if( m_pMenuBase == NULL )
  {
    return true;
  }

  m_pMenuBase->DeleteRotomMsgWindow();

  if( m_pMenuBase->Terminate() == false )
  {
    return false;
  }

  /*
    @fix MMCat[310]:
      特定のメッセージを消す。
      消したことによって表示するメッセージがなくなった場合は、
      次に下画面を生成したときのためにメッセージ非表示状態にする。
  */
  m_pMenuBase->DeleteRotomMessage();
  if( m_playData->GetEnableRottomMsgIndex() == PlayData::ROTOM_MSG_MAX )
  {
    m_saveFieldMenu->SetVisibleRotom( false );
  }

  GFL_SAFE_DELETE( m_pMenuBase );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuDraw
 * @brief   MenuDrawクラス生成
 * @date    2015.05.21
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateMenuDraw(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pMenuDraw = GFL_NEW(m_heap->GetSystemHeap()) MenuDraw( m_heap, m_renderingManager, &m_isMenuOpen, &m_dispMode );
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuDraw->LoadLayoutResource() == false )
    {
      break;
    }
    m_createSeq++;
    /* FALL THROUGH */
  case 2:
    if(m_pMenuDraw->LoadMessage() == false )
    {
      break;
    }
    m_createSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_pMenuDraw->CreateMenuCursor() == false )
    {
      break;
    }
    m_pMenuDraw->Initialize2D();
    m_createSeq++;
    break;

  case 4:
    m_pMenuDraw->CreateLayout();
    m_createSeq++;
    break;

  case 5:
    m_pMenuDraw->CreateMenuCursorLayout();
    m_createSeq++;
    break;

  case 6:
    if( m_pMenuDraw->CreateCommonIcon() == false )
    {
      break;
    }
    m_pMenuDraw->Init();
    m_pMenuDraw->SetFrameListener( this );
    m_pMenuBase->SetRotomMsgData( m_pMenuDraw->GetRotomMsgData() );
    m_pMenuBase->AddSubView( m_pMenuDraw );
    m_isMenuDrawCreate = true;
    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuDraw
 * @brief   MenuDrawクラス削除
 * @date    2015.05.21
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteMenuDraw(void)
{
  if( m_pMenuDraw == NULL )
  {
    return true;
  }

  m_isMenuDrawCreate = false;   // @note 参照されるので先に落とす

  m_pMenuBase->SetRotomMsgData( NULL );  // 消される前にセット

  m_pMenuDraw->RemoveFromSuperView();

  if( m_pMenuDraw->End() == false )
  {
    return false;
  }

  m_pMenuDraw->DeleteCommonIcon();

  if( m_pMenuDraw->DeleteMenuCursor() == false )
  {
    return false;
  }
  m_pMenuDraw->UnloadMessage();
  m_pMenuDraw->UnloadLayoutResource();
  GFL_SAFE_DELETE( m_pMenuDraw );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateNaviDraw
 * @brief   NaviDrawクラス生成
 * @date    2015.05.29
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateNaviDraw(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pNaviDraw = GFL_NEW(m_heap->GetSystemHeap()) NaviDraw( m_heap, m_renderingManager, &m_isMenuOpen, &m_isPassive, &m_dispMode, m_pMenuBase->GetMapCenterPane() );
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pNaviDraw->Initialize() == false )
    {
      break;
    }

    // ファインダーアイコン
    m_pNaviDraw->ChangePosIcon( static_cast<NaviDraw::PosIconMode>(GetNaviPosIcon()) );

    if( m_dispMode == DISP_MODE_NORMAL )
    {
      if( m_playData->IsEndContinueRotomMessage() == false )
      {
        m_playData->SetEndContinueRotomMessage();
        if( System::GflUse::GetPublicRand(100) < START_ROTOM_FREE_TALK_VALUE )
        {
          m_pMenuBase->SetRequestRotomMessageFreeTalk();
        }
        else
        {
          SetRequestRotomMessage();
        }
      }

      m_pMenuBase->SetRotomMessageUpdateEnable( true );
    }

    m_pNaviDraw->SetFrameListener( this );

    AddSubModuleView( m_pNaviDraw );

    m_isNaviDrawCreate = true;

    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteNaviDraw
 * @brief   NaviDrawクラス削除
 * @date    2015.05.29
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteNaviDraw(void)
{
  if( m_pNaviDraw == NULL )
  {
    return true;
  }

  m_isNaviDrawCreate = false;   // @note 参照されるので先に落とす

  m_pNaviDraw->RemoveFromSuperView();

  if( m_pNaviDraw->Terminate() == false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pNaviDraw );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateRideDraw
 * @brief   RideDrawクラス生成
 * @date    2015.07.15
 *
 * @param   is_frame_start  true = 下画面初期化時（ライド説明画面からの復帰時）
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateRideDraw( bool is_frame_start )
{
  switch( m_createSeq )
  {
  case 0:
    // 通常起動はパラメータを生成
    if( is_frame_start == false )
    {
      RIDE_PARAM param;
      param.on_id = GetMenuRideID( Field::EventPokemonRideTool::GetPokemonRideOnID(m_gameManager) );
      for( u32 i=0; i<Savedata::FieldMenu::RIDE_ID_MAX; i++ )
      {
        Savedata::FieldMenu::RideID ride_id = static_cast<Savedata::FieldMenu::RideID>( i );
        if( ride_id == param.on_id )
        {
          param.check[i] = IsRideOffEnable( ride_id );
        }
        else
        {
          param.check[i] = IsRideOnEnable( ride_id );
        }
      }
      m_playData->SetRideParam( &param );
      m_isRideCollisionUpdate = false;
    }
    // 情報画面から復帰した場合
    else
    {
      m_isRideCollisionUpdate = true;
    }
    m_pRideDraw = GFL_NEW(m_heap->GetSystemHeap()) RideDraw( m_heap, m_renderingManager, m_playData->GetRideParam(), m_pMenuBase->GetUpperLayout(), m_pMenuBase->GetMultiResID() );
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pRideDraw->Initialize() == false )
    {
      break;
    }
    m_pRideDraw->SetSelectListener( this );

//    AddSubModuleView( m_pRideDraw );
    m_pMenuBase->AddSubView( m_pRideDraw );

    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRideDraw
 * @brief   RideDrawクラス削除
 * @date    2015.07.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteRideDraw(void)
{
  if( m_pRideDraw == NULL )
  {
    return true;
  }
  m_pRideDraw->RemoveFromSuperView();
  if( m_pRideDraw->Terminate() == false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pRideDraw );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateReportDraw
 * @brief   ReportDrawクラス生成
 * @date    2015.08.18
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateReportDraw(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pReportDraw = GFL_NEW(m_heap->GetSystemHeap()) ReportDraw( ReportDraw::CALL_MODE_NORMAL, m_heap, m_renderingManager, NULL );
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pReportDraw->Initialize() == false )
    {
      break;
    }
//    AddSubModuleView( m_pReportDraw );
    m_pMenuBase->AddSubView( m_pReportDraw );
    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteReportDraw
 * @brief   ReportDrawクラス削除
 * @date    2015.08.18
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteReportDraw(void)
{
  if( m_pReportDraw == NULL )
  {
    return true;
  }
  m_pReportDraw->RemoveFromSuperView();
  if( m_pReportDraw->Terminate() == false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pReportDraw );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateQuickMatchDraw
 * @brief   QuickMatchDrawクラス生成
 * @date    2015.09.09
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateQuickMatchDraw(void)
{
  switch( m_createSeq )
  {
  case 0:
    m_pQuickMatch = GFL_NEW(m_heap->GetSystemHeap()) QuickMatchDraw( m_heap, m_renderingManager );
    if( m_gameManager )
    {
      m_gameManager->GetGameData()->GetPlayerStatus()->SetQuickMatchType( NetEvent::QuickMatch::MATCH_TYPE_NORMAL );
    }
    m_createSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pQuickMatch->Initialize() == false )
    {
      break;
    }
    m_pQuickMatch->SetListener( this );
//    AddSubModuleView( m_pQuickMatch );
    m_pMenuBase->AddSubView( m_pQuickMatch );
    m_createSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteQuickMatchDraw
 * @brief   QuickMatchDrawクラス削除
 * @date    2015.09.09
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteQuickMatchDraw(void)
{
  if( m_pQuickMatch == NULL )
  {
    return true;
  }
  m_pQuickMatch->RemoveFromSuperView();
  if( m_pQuickMatch->Terminate() == false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pQuickMatch );
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSubSeq
 * @brief   アップデートのシーケンス切り替え
 * @date    2015.05.22
 *
 * @param   main  メインシーケンス
 * @param   sub   サブシーケンス
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ChangeSubSeq( u32 main, u32 sub )
{
  m_subSeq = sub;
  SetSubSeq( main );
  SetEndEnableSequence( main );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnd
 * @brief   下画面終了をセット
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetEnd(void)
{
  ChangeSubSeq( SEQ_END );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSetEnd
 * @brief   下画面を終了できるか
 * @date    2015.09.28
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::IsSetEnd(void)
{
/*
  int man_seq = GetSubSeq();

  if( man_seq == SEQ_INIT ||
      man_seq == SEQ_INIT_RIDE_LIST ||
      man_seq == SEQ_EXIT_RIDE_LIST ||
      man_seq == SEQ_INIT_REPORT ||
      man_seq == SEQ_MAIN_REPORT ||
      man_seq == SEQ_EXIT_REPORT ||
      man_seq == SEQ_INIT_QUICKMATCH ||
      man_seq == SEQ_EXIT_QUICKMATCH )
  {
    return false;
  }
  return true;
*/
  return m_isEndEnable;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEndEnable
 * @brief   終了設定制御フラグをセット
 * @date    2016.01.09
 *
 * @param   flg   true = 終了可能, false = 終了不可
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetEndEnable( bool flg )
{
  m_isEndEnable = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEndEnableSequence
 * @brief   アップデートシーケンスをチェックして終了設定制御フラグをセット
 * @date    2016.01.09
 *
 * @param   seq   アップデートシーケンス
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetEndEnableSequence( u32 seq )
{
  if( seq == SEQ_INIT ||
      seq == SEQ_INIT_RIDE_LIST ||
      seq == SEQ_EXIT_RIDE_LIST ||
      seq == SEQ_INIT_REPORT ||
      seq == SEQ_MAIN_REPORT ||
      seq == SEQ_EXIT_REPORT ||
      seq == SEQ_INIT_QUICKMATCH ||
      seq == SEQ_EXIT_QUICKMATCH )
  {
    SetEndEnable( false );
  }
  else
  {
    SetEndEnable( true );
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    CreateFieldMenu
 * @brief   フィールドメニュー生成
 * @date    2015.08.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 *
 * @note  下画面アプリからの復帰に使用
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CreateFieldMenu(void)
{
  switch( m_setupFieldMenuSeq )
  {
  case 0:
    if( CreateMenuDraw() == false )
    {
      break;
    }
    m_setupFieldMenuSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_dispMode == DISP_MODE_SLEEP )
    {
      m_pMenuBase->SetRotomSleepAnime();
    }
    else if( m_dispMode != DISP_MODE_FRAME_OFF )
    {
      if( CreateNaviDraw() == false )
      {
        break;
      }
/*
      if( m_isMenuOpen == false )
      {
        bool is_enable = m_saveFieldMenu->IsVisibleRotom();
        m_pMenuBase->SetRotomAnime( is_enable, true );
        if( is_enable != false )
        {
          m_pMenuBase->SetVisibleButton( false );
        }
      }
*/
    }
    m_setupFieldMenuSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pMenuDraw->IsMenuIconAnimeEnd() == false )
    {
      break;
    }
    if( m_dispMode == DISP_MODE_SLEEP )
    {
      if( m_pMenuBase->IsRotomSleepAnimeEnd() == false )
      {
        break;
      }
    }
    else if( m_dispMode != DISP_MODE_FRAME_OFF )
    {
      if( m_pMenuBase->IsRotomAnimeEnd() == false )
      {
        break;
      }
      if( m_pMenuBase->IsMessageAnime() == false )
      {
        break;
      }
    }
    m_setupFieldMenuSeq++;
    /* FALL THROUGH */
  case 3:
    if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
    {
      m_pNaviDraw->Start();
      SetNaviDrawInputEnable( true );

      m_pMenuDraw->Stop();
      SetMenuDrawInputEnable( false );
    }
    if( m_isMenuOpen == false )
    {
      ChangeSubSeq( SEQ_MAIN );
      SetMenuBaseInputEnable( true );
      m_pMenuBase->SetVisibleCover( true );

      // @fix NMCat[3710]: 通常時以外はロトムの処理をしない
      if( m_dispMode == DISP_MODE_NORMAL )
      {
        bool is_enable = m_saveFieldMenu->IsVisibleRotom();
        m_pMenuBase->SetRotomAnime( is_enable, true );
        if( is_enable != false )
        {
          m_pMenuBase->SetVisibleButton( false );
          m_pMenuBase->SetRotomAnime( true, false );
        }
        m_pMenuBase->SetRotomMessageUpdateEnable( true );
      }
    }
    else
    {
      OpenMenu();
    }

    if( m_isPassive != false )
    {
      m_pMenuBase->SetVisiblePassivePane( true );
    }

    m_setupFieldMenuSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFieldMenu
 * @brief   フィールドメニュー削除
 * @date    2015.08.24
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 *
 * @note  下画面アプリ起動のために一部を削除
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::DeleteFieldMenu(void)
{
  if( DeleteNaviDraw() == false )
  {
    return false;
  }
  if( DeleteMenuDraw() == false )
  {
    return false;
  }
  return true;
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInit
 * @brief   アップデート：初期化
 * @date    2016.01.09
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInit(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( CreateMenuDraw() == false )
    {
      break;
    }

    if( m_dispMode != DISP_MODE_NORMAL )
    {
      m_playData->SetEndContinueRotomMessage();
    }

    if( m_dispMode == DISP_MODE_FRAME_OFF || m_dispMode == DISP_MODE_SLEEP )
    {
      {
        GFL_PRINT(
          "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetSystemHeap()->GetTotalFreeSize(),
          m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
        GFL_PRINT(
          "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
          m_heap->GetDeviceHeap()->GetTotalFreeSize(),
          m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
      }
      ChangeSubSeq( SEQ_START );
      break;
    }
    // 終了リクエストがきていたら終了可能にする
    if( m_inRequestID == Field::SubScreen::IN_REQ_ID_MENU_END )
    {
      SetEndEnable( true );
      m_subSeq = 0;   // @note 次のフレームではこの関数にこないはず
      break;
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateNaviDraw() == false )
    {
      break;
    }

    { // @fix MMCat[517]: イベントメッセージを更新
      u32 event_text = m_pNaviDraw->GetMainEventMsgID();
      if( event_text != NaviDraw::EVENT_MSG_NONE )
      {
        u32 event_value = m_pNaviDraw->GetMainEventWorkValue();
        if( event_value != PlayData::ROTOM_WORK_NONE )
        {
          u32 rotom_work = m_playData->GetRotomMessageWork( PlayData::ROTOM_MSG_EVENT );
          if( rotom_work != PlayData::ROTOM_WORK_NONE && rotom_work != event_value )
          {
            m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_EVENT, event_text, event_value );
          }
        }
      }
    }

    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    ChangeSubSeq( SEQ_START );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInitMenu
 * @brief   アップデート：メニューを開いた状態で初期化
 * @date    2016.02.13
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInitMenu(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( CreateMenuDraw() == false )
    {
      break;
    }
    if( m_dispMode != DISP_MODE_NORMAL )
    {
      m_playData->SetEndContinueRotomMessage();
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    m_pMenuDraw->EndPageChangeAnime( m_saveFieldMenu->GetMenuPage() );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pMenuDraw->IsMenuIconAnimeEnd() == false )
    {
      break;
    }
    OpenMenu();
    m_isCreate = true;

    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInitRide
 * @brief   アップデート：ライド選択画面を開いた状態で初期化
 * @date    2016.03.30
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInitRide(void)
{
  switch( m_subSeq )
  {
  case 0:
    SetMenuBaseInputEnable( false );
    m_pMenuBase->SetVisibleLayout( true );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateRideDraw(true) == false )
    {
      break;
    }
    m_pRideDraw->SetInputEnabled( false );
    m_isCreate = true;
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_isStartGetRequest == false )
    {
      break;
    }
    m_pRideDraw->SetInputEnabled( true );
    m_subSeq = 0;
    ChangeSubSeq( SEQ_MAIN );
    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.05.22
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqStart(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMenuDraw->EndPageChangeAnime( m_saveFieldMenu->GetMenuPage() );
    if( m_dispMode == DISP_MODE_SLEEP )
    {
      m_pMenuBase->SetRotomSleepAnime();
    }
    else if( m_dispMode != DISP_MODE_FRAME_OFF )
    {
      if( m_isMenuOpen == false )
      {
        bool is_enable = m_saveFieldMenu->IsVisibleRotom();
        m_pMenuBase->SetRotomAnime( is_enable, true );
        if( is_enable != false )
        {
          m_pMenuBase->SetVisibleButton( false );
        }
      }
    }
    m_subSeq++;
    break;

  case 1:
    if( m_pMenuDraw->IsMenuIconAnimeEnd() == false )
    {
      break;
    }
    if( m_dispMode == DISP_MODE_SLEEP )
    {
      if( m_pMenuBase->IsRotomSleepAnimeEnd() == false )
      {
        break;
      }
    }
    else if( m_dispMode != DISP_MODE_FRAME_OFF )
    {
      if( m_pMenuBase->IsRotomAnimeEnd() == false )
      {
        break;
      }
      if( m_pMenuBase->IsMessageAnime() == false )
      {
        break;
      }
    }

    SetMenuBaseInputEnable( true );

    if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
    {
      m_pNaviDraw->Start();
      SetNaviDrawInputEnable( true );

      m_pMenuDraw->Stop();
      SetMenuDrawInputEnable( false );
    }

    if( m_isMenuOpen == false )
    {
      ChangeSubSeq( SEQ_MAIN );
      SetMenuBaseInputEnable( true );
      m_pMenuBase->SetVisibleCover( true );
    }
    else
    {
      OpenMenu();
    }

    if( m_isPassive != false )
    {
      m_pMenuBase->SetVisiblePassivePane( true );
    }

    m_isCreate = true;

    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMain
 * @brief   アップデート：メイン処理
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqMain(void)
{
  if( m_isMenuBaseCreate == false )
  {
    return;
  }

  if( m_pNaviDraw != NULL && m_isNaviDrawCreate != false )
  {
    if( m_pMenuBase->IsRotomMainEventUpdate() == false )
    { // メインイベントを監視してメッセージをセット
      u32 msg = m_pNaviDraw->GetMainEventMsgID();
      if( msg != NaviDraw::EVENT_MSG_NONE )
      {
        u32 event_value = m_pNaviDraw->GetMainEventWorkValue();
        if( event_value != PlayData::ROTOM_WORK_NONE )
        {
          if( m_saveMisc->IsEventFirstCheck(event_value) == false )
          {
            m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_EVENT, msg, event_value );
          }
        }
      }
    }

    // ファインダーアイコン
    m_pNaviDraw->ChangePosIcon( static_cast<NaviDraw::PosIconMode>(GetNaviPosIcon()) );
  }

  // ロトムメッセージ表示
  // @fix NMCat[3392]: 通常時以外はメッセージ処理をしない
  if( m_dispMode == DISP_MODE_NORMAL )
  {
    // 会話中じゃない、ロトポン中じゃない
    if( m_saveFieldMenu->IsVisibleRotom() == false &&
        m_pMenuBase->IsUpdateRotopon() == false )
    {
      m_pMenuBase->SetEnableButton( true );  // タウンマップボタンの入力切り替え

      // メッセージあり
      if( m_playData->GetEnableRottomMsgIndex() != PlayData::ROTOM_MSG_MAX )
      {
        SetRotomMessage();
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqOpenMenu
 * @brief   アップデート：メニューを開く
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqOpenMenu(void)
{
  m_pMenuDraw->SetVisibleCursor( true );
  m_pMenuDraw->PutCursor( m_saveFieldMenu->GetMenuIconID(m_saveFieldMenu->GetMenuPage(),m_cursorPos) );
  m_pMenuDraw->ChangeButtonEnable( true );
  SetMenuDrawInputEnable( true );
  ChangeSubSeq( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqCloseMenu
 * @brief   アップデート：メニューを閉じる
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqCloseMenu(void)
{
  switch( m_subSeq )
  {
  case 0:
    // 通常
    if( !( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP && m_pNaviDraw == NULL ) )
    {
      break;
    }
    SetEndEnable( false );
    m_subSeq++;
  case 1:
    if( CreateNaviDraw() == false )
    {
      return;
    }
    m_pNaviDraw->Start();
    SetEndEnable( true );
    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
  }

  m_isMenuOpen = false;
  m_outRequestID = Field::SubScreen::OUT_REQ_ID_MENU_CLOSE;
  SetMenuBaseInputEnable( true );
  m_pMenuDraw->ChangeButtonEnable( false );
  SetMenuDrawInputEnable( false );
  if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
  {
    SetNaviDrawInputEnable( true );
  }
  ChangeSubSeq( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqChangeMenuPage
 * @brief   アップデート：メニューのページ切り替え
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqChangeMenuPage(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_pMenuDraw->IsPageChangeAnimeEnd() != false )
    {
      m_subSeq++;
    }
    break;

  case 1:
    {
      u32 page = EndChangePage();
      m_pMenuDraw->SetVisibleCursor( m_isMenuOpen );
      SetMenuDrawInputEnable( true );
      if( m_isMenuOpen != false )
      {
        m_pMenuDraw->ChangePageArrowVisible( page );
        m_pMenuDraw->SetVisibleIconArea( true );
      }
      else
      {
        SetMenuBaseInputEnable( true );
      }
      ChangeSubSeq( SEQ_MAIN );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqSlideMenuPage
 * @brief   アップデート：スライド操作でのメニューページ切り替え
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqSlideMenuPage(void)
{
  if( m_pMenuDraw->UpdateSlideMenuPageScroll() == false )
  {
    return;
  }

  u32 page = EndChangePage();
  m_pMenuDraw->SetForcePagePointerAnime();
  m_pMenuDraw->SetVisibleCursor( m_isMenuOpen );
  SetMenuDrawInputEnable( true );
  m_pMenuDraw->EndSlideMenuPageScroll();
  if( m_isMenuOpen != false )
  {
    m_pMenuDraw->ChangePageArrowVisible( page );
    m_pMenuDraw->SetVisibleIconArea( true );
  }
  else
  {
    SetMenuBaseInputEnable( true );
  }
  ChangeSubSeq( SEQ_MAIN );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMenuIconCatch
 * @brief   アップデート：メニュー項目キャッチムーブ
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqMenuIconCatch(void)
{
  switch( m_subSeq )
  {
  case 0:   // メニュー開き待ち
    m_pMenuDraw->MoveCatchMenuIcon();
    m_pMenuDraw->ChangeButtonEnable( true );
    m_pMenuDraw->SetVisibleCloseButton( false );
    m_pMenuBase->SetVisibleCover( false );
    m_subSeq = 1;
    break;

  case 1:   // メイン
    if( m_pMenuDraw->IsIconCatch() != false )
    {
      m_pMenuDraw->MoveCatchMenuIcon();
      if( m_catchScrollCount != MENU_ICON_CATCH_SCROLL_COUNT )
      {
        m_catchScrollCount++;
      }
      else
      {
        if( m_pMenuDraw->GetIconCatchPutID() == MenuDraw::PUT_ID_ARROW )
        {
          // 右
          if( m_pMenuDraw->GetIconCatchPutIndex() == 0 )
          {
            ChangePageCore( 1 );
          }
          // 左
          else
          {
            ChangePageCore( -1 );
          }
          PlaySE( SEQ_SE_PAGE2 );
          m_subSeq = 2;
        }
      }
    }
    else
    {
      SetMenuDrawInputEnable( false );

      if( m_pMenuDraw->GetIconCatchPutID() != MenuDraw::PUT_ID_ICON )
      {
        u32 catch_page = m_pMenuDraw->GetIconCatchPage();
        if( catch_page != m_saveFieldMenu->GetMenuPage() )
        {
          ChangePageButtonCore( catch_page );
          m_subSeq = 3;
          break;
        }
      }

      m_pMenuDraw->EndCatchMenuIcon();
      PlaySE( SEQ_SE_RELEASE );
      m_subSeq = 4;
    }
    break;

  case 2:   // ページ切り替え
    m_pMenuDraw->MoveCatchMenuIcon();
    if( m_pMenuDraw->IsPageChangeAnimeEnd() != false )
    {
      u32 page = EndChangePage();
      m_pMenuDraw->ChangePageArrowVisible( page );
      m_catchScrollCount = 0;
      m_subSeq = 1;
    }
    break;

  case 3:   // ページ切り替え（終了処理）
    if( m_pMenuDraw->IsPageChangeAnimeEnd() != false )
    {
/*
      EndChangePage();
      m_pMenuDraw->EndCatchMenuIcon();
      PlaySE( SEQ_SE_RELEASE );
      m_subSeq = 4;
*/
      m_subSeq = 6;
    }
    break;

  case 4:   // アイコン移動
    if( m_pMenuDraw->ReleaseMenuIcon() != false )
    {
      if( m_pMenuDraw->GetIconCatchPutID() == MenuDraw::PUT_ID_ICON )
      {
        m_saveFieldMenu->ChangeMenuIconID(
          m_pMenuDraw->GetCatchMenuIconID(),
          static_cast<Savedata::FieldMenu::IconID>(m_pMenuDraw->GetIconCatchPutIndex()) );
      }
      m_pMenuDraw->ResetReleaseMenuIconPos();
      m_subSeq = 5;
    }
    break;

  case 5:   // 終了
    if( m_pMenuDraw->IsReleaseMenuIconEnd() != false )
    {
      m_pMenuDraw->SetVisibleMenuIconInfo( true );
      m_pMenuDraw->SetIconInfoMoveAnime( true );
      m_pMenuDraw->SetMenuInfoAnimeEnable( true );
      if( m_isMenuOpen != false )
      {
        Savedata::FieldMenu::IconID get_id = m_pMenuDraw->GetCatchMenuIconID();
        m_cursorPos = m_saveFieldMenu->GetMenuIconPos( get_id );
        m_pMenuDraw->PutCursor( get_id );
        m_pMenuDraw->SetVisibleCursor( true );
        m_pMenuDraw->SetVisibleIconArea( true );
        m_pMenuDraw->SetVisibleCloseButton( true );
      }
      else
      {
        m_pMenuDraw->ChangeButtonEnable( false );
        m_pMenuBase->SetVisibleCover( true );
        SetMenuBaseInputEnable( true );
      }
      m_pMenuDraw->SetInputLock( false );
      SetMenuDrawInputEnable( true );
      ChangeSubSeq( SEQ_MAIN );
    }
    break;

  case 6:
    {
      u32 page = EndChangePage();
      m_pMenuDraw->ChangePageArrowVisible( page );
    }
    m_pMenuDraw->EndCatchMenuIcon();
    PlaySE( SEQ_SE_RELEASE );
    m_subSeq = 4;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInitRideList
 * @brief   アップデート：ライド選択画面起動
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInitRideList(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteFieldMenu() == false )
    {
      break;
    }
    if( m_saveFieldMenu->IsVisibleRotom() != false )
    {
      m_pMenuBase->SetRotomAnime( false, true );
    }
    m_pMenuBase->SetRotomMessageUpdateEnable( false );
    m_pMenuBase->SetVisibleCover( false );
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateRideDraw(false) == false )
    {
      break;
    }
    m_moduleSeq = 0;
    ChangeSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqExitRideList
 * @brief   アップデート：ライド選択画面終了
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqExitRideList(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteRideDraw() == false )
    {
      break;
    }
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateFieldMenu() != false )
    {
      m_moduleSeq = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInitReport
 * @brief   アップデート：レポート画面起動
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInitReport(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteFieldMenu() == false )
    {
      break;
    }
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateReportDraw() == false )
    {
      break;
    }
    m_moduleSeq = 0;
    ChangeSubSeq( SEQ_MAIN_REPORT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMainReport
 * @brief   アップデート：レポート画面メイン処理
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqMainReport(void)
{
  if( m_pReportDraw->IsModuleEnd() != false )
  {
    CloseReport();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqExitReport
 * @brief   アップデート：レポート画面終了
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqExitReport(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteReportDraw() == false )
    {
      break;
    }
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateFieldMenu() != false )
    {
      m_moduleSeq = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqInitQuickMatch
 * @brief   アップデート：クイック通信画面起動
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqInitQuickMatch(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteFieldMenu() == false )
    {
      break;
    }
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateQuickMatchDraw() == false )
    {
      break;
    }
    m_saveFieldMenu->ResetIconNew( Savedata::FieldMenu::ICON_ID_NET_PLAY );
    m_moduleSeq = 0;
    ChangeSubSeq( SEQ_MAIN );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqExitQuickMatch
 * @brief   アップデート：クイック通信終了
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqExitQuickMatch(void)
{
  switch( m_moduleSeq )
  {
  case 0:
    if( DeleteQuickMatchDraw() == false )
    {
      break;
    }
    m_moduleSeq++;
    /* FALL THROUGH */
  case 1:
    if( CreateFieldMenu() != false )
    {
      m_moduleSeq = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqRotomEvent
 * @brief   アップデート：ロトムイベント
 * @date    2015.11.11
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::Update_SeqRotomEvent(void)
{
  switch( m_subSeq )
  {
  case 0:
    SetDispMask( false );
    m_isRotomEventStartOpen = false;
    if( m_dispMode == DISP_MODE_SLEEP )
    {
      GFL_ASSERT( 0 );
      m_subSeq = 3;
      break;
    }
    if( m_dispMode == DISP_MODE_FRAME_OFF )
    {
      m_pMenuBase->SetVisibleCover( true, true );
      m_pMenuBase->VanishRotomEye();
      m_pMenuBase->SetRotomPowerOnAnime();
      m_subSeq = 1;
    }
    else if( m_saveFieldMenu->IsVisibleRotom() == false )
    {
      m_saveFieldMenu->SetVisibleRotom( true );
      m_pMenuBase->SetRotomAnime( true, false );
      m_pMenuBase->SetMessageAnime( true, false );
      m_isRotomEventStartOpen = true;
      m_subSeq = 2;
    }
    else
    {
      m_subSeq = 2;
    }
    break;

  case 1:
    if( m_pMenuBase->IsRotomPowerOnAnimeEnd() != false )
    {
      m_saveFieldMenu->SetVisibleRotom( true );
      m_pMenuBase->SetRotomAnime( true, false );
      m_isRotomEventStartOpen = true;
      m_subSeq = 2;
    }
    break;

  case 2:
    break;

  case 3:
    if( m_pMenuBase->IsMessageAnime() != false && m_pMenuBase->IsRotomAnimeEnd() != false )
    {
      SetDispMask( true );
      m_isRotomEvent = false;
      ChangeSubSeq( SEQ_MAIN );
    }
    break;
  }
}


//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力リスナー
 * @date    2015.05.21
 *
 * @param   id  入力ID
 *
 * @note  override FrameListener
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OnAction( u32 id )
{
  if( m_isMenuOpen == false )
  {
    OnActionField( id );
  }
  else
  {
    OnActionMenu( id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnActionField
 * @brief   フィールドフリームーブ中の入力
 * @date    2015.05.29
 *
 * @param   id  入力ID
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OnActionField( u32 id )
{
  switch( id )
  {
  case FrameListener::ACTION_CALL_RIDE:
    if( m_eventWork->CheckEventFlag(SYS_FLAG_POKE_RIDE_OPEN) != false )
    {
      m_outRequestID = Field::SubScreen::OUT_REQ_ID_RIDE_OPEN;
#if PM_DEBUG
      if( m_debugModeFlag != false )
      {
        OpenRideList();
      }
#endif  // PM_DEBUG
    }
    break;

  case FrameListener::ACTION_CALL_FINDER:
    if( m_pNaviDraw != NULL )
    {
      if( m_pNaviDraw->IsFinderIcon() != false )
      {
        if( CallFinder() != false )
        {
          Sound::PlaySE( SEQ_SE_DECIDE1 );
        }
      }
    }
    break;

  case FrameListener::ACTION_CALL_MENU:
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_MENU_OPEN;
    break;

  case FrameListener::ACTION_POKELIST:
  case FrameListener::ACTION_BAG:
  case FrameListener::ACTION_ZUKAN:
  case FrameListener::ACTION_TR_CARD:
  case FrameListener::ACTION_CONFIG:
  case FrameListener::ACTION_ALBUM:
  case FrameListener::ACTION_POKE_PLANET:
  case FrameListener::ACTION_POKE_PARURE:
  case FrameListener::ACTION_NET_PLAY:
  case FrameListener::ACTION_JOIN_FESTA:
  case FrameListener::ACTION_BATTLE_VIDEO:
  case FrameListener::ACTION_QR_READ:
    if( id == FrameListener::ACTION_BAG || IsValidFieldMap() != false )
    {
      if( id == FrameListener::ACTION_ZUKAN )
      {
        Savedata::IncRecord( Savedata::Record::RECID_ROTOM );   // レコード
      }
      u32 icon = id - FrameListener::ACTION_POKELIST;
      m_cursorPos = m_saveFieldMenu->GetMenuIconPos( static_cast<Savedata::FieldMenu::IconID>(icon) );

      u32 req = Field::SubScreen::OUT_REQ_ID_POKELIST + icon;
      m_outRequestID = static_cast<Field::SubScreen::FieldMenuOutRequestID>( req );
    }
    GFL_PRINT( "★OnActionField() %d\n", m_outRequestID );
    break;

  case FrameListener::ACTION_REPORT:
    // @fix MMCat[728]: プレイヤーの入力を受け付けているかをチェック
    if( IsPlayerEnableInput() != false )
    {
      if( IsValidFieldMap() != false )
      {
        m_outRequestID = static_cast<Field::SubScreen::FieldMenuOutRequestID>( Field::SubScreen::OUT_REQ_ID_REPORT );
      }
    }
    break;

  case FrameListener::ACTION_PAGE_00:
    ChangePageButton( 0 );
    break;
  case FrameListener::ACTION_PAGE_01:
    ChangePageButton( 1 );
    break;

  case FrameListener::ACTION_CATCH_POKELIST:
  case FrameListener::ACTION_CATCH_BAG:
  case FrameListener::ACTION_CATCH_ZUKAN:
  case FrameListener::ACTION_CATCH_TR_CARD:
  case FrameListener::ACTION_CATCH_CONFIG:
  case FrameListener::ACTION_CATCH_ALBUM:
  case FrameListener::ACTION_CATCH_POKE_PLANET:
  case FrameListener::ACTION_CATCH_POKE_PARURE:
  case FrameListener::ACTION_CATCH_NET_PLAY:
  case FrameListener::ACTION_CATCH_JOIN_FESTA:
  case FrameListener::ACTION_CATCH_BATTLE_VIDEO:
  case FrameListener::ACTION_CATCH_QR_READ:
    CatchMenuIcon( id-FrameListener::ACTION_CATCH_POKELIST );
    break;

  case FrameListener::ACTION_MENU_PAGE_DRAG_START:
    SetMenuBaseInputEnable( false );
    break;
  case FrameListener::ACTION_MENU_PAGE_DRAG:
    DragMenuPage();
    break;
  case FrameListener::ACTION_MENU_PAGE_RELEASE:
    ReleaseMenuPage();
    break;
  case FrameListener::ACTION_MENU_PAGE_SCROLL:
    ScrollMenuPage();
    break;

  case FrameListener::ACTION_END:
    SetEnd();
    break;

  case FrameListener::ACTION_RIDE_SC_UP:
  case FrameListener::ACTION_RIDE_SC_DOWN:
  case FrameListener::ACTION_RIDE_SC_LEFT:
  case FrameListener::ACTION_RIDE_SC_RIGHT:
    {
      Savedata::FieldMenu::RideID ride = m_saveFieldMenu->GetRideShortcut( static_cast<Savedata::FieldMenu::RideKeyID>(id-FrameListener::ACTION_RIDE_SC_UP) );
      if( ride != Savedata::FieldMenu::RIDE_ID_NONE )
      {
        m_outRequestID = Field::SubScreen::OUT_REQ_ID_RIDE_POKEMON;
        m_rideID = ride;
      }
    }
    break;

  case FrameListener::ACTION_CALL_TOWNMAP:
    SelectRotomDisp( false );
    break;

  case FrameListener::ACTION_ROTOM_CLOSE:
    if( m_isRotomEvent != false )
    {
      if( m_isRotomEventStartOpen != false )
      {
        SetRotomMessage();
      }
      m_subSeq++;
    }
    else
    {
      SetRotomMessage();
    }
    break;

  case FrameListener::ACTION_ROTOM_OPEN:
    SelectRotomDisp( true );
    break;

  case FrameListener::ACTION_ROTOPON_START:
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_START_ROTOPON;
//    m_outRequestID = Field::SubScreen::OUT_REQ_ID_REPORT;
    break;

  case FrameListener::ACTION_ROTOPON_END:
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_END_ROTOPON;
    break;

  case FrameListener::ACTION_ROTOPON_ITEMEVENT:
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_ROTOPON_ITEM_EVENT;
    break;

  case FrameListener::ACTION_CALL_FINDER_STUDIO:
    if( IsValidFieldMap() != false )
    {
      m_outRequestID = Field::SubScreen::OUT_REQ_ID_FINDER_STUDIO;
    }
    break;

  default:
//    GFL_ASSERT( 0 );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnActionMenu
 * @brief   メニューを開いている場合の入力
 * @date    2015.05.29
 *
 * @param   id  入力ID
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OnActionMenu( u32 id )
{
  switch( id )
  {
  case FrameListener::ACTION_CALL_MENU:
    PlaySE( SEQ_SE_CLOSE1 );
    /* FALL THROUGH */
  case FrameListener::ACTION_CLOSE_MENU:
    CloseMenu();
#if PM_DEBUG
    DebugPrintHeapSizeLow( "MENU_CLOASE" );
#endif  // PM_DEBUG
    break;

  case FrameListener::ACTION_POKELIST:
  case FrameListener::ACTION_BAG:
  case FrameListener::ACTION_ZUKAN:
  case FrameListener::ACTION_TR_CARD:
  case FrameListener::ACTION_CONFIG:
  case FrameListener::ACTION_ALBUM:
  case FrameListener::ACTION_POKE_PLANET:
  case FrameListener::ACTION_POKE_PARURE:
  case FrameListener::ACTION_NET_PLAY:
  case FrameListener::ACTION_JOIN_FESTA:
  case FrameListener::ACTION_BATTLE_VIDEO:
  case FrameListener::ACTION_QR_READ:
    if( id == FrameListener::ACTION_BAG || IsValidFieldMap() != false )
    {
      u32 icon = id - FrameListener::ACTION_POKELIST;
      m_cursorPos = m_saveFieldMenu->GetMenuIconPos( static_cast<Savedata::FieldMenu::IconID>(icon) );

      u32 req = Field::SubScreen::OUT_REQ_ID_POKELIST + icon;
      m_outRequestID = static_cast<Field::SubScreen::FieldMenuOutRequestID>( req );

      m_pMenuDraw->SetInputEnabled( false );  // 連打対策のため入力を止める
    }
    break;

  case FrameListener::ACTION_REPORT:
    if( IsValidFieldMap() != false )
    {
      m_pMenuDraw->SetInputEnabled( false );  // 連打対策のため入力を止める
      m_outRequestID = static_cast<Field::SubScreen::FieldMenuOutRequestID>( Field::SubScreen::OUT_REQ_ID_REPORT );
    }
    break;

  case FrameListener::ACTION_CHANGE_PAGE_LEFT:
    ChangePage( -1, false );
    break;
  case FrameListener::ACTION_CHANGE_PAGE_RIGHT:
    ChangePage( 1, false );
    break;

  case FrameListener::ACTION_PAGE_00:
    ChangePageButton( 0 );
    break;
  case FrameListener::ACTION_PAGE_01:
    ChangePageButton( 1 );
    break;

  case FrameListener::ACTION_MENU_POS_UP:
  case FrameListener::ACTION_MENU_POS_DOWN:
  case FrameListener::ACTION_MENU_POS_LEFT:
  case FrameListener::ACTION_MENU_POS_RIGHT:
    MoveMenuPos( id, m_pMenuDraw->IsInputTrigger() );
    break;

  case FrameListener::ACTION_CATCH_POKELIST:
  case FrameListener::ACTION_CATCH_BAG:
  case FrameListener::ACTION_CATCH_ZUKAN:
  case FrameListener::ACTION_CATCH_TR_CARD:
  case FrameListener::ACTION_CATCH_CONFIG:
  case FrameListener::ACTION_CATCH_ALBUM:
  case FrameListener::ACTION_CATCH_POKE_PLANET:
  case FrameListener::ACTION_CATCH_POKE_PARURE:
  case FrameListener::ACTION_CATCH_NET_PLAY:
  case FrameListener::ACTION_CATCH_JOIN_FESTA:
  case FrameListener::ACTION_CATCH_BATTLE_VIDEO:
  case FrameListener::ACTION_CATCH_QR_READ:
    CatchMenuIcon( id-FrameListener::ACTION_CATCH_POKELIST );
    break;

  case FrameListener::ACTION_MENU_PAGE_DRAG_START:
    m_pMenuDraw->SetVisibleCursor( false );
    m_pMenuDraw->SetVisibleIconArea( false );
    SetMenuBaseInputEnable( false );
    break;
  case FrameListener::ACTION_MENU_PAGE_DRAG:
    DragMenuPage();
    break;
  case FrameListener::ACTION_MENU_PAGE_RELEASE:
    ReleaseMenuPage();
    break;
  case FrameListener::ACTION_MENU_PAGE_SCROLL:
    ScrollMenuPage();
    break;

  default:
//    GFL_ASSERT( 0 );
    break;
  }
}



//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @func    OpenMenu
 * @brief   フィールドメニューを開く
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OpenMenu(void)
{
  SetMenuBaseInputEnable( false );

  m_pMenuDraw->OpenMenu();
  SetMenuDrawInputEnable( false );

  if( m_pNaviDraw != NULL )
  {
    m_pNaviDraw->OpenMenu();
    SetNaviDrawInputEnable( false );
  }

  if( m_isMenuBaseCreate != false )
  {
    if( m_saveFieldMenu->IsVisibleRotom() != false )
    {
      m_pMenuBase->SetRotomAnime( false, true );
    }
    m_pMenuBase->SetRotomMessageUpdateEnable( false );
    m_pMenuBase->VanishRotomSelectButton();

    m_pMenuBase->SetVisibleLayout( false );   // 消しておく
    m_pMenuBase->SetVisibleCover( false );
  }

  m_isMenuOpen = true;
  ChangeSubSeq( SEQ_OPEN_MENU );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseMenu
 * @brief   フィールドメニューを閉じる
 * @date    2015.05.29
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CloseMenu(void)
{
  InitMenuBaseSync();

  SetMenuBaseInputEnable( false );

  m_pMenuDraw->SetVisibleCursor( false );

  m_pMenuDraw->CloseMenu();
  SetMenuDrawInputEnable( false );

  if( m_pNaviDraw != NULL )
  {
    m_pNaviDraw->CloseMenu();
  }

  if( m_saveFieldMenu->IsVisibleRotom() != false )
  {
    m_pMenuBase->SetEnableButton( false );
    m_pMenuBase->SetRotomAnime( true, false );
  }
  m_pMenuBase->SetRotomMessageUpdateEnable( true );

  m_pMenuBase->SetVisibleLayout( false );   // 消しておく
  m_pMenuBase->SetVisibleCover( true );

  ChangeSubSeq( SEQ_CLOSE_MENU );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePage
 * @brief   メニューのページを切り替える
 * @date    2015.06.02
 *
 * @param   mv            切り替え方向
 * @param   is_arrow_anm  true = 切り替え矢印のアニメを再生
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ChangePage( int mv, bool is_arrow_anm )
{
  SetMenuBaseInputEnable( false );
  SetMenuDrawInputEnable( false );
  ChangePageCore( mv, is_arrow_anm );
  m_pMenuDraw->SetVisibleIconArea( false );
  m_pMenuDraw->SetVisibleCursor( false );
  ChangeSubSeq( SEQ_CHANGE_MENU_PAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePageCore
 * @brief   メニューのページを切り替える（共通部分）
 * @date    2015.06.04
 *
 * @param   mv            切り替え方向
 * @param   is_arrow_anm  true = 切り替え矢印のアニメを再生
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ChangePageCore( int mv, bool is_arrow_anm )
{
  u32 next_page = m_saveFieldMenu->GetMoveMenuPage( mv );
  m_pMenuDraw->StartPageChangeAnime( next_page, mv );
  if( is_arrow_anm != false )
  {
    m_pMenuDraw->StartPageChangeArrowAnime( mv );
  }
  m_saveFieldMenu->SetMenuPage( next_page );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePageButton
 * @brief   ページボタンからメニューのページを切り替える
 * @date    2015.06.02
 *
 * @param   next_page   切り替え後のページ
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ChangePageButton( int next_page )
{
  SetMenuBaseInputEnable( false );
  SetMenuDrawInputEnable( false );
  ChangePageButtonCore( next_page );
  m_pMenuDraw->SetVisibleIconArea( false );
  m_pMenuDraw->SetVisibleCursor( false );
  ChangeSubSeq( SEQ_CHANGE_MENU_PAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePageButtonCore
 * @brief   ページボタンからメニューのページを切り替える（共通部分）
 * @date    2015.06.04
 *
 * @param   next_page   切り替え後のページ
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ChangePageButtonCore( int next_page )
{
  int mv = 1;
  if( next_page == 0 )
  {
    mv = -1;
  }
  m_pMenuDraw->StartPageChangeAnime( next_page, mv );
  m_saveFieldMenu->SetMenuPage( next_page );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPageLowPos
 * @brief   ページの並び位置を取得
 * @date    2015.06.02
 *
 * @param   page  ページ番号
 *
 * @return  並び位置
 *
 * @note  未使用（３ページあったときの処理）
 */
//-----------------------------------------------------------------------------
 u32 FieldMenuFrame::GetPageLowPos( u32 page )
{
//  static const u32 tbl[] = { 2, 0, 1 };
  static const u32 tbl[] = { 0, 1 };
  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( tbl[i] == page )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndChangePage
 * @brief   ページ切り替え終了
 * @date    2015.06.04
 *
 * @return  現在のページ番号
 */
//-----------------------------------------------------------------------------
u32 FieldMenuFrame::EndChangePage(void)
{
  u32 page = m_saveFieldMenu->GetMenuPage();
  m_pMenuDraw->PutCursor( m_saveFieldMenu->GetMenuIconID(page,m_cursorPos) );
  m_pMenuDraw->EndPageChangeAnime( page );
  m_pMenuDraw->ChangePageButton();
  return page;
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveMenuPos
 * @brief   メニューカーソル移動
 * @date    2015.06.02
 *
 * @parma   id      動作ID
 * @param   is_trg  true = トリガー入力
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::MoveMenuPos( u32 id, bool is_trg )
{
  u32 page = m_saveFieldMenu->GetMenuPage();
  u32 now_pos = m_cursorPos;

  if( id == FrameListener::ACTION_MENU_POS_UP )
  {
    if( m_cursorPos < MENU_ICON_H_MAX )
    {
      if( is_trg != false )
      {
        m_cursorPos += ( Savedata::FieldMenu::PAGE_ICON_MAX - MENU_ICON_H_MAX );
      }
    }
    else
    {
      m_cursorPos -= MENU_ICON_H_MAX;
    }
  }
  else if( id == FrameListener::ACTION_MENU_POS_DOWN )
  {
    if( m_cursorPos >= (Savedata::FieldMenu::PAGE_ICON_MAX-MENU_ICON_H_MAX) )
    {
      if( is_trg != false )
      {
        m_cursorPos -= ( Savedata::FieldMenu::PAGE_ICON_MAX - MENU_ICON_H_MAX );
      }
    }
    else
    {
      m_cursorPos += MENU_ICON_H_MAX;
    }
  }
  else if( id == FrameListener::ACTION_MENU_POS_LEFT )
  {
    if( ( m_cursorPos & 1 ) != 0 )
    {
      m_cursorPos--;
    }
    else
    {
      if( page != m_saveFieldMenu->GetMoveMenuPage(-1) )
      {
        m_cursorPos++;
        ChangePage( -1 );
        PlaySE( SEQ_SE_PAGE2 );
        return;
      }
      else if( is_trg != false )
      {
        m_cursorPos++;
      }
    }
  }
  else if( id == FrameListener::ACTION_MENU_POS_RIGHT )
  {
    if( ( m_cursorPos & 1 ) == 0 )
    {
      m_cursorPos++;
    }
    else
    {
      if( page != m_saveFieldMenu->GetMoveMenuPage(1) )
      {
        m_cursorPos--;
        ChangePage( 1 );
        PlaySE( SEQ_SE_PAGE2 );
        return;
      }
      else if( is_trg != false )
      {
        m_cursorPos--;
      }
    }
  }

  if( now_pos != m_cursorPos )
  {
    m_pMenuDraw->PutCursor( m_saveFieldMenu->GetMenuIconID(page,m_cursorPos) );
    PlaySE( SEQ_SE_SELECT1 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CatchMenuIcon
 * @brief   メニュー項目キャッチ
 * @date    2015.06.04
 *
 * @param   id  メニュー項目ID
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CatchMenuIcon( u32 id )
{
  SetMenuBaseInputEnable( false );

  m_pMenuDraw->InitCatchMenuIcon( static_cast<Savedata::FieldMenu::IconID>(id) );

  m_catchScrollCount = 0;

  m_pMenuDraw->SetInputLock( true );
  m_pMenuDraw->SetVisibleMenuIconInfo( false );
  m_pMenuDraw->SetIconInfoMoveAnime( false );
  m_pMenuDraw->SetMenuInfoAnimeEnable( false );

  if( m_isMenuOpen != false )
  {
    m_pMenuDraw->SetVisibleCursor( false );
    m_pMenuDraw->SetVisibleCloseButton( false );
    ChangeSubSeq( SEQ_MENU_ICON_CATCH, 1 );
  }
  else
  {
    ChangeSubSeq( SEQ_MENU_ICON_CATCH, 0 );
  }

  PlaySE( SEQ_SE_CATCH );
}

//-----------------------------------------------------------------------------
/**
 * @func    DragMenuPage
 * @brief   ページのドラッグ処理
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::DragMenuPage(void)
{
  m_pMenuDraw->DragMenuPage();
}

//-----------------------------------------------------------------------------
/**
 * @func    ReleaseMenuPage
 * @brief   ページのリリース処理
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ReleaseMenuPage(void)
{
  SetMenuDrawInputEnable( false );
  int mv = m_pMenuDraw->ReleaseMenuPage();
  if( mv != 0 )
  {
    m_saveFieldMenu->SetMenuPage( m_saveFieldMenu->GetMoveMenuPage(mv) );
    PlaySE( SEQ_SE_PAGE2 );
  }
  ChangeSubSeq( SEQ_SLIDE_MENU_PAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    ScrollMenuPage
 * @brief   ページスクロール
 * @date    2015.06.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::ScrollMenuPage(void)
{
  SetMenuDrawInputEnable( false );
  m_pMenuDraw->SetVisibleIconArea( false );
  int mv = m_pMenuDraw->SlideMenuPage();
  if( mv != 0 )
  {
    m_saveFieldMenu->SetMenuPage( m_saveFieldMenu->GetMoveMenuPage(mv) );
    PlaySE( SEQ_SE_PAGE2 );
  }
  ChangeSubSeq( SEQ_SLIDE_MENU_PAGE );
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenRideList
 * @brief   ライド選択画面を開く
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OpenRideList(void)
{
  if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
  {
    if( m_pNaviDraw != NULL )
    {
      m_pNaviDraw->Stop();
    }
    SetNaviDrawInputEnable( false );
  }
  m_pMenuDraw->Stop();
  SetMenuDrawInputEnable( false );
  SetMenuBaseInputEnable( false );
  m_pMenuBase->VanishRotomSelectButton(); // @fix MMCat[615]: ロトム選択肢非表示
  m_pMenuBase->SetVisibleLayout( true );
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_INIT_RIDE_LIST );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseRideList
 * @brief   ライド選択画面を閉じる
 * @date    2015.07.15
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CloseRideList(void)
{
  m_pRideDraw->SetInputEnabled( false );
  if( m_rideID != Savedata::FieldMenu::RIDE_ID_FLY )
  {
    m_pMenuBase->SetVisibleLayout( false );
  }
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_EXIT_RIDE_LIST );
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectRide
 * @brief   ライド画面選択時のリスナー
 * @date    2015.07.15
 *
 * @param   id  入力ID
 *
 * @note  override RideDrawListener
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SelectRide( Savedata::FieldMenu::RideID id )
{
  if( id == Savedata::FieldMenu::RIDE_ID_NONE )
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_RIDE_CLOSE;
  }
  else
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_RIDE_DECIDE;
    // "なみのり"のコリジョンを更新
    if( m_isRideCollisionUpdate != false )
    {
      Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();
      if( fieldmap == NULL )
      {
        GFL_ASSERT( 0 );
      }
      else
      {
        Field::FieldEventCheck * ev_check = fieldmap->GetEventCheck();
        if( ev_check == NULL )
        {
          GFL_ASSERT( 0 );
        }
        else
        {
          ev_check->UpdateCollisionNaminori();
        }
      }
    }
  }
  m_rideID = id;
  CloseRideList();
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectRideInfo
 * @brief   ライド画面説明選択時のリスナー
 * @date    2016.03.30
 *
 * @param   id  入力ID
 *
 * @note  override RideDrawListener
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SelectRideInfo( Savedata::FieldMenu::RideID id )
{
  m_playData->SetSelectRideID( id );
  m_pRideDraw->SetInputEnabled( false );
  m_outRequestID = Field::SubScreen::OUT_REQ_ID_RIDE_INFO;
  m_rideID = id;
  m_moduleSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRideID
 * @brief   選択されたライドIDを取得
 * @date    2015.07.15
 *
 * @return  ライドID
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RideID FieldMenuFrame::GetRideID(void)
{
  return m_rideID;
}

//! ライドポケモンID変換テーブル
const FieldMenuFrame::RIDE_ID_CNV FieldMenuFrame::RideConvTable[] =
{
  { Savedata::FieldMenu::RIDE_ID_RUSH,    Field::RIDE_KENTAROSU },
  { Savedata::FieldMenu::RIDE_ID_JET,     Field::RIDE_SAMEHADA  },
  { Savedata::FieldMenu::RIDE_ID_SEARCH,  Field::RIDE_MUURANDO  },
  { Savedata::FieldMenu::RIDE_ID_SONAR,   Field::RIDE_LAPLACE   },
  { Savedata::FieldMenu::RIDE_ID_STRINGS, Field::RIDE_KAIRIKY   },
  { Savedata::FieldMenu::RIDE_ID_FLY,     Field::RIDE_RIZAADON  },
  { Savedata::FieldMenu::RIDE_ID_CRASH,   Field::RIDE_ROBA2     },
};

//-----------------------------------------------------------------------------
/**
 * @func    GetMenuRideID
 * @brief   フィールドのライドIDからメニュー画面のライドIDを取得
 * @date    2015.09.16
 *
 * @param   id  フィールドのライドID
 *
 * @return  メニュー画面のライドID
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RideID FieldMenuFrame::GetMenuRideID( Field::RIDE_POKEMON_ID id )
{
  for( u32 i=0; i<GFL_NELEMS(RideConvTable); i++ )
  {
    if( id == RideConvTable[i].field )
    {
      return RideConvTable[i].list;
    }
  }
  return Savedata::FieldMenu::RIDE_ID_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetFieldRideID
 * @brief   メニュー画面のライドIDをフールドのライドIDに変換
 * @date    2015.09.16
 *
 * @param   id  メニュー画面のライドID
 *
 * @return  フィールドのライドID
 */
//-----------------------------------------------------------------------------
Field::RIDE_POKEMON_ID FieldMenuFrame::GetFieldRideID( Savedata::FieldMenu::RideID id )
{
  for( u32 i=0; i<GFL_NELEMS(RideConvTable); i++ )
  {
    if( id == RideConvTable[i].list )
    {
      return RideConvTable[i].field;
    }
  }
  return Field::RIDE_NO_POKEMON;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRideOnEnable
 * @brief   ライドポケモンに乗れるかをチェック
 * @date    2015.09.16
 *
 * @param   id  ライドID
 *
 * @return  チェック結果
 */
//-----------------------------------------------------------------------------
Field::RIDE_CHECK_RESULT FieldMenuFrame::IsRideOnEnable( Savedata::FieldMenu::RideID id )
{
  Field::RIDE_POKEMON_ID field_id = GetFieldRideID( id );
  if( field_id == Field::RIDE_NO_POKEMON )
  {
    return Field::RCR_ERROR_OTHERS;
  }
  return Field::EventPokemonRideOn::IsPokemonRideOnEnable( m_gameManager, field_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRideOffEnable
 * @brief   ライドポケモンから降りられるかをチェック
 * @date    2015.09.16
 *
 * @param   id  ライドID
 *
 * @return  チェック結果
 */
//-----------------------------------------------------------------------------
Field::RIDE_CHECK_RESULT FieldMenuFrame::IsRideOffEnable( Savedata::FieldMenu::RideID id )
{
  Field::RIDE_POKEMON_ID field_id = GetFieldRideID( id );
  if( field_id == Field::RIDE_NO_POKEMON )
  {
    return Field::RCR_ERROR_OTHERS;
  }
  return Field::EventPokemonRideOff::IsPokemonRideOffEnable( m_gameManager );
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenReport
 * @brief   レポート画面を開く
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OpenReport(void)
{
  InitMenuBaseSync();

  if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
  {
    if( m_pNaviDraw != NULL )
    {
      m_pNaviDraw->Stop();
    }
    SetNaviDrawInputEnable( false );
  }
  m_pMenuDraw->Stop();
  SetMenuDrawInputEnable( false );
  SetMenuBaseInputEnable( false );
  m_pMenuBase->SetVisibleLayout( true );
  // @fix GFMMCat[237]: ロトムメッセージの更新を止める
  if( m_isMenuOpen == false )
  {
    if( m_playData->GetEnableRottomMsgIndex() != PlayData::ROTOM_MSG_MAX )
    {
      SetRotomMessage();
    }
    m_pMenuBase->SetRotomMessageUpdateEnable( false );
  }
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_INIT_REPORT );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseReport
 * @brief   レポート画面を閉じる
 * @date    2015.08.18
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CloseReport(void)
{
  if( m_pReportDraw->IsCancel() == false )
  {
    m_isMenuOpen = false;
  }

  if( m_isMenuOpen == false )
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_REPORT_CLOSE;
    m_pMenuBase->SetVisibleLayout( false );
  }
  else
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_REPORT_KEEP_MENU;
  }

  m_pReportDraw->SetInputEnabled( false );
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_EXIT_REPORT );
}

//-----------------------------------------------------------------------------
/**
 * @func    OpenQuickMatch
 * @brief   クイック通信画面を開く
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OpenQuickMatch(void)
{
  InitMenuBaseSync();

  if( m_dispMode != DISP_MODE_FRAME_OFF && m_dispMode != DISP_MODE_SLEEP )
  {
    if( m_pNaviDraw != NULL )
    {
      m_pNaviDraw->Stop();
    }
    SetNaviDrawInputEnable( false );
  }
  m_pMenuDraw->Stop();
  SetMenuDrawInputEnable( false );
  SetMenuBaseInputEnable( false );
  m_pMenuBase->SetVisibleLayout( true );
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_INIT_QUICKMATCH );
}

//-----------------------------------------------------------------------------
/**
 * @func    CloseQuickMatch
 * @brief   クイック通信画面を閉じる
 * @date    2015.09.09
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::CloseQuickMatch(void)
{
  if( m_isMenuOpen == false )
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CLOSE;
    m_pMenuBase->SetVisibleLayout( false );
  }
  else
  {
    m_outRequestID = Field::SubScreen::OUT_REQ_ID_QUICKMATCH_KEEP_MENU;
  }

  m_pQuickMatch->SetInputEnabled( false );
  m_moduleSeq = 0;
  ChangeSubSeq( SEQ_EXIT_QUICKMATCH );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnQuickMatchDrawExit
 * @brief   クイック通信がBボタンで終了した時に呼ばれる
 * @date    2015.09.09
 *
 * @note  override QuickMatchDrawListener
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OnQuickMatchDrawExit(void)
{
  CloseQuickMatch();
}

//-----------------------------------------------------------------------------
/**
 * @func    OnQuickMatchDrawNext
 * @brief   クイック通信がマッチングに成功してアプリに遷移するときに呼ばれる
 * @date    2015.09.09
 *
 * @note  override QuickMatchDrawListener
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::OnQuickMatchDrawNext(void)
{
  m_pQuickMatch->SetInputEnabled( false );
  m_outRequestID = Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomMessage
 * @brief   ロトムメッセージをセット
 * @date    2015.09.04
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetRotomMessage(void)
{
  if( m_saveFieldMenu->IsVisibleRotom() == false )
  {
    m_saveFieldMenu->SetVisibleRotom( true );
  }
  else
  {
    m_saveFieldMenu->SetVisibleRotom( false );
    m_pMenuBase->EndPrintRotomMessage();
  }
  m_pMenuBase->SetEnableButton( false );

  bool flg = m_saveFieldMenu->IsVisibleRotom();
  m_pMenuBase->SetRotomAnime( flg, false );
  m_pMenuBase->SetMessageAnime( flg, false );
  m_pMenuBase->SetVisibleButton( !flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomEvent
 * @brief   外部からロトムメッセージをセット
 * @date    2015.11.11
 *
 * @param   msg_id  メッセージID
 *
 * @note  パッシブ状態のイベント中はこちらを使用すること
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::SetRotomEvent( u32 msg_id )
{
  // パッシブ状態じゃない
  if( m_isPassive == false )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  // 起動できないモード
  if( m_dispMode == DISP_MODE_SLEEP )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  if( m_isMenuBaseCreate == false )
  {
    return false;
  }
  m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_SCRIPT, msg_id, PlayData::ROTOM_WORK_NONE );
  m_pMenuBase->SetRotomMessageUpdateEnable( true );
  m_isRotomEvent = true;
  ChangeSubSeq( SEQ_ROTOM_EVENT );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomEvent
 * @brief   ロトムイベント中か
 * @date    2015.11.11
 *
 * @retbal  true  = イベント中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::IsRotomEvent(void)
{
  return m_isRotomEvent;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomPosEvent
 * @brief   POSイベント用ロトムメッセージセット
 * @date    2016.01.15
 *
 * @param   msg_id  メッセージID
 *
 * @note  メッセージを表示するだけ
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetRotomPosEvent( u32 msg_id )
{
  m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_POS, msg_id, PlayData::ROTOM_WORK_NONE );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetNaviPosIcon
 * @brief   現在地アイコンの表示モードを取得
 * @date    2016.01.15
 *
 * @return  現在地アイコンの表示モード
 */
//-----------------------------------------------------------------------------
int FieldMenuFrame::GetNaviPosIcon(void)
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();
  if( fieldmap != NULL )
  {
    Field::FieldEventCheck * ev_check = fieldmap->GetEventCheck();
    if( ev_check != NULL )
    {
      Field::FieldPokeFinderActor * finder = NULL;
      // ファインダーを入手したか
      if( m_eventWork->CheckEventFlag(SYS_FLAG_POKE_FINDER_GET) != false )
      {
        finder = ev_check->GetPokeFinderActor();
      }
      if( finder != NULL )
      {
        // レア
        if( finder->IsRare() != false )
        {
          return NaviDraw::MODE_FINDER_NORMAL;
        }
        else
        {
          return NaviDraw::MODE_FINDER_RARE;
        }
      }
    }
  }
  return NaviDraw::MODE_HERO_ICON;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsValidFieldMap
 * @brief   フィールドマップの有効位置にいるかをチェック
 * @date    2015.12.02
 *
 * @retval  true  = 有効位置
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::IsValidFieldMap(void)
{
  // フィールドの有無に関係なく、プレイヤーが表世界にいるかどうかチェックを行う
  Field::MoveModel::FieldMoveModel* pCharaModel = m_gameManager->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  if( pCharaModel && pCharaModel->CheckValidGround() )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPlayerEnableInput
 * @brief   プレイヤーキャラが入力を受け付けているか
 * @date    2017.05.19
 *
 * @retval  true  = 受け付けている
 * @retval  false = それ以外
 *
 * @fix MMCat[728]
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::IsPlayerEnableInput(void)
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();
  if( fieldmap != NULL )
  {
    Field::MoveModel::FieldMoveModelPlayer * player = fieldmap->GetPlayerCharacter();
    if( player != NULL )
    {
      return player->IsEnableInput();
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectRotomDisp
 * @brief   ロトムの画面を選択
 * @date    2016.02.16
 *
 * @param   is_talk   true = メッセージ表示へ
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SelectRotomDisp( bool is_talk )
{
  // 通常
  if( m_pNaviDraw->IsFinderIcon() == false )
  {
    // 会話へ
    if( is_talk != false )
    {
      SetRequestRotomMessage();
      SetRotomMessage();
    }
    // タウンマップ
    else
    {
      m_outRequestID = Field::SubScreen::OUT_REQ_ID_TOWNMAP;
      GFL_PRINT( "★OnActionField() Field::SubScreen::OUT_REQ_ID_TOWNMAP\n" );
    }
    Savedata::IncRecord( Savedata::Record::RECID_ROTOM );   // レコード
  }
  // ファインダー
  else
  {
    if( CallFinder() != false )
    {
      Savedata::IncRecord( Savedata::Record::RECID_ROTOM );   // レコード
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequestRotomMessage
 * @brief   ロトムメッセージ表示リクエスト
 * @date    2016.02.16
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetRequestRotomMessage(void)
{
  u32 msg = m_pNaviDraw->GetMainEventMsgID();

  if( msg != NaviDraw::EVENT_MSG_NONE )
  {
    m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_EVENT, msg, m_pNaviDraw->GetMainEventWorkValue() );
  }
  else
  {
    if( m_eventWork->CheckEventFlag(SYS_FLAG_GAME_CLEAR) != false )
    {
      static const u32 tbl[] =
      {
        msg_field_rotom_141,
        msg_field_rotom_142,
        msg_field_rotom_143,
        msg_field_rotom_144,
        msg_field_rotom_145,
        msg_field_rotom_146,
        msg_field_rotom_147,
        msg_field_rotom_148,
        msg_field_rotom_149,
        msg_field_rotom_150,
        msg_field_rotom_151,
        msg_field_rotom_152,
        msg_field_rotom_153,
        msg_field_rotom_154,
        msg_field_rotom_155,
        msg_field_rotom_156,
        msg_field_rotom_157,
        msg_field_rotom_158,
        msg_field_rotom_159,
        msg_field_rotom_160,
      };
      u32 rand = System::GflUse::GetPublicRand( GFL_NELEMS(tbl) );
#if PM_DEBUG
      u32 d_index = m_playData->DebugGetRotomGameClearMsgID();
      if( d_index != 0 && d_index <= GFL_NELEMS(tbl) )
      {
        rand = d_index - 1;
      }
#endif  // PM_DEBUG
      m_pMenuBase->SetRequestRotomMessage( PlayData::ROTOM_MSG_ETC, tbl[rand], PlayData::ROTOM_WORK_NONE );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CallFinder
 * @brief   ファインダー呼び出し
 * @date    2016.02.16
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool FieldMenuFrame::CallFinder(void)
{
  Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();
  if( fieldmap != NULL )
  {
    Field::FieldEventCheck * ev_check = fieldmap->GetEventCheck();
    if( ev_check != NULL )
    {
      Field::FieldPokeFinderActor * finder = ev_check->GetPokeFinderActor();
      if( finder != NULL )
      {
        m_outRequestID = Field::SubScreen::OUT_REQ_ID_POKEFINDER;
        return true;
      }
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    StartRotopon
 * @brief   ロトポン開始
 * @date    2016.12.08
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::StartRotopon(void)
{
  m_pMenuBase->StartRotopon();
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotoponItem
 * @brief   ロトポンで獲得した道具を取得
 * @date    2016.12.16
 *
 * @return  アイテム番号
 */
//-----------------------------------------------------------------------------
u16 FieldMenuFrame::GetRotoponItem(void)
{
  return m_pMenuBase->GetRotoponItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTimeRotomColor
 * @brief   ロトム時間帯アニメセット
 * @date    2017.03.30
 */
//-----------------------------------------------------------------------------
void FieldMenuFrame::SetTimeRotomColor(void)
{
  bool is_room = false;
  bool is_weather = false;

  Field::ZoneID zone_id = ZONE_ID_MAX;
  {
    Field::Fieldmap * fieldmap = m_gameManager->GetFieldmap();
    if( fieldmap == NULL )
    {
      zone_id = m_gameManager->GetGameData()->GetStartLocation()->GetZoneID();
    }
    else
    {
      zone_id = fieldmap->GetZoneID();
    }
  }

  if( zone_id != Field::ZONEID_ERROR )
  {
    void * zone_data = m_gameManager->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( zone_id );
    if( zone_data == NULL )
    {
      GFL_ASSERT( 0 );
    }
    else
    {
      Field::ZoneDataAccessor accessor( zone_data );
      if( accessor.GetRottomEffectType() == Field::ROTTOM_EFF_OFF )
      {
        is_room = true;
      }
    }
  }

  //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >> Mcatコメント内有るaの案件対処
  if (m_gameManager->GetGameData()->GetSkyboxDarkFlag() != false)
  {
    is_weather = Field::ZoneDataAccessor::IsDarkWeatherZoneForRotom( zone_id );
  }

  m_pMenuBase->SetTimeRotomColor( is_room, is_weather );
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
bool FieldMenuFrame::IsMenuDrawCreate(void) const
{
  return m_isMenuDrawCreate;
}


//=============================================================================
//=============================================================================
//=============================================================================
#if PM_DEBUG
void FieldMenuFrame::DebugPrintTime( u32 id )
{
#if defined(GF_PLATFORM_CTR)
  static u32 total = 0;
  static nn::os::Tick start;

  if( id == DEBUG_PRINT_TIME_ID_INIT )
  {
    total = 0;
  }
  else
  {
    nn::os::Tick now = nn::os::Tick::GetSystemCurrent();
    nn::fnd::TimeSpan ss = start.ToTimeSpan();
    nn::fnd::TimeSpan es = now.ToTimeSpan();
    u32 sec = es.GetMilliSeconds() - ss.GetMilliSeconds();
    total += sec;
    GFL_PRINT( "<mil>[%03d]: %04d total[%04d]\n", id, sec, total );
  }

  start = nn::os::Tick::GetSystemCurrent();
#endif
}

bool FieldMenuFrame::DebugSetHeapSizeLow(void)
{
  bool ret = false;

  u32 size = m_heap->GetSystemHeap()->GetTotalAllocatableSize();

  if( size < m_debugSysHeapSizeLow )
  {
    m_debugSysHeapSizeLow = size;
    ret = true;
  }

  size = m_heap->GetDeviceHeap()->GetTotalAllocatableSize();

  if( size < m_debugDevHeapSizeLow )
  {
    m_debugDevHeapSizeLow = size;
    ret = true;
  }

  return ret;
}

void FieldMenuFrame::DebugPrintHeapSizeLow( const char * str )
{
  NAKAHIRO_PRINT( "■▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽■\n" );
  if( str == NULL )
  {
    NAKAHIRO_PRINT( "  FieldMenu Heap Size Low\n" );
  }
  else
  {
    NAKAHIRO_PRINT( "  FieldMenu Heap Size Low [%s]\n", str );
  }
  NAKAHIRO_PRINT( "    sys: 0x%08x\n", m_debugSysHeapSizeLow );
  NAKAHIRO_PRINT( "    dev: 0x%08x\n", m_debugDevHeapSizeLow );
  NAKAHIRO_PRINT( "■△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△■\n" );
}

#endif


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

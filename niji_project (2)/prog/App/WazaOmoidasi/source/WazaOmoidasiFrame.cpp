//=============================================================================
/**
 * @file    WazaOmoidasiFrame.cpp
 * @brief   技思い出し画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================

// module
#include "WazaOmoidasiFrame.h"
#include "WazaOmoidasiDraw.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "Print/include/SystemFont.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/Ui/UIViewManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/WazaOmoidasi.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_waza_omoidashi.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,        //!< 開始
  SEQ_MAIN,             //!< メイン
  SEQ_DECIDE,           //!< 技決定
  SEQ_RETURN_MAIN,      //!< メッセージ表示を終了してメインへ戻る
  SEQ_CALL_WAZASELECT,  //!< 技忘れ画面へ
  SEQ_WAZA_CHANGE,      //!< 技を忘れて覚える
  SEQ_APP_CANCEL,       //!< 思い出し画面キャンセル
  SEQ_END               //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiFrame::WazaOmoidasiFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pTokuseiMsgBuff( NULL )
  , m_pWazaMsgBuff( NULL )
  , m_pWordSetLoader( NULL )
  , m_pDraw( NULL )
  , m_pMenuWindow( NULL )
  , m_ynResult( YN_RESULT_NONE )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_pPaneListView( NULL )
  , m_createNumFont( false )
  , m_selectWazaNo( WAZANO_NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  m_isUIViewInputExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
  man->SetInputExclusiveKeyByButtonAnimation( true );
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
WazaOmoidasiFrame::~WazaOmoidasiFrame()
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  man->SetInputExclusiveKeyByButtonAnimation( m_isUIViewInputExclusiveKey );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
/*
void WazaOmoidasiFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveMisc = gd->GetMisc();
  m_myItem   = gd->GetMyItem();
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.12.10
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.12.10
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result WazaOmoidasiFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    InitFade();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    if( CreateWordSetLoader() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 4:
    if( CreateNumFont() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 5:
    if( CreateDraw() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 6:
    if( CreateMenuCurosr() == false )
    {
      break;
    }
    CreatePaneListView();
    CreateMenuWindow();
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.12.10
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result WazaOmoidasiFrame::EndFunc(void)
{
  if( DeleteMenuWindow() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  DeletePaneListView();

  if( DeleteMenuCursor() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  DeleteNumFont();
  DeleteWordSetLoader();
  UnloadMessage();
  UnloadLayoutResource();

  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.12.10
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result WazaOmoidasiFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    break;

  case SEQ_MAIN:
    break;

  case SEQ_DECIDE:
    Update_SeqDecide();
    break;

  case SEQ_RETURN_MAIN:
    Update_SeqReturnMain();
    break;

  case SEQ_CALL_WAZASELECT:
    Update_SeqCallWazaSelect();
    break;

  case SEQ_WAZA_CHANGE:
    Update_SeqWazaChange();
    break;

  case SEQ_APP_CANCEL:
    Update_SeqAppCancel();
    break;

  case SEQ_END:
    if( Update_SeqEnd() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.12.10
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}



//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::CreateDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) WazaOmoidasiDraw( m_heap, m_renderingManager, m_appParam->pp );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->Init( m_pLytResBuff, m_pMsgBuff, m_pTokuseiMsgBuff, m_pWazaMsgBuff, m_pWordSetLoader ) == false )
    {
      break;
    }
    m_pDraw->SetListener( this );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_WAZAOMOIDASI_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_WAZAOMOIDASI_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_WAZAOMOIDASI_GRA,
      GARC_WazaOmoidasi_WazaOmoidasi_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_WAZAOMOIDASI_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_WAZAOMOIDASI_GRA ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_waza_omoidashi_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_tokuseiinfo_DAT,
      &m_pTokuseiMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileLoad( &m_pTokuseiMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_wazainfo_DAT,
      &m_pWazaMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:
    if( app::util::FileAccessor::IsFileLoad( &m_pWazaMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 5:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pWazaMsgBuff );
  GflHeapFreeMemory( m_pTokuseiMsgBuff );
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateWordSetLoader
 * @brief   WordSetLoader生成
 * @date    2015.12.10
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::CreateWordSetLoader(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_WAZANAME|PRELOAD_TOKUSEI );
    m_pWordSetLoader->StartLoad( m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pWordSetLoader->WaitLoad() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteWordSetLoader
 * @brief   WordSetLoader削除
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::DeleteWordSetLoader(void)
{
  GFL_SAFE_DELETE( m_pWordSetLoader );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateNumFont
 * @brief   Numフォント生成
 * @date    2015.12.11
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::CreateNumFont(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( print::SystemFont_GetNumFont() != NULL )
    {
      return true;
    }
    print::SystemFont_LoadNumFont( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap() );
    m_createNumFont = true;
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( print::SystemFont_IsLoadNumFont() == false )
    {
      break;
    }
    print::SystemFont_SetLoadNumFont( m_heap->GetSystemHeap() );
    print::SystemFont_SetLytSystemNumFont();
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteNumFont
 * @brief   Numフォント削除
 * @date    2015.12.11
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::DeleteNumFont(void)
{
  if( m_createNumFont != false )
  {
    print::SystemFont_ResetNumFontInfo();
    print::SystemFont_DeleteNumFont();
    m_createNumFont = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCurosr
 * @brief   メニューカーソル生成
 * @date    2015.12.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::CreateMenuCurosr(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pMenuCursor = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuCursor( m_heap, &m_pMenuCursorBuffer );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pMenuCursor->LoadResource() == false )
    {
      break;
    }
    m_pDraw->CreateMenuCursorLayout( m_pMenuCursor );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursor
 * @brief   メニューカーソル削除
 * @date    2015.12.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::DeleteMenuCursor(void)
{
  if( m_pMenuCursor != NULL )
  {
    if( m_pMenuCursor->IsModuleFree() == false )
    {
      return false;
    }
    if( m_pDraw->DeleteMenuCursorLayout() == false )
    {
      return false;
    }
    m_pMenuCursor->DeleteResource();
    GFL_SAFE_DELETE( m_pMenuCursor );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuWindow
 * @brief   メニューウィンドウ生成
 * @date    2015.12.12
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::CreateMenuWindow(void)
{
  m_pMenuWindow = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::MenuWindow( m_heap, m_renderingManager, m_pMenuCursorBuffer );
  m_pMenuWindow->SetListener( this );
  m_pDraw->AddSubView( m_pMenuWindow );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuWindow
 * @brief   メニューウィンドウ削除
 * @date    2015.12.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::DeleteMenuWindow(void)
{
  if( m_pMenuWindow != NULL )
  {
    if( m_pMenuWindow->IsModuleFree() == false )
    {
      return false;
    }
    m_pMenuWindow->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pMenuWindow );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneListView
 * @brief   ペインリストビュー生成
 * @date    2015.12.12
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::CreatePaneListView(void)
{
  m_pPaneListView = GFL_NEW(m_heap->GetDeviceHeap()) app::tool::PaneListView( m_heap, true );
  m_pPaneListView->SetListener( this );
  m_pDraw->InitPaneList( m_pPaneListView, m_appParam->list_pos, m_appParam->list_scroll );
  SetPaneListInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneListView
 * @brief   ペインリストビュー削除
 * @date    2015.12.12
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::DeletePaneListView(void)
{
  if( m_pPaneListView != NULL )
  {
    m_pDraw->EndPaneList( m_pPaneListView );
    GFL_SAFE_DELETE( m_pPaneListView );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.12.10
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::IsFadeEnd(void)
{
  gfl2::Fade::FadeManager * man = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  if( man->IsEnd(gfl2::Fade::DISP_UPPER) == false || man->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartYesNo
 * @brief   "はい・いいえ"処理開始
 * @date    2015.12.26
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::StartYesNo(void)
{
  m_ynResult = YN_RESULT_NONE;
  { // はい
    gfl2::str::StrBuf * str = m_pDraw->GetString( msg_waza_omoidashi_50_02 );
    m_pMenuWindow->AddItem( *str );
  }
  { // いいえ
    gfl2::str::StrBuf * str = m_pDraw->GetString( msg_waza_omoidashi_50_03 );
    m_pMenuWindow->AddItem( *str, true );
  }
  m_pMenuWindow->StartYesNoMenu( 0 );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPaneListPos
 * @brief   ペインリストの位置を取得
 * @date    2015.12.26
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::GetPaneListPos(void)
{
  u32 pos;
  f32 scroll;
  m_pPaneListView->GetPaneList()->GetCursorData( &pos, &scroll );
  m_appParam->list_pos    = static_cast<u16>( pos );
  m_appParam->list_scroll = static_cast<u16>( scroll );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListInputEnabled
 * @brief   ペインリストの入力切り替え
 * @date    2016.01.26
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SetPaneListInputEnabled( bool flg )
{
  m_pPaneListView->SetInputEnabled( flg );
  m_pPaneListView->GetPaneList()->SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2015.12.10
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnd
 * @brief   終了設定
 * @date    2015.12.14
 *
 * @param   waza  技番号
 * @param   mode  終了モード
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SetEnd( WazaNo waza, EndMode mode )
{
  m_appParam->select_no = waza;
  m_appParam->end_mode  = mode;
  ChangeSequence( SEQ_END );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.12.10
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqStart(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->Start();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsStart() == false )
    {
      break;
    }
    if( m_appParam->delete_pos != pml::MAX_WAZA_NUM )
    {
      m_pMenuCursor->SetVisible( false );
    }
    SetFadeIn();
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( IsFadeEnd() == false )
    {
      break;
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

    if( m_appParam->delete_pos == pml::MAX_WAZA_NUM )
    {
      m_pDraw->SetInputEnabled( true );
      SetPaneListInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    else
    {
      m_pDraw->SetInputEnabled( false );
      SetPaneListInputEnabled( false );
      ChangeSequence( SEQ_WAZA_CHANGE );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqDecide
 * @brief   アップデート：技決定処理
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqDecide(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartMessage();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsMessageWindowAnimeEnd() == false )
    {
      break;
    }
    m_pDraw->SetVisibleMessage( true );
    m_pDraw->SetMessage( WazaOmoidasiDraw::MSGID_SELECT, m_selectWazaNo, WAZANO_NULL );
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pDraw->PrintMessage() != false )
    {
      break;
    }
    StartYesNo();
    m_mainSeq++;
    break;

  case 3:
    if( m_ynResult == YN_RESULT_YES )
    {
      if( m_appParam->pp->GetWazaCount() == pml::MAX_WAZA_NUM )
      {
        ChangeSequence( SEQ_CALL_WAZASELECT );
      }
      else
      {
        SetWazaNo();
        m_pDraw->SetMessage( WazaOmoidasiDraw::MSGID_DECIDE, m_selectWazaNo, WAZANO_NULL );
        m_mainSeq++;
      }
    }
    else if( m_ynResult == YN_RESULT_NO )
    {
      ChangeSequence( SEQ_RETURN_MAIN );
    }
    break;

  case 4:
    if( m_pDraw->PrintMessage() != false )
    {
      break;
    }
    SetEnd( m_selectWazaNo, END_DECIDE );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqReturnMain
 * @brief   アップデート：メッセージ表示を終了してメインへ戻る
 * @date    2015.12.14
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqReturnMain(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->EndMessage();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsMessageWindowAnimeEnd() == false )
    {
      break;
    }
    m_pDraw->SetVisibleMessage( false );
    m_pDraw->SetInputEnabled( true );
    SetPaneListInputEnabled( true );
    m_pMenuCursor->SetVisible( true );
    ChangeSequence( SEQ_MAIN );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqCallWazaSelect
 * @brief   アップデート：技忘れ画面へ
 * @date    2015.12.26
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqCallWazaSelect(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->SetMessage( WazaOmoidasiDraw::MSGID_CHANGE_START, m_selectWazaNo, WAZANO_NULL );
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->PrintMessage() != false )
    {
      break;
    }
    GetPaneListPos();
    SetEnd( m_selectWazaNo, END_CHANGE );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqWazaChange
 * @brief   アップデート：技を忘れて覚える
 * @date    2015.12.26
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqWazaChange(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_selectWazaNo = m_appParam->select_no;
    m_pDraw->StartMessage();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsMessageWindowAnimeEnd() == false )
    {
      break;
    }
    m_pDraw->SetVisibleMessage( true );
    m_pDraw->SetMessage( WazaOmoidasiDraw::MSDID_CHANGE_DECIDE, m_selectWazaNo, m_appParam->pp->GetWazaNo(m_appParam->delete_pos) );
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pDraw->PrintMessage() != false )
    {
      break;
    }
    SetEnd( m_selectWazaNo, END_DECIDE );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqAppCancel
 * @brief   アップデート：思い出し画面キャンセル
 * @date    2015.12.26
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::Update_SeqAppCancel(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartMessage();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsMessageWindowAnimeEnd() == false )
    {
      break;
    }
    m_pDraw->SetVisibleMessage( true );
    m_pDraw->SetMessage( WazaOmoidasiDraw::MSGID_CANCEL, WAZANO_NULL, WAZANO_NULL );
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pDraw->PrintMessage() != false )
    {
      break;
    }
    StartYesNo();
    m_mainSeq++;
    break;

  case 3:
    if( m_ynResult == YN_RESULT_YES )
    {
      SetEnd( WAZANO_NULL, END_CANCEL );
    }
    else if( m_ynResult == YN_RESULT_NO )
    {
      ChangeSequence( SEQ_RETURN_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2015.12.10
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool WazaOmoidasiFrame::Update_SeqEnd(void)
{
  switch( m_mainSeq )
  {
  case 0:
    SetFadeOut();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsFadeEnd() == false )
    {
      break;
    }
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力時のリスナー
 * @date    2015.12.10
 *
 * @param   id    動作ID
 *
 * @note  override WazaOmoidasiDraw::InputListener
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::OnAction( WazaOmoidasiDraw::InputListener::ActionID id )
{
  switch( id )
  {
  case WazaOmoidasiDraw::InputListener::ACTION_ID_RETURN:  // 戻る
    m_pDraw->SetInputEnabled( false );
    SetPaneListInputEnabled( false );
    m_pMenuCursor->SetVisible( false );
    ChangeSequence( SEQ_APP_CANCEL );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetWazaNo
 * @brief   技をセット
 * @date    2015.12.26
 *
 * @note  override WazaOmoidasiDraw::InputListener
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SetWazaNo(void)
{
  u32 pos = m_appParam->pp->GetWazaCount();

  if( pos == pml::MAX_WAZA_NUM )
  {
    if( m_appParam->delete_pos >= pml::MAX_WAZA_NUM )
    {
      GFL_ASSERT( 0 );
      return;
    }
    pos = m_appParam->delete_pos;
  }
  else
  {
    // 一応、取得位置を確認しておく
    if( m_appParam->pp->GetWazaNo(pos) != WAZANO_NULL )
    {
      GFL_ASSERT( 0 );
      for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
      {
        if( m_appParam->pp->GetWazaNo(i) == WAZANO_NULL )
        {
          pos = i;
          break;
        }
      }
    }
  }

  m_appParam->pp->SetWaza( pos, m_selectWazaNo );
  m_pDraw->PutPokeWaza( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    PaneListItemSelectListener
 * @brief   ペインリスト決定時に呼ばれるリスナー
 * @date    2015.12.12
 *
 * @param   pos   カーソル位置
 *
 * @note  override app::tool::PaneListView::Listener
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::PaneListItemSelectListener( u32 pos )
{
  m_selectWazaNo = m_pDraw->GetWazaNo( pos );
  m_pDraw->PutListWazaInfo( pos );
  m_pDraw->SetInputEnabled( false );
  SetPaneListInputEnabled( false );
  m_pMenuCursor->SetVisible( false );
  ChangeSequence( SEQ_DECIDE );
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー
 * @date    2015.12.12
 *
 * @param   id  メニュー項目のID
 *
 * @note  override app::tool::MenuWindowListener
 */
//-----------------------------------------------------------------------------
void WazaOmoidasiFrame::SelectMenu( u32 id )
{
  m_pMenuWindow->EndMenu();

  // はい
  if( id == 0 )
  {
    m_ynResult = YN_RESULT_YES;
  }
  // いいえ
  else
  {
    m_ynResult = YN_RESULT_NO;
  }
};


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)

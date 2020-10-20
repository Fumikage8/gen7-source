//======================================================================
/**
 * @file    TitleMenuLowerDraw.cpp
 * @date    2015/07/21 14:52:41
 * @author  fukushima_yuya
 * @brief   タイトルメニュー下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "TitleMenuLowerDraw.h"
#include "TitleMenuDrawerBase.h"

#include <AppLib/include/Tool/app_tool_TimeIcon.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <Savedata/include/SaveData.h>
#include <Savedata/include/MysteryGiftSave.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/title_menu/title_menu.h>
#include <niji_conv_header/app/title_menu/title_menu_pane.h>
#include <niji_conv_header/app/title_menu/title_menu_anim_list.h>
#include <niji_conv_header/message/msg_title_menu.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };
}


//------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------------------
TitleMenuLowerDraw::TitleMenuLowerDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap(pHeap)
  , m_pAppRenderingManager(pAppRenderingManager)
  , m_pFadeManager(NULL)
  , m_pAppParam(NULL)
  , m_pDrawerBase(NULL)
  , m_Seq(SEQ_INIT)
  , m_pPartsMsgWindow(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam       外部設定パラメータ
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   msgData         メッセージデータ
 * @param   wordSet         ワードセット
 * @param   wordSetLoader   ワードセットローダー
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader )
{
  {
    m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    m_pAppParam    = pAppParam;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  // ワードセットローダーのセット
  SetWordSetLoader( wordSetLoader );
  // 2D関連作成
  Initialize2D( pAppLytBuff, msgData, wordSet );
  // ペインの初期化
  InitializePane();
  // 初期テキスト表示
  InitializeText();

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool TitleMenuLowerDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ステータス画面を開く
      TitleMenuOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool TitleMenuLowerDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void TitleMenuLowerDraw::TitleMenuOpen( void )
{
  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void TitleMenuLowerDraw::TitleMenuClose( void )
{
  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool TitleMenuLowerDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   appLytBuff    レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::Initialize2D( void* appLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { appLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_TITLE_MENU_TITLEMENU_MIN_LOW_000_BFLYT,
      LA_TITLE_MENU_TITLEMENU_MIN_LOW_000___NUM,
      appLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    msgData,
    wordSet );

  this->SetInputEnabled( false );

  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, wordSet, msgData );
    GFL_ASSERT( m_pDrawerBase );

    // 背景アニメの再生
    m_pDrawerBase->StartAnim( LA_TITLE_MENU_TITLEMENU_MIN_LOW_000__BG_00_BG_LOOP_00 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::InitializePane( void )
{
  // メッセージウィンドウ
  m_pPartsMsgWindow = m_pDrawerBase->GetParts( PANENAME_TITLEMENU_MIN_LOW_000_PANE_MESSAGE_L8 );
  {
    // テキストボックス
    m_pTextMsgWindow = m_pDrawerBase->GetTextBox( PANENAME_COMMON_MSG_LOW_007_PANE_TEXTBOX_00, m_pPartsMsgWindow );

    // タイマーアイコンの表示
    gfl2::lyt::LytPane* paneTimeIcon = m_pDrawerBase->GetPane( PANENAME_COMMON_MSG_LOW_007_PANE_TIMERICON, m_pPartsMsgWindow );
    {
      m_pDrawerBase->SetVisiblePane( paneTimeIcon, true );
    }
  }

  // メッセージウィンドウを非表示
  SetVisibleMsgWindow( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::InitializeText( void )
{
  Savedata::MysteryGiftSave* mysterySave = NULL;
  {
    GameSys::GameData* gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    {
      Savedata::SaveData* sd = gd->GetSaveData();
      {
        mysterySave = sd->GetMysteryGiftSave();
      }
    }
  }

  if( m_pAppParam->initMode == INIT_MODE_NONE )
  {
    // 全ギフトの個数を取得
    u32 giftNum = (mysterySave->GetPokeGiftNum() + mysterySave->GetItemGiftNum() + mysterySave->GetBPGiftNum() + mysterySave->GetMameGiftNum());

    // 受け取れるアイテムがある
    if( giftNum > 0 )
    {
      SetMessageID( title_menu_20_01 );
      // メッセージウィンドウの表示
      SetVisibleMsgWindow( true );
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   メッセージのセット
 *
 * @param   msgID   メッセージID
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::SetMessageID( u32 msgID )
{
  this->SetTextboxPaneMessage( m_pTextMsgWindow, msgID );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージウィンドウの表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::SetVisibleMsgWindow( bool visible )
{
  m_pDrawerBase->SetVisiblePane( m_pPartsMsgWindow, visible );
}



//------------------------------------------------------------------------------
/**
 * @brief   タイマーアイコンのアニメを再生
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::StartTimeIcon( void )
{
  // タイマーアイコンのインアニメ再生
  m_pDrawerBase->StartAnim( LA_TITLE_MENU_TITLEMENU_MIN_LOW_000__MESSAGE_L8__TIMERICON_IN );

  gfl2::lyt::LytWk* lytwk = m_pDrawerBase->GetLytWk();
  {
    // タイマーアイコンのループアニメ再生
    App::Tool::TimeIcon::StartAnime( lytwk, LA_TITLE_MENU_TITLEMENU_MIN_LOW_000__MESSAGE_L8__TIMERICON_KEEP );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイマーアイコンのアニメの停止
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::EndTimeIcon( void )
{
  // アニメの停止
  m_pDrawerBase->StopAnim( LA_TITLE_MENU_TITLEMENU_MIN_LOW_000__MESSAGE_L8__TIMERICON_KEEP );

  // タイマーアイコンのアウトアニメ再生
  m_pDrawerBase->StartAnim( LA_TITLE_MENU_TITLEMENU_MIN_LOW_000__MESSAGE_L8__TIMERICON_OUT );
}



//------------------------------------------------------------------------------
/**
 * @brief   セーブデータフォーマット用の表示
 */
//------------------------------------------------------------------------------
void TitleMenuLowerDraw::VisibleSavedataFormat( void )
{
  // メッセージのセット
  SetMessageID( title_menu_30_01 );

  // タイマーアイコンの再生
  StartTimeIcon();

  // メッセージウィンドウの表示
  SetVisibleMsgWindow( true );
}

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

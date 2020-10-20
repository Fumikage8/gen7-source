//======================================================================
/**
 * @file    FinderMenuCommentListDraw.cpp
 * @date    2015/11/02 19:28:33
 * @author  fukushima_yuya
 * @brief   評価リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuCommentListDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <GameSys/include/GameData.h>
#include <Field/FieldRo/include/TrialModel/FieldFinderModel_define.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_ROTOM_ANIM,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    BTN_ID_NEXT,
    BTN_ID_NUM,
  };

//  static const u8 ORDER_COMMENT_LIST_DRAW = 130;
  static const u8 ORDER_COMMENT_LIST_DRAW = 128;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuCommentListDraw::FinderMenuCommentListDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pPhotoData(NULL)
  , m_pNextBtn(NULL)
  , m_pPaneListView(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   appParam      外部設定パラメータ
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgData  = msgData;
  m_pWordSet  = wordSet;
  m_pAppParam = appParam;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuCommentListDraw::End( void )
{
  if( m_pPaneListView )
  {
    if( m_pPaneListView->GetPaneList() )
    {
      // スクロールバーの破棄
      m_pPaneListView->DeleteScrollBar();

      // ペインリストの破棄
      m_pPaneListView->DeletePaneList();
    }

    GFL_SAFE_DELETE( m_pPaneListView );
  }

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
void FinderMenuCommentListDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_ROTOM_ANIM:
    {
      this->SetInputEnabled( true );

      // 次へボタン復帰
      m_pDrawerBase->SetVisiblePane( m_pNextBtn, true );
      m_pDrawerBase->GetButtonManager()->SetButtonInputEnable( BTN_ID_NEXT, true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pPaneListView )
      {
        m_pPaneListView->Update();
      }
    } break;

  case SEQ_OUT:
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
void FinderMenuCommentListDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, ORDER_COMMENT_LIST_DRAW );

  if( m_pPaneListView )
  {
    m_pPaneListView->Draw( no );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderMenuCommentListDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::Open( void )
{
  // レイアウトの表示
  this->SetVisible( true );

  m_Seq = SEQ_ROTOM_ANIM;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::Close( void )
{
  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 * @param   photoIdx    写真インデックス
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::Setup( u32 photoIdx )
{
  // 評価対象の写真データ
  m_pPhotoData = m_pAppParam->photo_data[photoIdx];

  u32 listMax = 0;

  for( u32 i=0; i<15; ++i )
  {
    if( m_pPhotoData->data.comment[i] > 0 )
    {
      listMax++;
    }
  }

  // ペインリストの生成
  CreatePaneList( listMax );
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_COMMENTLIST_LOW_000_BFLYT,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet);

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  // ボタンマネージャの生成
  CreateButtonManager();

  // ペインリストの初期化
  InitPaneList();

  // 次へボタン
  m_pNextBtn = m_pDrawerBase->GetPane( PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_NEXTBUTTON );
  m_pDrawerBase->SetVisiblePane( m_pNextBtn, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::InitializeText( void )
{
  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_NEXTBUTTON );
  gfl2::lyt::LytTextBox* btnText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_018_PANE_TEXTBOX_00, btnParts );

  this->SetTextboxPaneMessage( btnText, msg_pokefinder_30_02 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの初期化
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::InitPaneList( void )
{
  static const u32 c_ListParts[] = {
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_00,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_01,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_02,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_03,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_04,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_05,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_06,
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_LIST_07,
  };

  for( u32 i=0; i<LIST_MAX; ++i )
  {
    app::tool::PaneList::PANE_DATA* data = &m_paneData[i];
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_ListParts[i] );

    data->base  = parts;
    data->text  = m_pDrawerBase->GetTextBox( PANENAME_FINDER_LIST_LOW_000_PANE_TEXTBOX_00, parts );
    data->bound = m_pDrawerBase->GetBounding( PANENAME_FINDER_LIST_LOW_000_PANE_BOUND_00, parts );
    data->cursor_pos = NULL;
    
    data->holdAnime    = app::tool::ButtonManager::ANIMATION_NULL;
    data->releaseAnime = app::tool::ButtonManager::ANIMATION_NULL;
    data->cancelAnime  = app::tool::ButtonManager::ANIMATION_NULL;
    data->selectAnime  = app::tool::ButtonManager::ANIMATION_NULL;
  }

  m_pPaneListView = GFL_NEW(m_pDrawerBase->GetDeviceHeap()) app::tool::PaneListView( m_pDrawerBase->GetHeap() );
  GFL_ASSERT( m_pPaneListView );
  m_pPaneListView->SetListener( this );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの生成
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CreatePaneList( u32 listMax )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();

  // リストパラメータのセット
  app::tool::PaneList::SETUP_PARAM list_setup;
  SetParamList( &list_setup, listMax );

  // スクロール値の計算
  u32 max = 0;
  if( list_setup.valMax > list_setup.listPosMax )
  {
    max = list_setup.valMax - list_setup.listPosMax;
  }

  // スクロールバーパラメータのセット
  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup = {
    devHeap,
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    max,

    m_pDrawerBase->GetLytWk(),
    m_pDrawerBase->GetG2D()->GetLayoutResourceID(0),
    PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL, NULL, NULL
  };

  // PaneListViewのセットアップ
  m_pPaneListView->CreatePaneList( &list_setup );
  m_pPaneListView->CreateScrollBar( &scroll_bar_setup );

  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    // コンフィグ
    paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_NONE, false, true, false, false );
    // SE設定
    paneList->SetSE(
      ((max > 0) ? SEQ_SE_SELECT1 : app::tool::PaneList::SE_NONE),
      app::tool::PaneList::SE_NONE,
      app::tool::PaneList::SE_NONE );
    // 十字キー入力を無効
    paneList->SetCrossKeyEnable( false ); //!< @fix NMCat[473]
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの更新
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::UpdatePaneList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  u32 cmtIdx = m_pPhotoData->data.comment[pos];
  gfl2::lyt::LytTextBox* textBox = pane[pane_index].text;

  this->SetTextboxPaneMessage( textBox, cmtIdx );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータのセット
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::SetParamList( app::tool::PaneList::SETUP_PARAM* param, u32 listMax )
{
  param->heap        = m_pDrawerBase->GetDeviceHeap();;
  param->pLytWk      = m_pDrawerBase->GetLytWk();
  param->paneList    = m_paneData;
  param->paneListMax = LIST_MAX;
  param->valMax      = listMax;       //!< 実際に使用するリストの項目数（ペインの数ではない）

  const u32 listPosMax = 6;
  const u32 listPos    = listPosMax;
  f32 listScroll = 0.0f;

  if( listMax > listPosMax )
  {
    listScroll = static_cast<f32>(listMax - listPosMax);
  }

  param->listPosMax = listPosMax;       //!< カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos    = listPos;          //!< 初期カーソル位置 ( 0 〜 listPosMax-1 の範囲 )
  param->listScroll = listScroll;       //!< 初期スクロール位置 ( 0 〜 valMax-listPosMax-1 の範囲 )

  param->defPY = 100.0f;                //!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX = 320.0f;                //!< ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = 34.0f;                 //!< ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;             //!< 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = NULL;          //!< 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  param->cbFunc = this;                 //!< コールバック関数
  param->cbWork = NULL;                 //!< コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();   //!< UIデバイスマネージャ
  param->menuCursor = NULL;   //!< カーソル
}







//==============================================================================
/**
 *      ↓　以下がリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuCommentListDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( paneId == BTN_ID_NEXT )
  {
    m_uiListener->OpenScore();
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル移動時のコールバック
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{

}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルを非表示にするコールバック関数
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{

}

//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画するコールバック関数
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  UpdatePaneList( pane, pane_index, pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   入れ替え用コールバック関数
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{

}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリスト決定時に呼ばれるリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::PaneListItemSelectListener( u32 pos )
{

}





//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void FinderMenuCommentListDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_NEXT, NULL, NULL,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_TOUCH,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_RELEASE,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_CANSEL,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_ACTIVE,
      LA_FINDER_MENU_FINDER_COMMENTLIST_LOW_000__NEXTBUTTON_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FINDER_COMMENTLIST_LOW_000_PANE_NEXTBUTTON, PANENAME_COMMON_BTN_LOW_018_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    u32 partsIndex = c_ButtonPaneIndex[i][0];
    u32 paneIndex  = c_ButtonPaneIndex[i][1];

    gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( partsIndex );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts;
    info[i].bound_pane = m_pDrawerBase->GetPane( paneIndex, pParts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  btnMan->SetButtonBindKey( BTN_ID_NEXT, gfl2::ui::BUTTON_A, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  btnMan->SetButtonInputEnable( BTN_ID_NEXT, false );

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

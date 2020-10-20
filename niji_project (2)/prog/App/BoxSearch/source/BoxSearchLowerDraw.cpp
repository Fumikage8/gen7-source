//======================================================================
/**
 * @file    BoxSearchLowerDraw.cpp
 * @date    2015/07/27 12:59:25
 * @author  fukushima_yuya
 * @brief   ボックス検索：下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchLowerDraw.h"
#include "BoxSearchDrawListener.h"
#include "BoxSearchDrawerBase.h"

#include "App/BoxSearch/source/BoxSearchConst.h"

#include <App/BoxSearch/source/parts/BoxSearchItemList.h>
#include <App/BoxSearch/source/parts/BoxSearchContentsList.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <str/include/gfl2_Str.h>

// arc
#include <niji_conv_header/app/box/res2d/srclow.h>
#include <niji_conv_header/app/box/res2d/srclow_pane.h>
#include <niji_conv_header/app/box/res2d/srclow_anim_list.h>
#include <niji_conv_header/message/msg_box.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

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

  //! @brief  ボタンID
  enum
  {
    BTN_ID_CANCEL,    //!< キャンセル
    BTN_ID_SEARCH,    //!< さがす
    BTN_ID_RESET,     //!< リセット
    BTN_ID_NUM,
  };

  //! @brief  画面表示アニメ
  static const u32 c_DispAnimIndex[] = {
    LA_SRCLOW_BOX_SRC_LOW_000_PATERN_01,
    LA_SRCLOW_BOX_SRC_LOW_000_PATERN_00,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
BoxSearchLowerDraw::BoxSearchLowerDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_uiListener( NULL )
  , m_pFadeManager( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager) )
  , m_pAppParam( NULL )
  , m_pDrawerBase(NULL)
  , m_pCursorItem( NULL )
  , m_pCursorContents( NULL )
  , m_pItemList( NULL )
  , m_pContentsList( NULL )
  , m_DispMode( DISP_MODE_TOP )
  , m_Seq( SEQ_INIT )
  , m_InputExclusive(false)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::Init( InitParam& param )
{
  {
    m_pAppParam       = param.appParam;
    m_pCursorItem     = param.cursor_item;
    m_pCursorContents = param.cursor_cont;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  // 2D関連作成
  Initialize2D( param );
  // クラスの初期化
  InitializeClass( param );
  // 初期テキスト表示
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_SRCLOW_BOX_SRC_LOW_000_BFLYT,
      LA_SRCLOW_BOX_SRC_LOW_000___NUM,
      param.lytBuff,
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
    param.msgData_Main,
    param.wordSet );

  m_InputExclusive = this->GetInputExclusiveKeyByButtonAnimation();
  this->SetInputExclusiveKeyByButtonAnimation( true );

  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, param.msgData_Main, param.wordSet );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager();
  }

  // カーソルのセットアップ
  app::util::G2DUtil* g2d = m_pDrawerBase->GetG2D();
  {
    if( m_pCursorItem )
    {
      m_pCursorItem->SetupCursor( g2d, NULL, 0, gfl2::lyt::DISPLAY_LOWER );
    }
    if( m_pCursorContents )
    {
      m_pCursorContents->SetupCursor( g2d, NULL, 0, gfl2::lyt::DISPLAY_LOWER );
    }
  }

  // ペインリストの生成
  CreatePaneList( param );

  SetDispMode( DISP_MODE_TOP );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::InitializeText( void )
{
  // リセット
  gfl2::lyt::LytParts* resetBtn = m_pDrawerBase->GetParts( PANENAME_BOX_SRC_LOW_000_PANE_BTN_RESET );
  gfl2::lyt::LytTextBox* resetText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00, resetBtn );

  this->SetTextboxPaneMessage( resetText, msg_box_search_01_01 );

  // さがす
  gfl2::lyt::LytParts* okBtn = m_pDrawerBase->GetParts( PANENAME_BOX_SRC_LOW_000_PANE_BTN_OK );
  gfl2::lyt::LytTextBox* okText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00, okBtn );

  this->SetTextboxPaneMessage( okText, msg_box_search_01_02 );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool BoxSearchLowerDraw::End( void )
{
  if( m_pContentsList )
  {
    m_pContentsList->Terminate();

    GFL_SAFE_DELETE( m_pContentsList );
  }
  if( m_pItemList )
  {
    m_pItemList->Terminate();

    GFL_SAFE_DELETE( m_pItemList );
  }
  GFL_SAFE_DELETE( m_pDrawerBase );

  this->SetInputExclusiveKeyByButtonAnimation( m_InputExclusive );

  Delete2D();
  DeleteGraphicsSystem();
  
  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_pCursorItem )     m_pCursorItem->Update();
  if( m_pCursorContents ) m_pCursorContents->Update();
  if( m_pContentsList )   m_pContentsList->Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ステータス画面を開く
      BoxSearchOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->EndBoxSearch();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil(); 
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }

  if( m_pCursorItem )     m_pCursorItem->Draw( no );
  if( m_pCursorContents ) m_pCursorContents->Draw( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void BoxSearchLowerDraw::BoxSearchOpen( void )
{
  // フェードイン
  if( m_pFadeManager )
  {
    m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
  }

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void BoxSearchLowerDraw::BoxSearchClose( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // フェードアウト
  if( m_pFadeManager )
  {
    gfl2::math::Vector4 color( 0, 0, 0, 255 );
    m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
  }

  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool BoxSearchLowerDraw::IsEndInOutAnim( bool in )
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
 * @brief   ペインリストの生成
 *
 * @param   initParam   初期化パラメータ
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::CreatePaneList( InitParam& initParam )
{
  Savedata::BOX* boxSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  {
    // 検索項目リスト
    ItemList::CreateParam itemListCreateParam;
    {
      ItemList::CreateParam* param = &itemListCreateParam;
      {
        param->appParam   = m_pAppParam;
        param->base       = m_pDrawerBase;
        param->boxSave    = boxSave;
        param->cursor     = m_pCursorItem;
        param->listener   = this;
      }
    }

    m_pItemList = GFL_NEW(heap) ItemList( itemListCreateParam );
    {
      GFL_ASSERT( m_pItemList );

      // 初期化
      m_pItemList->Initialize();
    }

    // 検索内容リスト
    ContentsList::CreateParam contentsListCreateParam;
    {
      ContentsList::CreateParam* param = &contentsListCreateParam;
      {
        param->appParam         = m_pAppParam;
        param->base             = m_pDrawerBase;
        param->boxSave          = boxSave;
        param->cursor           = m_pCursorContents;
        param->msgData_Initial  = initParam.msgData_Initial;
        param->listener         = this;
        param->drawListener     = m_uiListener;
      }
    }

    m_pContentsList = GFL_NEW(heap) ContentsList( contentsListCreateParam );
    {
      GFL_ASSERT( m_pContentsList );

      // 初期化
      m_pContentsList->Initialize();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードのセット
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::SetDispMode( DispMode dispMode )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    // ボタン入力の設定変更
    btnMan->SetButtonInputEnable( BTN_ID_SEARCH, (dispMode == DISP_MODE_TOP) );
    btnMan->SetButtonInputEnable( BTN_ID_RESET,  (dispMode == DISP_MODE_TOP) );
  }

  // 表示の変更
  m_pDrawerBase->StartAnim( c_DispAnimIndex[dispMode] );

  m_DispMode = dispMode;
}



//------------------------------------------------------------------------------
/**
 * @brief   キーアクション
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::KeyAction_Top( gfl2::ui::Button* button )
{
  if( button->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    // ペインリストの入力を無効
    SetPaneListEnable_ItemList( false );
    // ボタンアクションの開始
    ButtonSelectAction( BTN_ID_CANCEL );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    // ペインリストの入力を無効
    SetPaneListEnable_ItemList( false );
    // ボタンアクションの開始
    ButtonSelectAction( BTN_ID_SEARCH );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    // ペインリストの入力を無効
    SetPaneListEnable_ItemList( false );
    // ボタンアクションの開始
    ButtonSelectAction( BTN_ID_RESET );
  }
}

void BoxSearchLowerDraw::KeyAction_Cond( gfl2::ui::Button* button )
{
  if( button->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    // ペインリストの入力を無効
    SetPaneListEnable_ContentsList( false );
    // ボタンアクションの開始
    ButtonSelectAction( BTN_ID_CANCEL );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの有効設定
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::SetPaneListEnable_ItemList( bool enable )
{
  if( m_pItemList )
  {
    // 検索項目の入力設定
    m_pItemList->SetInputEnable( enable );
  }
}

void BoxSearchLowerDraw::SetPaneListEnable_ContentsList( bool enable )
{
  if( m_pContentsList )
  {
    // 検索条件項目の入力設定
    m_pContentsList->SetInputEnable( enable );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインリストのリザルトの取得
 */
//------------------------------------------------------------------------------
app::tool::PaneList::Result BoxSearchLowerDraw::GetPaneListResult( void )
{
  app::tool::PaneList::Result result = app::tool::PaneList::RET_NONE;

  if( m_DispMode == DISP_MODE_TOP )
  {
    if( m_pItemList )
    {
      // 検索項目リスト
      result = m_pItemList->GetPaneListUpdateResult();
    }
  }
  else {
    if( m_pContentsList )
    {
      // 検索条件項目リスト
      result = m_pContentsList->GetPaneListUpdateResult();
    }
  }

  return result;
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンアクション
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::ButtonSelectAction( u32 btnID )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    // ボタンアクション開始
    btnMan->StartSelectedAct( btnID );
  }
}






//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BoxSearchLowerDraw::OnLayoutPaneEvent( const u32 paneId )
{
  GFL_UNUSED( paneId );

  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      //if( !m_pSearchList->PrevHierarchy() ) //!< 検索トップ画面でキャンセルが押されたら終了へ
      //{
      //  m_uiListener->SetEndMode( END_MODE_CANCEL );

      //  BoxSearchClose();
      //}
      if( m_DispMode == DISP_MODE_TOP )
      {
        m_uiListener->SetEndMode( END_MODE_CANCEL );

        BoxSearchClose();
      }
      else if( m_DispMode == DISP_MODE_COND )
      {
        if( m_pContentsList )
        {
          m_pContentsList->PrevHierarchy();
        }
      }
    } break;

  case BTN_ID_SEARCH:
    {
      m_uiListener->SetEndMode( END_MODE_SEARCH );

      BoxSearchClose();
    } break;

  case BTN_ID_RESET:
    {
      //m_pSearchList->ResetCondition();
      if( m_DispMode == DISP_MODE_TOP )
      {
        m_pItemList->ResetSearchParam();

        return MYSUBVIEW_ONLY_ACCESS;
      }
    } // no break;
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペイン選択アニメーションの開始の検知
 * @param   paneId
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  // @fix GFNMCat[3313]
  if( paneId == BTN_ID_CANCEL )
  {
    // 動作中のペインリストの入力を無効に
    if( m_DispMode == DISP_MODE_TOP )
    {
      if( m_pItemList )
      {
        m_pItemList->SetInputEnable( false );
      }
    }
    else if( m_DispMode == DISP_MODE_COND )
    {
      if( m_pContentsList )
      {
        m_pContentsList->SetInputEnable( false );
      }
    }
  }
  else if( paneId == BTN_ID_RESET )
  {
    // 動作中のペインリストの入力を無効に
    if( m_DispMode == DISP_MODE_TOP )
    {
      if( m_pItemList )
      {
        m_pItemList->SetInputEnable( false );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BoxSearchLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  app::tool::PaneList::Result result = GetPaneListResult();

#if PM_DEBUG
  static const char* c_PaneListUpdateResultName[] = {
    "RET_NONE：動作なし",
    "RET_LIST_UP：上へスクロール中",
    "RET_LIST_DOWN：下へスクロール中",
    "RET_LIST_PAGE_UP：上へページ送り",
    "RET_LIST_PAGE_DOWN：下へページ送り",
    "RET_LIST_TOUCH：タッチ処理中",
    "RET_LIST_AUTO_SCRILL：オートスクロール中",
    "RET_LIST_SELECT：項目選択",
    "RET_LIST_CANCEL：キャンセル ( Bボタン )",
    "RET_LIST_CATCH：項目キャッチ",
    "RET_LIST_PROCESSING：なんらかの処理を実行中 ( Aボタンアニメなど )",
  };

  if( m_DispMode == DISP_MODE_TOP )
  {
    FUKUSHIMA_PRINT( "ItemList UpdateResult = [%s]\n",
      c_PaneListUpdateResultName[result] );
  }
  else {
    FUKUSHIMA_PRINT( "ContentsList UpdateResult = [%s]\n",
      c_PaneListUpdateResultName[result] );
  }
#endif

  if( result != app::tool::PaneList::RET_NONE )
  {
    // @fix NMCat[1459]
    // @fix NMCat[2375]
    return ENABLE_ACCESS;
  }

  if( m_DispMode == DISP_MODE_TOP )
  {
    // 検索項目
    KeyAction_Top( pButton );
  }
  else {
    // 検索条件項目
    KeyAction_Cond( pButton );
  }

  return ENABLE_ACCESS;
}


//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::OnPreLoadResource( void )
{
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}






//==============================================================================
/**
 *      ↓　以下が検索リストリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   検索項目リストの表示
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::OpenItemList( void )
{
  if( m_pContentsList )
  {
    m_pContentsList->DeleteList();
  }

  if( m_pItemList )
  {
    m_pItemList->CreateList();
  }

  if( m_uiListener )
  {
    m_uiListener->SetUpperMessage_ItemList();
  }

  // 画面モードの変更
  SetDispMode( DISP_MODE_TOP );
}

//------------------------------------------------------------------------------
/**
 * @brief   検索内容リストの表示
 *
 * @param   srcID   検索項目ID
 */
//------------------------------------------------------------------------------
void BoxSearchLowerDraw::OpenContentsList( u32 srcID )
{
  if( m_pItemList )
  {
    m_pItemList->DeleteList();
  }

  if( m_pContentsList )
  {
    m_pContentsList->SetItemListID( srcID );
    m_pContentsList->CreateList();
  }

  // 画面モードの変更
  SetDispMode( DISP_MODE_COND );
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
void BoxSearchLowerDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_TOUCH,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_CANSEL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_ACTIVE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_BACK_PASSIVE,
    },
    {
      BTN_ID_SEARCH, NULL, NULL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_TOUCH,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_CANSEL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_TOUCH_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_ACTIVE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_OK_PASSIVE,
    },
    {
      BTN_ID_RESET, NULL, NULL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_TOUCH,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_CANSEL,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_TOUCH_RELEASE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_ACTIVE,
      LA_SRCLOW_BOX_SRC_LOW_000__BTN_RESET_PASSIVE,
    },
  };

  const u32 c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_BOX_SRC_LOW_000_PANE_BTN_BACK,  PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { PANENAME_BOX_SRC_LOW_000_PANE_BTN_OK,    PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00 },
    { PANENAME_BOX_SRC_LOW_000_PANE_BTN_RESET, PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  {
    // ボタンマネージャに登録するパラメータの設定
    app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(heap) app::ui::ButtonInfoEx[BTN_ID_NUM];
    {
      GFL_ASSERT( info );
      
      for( u32 i=0; i<BTN_ID_NUM; ++i )
      {
        const u32* pAry = c_ButtonPaneIndex[i];

        info[i] = c_DefInfo[i];
        {
          gfl2::lyt::LytParts* parts =
            m_pDrawerBase->GetParts( pAry[0] );
          
          info[i].picture_pane = parts;
          info[i].bound_pane   = m_pDrawerBase->GetPane( pAry[1], parts );
        }
      }
    }
    
    // ボタンマネージャの生成
    app::ui::UIView* view = m_pDrawerBase->GetUIView();
    {
      view->CreateButtonManager( m_pHeap, m_pDrawerBase->GetLytWk(), info, BTN_ID_NUM );
    }
    
    GFL_DELETE_ARRAY info;
  }
  
  app::tool::ButtonManager* btnMan = app::ui::UIResponder::GetButtonManager();
  {
    //btnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B );
    //btnMan->SetButtonBindKey( BTN_ID_SEARCH, gfl2::ui::BUTTON_Y );
    //btnMan->SetButtonBindKey( BTN_ID_RESET,  gfl2::ui::BUTTON_X );
    
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      if( i == BTN_ID_CANCEL )
      {
        btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
      }
      else {
        btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
      }
    }
  }
}

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

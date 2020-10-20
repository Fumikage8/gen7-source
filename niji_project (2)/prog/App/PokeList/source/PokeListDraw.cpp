//==============================================================================
/**
 * @file    PokeListDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面(フィールド用)
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListDraw.h"
#include "App/PokeList/source/PokeListDrawListener.h"
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListItemExchange.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListPokeExchange.h"
#include "App/PokeList/source/parts/PokeListWindow.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/SystemFont.h>
#include <Sound/include/Sound.h>

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

enum CancelBtnState
{
  CANCEL_STATE_NONE,
  CANCEL_STATE_TOUCH,
  CANCEL_STATE_CANCEL,
  CANCEL_STATE_CANCEL_WAIT,
  CANCEL_STATE_RELEASE,
  CANCEL_STATE_NUM,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListDraw::PokeListDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager, App::Tool::ItemEffect* itemEffect )
  : PokeListBaseDraw( pHeap, pAppRenderingManager )
  , m_pPokeExchange( NULL )
  , m_pItemExchange( NULL )
  , m_pFormChange( NULL )
  , m_pItemEffect(itemEffect)
  , m_CancelBtnState(CANCEL_STATE_NONE)
  , m_TouchEnable(true)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListDraw::~PokeListDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeListDraw::EndFunc( void )
{
  if( m_pDrawerBase )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();
    app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();

    if( pokeIcon && !pokeIcon->EndFunc() ) return false;
    if( itemIcon && !itemIcon->EndFunc() ) return false;

    m_pDrawerBase->DeletePokeIcon();
    m_pDrawerBase->DeleteItemIcon();
  }

  if( m_pFormChange )
  {
    if( !m_pFormChange->DeleteFunc() ) return false;

    GFL_SAFE_DELETE( m_pFormChange );
  }

  GFL_SAFE_DELETE( m_pItemExchange );
  GFL_SAFE_DELETE( m_pPokeExchange );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void PokeListDraw::UpdateFunc( void )
{
  if( m_pPokeExchange ) m_pPokeExchange->Update();
  if( m_pItemExchange ) m_pItemExchange->Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( !IsSetupUIReady() ) break;

      // ポケモンリストを開く
      PokeListOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim(true) ) break;

      // カーソルの表示
      if( m_pCursor )
      {
        m_pCursor->SetCursorPos( m_pStaticWork->GetInitCursorPos() );
        m_pCursor->SetVisible( true );
      }

      // 入力の有効化
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pLocalWork->GetDispMode() != DISP_MODE_NORMAL )
      {
        InputedCancelBtn();
      }

      // キャンセルボタンの状態変更
      UpdateCancelBtn();
    } break;

  case SEQ_OUT:
    {
      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->PokeListEnd();    //!< ポケモンリストの終了を通知

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画処理
 */
//------------------------------------------------------------------------------
void PokeListDraw::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = m_pDrawerBase->GetG2D();

  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, m_LayoutID );

  if( m_pCursor ) m_pCursor->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   UIのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListDraw::SetupUI( void )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  m_MemberCount = m_pStaticWork->GetMemberCount();

  // 背景の変更
  m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_COLOR_FIELD );

  {
    // ポケモンアイコンの生成
    app::tool::PokeIconToolEx::PokeIconToolParam poke_icon_param;
    {
      poke_icon_param.pUIView = this;
      poke_icon_param.pHeap   = m_pHeap;
      poke_icon_param.iconMax = POKE_ICON_NUM;
      poke_icon_param.sync    = true;
    }
    m_pDrawerBase->CreatePokeIcon( poke_icon_param );

    // アイテムアイコンの生成
    app::tool::ItemIconToolEx::ItemIconToolParam item_icon_param;
    {
      item_icon_param.pUIView = this;
      item_icon_param.pHeap   = m_pHeap;
      item_icon_param.iconMax = ITEM_ICON_NUM;
      item_icon_param.sync    = true;
    }
    m_pDrawerBase->CreateItemIcon( item_icon_param );
  }

  // プレートの作成
  {
    // プレートの有効設定
    SetPlateEnable();

    for( u8 i=0; i<PLATE_NUM; ++i )
    {
      m_pPokePlate[i] = GFL_NEW(devHeap) PlateDraw( m_pDrawerBase, static_cast<CursorPos>(i) );
      GFL_ASSERT( m_pPokePlate[i] );

      if( i < m_MemberCount )
      {
        m_pPokePlate[i]->Setup( PLATE_MODE_PARAM );
      }
      else {
        m_pPokePlate[i]->SetVisible( false );
        app::tool::ButtonManager * bm = m_pDrawerBase->GetButtonManager();
        bm->SetButtonInputEnable( BTN_ID_PLATE_TL + i, false );
      }
    }
  }

  // カーソル
  {
    m_pCursor = GFL_NEW(devHeap) CursorDraw( m_pDrawerBase, m_pAppRenderingManager, m_MemberCount );
    {
      GFL_ASSERT( m_pCursor );

      m_pCursor->SetCursorID( CID_POKE );
      m_pCursor->SetCursorMoveDefault();

      for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
      {
        m_pCursor->SetCursorEnable( i, m_PlateEnable[i] );
      }
    }
  }

  // メッセージウィンドウ
  m_pMessageWindow = GFL_NEW(devHeap) MessageWindow( m_pDrawerBase );
  GFL_ASSERT( m_pMessageWindow );

  // ポケモン入れ替え
  m_pPokeExchange = GFL_NEW(devHeap) PokeExchange( m_pDrawerBase, m_pPokePlate, m_pCursor, m_pMessageWindow, m_MemberCount );
  GFL_ASSERT( m_pPokeExchange );

  // 道具入れ替え
  m_pItemExchange = GFL_NEW(devHeap) ItemExchange( m_pDrawerBase, m_pPokePlate, m_pCursor, m_pMessageWindow, m_MemberCount );
  GFL_ASSERT( m_pItemExchange );

  // 機能ボタン、通信用のパーツを非表示
  {
    const gfl2::lyt::LytPaneIndex c_PokeListPartsIndex[] = {
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00,
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_01,
    };

    for( u8 i=0; i<GFL_NELEMS(c_PokeListPartsIndex); ++i )
    {
      m_pVisiblePane[i] = m_pDrawerBase->GetPane( c_PokeListPartsIndex[i] );
      m_pDrawerBase->SetVisiblePane( m_pVisiblePane[i], (i == 0) );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  if( btnMan )
  {
    // X, Yボタンの設定
    btnMan->SetButtonBindKey( BTN_ID_X, gfl2::ui::BUTTON_X ); //!< 道具入れ替え
    btnMan->SetButtonBindKey( BTN_ID_Y, gfl2::ui::BUTTON_Y ); //!< ポケモン入れ替え
    
    btnMan->SetButtonPassive( BTN_ID_DECIDE );
    // ボタンの入力判定を無効
    btnMan->SetButtonInputEnable( BTN_ID_DECIDE, false );  //!< 参加選択の決定ボタン

    // メンバーの数が１より大きければ入れ替えのボタンを有効：要望対応
    if( m_pStaticWork->GetMemberCount() > 1 )
    {
      btnMan->SetButtonInputEnable( BTN_ID_X, true );
      btnMan->SetButtonInputEnable( BTN_ID_Y, true );
      btnMan->SetButtonVisible( BTN_ID_X );
      btnMan->SetButtonVisible( BTN_ID_Y );
    }
    // メンバーの数が１ならば入れ替えのボタンを無効：要望対応
    else {
      btnMan->SetButtonInputEnable( BTN_ID_X, false );
      btnMan->SetButtonInputEnable( BTN_ID_Y, false );
      btnMan->SetButtonInvisible( BTN_ID_X );
      btnMan->SetButtonInvisible( BTN_ID_Y );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   UIのセットアップ完了待ち
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListDraw::IsSetupUIReady( void )
{
  if( m_pCursor )
  {
    if( !m_pCursor->IsReady() ) return false;
  }

  if( m_pPokeWindow )
  {
    if( !m_pPokeWindow->IsReady() ) return false;
  }
  else {
    gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();

    // ポケリストウィンドウ
    m_pPokeWindow = GFL_NEW(devHeap) PokeListWindow( m_pDrawerBase, m_pPokePlate, m_pCursor, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pPokeWindow );

      m_pPokeWindow->Initialize();
      m_pPokeWindow->SetupModeField( m_pItemExchange, m_pMessageWindow );
      m_pPokeWindow->SetUIListener( m_uiListener );
    }

    return false;
  }

  if( m_pFormChange == NULL )
  {
    gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();

    // フォルムチェンジ
    m_pFormChange = GFL_NEW(devHeap) FormChange( m_pDrawerBase, m_pPokePlate, m_pPokeWindow, m_MemberCount, m_pItemEffect );
    GFL_ASSERT( m_pFormChange );
    m_pItemExchange->SetFormChange( m_pFormChange );

    return false;
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void PokeListDraw::InitializeText( void )
{
  gfl2::lyt::LytParts* pParts = NULL;
  gfl2::lyt::LytTextBox* pTextBox = NULL;
  
  switch( m_pStaticWork->GetProcMode() )
  {
  case MODE_FIELD:
    {
      // X
      pParts = m_pDrawerBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTSBUTTON_X );
      pTextBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00, pParts );
  
      this->SetTextboxPaneMessage( pTextBox, mes_pokelist_01_31 );  //!< ならびかえ
  
      // Y
      pParts = m_pDrawerBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTSBUTTON_Y );
      pTextBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00, pParts );
  
      this->SetTextboxPaneMessage( pTextBox, mes_pokelist_01_30 );  //!< ならびかえ
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの入力判定処理
 *
 * @param   touchPanel    タッチパネル
 */
//------------------------------------------------------------------------------
bool PokeListDraw::InputCancelBtn( gfl2::ui::TouchPanel* touchPanel )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    if( touchPanel->IsTouchTrigger() && m_CancelBtnState == CANCEL_STATE_NONE )
    {
      if( btnMan->IsButtonHit( BTN_ID_CANCEL, touchPanel->GetX(), touchPanel->GetY() ) )
      {
        m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH );

        m_CancelBtnState = CANCEL_STATE_TOUCH;
      }
    }
    else if( touchPanel->IsTouch() )
    {
      if( m_CancelBtnState == CANCEL_STATE_TOUCH )
      {
        if( !btnMan->IsButtonHit( BTN_ID_CANCEL, touchPanel->GetX(), touchPanel->GetY() ) )
        {
          m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_CANSEL );

          m_CancelBtnState = CANCEL_STATE_CANCEL;
        }
      }
      else if( m_CancelBtnState == CANCEL_STATE_CANCEL )
      {
        if( btnMan->IsButtonHit( BTN_ID_CANCEL, touchPanel->GetX(), touchPanel->GetY() ) )
        {
          m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_TOUCH );

          m_CancelBtnState = CANCEL_STATE_TOUCH;
        }
      }
    }
    else if( touchPanel->IsTouchRelease() )
    {
      if( m_CancelBtnState == CANCEL_STATE_TOUCH )
      {
        m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_RELEASE );

        Sound::PlaySE( SEQ_SE_CANCEL1 );

        m_TouchEnable = false;  //!< タッチの入力だけを無効  // @fix NMCat[688]
        m_CancelBtnState = CANCEL_STATE_RELEASE;
      }
      else if( m_CancelBtnState == CANCEL_STATE_CANCEL )
      {
        m_CancelBtnState = CANCEL_STATE_CANCEL_WAIT;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの入力終了処理
 */
//------------------------------------------------------------------------------
void PokeListDraw::InputedCancelBtn( void )
{
  if( m_CancelBtnState == CANCEL_STATE_CANCEL_WAIT )
  {
    if( m_pDrawerBase->IsAnimEnd( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_CANSEL ) )
    {
      m_CancelBtnState = CANCEL_STATE_NONE;
    }
  }
  else if( m_CancelBtnState == CANCEL_STATE_RELEASE )
  {
    if( m_pDrawerBase->IsAnimEnd( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_BACK_RELEASE ) )
    {
      DISP_MODE dispMode = m_pLocalWork->GetDispMode();

      if( dispMode == DISP_MODE_POKE_EXCHANGE )
      {
        if( m_pPokeExchange->Cancel() )
        {
          m_pLocalWork->SetDispMode( DISP_MODE_NORMAL );
        }
      }
      else if( dispMode == DISP_MODE_ITEM_EXCHANGE )
      {
        m_pItemExchange->Cancel();
      }

      m_TouchEnable = true; //!< 入力を有効  // @fix NMCat[688]
      m_CancelBtnState = CANCEL_STATE_NONE;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンの更新処理
 */
//------------------------------------------------------------------------------
void PokeListDraw::UpdateCancelBtn( void )
{
  bool isDrag = false;
  {
    if( m_pPokeExchange )   // @fix cov_ctr[10156]
    {
      isDrag |= m_pPokeExchange->IsDragMode();
    }
    if( m_pItemExchange )   // @fix cov_ctr[10155]
    {
      isDrag |= m_pItemExchange->IsDragMode();
    }

    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
    {
      // キャンセルボタンのアクティブ状態を取得
      bool active = btnMan->IsButtonActive( BTN_ID_CANCEL );

      // ドラッグ中はパッシブ状態に変更
      if( active && isDrag )
      {
        btnMan->SetButtonPassive( BTN_ID_CANCEL );
      }
      // ドラッグ中でなければアクティブ状態に変更
      else if( !active && !isDrag )
      {
        btnMan->SetButtonActive( BTN_ID_CANCEL );
      }
    }
  }
}






//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション(通常時)
 * @param   paneId    ペインID
 */
//------------------------------------------------------------------------------
void PokeListDraw::PaneEventNormal( const u32 paneId )
{
  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      PokeListClose();
    } break;

  case BTN_ID_PLATE_TL:
  case BTN_ID_PLATE_TR:
  case BTN_ID_PLATE_ML:
  case BTN_ID_PLATE_MR:
  case BTN_ID_PLATE_BL:
  case BTN_ID_PLATE_BR:
    {
      CursorPos cpos = static_cast<CursorPos>( paneId - BTN_ID_PLATE_TL );
      m_pCursor->SetCursorPos( cpos );
      if( m_pPokeWindow ) m_pPokeWindow->Open( MENU_POKE );
    } break;

  case BTN_ID_X:
    {
      if( m_pItemExchange ) m_pItemExchange->StartExchange();
      m_pLocalWork->SetDispMode( DISP_MODE_ITEM_EXCHANGE );
    } break;

  case BTN_ID_Y:
    {
      if( m_pPokeExchange ) m_pPokeExchange->StartExchange();
      m_pLocalWork->SetDispMode( DISP_MODE_POKE_EXCHANGE );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション(ポケモン入れ替え)
 * @param   paneId    ペインID
 */
//------------------------------------------------------------------------------
void PokeListDraw::PaneEventPokeExchange( const u32 paneId )
{
  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      if( m_pPokeExchange->Cancel() )
      {
        m_pLocalWork->SetDispMode( DISP_MODE_NORMAL );
      }

      m_TouchEnable = true;
    } break;

  case BTN_ID_PLATE_TL:
  case BTN_ID_PLATE_TR:
  case BTN_ID_PLATE_ML:
  case BTN_ID_PLATE_MR:
  case BTN_ID_PLATE_BL:
  case BTN_ID_PLATE_BR:
    {
      CursorPos cpos = static_cast<CursorPos>( paneId - BTN_ID_PLATE_TL );

      m_pPokeExchange->Decide( cpos );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション(アイテム入れ替え)
 * @param   paneId    ペインID
 */
//------------------------------------------------------------------------------
void PokeListDraw::PaneEventItemExchange( const u32 paneId )
{
  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      m_pItemExchange->Cancel();

      m_TouchEnable = true;
    } break;

  case BTN_ID_PLATE_TL:
  case BTN_ID_PLATE_TR:
  case BTN_ID_PLATE_ML:
  case BTN_ID_PLATE_MR:
  case BTN_ID_PLATE_BL:
  case BTN_ID_PLATE_BR:
    {
      CursorPos cpos = static_cast<CursorPos>( paneId - BTN_ID_PLATE_TL );

      m_pItemExchange->Decide( cpos );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション(ウィンドウ)
 * @param   paneId    ペインID
 */
//------------------------------------------------------------------------------
void PokeListDraw::PaneEventWindow( const u32 paneId )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント(ポケモン入れ替え)
 * @param   pTouchPanel   タッチパネル
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::TouchEventNormal( gfl2::ui::TouchPanel* pTouchPanel )
{
  if( m_pPokeExchange )
  {
    if( m_pPokeExchange->TouchEvent( pTouchPanel ) )
    {
      return MYSUBVIEW_ONLY_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント(ポケモン入れ替え)
 * @param   pTouchPanel   タッチパネル
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::TouchEventPokeExchange( gfl2::ui::TouchPanel* pTouchPanel )
{
  // キャンセルボタン
  if( m_pPokeExchange->GetSequence() == PokeExchange::SEQ_POKE_MOVE_SELECT
    && InputCancelBtn( pTouchPanel ) )
  {
    return DISABLE_ACCESS;
  }

  if( m_TouchEnable )  // @fix NMCat[688]
  {
    if( m_pPokeExchange )
    {
      if( m_pPokeExchange->TouchEvent( pTouchPanel ) )
      {
        return MYSUBVIEW_ONLY_ACCESS;
      }
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント(アイテム入れ替え)
 * @param   pTouchPanel   タッチパネル
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::TouchEventItemExchange( gfl2::ui::TouchPanel* pTouchPanel )
{
  // キャンセルボタン
  if( m_pItemExchange->GetSequence() == ItemExchange::SEQ_ITEM_MOVE_SELECT
    && InputCancelBtn( pTouchPanel ) )
  {
    return DISABLE_ACCESS;
  }

  if( m_TouchEnable )  // @fix NMCat[688]
  {
    if( m_pItemExchange )
    {
      if( m_pItemExchange->TouchEvent( pTouchPanel ) )
      {
        return MYSUBVIEW_ONLY_ACCESS;
      }
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチイベント(ウィンドウ)
 * @param   pTouchPanel   タッチパネル
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::TouchEventWindow( gfl2::ui::TouchPanel* pTouchPanel )
{
  return ENABLE_ACCESS;
}






//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void PokeListDraw::OnPreLoadResource( void )
{
  // num_font.bffntの登録
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();

  m_bCreatedNumFont = true;
}
  
//------------------------------------------------------------------------------
/**
 * @brief   タッチパネルイベントの検知
 * @param   pTouchPanel
 * @param   isTouch
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  app::ui::UIInputListener::ListenerResult ( PokeListDraw::*POKELIST_TOUCH_EVENT[DISP_MODE_NUM] )( gfl2::ui::TouchPanel* ) = {
    &PokeListDraw::TouchEventNormal,
    &PokeListDraw::TouchEventPokeExchange,
    &PokeListDraw::TouchEventItemExchange,
    &PokeListDraw::TouchEventWindow,
  };

  DISP_MODE dispMode = m_pLocalWork->GetDispMode();

  return ( this->*POKELIST_TOUCH_EVENT[dispMode] )( pTouchPanel );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペイン選択アニメーションの開始の検知
 * @param   paneId
 */
//------------------------------------------------------------------------------
void PokeListDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  DISP_MODE dispMode = m_pLocalWork->GetDispMode();

  if( dispMode == DISP_MODE_POKE_EXCHANGE || dispMode == DISP_MODE_ITEM_EXCHANGE )
  {
    if( paneId == BTN_ID_CANCEL )
    {
      m_TouchEnable = false;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  FUKUSHIMA_PRINT( "pane_Id = %d\n", paneId );

  void ( PokeListDraw::*POKELIST_PANE_EVENT[DISP_MODE_NUM] )( const u32 ) = {
    &PokeListDraw::PaneEventNormal,
    &PokeListDraw::PaneEventPokeExchange,
    &PokeListDraw::PaneEventItemExchange,
    &PokeListDraw::PaneEventWindow,
  };

  DISP_MODE dispMode = m_pLocalWork->GetDispMode();

  ( this->*POKELIST_PANE_EVENT[dispMode] )( paneId );

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  DISP_MODE dispMode = m_pLocalWork->GetDispMode();

  // カーソルの更新
  if( dispMode != DISP_MODE_WINDOW )
  {
    if( m_pCursor ) m_pCursor->InputButton( pButton );

    // カーソル終端停止処理
    //MoveCursor( pKey );
    //CursorID cid = m_pStaticWork->GetCursorID();
    //CursorPos curPos = m_pStaticWork->GetCursorPos( cid );
    //m_pCursor->SetCursorPos( curPos );
  }

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    // カーソル位置の取得
    CursorPos cpos = m_pCursor->GetCursorPos();

    if( cpos <= CPOS_PLATE_BR )
    {
      btnMan->StartSelectedAct( BTN_ID_PLATE_TL + cpos );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    btnMan->StartSelectedAct( BTN_ID_CANCEL );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    if( dispMode == DISP_MODE_NORMAL )
    {
      if( btnMan->IsButtonInputEnable( BTN_ID_Y ) )
      {
        btnMan->StartSelectedAct( BTN_ID_Y );
      }
    }
    // @fix 調整要望[270]
    else if( dispMode == DISP_MODE_POKE_EXCHANGE )
    {
      // カーソル位置の取得
      CursorPos cpos = m_pCursor->GetCursorPos();

      if( cpos <= CPOS_PLATE_BR )
      {
        btnMan->StartSelectedAct( BTN_ID_PLATE_TL + cpos );
      }
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    if( dispMode == DISP_MODE_NORMAL )
    {
      if( btnMan->IsButtonInputEnable( BTN_ID_X ) )
      {
        btnMan->StartSelectedAct( BTN_ID_X );
      }
    }
    // @fix 調整要望[270]
    else if( dispMode == DISP_MODE_ITEM_EXCHANGE )
    {
      // カーソル位置の取得
      if( m_pCursor )   // @fix cov_ctr[10160]
      {
        CursorPos cpos = m_pCursor->GetCursorPos();

        if( cpos <= CPOS_PLATE_BR )
        {
          btnMan->StartSelectedAct( BTN_ID_PLATE_TL + cpos );
        }
      }
    }
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

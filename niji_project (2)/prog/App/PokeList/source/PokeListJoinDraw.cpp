//==============================================================================
/**
 * @file    PokeListJoinDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面シンプル描画(バトル・対象選択)
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListJoinDraw.h"
#include "App/PokeList/source/PokeListDrawListener.h"
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/parts/PokeListWindow.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <Print/include/SystemFont.h>
#include <Sound/include/Sound.h>

#if PM_DEBUG
#include <System/include/gflUse.h>
#endif

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListJoinDraw::PokeListJoinDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : PokeListBaseDraw( pHeap, pAppRenderingManager )
  , m_JoinedCount( 0 )
  , m_pDecideBtnParts( NULL )
  , m_pJoinCountText( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListJoinDraw::~PokeListJoinDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeListJoinDraw::EndFunc( void )
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

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::UpdateFunc( void )
{
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

      // 参加数表示の更新
      UpdateJoinCount();

      // 入力の有効化
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      // 参加数表示の更新
      UpdateJoinCount();
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
void PokeListJoinDraw::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = m_pDrawerBase->GetG2D();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, m_LayoutID );
  }

  if( m_pCursor ) m_pCursor->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   UIのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::SetupUI( void )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  m_MemberCount = m_pStaticWork->GetMemberCount();

  // 背景の変更
  m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_COLOR_NET );

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

        if( m_pStaticWork->IsJoinPoke(i) )
        {
          PlateMessageWindow* msgWnd = m_pPokePlate[i]->GetPlateMessageWindow();
          msgWnd->SetupMessageWindow( PLATE_MODE_JOIN );
        }
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
      m_pCursor->SetCursorMoveDecide();

      for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
      {
        m_pCursor->SetCursorEnable( i, m_PlateEnable[i] );
      }
    }
  }

  // 決定ボタン
  m_pDecideBtnParts = m_pDrawerBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_OK );
  //m_pDrawerBase->SetVisiblePane( m_pDecideBtnParts, false );

  // 参加数表示テキスト
  m_pJoinCountText = m_pDrawerBase->GetTextBox( PANENAME_POKELIST_MIN_LOW_000_PANE_TEXTBOX_00 );

  // 機能ボタン、通信用のパーツを非表示
  {
    const gfl2::lyt::LytPaneIndex c_PokeListPartsIndex[] = {
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00,
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_01,
    };

    for( u8 i=0; i<GFL_NELEMS(c_PokeListPartsIndex); ++i )
    {
      gfl2::lyt::LytPane* pPane = m_pDrawerBase->GetPane( c_PokeListPartsIndex[i] );
      m_pDrawerBase->SetVisiblePane( pPane, (i == 1) );
    }

    m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTSMSG_00_OUT );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::SetupButton( app::tool::ButtonManager* pBtnMan )
{
  if( pBtnMan )
  {
    // ボタンの入力判定を無効
    pBtnMan->SetButtonInputEnable( BTN_ID_X, false );
    pBtnMan->SetButtonInputEnable( BTN_ID_Y, false );

    // 決定ボタンをパッシブ状態に
    pBtnMan->SetButtonPassive( BTN_ID_DECIDE );
    pBtnMan->SetButtonInputEnable( BTN_ID_DECIDE, false );

    ModeContext context = m_pStaticWork->GetModeContext();
    pBtnMan->SetButtonInputEnable( BTN_ID_CANCEL, context.join.cancel_enable );
    if( !context.join.cancel_enable )
    {
      pBtnMan->SetButtonInvisible( BTN_ID_CANCEL );
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
bool PokeListJoinDraw::IsSetupUIReady( void )
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
    gfl2::heap::HeapBase* heap = m_pDrawerBase->GetDeviceHeap();

    // ポケリストウィンドウ
    m_pPokeWindow = GFL_NEW(heap) PokeListWindow( m_pDrawerBase, m_pPokePlate, m_pCursor, m_pAppRenderingManager );
    {
      GFL_ASSERT( m_pPokeWindow );

      m_pPokeWindow->Initialize();
      m_pPokeWindow->SetUIListener( m_uiListener );
    }

    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::InitializeText( void )
{
  app::ui::UIView* view = m_pDrawerBase->GetUIView();

  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_OK );
  {
    gfl2::lyt::LytTextBox* btnText = m_pDrawerBase->GetTextBox( PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, btnParts );
    {
      view->SetTextboxPaneMessage( btnText, mes_pokelist_01_21 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューIDの取得
 * @param   cpos    プレート位置
 */
//------------------------------------------------------------------------------
MenuID PokeListJoinDraw::GetMenuID( CursorPos cpos )
{
  StaticWork* pStaticWork = m_pDrawerBase->GetStaticWork();
  u8 pokeIndex = pStaticWork->GetPokeIndex( cpos );

  MenuID menuID = MENU_JOIN;

  if( !pStaticWork->IsJoinEnable( pokeIndex ) )
  {
    menuID = MENU_JOIN_DISABLE;
  }
  else if( pStaticWork->IsJoinPoke( pokeIndex ) )
  {
    menuID = MENU_JOIN_ALREADY;
  }

  return menuID;
}

//------------------------------------------------------------------------------
/**
 * @brief   レギュレーションNGメッセージの取得
 *
 * @param   result    レギュレーション結果
 *
 * @return  メッセージID
 */
//------------------------------------------------------------------------------
s32 PokeListJoinDraw::GetRegulationNGMsgID( PokeRegulation::PokelistCheckReturn result )
{
  // NGメッセージ
  const s32 c_RegNgMsgID[] = {
    -1,
    -1,
    mes_pokelist_07_04, //!< 伝説３匹以上
    mes_pokelist_07_05, //!< 参加指定ポケモンが未参加
    mes_pokelist_07_06, //!< 拡張レギュレーション違反
  };

  GFL_ASSERT( result > PokeRegulation::RET_NUM_FAILED );

  return c_RegNgMsgID[ result ];
}

//------------------------------------------------------------------------------
/**
 * @brief   参加ポケモン数、参加可能ポケモン数の表示
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::UpdateJoinCount( void )
{
  if( !m_pDrawerBase ) return;

  u8 joinMax      = m_pStaticWork->GetModeContext().join.join_poke_count_max;   //!< 参加可能最大数
  u8 joinMin      = m_pStaticWork->GetModeContext().join.join_poke_count_min;   //!< 参加可能最少数
  u8 joinedCount  = m_pStaticWork->GetJoinPokeCount();                          //!< 参加済みのポケモン数

  if( m_JoinedCount != joinedCount )
  {
    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

    // 参加可能最低数以上になったら決定ボタンを表示
    if( joinMin <= joinedCount )
    {
      if( m_pCursor ) m_pCursor->SetCursorEnable( CPOS_DECIDE, true );    // @fix cov_ctr[10165]

      btnMan->SetButtonActive( BTN_ID_DECIDE );
      btnMan->SetButtonInputEnable( BTN_ID_DECIDE, true );
    }
    else {
      if( m_pCursor ) m_pCursor->SetCursorEnable( CPOS_DECIDE, false );    // @fix cov_ctr[10165]

      btnMan->SetButtonPassive( BTN_ID_DECIDE );
      btnMan->SetButtonInputEnable( BTN_ID_DECIDE, false );
    }

    // 参加可能最大数の場合カーソルを決定キーへ移動
    if( joinedCount == joinMax )
    {
      if( m_pCursor ) m_pCursor->SetCursorPos( CPOS_DECIDE );    // @fix cov_ctr[10165]
    }

    // 参加数を更新
    m_JoinedCount = joinedCount;
  }

  // 参加数の表示
  print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
  {
    wordSet->RegisterNumber( 0, joinedCount, 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    wordSet->RegisterNumber( 1, joinMax, 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  }

  app::ui::UIView* view = m_pDrawerBase->GetUIView();
  {
    view->SetTextboxPaneMessage( m_pJoinCountText, mes_pokelist_30_01 );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   キーアクション
 * @param   pButton
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::KeyActionNormal( gfl2::ui::Button* pButton )
{
  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( m_pCursor )   // @fix cov_ctr[10182]
    {
      // カーソル位置の取得
      CursorPos cpos = m_pCursor->GetCursorPos();

      if( cpos <= CPOS_DECIDE )
      {
        pBtnMan->StartSelectedAct( BTN_ID_PLATE_TL + cpos );
      }
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    pBtnMan->StartSelectedAct( BTN_ID_CANCEL );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   レギュレーションチェック
 */
//------------------------------------------------------------------------------
bool PokeListJoinDraw::CheckRegulation( void )
{
  // レギュレーションチェック
  PokeRegulation::PokelistCheckReturn result = PokeRegulation::RET_OK;
  {
    gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
    {
      result = static_cast<PokeRegulation::PokelistCheckReturn>( m_pStaticWork->CheckRegulation( heap ) );
    }
  }

#if 0
#if PM_DEBUG
  GFL_ASSERT( result != PokeRegulation::RET_NUM_FAILED );
#ifdef DEBUG_ONLY_FOR_fukushima_yuya
  static const PokeRegulation::PokelistCheckReturn c_RegulationResultTable[] = {
    PokeRegulation::RET_OVER_LEGEND,
    PokeRegulation::RET_NO_MASTPOKE,
    PokeRegulation::RET_EX_SCRIPT,
  };
  u32 rand = System::GflUse::GetPublicRand() % GFL_NELEMS(c_RegulationResultTable);
  {
    result = c_RegulationResultTable[rand];
    FUKUSHIMA_PRINT( "result=[%d]\n", result );
  }
#endif
#endif
#endif

  if( result != PokeRegulation::RET_OK )
  {
    s32 msgID = GetRegulationNGMsgID( result );

    // メッセージウィンドウオープン
    if( msgID != -1 )
    {
      // 伝説ポケモン数の表示
      if( result == PokeRegulation::RET_OVER_LEGEND )
      {
        // 伝説ポケモンの上限数を取得
        u32 val = m_pStaticWork->GetRegulationLegendNum();

        app::util::G2DUtil* g2d = m_pDrawerBase->GetG2D();
        {
          g2d->SetRegisterNumber( 0, val, 1 );
        }
      }

      // メッセージウィンドウをメッセージオンリーで開く
      m_pPokeWindow->OpenMsgOnly( PokeListWindow::MSG_TYPE_MSG_ONLY, msgID );
      m_pPokeWindow->RequestButtonDisable();    // @fix GFNMCat[5661]
      m_pPokeWindow->RequestSetWindowMode();    // @fix GFNMCat[5661]
      
      return false; //!< レギュレーション違反
    }
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   参加ポケモンの決定
 */
//------------------------------------------------------------------------------
void PokeListJoinDraw::DecideJoinPoke( void )
{
  ModeContext context = m_pStaticWork->GetModeContext();

  if( context.join.join_upp_mode != JOIN_UPP_MODE_BTL_TREE )
  {
    // 通信待機中を表示
    m_pPokeWindow->SetMessage( PokeListWindow::MSG_TYPE_MSG_ONLY, mes_pokelist_10_01 );
    m_pPokeWindow->OpenMsgOnly();
    m_pPokeWindow->StartTimeIcon();

    m_pStaticWork->SetEndMode( END_MODE_DECIDE );
    m_pStaticWork->SetResultJoinPokeIndex( true );

    // 自分の選択が終了したことを通知
    m_uiListener->FinishJoinSelect();

    // 入力を無効にしてイベント側から終了通知がくるまで待機する
    this->SetInputEnabled( false );
  }
  // バトルツリーでは通信待機を待つことは無い
  else {
    m_pStaticWork->SetResultJoinPokeIndex( true );

    PokeListClose( END_MODE_DECIDE );
  }
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
void PokeListJoinDraw::OnPreLoadResource( void )
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
app::ui::UIInputListener::ListenerResult PokeListJoinDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( m_pLocalWork->GetDispMode() == DISP_MODE_WINDOW )
  {
    //// 入力待ちの場合ならウィンドウを閉じる
    //if( m_pPokeWindow->GetInputWait() )
    //{
    //  m_pPokeWindow->Close();
    //}
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListJoinDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      // ポケモンリストを閉じる
      PokeListClose();

      return DISABLE_ACCESS;
    } //break;

  case BTN_ID_PLATE_TL:
  case BTN_ID_PLATE_TR:
  case BTN_ID_PLATE_ML:
  case BTN_ID_PLATE_MR:
  case BTN_ID_PLATE_BL:
  case BTN_ID_PLATE_BR:
    {
      CursorPos cpos = static_cast<CursorPos>( paneId - BTN_ID_PLATE_TL );
      m_pCursor->SetCursorPos( cpos );

      MenuID menuID = GetMenuID( cpos );
      if( m_pPokeWindow ) m_pPokeWindow->Open( menuID );
    } break;

  case BTN_ID_DECIDE:
    {
      // レギュレーションチェック
      if( !CheckRegulation() )
      {
        break;  //!< レギュレーション違反
      }

      // 参加ポケモンの決定
      DecideJoinPoke();
    } break;
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListJoinDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( m_pPokeWindow->IsWindowIdle() )
  {
    // カーソルの更新
    if( m_pCursor ) m_pCursor->InputButton( pButton );

    //MoveCursor( pKey );

    //// 仮対処
    //CursorID cid = m_pStaticWork->GetCursorID();
    //CursorPos curPos = m_pStaticWork->GetCursorPos( cid );
    //m_pCursor->SetCursorPos( curPos );

    // 入力待ちの場合ならウィンドウを閉じる
    //if( m_pPokeWindow->GetInputWait() )
    //{
    //  m_pPokeWindow->Close();
    //  return ENABLE_ACCESS;
    //}

    KeyActionNormal( pButton );
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

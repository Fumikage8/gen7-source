//==============================================================================
/**
 * @file    PokeListSimpleDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面シンプル描画(バトル・対象選択)
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListSimpleDraw.h"
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

// arc
#include <arc_def_index/arc_def.h>

#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>

#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! @brief  シーケンス
enum {
  SEQ_READY_DRAW = SEQ_END + 1,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListSimpleDraw::PokeListSimpleDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : PokeListBaseDraw( pHeap, pAppRenderingManager )
{
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_PlateEnable[i] = false;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListSimpleDraw::~PokeListSimpleDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::EndFunc( void )
{
  if( m_pDrawerBase )
  {
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();
    {
      if( pokeIcon )
      {
        if( !pokeIcon->EndFunc() )
        {
          return false;
        }

        // ポケモンアイコンの破棄
        m_pDrawerBase->DeletePokeIcon();
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::UpdateFunc( void )
{
  // モードの取得
  Mode mode = m_pStaticWork->GetProcMode();

  if( mode != MODE_BATTLE )
  {
    // MODE_SIMPLE, MODE_SIMPLE2
    UpdateFunc_Simple();
  }
  else {
    // MODE_BATTLE
    UpdateFunc_Battle();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：対象選択
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::UpdateFunc_Simple( void )
{
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // 準備シーケンス
      if( !UpdateFunc_SeqReady() )
      {
        break;    //!< 準備完了していない
      }

      // メッセージウィンドウを開く
      m_pMessageWindow->Open();

      // フェードイン
      PokeListOpen();

      m_Seq = SEQ_IN;
    } // no break;

  case SEQ_IN:
    {
      // フェードインシーケンス
      UpdateFunc_SeqFadeIn();
    } break;

  case SEQ_OUT:
    {
      // フェードアウトシーケンス
      UpdateFunc_SeqFadeOut();
    } break;

  case SEQ_END_READY:
    {
      // 終了準備シーケンス
      UpdateFunc_SeqEndReady();
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：戦闘時
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::UpdateFunc_Battle( void )
{
  switch( m_Seq )
  {
  case SEQ_OUT:
    {
      // フェードアウトシーケンス
      UpdateFunc_SeqFadeOut();
    } break;

  case SEQ_END_READY:
    {
      // 終了準備シーケンス
      UpdateFunc_SeqEndReady();
    } break;

  default:
    {
      const ModeContext& context = m_pStaticWork->GetModeContext();

      if( context.battle.from_top )
      {
        // トップ画面からの遷移
        UpdateFunc_BattleFromTop();
      }
      else {
        // ステータス画面からの遷移
        UpdateFunc_BattleFromStatus();
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：戦闘時(戦闘トップから遷移)
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::UpdateFunc_BattleFromTop( void )
{
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // 準備シーケンス
      if( !UpdateFunc_SeqReady() )
      {
        break;    //!< 準備完了していない
      }

      // メッセージウィンドウを開く
      m_pMessageWindow->Open();

      m_Seq = SEQ_IN;
    } break;

  case SEQ_IN:
    {
      // フェードインシーケンス
      UpdateFunc_SeqFadeIn();

      if( m_uiListener )
      {
        // 描画フラグの有効化
        m_uiListener->SetEnableFlag_Draw();
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：戦闘時(ステータスから遷移)
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::UpdateFunc_BattleFromStatus( void )
{
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // 準備シーケンス
      if( !UpdateFunc_SeqReady() )
      {
        break;    //!< 準備完了していない
      }

      // メッセージウィンドウを開く
      m_pMessageWindow->Open();

      if( m_uiListener )
      {
        // 描画フラグの有効化
        m_uiListener->SetEnableFlag_Draw();
      }

      m_Seq = SEQ_READY_DRAW;
    } break;

  case SEQ_READY_DRAW:
    {
      // ポケリストを開く
      PokeListOpen();

      m_Seq = SEQ_IN;
    } break;

  case SEQ_IN:
    {
      // フェードインシーケンス
      UpdateFunc_SeqFadeIn();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新処理：準備シーケンス
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::UpdateFunc_SeqReady( void )
{
  // ポケモンアイコン
  app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();
  {
    if( pokeIcon )
    {
      // 更新
      pokeIcon->Update();

      // 差し替えが完了したかどうか
      if( !pokeIcon->IsAllReplaced() )
      {
        return false;
      }
    }
  }

  // メニューカーソル
  if( m_pCursor )
  {
    if( !m_pCursor->IsReady() )
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：フェードインシーケンス
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::UpdateFunc_SeqFadeIn( void )
{
  // フェード処理の終了待ち
  if( !IsEndInOutAnim(true) )
  {
    return false;
  }

  // カーソルの表示
  if( m_pCursor )
  {
    m_pCursor->SetCursorPos( m_pStaticWork->GetInitCursorPos() );
    m_pCursor->SetVisible( true );
  }

  // 入力の有効化
  this->SetInputEnabled( true );

  m_Seq = SEQ_MAIN;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：フェードアウトシーケンス
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::UpdateFunc_SeqFadeOut( void )
{
  if( m_pCursor )
  {
    m_pCursor->EndMenuCursor();
  }

  m_Seq = SEQ_END_READY;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新処理：終了準備シーケンス
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::UpdateFunc_SeqEndReady( void )
{
  if( m_pCursor )
  {
    if( !m_pCursor->IsEnd() )
    {
      return false;
    }
  }

  if( m_uiListener )
  {
    m_uiListener->PokeListEnd();    //!< ポケモンリストの終了を通知
  }

  m_Seq = SEQ_END;

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   描画処理
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = m_pDrawerBase->GetG2D();
  {
    if( m_uiListener && m_uiListener->GetEnableFlag_Draw() )
    {
      g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, m_LayoutID );
    }
  }

  if( m_pCursor ) m_pCursor->Draw( no );
}

//------------------------------------------------------------------------------
/**
 * @brief   UIのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::SetupUI( void )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  m_MemberCount = m_pStaticWork->GetMemberCount();

  ModeContext context = m_pStaticWork->GetModeContext();
  Mode mode = m_pStaticWork->GetProcMode();

  if( mode == MODE_BATTLE )
  {
    // 背景の変更
    m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_COLOR_BATTLE );
  }
  else {    
    // 背景の変更
    m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_COLOR_FIELD );
  }

  {
    // ポケモンアイコンの生成
    app::tool::PokeIconToolEx::PokeIconToolParam poke_icon_param;
    {
      poke_icon_param.pUIView = this;
      poke_icon_param.pHeap   = m_pHeap;
      poke_icon_param.iconMax = POKE_ICON_NUM;
    }
    m_pDrawerBase->CreatePokeIcon( poke_icon_param );
  }

  // プレートの作成
  {
    // プレートの有効設定
    SetPlateEnable();

    for( u8 i=0; i<CPOS_PLATE_NUM; ++i )    // @fix cov_ctr[11846]
    {
      m_pPokePlate[i] = GFL_NEW(devHeap) PlateDraw( m_pDrawerBase, static_cast<CursorPos>(i) );
      GFL_ASSERT( m_pPokePlate[i] );

      if( m_PlateEnable[i] )
      {
        m_pPokePlate[i]->Setup( (mode == MODE_SIMPLE2) ? PLATE_MODE_SIMPLE2 : PLATE_MODE_PARAM );

        if( mode == MODE_SIMPLE2 )
        {
          PlateMessageWindow* pMsgWindow = m_pPokePlate[i]->GetPlateMessageWindow();
          pMsgWindow->SetupMessageWindow( PLATE_MODE_SIMPLE2 );
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

  {
    s32 msgID = mes_pokelist_02_01;

    switch( mode )
    {
    case MODE_BATTLE:
      {
        BattleMode btl_mode = static_cast<BattleMode>( context.battle.btl_mode );

        if( btl_mode == BTL_MODE_EXCHANGE )
        {
          msgID = mes_pokelist_02_13;   //!< どのポケモンでたたかう？
        }
      } break;
    case MODE_SIMPLE:
      {
        if( context.simple.send_box_flag )
        {
          // どのポケモンをおくりますか？
          msgID = mes_pokelist_02_12;
        }
      } break;
    }

    m_pMessageWindow->SetMessage( msgID );
  }

  // 機能ボタン、通信用のパーツを非表示
  {
    const gfl2::lyt::LytPaneIndex c_PokeListPartsIndex[] = {
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_00,
      PANENAME_POKELIST_MIN_LOW_000_PANE_NULLVISIBLE_01,
    };

    for( u8 i=0; i<GFL_NELEMS(c_PokeListPartsIndex); ++i )
    {
      gfl2::lyt::LytPane* pPane = m_pDrawerBase->GetPane( c_PokeListPartsIndex[i] );
      m_pDrawerBase->SetVisiblePane( pPane, false );
    }
  }

  // キャンセルボタンの表示切替
  {
    app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();
    bool isCancel = true;

    if( m_pStaticWork->GetProcMode() == MODE_BATTLE )
    {
      isCancel = context.battle.cancel_flag;      //!< バトル用のキャンセルフラグをセット
    }
    else {
      isCancel = context.simple.cancel_enable;    //!< シンプル用のキャンセルフラグをセット
    }

    if( !isCancel )
    {
      u32 btnID = BTN_ID_CANCEL;

      pBtnMan->SetButtonInputEnable( btnID, false );
      pBtnMan->SetButtonInvisible( btnID );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::SetupButton( app::tool::ButtonManager* pBtnMan )
{
  if( pBtnMan )
  {
    // ボタンの入力判定を無効
    pBtnMan->SetButtonInputEnable( BTN_ID_X, false );
    pBtnMan->SetButtonInputEnable( BTN_ID_Y, false );
    pBtnMan->SetButtonInputEnable( BTN_ID_DECIDE, false );

    if( m_pStaticWork->GetProcMode() == MODE_SIMPLE2 )
    {
      const u32 c_PlateButtonID[] = {
        BTN_ID_PLATE_TL, BTN_ID_PLATE_TR,
        BTN_ID_PLATE_ML, BTN_ID_PLATE_MR,
        BTN_ID_PLATE_BL, BTN_ID_PLATE_BR,
      };

      ModeContext context = m_pStaticWork->GetModeContext();

      for( u32 i=0; i<m_MemberCount; ++i )
      {
        if( context.simple.poke_select_enable[i] == 0 )
        {
          pBtnMan->SetButtonSelectSE( c_PlateButtonID[i], SEQ_SE_BEEP );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::InitializeText( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 * @param   pButton
 */
//------------------------------------------------------------------------------
void PokeListSimpleDraw::KeyActionNormal( gfl2::ui::Button* pButton )
{
  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    // カーソル位置の取得
    CursorPos cpos = CPOS_NULL;
    
    if( m_pCursor )   //@fix cov_ctr[10175]
    {
      cpos = m_pCursor->GetCursorPos();
    }

    if( cpos <= CPOS_PLATE_BR )
    {
      pBtnMan->StartSelectedAct( BTN_ID_PLATE_TL + cpos );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    pBtnMan->StartSelectedAct( BTN_ID_CANCEL );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シーケンスがメイン状態かどうか
 *
 * @return  "true  = メイン"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeListSimpleDraw::IsSeqMain( void ) const
{
  return m_Seq == SEQ_MAIN;
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
void PokeListSimpleDraw::OnPreLoadResource( void )
{
  Mode mode = m_pStaticWork->GetProcMode();

  // num_font.bffntの登録
  if( mode != MODE_BATTLE )
  {
    print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
    print::SystemFont_SetLytSystemNumFont();

    m_bCreatedNumFont = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListSimpleDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  FUKUSHIMA_PRINT( "pane_Id = %d\n", paneId );

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
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
      // カーソル位置のセット
      CursorPos cpos = static_cast<CursorPos>( paneId - BTN_ID_PLATE_TL );
      m_pCursor->SetCursorPos( cpos );

      if( m_pStaticWork->GetProcMode() == MODE_SIMPLE2 )
      {
        u8 poke_index = m_pStaticWork->GetPokeIndex( cpos );
        u8 enable = m_pStaticWork->GetSimple2SelectEnable( poke_index );

        if( enable )
        {
          // 決定のデータ設定
          m_uiListener->SetupOutData( END_MODE_SELECT );

          PokeListClose( END_MODE_SELECT );

          return DISABLE_ACCESS;
        }
      }
      else {
        // 決定のデータ設定
        m_uiListener->SetupOutData( END_MODE_SELECT );

        PokeListClose( END_MODE_SELECT );

        return DISABLE_ACCESS;
      }
    } //break;
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
app::ui::UIInputListener::ListenerResult PokeListSimpleDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  // カーソルの更新
  if( m_pCursor ) m_pCursor->InputButton( pButton );

  KeyActionNormal( pButton );

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

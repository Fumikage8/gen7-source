//==============================================================================
/**
 * @file    PokeListWazaSodateyaDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面　技教え・育て屋
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListWazaSodateyaDraw.h"
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
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListWazaSodateyaDraw::PokeListWazaSodateyaDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : PokeListBaseDraw( pHeap, pAppRenderingManager )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListWazaSodateyaDraw::~PokeListWazaSodateyaDraw( void )
{
  ;
}




//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool PokeListWazaSodateyaDraw::EndFunc( void )
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
void PokeListWazaSodateyaDraw::UpdateFunc( void )
{
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( !IsSetupUIReady() ) break;

      // ポケモンリストを開く
      PokeListOpen();
      // メッセージウィンドウを開く
      m_pMessageWindow->Open();
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
void PokeListWazaSodateyaDraw::DrawFunc( gfl2::gfx::CtrDisplayNo no )
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
void PokeListWazaSodateyaDraw::SetupUI( void )
{
  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  m_MemberCount = m_pStaticWork->GetMemberCount();

  // 背景の変更
  m_pDrawerBase->StartAnim( LA_COMMON_POKELIST_POKELIST_MIN_LOW_000_COLOR_FIELD );

  Mode mode = m_pStaticWork->GetProcMode();
  {
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

      for( u8 i=0; i<CPOS_PLATE_NUM; ++i )    // @fix cov_ctr[11845]
      {
        m_pPokePlate[i] = GFL_NEW(devHeap) PlateDraw( m_pDrawerBase, static_cast<CursorPos>(i) );
        GFL_ASSERT( m_pPokePlate[i] );

        if( i < m_MemberCount )
        {
          m_pPokePlate[i]->Setup( (mode == MODE_WAZAOSHIE) ? PLATE_MODE_WAZAOSHIE : PLATE_MODE_PARAM );

          if( mode == MODE_WAZAOSHIE )
          {
            PlateMessageWindow* pMsgWindow = m_pPokePlate[i]->GetPlateMessageWindow();
            pMsgWindow->SetupMessageWindow( PLATE_MODE_WAZAOSHIE );
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
    m_pMessageWindow->SetMessage( mes_pokelist_02_01 );
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
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaDraw::SetupButton( app::tool::ButtonManager* pBtnMan )
{
  if( pBtnMan )
  {
    // ボタンの入力判定を無効
    pBtnMan->SetButtonInputEnable( BTN_ID_X, false );
    pBtnMan->SetButtonInputEnable( BTN_ID_Y, false );
    pBtnMan->SetButtonInputEnable( BTN_ID_DECIDE, false );
    
    if( m_pStaticWork->GetProcMode() == MODE_WAZAOSHIE )
    {
      const u32 c_PlateButtonID[] = {
        BTN_ID_PLATE_TL, BTN_ID_PLATE_TR,
        BTN_ID_PLATE_ML, BTN_ID_PLATE_MR,
        BTN_ID_PLATE_BL, BTN_ID_PLATE_BR,
      };

      ModeContext context = m_pStaticWork->GetModeContext();

      for( u32 i=0; i<m_MemberCount; ++i )
      {
        if( context.waza.waza_set_param[i] != 1 ) //!< 覚えられる以外はBEEPに設定
        {
          pBtnMan->SetButtonSelectSE( c_PlateButtonID[i], SEQ_SE_BEEP );
        }
      }
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
bool PokeListWazaSodateyaDraw::IsSetupUIReady( void )
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
void PokeListWazaSodateyaDraw::InitializeText( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 * @param   pButton
 */
//------------------------------------------------------------------------------
void PokeListWazaSodateyaDraw::KeyActionNormal( gfl2::ui::Button* pButton )
{
  app::tool::ButtonManager* pBtnMan = m_pDrawerBase->GetButtonManager();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    // カーソル位置の取得
    CursorPos cpos = CPOS_NULL;
    
    if( m_pCursor )   // @fix cov_ctr[10187]
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
void PokeListWazaSodateyaDraw::OnPreLoadResource( void )
{
  // num_font.bffntの登録
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();

  m_bCreatedNumFont = true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult PokeListWazaSodateyaDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  FUKUSHIMA_PRINT( "pane_Id = %d\n", paneId );

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      // キャンセルのデータ設定
      m_uiListener->SetupOutData( END_MODE_CANCEL );

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

      if( m_pStaticWork->GetProcMode() == MODE_SODATEYA )
      {
        if( m_pPokeWindow ) m_pPokeWindow->Open( MENU_SODATEYA );
      }
      else {
        u8 pokeIndex = m_pStaticWork->GetPokeIndex( cpos );
        if( m_pStaticWork->IsEggOnCursor(CID_POKE) ) break;
        if( m_pStaticWork->GetWazaSetParam(pokeIndex) != 1 ) break;

        m_uiListener->SetupOutData( END_MODE_SELECT );

        PokeListClose( END_MODE_SELECT );

        return DISABLE_ACCESS;
      }
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
app::ui::UIInputListener::ListenerResult PokeListWazaSodateyaDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  // カーソルの更新
  if( m_pCursor ) m_pCursor->InputButton( pButton );

  KeyActionNormal( pButton );

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

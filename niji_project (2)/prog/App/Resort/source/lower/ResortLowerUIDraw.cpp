//======================================================================
/**
 * @file    ResortLowerUIDraw.cpp
 * @date    2015/09/09 14:47:42
 * @author  fukushima_yuya
 * @brief   ポケリゾート：UI画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerUIDraw.h"

#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortSequenceListener.h>
#include <App/Resort/source/ResortUtility.h>

#include <AppLib/include/tool/FingerCursor.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      LowerUIDraw::BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      LowerUIDraw::BTN_ID_MENU, NULL, NULL,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_TOUCH,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_CANSEL,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_ACTIVE,
      LA_RESORT_RESORT_MIN_LOW_000__HOMEBUTTON_PASSIVE,
    },
    {
      LowerUIDraw::BTN_ID_ARROW_L, NULL, NULL,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_TOUCH,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_CANSEL,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_TOUCH_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_ACTIVE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_L_PASSIVE,
    },
    {
      LowerUIDraw::BTN_ID_ARROW_R, NULL, NULL,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_TOUCH,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_CANSEL,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_TOUCH_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_ACTIVE,
      LA_RESORT_RESORT_MIN_LOW_000__PAGECURSOR_R_PASSIVE,
    },
    {
      LowerUIDraw::BTN_ID_EVENT_STATUS, NULL, NULL,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_TOUCH,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_CANSEL,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_ACTIVE,
      LA_RESORT_RESORT_MIN_LOW_000__BUTTON_S_01_PASSIVE,
    },
  };
  
  static const u32 c_ButtonBaseIndex[LowerUIDraw::BTN_ID_NUM] = {
    PANENAME_RESORT_MIN_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_MIN_LOW_000_PANE_HOMEBUTTON,
    PANENAME_RESORT_MIN_LOW_000_PANE_PAGECURSOR_L,
    PANENAME_RESORT_MIN_LOW_000_PANE_PAGECURSOR_R,
    PANENAME_RESORT_MIN_LOW_000_PANE_BUTTON_S_01,
  };
  static const u32 c_ButtonBoundIndex[LowerUIDraw::BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerUIDraw::LowerUIDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_pFingerCursor(NULL)
  , m_pSeqListener(NULL)
  , m_DispType(0)
  , m_ProgressFlag_TouchBox(true)
  , m_ProgressFlag_Menu(true)
  , m_TempDisableBtn(false)
  , m_pBrackBG(NULL)
  , m_pPaneVisible(NULL)
{
  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    m_BeansCount[i]       = 0xFFFFFFFF;
    m_pBeansCountText[i]  = NULL;
  }

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    m_TempDisableBtnID[i] = BTN_ID_NUM;
  }

  if( m_pResortSave )
  {
    m_ProgressFlag_TouchBox = m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX );
    m_ProgressFlag_Menu     = m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerUIDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_MIN_LOW_000_BFLYT,
      LA_RESORT_RESORT_MIN_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerUIDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // 指カーソル
    m_pCursorBuff = app::tool::FingerCursor::CreateLytBinData( m_pLytHeap );
    {
      GFL_ASSERT( m_pCursorBuff );

      m_pFingerCursor = GFL_NEW(heap) app::tool::FingerCursor( m_pLytHeap, m_pCursorBuff, m_pAppRenderingManager, app::util::G2DUtil::SETUP_LOWER, 129 );
      {
        GFL_ASSERT( m_pFingerCursor );

        // 非表示
        m_pFingerCursor->SetVisible( false );

        this->AddSubView( m_pFingerCursor );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerUIDraw::InitializePane( void )
{
  m_pBrackBG      = m_pDrawerBase->GetPane( PANENAME_RESORT_MIN_LOW_000_PANE_BG_BLACK );
  m_pPaneVisible  = m_pDrawerBase->GetPane( PANENAME_RESORT_MIN_LOW_000_PANE_NULL_VISIBLE );

  static const u32 c_BeansCountTextIndex[] = {
    PANENAME_RESORT_MIN_LOW_000_PANE_MAMENUMB_00,
    PANENAME_RESORT_MIN_LOW_000_PANE_MAMENUMB_01,
    PANENAME_RESORT_MIN_LOW_000_PANE_MAMENUMB_02,
  };

  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    m_pBeansCountText[i] = m_pDrawerBase->GetTextBox( c_BeansCountTextIndex[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerUIDraw::InitializeText( void )
{
  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_MIN_LOW_000_PANE_BUTTON_S_01 );
  {
    gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, btnParts );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_button_04 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool LowerUIDraw::Terminate( void )
{
  if( m_pFingerCursor )
  {
    m_pFingerCursor->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pFingerCursor );
  }

  // @fix cov_ctr[10970]
  app::tool::FingerCursor::DeleteLytBinData( m_pCursorBuff );

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerUIDraw::Update( void )
{
  app::ui::UIView::Update();

//  this->SetInputEnabled( true );

  // 
  CheckResort();

  // ポケマメ数の表示
  UpdateBeansCount();

  // ボタンの更新
  UpdateButton();
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ数の更新
 */
//------------------------------------------------------------------------------
void LowerUIDraw::UpdateBeansCount( void )
{
  u32 beansCount[3] = { 0, 0, 0 };

  // ランクごとのポケマメ数を取得
  util::GetRankBeansCount( m_pResortSave, beansCount );

  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    if( m_BeansCount[i] != beansCount[i] )
    {
      this->SetTextboxPaneNumber( m_pBeansCountText[i], msg_prsys_common_15, beansCount[i], 4 );

      m_BeansCount[i] = beansCount[i];
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの更新
 */
//------------------------------------------------------------------------------
void LowerUIDraw::UpdateButton( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    if( !btnMan->IsButtonVisible( BTN_ID_CANCEL ) )
    {
      //if( m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_END_DEVELOP ) )
      if( !m_ProgressFlag_Menu && m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) )
      {
        btnMan->SetButtonInputEnable( BTN_ID_CANCEL, true );
        btnMan->SetButtonVisible( BTN_ID_CANCEL );
        m_ProgressFlag_Menu = true;
      }
    }
    if( !btnMan->IsButtonVisible( BTN_ID_MENU ) )
    {
      if( !m_ProgressFlag_TouchBox && m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX ) )
      {
        btnMan->SetButtonInputEnable( BTN_ID_MENU, true );
        btnMan->SetButtonVisible( BTN_ID_MENU );
        m_ProgressFlag_TouchBox = true;
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   表示タイプを設定
 */
//------------------------------------------------------------------------------
void LowerUIDraw::SetDisplayType( u8 type )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  // 無効化したボタンを復帰
  ReturnDisableBtn();   //@fix NMCat[1618]

  m_DispType = type;

  switch( type )
  {
  case DISP_TYPE_MAIN:
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, true );
      btnMan->SetButtonInputEnable( BTN_ID_MENU, true );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_L, true );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_R, true );
      btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, false );

      m_pDrawerBase->StartAnim( LA_RESORT_RESORT_MIN_LOW_000_PATERN_00 );

      m_pDrawerBase->SetVisiblePane( m_pBrackBG, false );
      m_pDrawerBase->SetVisiblePane( m_pPaneVisible, true );
    } break;

  case DISP_TYPE_BOX_POKEMON_EVENT:
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, true );
      btnMan->SetButtonInputEnable( BTN_ID_MENU, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_L, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_R, false );
      btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, true );

      btnMan->SetButtonVisible( BTN_ID_EVENT_STATUS );

      m_pDrawerBase->StartAnim( LA_RESORT_RESORT_MIN_LOW_000_PATERN_04 );

      m_pDrawerBase->SetVisiblePane( m_pBrackBG, false );
      m_pDrawerBase->SetVisiblePane( m_pPaneVisible, true );

      this->InitResult();
    } break;

  case DISP_TYPE_WILD_POKEMON_EVENT:
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, true );
      btnMan->SetButtonInputEnable( BTN_ID_MENU, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_L, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_R, false );
      btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, false );

      btnMan->SetButtonInvisible( BTN_ID_EVENT_STATUS );

      m_pDrawerBase->StartAnim( LA_RESORT_RESORT_MIN_LOW_000_PATERN_04 );

      m_pDrawerBase->SetVisiblePane( m_pBrackBG, false );
      m_pDrawerBase->SetVisiblePane( m_pPaneVisible, true );

      this->InitResult();
    } break;

  case DISP_TYPE_MASK:
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, false );
      btnMan->SetButtonInputEnable( BTN_ID_MENU, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_L, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_R, false );
      btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, false );

      btnMan->SetButtonInvisible( BTN_ID_EVENT_STATUS );

      m_pDrawerBase->SetVisiblePane( m_pBrackBG, true );
      m_pDrawerBase->SetVisiblePane( m_pPaneVisible, true );
    } break;

  case DISP_TYPE_INPUT_WAIT_NO_MASK:
    {
      btnMan->SetButtonInputEnable( BTN_ID_CANCEL, false );
      btnMan->SetButtonInputEnable( BTN_ID_MENU, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_L, false );
      btnMan->SetButtonInputEnable( BTN_ID_ARROW_R, false );
      btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, false );

      btnMan->SetButtonInvisible( BTN_ID_EVENT_STATUS );

      m_pDrawerBase->SetVisiblePane( m_pBrackBG, false );
      m_pDrawerBase->SetVisiblePane( m_pPaneVisible, false );
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾートの状態を確認
 */
//------------------------------------------------------------------------------
void LowerUIDraw::CheckResort( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( !btnMan->IsButtonActive( BTN_ID_ARROW_L ) && !btnMan->IsButtonActive( BTN_ID_ARROW_R ) )
  {
    u8 resortCount = util::GetDevelopedResortCount( m_pResortSave );

    if( resortCount == 1 )
    {
      btnMan->SetButtonPassive( BTN_ID_ARROW_L );
      btnMan->SetButtonPassive( BTN_ID_ARROW_R );
    }
    else {
      btnMan->SetButtonActive( BTN_ID_ARROW_L );
      btnMan->SetButtonActive( BTN_ID_ARROW_R );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   チュートリアル矢印の表示
 *
 * @param   progID    進行度ID
 * @param   x         Ｘ：スクリーン座標
 * @param   y         Ｙ：スクリーン座標
 */
//------------------------------------------------------------------------------
void LowerUIDraw::VisibleTutorialArrow( u8 progID, int x, int y )
{
  if( m_pFingerCursor == NULL ) return;

  switch( progID )
  {
  case Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU:
    {
      gfl2::lyt::LytPane* pane = m_pDrawerBase->GetPane( PANENAME_RESORT_MIN_LOW_000_PANE_HOMEBUTTON );
      {
        gfl2::math::VEC3 pos = pane->GetTranslate();
        {
          pos.y += 10.0f;
        }
        m_pFingerCursor->SetPos( pos );
      }
    } break;

  default:
    {
      gfl2::math::VEC3 pos( x, y, 0 );
      {
        pos.x = pos.x - 160.0f;
        pos.y = 120.0f - pos.y;
      }

      m_pFingerCursor->SetPos( pos );
    }
  }

  // 表示
  m_pFingerCursor->SetVisible( true );
  // アニメの再生
  m_pFingerCursor->StartAnime( app::tool::FingerCursor::FORM_TYPE_DOWN );
}

//------------------------------------------------------------------------------
/**
 * @brief   チュートリアル矢印を非表示
 */
//------------------------------------------------------------------------------
void LowerUIDraw::InvisibleTurorialArrow( void )
{
  if( m_pFingerCursor == NULL ) return;

  // 非表示
  m_pFingerCursor->SetVisible( false );
}


//------------------------------------------------------------------------------
/**
 * @brief   一時的にボタンを無効化
 *
 * @param   except    除外ボタンID
 */
//------------------------------------------------------------------------------
void LowerUIDraw::TempDisableBtn( u32 except )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    u8 index = 0;

    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      // 除外ボタンはチェックしない
      if( except == i ) continue;

      // 有効になっているボタンを記憶
      if( btnMan->IsButtonInputEnable( i ) )
      {
        m_TempDisableBtnID[index] = i;

        ++index;
      }

      // ボタンを無効
      btnMan->SetButtonInputEnable( i, false );
    }
  }

  // ボタンの無効化フラグ
  m_TempDisableBtn = true;
}
  
//------------------------------------------------------------------------------
/**
 * @brief   無効化ボタンを復帰
 */
//------------------------------------------------------------------------------
void LowerUIDraw::ReturnDisableBtn( void )
{
  // 無効化された時だけ復帰させる
  if( !m_TempDisableBtn ) return;

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      u32 btnID = m_TempDisableBtnID[i];

      if( btnID != BTN_ID_NUM )
      {
        // 一時的に無効にしたボタンを有効に戻す
        btnMan->SetButtonInputEnable( btnID, true );
      }

      m_TempDisableBtnID[i] = BTN_ID_NUM;
    }
  }
}





//==============================================================================
/**
 *      app::ui::UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerUIDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  switch( paneID )
  {
  case BTN_ID_CANCEL:
    {
      if( m_DispType == DISP_TYPE_MAIN )
      {
        m_uiListener->OnEndResort();
      }
      else {
        m_Result = RESULT_CANCEL;
      }
    } break;

  case BTN_ID_ARROW_L:
    {
      // 無効化したボタンを復帰
      ReturnDisableBtn();   //@fix NMCat[1655]

      if(m_uiListener->FeedResort( 0 )){
        // 入力を無効化
        this->SetInputEnabled( false );   // @fix NMCat[1655]
      }
    } break;

  case BTN_ID_ARROW_R:
    {
      // 無効化したボタンを復帰
      ReturnDisableBtn();   //@fix NMCat[1655]

      if(m_uiListener->FeedResort( 1 )){
        // 入力を無効化
        this->SetInputEnabled( false );   // @fix NMCat[1655]
      }
    } break;

  case BTN_ID_MENU:
    {
      m_uiListener->OpenHomeMenu();
    } break;

  case BTN_ID_EVENT_STATUS:
    {
      m_Result = RESULT_STATUS;
    } break;
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペイン選択アニメーションの開始通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
void LowerUIDraw::OnLayoutPaneStartingAnimeEvent( const u32 paneID )
{
  switch( paneID )
  {
  case BTN_ID_MENU:
  case BTN_ID_CANCEL:
    {
      // ボタンを一時的に無効化
      TempDisableBtn( paneID );   //@fix NMCat[1618]

      if( m_pSeqListener )
      {
        // 3D側の入力を無効
        m_pSeqListener->OpenHomeMneu( true );
      }
    } break;

  case BTN_ID_ARROW_L:
  case BTN_ID_ARROW_R:
    {
      // ボタンを一時的に無効化
      TempDisableBtn( paneID );   //@fix NMCat[1655]

      if( m_pSeqListener )
      {
        // 3D側の入力を無効
        m_pSeqListener->OpenHomeMneu( true );
      }
    } break;
  }
}






//==============================================================================
/**
 *      app::tool::ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerUIDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  btnMan->SetButtonBindKey( BTN_ID_MENU,    gfl2::ui::BUTTON_X );
  btnMan->SetButtonBindKey( BTN_ID_ARROW_L, gfl2::ui::BUTTON_L );
  btnMan->SetButtonBindKey( BTN_ID_ARROW_R, gfl2::ui::BUTTON_R );

  btnMan->SetButtonPassive( BTN_ID_ARROW_L );
  btnMan->SetButtonPassive( BTN_ID_ARROW_R );

  btnMan->SetButtonInputEnable( BTN_ID_EVENT_STATUS, false );

  // チュートリアルのため非表示に
  {
    btnMan->SetButtonInputEnable( BTN_ID_CANCEL, false );
    btnMan->SetButtonInputEnable( BTN_ID_MENU, false );

    if( !m_ProgressFlag_Menu )     btnMan->SetButtonInvisible( BTN_ID_CANCEL );
    if( !m_ProgressFlag_TouchBox ) btnMan->SetButtonInvisible( BTN_ID_MENU );
  }

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
  
  CheckResort();
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

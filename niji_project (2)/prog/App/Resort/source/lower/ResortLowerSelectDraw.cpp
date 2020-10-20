//======================================================================
/**
 * @file    ResortLowerSelectDraw.cpp
 * @date    2015/09/09 18:29:58
 * @author  fukushima_yuya
 * @brief   ポケリゾート：選択画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerSelectDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortConst.h>

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
  enum BtnID
  {
    BTN_ID_CANCEL,
    BTN_ID_00,
    BTN_ID_01,
    BTN_ID_02,
    BTN_ID_NUM,
  };
  
  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_SLC_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_00, NULL, NULL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_TOUCH,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_CANSEL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_ACTIVE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_00_PASSIVE,
    },
    {
      BTN_ID_01, NULL, NULL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_TOUCH,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_CANSEL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_ACTIVE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_01_PASSIVE,
    },
    {
      BTN_ID_02, NULL, NULL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_TOUCH,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_CANSEL,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_ACTIVE,
      LA_RESORT_RESORT_SLC_LOW_000__BUTTON_M_02_PASSIVE,
    },
  };

  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_SLC_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_00,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_01,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_02,
  };
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerSelectDraw::LowerSelectDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_SelectType(SEL_TYPE_NUM)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerSelectDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_SLC_LOW_000_BFLYT,
      LA_RESORT_RESORT_SLC_LOW_000___NUM,
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

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerSelectDraw::InitializeClass( InitParam& param )
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
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerSelectDraw::Open( void )
{
  GFL_ASSERT( m_SelectType < SEL_TYPE_NUM );

  static const u32 c_SelectTypeAnim[] = {
    LA_RESORT_RESORT_SLC_LOW_000_PATERN_01,
    LA_RESORT_RESORT_SLC_LOW_000_PATERN_02,
    LA_RESORT_RESORT_SLC_LOW_000_PATERN_03,
  };
  m_pDrawerBase->StartAnim( c_SelectTypeAnim[m_SelectType] );

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonInputEnable( BTN_ID_CANCEL, (m_SelectType != SEL_TYPE_YESNO) );    // @fix NMCat[1970]
    btnMan->SetButtonInputEnable( BTN_ID_00, (m_SelectType != SEL_TYPE_STATUS) );
    btnMan->SetButtonInputEnable( BTN_ID_01, (m_SelectType != SEL_TYPE_STATUS) );
    btnMan->SetButtonInputEnable( BTN_ID_02, (m_SelectType == SEL_TYPE_HOUSE) );
  }

  // ボタンのテキスト設定
  SetBtnText( m_SelectType );

  // リザルトの初期化
  this->InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンのテキスト設定
 *
 * @param   type    選択画面のタイプ
 */
//------------------------------------------------------------------------------
void LowerSelectDraw::SetBtnText( SelectType type )
{
  static const u32 c_HouseBtnParts[] = {
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_00,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_01,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_02,
  };
  static const u32 c_ConfBtnParts[] = {
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_00,
    PANENAME_RESORT_SLC_LOW_000_PANE_BUTTON_M_01,
  };

  static const u32 c_HouseBtnMsgID[] = {
    msg_prsys_button_07,
    msg_prsys_button_15,
    msg_prsys_button_08,
  };
  static const u32 c_ConfBtnMsgID[] = {
    msg_prsys_button_09,
    msg_prsys_button_10,
  };

  const u32* partsArray = NULL;
  const u32* msgIDArray = NULL;
  u32 btnNum = 0;

  if( type == SEL_TYPE_HOUSE )
  {
    partsArray = c_HouseBtnParts;
    msgIDArray = c_HouseBtnMsgID;
    btnNum     = GFL_NELEMS(c_HouseBtnParts);
  }
  else if( type == SEL_TYPE_YESNO )
  {
    partsArray = c_ConfBtnParts;
    msgIDArray = c_ConfBtnMsgID;
    btnNum     = GFL_NELEMS(c_ConfBtnParts);
  }

  for( u32 i=0; i<btnNum; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( partsArray[i] );
    {
      gfl2::lyt::LytTextBox* text = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, parts );
      {
        this->SetTextboxPaneMessage( text, msgIDArray[i] );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void LowerSelectDraw::Close( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonInputEnable( i, false );
    }
  }

  this->SetInputEnabled( false );
  this->SetVisible( false );
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
app::ui::UIInputListener::ListenerResult LowerSelectDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  static const u8 c_HouseResult[] = {
    RESULT_CANCEL,
    RESULT_DEVELOP,
    RESULT_TRADE,
    RESULT_MAME_SEND,
  };
  static const u8 c_ConfResult[] = {
    RESULT_CANCEL,
    RESULT_YES,
    RESULT_NO,
    RESULT_CANCEL,
  };
  static const u8 c_StatusResult[] = {
    RESULT_CANCEL,
    RESULT_CANCEL,
    RESULT_CANCEL,
    RESULT_CANCEL,
  };

  const u8* resArray = NULL;

  switch( m_SelectType )
  {
  case SEL_TYPE_HOUSE:  resArray = c_HouseResult;  break;
  case SEL_TYPE_YESNO:  resArray = c_ConfResult;   break;
  case SEL_TYPE_STATUS: resArray = c_StatusResult; break;
  default: GFL_ASSERT(0);
  }

  // 入力結果を反映
  m_Result = resArray[paneID];

  return ENABLE_ACCESS;
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
void LowerSelectDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i == BTN_ID_CANCEL )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }

    btnMan->SetButtonInputEnable( i, false );
  }
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

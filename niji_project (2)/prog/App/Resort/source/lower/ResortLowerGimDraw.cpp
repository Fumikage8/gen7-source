//======================================================================
/**
 * @file    ResortLowerGimDraw.cpp
 * @date    2015/10/02 12:04:03
 * @author  fukushima_yuya
 * @brief   ポケリゾート：うんどう状態画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerGimDraw.h"

#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortInitLogic.h>
#include <App/Resort/source/ResortUtility.h>
#include <App/Resort/source/parts/ResortPartsNumCounter.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum LowGimSeq {
    SEQ_IDLE,
    SEQ_OPEN,
    SEQ_MAIN,
    SEQ_NUM,
  };

  enum {
    BTN_ID_CANCEL,
    BTN_ID_DECIDE,
    BTN_ID_CNT_UP_10,
    BTN_ID_CNT_DW_10,
    BTN_ID_CNT_UP_1,
    BTN_ID_CNT_DW_1,
    BTN_ID_TAKEBACK,
    BTN_ID_CONTINUE,
    BTN_ID_NUM,
  };

  enum {
    GIM_STATE_NONE,     //!< 起動していない
    GIM_STATE_MIDST,    //!< うんどう中
    GIM_STATE_END,      //!< うんどう終了
    GIM_STATE_NUM,
  };

  //! ポケモンペインデータインデックス
  static const u32 c_PokePaneDataIndex[GIM_GROUP_POKE_MAX][POKE_PANE_ID_NUM] = {
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_00,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_00,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_00,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_00,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_00,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_01,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_01,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_01,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_01,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_01,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_02,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_02,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_02,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_02,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_02,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_03,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_03,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_03,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_03,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_03,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_04,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_04,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_04,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_04,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_04,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
    {
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKESET_05,
      LYTRES_PANENAME_RESORT_PANE_END,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEICON_05,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELV_05,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKELVNUMB_05,
      PANENAME_RESORT_UNDOU_LOW_000_PANE_POKEGENDER_05,
      LYTRES_PANENAME_RESORT_PANE_END,
      LYTRES_PANENAME_RESORT_PANE_END,
    },
  };

  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_DECIDE, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_PASSIVE,
    },
    {
      BTN_ID_CNT_UP_10, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_00_PASSIVE,
    },
    {
      BTN_ID_CNT_DW_10, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_01_PASSIVE,
    },
    {
      BTN_ID_CNT_UP_1, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_02_PASSIVE,
    },
    {
      BTN_ID_CNT_DW_1, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__DIAL_00__COUNTERCURSOR_03_PASSIVE,
    },
    {
      BTN_ID_TAKEBACK, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_00_PASSIVE,
    },
    {
      BTN_ID_CONTINUE, NULL, NULL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_TOUCH,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_CANSEL,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_ACTIVE,
      LA_RESORT_RESORT_UNDOU_LOW_000__SELECTBUTTON_01_PASSIVE,
    },
  };
  
  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_UNDOU_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_DIAL_00,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_DIAL_00,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_DIAL_00,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_DIAL_00,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON_00,
    PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON_01,
  };
  static const u32 c_ButtonPartsIndex[BTN_ID_NUM] = {
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_01,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_02,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_03,
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
  };
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
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
LowerGimDraw::LowerGimDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_pNumCounter( NULL )
  , m_Seq(SEQ_IDLE)
  , m_GimState(GIM_STATE_NUM)
  , m_PrevGimState(GIM_STATE_NUM)
  , m_pTakebackBtn( NULL )
  , m_pContinueBtn( NULL )
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
void LowerGimDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_UNDOU_LOW_000_BFLYT,
      LA_RESORT_RESORT_UNDOU_LOW_000___NUM,
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
void LowerGimDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonPartsIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // 描画共通処理
    m_pNumCounter = GFL_NEW(heap) ResortCounter( m_pDrawerBase, PANENAME_RESORT_UNDOU_LOW_000_PANE_DIAL_00 );
    {
      GFL_ASSERT( m_pNumCounter );
    }

    // ポケモンアイコン
    app::tool::PokeIconToolEx::PokeIconToolParam pokeIconParam;
    {
      pokeIconParam.pUIView = this;
      pokeIconParam.pHeap   = m_pIconHeap;
      pokeIconParam.iconMax = GIM_GROUP_POKE_MAX;
      pokeIconParam.sync    = true;
      pokeIconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreatePokeIcon( pokeIconParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerGimDraw::InitializePane( void )
{
  // ポケペインデータのセット
  for( u32 i=0; i<GIM_GROUP_POKE_MAX; ++i )
  {
    PokePaneData* paneData = &m_PokePane[i];
    util::SetPokePaneData( m_pDrawerBase, paneData, c_PokePaneDataIndex[i] );
  }

  m_pHeaderMain     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_HEADDER_MAIN );
  m_pCourseDesc     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_09 );
  m_pRemainingCount = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_06 );
  m_pTimeTitle      = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_07 );
  m_pTimeText       = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_08 );

  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON );
  {
    m_pSelBtnText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, btnParts );
  }

  // 決定ボタン(ひきとる／つづける)
  m_pTakebackBtn = m_pDrawerBase->GetPane( PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON );
  m_pContinueBtn = m_pDrawerBase->GetPane( PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECT_PARENT );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerGimDraw::InitializeText( void )
{
  gfl2::lyt::LytTextBox* textBox = NULL;
  {
    // セット回数
    textBox = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_10 );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_low_04_01 );
    }

    // セット
    textBox = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_11 );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_low_04_02 );
    }

    // 終わったセット
    textBox = m_pDrawerBase->GetTextBox( PANENAME_RESORT_UNDOU_LOW_000_PANE_TEXTBOX_05 );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_low_04_04 );
    }

    // ひきとるボタン
    gfl2::lyt::LytParts* takeBtnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON_00 );
    textBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, takeBtnParts );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_button_03 );
    }

    // つづけるボタン
    gfl2::lyt::LytParts* continueBtnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_UNDOU_LOW_000_PANE_SELECTBUTTON_01 );
    textBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, continueBtnParts );
    {
      this->SetTextboxPaneMessage( textBox, msg_prsys_button_17 );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerGimDraw::Terminate( void )
{
  GFL_SAFE_DELETE( m_pNumCounter );

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerGimDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_Seq == SEQ_MAIN )
  {
    app::util::G2DUtil* g2d = UIView::GetG2DUtil();
    {
      g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerGimDraw::Open( void )
{
  // うんどう状態をセット
  UpdateGimState();
  // 開いた時の状態を保存
  m_PrevGimState = m_GimState;

  // 画面のセットアップ
  Setup();

  // 時間表示
  UpdateEndTime();

  // ボタンの有効設定の変更
  SetButtonEnable();

  // リザルトの初期化
  this->InitResult();

  // 表示
  this->SetVisible( true );

  m_Seq = SEQ_OPEN;
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void LowerGimDraw::Close( void )
{
  this->SetInputEnabled( false );
  this->SetVisible( false );

  m_GimState     = GIM_STATE_NUM;
  m_PrevGimState = GIM_STATE_NUM;

  m_Seq = SEQ_IDLE;
}




//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void LowerGimDraw::Setup( void )
{
  // グループ名
  {
    static const u32 c_GroupName[] = {
      msg_prsys_up_04_03,   //!< グループ１
      msg_prsys_up_04_04,   //!< グループ２
      msg_prsys_up_04_05,   //!< グループ３
    };

    this->SetTextboxPaneMessage( m_pHeaderMain, c_GroupName[m_GroupIndex] );
  }

  // 決定ボタン
  {
    m_pDrawerBase->SetVisiblePane( m_pTakebackBtn, ( m_GimState != GIM_STATE_END ) );
    m_pDrawerBase->SetVisiblePane( m_pContinueBtn, ( m_GimState == GIM_STATE_END ) );
  }

  // 決定ボタンテキスト
  {
    static const u32 c_SelectButtonText[] = {
      msg_prsys_button_01,  //!< 決定
      msg_prsys_button_16,  //!< 中断
      msg_prsys_button_03,  //!< 引き取る
    };

    this->SetTextboxPaneMessage( m_pSelBtnText, c_SelectButtonText[m_GimState] );
  }

  // 画面の表示設定
  {
    static const u32 c_DispModeAnim[] = {
      LA_RESORT_RESORT_UNDOU_LOW_000_UNDOU_01,
      LA_RESORT_RESORT_UNDOU_LOW_000_UNDOU_00,
      LA_RESORT_RESORT_UNDOU_LOW_000_UNDOU_00,
    };

    m_pDrawerBase->StartAnim( c_DispModeAnim[m_GimState] );
  }

  // コース説明
  {
    static const u32 c_CourseDesc[] = {
      msg_prsys_low_04_06,
      msg_prsys_low_04_12,
      msg_prsys_low_04_07,
      msg_prsys_low_04_08,
      msg_prsys_low_04_09,
      msg_prsys_low_04_10,
      msg_prsys_low_04_11,
    };

    u8 course = m_pResortSave->GetGimKind( m_GroupIndex );

    this->SetTextboxPaneMessage( m_pCourseDesc, c_CourseDesc[course] );
  }

  // 時間項目
  {
    static const u32 c_TimeTitle[] = {
      msg_prsys_low_04_03,
      msg_prsys_low_04_05,
      msg_prsys_low_04_05,
    };

    this->SetTextboxPaneMessage( m_pTimeTitle, c_TimeTitle[m_GimState] );
  }

  // セット回数
  UpdateSetCount();

  // 終了時間
  if( m_GimState == GIM_STATE_NONE )
  {
    // カウンターの初期化
    m_pNumCounter->ResetTotalNumber();
    // 終了時間の設定
    SetEndTime();
  }

  // ポケモンの表示
  {
    static const u32 c_GenderAnim[] = {
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_00_ICON_MALE,
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_01_ICON_MALE,
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_02_ICON_MALE,
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_03_ICON_MALE,
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_04_ICON_MALE,
      LA_RESORT_RESORT_UNDOU_LOW_000__POKEGENDER_05_ICON_MALE,
    };

    gfl2::heap::HeapBase* sysHeap = m_pDrawerBase->GetSystemHeap();
    app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();

    for( u32 i=0; i<GIM_GROUP_POKE_MAX; ++i )
    {
      u32 index = m_GroupIndex * GIM_GROUP_POKE_MAX + i;
      pml::pokepara::CoreParam* cp = GFL_NEW_LOW(sysHeap) pml::pokepara::CoreParam( sysHeap );
      PokeTool::SimpleParam sp;
      u32 level = 0;

      // ポケモンの取得
      m_pResortSave->GetGimPokemon( index, cp );

      // 描画用パラメータのセット
      bool res = util::SetSimpleParam( &sp, &level, cp );
      
      const PokePaneData* paneData = &m_PokePane[i];

      if( res && !sp.isEgg )
      {
        // ポケモンアイコンのセット
        pokeIcon->SetPokeIcon( i, cp, paneData->iconPic );

        // レベルアイコン
        this->SetTextboxPaneMessage( paneData->textLv, msg_prsys_common_02 );

        // レベル値
        this->SetTextboxPaneNumber( paneData->textLvNum, msg_prsys_common_04, level, 3, 0, print::NUM_DISP_SPACE );

        // 性別
        if( sp.sex == pml::SEX_UNKNOWN )
        {
          m_pDrawerBase->SetVisiblePane( paneData->gender, false );
        }
        else {
          m_pDrawerBase->StartAnim( c_GenderAnim[i] + sp.sex );
          m_pDrawerBase->SetVisiblePane( paneData->gender, true );
        }

        m_pDrawerBase->SetVisiblePane( paneData->null, true );
      }
      else {
        m_pDrawerBase->SetVisiblePane( paneData->null, false );
      }

      GFL_DELETE cp;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerGimDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_OPEN:
    {
      // 入力を有効に
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      // うんどう状態を更新
      UpdateGimState();
      // 決定ボタンの更新
      UpdateDecideBtn();
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   状態の更新
 */
//------------------------------------------------------------------------------
void LowerGimDraw::UpdateGimState( void )
{
  // うんどう状態を取得
  u8 state = m_pResortSave->GetGimState( m_GroupIndex );

  if( state == GIM_STATE_NUM ) return;

  // うんどう状態を更新
  if( state != m_GimState )
  {
    m_GimState = state;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セット回数の更新
 */
//------------------------------------------------------------------------------
void LowerGimDraw::UpdateSetCount( void )
{
  if( m_GimState != GIM_STATE_NONE )
  {
    u8 setCount    = m_pResortSave->GetGimTrainingCount( m_GroupIndex );
    u8 setCountMax = m_pResortSave->GetGimTrainingCountMax( m_GroupIndex );

    util::SetTextBoxFraction( this, m_pDrawerBase->GetWordSet(), m_pRemainingCount, (setCountMax - setCount), setCountMax );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの更新
 */
//------------------------------------------------------------------------------
void LowerGimDraw::UpdateDecideBtn( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  if( m_GimState == GIM_STATE_NONE )
  {
    // 決定ボタンのアクティブ状態を取得
    bool btnActive = btnMan->IsButtonActive( BTN_ID_DECIDE );

    // セット回数の取得
    u32 setCount = m_pNumCounter->GetTotalNumber();

    if( setCount > 0 )
    {
      if( !btnActive )
      {
        // パッシブ状態ならアクティブに変更
        btnMan->SetButtonActive( BTN_ID_DECIDE );
      }
    }
    else if( btnActive )
    {
      // アクティブ状態ならパッシブに変更
      btnMan->SetButtonPassive( BTN_ID_DECIDE );
    }
  }
  else if( m_GimState == GIM_STATE_END && m_PrevGimState == GIM_STATE_MIDST )
  {
    // 決定ボタンのアクティブ状態を取得
    bool btnActive = btnMan->IsButtonActive( BTN_ID_DECIDE );

    if( btnActive )
    {
      btnMan->SetButtonPassive( BTN_ID_DECIDE );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   決定処理
 */
//------------------------------------------------------------------------------
void LowerGimDraw::DecideFunc( void )
{
  m_Result = RESULT_DECIDE;

  if( m_GimState == GIM_STATE_NONE )
  {
    u8 groupIndex = m_GroupIndex;
    u32 setCount  = m_pNumCounter->GetTotalNumber();

    // うんどうセット回数
    m_pResortSave->SetGimTrainingCount( groupIndex, setCount );
    m_pResortSave->SetGimTrainingCountMax( groupIndex, setCount );
    // うんどう終了時間
    //m_pResortSave->SetGimEndTime( groupIndex, m_EndTime.hour, m_EndTime.min, m_EndTime.sec );
    // うんどう開始
    m_pInitLogic->StartGimTimer( groupIndex );

    // レコード：回数をカウント
    Savedata::IncRecord( Savedata::Record::RECID_RESORT_WAIWAI );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   終了時間のセット
 */
//------------------------------------------------------------------------------
void LowerGimDraw::SetEndTime( void )
{
  // セットカウント
  u32 setCount = m_pNumCounter->GetTotalNumber();
  // １セット分の時間
  u32 value = GetTrainingTime() * setCount;

  // 終了時間のセット
  m_EndTime.SetTotalSec( value );

  // 時間の表示
  util::SetTextBoxTime( this, m_pDrawerBase->GetWordSet(), m_pTimeText, m_EndTime );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了時間のセット
 */
//------------------------------------------------------------------------------
void LowerGimDraw::UpdateEndTime( void )
{
  if( m_GimState == GIM_STATE_MIDST )
  {
    TimeParam time;

    // 残り時間の取得
    m_pInitLogic->GetGimTotalEndTime( m_GroupIndex, &time.hour, &time.min, &time.sec );

    //// １分以上ならタイムを表示
    //if( time.GetTotalSec() >= 60 )
    if( time.GetTotalSec() > 60 )
    {
      util::SetTextBoxTime( this, m_pDrawerBase->GetWordSet(), m_pTimeText, time );
    }
    // １分未満なら「あとちょっと！」を表示
    else {
      this->SetTextboxPaneMessage( m_pTimeText, msg_prsys_temp05 );
    }

    // 色変更
    {
      TimeParam bonusTime;
      {
        m_pResortSave->GetGimBonusTime( &bonusTime.hour, &bonusTime.min, &bonusTime.sec );
      }

      nw::ut::Color8 color = 0x961C00FF;
      {
        if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
        {
          color = 0x1987E5FF; // シアン：RGBA(25:135:22:255)
        }

        m_pTimeText->SetTextColor( color, color );
      }
    }
  }
  else if( m_GimState == GIM_STATE_END )
  {
    // --じかん--ふんを表示
    this->SetTextboxPaneMessage( m_pTimeText, msg_prsys_common_07 );

    nw::ut::Color8 color = 0x961C00FF;
    {
      m_pTimeText->SetTextColor( color, color );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   トレーニング時間の取得
 */
//------------------------------------------------------------------------------
u32 LowerGimDraw::GetTrainingTime( void )
{
  //if( m_pResortSave->GetGimKind( m_GroupIndex ) == Savedata::ResortSave::RESORT_GIM_KIND_LV )
  //{
  //  return (30 * 60); //!< 30分
  //}
  //else {
  //  return (30 * 60); //!< 30分
  //}

  // @fix cov_ctr[11067]
  return (30 * 60); //!< 30分
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
app::ui::UIInputListener::ListenerResult LowerGimDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  switch( paneID )
  {
    case BTN_ID_CANCEL:
    {
      m_Result = RESULT_CANCEL;
    } break;

    case BTN_ID_TAKEBACK:
    {
      m_Result = RESULT_GIM_TAKEBACK;
    } break;

    case BTN_ID_CONTINUE:
    {
      m_Result = RESULT_GIM_CONTINUE;
    } break;

    case BTN_ID_DECIDE:
    {
      DecideFunc();
    } break;

    case BTN_ID_CNT_UP_10:
    case BTN_ID_CNT_DW_10:
    case BTN_ID_CNT_UP_1:
    case BTN_ID_CNT_DW_1:
    {
      static const ResortCounter::Operation c_Operation[] = {
        ResortCounter::OPE_COUNT_UP_10,
        ResortCounter::OPE_COUNT_DW_10,
        ResortCounter::OPE_COUNT_UP_1,
        ResortCounter::OPE_COUNT_DW_1,
      };

      if( m_pNumCounter )
      {
        u32 index = paneID - BTN_ID_CNT_UP_10;
        m_pNumCounter->CounterOperation( c_Operation[index] );

        // 終了時間のセット
        SetEndTime();
      }
    } break;
  }

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
void LowerGimDraw::SetupButton( app::tool::ButtonManager* btnMan )
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
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerGimDraw::SetButtonEnable( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonInputEnable( i, true );
    }

    // カウンターボタン
    if( m_GimState != GIM_STATE_NONE )
    {
      // @fix GFNMCat[2275]
      for( u32 i=BTN_ID_CNT_UP_10; i<=BTN_ID_CNT_DW_1; ++i )
      {
        btnMan->SetButtonInputEnable( i, false );
      }
    }

    // 決定ボタン(ひきとる／つづける)
    btnMan->SetButtonInputEnable( BTN_ID_DECIDE, ( m_GimState != GIM_STATE_END ) );
    btnMan->SetButtonInputEnable( BTN_ID_TAKEBACK, ( m_GimState == GIM_STATE_END ) );
    btnMan->SetButtonInputEnable( BTN_ID_CONTINUE, ( m_GimState == GIM_STATE_END ) );

    // 決定ボタン
    if( m_GimState == GIM_STATE_NONE && m_pNumCounter->GetTotalNumber() == 0 )
    {
      // セット回数の選択時
      btnMan->SetButtonPassiveAnimeFrameLast( BTN_ID_DECIDE );  // @fix GFNMCat[4188]
    }
    else if( m_GimState == GIM_STATE_END )
    {
      if( m_DecideBtnEnable )
      {
        btnMan->SetButtonActiveAnimeFrameLast( BTN_ID_TAKEBACK );
      }
      else {
        btnMan->SetButtonPassiveAnimeFrameLast( BTN_ID_TAKEBACK );
      }
      btnMan->SetButtonActiveAnimeFrameLast( BTN_ID_CONTINUE );
    }
    else if( m_DecideBtnEnable )
    {
      btnMan->SetButtonActiveAnimeFrameLast( BTN_ID_DECIDE );
    }
  }
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

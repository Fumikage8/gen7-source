//======================================================================
/**
 * @file    ResortLowerBeansSelectDraw.cpp
 * @date    2015/09/10 13:37:53
 * @author  fukushima_yuya
 * @brief   ポケリゾート：ポケマメ選択画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBeansSelectDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortUtility.h>
#include <App/Resort/source/ResortInitLogic.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>
#include <Sound/include/Sound.h>

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
  enum BtnID
  {
    // 共通
    BTN_ID_CANCEL,    //!< キャンセルボタン
    BTN_ID_DECIDE,    //!< 決定ボタン／うえる／ひきとる

    // ポケマメ選択
    BTN_ID_POKEMAME_N_CNT_UP_10,    //!< ノーマルポケマメカウント10UP
    BTN_ID_POKEMAME_N_CNT_DW_10,    //!< ノーマルポケマメカウント10DW
    BTN_ID_POKEMAME_N_CNT_UP_1,     //!< ノーマルポケマメカウント1UP
    BTN_ID_POKEMAME_N_CNT_DW_1,     //!< ノーマルポケマメカウント1DW
    BTN_ID_POKEMAME_R_CNT_UP_10,    //!< レアポケマメカウント10UP
    BTN_ID_POKEMAME_R_CNT_DW_10,    //!< レアポケマメカウント10DW
    BTN_ID_POKEMAME_R_CNT_UP_1,     //!< レアポケマメカウント1UP
    BTN_ID_POKEMAME_R_CNT_DW_1,     //!< レアポケマメカウント1DW
    BTN_ID_POKEMAME_SR_CNT_UP_10,   //!< Ｓレアポケマメカウント10UP
    BTN_ID_POKEMAME_SR_CNT_DW_10,   //!< Ｓレアポケマメカウント10DW
    BTN_ID_POKEMAME_SR_CNT_UP_1,    //!< Ｓレアポケマメカウント1UP
    BTN_ID_POKEMAME_SR_CNT_DW_1,    //!< Ｓレアポケマメカウント1DW

    BTN_ID_NUM,
  };

  static const s32 DIAL_COUNT_MAX = 99;   //!< カウンターで設定できる最大値
  
  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_DECIDE, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__SELECTBUTTON_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_N_CNT_UP_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_00_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_N_CNT_DW_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_01_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_N_CNT_UP_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_02_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_N_CNT_DW_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_01__COUNTERCURSOR_03_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_R_CNT_UP_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_00_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_R_CNT_DW_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_01_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_R_CNT_UP_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_02_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_R_CNT_DW_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_02__COUNTERCURSOR_03_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_SR_CNT_UP_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_00_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_SR_CNT_DW_10, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_01_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_SR_CNT_UP_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_02_PASSIVE,
    },
    {
      BTN_ID_POKEMAME_SR_CNT_DW_1, NULL, NULL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_TOUCH,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_CANSEL,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_TOUCH_RELEASE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_ACTIVE,
      LA_RESORT_RESORT_CMD_LOW_000__DIAL_03__COUNTERCURSOR_03_PASSIVE,
    },
  };
  
  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_CMD_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_CMD_LOW_000_PANE_SELECTBUTTON,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_02,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_02,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_02,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_02,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_03,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_03,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_03,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_03,
  };
  static const u32 c_ButtonPartsIndex[BTN_ID_NUM] = {
    LYTRES_PANENAME_RESORT_PANE_END,
    LYTRES_PANENAME_RESORT_PANE_END,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_01,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_02,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_03,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_01,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_02,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_03,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_01,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_02,
    PANENAME_RESORT_PLT_LOW_000_PANE_COUNTERCURSOR_03,
  };
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
    PANENAME_RESORT_CUR_LOW_001_PANE_BOUND_00,
  };
}



//==============================================================================
// @brief   ポケマメ選択画面(下)の描画クラス
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerBeansSelectDraw::LowerBeansSelectDraw( CreateParam& param )
  : LowerBaseDraw( param )
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
void LowerBeansSelectDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_CMD_LOW_000_BFLYT,
      LA_RESORT_RESORT_CMD_LOW_000___NUM,
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
void LowerBeansSelectDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetSystemHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonPartsIndex, c_ButtonBoundIndex, BTN_ID_NUM );

    // カウンター
    {
      for( u32 i=0; i<BEANS_KIND_NUM; ++i )
      {
        m_pCounter[i] = GFL_NEW(heap) DialCounter( m_pDrawerBase, i );
        {
          GFL_ASSERT( m_pCounter[i] );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::InitializePane( void )
{
  m_pTextUseBeansNum    = m_pDrawerBase->GetTextBox( PANENAME_RESORT_CMD_LOW_000_PANE_TEXTBOX_01 );
  m_pTextBonusTime      = m_pDrawerBase->GetTextBox( PANENAME_RESORT_CMD_LOW_000_PANE_TEXTBOX_15 );
  m_pTextTitle          = m_pDrawerBase->GetTextBox( PANENAME_RESORT_CMD_LOW_000_PANE_HEADDER_MAIN );
  m_pTextUseBeansTitle  = m_pDrawerBase->GetTextBox( PANENAME_RESORT_CMD_LOW_000_PANE_TEXTBOX_12 );
  m_pTextBonusTimeTitle = m_pDrawerBase->GetTextBox( PANENAME_RESORT_CMD_LOW_000_PANE_TEXTBOX_14 );
  m_pPaneVisibleImage   = m_pDrawerBase->GetPane( PANENAME_RESORT_CMD_LOW_000_PANE_IMAGES );

  m_pDrawerBase->SetVisiblePane( m_pPaneVisibleImage, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキストの設定
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::InitializeText( void )
{
  // 決定
  gfl2::lyt::LytParts* decideBtnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_CMD_LOW_000_PANE_SELECTBUTTON );
  {
    gfl2::lyt::LytTextBox* decideBtnText =
      m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, decideBtnParts );
    {
      this->SetTextboxPaneMessage( decideBtnText, msg_prsys_button_01 );
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerBeansSelectDraw::Terminate( void )
{
  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pCounter[i] );
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ選択を開く
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::Open( void )
{
  // セットアップ
  Setup();

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonPassiveAnimeFrameLast( BTN_ID_DECIDE );
  }

  // リザルトの初期化
  this->InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ選択を閉じる
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::Close( void )
{
  this->SetInputEnabled( false );
  this->SetVisible( false );
}


//------------------------------------------------------------------------------
/**
 * @brief   決定処理
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::DecideFunc( void )
{
  // ポケマメ交換
  if( m_BeansSelectType == BEANS_SEL_TYPE_TRADE )
  {
    DecideFunc_Trade();
  }
  // ボーナスタイム
  else {
    DecideFunc_BonusTime();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   決定処理：ポケマメ交換用
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::DecideFunc_Trade( void )
{
  // 交換したポケマメを反映
  util::AddPokeMame( m_pResortSave, Savedata::ResortSave::POKEBEANS_RANK_NORMAL, m_UsedBeansNum );  // @fix NMCat[1977]

  // 使用したポケマメを反映
  m_pResortSave->SubPokeBeansNum(
    0,
    m_pCounter[BEANS_KIND_GARA]->GetUseNum(),
    m_pCounter[BEANS_KIND_NIJI]->GetUseNum() );

  // レコード：回数をカウント
  Savedata::IncRecord( Savedata::Record::RECID_POKEMAME_TRADE );
}

//------------------------------------------------------------------------------
/**
 * @brief   決定処理：ボーナスタイム用
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::DecideFunc_BonusTime( void )
{
  Savedata::ResortSave::PokeResortKind resortKind =
    static_cast<Savedata::ResortSave::PokeResortKind>( m_BeansSelectType );
  // 使用した数をポケマメいれにセット
  m_pResortSave->SetBeansBoxNum( resortKind, m_UsedBeansNum );

  // 使用したポケマメを反映
  m_pResortSave->SubPokeBeansNum(
    m_pCounter[BEANS_KIND_NORMAL]->GetUseNum(),
    m_pCounter[BEANS_KIND_GARA]->GetUseNum(),
    m_pCounter[BEANS_KIND_NIJI]->GetUseNum() );

  // ボーナス時間の反映
  m_pInitLogic->SetBonusTimer(
    m_pCounter[BEANS_KIND_NORMAL]->GetUseNum(),
    m_pCounter[BEANS_KIND_GARA]->GetUseNum(),
    m_pCounter[BEANS_KIND_NIJI]->GetUseNum() );

  // レコード：回数をカウント
  Savedata::IncRecord( Savedata::Record::RECID_POKEMAME_IN );
}



//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::Setup( void )
{
  // 共通のセットアップ
  {
    m_UsedBeansNum = 0;

    // 所持ポケマメのセット
    SetHaveBeansNum();
    // 選択画面のセット
    SetBeansSelectAnim();
    // イメージのセット
    SetImageAnim();
  }

  // 個別のセットアップ
  void (LowerBeansSelectDraw::*SETUP_FUNC[BEANS_SEL_TYPE_NUM])( void ) = {
    &LowerBeansSelectDraw::Setup_Bonus,
    &LowerBeansSelectDraw::Setup_Bonus,
    &LowerBeansSelectDraw::Setup_Bonus,
    &LowerBeansSelectDraw::Setup_Bonus,
    &LowerBeansSelectDraw::Setup_Bonus,
    &LowerBeansSelectDraw::Setup_Trade,
  };

  (this->*SETUP_FUNC[m_BeansSelectType])();

  // テキストの更新
  UpdateText();
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::Setup_Bonus( void )
{
  { // テキストの設定
    this->SetTextboxPaneMessage( m_pTextTitle, msg_prsys_common_10 );
    this->SetTextboxPaneMessage( m_pTextUseBeansTitle, msg_prsys_common_11 );
    this->SetTextboxPaneMessage( m_pTextBonusTimeTitle, msg_prsys_common_12 );
  }

  static const Savedata::ResortSave::PokeResortKind c_ResortKind[] = {
    Savedata::ResortSave::RESORT_KIND_FRIENDSHIP,
    Savedata::ResortSave::RESORT_KIND_FARM,
    Savedata::ResortSave::RESORT_KIND_ADVENTURE,
    Savedata::ResortSave::RESORT_KIND_GIM,
    Savedata::ResortSave::RESORT_KIND_HOTSPA,
  };
  // リゾートレベルの取得
  u8 level = m_pResortSave->GetResortLevel( c_ResortKind[m_BeansSelectType] );
  {
    static const u32 c_UseBeansMax[] = {
      0, 20, 30, 50,
    };
    // 使用できるマメの最大数
    m_UsedBeansMax = c_UseBeansMax[level];
  }

  // カウンターのセット
  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    m_pCounter[i]->ResetParam();
    m_pCounter[i]->SetDialType( DialCounter::DIAL_TYPE_BONUS );
    m_pCounter[i]->SetHaveBeansNum( m_HaveBeansNum[i] );
    m_pCounter[i]->SetUseBeansNum( m_UsedBeansMax );
    m_pCounter[i]->SetText();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::Setup_Trade( void )
{
  { // テキストの設定
    this->SetTextboxPaneMessage( m_pTextTitle, msg_prsys_button_15 );
    this->SetTextboxPaneMessage( m_pTextUseBeansTitle, msg_prsys_exchange_01 );
    this->SetTextboxPaneMessage( m_pTextBonusTimeTitle, msg_prsys_exchange_02 );
  }

  u32 beansNum[3] = { 0, 0, 0 };
  util::GetRankBeansCount( m_pResortSave, beansNum );

  m_UsedBeansMax = 999 - beansNum[0];

  // カウンターのセット
  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    m_pCounter[i]->ResetParam();
    m_pCounter[i]->SetDialType( DialCounter::DIAL_TYPE_TRADE );
    m_pCounter[i]->SetHaveBeansNum( m_HaveBeansNum[i] );
    m_pCounter[i]->SetUseBeansNum( m_UsedBeansMax );
    m_pCounter[i]->SetText();
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   所持ポケマメのセット
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::SetHaveBeansNum( void )
{
  u32 count[BEANS_KIND_NUM] = { 0, 0, 0 };
  {
    util::GetRankBeansCount( m_pResortSave, count );

    for( u32 i=0; i<BEANS_KIND_NUM; ++i )
    {
      // @fix NMCat[1614]
      if( m_BeansSelectType == BEANS_SEL_TYPE_TRADE )
      {
        if( i == 0 )
        {
          // ポケマメ交換ではノーマルのポケマメは使用しないので０を入れておく
          m_HaveBeansNum[0] = 0;
          continue;
        }
      }

      m_HaveBeansNum[i] = static_cast<s32>( count[i] );
      FUKUSHIMA_PRINT( "HaveBeansNum[%d] = [%d]\n", i, m_HaveBeansNum[i] );
    }
  }

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    static const u32 c_CounterBtnID[BEANS_KIND_NUM][DIAL_KIND_NUM] = {
      { BTN_ID_POKEMAME_N_CNT_UP_10,  BTN_ID_POKEMAME_N_CNT_DW_10,  BTN_ID_POKEMAME_N_CNT_UP_1,  BTN_ID_POKEMAME_N_CNT_DW_1 },
      { BTN_ID_POKEMAME_R_CNT_UP_10,  BTN_ID_POKEMAME_R_CNT_DW_10,  BTN_ID_POKEMAME_R_CNT_UP_1,  BTN_ID_POKEMAME_R_CNT_DW_1 },
      { BTN_ID_POKEMAME_SR_CNT_UP_10, BTN_ID_POKEMAME_SR_CNT_DW_10, BTN_ID_POKEMAME_SR_CNT_UP_1, BTN_ID_POKEMAME_SR_CNT_DW_1 },
    };

    for( u32 i=0; i<BEANS_KIND_NUM; ++i )
    {
      if( m_HaveBeansNum[i] == 0 )
      {
        btnMan->SetButtonPassive( c_CounterBtnID[i][DIAL_KIND_UP_10] );
        btnMan->SetButtonPassive( c_CounterBtnID[i][DIAL_KIND_DW_10] );
        btnMan->SetButtonPassive( c_CounterBtnID[i][DIAL_KIND_UP_1] );
        btnMan->SetButtonPassive( c_CounterBtnID[i][DIAL_KIND_DW_1] );
      }
      else {
        btnMan->SetButtonActive( c_CounterBtnID[i][DIAL_KIND_UP_10] );
        btnMan->SetButtonActive( c_CounterBtnID[i][DIAL_KIND_DW_10] );
        btnMan->SetButtonActive( c_CounterBtnID[i][DIAL_KIND_UP_1] );
        btnMan->SetButtonActive( c_CounterBtnID[i][DIAL_KIND_DW_1] );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   選択画面のセット
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::SetBeansSelectAnim( void )
{
  static const u32 c_SelectTypeAnim[] = {
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_00,   //!< ふれあい
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_01,   //!< きのみ
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_00,   //!< たんけん
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_00,   //!< うんどう
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_00,   //!< おんせん
    LA_RESORT_RESORT_CMD_LOW_000_COMMON_MAMESET_02,   //!< こうかん
  };

  m_pDrawerBase->StartAnim( c_SelectTypeAnim[m_BeansSelectType] );
}

//------------------------------------------------------------------------------
/**
 * @brief   イメージのセット
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::SetImageAnim( void )
{
  // ポケマメ交換以外
  if( m_BeansSelectType != BEANS_SEL_TYPE_TRADE )
  {
    // アニメを再生
    m_pDrawerBase->StartAnim( LA_RESORT_RESORT_CMD_LOW_000_IMAGELV, false );

    // リゾートのレベルを取得
    u8 level = util::GetResortLevel( m_pResortSave, m_BeansSelectType );
    {
      // フレームの設定
      m_pDrawerBase->SetAnimFrame(
        LA_RESORT_RESORT_CMD_LOW_000_IMAGELV,
        static_cast<f32>(level - 1) );
    }

    // イメージの表示
    m_pDrawerBase->SetVisiblePane( m_pPaneVisibleImage, true );
  }
  else {
    // イメージを非表示
    m_pDrawerBase->SetVisiblePane( m_pPaneVisibleImage, false );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   カウンターの更新
 *
 * @param   beansKind   ポケマメの種類
 * @param   btnTYpe     ダイヤルボタンのタイプ
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::UpdateCounter( LowerBeansSelectDraw::BeansKind beansKind, DialCounter::DialBtnType btnType )
{
  DialCounter* counter = m_pCounter[beansKind];
  {
    GFL_ASSERT( counter );

    // ダイヤル更新
    counter->Dial( btnType, m_UsedBeansNum );
  }

  // テキストの更新
  UpdateText();
  // 決定ボタンの更新
  UpdateDecideBtn();
}

//------------------------------------------------------------------------------
/**
 * @brief   追加する値の取得
 *
 * @param   dialKind    ダイヤルの種類
 */
//------------------------------------------------------------------------------
s32 LowerBeansSelectDraw::GetAddValue( LowerBeansSelectDraw::DialKind dialKind )
{
  switch( dialKind )
  {
  case DIAL_KIND_UP_10:
    {
      return (10);
    }
  case DIAL_KIND_DW_10:
    {
      return (-10);
    }
  case DIAL_KIND_UP_1:
    {
      return (1);
    }
  case DIAL_KIND_DW_1:
    {
      return (-1);
    }
  }

  GFL_ASSERT(0);
  return 0;
}



//------------------------------------------------------------------------------
/**
 * @brief   テキストの更新
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::UpdateText( void )
{
  if( m_BeansSelectType != BEANS_SEL_TYPE_TRADE )
  {
    UpdateText_UseBeansNum();
    UpdateText_BonusTime();
  }
  else {
    UpdateText_UseBeansNum();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   テキストの更新（使用数）
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::UpdateText_UseBeansNum( void )
{
  m_UsedBeansNum = 0;

  if( m_BeansSelectType != BEANS_SEL_TYPE_TRADE )
  {
    for( u32 i=0; i<BEANS_KIND_NUM; ++i )
    {
      m_UsedBeansNum += m_pCounter[i]->GetUseNum();
    }

    print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
    {
      wordSet->RegisterNumber( 0, m_UsedBeansNum, 2, print::NUM_DISP_LEFT,  print::NUM_CODE_ZENKAKU );
      wordSet->RegisterNumber( 1, m_UsedBeansMax, 2, print::NUM_DISP_SPACE, print::NUM_CODE_ZENKAKU );
    }

    this->SetTextboxPaneMessage( m_pTextUseBeansNum, msg_prsys_common_13 );
  }
  else {
    static const s32 c_TradeRate[] = {
      0, 3, 7,
    };
    for( u32 i=0; i<BEANS_KIND_NUM; ++i )
    {
      m_UsedBeansNum += m_pCounter[i]->GetUseNum() * c_TradeRate[i];
    }

    this->SetTextboxPaneNumber(
      m_pTextUseBeansNum, msg_prsys_common_04, m_UsedBeansNum,
      3, 0, print::NUM_DISP_SPACE, print::NUM_CODE_ZENKAKU );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   テキストの更新（追加タイム）
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::UpdateText_BonusTime( void )
{
  TimeParam time;

  u32 min = m_pCounter[0]->GetUseNum() * 20;
  u32 hour = min / 60;
  min = min - (hour * 60);

  time.hour = hour + m_pCounter[1]->GetUseNum() + m_pCounter[2]->GetUseNum() * 2;
  time.min  = min;
  time.sec  = 0;

  if( time.hour >= 100 )
  {
    time.hour = 99;
    time.min  = 59;   // @fix GFNMCat[2539]
    time.sec  = 59;   // @fix GFNMCat[2539]
  }

  // 時間表示
  util::SetTextBoxTime( this, m_pDrawerBase->GetWordSet(), m_pTextBonusTime, time );
}



//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの更新
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::UpdateDecideBtn( void )
{
  s32 val = 0;
  {
    for( u32 i=0; i<BEANS_KIND_NUM; ++i )
    {
      val += m_pCounter[i]->GetUseNum();
    }
  }

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    if( val == 0 )
    {
      if( btnMan->IsButtonActive( BTN_ID_DECIDE ) )
      {
        btnMan->SetButtonPassive( BTN_ID_DECIDE );
      }
    }
    else {
      if( !btnMan->IsButtonActive( BTN_ID_DECIDE ) )
      {
        btnMan->SetButtonActive( BTN_ID_DECIDE );
      }
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケマメの種類を取得
 *
 * @param   btnID   ボタンID
 *
 * @return  ポケマメの種類(ノーマル・ガラ・ニジ)
 */
//------------------------------------------------------------------------------
LowerBeansSelectDraw::BeansKind LowerBeansSelectDraw::GetBeansKind( u32 btnID )
{
  switch( btnID )
  {
  case BTN_ID_POKEMAME_N_CNT_UP_10:
  case BTN_ID_POKEMAME_N_CNT_DW_10:
  case BTN_ID_POKEMAME_N_CNT_UP_1:
  case BTN_ID_POKEMAME_N_CNT_DW_1:
    {
      return BEANS_KIND_NORMAL;
    }
  case BTN_ID_POKEMAME_R_CNT_UP_10:
  case BTN_ID_POKEMAME_R_CNT_DW_10:
  case BTN_ID_POKEMAME_R_CNT_UP_1:
  case BTN_ID_POKEMAME_R_CNT_DW_1:
    {
      return BEANS_KIND_GARA;
    }
  case BTN_ID_POKEMAME_SR_CNT_UP_10:
  case BTN_ID_POKEMAME_SR_CNT_DW_10:
  case BTN_ID_POKEMAME_SR_CNT_UP_1:
  case BTN_ID_POKEMAME_SR_CNT_DW_1:
    {
      return BEANS_KIND_NIJI;
    }
  }

  GFL_ASSERT(0);
  return BEANS_KIND_NORMAL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ダイヤルの種類を取得
 *
 * @param   btnID   ボタンID
 *
 * @return  ダイヤルの種類
 */
//------------------------------------------------------------------------------
DialCounter::DialBtnType LowerBeansSelectDraw::GetDialBtnType( u32 btnID )
{
  switch( btnID )
  {
  case BTN_ID_POKEMAME_N_CNT_UP_10:
  case BTN_ID_POKEMAME_R_CNT_UP_10:
  case BTN_ID_POKEMAME_SR_CNT_UP_10:
    {
      return DialCounter::DIAL_BTN_TYPE_UP_10;
    }
  case BTN_ID_POKEMAME_N_CNT_DW_10:
  case BTN_ID_POKEMAME_R_CNT_DW_10:
  case BTN_ID_POKEMAME_SR_CNT_DW_10:
    {
      return DialCounter::DIAL_BTN_TYPE_DW_10;
    }
  case BTN_ID_POKEMAME_N_CNT_UP_1:
  case BTN_ID_POKEMAME_R_CNT_UP_1:
  case BTN_ID_POKEMAME_SR_CNT_UP_1:
    {
      return DialCounter::DIAL_BTN_TYPE_UP_1;
    }
  case BTN_ID_POKEMAME_N_CNT_DW_1:
  case BTN_ID_POKEMAME_R_CNT_DW_1:
  case BTN_ID_POKEMAME_SR_CNT_DW_1:
    {
      return DialCounter::DIAL_BTN_TYPE_DW_1;
    }
  }

  GFL_ASSERT(0);
  return DialCounter::DIAL_BTN_TYPE_UP_1;
}





//==============================================================================
/**
 *      UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerBeansSelectDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  switch( paneID )
  {
  case BTN_ID_CANCEL:
    {
      m_Result = RESULT_CANCEL;
    } break;

  case BTN_ID_DECIDE:
    {
      DecideFunc();    //!< 決定処理

      m_Result = RESULT_DECIDE;
    } break;

  default:
    {
      BeansKind beansKind = GetBeansKind( paneID );
      DialCounter::DialBtnType btnType = GetDialBtnType( paneID );

      // カウンターの更新
      UpdateCounter( beansKind, btnType );
    }
  }

  return ENABLE_ACCESS;
}






//==============================================================================
/**
 *      ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerBeansSelectDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  btnMan->SetButtonPassive( BTN_ID_DECIDE );

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





//==============================================================================
// @brief   カウンタークラス
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   base    描画共通処理
 * @param   index   カウンターのインデックス
 */
//------------------------------------------------------------------------------
DialCounter::DialCounter( DrawerBase* base, u32 index )
  : m_pBase(base)
  , m_DialType(DIAL_TYPE_BONUS)
  , m_UseNumMax(0)
  , m_HaveNumMax(0)
  , m_HaveNum(0)
  , m_UseNum(0)
{
  // ペインの初期化
  InitializePane( index );

  // 値の表示を更新
  SetText();
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 *
 * @param   index   カウンターのインデックス
 */
//------------------------------------------------------------------------------
void DialCounter::InitializePane( u32 index )
{
  // カウンターの数
  const u32 c_CounterParts[] = {
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_02,
    PANENAME_RESORT_CMD_LOW_000_PANE_DIAL_03,
  };
  static const u32 c_ValueTextIndex[] = {
    PANENAME_RESORT_PLT_LOW_000_PANE_TEXTCOUNTER_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_TEXTCOUNTER_01,
  };

  gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_CounterParts[index] );
  {
    for( u32 i=0; i<KETA_NUM; ++i )
    {
      m_pTextCounterNum[i] = m_pBase->GetTextBox( c_ValueTextIndex[i], parts );
      GFL_ASSERT( m_pTextCounterNum[i] );
    }
  }

  // 所持マメの数
  const u32 c_HaveBeansPaneIndex[] = {
    PANENAME_RESORT_CMD_LOW_000_PANE_MAMENUMB_00,
    PANENAME_RESORT_CMD_LOW_000_PANE_MAMENUMB_01,
    PANENAME_RESORT_CMD_LOW_000_PANE_MAMENUMB_02,
  };

  m_pTextHaveNum = m_pBase->GetTextBox( c_HaveBeansPaneIndex[index] );
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのリセット
 */
//------------------------------------------------------------------------------
void DialCounter::ResetParam( void )
{
  m_UseNumMax  = 0;
  m_HaveNumMax = 0;
  m_HaveNum    = 0;
  m_UseNum     = 0;

  // 値の表示を更新
  SetText();
}


//------------------------------------------------------------------------------
/**
 * @brief   ダイヤル操作
 *
 * @param   type      ダイヤルボタンのタイプ
 * @param   useNum    全体の使用数
 */
//------------------------------------------------------------------------------
void DialCounter::Dial( DialCounter::DialBtnType type, u32 useNum )
{
  u32 useMax = COUNTER_NUM_MAX;

  if( m_DialType == DIAL_TYPE_BONUS )
  {
    // セット可能数 = 全体でセットできる数 - 現在の使用数 + カウンターでセットしている数
    useMax = m_UseNumMax - useNum + m_UseNum;
  }

  FUKUSHIMA_PRINT( "type[%d] useNum[%d] useMax[%d]\n", type, useNum, useMax );
  FUKUSHIMA_PRINT( "m_HaveNum[%d] m_UseNum[%d]\n", m_HaveNum, m_UseNum );

  void (DialCounter::*DIAL_FUNC_TABLE[DIAL_BTN_TYPE_NUM])( u32 ) = {
    &DialCounter::Dial_Up_10,
    &DialCounter::Dial_Up_1,
    &DialCounter::Dial_Dw_10,
    &DialCounter::Dial_Dw_1,
  };
  (this->*DIAL_FUNC_TABLE[type])( useMax );

  FUKUSHIMA_PRINT( "m_HaveNum[%d] m_UseNum[%d]\n", m_HaveNum, m_UseNum );

  // 値の表示を更新
  SetText();
}

//------------------------------------------------------------------------------
/**
 * @brief   ダイヤル操作
 *
 * @param   useMax    セットできる最大数
 */
//------------------------------------------------------------------------------
void DialCounter::Dial_Up_10( u32 useMax )
{
  if( useMax > 0 )
  {
    if( useMax == m_UseNum || m_HaveNum == 0 )
    {
      u32 subNum = m_UseNum;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
    else if( (m_HaveNum > 0 && m_HaveNum < 10) || (useMax - m_UseNum) < 10 )
    {
      u32 subNum = 0;

      if( m_HaveNum < (useMax - m_UseNum) )
      {
        subNum = m_HaveNum;
      }
      else {
        subNum = (useMax - m_UseNum);
      }

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
    else {
      u32 subNum = 10;

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
  }
}

void DialCounter::Dial_Up_1( u32 useMax )
{
  if( useMax > 0 )
  {
    if( useMax == m_UseNum )
    {
      u32 subNum = m_UseNum;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
    else if( m_HaveNum == 0 )
    {
      u32 subNum = m_UseNum;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
    else if( (m_UseNum + 1) > useMax )
    {
      u32 subNum = useMax - m_UseNum;

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
    else {
      u32 subNum = 1;

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
  }
}

void DialCounter::Dial_Dw_10( u32 useMax )
{
  if( !((useMax == 0) && (m_UseNum == 0)) )
  {
    if( m_UseNum == 0 )
    {
      u32 subNum = 0;

      if( useMax < m_HaveNum )
      {
        subNum = useMax;
      }
      else {
        subNum = m_HaveNum;
      }

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
    else if( m_UseNum < 10 )
    {
      u32 subNum = m_UseNum;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
    else {
      u32 subNum = 10;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
  }
}

void DialCounter::Dial_Dw_1( u32 useMax )
{
  if( !((useMax == 0) && (m_UseNum == 0)) )
  {
    if( m_UseNum == 0 )
    {
      u32 subNum = 0;

      if( useMax < m_HaveNum )
      {
        subNum = useMax;
      }
      else {
        subNum = m_HaveNum;
      }

      m_HaveNum -= subNum;
      m_UseNum  += subNum;
    }
    else {
      u32 subNum = 1;

      m_HaveNum += subNum;
      m_UseNum  -= subNum;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   テキストのセット
 */
//------------------------------------------------------------------------------
void DialCounter::SetText( void )
{
  // 所持数テキストの設定
  SetText_HaveNum();
  // カウンターテキストの設定
  SetText_CounterNum();
}

void DialCounter::SetText_HaveNum( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    view->SetTextboxPaneNumber( m_pTextHaveNum, msg_prsys_common_15, m_HaveNum, 4 );
  }
}

void DialCounter::SetText_CounterNum( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    // 10
    view->SetTextboxPaneNumber( m_pTextCounterNum[0], msg_prsys_common_04, (m_UseNum / 10), 1 );
    // 1
    view->SetTextboxPaneNumber( m_pTextCounterNum[1], msg_prsys_common_04, (m_UseNum % 10), 1 );
  }
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

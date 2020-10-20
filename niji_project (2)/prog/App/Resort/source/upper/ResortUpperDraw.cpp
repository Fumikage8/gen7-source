//======================================================================
/**
 * @file    ResortUpperDraw.cpp
 * @date    2015/09/08 19:20:16
 * @author  fukushima_yuya
 * @brief   ポケリゾート：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortInitLogic.h>
#include <App/Resort/source/ResortUtility.h>

#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/ZukanSave.h>

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
enum {
  RESORT_FEED_OUT_L,    //!< 左へアウト
  RESORT_FEED_IN_L,     //!< 左からイン
  RESORT_FEED_OUT_R,    //!< 右へアウト
  RESORT_FEED_IN_R,     //!< 右からイン
  RESORT_FEED_NUM,
};

//! ポケモンアイコンID
enum {
  POKEICON_ID_FUREAI_BEGIN  = 0,
  POKEICON_ID_FUREAI_END    = POKEICON_ID_FUREAI_BEGIN + FUREAI_WILD_MAX,
  POKEICON_ID_GIM_BEGIN     = POKEICON_ID_FUREAI_END,
  POKEICON_ID_GIM_END       = POKEICON_ID_GIM_BEGIN + GIM_POKE_MAX,
  POKEICON_ID_HOTSPA_BEGIN  = POKEICON_ID_GIM_END,
  POKEICON_ID_HOTSPA_END    = POKEICON_ID_HOTSPA_BEGIN + HOTSPA_POKE_MAX,
  POKEICON_ID_NUM           = POKEICON_ID_HOTSPA_END,
};

//! 切り替え
static const u32 c_FeedResort[RESORT_ID_MAX][RESORT_FEED_NUM] = {
  { // ふれあい
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_16,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_17,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_18,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_19,
  },
  { // きのみ
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_12,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_13,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_14,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_15,
  },
  { // たんけん
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_08,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_09,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_10,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_11,
  },
  { // うんどう
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_04,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_05,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_06,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_07,
  },
  { // おんせん
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_00,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_01,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_02,
    LA_RESORT_RESORT_MIN_UPP_000_PAGE_CHANGE_03,
  },
};

//! レベルごとの表示チームアニメ
static const u32 c_LevelTeamCountAnim[] = {
  LA_RESORT_RESORT_MIN_UPP_000_TEAM_00,     //!< ふれあい
  LA_RESORT_RESORT_MIN_UPP_000_TEAM_01,     //!< きのみ
  LA_RESORT_RESORT_MIN_UPP_000_TEAM_00,     //!< 帳尻あわせで追加
  LA_RESORT_RESORT_MIN_UPP_000_TEAM_02,     //!< うんどう
  LA_RESORT_RESORT_MIN_UPP_000_TEAM_03,     //!< おんせん
};


#include "ResortPaneData_Friendship.cdat"
#include "ResortPaneData_Farm.cdat"
#include "ResortPaneData_Gim.cdat"
#include "ResortPaneData_Hotspa.cdat"


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
UpperDraw::UpperDraw( CreateParam& param )
  : UpperBaseDraw( param )
  , m_ResortID(0)
  , m_NextFarmIndex(FARM_MAX)
  , m_AdventureEnable(false)
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
void UpperDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_MIN_UPP_000_BFLYT,
      LA_RESORT_RESORT_MIN_UPP_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
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
void UpperDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );

      m_pDrawerBase->StartAnim( LA_RESORT_RESORT_MIN_UPP_000_KEEP );
    }
    
    // ポケモンアイコン
    app::tool::PokeIconToolEx::PokeIconToolParam pokeIconParam;
    {
      pokeIconParam.pUIView = this;
      pokeIconParam.pHeap   = m_pIconHeap;
      pokeIconParam.iconMax = POKEICON_ID_NUM;
      pokeIconParam.sync    = true;
      pokeIconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreatePokeIcon( pokeIconParam );
    
    // アイテムアイコン
    app::tool::ItemIconToolEx::ItemIconToolParam itemIconParam;
    {
      itemIconParam.pUIView = this;
      itemIconParam.pHeap   = m_pIconHeap;
      itemIconParam.iconMax = FARM_MAX + 1; //!< はたけ[18]＋次[1]
      itemIconParam.sync    = true;
      itemIconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreateItemIcon( itemIconParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane( void )
{
  // 各リゾートのペインの初期化
  void (UpperDraw::*INITIALIZE_PANE[RESORT_ID_MAX])(void) = {
    &UpperDraw::InitializePane_Friendship,    //!< ふれあい
    &UpperDraw::InitializePane_Farm,          //!< きのみ
    &UpperDraw::InitializePane_Adventure,     //!< 探検
    &UpperDraw::InitializePane_Gim,           //!< うんどう
    &UpperDraw::InitializePane_Hotspa,        //!< 温泉
  };

  for( u32 i=0; i< RESORT_ID_MAX; ++i )
  {
    (this->*INITIALIZE_PANE[i])();
  }

  // 時間表示テキストボックスのセット
  static const u32 c_TimeTextBox[TIME_KIND_NUM] = {
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_F,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F3_L_K,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F3_R_K,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_L_T,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_R_T,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F3_L_U,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F3_R_U,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_O,
  };
  for( u32 i=0; i<TIME_KIND_NUM; ++i )
  {
    m_pTextTime[i] = m_pDrawerBase->GetTextBox( c_TimeTextBox[i] );
  }

  // 時間の更新
  UpdateTime();

  // 各リゾートのセットアップ
  for( u8 i=0; i<RESORT_ID_MAX; ++i )
  {
    SetupResort( i );
  }

  SetupResortOther_Adventure();
}



//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializeText( void )
{
  // リゾート名
  {
    static const u32 c_ResortNamePane[] = {
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_TITLE_F,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_TITLE_K,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_TITLE_T,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_TITLE_U,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_TITLE_O,
    };
    static const u32 c_ResortNameMsgID[] = {
      msg_prsys_up_01_01,
      msg_prsys_up_02_01,
      msg_prsys_up_03_01,
      msg_prsys_up_04_01,
      msg_prsys_up_05_01,
    };

    for( u8 i=0; i<GFL_NELEMS(c_ResortNamePane); ++i )
    {
      gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( c_ResortNamePane[i] );

      this->SetTextboxPaneMessage( textBox, c_ResortNameMsgID[i] );
    }
  }

  // ガイドメッセージ
  {
    static const u32 c_ResortGuidePane[] = {
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_F,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_K,
    };
    static const u32 c_ResortGuideMsgID[] = {
      msg_prsys_up_01_02,
      msg_prsys_up_02_02,
    };

    for( u8 i=0; i<GFL_NELEMS(c_ResortGuidePane); ++i )
    {
      gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( c_ResortGuidePane[i] );

      this->SetTextboxPaneMessage( textBox, c_ResortGuideMsgID[i] );
    }
  }

  // グループ名
  {
    static const u32 c_GroupNamePane[] = {
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_K_00,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_K_01,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_K_02,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_U_00,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_U_01,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_U_02,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_O_00,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_O_01,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_O_02,
    };
    static const u32 c_GroupNameID[] = {
      msg_prsys_up_02_05,
      msg_prsys_up_02_06,
      msg_prsys_up_02_07,
      msg_prsys_up_04_03,
      msg_prsys_up_04_04,
      msg_prsys_up_04_05,
      msg_prsys_up_05_03,
      msg_prsys_up_05_04,
      msg_prsys_up_05_05,
    };

    for( u8 i=0; i<GFL_NELEMS(c_GroupNamePane); ++i )
    {
      gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( c_GroupNamePane[i] );

      this->SetTextboxPaneMessage( textBox, c_GroupNameID[i] );
    }
  }

  // 時間項目
  {
    static const u32 c_TimeTitlePane[] = {
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_F,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_L_K,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_R_K,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_L_T,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_R_T,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_L_U,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_R_U,
      PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F1_O,
    };
    static const u32 c_TimeTitleMsgID[] = {
      msg_prsys_up_01_04,
      msg_prsys_up_02_03,
      msg_prsys_up_02_04,
      msg_prsys_up_03_03,
      msg_prsys_up_03_04,
      msg_prsys_up_04_06,
      msg_prsys_up_04_07,
      msg_prsys_up_05_06,
    };

    for( u8 i=0; i<GFL_NELEMS(c_TimeTitlePane); ++i )
    {
      gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( c_TimeTitlePane[i] );

      this->SetTextboxPaneMessage( textBox, c_TimeTitleMsgID[i] );
    }
  }

  // レベルアイコン
  {
    // うんどうリゾート
    for( u32 i=0; i<GIM_POKE_MAX; ++i )
    {
      PokePaneData* paneData = &m_GimPokePaneData[i];
      {
        this->SetTextboxPaneMessage( paneData->textLv, msg_prsys_common_02 );
      }
    }
    // おんせんリゾート
    for( u32 i=0; i<HOTSPA_POKE_MAX; ++i )
    {
      PokePaneData* paneData = &m_HotSpaPokePaneData[i];
      {
        this->SetTextboxPaneMessage( paneData->textLv, msg_prsys_common_02 );
      }
    }
  }

  // アラートメッセージ
  {
    // ボーナスタイム用
    for( u32 i=0; i<RESORT_ID_MAX; ++i )
    {
      this->SetTextboxPaneMessage( m_pTextAlertBonusTime[i], msg_prsys_common_17 );
    }

    // 次に終わる用
    static const u32 c_NextAlertMsg[] = {
      msg_prsys_temp01,
      msg_prsys_temp02,
      msg_prsys_temp03,
    };
    for( u32 i=0; i<NEXT_ALERT_NUM; ++i )
    {
      this->SetTextboxPaneMessage( m_pTextAlertNext[i], c_NextAlertMsg[i] );
    }

    // ふれあい用
    this->SetTextboxPaneMessage( m_pTextAlert_Friendship, msg_prsys_up_01_03 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：ふれあいリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_Friendship( void )
{
  // ベースペインの初期化
  InitializePane_BasePane( RESORT_ID_FRIENDSHIP );

  // ペインデータ
  for( u32 i=0; i<FUREAI_WILD_MAX; ++i )
  {
    SetPokePaneData( &m_FureaiPokePaneData[i], c_FureaiPokePane[i] );
  }

  // ボーナスタイム未設定テキスト
  InitializePane_BonusTimeAlert( RESORT_ID_FRIENDSHIP );

  // 出現数０
  m_pTextAlert_Friendship = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_ERROR );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：きのみリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_Farm( void )
{
  // ベースペインの初期化
  InitializePane_BasePane( RESORT_ID_FARM );

  // ボーナスタイム未設定テキスト
  InitializePane_BonusTimeAlert( RESORT_ID_FARM );

  // 次に収穫できるきのみアイコン
  m_pPicNextIcon_Farm = m_pDrawerBase->GetPicture( PANENAME_RESORT_MIN_UPP_000_PANE_KINOMIICON_SAMPLE );
  // 次に収穫できるきのみ名
  m_pTextNextName_Farm = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_K );
  // きのみをうえていない：アラートテキスト
  m_pTextAlertNext[NEXT_ALERT_FARM] = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_K_00 );
  // きのみをうえていない：表示切替
  m_pPaneNextVisible_Farm = m_pDrawerBase->GetPane( PANENAME_RESORT_MIN_UPP_000_PANE_NEXT_VISIBLE_K );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：たんけんリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_Adventure( void )
{
  // ベースペインの初期化
  InitializePane_BasePane( RESORT_ID_ADVENTURE );

  // ボーナスタイム未設定テキスト
  InitializePane_BonusTimeAlert( RESORT_ID_ADVENTURE );

  // ガイドテキスト
  m_pTextGuide_Adv = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_H2_T );

  // コース表示用ペイン
  static const u32 c_AdvVisiblePane[] = {
    PANENAME_RESORT_MIN_UPP_000_PANE_BG_ITEM,
    PANENAME_RESORT_MIN_UPP_000_PANE_IMAGE_T_01,
  };
  for( u32 i=0; i<GFL_NELEMS(c_AdvVisiblePane); ++i )
  {
    m_pPaneVisible_Adv[i] = m_pDrawerBase->GetPane( c_AdvVisiblePane[i] );
  }

  // たんけんに出していない
  m_pTextAlertNext[NEXT_ALERT_ADV] = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_T_00 );
  // たんけんコース名
  m_pTextCourseName_Adv = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：うんどうリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_Gim( void )
{
  // ベースペインの初期化
  InitializePane_BasePane( RESORT_ID_GIM );

  // ペインデータ
  for( u32 i=0; i<GIM_POKE_MAX; ++i )
  {
    SetPokePaneData( &m_GimPokePaneData[i], c_GimPokePane[i] );
  }

  // ボーナスタイム未設定テキスト
  InitializePane_BonusTimeAlert( RESORT_ID_GIM );

  // 次に終わるグループ名
  m_pTextNextName_Gim = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXT_F2_U );

  // うんどうをさせていない
  m_pTextAlertNext[NEXT_ALERT_GIM] = m_pDrawerBase->GetTextBox( PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_U_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：おんせんリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_Hotspa( void )
{
  // ベースペインの初期化
  InitializePane_BasePane( RESORT_ID_HOTSPA );

  // ペインデータ
  for( u32 i=0; i<HOTSPA_POKE_MAX; ++i )
  {
    SetPokePaneData( &m_HotSpaPokePaneData[i], c_HotSpaPokePane[i] );
  }

  // ボーナスタイム未設定テキスト
  InitializePane_BonusTimeAlert( RESORT_ID_HOTSPA );
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：ベースペイン
 *
 * @param   resortID    リゾートID
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_BasePane( u8 resortID )
{
  static const u32 c_ResortNull[RESORT_ID_MAX] = {
    PANENAME_RESORT_MIN_UPP_000_PANE_FUREAI,
    PANENAME_RESORT_MIN_UPP_000_PANE_KINOMI,
    PANENAME_RESORT_MIN_UPP_000_PANE_TANKEN,
    PANENAME_RESORT_MIN_UPP_000_PANE_UNDOU,
    PANENAME_RESORT_MIN_UPP_000_PANE_ONSEN,
  };

  // ベースペインの設定
  m_pPaneResortBase[resortID] = m_pDrawerBase->GetPane( c_ResortNull[resortID] );

  // 
  if( resortID != m_ResortID )
  {
    gfl2::math::VEC3 pos = m_pPaneResortBase[resortID]->GetTranslate();
    {
      pos.x = -400.0f;
    }
    m_pPaneResortBase[resortID]->SetTranslate( pos );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化：ボーナスタイム未設定テキスト
 *
 * @param   resortID    リゾートID
 */
//------------------------------------------------------------------------------
void UpperDraw::InitializePane_BonusTimeAlert( u8 resortID )
{
  static const u32 c_BeansAlertTextBox[RESORT_ID_MAX] = {
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_F,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_K_01,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_T_01,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_U_01,
    PANENAME_RESORT_MIN_UPP_000_PANE_TEXTARART_O,
  };

  // ボーナスタイム未設定テキスト：「ポケマメが ありません」
  m_pTextAlertBonusTime[resortID] = m_pDrawerBase->GetTextBox( c_BeansAlertTextBox[resortID] );
}



//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ
 *
 * @param   resortID    リゾートID
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort( u8 resortID )
{
  // リゾートセットアップ関数
  void (UpperDraw::*SETUP_RESORT[RESORT_ID_MAX])(void) = {
    &UpperDraw::SetupResort_Friendship,     //!< ふれあい
    &UpperDraw::SetupResort_Farm,           //!< きのみ
    &UpperDraw::SetupResort_Adventure,      //!< たんけん
    &UpperDraw::SetupResort_Gim,            //!< うんどう
    &UpperDraw::SetupResort_Hotspa,         //!< おんせん
  };

  (this->*SETUP_RESORT[resortID])();
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ：ふれあいリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort_Friendship( void )
{
  // リゾートのタイプ設定
  SetResortType( RESORT_ID_FRIENDSHIP );

  // 野生ポケモンを非表示
  for( u8 i=0; i<FUREAI_WILD_MAX; ++i )
  {
    m_pDrawerBase->SetVisiblePane( m_FureaiPokePaneData[i].null, false );
  }

  u8 paneIndex = 0;

  // 野生ポケモンの表示
  for( u8 i=0; i<FUREAI_WILD_MAX; ++i )
  {
    if( IsWildPokemon( i ) )
    {
      // 野生ポケモンのパラメータを取得
      pml::pokepara::CoreParam* cp = CreatePokeParam( RESORT_ID_FRIENDSHIP, i );
      {
        // ポケモンアイコンのセット
        SetPokeIcon( cp, RESORT_ID_FRIENDSHIP, paneIndex );
        // 種族名のセット
        SetMonsName( cp, paneIndex );
        // 性格のセット
        SetSeikaku( cp, paneIndex );
        // 性別のセット
        SetPokeIcon( cp, RESORT_ID_FRIENDSHIP, paneIndex );
        // ポケモンアイコンのセット
        SetGender( cp, RESORT_ID_FRIENDSHIP, paneIndex );
        // 野生ポケモンのイベント状態のセット
        SetWildPokeEventState( cp, paneIndex );
      }
      // 野生ポケモンパラメータの破棄
      GFL_SAFE_DELETE( cp );

      // 野生ポケモンを表示
      m_pDrawerBase->SetVisiblePane( m_FureaiPokePaneData[paneIndex].null, true );

      paneIndex++;
    }
  }

  // 出現数０のメッセージ表示
  m_pDrawerBase->SetVisiblePane( m_pTextAlert_Friendship, (GetWildPokeAppearCount() == 0) );
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ：きのみリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort_Farm( void )
{
  // リゾートのタイプ設定
  SetResortType( RESORT_ID_FARM );

  // 畑内のアイテムアイコン
  for( u32 i=0; i<FARM_MAX; ++i )
  {
    gfl2::lyt::LytPicture* pic = m_pDrawerBase->GetPicture( c_IconPicPane[i] );

    u16 itemNo = m_pResortSave->GetFarmItemKind( i );
    bool vis = (itemNo != ITEM_DUMMY_DATA);

    if( vis )
    {
      app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
      {
        itemIcon->SetItemIcon( i, itemNo, pic );
      }
    }

    // アイコンの表示切替
    m_pDrawerBase->SetVisiblePane( pic, vis );
  }

  // 次の収穫アイコン
  if( m_NextFarmIndex < FARM_MAX )
  {
    u16 itemNo = m_pResortSave->GetFarmItemKind( m_NextFarmIndex );

    if( itemNo != ITEM_DUMMY_DATA )
    {
      // アイテムアイコン
      app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
      {
        itemIcon->SetItemIcon( FARM_MAX, itemNo, m_pPicNextIcon_Farm );
      }

      // アイテム名
      print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
      {
        wordSet->RegisterItemName( 0, itemNo, 1 );
      }
      this->SetTextboxPaneMessage( m_pTextNextName_Farm, msg_prsys_common_05 );
    }

    // アイコンの表示切替
    m_pDrawerBase->SetVisiblePane( m_pPaneNextVisible_Farm, (itemNo != ITEM_DUMMY_DATA) );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertNext[NEXT_ALERT_FARM], (itemNo == ITEM_DUMMY_DATA) );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pPaneNextVisible_Farm, false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertNext[NEXT_ALERT_FARM], true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ：たんけんリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort_Adventure( void )
{
  // リゾートのタイプ設定
  SetResortType( RESORT_ID_ADVENTURE );

  u8 courseID = m_pResortSave->GetAdventureCoursePattern();
  bool enable = !(courseID == Savedata::ResortSave::RESORT_ADV_COURSE_NONE);

  this->SetTextboxPaneMessage( m_pTextGuide_Adv, msg_prsys_up_03_02 );

  if( !enable )
  {
    // コース未選択
    this->SetTextboxPaneMessage( m_pTextCourseName_Adv, msg_prsys_temp04 );
  }
  else {
    static const u32 c_AdventureCourseName[] = {
      msg_prsys_low_menu_03_01_01,
      msg_prsys_low_menu_03_01_02,
      msg_prsys_low_menu_03_01_04,
      msg_prsys_low_menu_03_01_03,
      msg_prsys_low_menu_03_01_04,
    };
    this->SetTextboxPaneMessage( m_pTextCourseName_Adv, c_AdventureCourseName[courseID] );

    // 選択したコース
    static const u32 c_AdventureCourseAnim[] = {
      LA_RESORT_RESORT_MIN_UPP_000_ICON_TYPE_00,
      LA_RESORT_RESORT_MIN_UPP_000_ICON_TYPE_01,
      LA_RESORT_RESORT_MIN_UPP_000_ICON_TYPE_03,
      LA_RESORT_RESORT_MIN_UPP_000_ICON_TYPE_02,
      LA_RESORT_RESORT_MIN_UPP_000_ICON_TYPE_03,
    };
    m_pDrawerBase->StartAnim( c_AdventureCourseAnim[courseID] );
  }

  // 表示切替
  for( u32 i=0; i<ADV_VISIBLE_NUM; ++i )
  {
    m_pDrawerBase->SetVisiblePane( m_pPaneVisible_Adv[i], enable );
  }
  //m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_TANKEN_BACK], m_AdventureEnable );
  //m_pDrawerBase->SetVisiblePane( m_pTextAlertNext[NEXT_ALERT_ADV], !m_AdventureEnable );
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ：うんどうリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort_Gim( void )
{
  // リゾートのタイプ設定
  SetResortType( RESORT_ID_GIM );

  for( u32 i=0; i<GIM_POKE_MAX; ++i )
  {
    pml::pokepara::CoreParam* cp = CreatePokeParam( RESORT_ID_GIM, i );
    {
      PokeTool::SimpleParam sp;
      u32 level = 0;

      PokePaneData* paneData = &m_GimPokePaneData[i];

      if( !cp->IsNull() )
      {
        // ポケモンのパラメータをセット
        util::SetSimpleParam( &sp, &level, cp );

        // ポケモンアイコンのセット
        if( !PokeTool::CompareSimpleParam( sp, m_SimpleParam_Gim[i] ) )
        {
          SetPokeIcon( cp, RESORT_ID_GIM, i );
        }
        // レベルのセット
        if( level != m_LevelNum_Gim[i] )
        {
          SetLevel( cp, RESORT_ID_GIM, i );
        }
        // 性別のセット
        if( sp.sex != m_SimpleParam_Gim[i].sex )
        {
          SetGender( cp, RESORT_ID_GIM, i );
        }

        // 表示切替
        m_pDrawerBase->SetVisiblePane( paneData->null, true );
        m_pDrawerBase->SetVisiblePane( paneData->iconParts, !util::IsEgg( cp ) );
      }
      else {
        // 表示切替
        m_pDrawerBase->SetVisiblePane( paneData->null, false );
      }

      m_SimpleParam_Gim[i] = sp;
      m_LevelNum_Gim[i]    = level;
    }
    // ポケモンパラメータの破棄
    GFL_SAFE_DELETE( cp );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ：おんせんリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResort_Hotspa( void )
{
  // リゾートのタイプ設定
  SetResortType( RESORT_ID_HOTSPA );

  for( int i=0; i<HOTSPA_POKE_MAX; ++i )
  {
    pml::pokepara::CoreParam* cp = CreatePokeParam( RESORT_ID_HOTSPA, i );
    {
      PokePaneData* paneData = &m_HotSpaPokePaneData[i];

      if( !cp->IsNull() )
      {
        // ポケモンアイコンのセット
        SetPokeIcon( cp, RESORT_ID_HOTSPA, i );
        // レベルのセット
        SetLevel( cp, RESORT_ID_HOTSPA, i );
        // 性別のセット
        SetGender( cp, RESORT_ID_HOTSPA, i );

        // 表示切替
        m_pDrawerBase->SetVisiblePane( paneData->null, true );
      }
      else {
        // 表示切替
        m_pDrawerBase->SetVisiblePane( paneData->null, false );
      }
    }
    // ポケモンパラメータの破棄
    GFL_SAFE_DELETE( cp );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾートのセットアップ以外のセットアップ
 */
//------------------------------------------------------------------------------
void UpperDraw::SetupResortOther_Adventure( void )
{
  // コースIDの取得
  u8 courseID = m_pResortSave->GetAdventureCoursePattern();
  {
    m_AdventureEnable = !(courseID == Savedata::ResortSave::RESORT_ADV_COURSE_NONE);

    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_TANKEN_BACK], m_AdventureEnable );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertNext[NEXT_ALERT_ADV], !m_AdventureEnable );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void UpperDraw::Update( void )
{
  app::ui::UIView::Update();

  //// 時間の更新
  //UpdateTime();

  //// 野生ポケモンの表示を更新
  //UpdateWildPokemon();

  //// たんけんコースの表示を更新
  //UpdateAdvCourse();
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾートの更新
 *
 * @param   resortID
 *
 * @note    デフォルトはRESORT_ID_MAXを指定
 *          RESORT_ID_MAXの場合は特定のリゾート以外は更新を行わない
 *          それ以外のIDが指定された場合は指定されたリゾートを強制的に更新する
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateResort( u8 resortID )
{
  switch(m_ResortID)
  {
  case RESORT_ID_FRIENDSHIP:
    {
      // 野生ポケモンの更新フラグのチェック
      if( m_pInitLogic->IsWildPokemonUpdate() )
      {
        // ふれあい上画面の更新
        SetupResort_Friendship();

        // フラグを初期化
        m_pInitLogic->ResetWildPokemonUpdate();
      }
    } break;

  case RESORT_ID_FARM:
    {
      // 強制的かどうか
      if( resortID == RESORT_ID_FARM )
      {
        // きのみ上画面を更新
        SetupResort_Farm();
      }
    } break;

  case RESORT_ID_ADVENTURE:
    {
      // 強制的かどうか
      if( resortID == RESORT_ID_ADVENTURE )
      {
        // たんけん上画面を更新
        SetupResort_Adventure();
      }
    } break;

  case RESORT_ID_GIM:
    {
      // うんどう上画面を更新
      SetupResort_Gim();
    } break;

  case RESORT_ID_HOTSPA:
    {
      // 強制的かどうか
      if( resortID == RESORT_ID_HOTSPA )
      {
        // おんせん上画面を更新
        SetupResort_Hotspa();
      }
    } break;
  }

  if( resortID == RESORT_ID_MAX )
  {
    // 時間の更新
    UpdateTime();
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   時間の更新
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime( void )
{
  // 時間更新関数
  void (UpperDraw::*UPDATE_TIME[RESORT_ID_MAX])(void) = {
    &UpperDraw::UpdateTime_Friendship,    //!< ふれあい
    &UpperDraw::UpdateTime_Farm,          //!< きのみ
    &UpperDraw::UpdateTime_Adventure,     //!< たんけん
    &UpperDraw::UpdateTime_Gim,           //!< うんどう
    &UpperDraw::UpdateTime_Hotspa,        //!< おんせん
  };

  for( u8 i=0; i<RESORT_ID_MAX; ++i )
  {
    (this->*UPDATE_TIME[i])();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間の更新：ふれあいリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime_Friendship( void )
{
  // ボーナスタイム
  TimeParam time;
  {
    // セーブデータから時間を取得   野生ポケモン抽選確率２倍時間
    util::GetBonusTime( m_pResortSave, RESORT_ID_FRIENDSHIP, &time );

    SetTimeText( time, TIME_KIND_HUREAI_BONUS );
  }

  // ボーナスタイムの表示
  if( !time.IsInvalid() && time.GetTotalSec() > 0 )
  {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_HUREAI_BONUS], true );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_FRIENDSHIP], false );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_HUREAI_BONUS], false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_FRIENDSHIP], true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間の更新：きのみリゾート
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime_Farm( void )
{
  // ボーナスタイム
  TimeParam bonusTime;
  {
    util::GetBonusTime( m_pResortSave, RESORT_ID_FARM, &bonusTime );
  }

  // 次に終わる時間
  TimeParam nextTime;
  {
    u32 prevNextFarm = m_NextFarmIndex;

    // 一番小さい時間を取得
    GetMinimumTime( RESORT_ID_FARM, &nextTime, &m_NextFarmIndex );

    if( !nextTime.IsInvalid() && nextTime.GetTotalSec() > 0 )
    {
      // タイムを表示
      SetTimeText( nextTime, TIME_KIND_KINOMI_NEXT );
      SetTextColor( bonusTime, TIME_KIND_KINOMI_NEXT );
    }
    else {
      m_NextFarmIndex = FARM_MAX;
    }

    // はたけのインデックスが変わったら画面を更新
    if( prevNextFarm != m_NextFarmIndex )
    {
      UpdateResort( RESORT_ID_FARM );
    }
  }

  // ボーナスタイムの表示
  if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
  {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_KINOMI_BONUS], true );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_FARM], false );

    SetTimeText( bonusTime, TIME_KIND_KINOMI_BONUS );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_KINOMI_BONUS], false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_FARM], true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間の更新(たんけんリゾート)
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime_Adventure( void )
{
  // ボーナスタイム
  TimeParam bonusTime;
  {
    util::GetBonusTime( m_pResortSave, RESORT_ID_ADVENTURE, &bonusTime );
  }

  // 探検終了までの時間
  TimeParam backTime;
  // セーブデータから時間を取得
  m_pResortSave->GetAdventureTime( &backTime.hour, &backTime.min, &backTime.sec );
  // ボーナスタイムの表示
  SetTimeText( backTime, TIME_KIND_TANKEN_BACK );
  SetTextColor( bonusTime, TIME_KIND_TANKEN_BACK );

  if( m_AdventureEnable )
  {
    if( backTime.IsInvalid() || backTime.GetTotalSec() == 0 )
    {
      UpdateResort( RESORT_ID_ADVENTURE );
      SetupResortOther_Adventure();
    }
  }

  // ボーナスタイムの表示
  if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
  {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_TANKEN_BONUS], true );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_ADVENTURE], false );

    SetTimeText( bonusTime, TIME_KIND_TANKEN_BONUS );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_TANKEN_BONUS], false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_ADVENTURE], true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間の更新(うんどうリゾート)
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime_Gim( void )
{
  // ボーナスタイム
  TimeParam bonusTime;
  {
    util::GetBonusTime( m_pResortSave, RESORT_ID_GIM, &bonusTime );
  }

  TimeParam nextTime;
  {
    u32 groupIndex;

    // 一番小さい時間を取得
    GetMinimumTime( RESORT_ID_GIM, &nextTime, &groupIndex );

    //if( !nextTime.IsInvalid() && nextTime.GetTotalSec() > 0 )
    if( m_pResortSave->GetGimState(groupIndex) == Savedata::ResortSave::RESORT_GIM_STATE_TRAINING )
    {
      // タイムの表示
      SetTimeText( nextTime, TIME_KIND_UNDOU_FINISH );
      SetTextColor( bonusTime, TIME_KIND_UNDOU_FINISH );

      m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_UNDOU_FINISH], true );
    }
    else {
      m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_UNDOU_FINISH], false );
    }

    // 次に終わるグループ名の表示
    SetNextGimGroupText( groupIndex );
  }

  // ボーナスタイムの表示
  if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
  {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_UNDOU_BONUS], true );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_GIM], false );

    SetTimeText( bonusTime, TIME_KIND_UNDOU_BONUS );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_UNDOU_BONUS], false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_GIM], true );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間の更新(おんせんリゾート)
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateTime_Hotspa( void )
{
  // ボーナスタイム
  TimeParam time;
  {
    util::GetBonusTime( m_pResortSave, RESORT_ID_HOTSPA, &time );
  }

  // ボーナスタイムの表示
  if( !time.IsInvalid() && time.GetTotalSec() > 0 )
  {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_ONSEN_BONUS], true );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_HOTSPA], false );

    SetTimeText( time, TIME_KIND_ONSEN_BONUS );
  }
  else {
    m_pDrawerBase->SetVisiblePane( m_pTextTime[TIME_KIND_ONSEN_BONUS], false );
    m_pDrawerBase->SetVisiblePane( m_pTextAlertBonusTime[RESORT_ID_HOTSPA], true );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   リゾートの変更
 * @param   index リゾートインデックス
 * @param   dir   移動方向(0: 左、1: 右)
 */
//------------------------------------------------------------------------------
void UpperDraw::FeedResort( u8 index, u8 dir )
{
  if( m_ResortID != index )
  {
    const u32* nowResortAnim  = c_FeedResort[m_ResortID];
    const u32* nextResortAnim = c_FeedResort[index];

    // 左へ移動
    if( dir == 0 )
    {
      m_pDrawerBase->StartAnim( nowResortAnim[RESORT_FEED_OUT_R] );
      m_pDrawerBase->StartAnim( nextResortAnim[RESORT_FEED_IN_L] );
    }
    else {
      m_pDrawerBase->StartAnim( nowResortAnim[RESORT_FEED_OUT_L] );
      m_pDrawerBase->StartAnim( nextResortAnim[RESORT_FEED_IN_R] );
    }

    m_ResortID = index;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンペインデータのセット
 */
//------------------------------------------------------------------------------
void UpperDraw::SetPokePaneData( PokePaneData* paneData, const u32* paneTbl )
{
  const u32 end = LYTRES_PANENAME_RESORT_PANE_END;
  u32 id;
  
  if( (id = paneTbl[POKE_PANE_ID_NULL]) < end ) {
    paneData->null = m_pDrawerBase->GetPane( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_ICON_PARTS]) < end ) {
    paneData->iconParts = m_pDrawerBase->GetParts( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_ICON_PIC]) < end ) {
    paneData->iconPic = m_pDrawerBase->GetPicture( id, paneData->iconParts );
  }

  if( (id = paneTbl[POKE_PANE_ID_TEXT_LV]) < end ) {
    paneData->textLv = m_pDrawerBase->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_TEXT_LV_NUM]) < end ) {
    paneData->textLvNum = m_pDrawerBase->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_GENDER]) < end ) {
    paneData->gender = m_pDrawerBase->GetPane( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_NAME]) < end ) {
    paneData->name = m_pDrawerBase->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_SEIKAKU]) < end ) {
    paneData->seikaku = m_pDrawerBase->GetTextBox( id );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   野生ポケモンの出現数の取得
 *
 * @return  "野生ポケモンの出現数"
 */
//------------------------------------------------------------------------------
u8 UpperDraw::GetWildPokeAppearCount( void )
{
  u8 appearCount = 0;
  {
    for( int i=0; i<Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX; ++i )
    {
      // 野生がいる場合はカウント
      if( IsWildPokemon( i ) )
      {
        appearCount++;
      }
    }
  }

  // 初回チュートリアルが終了していなければ強制的に０にする
  if( !m_pResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) )
  {
    appearCount = 0;
  }

  return appearCount;
}

//------------------------------------------------------------------------------
/**
 * @brief   野生ポケモンがいるかどうか
 *
 * @param   index   インデックス
 */
//------------------------------------------------------------------------------
bool UpperDraw::IsWildPokemon( u32 index )
{
  pml::pokepara::CoreParam* cp = NULL;
  {
    gfl2::heap::HeapBase* heap = m_pDrawerBase->GetDeviceHeap();
    {
      cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
    }
  }

  // 野生ポケモンを取得
  m_pResortSave->GetWildPokemon( index, cp );

  bool null = cp->IsNull();

  GFL_DELETE cp;

  return !null;
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのタイプ設定
 *
 * @param   resortID    リゾートID
 */
//------------------------------------------------------------------------------
void UpperDraw::SetResortType( u8 resortID )
{
  // リゾートのレベルを取得
  u8 level = util::GetResortLevel( m_pResortSave, resortID );

  switch( resortID )
  {
  case RESORT_ID_FRIENDSHIP:
    {
      m_pDrawerBase->StartAnim( c_LevelTeamCountAnim[resortID], false );

      // 野生ポケモンの出現数を取得
      u8 appearCount = GetWildPokeAppearCount();

      if( appearCount > 0 )
      {
        m_pDrawerBase->SetAnimFrame(
          c_LevelTeamCountAnim[RESORT_ID_FRIENDSHIP],
          static_cast<u32>(appearCount - 1) );
      }
    } break;

  case RESORT_ID_ADVENTURE:
    {
      static const u32 c_AdventureLevelAnim[] = {
        LA_RESORT_RESORT_MIN_UPP_000_IMAGE_TYPE_00,
        LA_RESORT_RESORT_MIN_UPP_000_IMAGE_TYPE_01,
        LA_RESORT_RESORT_MIN_UPP_000_IMAGE_TYPE_02,
      };
      m_pDrawerBase->StartAnim( c_AdventureLevelAnim[level - 1] );
    } break;

  case RESORT_ID_FARM:
  case RESORT_ID_GIM:
  case RESORT_ID_HOTSPA:
    {
      m_pDrawerBase->StartAnim( c_LevelTeamCountAnim[resortID], false );

      if( level > RESORT_LV_0 )
      {
        m_pDrawerBase->SetAnimFrame( c_LevelTeamCountAnim[resortID], static_cast<u32>(level - 1) );
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   各リゾートのポケモンパラメータを生成
 *
 * @param   resortID    リゾートID
 * @param   index       インデックス
 */
//------------------------------------------------------------------------------
pml::pokepara::CoreParam* UpperDraw::CreatePokeParam( u8 resortID, u32 index )
{
  pml::pokepara::CoreParam* cp = NULL;
  {
    gfl2::heap::HeapBase* heap = m_pDrawerBase->GetDeviceHeap();
    {
      // CoreParamの生成
      cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
      {
        GFL_ASSERT(cp);
      }
    }
  }

  switch( resortID )
  {
  case RESORT_ID_FRIENDSHIP:
    {
      m_pResortSave->GetWildPokemon( static_cast<int>(index), cp );
    } break;

  case RESORT_ID_GIM:
    {
      m_pResortSave->GetGimPokemon( static_cast<int>(index), cp );
    } break;

  case RESORT_ID_HOTSPA:
    {
      m_pResortSave->GetHotSpaPokemon( static_cast<int>(index), cp );
    } break;

  default:
    {
      GFL_ASSERT(0);  //!< ここに来てはいけない
    }
  }

  return cp;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの設定
 *
 * @param   cp        ポケモンパラメータ
 * @param   resortID  リゾートID
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetPokeIcon( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index )
{
  // ペインデータの取得
  PokePaneData* paneData = GetPaneData( resortID, index );
  // アイコンIDの取得
  u32 iconID = 0;
  {
    if( resortID == RESORT_ID_FRIENDSHIP )
    {
      iconID = POKEICON_ID_FUREAI_BEGIN + index;
    }
    else if( resortID == RESORT_ID_GIM )
    {
      iconID = POKEICON_ID_GIM_BEGIN + index;
    }
    else if( resortID == RESORT_ID_HOTSPA )
    {
      iconID = POKEICON_ID_HOTSPA_BEGIN + index;
    }
  }

  // ポケモンアイコンの差し替え
  app::tool::PokeIconToolEx* pokeIcon = m_pDrawerBase->GetPokeIcon();
  {
    pokeIcon->SetPokeIcon( iconID, cp, paneData->iconPic );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン種族名のセット
 *
 * @param   cp        ポケモンパラメータ
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetMonsName( const pml::pokepara::CoreParam* cp, u32 index )
{
  // 種族Noの取得
  MonsNo monsNo = MONSNO_NULL;
  {
    bool fastmode = cp->StartFastMode();
    {
      monsNo = cp->GetMonsNo();
    }
    cp->EndFastMode( fastmode );
  }

  gfl2::lyt::LytTextBox* textBox = m_FureaiPokePaneData[index].name;

  //// 種族がNULL以外で捕獲済みの場合、種族名を表示
  //if( monsNo != MONSNO_NULL && m_pZukanSave->GetPokeGetFlag( monsNo ) )
  if( monsNo != MONSNO_NULL )
  {
    print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
    {
      wordSet->RegisterPokeMonsName( 0, cp );
    }
    this->SetTextboxPaneMessage( textBox, msg_prsys_common_01 );
  }
  else {
    this->SetTextboxPaneMessage( textBox, msg_prsys_common_14 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの性格のセット
 *
 * @param   cp        ポケモンパラメータ
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetSeikaku( const pml::pokepara::CoreParam* cp, u32 index )
{
  // 性格の取得
  pml::pokepara::Seikaku seikaku = pml::pokepara::SEIKAKU_NUM;
  {
    bool fastmode = cp->StartFastMode();
    {
      seikaku = cp->GetSeikaku();
    }
    cp->EndFastMode( fastmode );

    GFL_ASSERT( seikaku < pml::pokepara::SEIKAKU_NUM );
  }

  print::WordSet* wordSet = m_pDrawerBase->GetWordSet();
  {
    wordSet->RegisterSeikakuName( 0, seikaku );
  }

  gfl2::lyt::LytTextBox* textBox = m_FureaiPokePaneData[index].seikaku;
  {
    this->SetTextboxPaneMessage( textBox, msg_prsys_common_03 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの性別のセット
 *
 * @param   cp        ポケモンパラメータ
 * @param   resortID  リゾートID
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetGender( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index )
{
  static const u32* c_GenderAnim_Male[] = {
    c_FureaiGenderAnim_Male,
    c_FureaiGenderAnim_Male,
    c_FureaiGenderAnim_Male,
    c_GimGenderAnim_Male,
    c_HotspaGenderAnim_Male,
  };
  static const u32* c_GenderAnim_Female[] = {
    c_FureaiGenderAnim_Female,
    c_FureaiGenderAnim_Female,
    c_FureaiGenderAnim_Female,
    c_GimGenderAnim_Female,
    c_HotspaGenderAnim_Female,
  };

  // ペインデータの取得
  PokePaneData* paneData = GetPaneData( resortID, index );
  
  // タマゴかどうか
  bool isEgg = util::IsEgg( cp );

  // 性別の取得
  pml::Sex sex = util::GetSex( cp );
  {
    if( sex != pml::SEX_UNKNOWN && !isEgg )
    {
      const u32* animTable = NULL;

      if( sex == pml::SEX_MALE )
      {
        animTable = c_GenderAnim_Male[resortID];
      }
      else {
        animTable = c_GenderAnim_Female[resortID];
      }
      m_pDrawerBase->StartAnim( animTable[index] );
    }

    // 表示切替
    m_pDrawerBase->SetVisiblePane( paneData->gender, (sex != pml::SEX_UNKNOWN && !isEgg) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンのレベルのセット
 *
 * @param   cp        ポケモンパラメータ
 * @param   resortID  リゾートID
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetLevel( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index )
{
  // ポケモンのレベルを取得
  u32 level = 0;
  {
    bool fastmode = cp->StartFastMode();
    {
      level = cp->GetLevel();
    }
    cp->EndFastMode( fastmode );
  }
  // タマゴかどうか
  bool isEgg = util::IsEgg( cp );

  // ペインデータの取得
  PokePaneData* paneData = GetPaneData( resortID, index );
  {
    // テキストのセット
    this->SetTextboxPaneNumber( paneData->textLvNum, msg_prsys_common_04, level, 3 );

    // 表示切替
    m_pDrawerBase->SetVisiblePane( paneData->textLv, !isEgg );
    m_pDrawerBase->SetVisiblePane( paneData->textLvNum, !isEgg );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   野生ポケモンのイベント状態のセット
 *
 * @param   cp        ポケモンパラメータ
 * @param   resortID  リゾートID
 * @param   index     インデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetWildPokeEventState( const pml::pokepara::CoreParam* cp, u32 index )
{
  // 野生ポケモンのイベント状態を取得
  pml::pokepara::ResortEventState state =
    static_cast<pml::pokepara::ResortEventState>( cp->GetResortEventStatus() );

  u32 anmID = c_FureaiWildPokeStateAnim[index];

  if( state == pml::pokepara::RESORTEVENT_WILD )
  {
    m_pDrawerBase->StartAnim( anmID, false, false );
    m_pDrawerBase->SetAnimFrame( anmID, 2.0f );
    m_pDrawerBase->SetVisiblePane( m_pDrawerBase->GetPane(c_FureaiWildPokeBubblePane[index]), true );
  }
  else if( /*state == pml::pokepara::RESORTEVENT_WILD_LOVE || */state == pml::pokepara::RESORTEVENT_WILD_LIVE )
  {
    m_pDrawerBase->StartAnim( anmID, false, false );
    m_pDrawerBase->SetAnimFrame( anmID, 0.0f );
    m_pDrawerBase->SetVisiblePane( m_pDrawerBase->GetPane(c_FureaiWildPokeBubblePane[index]), true );
  }
  else
  {
    GFL_ASSERT( state == pml::pokepara::RESORTEVENT_NONE || state == pml::pokepara::RESORTEVENT_WILD_LOVE );
    m_pDrawerBase->SetVisiblePane( m_pDrawerBase->GetPane(c_FureaiWildPokeBubblePane[index]), false );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ペインデータの取得
 *
 * @param   resortID    リゾートID
 * @param   index       インデックス
 */
//------------------------------------------------------------------------------
PokePaneData* UpperDraw::GetPaneData( u8 resortID, u32 index )
{
  PokePaneData* paneData = NULL;

  if( resortID == RESORT_ID_FRIENDSHIP )
  {
    paneData = &m_FureaiPokePaneData[index];
  }
  else if( resortID == RESORT_ID_GIM )
  {
    paneData = &m_GimPokePaneData[index];
  }
  else if( resortID == RESORT_ID_HOTSPA )
  {
    paneData = &m_HotSpaPokePaneData[index];
  }
  else {
    GFL_ASSERT(0);
  }

  return paneData;
}





//------------------------------------------------------------------------------
/**
 * @brief   タイムの表示
 *
 * @param   time      表示する時間
 * @param   timeType  表示箇所
 */
//------------------------------------------------------------------------------
void UpperDraw::SetTimeText( TimeParam time, u8 timeType )
{
  if( !time.IsInvalid() && time.GetTotalSec() > 0 )
  {
    //// １分以上ならタイムを表示
    //if( time.GetTotalSec() >= 60 )
    if( time.GetTotalSec() > 60 )
    {
      util::SetTextBoxTime( this, m_pDrawerBase->GetWordSet(), m_pTextTime[timeType], time );
    }
    // １分未満なら「あとちょっと！」を表示
    else {
      this->SetTextboxPaneMessage( m_pTextTime[timeType], msg_prsys_temp05 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイムのテキストカラー変更
 *
 * @param   bonusTime   ボーナスタイム
 * @param   timeType    変更箇所
 */
//------------------------------------------------------------------------------
void UpperDraw::SetTextColor( TimeParam bonusTime, u8 timeType )
{
  switch( timeType )
  {
  case TIME_KIND_KINOMI_NEXT:
  case TIME_KIND_TANKEN_BACK:
  case TIME_KIND_UNDOU_FINISH:
    {
      // テキストカラーの初期値
      nw::ut::Color8 color = nw::ut::Color8::WHITE;

      if( !bonusTime.IsInvalid() && bonusTime.GetTotalSec() > 0 )
      {
        color = 0x1987E5FF; // シアン：RGBA(25:135:22:255)
      }

      // テキストカラーの変更
      m_pTextTime[timeType]->SetTextColor( color, color );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   次に終わるグループ名の表示：うんどうリゾート
 *
 * @param   index   グループインデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::SetNextGimGroupText( u32 index )
{
  bool visible = (index < 3) && m_pResortSave->GetGimState(index) == Savedata::ResortSave::RESORT_GIM_STATE_TRAINING;

  if( visible )
  {
    static const u32 c_NextGimGroupName[] = {
      msg_prsys_up_04_03,
      msg_prsys_up_04_04,
      msg_prsys_up_04_05,
    };

    this->SetTextboxPaneMessage( m_pTextNextName_Gim, c_NextGimGroupName[index] );
  }

  // 表示切替
  m_pDrawerBase->SetVisiblePane( m_pTextNextName_Gim, visible );
  m_pDrawerBase->SetVisiblePane( m_pTextAlertNext[NEXT_ALERT_GIM], !visible );
}


//------------------------------------------------------------------------------
/**
 * @brief   一番小さい時間の取得
 * @param   resort    [in]リゾートの種類
 * @param   time      [out]一番小さい時間
 * @param   index     [out]一番小さい時間のインデックス
 */
//------------------------------------------------------------------------------
void UpperDraw::GetMinimumTime( u32 resort, TimeParam* time, u32* index )
{
  if( resort != RESORT_ID_FARM && resort != RESORT_ID_GIM ) return;

  u32 max = (resort == RESORT_ID_FARM) ? FARM_MAX : 3;

  // 0番目のインデックスの時間を保存
  TimeParam minTime = GetCheckMinTime( resort, 0 );
  // 最小時間のインデックス
  u32 minIndex = 0;

  for( u32 i=1; i<max; ++i )
  {
    // 比較対象の時間を取得
    TimeParam checkTime = GetCheckMinTime( resort, i );

    // 最小時間
    if( IsMinimumTime( minTime, checkTime ) )
    {
      minIndex = i;
      minTime  = checkTime;
    }
  }

  // データが空かをチェック
  if( minIndex == 0 )
  {
    if( minTime.IsInvalid() )
    {
      minIndex = max;
    }
  }

  // データの反映
  if( time )
  {
    *time = minTime;
  }
  if( index )
  {
    *index = minIndex;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   最小時間と比較する時間の取得
 *
 * @param   resort  リゾートインデックス
 * @param   index   インデックス
 *
 * @return  比較する時間
 */
//------------------------------------------------------------------------------
TimeParam UpperDraw::GetCheckMinTime( u32 resort, u32 index )
{
  TimeParam minTime;
  {
    if( resort == RESORT_ID_FARM )
    {
      // @fix GFNMCat[4177]
      if( m_pResortSave->GetFarmItemKind( index ) != ITEM_DUMMY_DATA )
      {
        m_pResortSave->GetFarmTime( index, &minTime.hour, &minTime.min, &minTime.sec );
      }
    }
    else if( resort == RESORT_ID_GIM )
    {
      m_pInitLogic->GetGimTotalEndTime( index, &minTime.hour, &minTime.min, &minTime.sec );
    }
  }

  return minTime;
}

//------------------------------------------------------------------------------
/**
 * @brief   最小時間の更新かどうか
 *
 * @param   minTime   最小時間
 * @param   time      比較する時間
 *
 * @return  "true  == 更新"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool UpperDraw::IsMinimumTime( TimeParam minTime, TimeParam time ) const
{
  // 比較対象が無効データまたは０秒だったらfalseを返す
  if( time.IsInvalid() )        return false;
  if( time.GetTotalSec() == 0 ) return false;

  // 比較対象が正常なデータの時に最小時間が無効データまたは０秒だったらtrueを返す
  if( minTime.IsInvalid() )        return true;
  if( minTime.GetTotalSec() == 0 ) return true;
  
  // 正常なデータ同士の場合は比較を行う
  if( minTime.GetTotalSec() > time.GetTotalSec() )
  {
    return true;  //!< 最小を更新
  }

  return false;   //!< 最小を維持
}





//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの取得
 *
 * @param   cp    [in]コアパラメータ
 * @param   sp    [out]描画用パラメータ(PokeTool::SimpleParam)
 * @param   lv    [out]レベル値
 *
 * @return  "true" = パラメータあり
 * @return  "false" = パラメータなし
 */
//------------------------------------------------------------------------------
bool UpperDraw::GetPokeParam( pml::pokepara::CoreParam* cp, PokeTool::SimpleParam* sp, u32* lv )
{
  util::SetSimpleParam( sp, lv, cp );

  return sp->monsNo != MONSNO_NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief   リゾート画面のレベル更新
 */
//------------------------------------------------------------------------------
void UpperDraw::UpdateResortLevel( void )
{
  static const u32 c_UpdateLevelResortID[] = {
    RESORT_ID_FARM,
    RESORT_ID_GIM,
    RESORT_ID_HOTSPA,
  };

  for( u32 i=0; i<GFL_NELEMS(c_UpdateLevelResortID); ++i )
  {
    SetResortType( c_UpdateLevelResortID[i] );
  }

  // たんけんも追加する必要あり
  SetupResort_Adventure();
}

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

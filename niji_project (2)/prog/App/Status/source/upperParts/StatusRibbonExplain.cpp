//======================================================================
/**
 * @file    StatusRibbonExplain.cpp
 * @date    2015/12/02 17:07:24
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：リボン説明(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusRibbonExplain.h"

#include <App/Status/source/StatusDrawerBase.h>
#include <App/Status/source/StatusDrawListener.h>

#include <AppLib/include/Ui/UIView.h>
#include <PokeTool/include/PokeToolRibbon.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>
#include <niji_conv_header/message/msg_ribbon.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //!< @brief   リボン名
  static const u32 c_RibbonNameID[] = {
    mes_ribbon_name_001,
    mes_ribbon_name_002,
    mes_ribbon_name_003,
    mes_ribbon_name_004,
    mes_ribbon_name_005,
    mes_ribbon_name_006,
    mes_ribbon_name_007,
    mes_ribbon_name_008,
    mes_ribbon_name_009,
    mes_ribbon_name_010,
    mes_ribbon_name_011,
    mes_ribbon_name_012,
    mes_ribbon_name_013,
    mes_ribbon_name_014,
    mes_ribbon_name_015,
    mes_ribbon_name_016,
    mes_ribbon_name_017,
    mes_ribbon_name_018,
    mes_ribbon_name_019,
    mes_ribbon_name_020,
    mes_ribbon_name_021,
    mes_ribbon_name_022,
    mes_ribbon_name_023,
    mes_ribbon_name_024,
    mes_ribbon_name_025,
    mes_ribbon_name_026,
    mes_ribbon_name_027,
    mes_ribbon_name_028,
    mes_ribbon_name_029,
    mes_ribbon_name_030,
    mes_ribbon_name_031,
    mes_ribbon_name_032,
    mes_ribbon_name_033,
    mes_ribbon_name_034,
    mes_ribbon_name_035,
    mes_ribbon_name_036,
    mes_ribbon_name_037,
    mes_ribbon_name_038,
    mes_ribbon_name_039,
    mes_ribbon_name_040,
    mes_ribbon_name_041,
    mes_ribbon_name_042,
    mes_ribbon_name_043,
    mes_ribbon_name_044,
    mes_ribbon_name_045,
    mes_ribbon_name_046,
    mes_ribbon_name_047,
    mes_ribbon_name_048,
    mes_ribbon_name_049,
    mes_ribbon_name_050,
  };

  //!< @brief   リボン説明
  static const u32 c_RibbonDescID[] = {
    mes_ribbon_info_001,
    mes_ribbon_info_002,
    mes_ribbon_info_003,
    mes_ribbon_info_004,
    mes_ribbon_info_005,
    mes_ribbon_info_006,
    mes_ribbon_info_007,
    mes_ribbon_info_008,
    mes_ribbon_info_009,
    mes_ribbon_info_010,
    mes_ribbon_info_011,
    mes_ribbon_info_012,
    mes_ribbon_info_013,
    mes_ribbon_info_014,
    mes_ribbon_info_015,
    mes_ribbon_info_016,
    mes_ribbon_info_017,
    mes_ribbon_info_018,
    mes_ribbon_info_019,
    mes_ribbon_info_020,
    mes_ribbon_info_021,
    mes_ribbon_info_022,
    mes_ribbon_info_023,
    mes_ribbon_info_024,
    mes_ribbon_info_025,
    mes_ribbon_info_026,
    mes_ribbon_info_027,
    mes_ribbon_info_028,
    mes_ribbon_info_029,
    mes_ribbon_info_030,
    mes_ribbon_info_031,
    mes_ribbon_info_032,
    mes_ribbon_info_033,
    mes_ribbon_info_034,
    mes_ribbon_info_035,
    mes_ribbon_info_036,
    mes_ribbon_info_037,
    mes_ribbon_info_038,
    mes_ribbon_info_039,
    mes_ribbon_info_040,
    mes_ribbon_info_041,
    mes_ribbon_info_042,
    mes_ribbon_info_043,
    mes_ribbon_info_044,
    mes_ribbon_info_045,
    mes_ribbon_info_046,
    mes_ribbon_info_047,
    mes_ribbon_info_048,
    mes_ribbon_info_049,
    mes_ribbon_info_050,
  };
};

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
RibbonExplain::RibbonExplain( DrawerBase* base, gfl2::str::MsgData* msgData )
  : m_pBase(base)
  , m_pMsgData(msgData)
  , m_pPokeParam(NULL)
  , m_RibbonID(0xFFFFFFFF)
  , m_isOpen(false)
  , m_isRibbonNone(false)
  , m_pTextName(NULL)
  , m_pTextDesc1(NULL)
  , m_pTextDesc2(NULL)
  , m_pTextCount(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   listener    リスナー
 */
//------------------------------------------------------------------------------
void RibbonExplain::Initialize( StatusDrawListener* listener )
{
  // リスナーのセット
  m_pListener = listener;

  // ペインのセット
  {
    // リボン名
    m_pTextName  = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTRIBBON_00 );
    // リボン説明１
    m_pTextDesc1 = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTRIBBON_01 );
    // リボン説明２
    m_pTextDesc2 = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTRIBBON_02 );
    // リボン数
    m_pTextCount = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTRIBBON_03 );
    // ヘルプ
    m_pHelpParts = m_pBase->GetParts( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_PARTS_BTNHELP );

    // リボン説明２を非表示
    m_pBase->SetVisiblePane( m_pTextDesc2, false );
  }

  // 初期テキスト
  InitializeText();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void RibbonExplain::InitializeText( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // 個数の単位
  gfl2::lyt::LytTextBox* textbox = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTRIBBON_04 );
  view->SetTextboxPaneMessage( textbox, PARAM_RIBBON_RUMPLING );
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void RibbonExplain::Setup( const pml::pokepara::PokemonParam* pp )
{
  bool fastmode_flag = pp->StartFastMode();
  {
    u32 count = 0;

    // リボン数を調べる
    for( u32 i=0; i<PokeTool::RIBBON_NO_MAX; ++i )
    {
      if( i == PokeTool::RIBBON_NO_LUMPING_CONTEST || i == PokeTool::RIBBON_NO_LUMPING_TOWER )
      {
        u32 num = pp->GetLumpingRibbon( i );

        // リボンを１以上所持していればカウント
        if( num > 0 )
        {
          count++;
        }
      }
      else {
        if( pp->HaveRibbon( i ) )
        {
          // リボンを所持していればカウント
          count++;
        }
      }
    }

    // count == 0ならリボンを身所持のため、説明文は出さない
    m_isRibbonNone = (count == 0);
  }
  pp->EndFastMode( fastmode_flag );

  // ヘルプガイドの表示切替
  m_pBase->SetVisiblePane( m_pHelpParts, !m_isRibbonNone );

  // 通知
  m_pListener->SetRibbonNone( m_isRibbonNone );

  m_pPokeParam = pp;
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンのセットアップ
 *
 * @param   ribbonID    リボンのID
 */
//------------------------------------------------------------------------------
void RibbonExplain::SetupRibbon( u32 ribbonID )
{
  FUKUSHIMA_PRINT( "%s : ribbonID[%d]\n", __FUNCTION__, ribbonID );

  if( m_RibbonID != ribbonID )
  {
    // リボン名のセット
    SetRibbonName( ribbonID );
    // リボン説明のセット
    SetRibbonDesc( ribbonID );
    // リボン数のセット
    SetRibbonCount( ribbonID );

    m_RibbonID = ribbonID;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   開く
 *
 * @param   ribbonID    リボンのID
 *
 * @param   "true  = 開くことに成功"
 * @param   "false = それ以外"
 */
//------------------------------------------------------------------------------
bool RibbonExplain::Open( u32 ribbonID )
{
  // リボン身所持のため開かない
  if( m_isRibbonNone ) return false;

  if( !m_isOpen )
  {
    // リボンのセットアップ
    SetupRibbon( ribbonID );

    // 開く
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_SUB_UPP_000_VISIBLE_01 );
    // 「リボンをみる」を非表示
    m_pBase->SetVisiblePane( m_pHelpParts, false );

    m_isOpen = true;

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void RibbonExplain::Close( void )
{
  if( m_isOpen )
  {
    // 閉じる
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_SUB_UPP_000_VISIBLE_00 );
    // 「リボンをみる」を表示
    m_pBase->SetVisiblePane( m_pHelpParts, true );

    m_isOpen = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   リボン名のセット
 *
 * @param   ribbonID    リボンのID
 */
//------------------------------------------------------------------------------
void RibbonExplain::SetRibbonName( u32 ribbonID )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();

  // リボンメッセージデータをセット
  g2d->SetMessageData( m_pMsgData, m_pBase->GetWordSet() );

  // リボン名のセット
  g2d->SetTextBoxPaneString( m_pTextName, c_RibbonNameID[ribbonID] );

  // メッセージデータを元に戻す
  g2d->SetMessageData( m_pBase->GetMsgData(), m_pBase->GetWordSet() );
}

//------------------------------------------------------------------------------
/**
 * @brief   リボン説明文のセット
 *
 * @param   ribbonID    リボンのID
 */
//------------------------------------------------------------------------------
void RibbonExplain::SetRibbonDesc( u32 ribbonID )
{
  app::util::G2DUtil* g2d = m_pBase->GetG2D();

  // リボンメッセージデータをセット
  g2d->SetMessageData( m_pMsgData, m_pBase->GetWordSet() );

  // リボン名のセット
  g2d->SetTextBoxPaneString( m_pTextDesc1, c_RibbonDescID[ribbonID] );

  // メッセージデータを元に戻す
  g2d->SetMessageData( m_pBase->GetMsgData(), m_pBase->GetWordSet() );
}

//------------------------------------------------------------------------------
/**
 * @brief   リボン所持数のセット
 *
 * @param   ribbonID    リボンのID
 */
//------------------------------------------------------------------------------
void RibbonExplain::SetRibbonCount( u32 ribbonID )
{
  bool visible = ((ribbonID == PokeTool::RIBBON_NO_LUMPING_CONTEST) || (ribbonID == PokeTool::RIBBON_NO_LUMPING_TOWER));

  if( visible )
  {
    u32 count = m_pPokeParam->GetLumpingRibbon( ribbonID );

    // リボン数のセット
    app::ui::UIView* view = m_pBase->GetUIView();
    view->SetTextboxPaneNumber( m_pTextCount, PARAM_RIBBON_NUM, count, 2 );
  }

  // 個数の表示切替
  m_pBase->SetVisiblePane( m_pTextCount, visible );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

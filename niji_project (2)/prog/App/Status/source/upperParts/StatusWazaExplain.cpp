//======================================================================
/**
 * @file    StatusWazaExplain.cpp
 * @date    2015/12/01 17:43:00
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技説明(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWazaExplain.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>
#include <niji_conv_header/message/msg_wazainfo.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
WazaExplain::WazaExplain( DrawerBase* base, gfl2::str::MsgData* msgData )
  : m_pBase(base)
  , m_pMsgData(msgData)
  , m_WazaNo(WAZANO_NULL)
  , m_isOpen(false)
  , m_pPlateParts(NULL)
  , m_pTypeIconParts(NULL)
  , m_pTypeIcon(NULL)
  , m_pTextPower(NULL)
  , m_pTextHit(NULL)
  , m_pTextWazaInfo(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void WazaExplain::Initialize( void )
{
  {
    // 技説明プレート
    m_pPlateParts    = m_pBase->GetParts( PANENAME_FIELDSTATUS_MIN_UPP_000_PANE_PARTS_WAZAPLT );
    // タイプアイコンパーツ
    m_pTypeIconParts = m_pBase->GetParts( PANENAME_STATUS_PLT_UPP_000_PANE_PARTS_SKILLTYPE, m_pPlateParts );
    // タイプアイコン
    m_pTypeIcon      = m_pBase->GetPicture( PANENAME_COMMON_ICN_005_PANE_ICONTYPE_00, m_pTypeIconParts );
    // いりょく
    m_pTextPower     = m_pBase->GetTextBox( PANENAME_STATUS_PLT_UPP_000_PANE_PALAM_03, m_pPlateParts );
    // めいちゅう
    m_pTextHit       = m_pBase->GetTextBox( PANENAME_STATUS_PLT_UPP_000_PANE_PALAM_04, m_pPlateParts );
    // 技説明
    m_pTextWazaInfo  = m_pBase->GetTextBox( PANENAME_STATUS_PLT_UPP_000_PANE_DESC_01, m_pPlateParts );

    m_pBase->SetVisiblePane( m_pPlateParts, true );
  }

  // 初期テキスト
  InitializeText();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void WazaExplain::InitializeText( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  const u32 c_TextBoxIndex[] = {
    PANENAME_STATUS_PLT_UPP_000_PANE_PALAM_00,
    PANENAME_STATUS_PLT_UPP_000_PANE_PALAM_01,
    PANENAME_STATUS_PLT_UPP_000_PANE_PALAM_02,
  };
  const u32 c_TextMsgID[] = {
    PARAM_WAZA_KIND,
    PARAM_WAZA_ATK,
    PARAM_WAZA_HIT,
  };

  for( u32 i=0; i<GFL_NELEMS(c_TextBoxIndex); ++i )
  {
    gfl2::lyt::LytTextBox* textbox = m_pBase->GetTextBox( c_TextBoxIndex[i], m_pPlateParts );
    view->SetTextboxPaneMessage( textbox, c_TextMsgID[i] );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   技のセットアップ
 *
 * @param   wazaNo    技No
 */
//------------------------------------------------------------------------------
void WazaExplain::Setup( u32 wazaNo )
{
  FUKUSHIMA_PRINT( "%s : wazaNo[%d]\n", __FUNCTION__, wazaNo );

  if( m_WazaNo != wazaNo )
  {
    // パラメータの設定
    SetParameter( wazaNo );
    // 技タイプ色の設定
    SetWazaTypeColor( wazaNo );
    // 技のダメージタイプの設定
    SetWazaDamageType( wazaNo );
    // 技の説明文の設定
    SetWazaDesc( wazaNo );

    m_WazaNo = wazaNo;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   開く
 *
 * @param   wazaNo    技No
 *
 * @return  "true  = 開くことに成功"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool WazaExplain::Open( u32 wazaNo )
{
  if( !m_isOpen )
  {
    // セットアップ
    Setup( wazaNo );

    // 開く
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_IN );

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
void WazaExplain::Close( void )
{
  if( m_isOpen )
  {
    // 閉じる
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_OUT );

    m_isOpen = false;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じ終わったか？
 *
 * @return  "true  == 閉じ終わった"
 * @return  "false == 継続"
 */
//------------------------------------------------------------------------------
bool WazaExplain::IsClosed( void ) const
{
  return m_pBase->IsAnimEnd( LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_OUT );
}


//------------------------------------------------------------------------------
/**
 * @brief   パラメータの設定
 * 
 * @param   wazaNo    技ID
 */
//------------------------------------------------------------------------------
void WazaExplain::SetParameter( u32 wazaNo )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  WazaNo waza = static_cast<WazaNo>( wazaNo );

  // 威力
  u16 power = pml::wazadata::GetParam( waza, pml::wazadata::PARAM_ID_POWER );
  if( power > 1 )
  {
    view->SetTextboxPaneNumber( m_pTextPower, PARAM_VALUE_3, power, 3, 0, print::NUM_DISP_SPACE );
  }
  else {
    view->SetTextboxPaneMessage( m_pTextPower, PARAM_NONE );
  }

  // 命中
  if( !pml::wazadata::IsAlwaysHit(waza) )
  {
    u16 hit = pml::wazadata::GetParam( waza, pml::wazadata::PARAM_ID_HITPER );
    view->SetTextboxPaneNumber( m_pTextHit, PARAM_VALUE_3, hit, 3, 0, print::NUM_DISP_SPACE );
  }
  else {
    view->SetTextboxPaneMessage( m_pTextHit, PARAM_NONE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技タイプ色の設定
 * 
 * @param   wazaNo    技ID
 */
//------------------------------------------------------------------------------
void WazaExplain::SetWazaTypeColor( u32 wazaNo )
{
  //! @brief  色変更アニメ
  const u32 c_WazaTypeColorAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_01,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_02,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_03,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_04,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_05,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_06,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_07,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_08,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_09,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_10,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_11,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_12,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_13,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_14,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_15,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_16,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT_COLOR_17,
  };

  WazaNo waza = static_cast<WazaNo>( wazaNo );

  pml::PokeType type = pml::wazadata::GetType( waza );

  m_pBase->StartAnim( c_WazaTypeColorAnim[type] );
}

//------------------------------------------------------------------------------
/**
 * @brief   技のダメージタイプの設定
 * 
 * @param   wazaNo    技ID
 */
//------------------------------------------------------------------------------
void WazaExplain::SetWazaDamageType( u32 wazaNo )
{
  //! @brief  ダメージタイプアニメ
  const u32 c_DamageTypeAnim[] = {
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT__PARTS_SKILLTYPE_TEXTURE_00,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT__PARTS_SKILLTYPE_TEXTURE_01,
    LA_FIELD_STATUS_FIELDSTATUS_MIN_UPP_000__PARTS_WAZAPLT__PARTS_SKILLTYPE_TEXTURE_02,
  };

  WazaNo waza = static_cast<WazaNo>( wazaNo );

  pml::wazadata::WazaDamageType type = pml::wazadata::GetDamageType( waza );
  m_pBase->StartAnim( c_DamageTypeAnim[type] );
}

//------------------------------------------------------------------------------
/**
 * @brief   技の説明文の設定
 * 
 * @param   wazaNo    技ID
 */
//------------------------------------------------------------------------------
void WazaExplain::SetWazaDesc( u32 wazaNo )
{
  app::util::G2DUtil* g2d         = m_pBase->GetG2D();

  // 技説明メッセージデータをセット
  g2d->SetMessageData( m_pMsgData, m_pBase->GetWordSet() );

  // テキストのセット
  g2d->SetTextBoxPaneString( m_pTextWazaInfo, (WAZAINFO_000 + wazaNo) );

  // メッセージデータを元に戻す
  g2d->SetMessageData( m_pBase->GetMsgData(), m_pBase->GetWordSet() );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

//======================================================================
/**
 * @file WazaOshieLowerExplain.cpp
 * @date 2015/07/01 18:59:08
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "WazaOshieLowerExplain.h"
#include "WazaOshieDrawerBase.h"
#include "WazaOshieConst.h"

#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_Common.h>

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/app/common_status/common_status_pane.h>
#include <niji_conv_header/app/common_status/common_status_anim_list.h>
#include <niji_conv_header/message/msg_btl_app.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   定数宣言
//------------------------------------------------------------------------------
static const u32 c_WazaTypeTopAnim = LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000_COLOR_00;

static const u32 c_WazaName[] = {
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PALAM_00,
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PALAM_01,
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PALAM_02,
};

static const u32 c_WazaValue[] = {
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PALAM_03,
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PALAM_04,
};

static const u32 c_ButtonParts[] = {
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_00,
  PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTSYN_01,
};

static const u32 c_SkillTypeAnim[] = {
  LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_00,
  LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_01,
  LA_COMMON_STATUS_COMMONSTATUS_DSC_LOW_000__PARTS_SKILLTYPE_TEXTURE_02,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pBase   描画共通処理
 */
//------------------------------------------------------------------------------
WazaOshieExplain::WazaOshieExplain( DrawerBase* pBase, app::tool::AppCommonGrpIconData* pCmnIcon )
  : m_pBase( pBase )
  , m_pCommonIcon( pCmnIcon )
  , m_pInfoMsg( NULL )
  , m_pStrBuf( NULL )
  , m_WazaNo( WAZANO_NULL )
  , m_pNullAll( NULL )
  , m_pWazaName( NULL )
  , m_pWazaPP( NULL )
  , m_pWazaPPNum( NULL )
  , m_pWazaPPMaxNum( NULL )
  , m_pWazaPPSlash( NULL )
  , m_pWazaDesc( NULL )
{
  Init();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
WazaOshieExplain::~WazaOshieExplain( void )
{
  GFL_SAFE_DELETE( m_pStrBuf );
  GFL_SAFE_DELETE( m_pInfoMsg );
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void WazaOshieExplain::Init( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();
  m_pInfoMsg = GFL_NEW(devHeap) gfl2::str::MsgData(
    print::GetMessageArcId(), GARC_message_wazainfo_DAT, devHeap, gfl2::str::MsgData::LOAD_FULL );

  m_pStrBuf = GFL_NEW(devHeap) gfl2::str::StrBuf( 256, devHeap );


  m_pNullAll = m_pBase->GetPane( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_ALL );

  m_pWazaName = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_TEXTBOX_00 );
  m_pWazaPP = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_TEXTBOX_01 );
  m_pWazaPPNum = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_TEXTBOX_02 );
  m_pWazaPPMaxNum = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_TEXTBOX_03 );
  m_pWazaPPSlash = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_TEXTBOX_04 );
  m_pWazaDesc = m_pBase->GetTextBox( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_DESC_00 );

  gfl2::lyt::LytParts* typeParts = m_pBase->GetParts( PANENAME_COMMONSTATUS_DSC_LOW_000_PANE_PARTS_WAZATYPE );
  m_pWazaTypeIcon = m_pBase->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, typeParts );

  for( u32 i=0; i<WAZA_PARAM_NUM; ++i )
  {
    m_pWazaParamName[i] = m_pBase->GetTextBox( c_WazaName[i] );
  }

  for( u32 i=0; i<WAZA_PARAM_VALUE_NUM; ++i )
  {
    m_pWazaParamNum[i] = m_pBase->GetTextBox( c_WazaValue[i] );
  }

  for( u32 i=0; i<BUTTON_NUM; ++i )
  {
    gfl2::lyt::LytParts* pParts = m_pBase->GetParts( c_ButtonParts[i] );
    m_pButtonText[i] = m_pBase->GetTextBox( PANENAME_COMMON_BTN_LOW_005_PANE_TEXTBOX_00, pParts );
  }

  // 固定テキストのセット
  app::ui::UIView* view = m_pBase->GetUIView();

  view->SetTextboxPaneMessage( m_pWazaPP, STR_PP );
  view->SetTextboxPaneMessage( m_pWazaPPSlash, str_btl_poke_hp_slash );
  view->SetTextboxPaneMessage( m_pWazaParamName[0], STR_WAZA_KIND );
  view->SetTextboxPaneMessage( m_pWazaParamName[1], STR_WAZA_POWER );
  view->SetTextboxPaneMessage( m_pWazaParamName[2], STR_WAZA_HIT );
  view->SetTextboxPaneMessage( m_pButtonText[1], STR_WLST_SELECT_02 );

  SetVisible( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 */
//------------------------------------------------------------------------------
void WazaOshieExplain::Setup( u32 mode, SetupParam& param )
{
  WazaNo wazaNo = param.wazaNo;

  if( m_WazaNo != wazaNo )
  {
    app::ui::UIView* view   = m_pBase->GetUIView();
    print::WordSet* wordSet = m_pBase->GetWordSet();

    // 技の属性
    pml::PokeType type = pml::wazadata::GetType( wazaNo );
    u32 anmID = c_WazaTypeTopAnim + type;
    m_pBase->StartAnim( anmID );

    PokeType_tag typeTag = static_cast<PokeType_tag>( type );
    m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( typeTag, m_pWazaTypeIcon );

    // 技名
    wordSet->RegisterWazaName( 0, wazaNo );
    view->SetTextboxPaneMessage( m_pWazaName, STR_WAZA_NAME );

    // 技の分類
    u32 skillType = pml::wazadata::GetDamageType( wazaNo );
    m_pBase->StartAnim( c_SkillTypeAnim[skillType] );

    // いりょく
    u32 power = pml::wazadata::GetPower( wazaNo );
    if( power > 1 )
    {
      view->SetTextboxPaneNumber( m_pWazaParamNum[0], STR_VALUE_NUM_3, power, 3 );
    }
    else {
      view->SetTextboxPaneMessage( m_pWazaParamNum[0], str_btl_poke_hyphen );
    }

    // 命中
    if( pml::wazadata::IsAlwaysHit( wazaNo ) )  //!< 必中かどうか
    {
      view->SetTextboxPaneMessage( m_pWazaParamNum[1], str_btl_poke_hyphen );
    }
    else {
      u32 hitper = pml::wazadata::GetParam( wazaNo, pml::wazadata::PARAM_ID_HITPER );
      view->SetTextboxPaneNumber( m_pWazaParamNum[1], STR_VALUE_NUM_3, hitper, 3 );
    }

    // PP
    view->SetTextboxPaneNumber( m_pWazaPPNum, STR_VALUE_NUM_3, param.nowPP, 2 );
    view->SetTextboxPaneNumber( m_pWazaPPMaxNum, STR_VALUE_NUM_3, param.maxPP, 2 );

    gfl2::lyt::ColorU8 color = app::util::GetWazaTextColor( param.nowPP, param.maxPP );
    {
      app::util::G2DUtil* g2d = m_pBase->GetG2D();

      g2d->SetTextBoxPaneColor( m_pWazaPP, color, color );          //!< 色替え
      g2d->SetTextBoxPaneColor( m_pWazaPPNum, color, color );       //!< 色替え
      g2d->SetTextBoxPaneColor( m_pWazaPPSlash, color, color );     //!< 色替え
      g2d->SetTextBoxPaneColor( m_pWazaPPMaxNum, color, color );    //!< 色替え
    }

    // 技説明
    m_pInfoMsg->GetString( wazaNo, *m_pStrBuf );
    view->SetTextboxPaneString( m_pWazaDesc, m_pStrBuf );

    if( mode == MODE_WAZA_OBOE )
    {
      view->SetTextboxPaneMessage( m_pButtonText[0], STR_WLST_SELECT_03 );
    }
    else {
      view->SetTextboxPaneMessage( m_pButtonText[0], STR_WLST_SELECT_01 );
    }

    m_WazaNo = wazaNo;
  }

  // 技説明画面を表示
  SetVisible( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   表示切替
 * @param   vis   表示フラグ
 */
//------------------------------------------------------------------------------
void WazaOshieExplain::SetVisible( bool vis )
{
  m_pBase->SetVisiblePane( m_pNullAll, vis );
}

//------------------------------------------------------------------------------
/**
 * @brief   技IDの取得
 * @return  wazaNo
 */
//------------------------------------------------------------------------------
WazaNo WazaOshieExplain::GetWazaNo( void ) const
{
  return m_WazaNo;
}


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

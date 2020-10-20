// ============================================================================
/*
 * @file MiracleTradePlayerInfoLowerView.cpp
 * @brief ミラクル交換の交換相手の情報を表示する画面です。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/Matching/MiracleTradePlayerInfoLowerView.h"
#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "System/include/PokemonLanguage.h"
#include "GameSys/include/GameManager.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/miracletrade/MiracleTrade.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_anim_list.h>
#include <niji_conv_header/app/miracletrade/MiracleTrade_pane.h>
#include <niji_conv_header/message/msg_miracletrade.h>
#include <niji_conv_header/battle/chara/p2_base.h>
#include <niji_conv_header/chara_test/dressup_item_id.h>
#include <niji_conv_header/chara_test/dressup_color_id.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(Matching)


MiracleTradePlayerInfoLowerView::MiracleTradePlayerInfoLowerView( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pMiracleTradeWork,
    NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_LAYOUT,
    LYTRES_MIRACLETRADE_MIRACLETRADE_MIN_LOW_BFLYT,
    LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pMiracleTradeWork->GetMessageUtility(),
    GARC_message_miracletrade_DAT ),
#ifdef GF_PLATFORM_CTR
  m_pSimpleAddressWorkMemory( NULL ),
  m_pCountryNameCache( NULL ),
  m_pRegionNameCache( NULL ),
#endif // GF_PLATFORM_CTR
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_pMiracleTradePlayerInfoLowerViewListener( NULL ),
  m_MessageWindowManipulator()
{
#ifdef GF_PLATFORM_CTR
  gfl2::heap::HeapBase* pDeviceHeap = m_pMiracleTradeWork->GetDeviceHeap();
  m_pSimpleAddressWorkMemory = GFL_NEW_ARRAY( pDeviceHeap ) u8[ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE ];
#endif // GF_PLATFORM_CTR

  setupMessageWindowManipulator();
  setupLayout( LAYOUT_WORK_ID_LOWER, LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW_PATERN_02 );
  setVisibleBackButton( false );

  SetInputListener( this );
}


MiracleTradePlayerInfoLowerView::~MiracleTradePlayerInfoLowerView()
{
#ifdef GF_PLATFORM_CTR
  GFL_SAFE_DELETE( m_pRegionNameCache );
  GFL_SAFE_DELETE( m_pCountryNameCache );
  GFL_SAFE_DELETE_ARRAY( m_pSimpleAddressWorkMemory );
#endif // GF_PLATFORM_CTR
}


void MiracleTradePlayerInfoLowerView::setupMessageWindowManipulator()
{
  NetAppLib::UI::NetAppCommonMessageWindowManipulator::SSetupInfo setupInfo;

  setupInfo.pHeap                = m_pMiracleTradeWork->GetAppHeap();
  setupInfo.pTargetView          = this;
  setupInfo.layoutWorkID         = LAYOUT_WORK_ID_LOWER;
  setupInfo.pHandler             = this;
  setupInfo.animID_TimerIcon     = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_KEEP;
  setupInfo.animID_TimerIcon_In  = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_IN;
  setupInfo.animID_TimerIcon_Out = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__TIMERICON_OUT;
  setupInfo.animID_MsgCursor     = LA_MIRACLETRADE_MIRACLETRADE_MIN_LOW__MESSAGE__CURSOR_00_KEEP;
  setupInfo.paneID_WindowParts   = PANENAME_MIRACLETRADE_MIN_LOW_PANE_MESSAGE;
  setupInfo.paneID_TextBox2      = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
  setupInfo.paneID_NullText1     = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
  setupInfo.paneID_TextBox1a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A;
  setupInfo.paneID_TextBox2a     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B;
  setupInfo.paneID_TimerIcon     = PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON;
  setupInfo.paneID_MsgCursor     = PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00;

  m_MessageWindowManipulator.Setup( setupInfo );

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pMiracleTradeWork->GetMessageUtility();
  m_MessageWindowManipulator.ShowMessage( &pMessageUtility->GetString( GARC_message_miracletrade_DAT, msg_miracletrade_win_05 ), true, false );
}


void MiracleTradePlayerInfoLowerView::SetupTextMessage()
{
#ifdef GF_PLATFORM_CTR

  gfl2::heap::HeapBase* pDeviceHeap = m_pMiracleTradeWork->GetDeviceHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID miracleTradeResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_MIRACLE_TRADE );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  Savedata::MyStatus& tradePersonStatus = m_pMiracleTradeWork->GetTradePersonStatus();

  // コメント
  {
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pMiracleTradeWork->GetMessageUtility();

    gfl2::lyt::LytTextBox* pTextComment_00 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTCOMMENT_00 );
    gfl2::lyt::LytTextBox* pTextComment_01 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTCOMMENT_01 );

    pG2DUtil->SetTextBoxPaneString( pTextComment_00, pMessageUtility->GetString( GARC_message_jf_phrase_DAT, m_pMiracleTradeWork->GetHelloPhraseMessageID() ).GetPtr() );
    pG2DUtil->SetTextBoxPaneString( pTextComment_01, pMessageUtility->GetString( GARC_message_jf_phrase_DAT, m_pMiracleTradeWork->GetEmotionPhraseMessageID() ).GetPtr() );
  }

  // ポケモントレーナーの名前
  {
    tradePersonStatus.GetNameString( m_TradePersonName );
    gfl2::lyt::LytTextBox* pTextPlayer_00 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTPLAYER_00 );
    pG2DUtil->SetTextBoxPaneString( pTextPlayer_00, m_TradePersonName );
  }

  // ポケモントレーナーのアイコン
  {
    gfl2::lyt::LytPicture* pPicturePane     = pLower->GetPicturePane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_PLAYERPHOTO );
    gfl2::lyt::LytPane*    pWindowFramePane = pLower->GetPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_WINDOW_PLAYER_FRAME );

    if( pPicturePane )
    {
      NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility = m_pMiracleTradeWork->GetTrainerIconUtility();

      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( tradePersonStatus.GetIcon() );
      
      iconObjectData.pReplacePane             = pPicturePane;
      iconObjectData.pSyncVisiblePane         = NULL;
      iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;

      pTrainerIconUtility->SetIconObjectData( 0, iconObjectData );
    }
  }

  u8 u8PokeLanguageID = System::GetLang();              // 自分の言語コードで

  if( u8PokeLanguageID == POKEMON_LANG_CHINA
   || u8PokeLanguageID == POKEMON_LANG_TAIWAN )
  {
    u8PokeLanguageID = POKEMON_LANG_ENGLISH; // @fix NMCat[4117] 繁体字、簡体字だったら英語に置き換える
  }

  u8 u8CountryCode = tradePersonStatus.GetCountry();    // 相手の国名と
  u8 u8RegionCode = tradePersonStatus.GetArea();        // 相手の地域名を 表示する
  nn::cfg::CTR::CfgLanguageCode cfgLanguageCode = static_cast<nn::cfg::CTR::CfgLanguageCode>( System::Language::ConvertPM2CTR( u8PokeLanguageID ) );
  nn::cfg::CTR::CfgCountryCode cfgCountryCode = static_cast<nn::cfg::CTR::CfgCountryCode>( u8CountryCode );

  // 国
  {
    m_pCountryNameCache = GFL_NEW( pDeviceHeap ) System::CTR::CountryNameCache();
    m_pCountryNameCache->LoadCountryName( pDeviceHeap, cfgLanguageCode );
    gfl2::lyt::LytTextBox* pTextPlayer_01 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTPLAYER_01 );
    pG2DUtil->SetTextBoxPaneString( pTextPlayer_01, m_pCountryNameCache->GetCountryName( cfgCountryCode ) );
  }
  
  // 地域
  {
    nn::cfg::CTR::SimpleAddressId simpleAddressId;
    simpleAddressId.id = ( ( u8CountryCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_COUNTRY_SHIFT ) |
                           ( u8RegionCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_REGION_SHIFT ) );
    m_pRegionNameCache = GFL_NEW( pDeviceHeap ) System::CTR::RegionNameCache( m_pCountryNameCache->GetUnkownCountryName(), m_pSimpleAddressWorkMemory );
    m_pRegionNameCache->LoadRegionName( pDeviceHeap, cfgLanguageCode, cfgCountryCode );
    gfl2::lyt::LytTextBox* pTextPlayer_02 = pLower->GetTextBoxPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_TEXTPLAYER_02 );
    pG2DUtil->SetTextBoxPaneString( pTextPlayer_02, m_pRegionNameCache->GetRegionName( simpleAddressId ) );
  }
  
#endif // GF_PLATFORM_CTR
}


void MiracleTradePlayerInfoLowerView::setVisibleBackButton( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytParts* pButtonBack = pLower->GetPartsPane( PANENAME_MIRACLETRADE_MIN_LOW_PANE_BUTTON_BACK );
  pButtonBack->SetVisible( bVisible );
}


void MiracleTradePlayerInfoLowerView::Update( void )
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MessageWindowManipulator.UpdateWindow();
}


void MiracleTradePlayerInfoLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pMiracleTradeWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
}


::app::ui::UIInputListener::ListenerResult MiracleTradePlayerInfoLowerView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( isTouch )
  {
    if( m_pMiracleTradePlayerInfoLowerViewListener )
    {
      m_pMiracleTradePlayerInfoLowerViewListener->OnTouchScreen();
    }
    result = DISABLE_ACCESS;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult MiracleTradePlayerInfoLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( m_pMiracleTradePlayerInfoLowerViewListener )
    {
      m_pMiracleTradePlayerInfoLowerViewListener->OnTouchScreen();
    }
    result = DISABLE_ACCESS;
  }

  return result;
}


void MiracleTradePlayerInfoLowerView::CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID event )
{
}


GFL_NAMESPACE_END(Matching)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

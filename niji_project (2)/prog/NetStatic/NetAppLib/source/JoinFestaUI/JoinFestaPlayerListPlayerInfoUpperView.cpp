// ============================================================================
/*
 * @file JoinFestaPlayerListPlayerInfoUpperView.cpp
 * @brief プレイヤーリストアプリの下画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>

#include <niji_conv_header/battle/chara/p2_base.h>
#include <niji_conv_header/chara_test/dressup_item_id.h>
#include <niji_conv_header/chara_test/dressup_color_id.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

JoinFestaPlayerListPlayerInfoUpperView::JoinFestaPlayerListPlayerInfoUpperView(
  NetAppLib::System::ApplicationWorkBase*       pWorkBase,
  NetAppLib::System::ResourceID                 layoutResourceID,
  NetAppLib::Util::NetAppTrainerIconUtility&    netAppTrainerIconUtility ) :
  NetAppLib::System::NetApplicationViewBase(
    pWorkBase,
    layoutResourceID,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pWorkBase->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT ),
  m_pWorkBase( pWorkBase ),
  m_NullString( 4, pWorkBase->GetDeviceHeap() ),
  m_NetAppTrainerIconUtility( netAppTrainerIconUtility ),
  m_CurrentJoinFestaFriendKey(),
  m_bDrawFlag( false )
{
}


JoinFestaPlayerListPlayerInfoUpperView::~JoinFestaPlayerListPlayerInfoUpperView()
{
}


void JoinFestaPlayerListPlayerInfoUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  gfl2::lyt::LytPicture* pVisibleSyncPane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_FESTA_WIN_01 );

  if( m_bDrawFlag )
  {
    app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  }

  if( pVisibleSyncPane )
  {
    pVisibleSyncPane->SetVisible( m_bDrawFlag );
  }
}


void JoinFestaPlayerListPlayerInfoUpperView::TimerIconDisable()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  gfl2::lyt::LytPicture* pVisibleSyncPane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_FESTA_WIN_01 );

  if( pVisibleSyncPane )
  {
    pVisibleSyncPane->SetVisible( false );
  }
}


void JoinFestaPlayerListPlayerInfoUpperView::SetJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  m_bDrawFlag = ( pData ) ? true : false;

  if( pData )
  {
    if( pData->IsBlack() )
    {
      setJoinFestaBlackPlayerData( pData );
    }
    else
    {
      setJoinFestaPlayerData( pData );
    }

    // ウィンドウの色変更
    {
      u32 animeID = LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000_PATERN_00;

      if( pData->IsGuest() )
      {
        animeID = LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000_PATERN_00;
      }
      else
      if( pData->IsVip() )
      {
        animeID = LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000_PATERN_01;
      }
      else
      if( pData->IsBlack() )
      {
        animeID = LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_000_PATERN_02;
      }

      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, animeID );
    }
  }

}


void JoinFestaPlayerListPlayerInfoUpperView::ResetCurrentJoinFestaFriendKey()
{
  m_CurrentJoinFestaFriendKey.initialize();
}


void JoinFestaPlayerListPlayerInfoUpperView::setJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  // トレーナーアイコン
  {
    JoinFestaFriendKey joinFestaFriendKey = pData->GetJoinFestaFriendKey();

    if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( joinFestaFriendKey, m_CurrentJoinFestaFriendKey ) )
    {
      gfl2::lyt::LytPicture* pPicturePane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_PLAYERPHOTO_00 );
      gfl2::lyt::LytPicture* pVisibleSyncPane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_FESTA_WIN_01 );
      gfl2::lyt::LytPicture* pWindowFramePane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_WINDOW_PLAYER_FRAME );

      if( pPicturePane )
      {
        const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pData->GetPacketPersonalInfo();

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( packetPersonalInfo.icon );

        iconObjectData.pReplacePane             = pPicturePane;
        iconObjectData.pSyncVisiblePane         = pVisibleSyncPane;
        iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_NetAppTrainerIconUtility.SetIconObjectData( 0, iconObjectData );

        m_CurrentJoinFestaFriendKey = joinFestaFriendKey;
      }
    }
  }

  // 言語アイコン
  {
    gfl2::lyt::LytParts* pLanguageIconParts = pUpper->GetPartsPane( PANENAME_JFPL_WIN_UPP_000_PANE_PARTS_00 );

    if( pLanguageIconParts )
    {
      gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( 0 );

      gfl2::lyt::LytPicture* pLanguageIcon = pUpper->GetPicturePane( pLanguageIconParts, PANENAME_COMMON_ICN_008_PANE_BG_00, &resourceID );

      if( pLanguageIcon )
      {
        const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pData->GetPacketPersonalInfo();
      
        app::tool::AppCommonGrpIconData* pAppCommonGrpIconData = m_pWorkBase->GetAppCommonGrpIconData();
      
        if( pAppCommonGrpIconData )
        {
          pAppCommonGrpIconData->ReplacePaneTextureByLangIcon( packetPersonalInfo.languageId, pLanguageIcon );
        }
      }
    }
  }

  // プレイヤー名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_PLAYER_NAME );
    pG2DUtil->SetTextBoxPaneString( pText, pData->GetName() );
  }

  // フレンドか
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_FRIEND_NAME_00 );

    if( pData->IsCtrFriend() )
    {
      pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_02 ) );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
    }
  }


#if defined(GF_PLATFORM_CTR)

  // 国名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_COUNTRY_NAME_00 );

    nn::cfg::SimpleAddressId simpleAddressID = pData->GetSimpleAddressId();

    app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility = m_pWorkBase->GetAppCountryRegionNameUtility();
    if( pAppCountryRegionNameUtility )
    {
      pG2DUtil->SetTextBoxPaneString( pText, pAppCountryRegionNameUtility->GetCountryName( simpleAddressID ) );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
    }
  }

  // 地域名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_AREA_NAME_00 );

    nn::cfg::SimpleAddressId simpleAddressID = pData->GetSimpleAddressId();

    app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility = m_pWorkBase->GetAppCountryRegionNameUtility();
    if( pAppCountryRegionNameUtility )
    {
      pG2DUtil->SetTextBoxPaneString( pText, pAppCountryRegionNameUtility->GetRegionName( simpleAddressID ) );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
    }
  }

#else

  // 国名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_COUNTRY_NAME_00 );
    pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
  }

  // 地域名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_AREA_NAME_00 );
    pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
  }

#endif // GF_PLATFORM_CTR
  
  // 各種情報メッセージ
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData coreData;
    pData->GetCoreData( &coreData );

    typedef struct
    {
      gfl2::lyt::LytTextBox* pText;
    } TextBox;

    TextBox textTable[7];

    textTable[0].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_00 );
    textTable[1].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_01 );
    textTable[2].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_02 );
    textTable[3].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_03 );
    textTable[4].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_04 );
    textTable[5].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_05 );
    textTable[6].pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_06 );

    for( u32 i = 0; i < 7; ++i )
    {
      textTable[i].pText->SetVisible( false );
    }

    u32 index = 0;

    {
      pG2DUtil->SetRegisterNumber( 0, coreData.m_date.year, 2, print::NUM_DISP_ZERO );
      pG2DUtil->SetRegisterNumber( 1, coreData.m_date.month, 2 );
      pG2DUtil->SetRegisterNumber( 2, coreData.m_date.day, 2 );
      pG2DUtil->SetTextBoxPaneStringExpand( textTable[ index ].pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_05 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    {
      u32 messageID = jf_playerlist_cap_66;
      switch( pData->GetPlaySituation() )
      {
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_NORAML:    messageID = jf_playerlist_cap_66; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_SHOP:      messageID = jf_playerlist_cap_64; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_JOIN_FESTA_PERSONAL:  messageID = jf_playerlist_cap_65; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_FIELD:                messageID = jf_playerlist_cap_74; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SINGLE:        messageID = jf_playerlist_cap_09; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_DOUBLE:        messageID = jf_playerlist_cap_10; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_MULTI:         messageID = jf_playerlist_cap_11; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_ROYAL:         messageID = jf_playerlist_cap_62; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_TRADE:                messageID = jf_playerlist_cap_08; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_GTS:                  messageID = jf_playerlist_cap_70; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_MIRACLE_TRADE:        messageID = jf_playerlist_cap_71; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_BATTLE_SPOT:          messageID = jf_playerlist_cap_72; break;
        case NetAppLib::JoinFesta::JoinFestaDefine::E_PLAY_SITUATION_ATTRACTION:           messageID = jf_playerlist_cap_73; break;
      }
      pG2DUtil->SetTextBoxPaneString( textTable[ index ].pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, messageID ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    const NetAppLib::JoinFesta::PacketPersonalInfo& packetPersonalInfo = pData->GetPacketPersonalInfo();

    if( packetPersonalInfo.isAlwaysBattleOk )
    {
      pG2DUtil->SetTextBoxPaneString( textTable[ index ].pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_67 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    if( packetPersonalInfo.isAlwaysTradeOk )
    {
      pG2DUtil->SetTextBoxPaneString( textTable[ index ].pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_68 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    if( packetPersonalInfo.isAlwaysAttractionOk )
    {
      pG2DUtil->SetTextBoxPaneString( textTable[ index ].pText, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_69 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    if( pData->IsVip() )
    {
      pG2DUtil->SetRegisterNumber( 0, pData->GetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TOTAL ), 2 );
      pG2DUtil->SetTextBoxPaneStringExpand( textTable[ index ].pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_06 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }
    else
    if( pData->IsGuest() )
    {
      pG2DUtil->SetRegisterNumber( 0, pData->GetRank( NetAppLib::JoinFestaUI::JoinFestaPlayerData::RECORD_CATEGORY_TOTAL ), 2 );
      pG2DUtil->SetTextBoxPaneStringExpand( textTable[ index ].pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_07 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

    NetAppLib::JoinFesta::PacketGameStatus1 packetGameStatus1;
    bool bResult = pData->GetPacketGameStatus1( &packetGameStatus1 );
    if( bResult )
    {
      pG2DUtil->SetRegisterNumber( 0, packetGameStatus1.jfRank, 3 );
      pG2DUtil->SetTextBoxPaneStringExpand( textTable[ index ].pText, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_76 ) );
      textTable[ index ].pText->SetVisible( true );
      ++index;
    }

  }
}


void JoinFestaPlayerListPlayerInfoUpperView::setJoinFestaBlackPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  // トレーナーアイコン
  {
    JoinFestaFriendKey joinFestaFriendKey = pData->GetBlackPlayerCoreData().GetJoinFestaFriendKey();

    if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( joinFestaFriendKey, m_CurrentJoinFestaFriendKey ) )
    {
      gfl2::lyt::LytPicture* pPicturePane  = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_PLAYERPHOTO_00 );
      gfl2::lyt::LytPicture* pVisibleSyncPane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_FESTA_WIN_01 );
      gfl2::lyt::LytPicture* pWindowFramePane = pUpper->GetPicturePane( PANENAME_JFPL_WIN_UPP_000_PANE_WINDOW_PLAYER_FRAME );

      if( pPicturePane )
      {
        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( pData->GetBlackPlayerCoreData().icon );

        iconObjectData.pReplacePane             = pPicturePane;
        iconObjectData.pSyncVisiblePane         = pVisibleSyncPane;
        iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_NetAppTrainerIconUtility.SetIconObjectData( 0, iconObjectData );

        m_CurrentJoinFestaFriendKey = joinFestaFriendKey;
      }
    }
  }

  /**
    MMcat[271]   【フェスサークル】おことわりリストにいるプレイヤーの使用言語が、直前に表示していたプレイヤーデータの使用言語と同じものになる
      blackListに言語アイコンの更新処理がなかったため追加
  */
  // 言語アイコン
  {
    gfl2::lyt::LytParts* pLanguageIconParts = pUpper->GetPartsPane( PANENAME_JFPL_WIN_UPP_000_PANE_PARTS_00 );

    if( pLanguageIconParts )
    {
      gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( 0 );

      gfl2::lyt::LytPicture* pLanguageIcon = pUpper->GetPicturePane( pLanguageIconParts, PANENAME_COMMON_ICN_008_PANE_BG_00, &resourceID );

      if( pLanguageIcon )
      {
        app::tool::AppCommonGrpIconData* pAppCommonGrpIconData = m_pWorkBase->GetAppCommonGrpIconData();
      
        if( pAppCommonGrpIconData )
        {
          pAppCommonGrpIconData->ReplacePaneTextureByLangIcon( pData->GetBlackPlayerCoreData().languageId, pLanguageIcon );
        }
      }
    }
  }

  // プレイヤー名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_PLAYER_NAME );
    pG2DUtil->SetTextBoxPaneString( pText, pData->GetBlackPlayerCoreData().name );
  }

  // フレンドか
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_FRIEND_NAME_00 );

    pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
  }


#if defined(GF_PLATFORM_CTR)

  // 国名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_COUNTRY_NAME_00 );

    nn::cfg::SimpleAddressId simpleAddressID;
    simpleAddressID.id = pData->GetBlackPlayerCoreData().addressId;

    app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility = m_pWorkBase->GetAppCountryRegionNameUtility();
    if( pAppCountryRegionNameUtility )
    {
      pG2DUtil->SetTextBoxPaneString( pText, pAppCountryRegionNameUtility->GetCountryName( simpleAddressID ) );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
    }
  }

  // 地域名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_AREA_NAME_00 );

    nn::cfg::SimpleAddressId simpleAddressID;
    simpleAddressID.id = pData->GetBlackPlayerCoreData().addressId;

    app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility = m_pWorkBase->GetAppCountryRegionNameUtility();
    if( pAppCountryRegionNameUtility )
    {
      pG2DUtil->SetTextBoxPaneString( pText, pAppCountryRegionNameUtility->GetRegionName( simpleAddressID ) );
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
    }
  }

#else

  // 国名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_COUNTRY_NAME_00 );
    pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
  }

  // 地域名
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_AREA_NAME_00 );
    pG2DUtil->SetTextBoxPaneString( pText, &m_NullString );
  }

#endif // GF_PLATFORM_CTR
  
  // 各種情報メッセージ
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_00 );
    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_01 );
    gfl2::lyt::LytTextBox* pText02 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_02 );
    gfl2::lyt::LytTextBox* pText03 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_03 );
    gfl2::lyt::LytTextBox* pText04 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_04 );
    gfl2::lyt::LytTextBox* pText05 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_05 );
    gfl2::lyt::LytTextBox* pText06 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_000_PANE_INFORMATION_06 );

    pText00->SetVisible( false );
    pText01->SetVisible( false );
    pText02->SetVisible( false );
    pText03->SetVisible( false );
    pText04->SetVisible( false );
    pText05->SetVisible( false );
    pText06->SetVisible( false );
  }
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

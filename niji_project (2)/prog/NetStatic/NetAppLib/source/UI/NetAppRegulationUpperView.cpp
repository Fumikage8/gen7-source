// ============================================================================
/*
 * @file NetAppRegulationUpperView.cpp
 * @brief 上画面にレギュレーションの情報を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationUpperView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/app/regulation/RegulationSelect.h>
#include <niji_conv_header/app/regulation/RegulationSelect_anim_list.h>
#include <niji_conv_header/app/regulation/RegulationSelect_pane.h>
#include <niji_conv_header/message/msg_regulation.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)

NetAppRegulationUpperView::NetAppRegulationUpperView(
  NetAppLib::System::ApplicationWorkBase* pWork,
  NetAppLib::System::ResourceID resourceID,
  NetAppLib::Message::MessageUtility* pMessageUtility ) :
  NetAppLib::System::NetApplicationViewBase(
    pWork,
    resourceID,
    LYTRES_REGULATIONSELECT_REGULATION_MIN_UPP_000_BFLYT,
    LA_REGULATIONSELECT_REGULATION_MIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pMessageUtility,
    GARC_message_regulation_DAT ),
  m_pAppHeap( pWork->GetAppHeap() ),
  m_pAppRenderingManager( pWork->GetAppRenderingManager() ),
  m_pMessageUtility( pMessageUtility ),
  m_pRegulationDrawInfo( NULL )
{
  UIView::GetG2DUtil()->StartAnime( 0, LA_REGULATIONSELECT_REGULATION_MIN_UPP_000__BG_00_BG_LOOP_00 );
}

NetAppRegulationUpperView::NetAppRegulationUpperView(
  app::util::Heap* pAppHeap,
  app::util::AppRenderingManager* pAppRenderingManager,
  void* pLayoutBuffer ) :
  NetAppLib::System::NetApplicationViewBase( pAppHeap ),
  m_pAppHeap( pAppHeap ),
  m_pAppRenderingManager( pAppRenderingManager ),
  m_pMessageUtility( NULL ),
  m_pRegulationDrawInfo( NULL )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( pAppHeap, false );

  m_LayoutResourceData.buf              = pLayoutBuffer;
  m_LayoutResourceData.addTexNum        = 0;
  m_LayoutResourceData.addRegisterFlag  = app::util::G2DUtil::ATTACH_NONE;
  
  m_LayoutWorkData.res_index            = 0;
  m_LayoutWorkData.lytres_id            = LYTRES_REGULATIONSELECT_REGULATION_MIN_UPP_000_BFLYT;
  m_LayoutWorkData.anm_max              = LA_REGULATIONSELECT_REGULATION_MIN_UPP_000___NUM;
  m_LayoutWorkData.buf                  = pLayoutBuffer;
  m_LayoutWorkData.calculateMtxFlag     = true;
  m_LayoutWorkData.setupDisp            = app::util::G2DUtil::SETUP_UPPER;
  m_LayoutWorkData.enable               = true;

  Create2D( pAppHeap, NULL, 1,
    &m_LayoutResourceData, 1,
    &m_LayoutWorkData, 1,
    GARC_message_regulation_DAT );

  UIView::GetG2DUtil()->StartAnime( 0, LA_REGULATIONSELECT_REGULATION_MIN_UPP_000__BG_00_BG_LOOP_00 );

}

NetAppRegulationUpperView::~NetAppRegulationUpperView()
{
}


void NetAppRegulationUpperView::ResetRegulationDrawInfo()
{
  m_pRegulationDrawInfo = NULL;
}


void NetAppRegulationUpperView::SetRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, NetApp::RegulationSelect::BootMode eBootMode )
{
  bool bChangeRegulation = false;

  if( pRegulationDrawInfo )
  {
    if( m_pRegulationDrawInfo )
    {
      // @fix NMCat[4266]配信レギュレーションで大会IDが同じデータが、ローカル側で別なデータとして扱われてしまう　の修正
      // レギュレーションの一致チェックは、crcではなく、大会IDとバージョンで比較する。
      // 
      // 対応メモ：プリセットには大会IDが入っていないので今まで通りcrcで見る必要がある。
      // 　　　　　比較パターンは下記３パターン
      //            １．プリセットと配信　→　無条件で別のレギュレーションと判断
      //            ２．プリセットとプリセット　→　今まで通りcrcで判定
      //            ３．配信と配信　→　大会IDとバージョンで判定

      // ２．プリセットとプリセット　→　今まで通りcrcで判定
      if( pRegulationDrawInfo->IsPreset() == true && m_pRegulationDrawInfo->IsPreset() == true )
      {
        if( pRegulationDrawInfo->GetRegulation().GetCrc() != m_pRegulationDrawInfo->GetRegulation().GetCrc() )
        {
          bChangeRegulation = true;
        }

      }
      // ３．配信と配信　→　大会IDとバージョンで判定
      else if( pRegulationDrawInfo->IsPreset() == false && m_pRegulationDrawInfo->IsPreset() == false )
      {
        const bool isSameDiscriminantId = (pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) == m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::DISCRIMINANT_ID) ) ? true : false;
        const bool isSameVersion = (pRegulationDrawInfo->GetRegulation().GetVersion() == m_pRegulationDrawInfo->GetRegulation().GetVersion() ) ? true : false;
        // 完全一致以外は別のレギュレーション
        if( !( isSameDiscriminantId == true && isSameVersion == true ) )
        {
          bChangeRegulation = true;
        }
      }
      // １．プリセットと配信　→　無条件で別のレギュレーションと判断
      else
      {
        bChangeRegulation = true;
      }
      
    }
    else
    {
      bChangeRegulation = true;
    }
  }

  if( bChangeRegulation )
  {
    m_pRegulationDrawInfo = pRegulationDrawInfo;


    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
    gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_REGULATION );

    gfl2::lyt::LytTextBox* pBattleType = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXT_BATTLETYPE );
    gfl2::lyt::LytTextBox* pBattleRule = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXT_BATTLERULE );

    gfl2::lyt::LytTextBox* pTextHead00 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTHEAD_00 );
    gfl2::lyt::LytTextBox* pTextHead01 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTHEAD_01 );
    gfl2::lyt::LytTextBox* pTextHead02 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTHEAD_02 );
    gfl2::lyt::LytTextBox* pTextHead03 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTHEAD_03 );
    gfl2::lyt::LytTextBox* pTextHead04 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTHEAD_04 );
    gfl2::lyt::LytTextBox* pTextContent00 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTCONTENT_00 );
    gfl2::lyt::LytTextBox* pTextContent01 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTCONTENT_01 );
    gfl2::lyt::LytTextBox* pTextContent02 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTCONTENT_02 );
    gfl2::lyt::LytTextBox* pTextContent03 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTCONTENT_03 );
    gfl2::lyt::LytTextBox* pTextContent04 = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXTCONTENT_04 );

    gfl2::lyt::LytTextBox* pTextDescription = pUpper->GetTextBoxPane( PANENAME_REGULATION_MIN_UPP_000_PANE_TEXT_DESCRIPTION );

    pG2DUtil->SetTextBoxPaneString( pTextHead00, getMessage( msg_reg_cap_11 ) );
    pG2DUtil->SetTextBoxPaneString( pTextHead01, getMessage( msg_reg_cap_12 ) );
    pG2DUtil->SetTextBoxPaneString( pTextHead02, getMessage( msg_reg_cap_13 ) );
    pG2DUtil->SetTextBoxPaneString( pTextHead03, getMessage( msg_reg_cap_14 ) );
    pG2DUtil->SetTextBoxPaneString( pTextHead04, getMessage( msg_reg_cap_15 ) );

    // ---------------------------------------------------------------------------
    // タイトル系
    // プリセットのレギュレーション
    if( pRegulationDrawInfo->IsPreset() )
    {
      u32 messageID1 = msg_reg_cap_16;
      u32 messageID2 = msg_reg_cap_20;

      Regulation::BATTLE_CATEGORY eBattleCategory = pRegulationDrawInfo->GetRegulation().GetBattleCategory();
      Regulation::BATTLE_RULE eBattleRule = pRegulationDrawInfo->GetRegulation().GetBattleRule();

      switch( eBattleCategory )
      {
        case Regulation::BATTLE_CATEGORY_NET_BTL:
        {
          messageID1 = pRegulationDrawInfo->GetBattleSystemMessageID();
          messageID2 = pRegulationDrawInfo->GetBattleRuleMessageID();
        }
        break;

        case Regulation::BATTLE_CATEGORY_TREE_NORMAL:
        {
          messageID1 = msg_reg_cap_23;

          switch( eBattleRule )
          {
            case Regulation::BATTLE_RULE_SINGLE: messageID2 = msg_reg_cap_29; break;
            case Regulation::BATTLE_RULE_DOUBLE: messageID2 = msg_reg_cap_30; break;
            case Regulation::BATTLE_RULE_MULTI:  messageID2 = msg_reg_cap_31; break;
          }
        }
        break;

        case Regulation::BATTLE_CATEGORY_TREE_SUPER:
        {
          messageID1 = msg_reg_cap_23;

          switch( eBattleRule )
          {
            case Regulation::BATTLE_RULE_SINGLE: messageID2 = msg_reg_cap_32; break;
            case Regulation::BATTLE_RULE_DOUBLE: messageID2 = msg_reg_cap_33; break;
            case Regulation::BATTLE_RULE_MULTI:  messageID2 = msg_reg_cap_34; break;
          }

        }
        break;

        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL:
        {
          messageID1 = msg_reg_cap_24;
          messageID2 = msg_reg_cap_35;
        }
        break;

        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER:
        {
          messageID1 = msg_reg_cap_24;
          messageID2 = msg_reg_cap_36;
        }
        break;

        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER:
        {
          messageID1 = msg_reg_cap_24;
          messageID2 = msg_reg_cap_37;
        }
        break;

        case Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER:
        {
          messageID1 = msg_reg_cap_24;
          messageID2 = msg_reg_cap_38;
        }
        break;
      }

      pG2DUtil->SetTextBoxPaneString( pBattleType, getMessage( messageID1 ) );
      pG2DUtil->SetTextBoxPaneString( pBattleRule, getMessage( messageID2 ) );

    }
    // 配信レギュレーション
    else
    {
      pG2DUtil->SetTextBoxPaneString( pBattleType, getMessage( pRegulationDrawInfo->GetBattleSystemMessageID() ) );

      gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
      pRegulationDrawInfo->GetRegulation().GetTextParam( Regulation::DELIVERY_NAME, pTempStrBuf );
      pG2DUtil->SetTextBoxPaneString( pBattleRule, pTempStrBuf );
    }

    // ---------------------------------------------------------------------------
    // ポケモンの数
    u32 selectNumLo = pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::SELECT_NUM_LO );
    u32 selectNumHi = pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::SELECT_NUM_HI );
    if( selectNumLo == selectNumHi )
    {
      pG2DUtil->SetRegisterNumber( 0, selectNumLo, 1 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextContent00, msg_reg_cap_02 );
    }
    else
    {
      pG2DUtil->SetRegisterNumber( 0, selectNumLo, 1 );
      pG2DUtil->SetRegisterNumber( 1, selectNumHi, 1 );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextContent00, msg_reg_cap_04 );
    }

    // ---------------------------------------------------------------------------
    // ポケモンのレベル
    u32 level = pRegulationDrawInfo->GetRegulation().GetValueParam( Regulation::LEVEL );
    Regulation::LEVEL_RANGE_TYPE levelRange = pRegulationDrawInfo->GetRegulation().GetLevelRangeType();
    u32 levelRangeMessageID = msg_reg_cap_01; // 制限無し
    switch( levelRange )
    {
      case Regulation::LEVEL_RANGE_NORMAL:    levelRangeMessageID = msg_reg_cap_01; break; // 制限無し
      case Regulation::LEVEL_RANGE_OVER:      levelRangeMessageID = msg_reg_cap_03; break; // 以上
      case Regulation::LEVEL_RANGE_LESS:      levelRangeMessageID = msg_reg_cap_27; break; // 以下
      case Regulation::LEVEL_RANGE_DRAG_DOWN: levelRangeMessageID = msg_reg_cap_06; break; // 以上補正
      case Regulation::LEVEL_RANGE_SAME:      levelRangeMessageID = msg_reg_cap_05; break; // 全補正
      case Regulation::LEVEL_RANGE_PULL_UP:   levelRangeMessageID = msg_reg_cap_28; break; // 以下補正
    }
    pG2DUtil->SetRegisterNumber( 0, level, 3 );
    pG2DUtil->SetTextBoxPaneStringExpand( pTextContent01, levelRangeMessageID );

    // ---------------------------------------------------------------------------
    // 特別なポケモン
    // @fix NMCat[3141]：とくべつなさんかポケモンの表示用関数
    bool isLegendEntryView = pRegulationDrawInfo->GetRegulation().IsLegendEntryView();
    u32 legendEntryMessageID = ( isLegendEntryView ) ? msg_reg_cap_07 : msg_reg_cap_08;
    pG2DUtil->SetTextBoxPaneStringExpand( pTextContent02, legendEntryMessageID );

    // ---------------------------------------------------------------------------
    // 同じポケモン
    bool bBothPoke = pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::BOTH_POKE );
    u32 bothPokeMessageID = ( bBothPoke ) ? msg_reg_cap_08 : msg_reg_cap_07;
    pG2DUtil->SetTextBoxPaneStringExpand( pTextContent03, bothPokeMessageID );

    // ---------------------------------------------------------------------------
    // 同じ道具
    bool bBothItem = pRegulationDrawInfo->GetRegulation().GetBoolParam( Regulation::BOTH_ITEM );
    u32 bothItemMessageID = ( bBothItem ) ? msg_reg_cap_10 : msg_reg_cap_09;
    pG2DUtil->SetTextBoxPaneStringExpand( pTextContent04, bothItemMessageID );

    // ---------------------------------------------------------------------------
    // レギュレーションの説明
    // プリセットのレギュレーションの場合
    if( pRegulationDrawInfo->IsPreset() )
    {
      Regulation::BATTLE_CATEGORY eBattleCategory = pRegulationDrawInfo->GetRegulation().GetBattleCategory();

      switch( eBattleCategory )
      {
        case Regulation::BATTLE_CATEGORY_NET_BTL:
        {
          pG2DUtil->SetTextBoxPaneString( pTextDescription, getMessage( pRegulationDrawInfo->GetP2PDescriptionMessageID() ) );
        }
        break;

        default:
        {
          pG2DUtil->SetTextBoxPaneString( pTextDescription, getMessage( pRegulationDrawInfo->GetDescriptionMessageID() ) );
        }
        break;
      }
    }
    // 配信レギュレーションの場合
    else
    {
      gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
      pRegulationDrawInfo->GetRegulation().GetTextParam( Regulation::DELIVERY_TEXT, pTempStrBuf );
    
      pG2DUtil->SetTextBoxPaneString( pTextDescription, pTempStrBuf );
    }

  }
}


gfl2::str::StrBuf* NetAppRegulationUpperView::getMessage( u32 messageID )
{
  if( m_pMessageUtility != NULL )
  {
    return &m_pMessageUtility->GetString( GARC_message_regulation_DAT, messageID );
  }

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);
  gfl2::str::MsgData* pMessageData = pG2DUtil->GetMsgData();
  if( pMessageData )
  {
    pMessageData->GetString( messageID, *pTempStrBuf );
  }
  else
  {
   GFL_ASSERT_MSG( 0, "Error !! : Not found message data.\n" );
  }
  return pTempStrBuf;
}


void NetAppRegulationUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

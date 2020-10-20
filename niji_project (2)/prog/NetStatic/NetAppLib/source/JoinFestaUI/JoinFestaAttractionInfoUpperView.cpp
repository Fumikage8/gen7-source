// ============================================================================
/*
 * @file JoinFestaAttractionInfoUpperView.cpp
 * @brief アトラクション情報上画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionInfoUpperView.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>



GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

u32 TOTAL_STAR_ICON_PANE_INDEX[] =
{
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_03,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_02,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_01,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_00,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B
};


u32 MY_STAR_ICON_PANE_INDEX[] =
{
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_04,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_05,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_06,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_07,
  PANENAME_JFPL_WIN_UPP_001_PANE_FESTA_ICON_01B_08
};


JoinFestaAttractionInfoUpperView::JoinFestaAttractionInfoUpperView(
  NetAppLib::System::ApplicationWorkBase*       pWorkBase,
  NetAppLib::System::ResourceID                 layoutResourceID ) :
  NetAppLib::System::NetApplicationViewBase(
    pWorkBase,
    layoutResourceID,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pWorkBase->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT ),
  m_pWorkBase( pWorkBase ),
  m_pJoinFestaAttractionData( NULL ),
  m_bDrawFlag( false )
{
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_ALL_ON );
  setupLayout( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_ALL_ON );

  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
    gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

    gfl2::lyt::LytTextBox* pMessage0 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_NEW_RECODE_00 );
    pMessage0->SetVisible( false );

    gfl2::lyt::LytTextBox* pMessage1 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_NEW_RECODE_01 );
    pMessage1->SetVisible( false );
  }
}


JoinFestaAttractionInfoUpperView::~JoinFestaAttractionInfoUpperView()
{
}


void JoinFestaAttractionInfoUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_bDrawFlag )
  {
    updateTimeLimitPaneText( m_pJoinFestaAttractionData );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWorkBase->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  }
}

void JoinFestaAttractionInfoUpperView::updateTimeLimitPaneText( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData )
{
  if( pData )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
    gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

    // 制限時間
    {
      gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TIME_LIMIT_00 );
      pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_42 ) );

      u32 timeLimit = pData->GetTimeLimit();

//      u32 hour      = timeLimit / 3600;
      u32 minute    = timeLimit % 3600 / 60;
      u32 second    = timeLimit % 60;
      
      gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TIME_LIMIT_01 );
      pG2DUtil->SetRegisterNumber( 0, minute, 2, print::NUM_DISP_ZERO );
      pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_49 ) );

      gfl2::lyt::LytTextBox* pText02 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TIME_LIMIT_02 );
      pG2DUtil->SetTextBoxPaneString( pText02, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_50 ) );

      gfl2::lyt::LytTextBox* pText03 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TIME_LIMIT_03 );
      pG2DUtil->SetRegisterNumber( 0, second, 2, print::NUM_DISP_ZERO );
      pG2DUtil->SetTextBoxPaneStringExpand( pText03, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_49 ) );
    }
  }
}


void JoinFestaAttractionInfoUpperView::SetJoinFestaAttractionData( NetAppLib::JoinFestaUI::JoinFestaAttractionData* pData )
{
  m_bDrawFlag = ( pData && pData->GetAttractionID() != JoinFestaScript::ATTRACTION_NULL ) ? true : false;

  m_pJoinFestaAttractionData = pData;

  if( pData && pData->GetAttractionID() != JoinFestaScript::ATTRACTION_NULL )
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( pData->GetAttractionID() );


    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();
    gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

    // アトラクション名
    {
      gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_ATTRACTION_NAME_00 );
      pG2DUtil->SetTextBoxPaneString( pText, &pMessageUtility->GetString( GARC_message_jf_attraction_DAT, static_cast<u32>( attractionData.attractionName ) ) );
    }

    // クリア目標
    {
      gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_CLEAR_GOALS_00 );
      pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_41 ) );

      u32 clearCondition = static_cast<u32>( attractionData.clearCondition );
      
      gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_CLEAR_GOALS_01 );
      pG2DUtil->SetRegisterNumber( 0, clearCondition, 5, print::NUM_DISP_ZERO );
      pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_48 ) );
    }

    // 制限時間
    {
      updateTimeLimitPaneText( pData );
    }

    // もらえるフェスコイン
    {
      gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_00 );
      pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_43 ) );

      u32 fesCoinMin = static_cast<u32>( attractionData.baseFasCoin );
      u32 fesCoinMax = static_cast<u32>( attractionData.baseFasCoin * 10 );

      gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_01 );
      pG2DUtil->SetRegisterNumber( 0, fesCoinMin, 3 );
      pG2DUtil->SetRegisterNumber( 1, fesCoinMax, 3 );
      pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_53 ) );
    }

    // 最高トータルスコア
    {
      gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_SCORE_00 );
      pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_44 ) );

      u32 totalScore = pJoinFestaDataSave->GetTotalHighScore( pData->GetAttractionID() );

      gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_SCORE_01 );
      pG2DUtil->SetRegisterNumber( 0, totalScore, 5, print::NUM_DISP_ZERO );
      pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_48 ) );

      // ★の数計算
      {
        u32 starNum = 0;

        if( attractionData.totalEvaluationStar5 <= totalScore )      { starNum = 5; }
        else if( attractionData.totalEvaluationStar4 <= totalScore ) { starNum = 4; }
        else if( attractionData.totalEvaluationStar3 <= totalScore ) { starNum = 3; }
        else if( attractionData.totalEvaluationStar2 <= totalScore ) { starNum = 2; }
        else if( attractionData.totalEvaluationStar1 <= totalScore ) { starNum = 1; }
        else                                                         { starNum = 0; }

        for( u32 i = 0; i < 5; ++i )
        {
          bool bVisible = ( i < starNum ) ? true : false;

          gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( TOTAL_STAR_ICON_PANE_INDEX[i] );
          pStarIcon->SetVisible( bVisible );
        }
      }

    }

    // 最高個人スコア
    {
      gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_INDIVIDUAL_SCORE_00 );
      pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_45 ) );

      u32 myScore = pJoinFestaDataSave->GetManHighScore( pData->GetAttractionID() );

      gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_INDIVIDUAL_SCORE_01 );
      pG2DUtil->SetRegisterNumber( 0, myScore, 3, print::NUM_DISP_ZERO );
      pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_52 ) );

      // ★の数計算
      {
        u32 starNum = 0;

        if( attractionData.manEvaluationStar5 <= myScore )      { starNum = 5; }
        else if( attractionData.manEvaluationStar4 <= myScore ) { starNum = 4; }
        else if( attractionData.manEvaluationStar3 <= myScore ) { starNum = 3; }
        else if( attractionData.manEvaluationStar2 <= myScore ) { starNum = 2; }
        else if( attractionData.manEvaluationStar1 <= myScore ) { starNum = 1; }
        else                                                    { starNum = 0; }

        for( u32 i = 0; i < 5; ++i )
        {
          bool bVisible = ( i < starNum ) ? true : false;

          gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( MY_STAR_ICON_PANE_INDEX[i] );
          pStarIcon->SetVisible( bVisible );
        }
      }
    }
  }
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

// ============================================================================
/*
 * @file JoinFestaAttractionResultUpperView.cpp
 * @brief アトラクション結果アプリの上画面を表示するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/View/JoinFestaAttractionResultUpperView.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(View)

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


JoinFestaAttractionResultUpperView::JoinFestaAttractionResultUpperView( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaAttractionResultWork,
    NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaAttractionResultWork->GetMessageUtility(),
    GARC_message_jf_playerlist_DAT ),
  m_pJoinFestaAttractionResultWork( pJoinFestaAttractionResultWork )
{
//  setupLayout( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_STAR_ON );
  
  SetVisibleScore( false );
  SetVisibleGetFesCoin( true ); // @note : 取得できるコインのアニメーションを行う場合は false
  SetVisibleTotalStarIcon( false );
  SetVisibleMyStarIcon( false );
  SetVisibleTotalScoreUpdateMessage( false );
  SetVisibleMyScoreUpdateMessage( false );

}


JoinFestaAttractionResultUpperView::~JoinFestaAttractionResultUpperView()
{
}


void JoinFestaAttractionResultUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaAttractionResultWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


void JoinFestaAttractionResultUpperView::SetTotalScore( u32 score, u32 digit )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaAttractionResultWork->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pTotalScore = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_SCORE_01 );
  pG2DUtil->SetRegisterNumber( 0, score, digit, print::NUM_DISP_ZERO );
  pG2DUtil->SetTextBoxPaneStringExpand( pTotalScore, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_48 ) );
}


void JoinFestaAttractionResultUpperView::SetMyScore( u32 score, u32 digit )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaAttractionResultWork->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pMyScore = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_INDIVIDUAL_SCORE_01 );
  pG2DUtil->SetRegisterNumber( 0, score, digit, print::NUM_DISP_ZERO );
  pG2DUtil->SetTextBoxPaneStringExpand( pMyScore, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_54 ) );
}


void JoinFestaAttractionResultUpperView::SetGetFesCoin( u32 fesCoin, u32 digit )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaAttractionResultWork->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_01 );
  pG2DUtil->SetRegisterNumber( 0, fesCoin, digit );
  pG2DUtil->SetRegisterNumber( 1, fesCoin, digit );
  pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_53 ) );
}


void JoinFestaAttractionResultUpperView::SetVisibleScore( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pTotalScore = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_SCORE_01 );
  gfl2::lyt::LytTextBox* pMyScore = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_INDIVIDUAL_SCORE_01 );

  pTotalScore->SetVisible( bVisible );
  pMyScore->SetVisible( bVisible );
}


void JoinFestaAttractionResultUpperView::SetVisibleGetFesCoin( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_01 );

  pText01->SetVisible( bVisible );
}


void JoinFestaAttractionResultUpperView::SetVisibleTotalStarIcon( bool bVisible, int index )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  if( index < 0 )
  {
    for( int i = 0; i < 5; ++i )
    {
      gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( TOTAL_STAR_ICON_PANE_INDEX[i] );
      pStarIcon->SetVisible( bVisible );
    }
  }
  else
  if( 0 <= index && index < 5 )
  {
    gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( TOTAL_STAR_ICON_PANE_INDEX[index] );
    pStarIcon->SetVisible( bVisible );
  }
}


void JoinFestaAttractionResultUpperView::SetVisibleMyStarIcon( bool bVisible, int index )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  if( index < 0 )
  {
    for( int i = 0; i < 5; ++i )
    {
      gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( MY_STAR_ICON_PANE_INDEX[i] );
      pStarIcon->SetVisible( bVisible );
    }
  }
  else
  if( 0 <= index && index < 5 )
  {
    gfl2::lyt::LytPane* pStarIcon = pUpper->GetPane( MY_STAR_ICON_PANE_INDEX[index] );
    pStarIcon->SetVisible( bVisible );
  }
}


void JoinFestaAttractionResultUpperView::SetVisibleTotalScoreUpdateMessage( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pMessage0 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_NEW_RECODE_00 );

  pMessage0->SetVisible( bVisible );

  pG2DUtil->SetTextBoxPaneString( pMessage0, jf_playerlist_cap_40 );
}


void JoinFestaAttractionResultUpperView::SetVisibleMyScoreUpdateMessage( bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
  
  gfl2::lyt::LytTextBox* pMessage1 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_NEW_RECODE_01 );

  pMessage1->SetVisible( bVisible );

  pG2DUtil->SetTextBoxPaneString( pMessage1, jf_playerlist_cap_40 );
}


void JoinFestaAttractionResultUpperView::StartTotalStarIconAnimation( u32 starNum )
{
  switch( starNum )
  {
    case 1: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_1 ); break;
    case 2: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_2 ); break;
    case 3: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_3 ); break;
    case 4: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_4 ); break;
    case 5: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_UP_STAR_5 ); break;
  }
}


void JoinFestaAttractionResultUpperView::StartMyStarIconAnimation( u32 starNum )
{
  switch( starNum )
  {
    case 1: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_1 ); break;
    case 2: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_2 ); break;
    case 3: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_3 ); break;
    case 4: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_4 ); break;
    case 5: UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_JOIN_FESTA_PLAYERLIST_JFPL_WIN_UPP_001_DOWN_STAR_5 ); break;
  }
}


void JoinFestaAttractionResultUpperView::SetupAttractionResultInfo()
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

  NetAppLib::JoinFesta::JoinFestaAttractionDefine::ResultData& resultData = pJoinFestaAttractionManager->GetResultData();

  NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( resultData.attractionId );

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaAttractionResultWork->GetMessageUtility();
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

    u32 clearCondition = resultData.clearCondition;
      
    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_CLEAR_GOALS_01 );
    pG2DUtil->SetRegisterNumber( 0, clearCondition, 5, print::NUM_DISP_ZERO );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_48 ) );
  }

  // 制限時間
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TIME_LIMIT_00 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_42 ) );

    u32 timeLimit = resultData.timeLimit;

//    u32 hour      = timeLimit / 3600;
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

  // もらえるフェスコイン
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_00 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_43 ) );

#if 1 // @note : 取得できるコインのアニメーションを行う場合は 0
    u32 fesCoinMin = static_cast<u32>( attractionData.baseFasCoin );
    u32 fesCoinMax = static_cast<u32>( attractionData.baseFasCoin * 10 );

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_FESCOIN_01 );
    pG2DUtil->SetRegisterNumber( 0, fesCoinMin, 3 );
    pG2DUtil->SetRegisterNumber( 1, fesCoinMax, 3 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_53 ) );
#endif
  }

  // トータルスコア
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_TOTAL_SCORE_00 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_46 ) );
  }

  // 個人スコア
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFPL_WIN_UPP_001_PANE_INDIVIDUAL_SCORE_00 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_47 ) );
  }
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

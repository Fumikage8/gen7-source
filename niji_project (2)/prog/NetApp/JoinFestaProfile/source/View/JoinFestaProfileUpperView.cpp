// ============================================================================
/*
 * @file JoinFestaProfileUpperView.cpp
 * @brief プロフィールアプリの上画面を表示するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileUpperView.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_profile/join_festa_profile.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_anim_list.h>
#include <niji_conv_header/app/join_festa_profile/join_festa_profile_pane.h>
#include <niji_conv_header/message/msg_jf_myprofile.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)

JoinFestaProfileUpperView::JoinFestaProfileUpperView( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::JOINFESTAPROFILE_RESOURCE_ID_LAYOUT,
    LYTRES_JOIN_FESTA_PROFILE_JFP_WIN_UPP_000_BFLYT,
    LA_JOIN_FESTA_PROFILE_JFP_WIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pJoinFestaProfileWork->GetMessageUtility(),
    GARC_message_jf_myprofile_DAT ),
  m_pJoinFestaProfileWork( pJoinFestaProfileWork ),
  m_JoinFestaName( 256, pJoinFestaProfileWork->GetDeviceHeap() )
{
  setupTextMessage();
}


JoinFestaProfileUpperView::~JoinFestaProfileUpperView()
{
}


void JoinFestaProfileUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pJoinFestaProfileWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


void JoinFestaProfileUpperView::UpdateFestaNameTextPane()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // [プレイヤー名]の[ジョインフェスタ名]
  {
    pJoinFestaDataSave->GetJoinFestaNameString( &m_JoinFestaName );

    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_DAIJI_00 );
    pG2DUtil->SetRegisterPlayerName( 0, pMyStatus );
    pG2DUtil->SetRegisterWord( 1, m_JoinFestaName );
    pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_01 ) );
  }
}


void JoinFestaProfileUpperView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  NetAppLib::Message::MessageUtility* pMessageUtility = m_pJoinFestaProfileWork->GetMessageUtility();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetRecordSaveData();

  // [プレイヤー名]の[ジョインフェスタ名]
  {
    pJoinFestaDataSave->GetJoinFestaNameString( &m_JoinFestaName );

    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_DAIJI_00 );
    pG2DUtil->SetRegisterPlayerName( 0, pMyStatus );
    pG2DUtil->SetRegisterWord( 1, m_JoinFestaName );
    pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_01 ) );
  }

  // 現在のランク
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_00 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_02 ) );

    u32 rank = static_cast<u32>( pJoinFestaDataSave->GetRank() );

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_00 );
    pG2DUtil->SetRegisterNumber( 0, rank, 4 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_13 ) );
  }

  // これまでに集めたコイン
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_01 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_03 ) );

    u32 totalFesCoin = pJoinFestaDataSave->GetTotalCoin();

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_01 );
    pG2DUtil->SetRegisterNumber( 0, totalFesCoin, 7 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_14 ) );
  }

  // ランクアップまであといくつ？
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_02 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_04 ) );

    u32 nextFesCoin = pJoinFestaDataSave->GetCoinNecessaryForNextRank();

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_02 );
    pG2DUtil->SetRegisterNumber( 0, nextFesCoin, 7 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_15 ) );
  }

  // [プレイヤー名]の記録
  {
    gfl2::lyt::LytTextBox* pText = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_DAIJI_01 );
    pG2DUtil->SetRegisterPlayerName( 0, pMyStatus );
    pG2DUtil->SetTextBoxPaneStringExpand( pText, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_05 ) );
  }

  // 持っているコイン
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_03 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_06 ) );

    u32 fesCoin = pJoinFestaDataSave->GetCoin();

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_03 );
    pG2DUtil->SetRegisterNumber( 0, fesCoin, 7 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_16 ) );
  }

  // 交流したゲスト
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_04 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_07 ) );

    u32 guestNum = pRecord->Get( Savedata::Record::RECID_JF_GEST_TALK_CNT );

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_04 );
    pG2DUtil->SetRegisterNumber( 0, guestNum, 7 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_17 ) );
  }

  // 招いたお店の数
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_05 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_08 ) );

    u32 shopNum = pRecord->Get( Savedata::Record::RECID_JF_SHOP_GET_CNT );

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_05 );
    pG2DUtil->SetRegisterNumber( 0, shopNum, 4 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_18 ) );
  }

  // 参加したアトラクション
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KOUMOKU_06 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_09 ) );

    u32 entryAttractionNum = pRecord->Get( Savedata::Record::RECID_JF_ATT_CNT );

    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_SUUCHI_06 );
    pG2DUtil->SetRegisterNumber( 0, entryAttractionNum, 4 );
    pG2DUtil->SetTextBoxPaneStringExpand( pText01, pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_19 ) );
  }

  // コロン置き換え
  {
    gfl2::lyt::LytTextBox* pText00 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_00 );
    gfl2::lyt::LytTextBox* pText01 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_01 );
    gfl2::lyt::LytTextBox* pText02 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_02 );
    gfl2::lyt::LytTextBox* pText03 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_03 );
    gfl2::lyt::LytTextBox* pText04 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_04 );
    gfl2::lyt::LytTextBox* pText05 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_05 );
    gfl2::lyt::LytTextBox* pText06 = pUpper->GetTextBoxPane( PANENAME_JFP_WIN_UPP_000_PANE_KONMA_06 );
    pG2DUtil->SetTextBoxPaneString( pText00, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText01, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText02, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText03, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText04, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText05, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
    pG2DUtil->SetTextBoxPaneString( pText06, &pMessageUtility->GetString( GARC_message_jf_myprofile_DAT, msg_jf_myprofile_cap_10 ) );
  }

}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

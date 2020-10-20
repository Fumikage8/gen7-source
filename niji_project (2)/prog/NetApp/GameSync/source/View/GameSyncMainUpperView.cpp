// ============================================================================
/*
 * @file GameSyncMainUpperView.cpp
 * @brief ゲームシンクアプリの上画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/View/GameSyncMainUpperView.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/gamesync/gamesync.h>
#include <niji_conv_header/app/gamesync/gamesync_anim_list.h>
#include <niji_conv_header/app/gamesync/gamesync_pane.h>
#include <niji_conv_header/message/msg_gamesync.h>


#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)

GameSyncMainUpperView::GameSyncMainUpperView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGameSyncWork,
    NetApp::GameSync::GAMESYNC_RESOURCE_ID_LAYOUT,
    LYTRES_GAMESYNC_GAMESYNC_MAIN_UP_000_BFLYT,
    LA_GAMESYNC_GAMESYNC_MAIN_UP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pGameSyncWork->GetMessageUtility(),
    GARC_message_gamesync_DAT ),
  m_pGameSyncWork( pGameSyncWork ),
  m_GameSyncID( 64, pGameSyncWork->GetDeviceHeap() )
{
  UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_GAMESYNC_GAMESYNC_MAIN_UP_000__BG_ANIM_BG_LOOP_00 );
}


GameSyncMainUpperView::~GameSyncMainUpperView()
{
}


void GameSyncMainUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGameSyncWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}


void GameSyncMainUpperView::SetGameSyncID( u32 descriptionMessageID, bool bExpandDateTime )
{
  Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

  NetApp::Util::ConvertUtility::U64ToStr( pMyStatus->GetNexUniqueID(), &m_GameSyncID );

  const gfl2::str::STRCODE* pGameSyncID = m_GameSyncID.GetPtr();

  gfl2::str::STRCODE gameSyncID1[5];
  gfl2::str::STRCODE gameSyncID2[5];
  gfl2::str::STRCODE gameSyncID3[5];
  gfl2::str::STRCODE gameSyncID4[5];

  gfl2::std::MemClear( gameSyncID1, sizeof( gameSyncID1 ) );
  gfl2::std::MemClear( gameSyncID2, sizeof( gameSyncID2 ) );
  gfl2::std::MemClear( gameSyncID3, sizeof( gameSyncID3 ) );
  gfl2::std::MemClear( gameSyncID4, sizeof( gameSyncID4 ) );

  gfl2::std::MemCopy( &pGameSyncID[0], &gameSyncID1[0], sizeof( gfl2::str::STRCODE ) * 4 );
  gfl2::std::MemCopy( &pGameSyncID[4], &gameSyncID2[0], sizeof( gfl2::str::STRCODE ) * 4 );
  gfl2::std::MemCopy( &pGameSyncID[8], &gameSyncID3[0], sizeof( gfl2::str::STRCODE ) * 4 );
  gfl2::std::MemCopy( &pGameSyncID[12], &gameSyncID4[0], sizeof( gfl2::str::STRCODE ) * 4 );

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTBOX_TITLE, msg_gamesync_sys_01 );
  pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTID_00, gameSyncID1 );
  pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTID_01, gameSyncID2 );
  pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTID_02, gameSyncID3 );
  pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTID_03, gameSyncID4 );

  if( bExpandDateTime )
  {
    Savedata::GameSyncSave* pGameSyncSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetGameSync();
    Savedata::GameSyncSave::DateInfo& dateInfo = pGameSyncSave->GetGameSyncDateTime();

    pG2DUtil->SetRegisterNumber( 0, dateInfo.m_year,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    pG2DUtil->SetRegisterNumber( 1, dateInfo.m_month,  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    pG2DUtil->SetRegisterNumber( 2, dateInfo.m_day,    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    pG2DUtil->SetRegisterNumber( 3, dateInfo.m_hour,   2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    pG2DUtil->SetRegisterNumber( 4, dateInfo.m_minute, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
    pG2DUtil->SetTextBoxPaneStringExpand( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTBOX_22X7, descriptionMessageID );
  }
  else
  {
    pG2DUtil->SetTextBoxPaneString( LAYOUT_WORK_ID_UPPER, PANENAME_GAMESYNC_MAIN_UP_000_PANE_TEXTBOX_22X7, descriptionMessageID );
  }
}

gfl2::lyt::LytPicture* GameSyncMainUpperView::GetIconPane(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pUpper = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );

  return pUpper->GetPicturePane( PANENAME_GAMESYNC_MAIN_UP_000_PANE_PLAYERPHOTO_04 );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

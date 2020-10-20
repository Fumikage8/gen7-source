// ============================================================================
/*
 * @file GameSyncUppCaptureUpperView.cpp
 * @brief ゲームシンクアプリの上画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/View/GameSyncUppCaptureUpperView.h"
#include "NetApp/GameSync/source/GameSyncResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/gamesync/gamesync.h>
#include <niji_conv_header/app/gamesync/gamesync_anim_list.h>
#include <niji_conv_header/app/gamesync/gamesync_pane.h>
#include <niji_conv_header/message/msg_gamesync.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)

GameSyncUppCaptureUpperView::GameSyncUppCaptureUpperView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGameSyncWork,
    NetApp::GameSync::GAMESYNC_RESOURCE_ID_LAYOUT,
    LYTRES_GAMESYNC_GAMESYNC_SAMPLEPIC_UP_000_BFLYT,
    LA_GAMESYNC_GAMESYNC_SAMPLEPIC_UP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    pGameSyncWork->GetMessageUtility(),
    GARC_message_gamesync_DAT ),
  m_pGameSyncWork( pGameSyncWork )
{
  UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_GAMESYNC_GAMESYNC_SAMPLEPIC_UP_000__BG_ANIM_BG_LOOP_00 );
  UIView::GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_UPPER, LA_GAMESYNC_GAMESYNC_SAMPLEPIC_UP_000_PATERN_00 );
}


GameSyncUppCaptureUpperView::~GameSyncUppCaptureUpperView()
{
}


void GameSyncUppCaptureUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGameSyncWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
}



GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

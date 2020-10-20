// ============================================================================
/*
 * @file JoinFestaSubScreenFrameFactory.cpp
 * @brief ミジョインフェスタ下画面のフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenFrameFactory.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"

// 各Frame
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenMainFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAskEndFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenBattleFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenBattleRuleFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenTradeFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenInvitationFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAttractionFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAttractionRetireFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAlertFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenTutorialFrame.h"
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenBgOnlyFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


JoinFestaSubScreenFrameFactory::JoinFestaSubScreenFrameFactory( 
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView,
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pWork
):
  m_pJoinFestaSubScreenParentView( pJoinFestaSubScreenParentView )
    ,m_pWork( pWork )
{
}


JoinFestaSubScreenFrameFactory::~JoinFestaSubScreenFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* JoinFestaSubScreenFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID ){
  case JOINFESTASUBSCREEN_FRAME_ID_MAIN:       //! メインフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_MAIN\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenMainFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_ASK_END:    //! 終了問いフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_ASK_END\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenAskEndFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_BATTLE:     //! 何の対戦をするか選択するフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_BATTLE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenBattleFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE://! 対戦のルールを選択するフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenBattleRuleFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_TRADE:      //! 何の交換をするか選択するフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_TRADE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenTradeFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_INVITATION: //! お誘い通知フレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_INVITATION\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenInvitationFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION: //! アトラクション中フレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenAttractionFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION_RETIRE: //! アトラクションリタイアフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_ATTRACTION_RETIRE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenAttractionRetireFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_ALERT: //! アラート表示フレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_ALERT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenAlertFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_TUTORIAL:    //! チュートリアル中フレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_TUTORIAL\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenTutorialFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  case JOINFESTASUBSCREEN_FRAME_ID_BG_ONLY:     //! BG表示のみフレーム
    GFL_PRINT("JFSS_FRAME> JOINFESTASUBSCREEN_FRAME_ID_BG_ONLY\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::JoinFestaSubScreenBgOnlyFrame( m_pWork, m_pJoinFestaSubScreenParentView );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

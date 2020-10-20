// ============================================================================
/*
 * @file NBRFrameFactory.cpp
 * @brief 通信対戦受付アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/ApplicationSystem/NBRFrameFactory.h"
#include "NetApp/NBR/source/NBRFrameID.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// NBRフレームベース
#include "NetApp/NBR/source/NBRFrameBase.h"
// NBR各フレーム
#include "NetApp/NBR/source/Frame/NBRRuleSelectorPriorityFrame.h"
#include "NetApp/NBR/source/Frame/NBRRuleSelectorDecideFrame.h"
#include "NetApp/NBR/source/Frame/NBRBattleStartFrame.h"
#include "NetApp/NBR/source/Frame/NBRContinueFrame.h"
#include "NetApp/NBR/source/Frame/NBRMultiParentWaitFrame.h"
#include "NetApp/NBR/source/Frame/NBRMultiPlayerSelectFrame.h"
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppCallFrame.h"
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppEndFrame.h"
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppEndCancelFrame.h"
#include "NetApp/NBR/source/Frame/NBRRuleSelectWaitFrame.h"
#include "NetApp/NBR/source/Frame/NBRTeamSelectAppEndFrame.h"
#include "NetApp/NBR/source/Frame/NBRTeamSelectFrame.h"
#include "NetApp/NBR/source/Frame/NBRTeamSelectWaitFrame.h"
#include "NetApp/NBR/source/Frame/NBRPokePartyCheckFrame.h"
#include "NetApp/NBR/source/Frame/NBRAppEndFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NBRFrameFactory::NBRFrameFactory( View::NBRParentView* pParentView, NBRWork* pWork, NBRNet* pNet )
  : m_pParentView( pParentView )
  ,m_pWork( pWork ) 
  ,m_pNet( pNet )
{
}


NBRFrameFactory::~NBRFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* NBRFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pWork->GetAppHeap();

  Frame::NBRFrameBase* pFrame = NULL;

  switch( frameID ){
  case NBR_FRAME_ID_RULE_SELECTOR_PRIORITY:  ///< ルール選択する人の優先順位決め
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECTOR_PRIORITY\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectorPriorityFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_RULE_SELECTOR_DECIDE:    ///< ルール選択する人を決定
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECTOR_DECIDE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectorDecideFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_RULE_SELECT_APP_CALL:    ///< ルール選択アプリ呼び出し
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECT_APP_CALL\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectAppCallFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_RULE_SELECT_WAIT:        ///< ルール選択待ち
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECT_WAIT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectWaitFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_RULE_SELECT_APP_END:     ///< ルール選択アプリ戻り
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECT_APP_END\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectAppEndFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL://! ルール選択アプリキャンセル戻り
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRRuleSelectAppEndCancelFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_TEAM_SELECT:             ///< チーム選択
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_TEAM_SELECT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRTeamSelectFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_TEAM_SELECT_APP_END:     ///< チーム選択アプリ終了
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_TEAM_SELECT_APP_END\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRTeamSelectAppEndFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_TEAM_SELECT_WAIT:        ///< チーム選択待ち
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_TEAM_SELECT_WAIT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRTeamSelectWaitFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_POKEPARTY_CHECK:         //! ポケパーティ検査
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_POKEPARTY_CHECK\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRPokePartyCheckFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_MULTI_PLAYER_SELECT:     ///< マルチチーム振り分け
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_MULTI_PLAYER_SELECT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRMultiPlayerSelectFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_MULTI_PARENT_WAIT:       ///< マルチ親待ち
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_MULTI_PARENT_WAIT\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRMultiParentWaitFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_BATTLE_START:            ///< バトルへ
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_BATTLE_START\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRBattleStartFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_CONTINUE:                ///< 続けて遊ぶ
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_CONTINUE\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRContinueFrame( m_pWork, m_pParentView, m_pNet );
    break;
  case NBR_FRAME_ID_APP_END:
    GFL_RELEASE_PRINT("NBR FRAME> NBR_FRAME_ID_APP_END\n");
    pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) Frame::NBRAppEndFrame( m_pWork, m_pParentView, m_pNet );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

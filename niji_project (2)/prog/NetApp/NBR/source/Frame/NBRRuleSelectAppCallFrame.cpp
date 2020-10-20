// ============================================================================
/*
 * @file NBRRuleSelectAppCallFrame.cpp
 * @brief 通信対戦受付アプリのルール選択アプリ呼び出しプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_CALL
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppCallFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectAppCallFrame::NBRRuleSelectAppCallFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
{
}

// コンストラクタ
NBRRuleSelectAppCallFrame::~NBRRuleSelectAppCallFrame()
{
}

//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   このフレームの起動処理を記述します。true で終了
 */
//--------------------------------------------------------------------------
bool NBRRuleSelectAppCallFrame::startup()
{
  // 下画面にBGしか表示しない
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->GetLowerViewReturnButton()->SetAllVisible(false);
  return true;
}
//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppCallFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case 0:
    // たいせんルールを　きめてください
    m_pMesageMenuView->StartMessage( msg_nbr_win_06, View::MESSAGE_FINISH_INPUT );
    AddSubSeq();
    break;
  case 1:
    if( m_pMesageMenuView->IsEndMessage() )
    {
      exitFrame( RuleSelectAppCallResult::RULE_SELECT_APP_CALL );
    }
    break;
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

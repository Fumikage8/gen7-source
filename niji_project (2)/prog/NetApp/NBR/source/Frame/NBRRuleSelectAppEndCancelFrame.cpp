// ============================================================================
/*
 * @file NBRRuleSelectAppEndCancelFrame.cpp
 * @brief 通信対戦受付アプリのルール選択アプリでキャンセル戻りしたプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_END_CANCEL
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppEndCancelFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectAppEndCancelFrame::NBRRuleSelectAppEndCancelFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_result( NetAppLib::System::FRAME_RESULT_INVALID )
{
}

// コンストラクタ
NBRRuleSelectAppEndCancelFrame::~NBRRuleSelectAppEndCancelFrame()
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
bool NBRRuleSelectAppEndCancelFrame::startup()
{
  // 上下画面にBGしか表示しない
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_SIMPLE );
  m_pParentView->GetUpperViewPlayerInfoSimple()->SetAllVisible(false);
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->GetLowerViewReturnButton()->SetAllVisible(false);

  // 選択肢表示
  m_pMesageMenuView->ShowRuleOtherList();

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::removeListener()
{
  m_pNet->UnregistRecvListener();
  m_pNet->ClearRecvPacketEnableCommand();
  m_pMesageMenuView->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_BUTTON_WAIT:
    // ボタン押下待ち
    break;
  case SEQ_SEND:
    // 他の人に通知
    if( m_pNet->SendRuleOther() )
    {
      SetSubSeq(SEQ_END);
    }
    break;
  case SEQ_END:
    exitFrame( m_result );
    break;
  }
}

// NetRecvListener
//------------------------------------------------------------------
/**
 * @brief  通信終了通知
 */
//------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");

  // 選択肢コマンドが出ている場合は消す
  if( m_pMesageMenuView->IsShowList() == true )
  {
    m_pMesageMenuView->HideMessage();
  }

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( RuleSelectAppEndCancelResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  m_result = RuleSelectAppEndCancelResult::TO_RULE_SELECTOR_DECIDE;

  // アプリ戻りなのでNBRWorkが初期化されている。
  // その為、自分の優先順位は見てはいけない。
  // 自分がルール選択者だった場合、ここで変える必要がある。
  if( m_pNBRWork->IsOneselfRuleSelector() )
  {
    m_pNBRWork->RuleSelectorChangeNextPriority( m_pNet->GetConnectionNum() );
  }

  // 送信開始
  SetSubSeq(SEQ_SEND);
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndCancelFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  m_result = RuleSelectAppEndCancelResult::TO_RULE_SELECT_APP_CALL;
  // 終了
  SetSubSeq(SEQ_END);
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

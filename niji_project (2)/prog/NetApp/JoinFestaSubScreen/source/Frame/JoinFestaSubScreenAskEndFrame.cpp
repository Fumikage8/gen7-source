// ============================================================================
/*
 * @file JoinFestaSubScreenAskEndFrame.cpp
 * @brief ジョインフェスタ下画面の終了を問うフレームです。
 * @date 2015.09.02
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAskEndFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenAskEndFrame::JoinFestaSubScreenAskEndFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenAskEndFrame::~JoinFestaSubScreenAskEndFrame()
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
bool JoinFestaSubScreenAskEndFrame::startup()
{
  m_pParentView->DisableAllView();
  // ボタン構築
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_ASK_END );
  m_pParentView->GetSelectLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetSelectLowerView()->SetInputEnabled( true ); // ダブルタッチ修正
    m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( true );
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAskEndFrame::setListener()
{
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAskEndFrame::removeListener()
{
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false );
  m_pParentView->GetSelectLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAskEndFrame::updateSequence()
{
  // 受信パケットのイベント通知更新
  UpdatePacketEventCheck();
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenAskEndFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAskEndFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("On2uttonAction buttonId %d\n", buttonId);

  if( buttonId == BUTTON_ID_END )
  {
    // 終了しフィールドへ戻る
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_JF_END );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAskEndFrame::OnBackButtonAction()
{
  // メインフレームへ
  exitFrame( AKS_END_RESULT_NO );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

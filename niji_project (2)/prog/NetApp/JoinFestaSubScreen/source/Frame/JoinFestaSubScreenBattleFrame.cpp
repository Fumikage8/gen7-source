// ============================================================================
/*
 * @file JoinFestaSubScreenBattleFrame.cpp
 * @brief ジョインフェスタ下画面の何の対戦をするか選ぶフレームです。
 * @date 2015.11.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenBattleFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenBattleFrame::JoinFestaSubScreenBattleFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenBattleFrame::~JoinFestaSubScreenBattleFrame()
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
bool JoinFestaSubScreenBattleFrame::startup()
{
  m_pParentView->DisableAllView();
  // ボタン構築
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_BATTLE );
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
void JoinFestaSubScreenBattleFrame::setListener()
{
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleFrame::removeListener()
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
void JoinFestaSubScreenBattleFrame::updateSequence()
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
bool JoinFestaSubScreenBattleFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("OnButtonAction buttonId %d\n", buttonId);

  switch( buttonId ){
  case BUTTON_ID_NBR:         
    exitFrame( BATTLE_RESULT_NBR );
    break;
  case BUTTON_ID_BTLSPOT:     
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_BATTLE_SPOT );
    break;
  case BUTTON_ID_RULE_DL:  
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_REGULATION_DOWNLOAD );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleFrame::OnBackButtonAction()
{
  // メインフレームへ
  exitFrame( BATTLE_RESULT_RETURN );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

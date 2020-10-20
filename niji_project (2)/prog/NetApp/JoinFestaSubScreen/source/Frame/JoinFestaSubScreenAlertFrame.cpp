// ============================================================================
/*
 * @file JoinFestaSubScreenAlertFrame.cpp
 * @brief ジョインフェスタ下画面のアラート表示を行うフレームです。
 * @date 2016.01.07
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAlertFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include <niji_conv_header/message/msg_jf_menu.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenAlertFrame::JoinFestaSubScreenAlertFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenAlertFrame::~JoinFestaSubScreenAlertFrame()
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
bool JoinFestaSubScreenAlertFrame::startup()
{
  m_pParentView->DisableAllView();
  m_pParentView->GetAlertLowerView()->SetMessage( m_pJoinFestaSubScreenWork->GetAlertMessageID() );
  m_pParentView->GetAlertLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetAlertLowerView()->SetInputEnabled( true ); // ダブルタッチ修正
    m_pParentView->GetAlertLowerView()->GetButtonManager()->SetInputEnable( true );
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAlertFrame::setListener()
{
  m_pParentView->GetAlertLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAlertFrame::removeListener()
{
  m_pParentView->GetAlertLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetAlertLowerView()->GetButtonManager()->SetInputEnable( false );
  m_pParentView->GetAlertLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAlertFrame::updateSequence()
{
  // @fix NMcat[322] ↓をすると、パケットイベントを処理するのでお誘いフレンドキーが上書きされる。
  // 受信パケットのイベント通知更新
  //UpdatePacketEventCheck();

  if( m_pJoinFestaSubScreenWork->GetAlertMessageID() == jf_menu_exp_01 )
  {
    if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      exitFrame( ALEART_RESULT_RETURN_TO_MAIN );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenAlertFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAlertFrame::OnBackButtonAction()
{
  if( m_pJoinFestaSubScreenWork->GetAlertMessageID() == jf_menu_exp_01 )
  {
    exitFrame( ALEART_RESULT_RETURN_TO_MAIN );
  }
  else
  {
    switch( m_pJoinFestaSubScreenWork->GetAlertBootFrameID() )
    {
      case NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN:
      {
        exitFrame( ALEART_RESULT_RETURN_TO_MAIN );
      }
      break;

      case NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE:
      {
        exitFrame( ALEART_RESULT_RETURN_TO_BATTLE_RULE );
      }
      break;

      case NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TRADE:
      {
        // @fix NMCat[401]：メインに戻すのが正しい
        exitFrame( ALEART_RESULT_RETURN_TO_MAIN );
      }
      break;

      case NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_INVITATION:
      {
        // @fix NMCat[197]：メインに戻す
        exitFrame( ALEART_RESULT_RETURN_TO_MAIN );
      }
      break;
    }
  }
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

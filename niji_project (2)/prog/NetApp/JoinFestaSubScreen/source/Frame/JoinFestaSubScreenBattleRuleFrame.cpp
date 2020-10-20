// ============================================================================
/*
 * @file JoinFestaSubScreenBattleRuleFrame.cpp
 * @brief ジョインフェスタ下画面の対戦ルールを選ぶフレームです。
 * @date 2015.11.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenBattleRuleFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"
#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_jf_menu.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenBattleRuleFrame::JoinFestaSubScreenBattleRuleFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenBattleRuleFrame::~JoinFestaSubScreenBattleRuleFrame()
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
bool JoinFestaSubScreenBattleRuleFrame::startup()
{
  m_pParentView->DisableAllView();
  // ボタン構築
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_RULE );
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
void JoinFestaSubScreenBattleRuleFrame::setListener()
{
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleRuleFrame::removeListener()
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
void JoinFestaSubScreenBattleRuleFrame::updateSequence()
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
bool JoinFestaSubScreenBattleRuleFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleRuleFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("OnButtonAction buttonId %d\n", buttonId);

  // 誰とでも募集がないから直接飛ばす。誰とでも募集の仕様はなくなった。
  switch( buttonId ){
  case BUTTON_ID_SINGLE:     
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S );
    break;
  case BUTTON_ID_DOUBLE:     
    {
      bool bDoubleBattlePokemonCountCheckFlag = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForDoubleBattle( m_pJoinFestaSubScreenWork->GetDeviceHeap() );

      if( bDoubleBattlePokemonCountCheckFlag == false )
      {
        m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_05, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_BATTLE_RULE );
        exitFrame( BATTLE_RULE_RESULT_ALERT );
      }
      else
      {
        m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D );
      }
    }
    break;
  case BUTTON_ID_MULTI:      
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M );
    break;
  case BUTTON_ID_ROYAL:      
    m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenBattleRuleFrame::OnBackButtonAction()
{
  // 何の対戦するか選ぶフレームへ
  exitFrame( BATTLE_RULE_RESULT_RETURN );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

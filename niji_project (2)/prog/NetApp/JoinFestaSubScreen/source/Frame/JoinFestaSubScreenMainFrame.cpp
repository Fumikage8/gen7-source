// ============================================================================
/*
 * @file JoinFestaSubScreenMainFrame.cpp
 * @brief ジョインフェスタ下画面のメインフレームです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenMainFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h" // リクエストID

#include "System/include/GflUse.h"

// ジョインフェスタ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include <niji_conv_header/message/msg_jf_menu.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenMainFrame::JoinFestaSubScreenMainFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
  m_updateIconRotateTime = 0;
}


JoinFestaSubScreenMainFrame::~JoinFestaSubScreenMainFrame()
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
bool JoinFestaSubScreenMainFrame::startup()
{
  m_pParentView->DisableAllView();

  // 下画面の初期フレームで初めてBG表示
  m_pParentView->GetBgLowerView()->SetVisible( true );

  m_pParentView->GetMainLowerView()->SetVisible( true );
  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetMainLowerView()->SetInputEnabled( true );
    m_pParentView->GetMainLowerView()->GetButtonManager()->SetInputEnable( true );
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::setListener()
{
  m_pParentView->GetMainLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::removeListener()
{
  m_pParentView->GetMainLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetMainLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正
  m_pParentView->GetMainLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::updateSequence()
{
  // 受信パケットのイベント通知更新
  UpdatePacketEventCheck();

#if defined( GF_PLATFORM_CTR )

  // @fix GFNMCat[2171]：城内ではストリーミングメッセージ流さない
  if( m_pJoinFestaSubScreenWork->IsRoom() )
  {
    // 強制的に非表示
    m_pParentView->GetMainLowerView()->ForceMessageHide();
  }
  else
  {
    // ストリーミングメッセージ再生更新
    m_pParentView->GetMainLowerView()->PlayMessageUpdate();
  }

  // 一定時間でリストボタン表示更新
  u64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  if( currentTime - m_updateIconRotateTime > UPDATE_ICON_ROTATE_TIME )
  {
    m_updateIconRotateTime = currentTime;

    // プレイヤーリストボタンの「！」アイコン表示状態更新
    m_pParentView->GetMainLowerView()->UpdatePlayerListModeIcon();
  }

#endif

}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenMainFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   対戦ボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnBattleButtonAction()
{
  ICHI_PRINT("OnBattleButtonAction \n");

  if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_01, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN );
    exitFrame( MAIN_RESULT_ALERT );
  }
  else
  {
    // 何の対戦をするか選ぶフレームへ
    exitFrame( MAIN_RESULT_BATTLE );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   交換ボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnTradeButtonAction()
{
  ICHI_PRINT("OnTradeButtonAction \n");

  bool bCheckTradePenalty = NetAppLib::Util::NetAppEntryChecker::CheckTradePenalty();

  if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_01, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN );
    exitFrame( MAIN_RESULT_ALERT );
  }
  else
  if( bCheckTradePenalty == false )
  {
    m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_02, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN );
    exitFrame( MAIN_RESULT_ALERT );
  }
  else
  {
    // 何の交換をするか選ぶフレームへ
    exitFrame( MAIN_RESULT_TRADE );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   ストリーミングメッセージタッチ通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnStreamMessageAction()
{
  ICHI_PRINT("OnStreamMessageAction \n");

  // 城内はアトラクション開催出来ないので無効
  if( m_pJoinFestaSubScreenWork->IsRoom() == true )
  {
    return;
  }

  if( m_pParentView->GetMainLowerView()->IsPlayMessage() )
  {
    if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
    {
      m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_01, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN );
      exitFrame( MAIN_RESULT_ALERT );
    }
    else
    {
      m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_OPEN_LIST );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   インターネットボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnInternetButtonAction()
{
  ICHI_PRINT("OnInternetButtonAction \n");

  if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_01, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN );
    exitFrame( MAIN_RESULT_ALERT );
  }
  else
  {
    // 今のネットワークモード切り替える
    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
    {
      m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_CHANGE_WIFI );
    }
    else
    {
      m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_CHANGE_LOCAL );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   お知らせボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnInfoButtonAction()
{
  ICHI_PRINT("OnInfoButtonAction \n");
}

//--------------------------------------------------------------------------
/**
 * @brief   プレイヤーリストボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnPlayerListButtonAction()
{
  ICHI_PRINT("OnPlayerListButtonAction \n");

  m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SHOW );
}

//--------------------------------------------------------------------------
/**
 * @brief   Myプロフィールボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnMyProfileButtonAction()
{
  ICHI_PRINT("OnMyProfileButtonAction \n");

  m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PROFILE );
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケモンリストボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnPokelistButtonAction()
{
  ICHI_PRINT("OnPokelistButtonAction \n");
  m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_POKELIST );
}

//--------------------------------------------------------------------------
/**
 * @brief   バッグボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnBagButtonAction()
{
  ICHI_PRINT("OnBagButtonAction \n");
  m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_BAG );
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenMainFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction \n");
  // 終了を問うフレームへ
  exitFrame( MAIN_RESULT_ASK_END );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

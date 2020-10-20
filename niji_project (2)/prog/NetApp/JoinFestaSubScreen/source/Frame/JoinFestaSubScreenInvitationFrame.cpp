// ============================================================================
/*
 * @file JoinFestaSubScreenInvitationFrame.cpp
 * @brief ジョインフェスタ下画面のお誘い通知フレームです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenInvitationFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h" // リクエストID

#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// パーソナル用
#include "Savedata/include/MyStatus.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_jf_menu.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenInvitationFrame::JoinFestaSubScreenInvitationFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenInvitationFrame::~JoinFestaSubScreenInvitationFrame()
{
}


//--------------------------------------------------------------------------
/**
 * @brief   システム側で一時停止呼び出し時にコールされる
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnSuspend()
{
  // @fix NMCat[101]：Suspend、Resumeで時間が止まらないようにする
  // 自動で閉じる制限時間の更新は無効にする
  //m_autoCloseTimeout.Suspend();
}

//--------------------------------------------------------------------------
/**
 * @brief   システム側で再起動呼び出し時にコールされる
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnResume()
{
  // @fix NMCat[101]：Suspend、Resumeで時間が止まらないようにする
  // 自動で閉じる制限時間の更新を有効にする
  //m_autoCloseTimeout.Resume();
}

//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   このフレームの起動処理を記述します。true で終了
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenInvitationFrame::startup()
{
  m_pParentView->DisableAllView();

  // 選択肢ビューをお断りリストで設定（表示はしない）
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_ADD );

  // お誘い画面に変更
  ChangeInvitationView();

  // @fix NMCat[216]：Suspendで入力不許可後にStartupが通る為、ここでSuspend状態だったら不許可にする処理を入れる。
  if( m_pJoinFestaSubScreenWork->IsSuspend() == true )
  {
    m_pParentView->GetInvitationLowerView()->SetInputEnabled( false );
  }

  // お誘い種類のテキスト表示
  {
    // パケットイベント通知をしてきた相手のパーソナル取得
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    JoinFestaFriendKey  targetFriendKey = m_pJoinFestaSubScreenWork->GetPacketEventFriendKey();
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = manager->GetPersonalData( targetFriendKey );

    if( personal == NULL )
    {
      GFL_ASSERT(0); // ソースミスってたらある。仕様的にはない。
    }
    else
    {
      // 名前表示用にMyStatusを作って渡す
      Savedata::MyStatus myStatus;
      gfl2::str::StrBuf tmp( personal->GetName(), m_pHeap );
      myStatus.SetNameString( &tmp );
      myStatus.SetSex( personal->GetSex() );

      m_pParentView->GetInvitationLowerView()->SetInvitationMessage( m_pJoinFestaSubScreenWork->GetPacketEventType(), myStatus );
    }
  }

  // 自動クローズ制限時間初期化
  m_pParentView->GetInvitationLowerView()->TimeOutStart();
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::setListener()
{
  m_pParentView->GetInvitationLowerView()->SetListener( this );
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::removeListener()
{
  m_pParentView->GetInvitationLowerView()->SetInputEnabled( false );
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false );
  m_pParentView->GetInvitationLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正
  m_pParentView->GetInvitationLowerView()->RemoveListener();
  m_pParentView->GetSelectLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::updateSequence()
{
  // 自動で閉じる制限時間更新
  bool isTimeout = m_pParentView->GetInvitationLowerView()->IsTimeOut();
  if( isTimeout == true )
  {// 制限時間経過
    // メインフレームへ
    exitFrame( INVITATION_RESULT_RETURN );
    // 入力無効
    m_pParentView->GetInvitationLowerView()->SetInputEnabled( false );
    m_pParentView->GetInvitationLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正

    //キャンセル or タイムアウトなのでキャンセルを飛ばす
    SendCancelRequest();
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenInvitationFrame::cleanup()
{
  return true;
}

//-----------------------------------------------------------------------------
// JoinFestaSubScreenInvitationLowerInputListener
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   申し込みOKボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnOkButtonAction()
{
  ICHI_PRINT("Invitaion::OnOkButtonAction \n");

  
  if( !NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_01, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_INVITATION );
    exitFrame( INVITATION_RESULT_ALERT );
  }
  // @fix GFNMcat[631] 違うお誘いでマッチングしてしまう
  else if( !CheckPacketEventAgree() )
  {
    // お誘いビューを無効化
    m_pParentView->GetInvitationLowerView()->SetVisible( false );
    m_pParentView->GetInvitationLowerView()->SetInputEnabled( false );
    m_pParentView->GetInvitationLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正

    // メッセージ出してTOP画面へ
    // 「つうしんが　ちゅうだんされました」
    m_pParentView->GetSelectLowerView()->SetVisible( true );
    m_pParentView->GetSelectLowerView()->SetVisibleSelectButton( false );
    // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
    if( !m_pJoinFestaSubScreenWork->IsSuspend() )
    {
      m_pParentView->GetSelectLowerView()->SetInputEnabled( true );
      m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( true );
    }
    m_pParentView->GetSelectLowerView()->SetStreamMessage( jf_menu_win_26 );

    // @fix GFNMCat[3968]：自動で閉じられると、メッセージカーソルが残ってしまうので無効にする
    m_pParentView->GetInvitationLowerView()->TimeOutSuspend();
  }
  else
  {
    bool bEntryOK = true;

    bool bCheckTradePenalty = NetAppLib::Util::NetAppEntryChecker::CheckTradePenalty();
    bool bCheckPokemonCountForTrade = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForTrade( m_pJoinFestaSubScreenWork->GetDeviceHeap() );
    bool bCheckPokemonCountForDoubleBattle = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForDoubleBattle( m_pJoinFestaSubScreenWork->GetDeviceHeap() );

    switch( m_pJoinFestaSubScreenWork->GetPacketEventType() )
    {
      case NetApp::JoinFestaSubScreen::Define::PACKET_EVENT_TYPE_TRADE:
      {
        if( bCheckTradePenalty == false )
        {
          m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_02, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_INVITATION );
          exitFrame( INVITATION_RESULT_ALERT );
          SendCancelRequest();

          bEntryOK = false;
        }
        else
        if( bCheckPokemonCountForTrade == false )
        {
          m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_03, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_INVITATION );
          exitFrame( INVITATION_RESULT_ALERT );
          SendCancelRequest();

          bEntryOK = false;
        }
      }
      break;

      case NetApp::JoinFestaSubScreen::Define::PACKET_EVENT_TYPE_BATTLE_D:
      {
        if( bCheckPokemonCountForDoubleBattle == false )
        {
          m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_05, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_INVITATION );
          exitFrame( INVITATION_RESULT_ALERT );
          SendCancelRequest();

          bEntryOK = false;
        }
      }
      break;
    }

    if( bEntryOK )
    {
      m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING );

      // PersonalDataManagerにフレンドキーを保存
      {
        NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
        manager->SetInviteFriendKey( m_pJoinFestaSubScreenWork->GetPacketEventFriendKey(), m_pJoinFestaSubScreenWork->GetPacketEventTicketId() );
      }
    }
  }

}

//--------------------------------------------------------------------------
/**
 * @brief   お断りリストボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnBlackListButtonAction()
{
  ICHI_PRINT("Invitaion::OnBlackListButtonAction \n");
  // お断りリスト追加画面に変更
  ChangeBlackListAddView();

  // 自動で閉じる制限時間の更新は無効にする
  m_pParentView->GetInvitationLowerView()->TimeOutSuspend();
}

//--------------------------------------------------------------------------
/**
 * @brief   申し込みNGボタン（戻るボタン）通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnNgButtonAction()
{
  ICHI_PRINT("Invitaion::OnNgButtonAction \n");
  // メインフレームへ
  exitFrame( INVITATION_RESULT_RETURN );

  //キャンセル or タイムアウトなのでキャンセルを飛ばす
  SendCancelRequest();
}

//-----------------------------------------------------------------------------
// JoinFestaSubScreenMainLowerInputListener
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
// @override
void JoinFestaSubScreenInvitationFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("Select::On2uttonAction buttonId %d\n", buttonId);

  switch( buttonId )
  {
    case View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_ADD: // 追加する
    {
      // @fix GFNMcat[802]ボタン無効にしないと何度も押せてしまう
      m_pParentView->GetSelectLowerView()->SetInputEnabled( false );
      m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false );

      // 「おことわりリストを　こうしんしました」
      m_pParentView->GetSelectLowerView()->SetStreamMessage( jf_menu_win_12 );
      m_pParentView->GetSelectLowerView()->SetVisibleSelectButton( false );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
      JoinFestaFriendKey  targetFriendKey = m_pJoinFestaSubScreenWork->GetPacketEventFriendKey();
      NetAppLib::JoinFesta::JoinFestaPersonalData* personal = manager->GetPersonalData( targetFriendKey );
      if( personal )
      {
        Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
        pJoinFestaDataSave->AddBlackList( *personal );

        // @fix GFNMCat[454]:ブラックリストに入れた時にマネージャーからも消す
        // @fix NMCat[2557]ブラックリストに入れた時に、同じ人物がVIPでフィールドにいたらゲストに降格させる
        manager->RemoveListAndSave( personal->GetJoinFestaFriendKey() );
      }
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
// @override
void JoinFestaSubScreenInvitationFrame::OnBackButtonAction()
{
  ICHI_PRINT("Select::OnBackButtonAction \n");
  // お誘い画面に変更
  ChangeInvitationView();

  // 自動で閉じる制限時間の更新を有効にする
  m_pParentView->GetInvitationLowerView()->TimeOutResume();
}

//--------------------------------------------------------------------------
/**
 * @brief   「おことわりリストを　こうしんしました」メッセージ終了通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnBlackListUpdateMessageFinish()
{
  exitFrame( INVITATION_RESULT_RETURN );
  SendCancelRequest();
}

//--------------------------------------------------------------------------
/**
 * @brief   「つうしんが　ちゅうだんされました」メッセージ終了通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::OnNetworkErrorMessageFinish()
{
  exitFrame( INVITATION_RESULT_RETURN );

  // @fix NMCat[2305]：相手がお誘いしていないのでキャンセル送る必要がなかった
  //SendCancelRequest();
}

//--------------------------------------------------------------------------
/**
 * @brief   お誘い画面に変更
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::ChangeInvitationView()
{
  // お誘いビューを有効化
  m_pParentView->GetInvitationLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetInvitationLowerView()->SetInputEnabled( true );
    m_pParentView->GetInvitationLowerView()->GetButtonManager()->SetInputEnable( true );
  }

  // 選択肢ビューを無効化
  m_pParentView->GetSelectLowerView()->SetVisible( false );
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false );
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false );
}
//--------------------------------------------------------------------------
/**
 * @brief   お断りリスト追加画面に変更
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::ChangeBlackListAddView()
{
  // お誘いビューを無効化
  m_pParentView->GetInvitationLowerView()->SetVisible( false );
  m_pParentView->GetInvitationLowerView()->SetInputEnabled( false );
  m_pParentView->GetInvitationLowerView()->GetButtonManager()->SetInputEnable( false );

  // 選択肢ビューを有効化
  m_pParentView->GetSelectLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetSelectLowerView()->SetInputEnabled( true );
    m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( true );
  }
  m_pParentView->GetSelectLowerView()->SetStreamMessage( jf_menu_win_11 );
  m_pParentView->GetSelectLowerView()->SetVisibleSelectButton( false );
}

//--------------------------------------------------------------------------
/**
 * @brief   キャンセル送信
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenInvitationFrame::SendCancelRequest()
{
#if defined(GF_PLATFORM_CTR)
  NetAppLib::JoinFesta::JoinFestaPacketManager* pPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  JoinFestaFriendKey  targetFriendKey = m_pJoinFestaSubScreenWork->GetPacketEventFriendKey();
  NetAppLib::JoinFesta::JoinFestaPersonalData* personal = manager->GetPersonalData( targetFriendKey );

  if( personal )
  {
    if( personal->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE )
    {//交換キャンセル
      manager->GetMyData()->SetRequestRecieveTrade( targetFriendKey , personal->GetPacketTradeInvite().ticketID , false );
    }
    else if( personal->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE )
    {//対戦キャンセル
      manager->GetMyData()->SetRequestRecieveBattle( targetFriendKey , personal->GetPacketBattleInvite().ticketID , false , true, personal->GetPacketBattleInvite().battle_rule );
    }

    personal->UnLock();

    pPacketManager->SendMyData();
  }
#endif

  // 保存してあったパケットイベントリセット
  m_pJoinFestaSubScreenWork->ResetPacketEvent();
}

//--------------------------------------------------------------------------
/**
 * @brief   パケットイベントの一致チェック
 * @return  trueで一致しているのでOK
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenInvitationFrame::CheckPacketEventAgree() const
{
  // パケットイベント通知をしてきた相手のパーソナル取得
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  JoinFestaFriendKey  targetFriendKey = m_pJoinFestaSubScreenWork->GetPacketEventFriendKey();
  NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = manager->GetPersonalData( targetFriendKey );

  if( pPersonalData == NULL )
  {
    return false;
  }

  // チケットIDでチェックしているため、交換と対戦で違う場合もチェック出来ている

  if( pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE )
  {
    if( pPersonalData->IsEnablePacketTradeInvite() )
    {
      // 保存してあるチケットIDと現在のチケットIDをチェック
      if( pPersonalData->GetPacketTradeInvite().ticketID == m_pJoinFestaSubScreenWork->GetPacketEventTicketId() )
      {
        return true;
      }
    }
  }
  else if( pPersonalData->GetRequestCommand() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE )
  {
    if( pPersonalData->IsEnablePacketBattleInvite() )
    {
      // 保存してあるチケットIDと現在のチケットIDをチェック
      if( pPersonalData->GetPacketBattleInvite().ticketID == m_pJoinFestaSubScreenWork->GetPacketEventTicketId() )
      {
        return true;
      }
    }
  }

  return false;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

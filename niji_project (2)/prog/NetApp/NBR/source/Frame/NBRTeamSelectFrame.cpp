// ============================================================================
/*
 * @file NBRTeamSelectFrame.cpp
 * @brief 通信対戦受付アプリのチーム選択プロセスです。
 *        フレームID:NBR_FRAME_ID_TEAM_SELECT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRTeamSelectFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/include/NBRAppParam.h"
#include "NetApp/NBR/source/View/NBRParentView.h"

// P2P
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include "NetStatic/NetAppLib/include/BgmSelect/NetworkBattleBgmSelect.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRTeamSelectFrame::NBRTeamSelectFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
    ,m_pLowerView(NULL)
    ,m_pUpperView(NULL)
    ,m_seq( SEQ_TEAM_BUTTON_WAIT )
    ,m_isAppEndList(false)
    ,m_battleParty( pNBRWork->GetDeviceHeap() )
{
}

// コンストラクタ
NBRTeamSelectFrame::~NBRTeamSelectFrame()
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
bool NBRTeamSelectFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_TEAM_SELECT );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_REGULATION );

  // View取得
  m_pLowerView = m_pParentView->GetLowerViewTeamSelect();
  m_pUpperView = m_pParentView->GetUpperViewRegulation();

  // 表示
  SetupTopMenuButton(); // ボタンの有効無効設定
  // @fix NMCat[807]パッシブにするのにアニメ更新しないと１フレームアクティブが見える
  m_pLowerView->GetG2DUtil()->Update2D();

  m_pUpperView->SetVisible( true );

  // ===== 上画面 =========
  // レギュレーション渡す
  m_pUpperView->SetRegulationDrawInfo( m_pNBRWork->m_pSelectRegulationDrawInfo );

  // ===== 下画面 =========

  // 参加出来るチームを選んでいる
  if( m_pNBRWork->m_teamSelectReturnMode ==  TEAM_SELECT_RETURN_DECIDE )
  {
    // 選んだチームのバトルパーティ作成
    const Savedata::BOX* pBox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
    m_battleParty.CreateBattleParty( pBox, pBoxPokemon, m_pNBRWork->m_teamSelectIndex, m_pNBRWork->GetDeviceHeap() ); 

    // チーム名の表示を変える
    m_pLowerView->SetTeamName( m_battleParty.GetTeamName() );
  }
  else
  {
    // チーム名「----」
    m_pLowerView->ClearTeamName();
  }


  // バトルチーム選択アプリ戻りで、キャンセルした場合はメッセージ表示
  if( m_pNBRWork->m_teamSelectReturnMode ==  TEAM_SELECT_RETURN_CANCEL &&
      m_pNBRWork->m_isTeamSelectReturn == true )
  {
    // げんざいの　たいせんルールで　さんかできないばあいには・・・
    m_pMesageMenuView->StartMessage( msg_nbr_win_33, View::MESSAGE_FINISH_INPUT );
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );
    m_seq = SEQ_CANCEL_MESSAGE_WAIT;
  }
  else
  {
    // たいせんする　チームを　えらんでください
    m_pMesageMenuView->ShowMessage( msg_nbr_win_38, false );
    m_seq = SEQ_TEAM_BUTTON_WAIT;
  }

  // 選択BGM
  {
    // ローカル接続で、ルール選択者でない時はBGM選択出来ない
    if( m_pNet->IsWifi() == false && m_pNBRWork->IsOneselfRuleSelector() == false )
    {
      m_pLowerView->SetVisibleBgmSelect( false );
    }
    else
    {
      m_pLowerView->SetVisibleBgmSelect( true );
      m_pLowerView->ChangeBgmText( *m_pNBRWork->m_pSelectBgmIndex, m_pNBRWork->IsNijiConnect() );
    }
  }

  // 最低ウェイトの為に計測開始
  TimeOutStart();

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_CANT_PARTICIPATE );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::removeListener()
{
  m_pNet->UnregistRecvListener();
  m_pNet->ClearRecvPacketEnableCommand();
  m_pLowerView->RemoveListener();
  m_pMesageMenuView->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::updateSequence()
{
  switch( m_seq ){
  case SEQ_CANCEL_MESSAGE_WAIT:    //! バトルチーム選択アプリキャンセルした時メッセージ
    {
      // メッセージ終了待ち
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // たいせんする　チームを　えらんでください
        m_pMesageMenuView->ShowMessage( msg_nbr_win_38, false );
        // ボタンの有効無効設定
        SetupTopMenuButton();
        m_seq = SEQ_TEAM_BUTTON_WAIT;
      }
    }
    break;

  case SEQ_TEAM_BUTTON_WAIT:       //! チームボタン押下待ち
    // 各種リスナー待ち
    break;

  case SEQ_CANNOT_JOIN_LIST_SEND:  //! 参加できない送信
    {
      // 参加出来ない送信
      if( IsTimeOut() && m_pNet->SendCantParticipate() )
      {
        // たいせんルールを　えらびなおします
        m_pMesageMenuView->StartMessage( msg_nbr_win_13, View::MESSAGE_FINISH_INPUT );
        m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );
        m_seq = SEQ_CANNOT_JOIN_MESSAGE_WAIT;
      }
    }
    break;

  case SEQ_CANNOT_JOIN_MESSAGE_WAIT:           //! ストリーミングメッセージ終了待ち
    {
      // メッセージ終了待ち
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // 通信待機中
        m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
        m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::RETURN_BUTTON_ONLY_ON );
        TimeOutStart();

        // @fix GFNMCat[1750]：やり直す場合は相手から対戦開始情報を受け取っている場合があるので、通信受信バッファを削除して戻す。
        // @fix GFNMCat[5401]：同期成功時にクリアしていたが、そのタイミングだと他の本体で同期が終わっていて、本来処理すべきものまで削除されていた事があった。
        m_pNet->ClearRecvPoolBuffer();

        m_seq = SEQ_RETURN_RULE_SELECT_TIMING;
      }
    }
    break;

  case SEQ_RETURN_RULE_SELECT_TIMING:  //! ルール選択へ戻る前同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_CANT_PARTICIPATE ) )
      {
        // @fix GFNMCat[2133]：バトルチーム選択情報を初期化する
        m_pNBRWork->m_isTeamSelectReturn = false;
        m_pNBRWork->m_teamSelectReturnMode = TEAM_SELECT_RETURN_CANCEL;
        exitFrame( TeamSelectResult::TO_RULE_SELECTOR_DECIDE );
      }
    }
    break;
  default:
    GFL_ASSERT(0);
    exitFrame( TeamSelectResult::APP_END );
    break;
      // 
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRTeamSelectFrame::cleanup()
{
  // メッセージの最低表示時間経過待ち
  if( IsTimeOut() == false )
  {
    return false;
  }

  // 選択肢コマンドが出ている場合は消す
  if( m_pMesageMenuView->IsShowList() == true )
  {
    m_pMesageMenuView->HideMessage();
  }

  return true;
}


// NetRecvListener
//------------------------------------------------------------------
/**
 * @brief  通信終了通知
 */
//------------------------------------------------------------------
void NBRTeamSelectFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 下画面非表示
  m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( TeamSelectResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief  参加出来ない受信通知
 */
//------------------------------------------------------------------
void NBRTeamSelectFrame::OnNBRNetRecvCantParticipate()
{
  ICHI_PRINT("OnNBRNetRecvCantParticipate\n");
  // たいせんできないひとが　いるため　ルールを　えらびなおします
  m_pMesageMenuView->StartMessage( msg_nbr_win_12, View::MESSAGE_FINISH_INPUT );
  m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );
  m_seq = SEQ_CANNOT_JOIN_MESSAGE_WAIT;

  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
}

// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   対戦チームボタン押下通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnTeamButtonAction()
{
  ICHI_PRINT("OnTeamButtonAction\n");
  // @fix GFNMCat[1439]：参加出来ない通知を受信した場合は来る為アサート削除
  //GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );
    exitFrame( TeamSelectResult::TEAM_SELECT_APP_CALL );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   参加できないボタン押下通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnCannnotJoinButtonAction()
{
  ICHI_PRINT("OnCannnotJoinButtonAction\n");
  // @fix GFNMCat[1439]：参加出来ない通知を受信した場合は来る為アサート削除
  //GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    // 別の対戦ルールにしますか[はい・いいえ]の選択肢出す
    m_pMesageMenuView->ShowCannotJoinList();
    // カーソル非表示
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::CURSOR_OFF );
    m_isAppEndList = false;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   しょうぶかいしボタン押下通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnBattleStartButtonAction()
{
  ICHI_PRINT("OnBattleStartButtonAction\n");
  // @fix GFNMCat[1439]：参加出来ない通知を受信した場合は来る為アサート削除
  //GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    // 通信待機中
    m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::RETURN_BUTTON_ONLY_ON );
    TimeOutStart();

    // 選択BGM反映
    m_pNBRWork->m_bgm = NetApp::BgmSelect::NetworkBattleBgmSelect::GetBgmID( *m_pNBRWork->m_pSelectBgmIndex, m_pNBRWork->IsNijiConnect() );

    exitFrame( TeamSelectResult::TO_TEAM_SELECT_WAIT );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   BGM選択ボタン押下通知（右左）
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnBgmLButtonAction()
{
  ICHI_PRINT("OnBgmLButtonAction\n");
  // @fix GFNMCat[1439]：参加出来ない通知を受信した場合は来る為アサート削除
  //GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    (*m_pNBRWork->m_pSelectBgmIndex) -= 1;
    if( *m_pNBRWork->m_pSelectBgmIndex < 0 ){ *m_pNBRWork->m_pSelectBgmIndex = NetApp::BgmSelect::NetworkBattleBgmSelect::GetMaxBgmIndex( m_pNBRWork->IsNijiConnect() ); }
    m_pLowerView->ChangeBgmText( *m_pNBRWork->m_pSelectBgmIndex, m_pNBRWork->IsNijiConnect() );
    ICHI_PRINT(">ICHI ChangeBgm %d \n", *m_pNBRWork->m_pSelectBgmIndex);
  }
}

void NBRTeamSelectFrame::OnBgmRButtonAction()
{
  ICHI_PRINT("OnBgmRButtonAction\n");
  // @fix GFNMCat[1439]：参加出来ない通知を受信した場合は来る為アサート削除
  //GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    (*m_pNBRWork->m_pSelectBgmIndex) += 1;
    if( *m_pNBRWork->m_pSelectBgmIndex > NetApp::BgmSelect::NetworkBattleBgmSelect::GetMaxBgmIndex( m_pNBRWork->IsNijiConnect() ) ){ *m_pNBRWork->m_pSelectBgmIndex = 0; }
    m_pLowerView->ChangeBgmText( *m_pNBRWork->m_pSelectBgmIndex, m_pNBRWork->IsNijiConnect() );
    ICHI_PRINT(">ICHI ChangeBgm %d \n", *m_pNBRWork->m_pSelectBgmIndex);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  switch( m_seq ){
  case SEQ_TEAM_BUTTON_WAIT:       //! チームボタン押下待ち
  case SEQ_CANNOT_JOIN_LIST_SEND:  //! 参加できない送信
  case SEQ_RETURN_RULE_SELECT_TIMING:  //! ルール選択へ戻る前同期
    {
      // やめますか[はい・いいえ]の選択肢出す
      m_pMesageMenuView->ShowAppEndList();
      // カーソル非表示
      m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::CURSOR_OFF );
      m_isAppEndList = true;
    }
    return;

  case SEQ_CANNOT_JOIN_MESSAGE_WAIT: //! ストリーミングメッセージ終了待ち
    // 通信受信で来るので無視
    return;
  case SEQ_CANCEL_MESSAGE_WAIT:    //! バトルチーム選択アプリキャンセルした時メッセージ
  default:
    GFL_ASSERT_MSG(0,"%d", m_seq );
    return;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");

  if( m_isAppEndList == true )
  {// アプリ終了選択肢の場合

    // 画面を終了時のメッセージにしておく
    m_pMesageMenuView->ShowAppEndMessage();
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_OFF );

    // 終了フレームへ
    m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
    exitFrame( TeamSelectResult::APP_END );
  }
  else
  {// 別の対戦ルール選択肢
    // この　対戦ルールに　参加しないことを　送っています
    m_pMesageMenuView->ShowMessage( msg_nbr_win_28, true );
    TimeOutStart();
    // 戻るボタンのみ表示
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::RETURN_BUTTON_ONLY_ON );
    m_seq = SEQ_CANNOT_JOIN_LIST_SEND;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  // 同期待ちはメッセージが違う
  if( m_seq == SEQ_RETURN_RULE_SELECT_TIMING )
  {
    // 通信待機中
    m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
  }
  else if( m_seq == SEQ_CANNOT_JOIN_LIST_SEND )
  {
    // この　対戦ルールに　参加しないことを　送っています
    m_pMesageMenuView->ShowMessage( msg_nbr_win_28, true );
  }
  else
  {
    // たいせんする　チームを　えらんでください
    m_pMesageMenuView->ShowMessage( msg_nbr_win_38, false );
    // カーソル表示
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::CURSOR_ON );
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   トップメニューのボタン有効・無効設定
 */
//--------------------------------------------------------------------------
void NBRTeamSelectFrame::SetupTopMenuButton()
{
  m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::ALL_VIEW_ON );

  // 参加出来るチームを選んでいる
  //  → しょうぶかいしをアクティブ。さんかできないをパッシブ
  if( m_pNBRWork->m_teamSelectReturnMode ==  TEAM_SELECT_RETURN_DECIDE )
  {
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::CANNOT_JOIN_BUTTON_PASSIVE );
  }
  // 参加出来るチームを選んでいない。　
  //  → しょうぶかいしをパッシブ。さんかできないをアクティブ
  else
  {
    m_pLowerView->SetVisible( View::NBRLowerViewTeamSelect::BATTLE_START_BUTTON_PASSIVE );
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

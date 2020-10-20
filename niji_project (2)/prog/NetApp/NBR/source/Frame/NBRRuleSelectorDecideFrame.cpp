// ============================================================================
/*
 * @file NBRRuleSelectorDecideFrame.cpp
 * @brief 通信対戦受付アプリのルール選択する人を決定プロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECTOR_DECIDE
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectorDecideFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectorDecideFrame::NBRRuleSelectorDecideFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pLowerView( NULL ),
  m_pUpperView( NULL )
{
}

// コンストラクタ
NBRRuleSelectorDecideFrame::~NBRRuleSelectorDecideFrame()
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
bool NBRRuleSelectorDecideFrame::startup()
{
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_2BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_SIMPLE );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerView2Button();
  m_pUpperView = m_pParentView->GetUpperViewPlayerInfoSimple();

  // 表示
  m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
  m_pUpperView->SetAllVisible(true);

  // ===== 上画面 =========
  SetupUpperViewPlayerInfoSimple();

  // ===== 下画面 =========
  // ボタンのテキスト （自分が決める／他の人にお任せ）
  m_pLowerView->SetButtonText( View::NBRLowerView2Button::BUTTON_TEXT_MODE_WHO_DECIDE );

  // 通信待機中です
  m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
  TimeOutStart();

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_RULE_SELECTOR );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_SELECTOR_SELF );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::removeListener()
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
void NBRRuleSelectorDecideFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_RULE_SELECTOR_SEND:       //! ルール選択者送信
    {
      // ルール選択者なら、その旨送信する。
      // →ここで送信するのは、ルール選択者が変わる時に、ここに戻ってくる場合がある為。
      //   ここに戻ってくるのは、他アプリ起動後だったりするので、貰わないと整合性が合わない。
      if( IsRuleSelector() == true )
      {
        if( m_pNet->SendRuleSelectorIsMe() )
        {
          SetSubSeq( SEQ_RULE_SELECTOR_TIMING_LOOP );
          GFL_RELEASE_PRINT(">NBR SEQ_RULE_SELECTOR_TIMING_LOOP\n");
        }
      }
      // ルール選択者以外は、受信リスナーで待つ
    }
    break;

  case SEQ_RULE_SELECTOR_TIMING_LOOP://! ルール選択者決定同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_RULE_SELECTOR ) )
      {
        SetSubSeq( SEQ_MESSAGE_WAIT );
        GFL_RELEASE_PRINT(">NBR SEQ_MESSAGE_WAIT\n");
      }
    }
    break;

  case SEQ_MESSAGE_WAIT:       //! 最低ウェイト
    {
      // メッセージ最低ウェイト表示
      if( IsTimeOut() )
      {
        SetSubSeq( SEQ_FIRST );
        GFL_RELEASE_PRINT(">NBR SEQ_FIRST\n");
      }
    }
    break;

  case SEQ_FIRST:                //! 初期シーケンス
    {
      // 自分がルール選ぶ人を決めるのか？
      int nextSeq = (IsRuleSelector() == true ) ? SEQ_TUTORIAL : SEQ_RECV_WAIT; 
      SetSubSeq( nextSeq );

      // 受信待ちの場合
      if( nextSeq == SEQ_RECV_WAIT )
      {
        // 決める人を選んでいます。しばらくお待ちください。
        m_pMesageMenuView->ShowMessage( msg_nbr_win_05, true );
        // 戻るボタンのみ表示
        m_pLowerView->SetVisible( View::NBRLowerView2Button::RETURN_BUTTON_ONLY_ON );
      }
    }
    break;

  case SEQ_RECV_WAIT:            //! ルール選択者受信待ち
    // リスナー待ち
    break;

  case SEQ_TUTORIAL:             //! ルール選ぶ人への説明
    {
      if( m_pNBRWork->m_isEndRuleSelectorTutorial == false )
      { // 初めてだったら
        m_pNBRWork->m_isEndRuleSelectorTutorial = true;
        m_pMesageMenuView->StartMessageRuleSelectorTutorial( m_pNet->GetConnectionNum(), m_pNBRWork->IsMutli() );
        SetSubSeq( SEQ_TUTORIAL_WAIT );
      }
      else
      {
        // ○○さんが対戦ルールを決めますか？
        m_pMesageMenuView->ShowMessageMyStatus( msg_nbr_win_34, m_pNBRWork->GetMineMyStatus(), false );
        // ボタン表示
        m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_ON );
        SetSubSeq( SEQ_RULE_SELECTOR_WAIT );
      }
    }
    break;

  case SEQ_TUTORIAL_WAIT:        //! ルール選ぶ人への説明待ち
    {
      // メッセージ終了待ち
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // ○○さんが対戦ルールを決めますか？
        m_pMesageMenuView->ShowMessageMyStatus( msg_nbr_win_34, m_pNBRWork->GetMineMyStatus(), false );
        // ボタン表示
        m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_ON );
        SetSubSeq( SEQ_RULE_SELECTOR_WAIT );
      }
    }
    break;

  case SEQ_RULE_SELECTOR_WAIT:   //! ルール選ぶ人ボタン押下待ち
    // ボタン押下リスナー待ち
    break;
  case SEQ_TIMING_LOOP:          //! 同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_RULE_SELECTOR_END ) )
      {
        // ルール決める人だとアプリに飛ばす。
        NetAppLib::System::FrameResult result;
        result = IsRuleSelector() ? RuleSelectorDecideResult::TO_RULE_SELECT_APP_CALL : RuleSelectorDecideResult::TO_RULE_SELECT_WAIT;
        exitFrame( result );

        // 下画面は非表示しておく
        m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
      }
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRRuleSelectorDecideFrame::cleanup()
{
  // 最低秒数待つ
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
void NBRRuleSelectorDecideFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  // 下画面は非表示しておく
  m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( RuleSelectorDecideResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief     ルール選択者通知
 * @param[in] ルール選択者通知コマンド
 */
//------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::OnNBRNetRecvRuleSelector( const NBRNetDefine::RuleSelectorCommand command )
{
  GFL_RELEASE_PRINT("OnNBRNetRecvRuleSelector command %d\n", command);
  switch( command ){
  case NBRNetDefine::RULE_SELECTOR_COMMAND_MINE:     //! 自分が決める
    {
      // 今のルール選択者で決定
      SetSubSeq( SEQ_TIMING_LOOP );
    }
    break;
  case NBRNetDefine::RULE_SELECTOR_COMMAND_OTHER:    //! 他の人にお任せする
    {
      // 次の優先順位の人に変更
      m_pNBRWork->RuleSelectorChangeNextPriority( m_pNet->GetConnectionNum() );
      // 最初のシーケンスに戻る
      SetSubSeq( SEQ_FIRST );
    }
    break;
  }
}

//------------------------------------------------------------------
/**
 * @brief  ルール選択者が送信者になった通知
 * @param[in]   netId        送信者のNetId
 */
//------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::OnNBRNetRecvRuleSelectorIsMe( int netId )
{
  GFL_RELEASE_PRINT("OnNBRNetRecvRuleSelectorIsMe command \n");
  ICHI_PRINT("ルール選択者のNetId  %d\n", netId);
  *m_pNBRWork->m_pRuleSelectorNetId = netId;
  SetSubSeq( SEQ_RULE_SELECTOR_TIMING_LOOP );
}

// NBRLowerViewReturnButtonListener
//------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::On2ButtonAction( int buttonId )
{
  ICHI_PRINT("On2ButtonAction buttonId %d\n", buttonId);
  // ボタン受付シーケンス以外は無視
  if( GetSubSeq() != SEQ_RULE_SELECTOR_WAIT )
  {
    return;
  }

  switch( buttonId ){
  case View::NBRLowerView2Button::BUTTON_ID_MYSELF: // 自分が決める
    {
      // ルール選択者通知送信
      if( m_pNet->SendRuleSelector( NBRNetDefine::RULE_SELECTOR_COMMAND_MINE ) )
      {
        // @fix NMCat[896]：自分が決めた際は戻るボタン非表示
        m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
        // 通信待機中です
        m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
        TimeOutStart();
        SetSubSeq( SEQ_TIMING_LOOP );
      }
    }
    break;
  case View::NBRLowerView2Button::BUTTON_ID_OTHERS: // 他の人におまかせ
    {
      // ルール選択者通知送信
      if( m_pNet->SendRuleSelector( NBRNetDefine::RULE_SELECTOR_COMMAND_OTHER ) )
      {
        // 戻るボタンのみ表示
        m_pLowerView->SetVisible( View::NBRLowerView2Button::RETURN_BUTTON_ONLY_ON );
        // 次の優先順位の人に変更
        m_pNBRWork->RuleSelectorChangeNextPriority( m_pNet->GetConnectionNum() );

        // 最初のシーケンスに戻る
        SetSubSeq( SEQ_FIRST );
      }
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  switch( GetSubSeq() ){
  case SEQ_RECV_WAIT:            //! ルール選択者受信待ち
  case SEQ_RULE_SELECTOR_WAIT:   //! ルール選ぶ人ボタン押下待ち
  case SEQ_TIMING_LOOP:          //! 同期待ち
    // やめますか[はい・いいえ]の選択肢出す
    m_pMesageMenuView->ShowAppEndList();
    // カーソル非表示
    m_pLowerView->SetVisible( View::NBRLowerView2Button::CURSOR_OFF );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  // 必要ないので非表示
  m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
  m_pUpperView->SetAllVisible(false);

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( RuleSelectorDecideResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorDecideFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  if( IsRuleSelector() == true )
  {
    // ○○さんが対戦ルールを決めますか？
    m_pMesageMenuView->ShowMessageMyStatus( msg_nbr_win_34, m_pNBRWork->GetMineMyStatus(), false );
    // カーソル表示
    m_pLowerView->SetVisible( View::NBRLowerView2Button::CURSOR_ON );
  }
  else
  {
    // 決める人を選んでいます。しばらくお待ちください。
    m_pMesageMenuView->ShowMessage( msg_nbr_win_05, true );
  }
}

/**
 * @brief  自分がルール選ぶ人を決めるのか？
 * @return trueで自分が決める
 */
bool NBRRuleSelectorDecideFrame::IsRuleSelector() const
{
  return m_pNBRWork->IsOneselfRuleSelector();
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

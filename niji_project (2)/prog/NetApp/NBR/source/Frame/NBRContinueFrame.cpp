// ============================================================================
/*
 * @file NBRContinueFrame.cpp
 * @brief 通信対戦受付アプリの続けて遊ぶプロセスです。
 *        フレームID:NBR_FRAME_ID_CONTINUE
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRContinueFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRContinueFrame::NBRContinueFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pLowerView(NULL),
  m_pUpperView(NULL),
  m_recvContinueCommand(),
  m_recvCount(0)
{
}

// コンストラクタ
NBRContinueFrame::~NBRContinueFrame()
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
bool NBRContinueFrame::startup()
{
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_2BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_SIMPLE );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerView2Button();
  m_pUpperView = m_pParentView->GetUpperViewPlayerInfoSimple();

  // 表示
  m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_ON );
  m_pUpperView->SetAllVisible(true);

  // ===== 上画面 =========
  SetupUpperViewPlayerInfoSimple();

  // ===== 下画面 =========
  // ボタンのテキスト （「同じルールで遊ぶ/ルールを変えて遊ぶ」）
  m_pLowerView->SetButtonText( View::NBRLowerView2Button::BUTTON_TEXT_MODE_RULE );

  // つづけて　あそびますか？
  m_pMesageMenuView->ShowMessage( msg_nbr_win_20, false );

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRContinueFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_CONTINUE );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRContinueFrame::removeListener()
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
void NBRContinueFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_BUTTON_WAIT:          //! ボタン押下待ち
    // リスナー待ち
    break;

  case SEQ_RULE_CHANGE_SEND:     //! ルールを変えて遊ぶ送信
    {
      if( m_pNet->SendContinue( NBRNetDefine::CONTINUE_COMMAND_CHANGE ) )
      {
        SetSubSeq( SEQ_RECV_WAIT );
      }
    }
    break;

  case SEQ_RULE_NOT_CHANGE_SEND: //! 同じルールで遊ぶ送信
    {
      if( m_pNet->SendContinue( NBRNetDefine::CONTINUE_COMMAND_NOT_CHANGE ) )
      {
        SetSubSeq( SEQ_RECV_WAIT );
      }
    }
    break;

  case SEQ_RECV_WAIT:            //! コマンド受信待ち
    {
      if( IsRecvCommandFinish() )
      {
        SetSubSeq( SEQ_TIMING_LOOP );
      }
    }
    break;

  case SEQ_TIMING_LOOP:          //! 同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_CONTINUE ) )
      {
        ShowGotoNextMessage();
        SetSubSeq( SEQ_GOTO_WAIT );
      }
    }
    break;

  case SEQ_GOTO_WAIT:            //! 次フレーム行きへのメッセージ表示待ち
    {
      if( IsTimeOut() )
      {
        exitFrame( GetNextFrame() );
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
bool NBRContinueFrame::cleanup()
{
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
void NBRContinueFrame::OnNBRNetRecvEnd()
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
  exitFrame( ContinueResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief     続けて遊ぶ受信結果通知
 * @param[in] 続けて遊ぶ受信結果通知コマンド
 */
//------------------------------------------------------------------
void NBRContinueFrame::OnNBRNetRecvContinue( const NBRNetDefine::ContinueCommand command )
{
  ICHI_PRINT("OnNBRNetRecvContinue commando %d\n", command);
  m_recvContinueCommand[ m_recvCount++ ] = command;
}

// NBRLowerViewReturnButtonListener
//------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//------------------------------------------------------------------
void NBRContinueFrame::On2ButtonAction( int buttonId )
{
  ICHI_PRINT("On2ButtonAction buttonId %d\n", buttonId);
  // ボタン受付シーケンス以外は無視
  if( GetSubSeq() != SEQ_BUTTON_WAIT )
  {
    GFL_ASSERT(0);
    return;
  }

  // ボタンIDが想定外も無視
  if( !(buttonId == View::NBRLowerView2Button::BUTTON_ID_CONTINUE_SAME_RULE ||
        buttonId == View::NBRLowerView2Button::BUTTON_ID_CONTINUE_DIFF_RULE ) )
  {
    GFL_ASSERT(0);
    return;
  }

  // あいてを待っています
  m_pMesageMenuView->ShowMessage( msg_nbr_win_22, true );
  // 戻るボタンのみ表示
  m_pLowerView->SetVisible( View::NBRLowerView2Button::RETURN_BUTTON_ONLY_ON );

  // コマンド送信シーケンスへ
  int nextSeq = (buttonId == View::NBRLowerView2Button::BUTTON_ID_CONTINUE_SAME_RULE) ? SEQ_RULE_NOT_CHANGE_SEND : SEQ_RULE_CHANGE_SEND; 
  SetSubSeq( nextSeq );

  TimeOutStart();
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRContinueFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  switch( GetSubSeq() ){
  case SEQ_BUTTON_WAIT:          //! ボタン押下待ち
  case SEQ_RULE_CHANGE_SEND:     //! ルールを変えて遊ぶ送信
  case SEQ_RULE_NOT_CHANGE_SEND: //! 同じルールで遊ぶ送信
  case SEQ_RECV_WAIT:            //! コマンド受信待ち
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
void NBRContinueFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  // 必要ないので非表示
  m_pLowerView->SetVisible( View::NBRLowerView2Button::ALL_VIEW_OFF );
  m_pUpperView->SetAllVisible(false);

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( ContinueResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRContinueFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  switch( GetSubSeq() ){
  case SEQ_BUTTON_WAIT:          //! ボタン押下待ち
    // つづけて　あそびますか？
    m_pMesageMenuView->ShowMessage( msg_nbr_win_20, false );
    // カーソル表示
    m_pLowerView->SetVisible( View::NBRLowerView2Button::CURSOR_ON );
    break;

  case SEQ_RULE_CHANGE_SEND:     //! ルールを変えて遊ぶ送信
  case SEQ_RULE_NOT_CHANGE_SEND: //! 同じルールで遊ぶ送信
  case SEQ_RECV_WAIT:            //! コマンド受信待ち
  case SEQ_TIMING_LOOP:          //! 同期待ち
    // あいてを待っています
    m_pMesageMenuView->ShowMessage( msg_nbr_win_22, true );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   全員のコマンド受信完了したか？
 * @return  trueで受信完了
 */
//--------------------------------------------------------------------------
bool NBRContinueFrame::IsRecvCommandFinish() const
{
  return ( m_pNet->GetConnectionNum() == m_recvCount );
}

//--------------------------------------------------------------------------
/**
 * @brief   次のフレームを取得
 * @return  ContinueResult
 */
//--------------------------------------------------------------------------
NetAppLib::System::FrameResult NBRContinueFrame::GetNextFrame() const
{
  // 一人でも変えてがいたらルール変更に行く
  for( int i=0; i<m_recvCount; i++ )
  {
    if( m_recvContinueCommand[i] == NBRNetDefine::CONTINUE_COMMAND_CHANGE )
    {
      return ContinueResult::TO_RULE_SELECTOR_DECIDE;
    }
  }
  return ContinueResult::TO_TEAM_SELECT;
}

//--------------------------------------------------------------------------
/**
 * @brief   次のフレームへ行くときのメッセージ表示
 */
//--------------------------------------------------------------------------
void NBRContinueFrame::ShowGotoNextMessage() const
{
  if( GetNextFrame() == ContinueResult::TO_RULE_SELECTOR_DECIDE )
  {
    // ルールをかえて　もういちど　たいせんします
    m_pMesageMenuView->ShowMessage( msg_nbr_win_25, true );
  }
  else
  {
    // ルールをかえずに　もういちど　たいせんします
    m_pMesageMenuView->ShowMessage( msg_nbr_win_26, true );
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

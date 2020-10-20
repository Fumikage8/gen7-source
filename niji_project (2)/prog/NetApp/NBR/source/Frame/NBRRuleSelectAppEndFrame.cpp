// ============================================================================
/*
 * @file NBRRuleSelectAppEndFrame.cpp
 * @brief 通信対戦受付アプリのルール選択アプリ戻りプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_APP_END
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectAppEndFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "Battle/Regulation/include/Regulation.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectAppEndFrame::NBRRuleSelectAppEndFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pLowerView( NULL ),
  m_pUpperView( NULL ),
  m_recvRegulationRetCount( 0 ),
  m_regulationRetCommand( NBRNetDefine::REGULATION_RET_COMMAND_OK ) // 初期値OK変えない
{
}

// コンストラクタ
NBRRuleSelectAppEndFrame::~NBRRuleSelectAppEndFrame()
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
bool NBRRuleSelectAppEndFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_REGULATION );

  // View取得
  m_pLowerView = m_pParentView->GetLowerViewReturnButton();
  m_pUpperView = m_pParentView->GetUpperViewRegulation();

  // 表示
  m_pLowerView->SetAllVisible( true );
  m_pUpperView->SetVisible( true );

  // ===== 上画面 =========
  // レギュレーション渡す
  m_pUpperView->SetRegulationDrawInfo( m_pNBRWork->m_pSelectRegulationDrawInfo );

  // ===== 下画面 =========
  // ルールを　おくっています
  m_pMesageMenuView->ShowMessage( msg_nbr_win_07, true );

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
void NBRRuleSelectAppEndFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_REGULATION_D );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_REGULATION_RET );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndFrame::removeListener()
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
void NBRRuleSelectAppEndFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_REGULATION_READY_SEND:  //! レギュレーション準備送信
    {
      // プリセットなのか配信なのか
      NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo = m_pNBRWork->m_pSelectRegulationDrawInfo;

      // 送信結果
      if( m_pNet->SendRegulationReady( pSelectRegulationDrawInfo->IsPreset() ) )
      {
        // 受信結果待ちへ
        SetSubSeq( SEQ_REGULATION_SEND_TIMING );
      }
    }
    break;

  case SEQ_REGULATION_SEND_TIMING: //! レギュレーション送信前同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_REGULATION_SEND ) )
      {
        SetSubSeq( SEQ_REGULATION_SEND );
      }
    }
    break;

  case SEQ_REGULATION_SEND:  //! レギュレーション送信
    {
      bool isSendSuccess = false;

      // プリセットなのか配信なのか
      NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo = m_pNBRWork->m_pSelectRegulationDrawInfo;
      if( pSelectRegulationDrawInfo->IsPreset() )
      {
        isSendSuccess = m_pNet->SendRegulation( pSelectRegulationDrawInfo->GetPreset() );
      }
      else
      {
        u8 certData[] = {0,1,2,3};
        isSendSuccess = m_pNet->SendRegulation( pSelectRegulationDrawInfo->GetRegulation(), certData, 4 );
      }

      // 送信結果
      if( isSendSuccess )
      {
        // 受信結果待ちへ
        SetSubSeq( SEQ_RET_RECV_WAIT );
      }
    }
    break;
  case SEQ_RET_RECV_WAIT:    //! レギュレーション受信結果通知待ち
    {
      // 受信リスナー待ち
#ifdef  GF_PLATFORM_WIN32
#if PM_DEBUG
      GFL_PRINT("NBR> Win32なのでSkip SEQ_RET_RECV_WAIT\n");
      SetSubSeq( SEQ_TO_BATTLE_TEAM_SELECT );
#endif
#endif

      // @fix GFNMCat[2144]：受信リスナーでシーケンス上書きされないように修正
      // 自分のデータは無視するので-1
      if( m_recvRegulationRetCount == m_pNet->GetConnectionNum() - 1 )
      {
        SetSubSeq( SEQ_TO_NEXT_TIMING );
      }
    }
    break;

  case SEQ_TO_NEXT_TIMING:   //! 次へ進む前の同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_REGULATION_RET ) )
      {

        // レギュレーション送信結果から次の行き先決める
        switch( m_regulationRetCommand ){
        case NBRNetDefine::REGULATION_RET_COMMAND_OK:        //! 問題なし
          // バトルチーム選択同期へ
          SetSubSeq( SEQ_TO_BATTLE_TEAM_SELECT );
          break;
        case NBRNetDefine::REGULATION_RET_COMMAND_INVALID:   //! 不正なデータ
          // エラーメッセージ
          m_pMesageMenuView->StartMessage( msg_nbr_win_09, View::MESSAGE_FINISH_INPUT );
          // 戻るボタン非表示
          m_pLowerView->SetAllVisible( false );
          SetSubSeq( SEQ_MESSAGE_WAIT );
          break;
        case NBRNetDefine::REGULATION_RET_COMMAND_BROKEN:    //! 壊れていた
          // エラーメッセージ
          m_pMesageMenuView->StartMessage( msg_nbr_win_08, View::MESSAGE_FINISH_INPUT );
          // 戻るボタン非表示
          m_pLowerView->SetAllVisible( false );
          SetSubSeq( SEQ_MESSAGE_WAIT );
          break;
        default:
          GFL_ASSERT(0);
          break;
        }

      }
    }
    break;

  case SEQ_MESSAGE_WAIT:     //! ストリーミングメッセージ終了待ち
    {
      // メッセージ終了待ち
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // ルール選択アプリに戻る
        exitFrame( RuleSelectAppEndResult::TO_RULE_SELECT_APP_CALL );
      }
    }
    break;
  case SEQ_TO_BATTLE_TEAM_SELECT: //! バトルチーム選択へ
    {
      exitFrame( RuleSelectAppEndResult::TO_TEAM_SELECT );
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
bool NBRRuleSelectAppEndFrame::cleanup()
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
void NBRRuleSelectAppEndFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 下画面非表示
  m_pLowerView->SetAllVisible( false );
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( RuleSelectAppEndResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief  レギュレーション受信通知
 * @param[in]   netId               送信者のNetId
 * @param[in]   pRegulationBuffer   レギュレーションデータ
 * @param[in]   regulationSize      レギュレーションデータのサイズ
 * @param[in]   pCerdData           署名データ
 * @param[in]   cerdDataSize        署名データサイズ
 */
//------------------------------------------------------------------
void NBRRuleSelectAppEndFrame::OnNBRNetRecvRegulation( const int netId,
                                                       const void* pRegulationBuffer, const u32 regulationSize,
                                                       const void* pCerdData, const u32 cerdDataSize )
{
  ICHI_PRINT("OnNBRNetRecvRegulationD\n");
  // プログラムの都合上受信するが、必要ないので捨てる。
}

/**
 * @brief     レギュレーション受信結果通知
 * @param[in] command         レギュレーション受信結果通知コマンド
 */
void NBRRuleSelectAppEndFrame::OnNBRNetRecvRegulationRet( const NBRNetDefine::RegulationRetCommand command )
{
  ICHI_PRINT("OnNBRNetRecvRegulationRet %d \n", command );

  m_recvRegulationRetCount++;

  // 失敗の結果で上書きする
  if( command != NBRNetDefine::REGULATION_RET_COMMAND_OK )
  {
    m_regulationRetCommand = command;
  }
}

// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  // ユーザー待ちの時だけ許可
  switch( GetSubSeq() ){
  case SEQ_REGULATION_SEND_TIMING: //! レギュレーション送信前同期
  case SEQ_RET_RECV_WAIT:    //! レギュレーション受信結果通知待ち
  case SEQ_TO_NEXT_TIMING:   //! 次へ進む前の同期
    // やめますか[はい・いいえ]の選択肢出す
    m_pMesageMenuView->ShowAppEndList();
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  m_pLowerView->SetAllVisible( false );

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( RuleSelectAppEndResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectAppEndFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  // 画面戻す
  // ルールを　おくっています
  m_pMesageMenuView->ShowMessage( msg_nbr_win_07, true );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

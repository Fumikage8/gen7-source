// ============================================================================
/*
 * @file NBRRuleSelectWaitFrame.cpp
 * @brief 通信対戦受付アプリのルール選択待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectWaitFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"

// Regulation
#include "Battle/Regulation/include/Regulation.h"
#include "Battle/Regulation/include/RegulationScriptChecker.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectWaitFrame::NBRRuleSelectWaitFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pLowerView(NULL),
  m_pUpperView(NULL),
  m_isPreset(false),
  m_isRecvRegulationReady(false),
  m_isRecvRegulation(false),
  m_sendRegulationRetCommand(NBRNetDefine::REGULATION_RET_COMMAND_OK),
  m_recvRegulationRetCount(0)
{
}

// コンストラクタ
NBRRuleSelectWaitFrame::~NBRRuleSelectWaitFrame()
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
bool NBRRuleSelectWaitFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_SIMPLE );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerViewReturnButton();
  m_pUpperView = m_pParentView->GetUpperViewPlayerInfoSimple();

  // 表示
  m_pLowerView->SetAllVisible( true );
  m_pUpperView->SetAllVisible( true );

  // ===== 上画面 =========
  SetupUpperViewPlayerInfoSimple();

  // ===== 下画面 =========
  // 待機中メッセージ
  this->ShowWaitMessage();

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
void NBRRuleSelectWaitFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_RULE_OTHER );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_REGULATION_READY );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_REGULATION_P );
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
void NBRRuleSelectWaitFrame::removeListener()
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
void NBRRuleSelectWaitFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_REGULATION_READY_RECV_WAIT://! レギュレーション準備受信待ち
    {
      // リスナー待ち
      if( m_isRecvRegulationReady == true )
      {
        m_isRecvRegulationReady = false;

        // @fix GFNMCat[5388]：同期成功時にクリアしていたが、そのタイミングだと他の本体で同期が終わっていて、
        //      通信コマンドを送ってくるので受信カウントが増えている。その為クリアすると受信カウンタが足りず進まない不具合になる。
        // レギュレーション結果受信カウント初期化
        m_recvRegulationRetCount = 0;
        m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_OK;

        // レギュレーション送信前同期へ
        SetSubSeq( SEQ_REGULATION_SEND_TIMING );
      }
    }
    break;

  case SEQ_REGULATION_SEND_TIMING: //! レギュレーション送信前同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_REGULATION_SEND ) )
      {
        // プリセットなら受信待ち。配信ならばダミーで送信する必要がある。
        int nextSeq = m_isPreset ? SEQ_REGULATION_RECV_WAIT : SEQ_DOWNLOAD_REGULATION_SEND;  
        SetSubSeq( nextSeq );
      }
    }
    break;

  case SEQ_REGULATION_RECV_WAIT:     //! レギュレーション受信待ち
    {
      // レギュレーション受信リスナー待ち
      if( m_isRecvRegulation == true )
      {
        m_isRecvRegulation = false;
        SetSubSeq( SEQ_REGULATION_RET_SEND );
      }
    }
    break;

  case SEQ_DOWNLOAD_REGULATION_SEND: //! 配信レギュレーション送信
    {
      // どうせ捨てるのでデータは何でもいい
      Regulation dummyRegulation( m_pNBRWork->GetDeviceHeap() );
      u8 dummyData[ Regulation::FIX_CORE_DATA_SIZE ] = {0};
      dummyRegulation.SetData( &dummyData, Regulation::FIX_CORE_DATA_SIZE );
      u8 certData[4] = {0};
      if( m_pNet->SendRegulation( dummyRegulation, certData, 4 ) )
      {
        SetSubSeq( SEQ_REGULATION_RECV_WAIT );
      }
    }
    break;

  case SEQ_REGULATION_RET_SEND:      //! レギュレーション受信結果送信
    {
      // 送信成功待ち
      if( m_pNet->SendRegulationRet( m_sendRegulationRetCommand ) )
      {
        SetSubSeq( SEQ_TO_NEXT_TIMING );
      }
    }
    break;

  case SEQ_TO_NEXT_TIMING:           //! 次へ進む前の同期
    {
      // 4人の場合、他の参加者の結果を待つ
      if( m_pNet->GetConnectionNum() == NBRNetDefine::NET_NUM_MAX )
      {
        // 自分とホストは送信しないので、カウント済みにしておく
        if( (m_recvRegulationRetCount+2) < m_pNet->GetConnectionNum() )
        {
          break;
        }
      }

      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_REGULATION_RET ) )
      {
        switch( m_sendRegulationRetCommand ){
        case NBRNetDefine::REGULATION_RET_COMMAND_OK:
          {
            // バトルチーム選択へ
            SetSubSeq( SEQ_TO_BATTLE_TEAM_SELECT );
          }
          break;
        case NBRNetDefine::REGULATION_RET_COMMAND_INVALID:
          {
            // うけとった　たいせんルールに　もんだいがあります
            StartRegulationErrorMessage( msg_nbr_win_10 );
          }
          break;
        case NBRNetDefine::REGULATION_RET_COMMAND_BROKEN:
          {
            // うけとった　たいせんルールが　こわれています
            StartRegulationErrorMessage( msg_nbr_win_11 );
          }
          break;
        default:
          {
            GFL_ASSERT(0);
            // うけとった　たいせんルールに　もんだいがあります
            StartRegulationErrorMessage( msg_nbr_win_10 );
          }
          break;
        }
      }
    }
    break;

  case SEQ_RULE_OTHER_MESSAGE_WAIT:  //! ルール決める人やめたメッセージ終了待ち
    {
      if( IsTimeOut() )
      {
        // 優先度を次の人に更新する
        m_pNBRWork->RuleSelectorChangeNextPriority( m_pNet->GetConnectionNum() );

        // ルール選択者決めまで戻る
        exitFrame( RuleSelectWaitResult::TO_RULE_SELECTOR_DECIDE );
      }
    }
    break;

  case SEQ_REG_ERROR_MESSAGE_WAIT:   //! レギュレーションエラーメッセージ終了待ち
    {
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // またレギュレーション送信待ちへ戻る
        this->ShowWaitMessage();
        SetSubSeq( SEQ_REGULATION_READY_RECV_WAIT );
      }
    }
    break;

  case SEQ_TO_BATTLE_TEAM_SELECT: //! バトルチーム選択へ
    {
      // 戻るボタン非表示
      m_pLowerView->SetAllVisible( false );
      // 選択肢表示中なら消す
      if( m_pMesageMenuView->IsShowList() )
      {
        m_pMesageMenuView->HideMessage();
      }
      // チーム選択へ進む
      exitFrame( RuleSelectWaitResult::TO_TEAM_SELECT );
    }
    break;
  default:
    GFL_ASSERT(0);
    exitFrame( RuleSelectWaitResult::APP_END );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRRuleSelectWaitFrame::cleanup()
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
void NBRRuleSelectWaitFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 戻るボタン非表示
  m_pLowerView->SetAllVisible( false );
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( RuleSelectWaitResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief  ルール選択を他の人に任せる通知
 */
//------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnNBRNetRecvRuleOther()
{
  ICHI_PRINT("OnNBRNetRecvRuleOther\n");
  Savedata::MyStatus* myStatus = m_pNBRWork->GetMyStatusFromNetId( *m_pNBRWork->m_pRuleSelectorNetId );
  // ○○さんが　ルールをきめるのを　やめました
  m_pMesageMenuView->ShowMessageMyStatus( msg_nbr_win_37, myStatus, true );
  // 1.5秒で自動送り
  TimeOutStart();
  // 戻るボタン非表示
  m_pLowerView->SetAllVisible( false );
  // メッセージ終了待ちへ
  SetSubSeq( SEQ_RULE_OTHER_MESSAGE_WAIT );
}

//------------------------------------------------------------------
/**
 * @brief  レギュレーション準備受信通知
 * @param[in]   isPreset    送信しようとしているレギュレーションがプリセットならtrue
 */
//------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnNBRNetRecvRegulationReady( bool isPreset )
{
  ICHI_PRINT("OnNBRNetRecvRegulationReady %d \n", isPreset);
  m_isPreset = isPreset;
  m_isRecvRegulationReady = true;
}

//------------------------------------------------------------------
/**
 * @brief  レギュレーション受信通知（プリセット）
 * @param[in]   presetIndex        プリセットレギュレーションの番号
 */
//------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnNBRNetRecvRegulation( u8 presetIndex )
{
  ICHI_PRINT("OnNBRNetRecvRegulationP %d \n", presetIndex );
  Regulation::PRESET preset = static_cast<Regulation::PRESET>(presetIndex);

  // 一応最大値チェック
  if( preset >= Regulation::PRESET_MAX )
  {
    GFL_ASSERT(0);
    preset = Regulation::PRESET_SINGLE_NORMAL;
  }

  // プリセットレギュレーション読み込み(実体は呼び出しEventが持っている）
  m_pNBRWork->m_pSelectRegulationDrawInfo->SetPreset( preset );
  m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation().LoadData( preset );

  // CRCチェック
  if( m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation().CheckCrc() == false )
  {
    // 送信する結果コマンド
    m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_BROKEN;
  }
  else
  {
    // 送信する結果コマンド
    m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_OK;
  }
  m_isRecvRegulation = true;
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
void NBRRuleSelectWaitFrame::OnNBRNetRecvRegulation( const int netId,
                                                     const void* pRegulationBuffer, const u32 regulationSize,
                                                     const void* pCerdData, const u32 cerdDataSize )
{
  GFL_PRINT("OnNBRNetRecvRegulationD\n");
  
  // @fix NMCat[2978]：ルール選択者以外の受信は無視する
  if( m_pNBRWork->IsRuleSelector( netId ) == false )
  {
    GFL_PRINT("ルール選択者以外なので無視\n");
    return;
  }


  // レギュレーションセット(実体は呼び出しEventが持っている）
  m_pNBRWork->m_pSelectRegulationDrawInfo->ResetPreset();
  m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation().SetData( pRegulationBuffer, regulationSize );
  m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation().DebugPrint();

  // 署名チェック
  bool isVerifyNg = false;
  {
    RegulationScriptChecker checker( m_pNBRWork->GetDeviceHeap() );
    checker.CheckStart( &m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation(),
                        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty(),
                        RegulationScriptChecker::MODE_BATTLE_TEAM );
    isVerifyNg = checker.IsVerifyNg();
  }

  // 署名チェック
  if( isVerifyNg == true )
  {
    // 送信する結果コマンド
    m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_INVALID;
  }
  // CRCチェック
  else if( m_pNBRWork->m_pSelectRegulationDrawInfo->GetRegulation().CheckCrc() == false )
  {
    // 送信する結果コマンド
    m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_BROKEN;
  }
  else
  {
    // 送信する結果コマンド
    m_sendRegulationRetCommand = NBRNetDefine::REGULATION_RET_COMMAND_OK;
  }
  m_isRecvRegulation = true;
}

/**
 * @brief     レギュレーション受信結果通知
 * @param[in] レギュレーション受信結果通知コマンド
 */
void NBRRuleSelectWaitFrame::OnNBRNetRecvRegulationRet( const NBRNetDefine::RegulationRetCommand command )
{
  ICHI_PRINT("OnNBRNetRecvRegulationRet %d \n", command );
  // マルチの場合他の参加者から送られてくる。
  // 結果がNGの場合進めてはいけないので処理する。
  m_recvRegulationRetCount++;

  // 失敗の結果で上書きする
  if( command != NBRNetDefine::REGULATION_RET_COMMAND_OK )
  {
    m_sendRegulationRetCommand = command;
  }
}

// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  // 待ちの時のみ。
  switch( GetSubSeq() ){
  case SEQ_REGULATION_READY_RECV_WAIT://! レギュレーション準備受信待ち
  case SEQ_REGULATION_SEND_TIMING:   //! レギュレーション送信前同期
  case SEQ_REGULATION_RECV_WAIT:     //! レギュレーション受信待ち
  case SEQ_TO_NEXT_TIMING:           //! 次へ進む前の同期
    {
      // やめますか[はい・いいえ]の選択肢出す
      m_pMesageMenuView->ShowAppEndList();
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  m_pLowerView->SetAllVisible( false );

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( RuleSelectWaitResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRRuleSelectWaitFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  // 画面戻す
  // 待機中メッセージ
  this->ShowWaitMessage();
}

//--------------------------------------------------------------------------
/**
 * @brief 待機中メッセージ表示
 */
//--------------------------------------------------------------------------
void NBRRuleSelectWaitFrame::ShowWaitMessage()
{
  Savedata::MyStatus* myStatus = m_pNBRWork->GetMyStatusFromNetId( *m_pNBRWork->m_pRuleSelectorNetId );

  // ○○さんが　たいせんルールをきめています
  m_pMesageMenuView->ShowMessageMyStatus( msg_nbr_win_36, myStatus, true );
  // 戻るボタン表示
  m_pLowerView->SetAllVisible( true );
}

//--------------------------------------------------------------------------
/**
 * @brief レギュレーションアラートメッセージ開始
 */
//--------------------------------------------------------------------------
void NBRRuleSelectWaitFrame::StartRegulationErrorMessage( const u32 msgId )
{
  m_pMesageMenuView->StartMessage( msgId, View::MESSAGE_FINISH_INPUT );
  // 戻るボタン非表示
  m_pLowerView->SetAllVisible( false );
  // メッセージ終了待ちへ
  SetSubSeq( SEQ_REG_ERROR_MESSAGE_WAIT );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

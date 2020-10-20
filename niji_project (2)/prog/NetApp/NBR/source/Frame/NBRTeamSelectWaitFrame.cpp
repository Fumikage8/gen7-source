// ============================================================================
/*
 * @file NBRTeamSelectWaitFrame.cpp
 * @brief 通信対戦受付アプリのルール選択待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_RULE_SELECT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRTeamSelectWaitFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRTeamSelectWaitFrame::NBRTeamSelectWaitFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
  ,m_pLowerView(NULL)
  ,m_pUpperView(NULL)
  ,m_battleParty( pNBRWork->GetDeviceHeap() )
  ,m_recvCount(0)
{
}

// コンストラクタ
NBRTeamSelectWaitFrame::~NBRTeamSelectWaitFrame()
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
bool NBRTeamSelectWaitFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_DEFAULT );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerViewReturnButton();
  m_pUpperView = m_pParentView->GetUpperViewPlayerInfoDefault();

  // 表示
  m_pLowerView->SetAllVisible( true );
  m_pUpperView->SetAllVisible( true );

  // ===== 上画面 =========
  SetupUpperViewPlayerInfoDefault();

  // ===== 下画面 =========
  // 待機中メッセージ
  this->ShowWaitMessage();

  // 選んだチームのバトルパーティ作成
  {
    const Savedata::BOX* pBox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
    m_battleParty.CreateBattleParty( pBox, pBoxPokemon, m_pNBRWork->m_teamSelectIndex, m_pNBRWork->GetDeviceHeap() ); 
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
void NBRTeamSelectWaitFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_BATTLE_START );
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
void NBRTeamSelectWaitFrame::removeListener()
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
void NBRTeamSelectWaitFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_BATTLESTART_SEND:         //! 勝負開始送信
    {
      if( m_pNet->SendBattleStart( m_pNBRWork->m_bgm, m_battleParty.IsQrTeam() ) )
      {
        SetSubSeq( SEQ_BATTLESTART_RECV_WAIT ); 
      }
    }
    break;

  case SEQ_BATTLESTART_RECV_WAIT:    //! 勝負開始受信待ち
    {
      if( IsBattleStartRecvFinish() )
      {
        SetSubSeq( SEQ_POKEPARTY_SEND ); 
      }
    }
    break;
    
  case SEQ_POKEPARTY_SEND:           //! 選んだパーティ送信
    {
      // バトルチームからポケパーティ引っ張ってくる
      pml::PokeParty* party = m_battleParty.GetPokeParty();

      //  WiFi時は署名もらってくる
      u8* certData = NULL;
      if( m_pNet->IsWifi() )
      {
        certData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSignature();
        GFL_ASSERT( certData );
      }

      // 戦闘開始通知送信
      if( m_pNet->SendPokeParty( party, certData ) )
      {
        SetSubSeq( SEQ_POKEPARTY_RECV_WAIT ); 
      }
    }
    break;

  case SEQ_POKEPARTY_RECV_WAIT:                //! 全員が選ぶの待ち
    {
      // ポケパーティ受信チェック
      if( m_pNet->IsRecvPokePartyEnd() )
      {
        SetSubSeq( SEQ_TO_NEXT_TIMING_LOOP ); 
      }
    }
    break;

  case SEQ_TO_NEXT_TIMING_LOOP:      //! 次へ行くための同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_TEAM_SELECT_WAIT2 ) )
      {
        // ポケパーティ検査へ進む
        exitFrame( TeamSelectWaitResult::TO_POKEPARTY_CHECK );
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
        m_pLowerView->SetAllVisible( true );
        TimeOutStart();
        SetSubSeq( SEQ_RETURN_RULE_SELECT_TIMING );
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
        exitFrame( TeamSelectWaitResult::TO_RULE_SELECTOR_DECIDE );
      }
    }
    break;
  
  default:
    GFL_ASSERT(0);
    exitFrame( TeamSelectWaitResult::APP_END );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRTeamSelectWaitFrame::cleanup()
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
void NBRTeamSelectWaitFrame::OnNBRNetRecvEnd()
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
  exitFrame( TeamSelectWaitResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief  勝負開始受信通知
 * @param[in] netId           通信ID
 * @param[in] bgm             選択されたBGM（ローカルで子のみ格納されている）
 * @param[in] isQrTeam        QRバトルチームかどうか
 */
//------------------------------------------------------------------
void NBRTeamSelectWaitFrame::OnNBRNetRecvBattleStart( const int netId, const u32 bgm, const bool isQrTeam )
{
  ICHI_PRINT("OnNBRNetRecvBattleStart netId:%d isQrTeam:%d\n", netId, isQrTeam);

  m_pNBRWork->m_isQrTeam[ netId ] = isQrTeam;
  m_recvCount++; // 受信カウント

  // 表示の選択状態を終了に変更
  PlayerStatusFinish( netId );

  // BGMは、ローカル通信で親が送ってきた場合のみ更新
  if( m_pNet->IsWifi() == false && m_pNBRWork->IsRuleSelector( netId ) )
  {
    m_pNBRWork->m_bgm = bgm;
    ICHI_PRINT(">ICHI NBR BGM更新 %d \n", bgm );
  }
}

//------------------------------------------------------------------
/**
 * @brief  参加出来ない受信通知
 */
//------------------------------------------------------------------
void NBRTeamSelectWaitFrame::OnNBRNetRecvCantParticipate()
{
  ICHI_PRINT("OnNBRNetRecvCantParticipate\n");
  // たいせんできないひとが　いるため　ルールを　えらびなおします
  m_pMesageMenuView->StartMessage( msg_nbr_win_12, View::MESSAGE_FINISH_INPUT );
  m_pLowerView->SetAllVisible( false );
  SetSubSeq( SEQ_CANNOT_JOIN_MESSAGE_WAIT );

  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
}

// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectWaitFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  // 待ちの時のみ。
  switch( GetSubSeq() ){
  case SEQ_BATTLESTART_RECV_WAIT:    //! 勝負開始受信待ち
  case SEQ_POKEPARTY_RECV_WAIT:      //! 全員が選ぶの待ち
  case SEQ_RETURN_RULE_SELECT_TIMING://! ルール選択へ戻る前同期
    // やめますか[はい・いいえ]の選択肢出す
    m_pMesageMenuView->ShowAppEndList();
    break;
  // @fix GFNMCat[1361]：Bボタン連打でアサートになるの修正
  case SEQ_BATTLESTART_SEND:         //! 勝負開始送信
  case SEQ_POKEPARTY_SEND:           //! 選んだパーティ送信
  case SEQ_TO_NEXT_TIMING_LOOP:      //! 次へ行くための同期待ち
  case SEQ_CANNOT_JOIN_MESSAGE_WAIT: //! ストリーミングメッセージ終了待ち
    break; // 何もしない
  default:
    GFL_ASSERT_MSG(0,"SubSeq=%d\n",GetSubSeq() );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectWaitFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");
  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  // 下画面非表示
  m_pLowerView->SetAllVisible( false );

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( TeamSelectWaitResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRTeamSelectWaitFrame::OnNoButtonAction()
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
void NBRTeamSelectWaitFrame::ShowWaitMessage()
{
  // ほかの　ひとを　まっています
  m_pMesageMenuView->ShowMessage( msg_nbr_win_14, true );
  // 戻るボタン表示
  m_pLowerView->SetAllVisible( true );
}

//--------------------------------------------------------------------------
/**
 * @brief 選択状態を選択完了に変える
 */
//--------------------------------------------------------------------------
void NBRTeamSelectWaitFrame::PlayerStatusFinish( int targetNetId )
{
  int placeNo = 0;
  for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  {
    if( netId == m_pNet->GetNetID() )
    {
      continue;
    }

    if( netId == targetNetId )
    {
      Savedata::MyStatus* pMyStatus = m_pNBRWork->GetMyStatusFromNetId( netId );
      View::NBRUpperViewPlayerInfoDefault::Place place = static_cast<View::NBRUpperViewPlayerInfoDefault::Place>(placeNo);

      if( pMyStatus != NULL )
      {
        m_pUpperView->SetPlayerStatus( place, View::NBRUpperViewPlayerInfoDefault::PLAYER_STATUS_FINISH );
      }
      break;
    }
    placeNo++;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   勝負開始通知受信完了か？
 * @return  trueで受信完了
 */
//--------------------------------------------------------------------------
bool NBRTeamSelectWaitFrame::IsBattleStartRecvFinish() const
{
  return ( m_pNet->GetConnectionNum() == m_recvCount );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

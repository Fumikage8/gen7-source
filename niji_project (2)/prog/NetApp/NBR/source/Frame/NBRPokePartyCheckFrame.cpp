// ============================================================================
/*
 * @file NBRPokePartyCheckFrame.cpp
 * @brief 通信対戦受付アプリのポケパーティ検査フレーム
          フレームID：NBR_FRAME_ID_POKEPARTY_CHECK
 * @date 2015.08.07
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRPokePartyCheckFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "System/include/GameStrLen.h"

// レギュレーション検査用
#include "Battle/Regulation/include/PokeRegulation.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRPokePartyCheckFrame::NBRPokePartyCheckFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet * pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
    ,m_pLowerView(NULL)
    ,m_result(NetAppLib::System::FRAME_RESULT_INVALID)
    ,m_recvCount(0)
    ,m_isSendOtherPokePartyRetCommandNg(false)
    ,m_isRecvMyPokePartyRetCommandNg(false)
{
}

// コンストラクタ
NBRPokePartyCheckFrame::~NBRPokePartyCheckFrame()
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
bool NBRPokePartyCheckFrame::startup()
{
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_DEFAULT );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerViewReturnButton();
  // メッセージは前の画面のままでOK

  // 戻る非表示
  m_pLowerView->SetAllVisible( false );
  m_pParentView->GetUpperViewPlayerInfoDefault()->SetAllVisible( true );

  // つうしんたいき中
  m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );

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
void NBRPokePartyCheckFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_POKEPARTY_RESULT );
  // @fix GFNMcat[1361]ボタン不要なのでリスナー解除
  //m_pLowerView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRPokePartyCheckFrame::removeListener()
{
  m_pNet->UnregistRecvListener();
  m_pNet->ClearRecvPacketEnableCommand();
  //m_pLowerView->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRPokePartyCheckFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_POKEPARTY_CHECK_AND_SEND:      //! ポケパーティ検査
    {
      if( CheckAndSendPokePartyCore() )
      {
        SetSubSeq( SEQ_RECV_WAIT );
      }
    }
    break;
  case SEQ_RECV_WAIT:            //! コマンド受信待ち
    {
      if( IsRecvCommandFinish() )
      {
        // 送受信コマンド見て次の処理確定
        SetNextSeqByNetCommand();
      }
    }
    break;

  case SEQ_MESSAGE_WAIT_TO_TEAM_SELECT: //! メッセージ終了待ち(終了後チーム選択へ)
    if( IsTimeOut() == true )
    {
      ToTimingLoop( PokePartyCheckResult::TO_TEAM_SELECT );
    }
    break;

  case SEQ_MESSAGE_WAIT_TO_TEAM_SELECT_WAIT:         //! メッセージ終了待ち
    if( IsTimeOut() == true )
    {
      ToTimingLoop( PokePartyCheckResult::TO_TEAM_SELECT_WAIT );
    }
    break;

  case SEQ_TIMING_LOOP:          //! 同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_POKEPARTY_CHECK ) )
      {
        exitFrame( m_result ); 
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
bool NBRPokePartyCheckFrame::cleanup()
{
  // メッセージの最低表示時間経過待ち
  if( IsTimeOut() == false )
  {
    return false;
  }

  // ここで非表示にしないと次のフレームで出ちゃう
  m_pParentView->GetUpperViewPlayerInfoDefault()->SetAllVisible( false );
  return true;
}


// NetRecvListener
//------------------------------------------------------------------
/**
 * @brief     ポケパーティ検査結果通知
 * @param[in] レポケパーティ検査結果コマンド
 */
//------------------------------------------------------------------
void NBRPokePartyCheckFrame::OnNBRNetRecvPokePartyCheckRet( const u8 command[NBRNetDefine::NET_NUM_MAX] )
{
  ICHI_PRINT("OnNBRNetRecvPokePartyCheckRet\n" );
  m_recvCount++;

  // 自分のNetID要素でチェックしてNGだった場合、自分のポケパーティがおかしい
  if( command[ m_pNet->GetNetID() ] == NBRNetDefine::POKEPARTY_RET_REG_NG )
  {
    m_isRecvMyPokePartyRetCommandNg = true;
    ICHI_PRINT(">ICHI 自分のパーティが相手NGだった\n");
  }
}

// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRPokePartyCheckFrame::OnBackButtonAction()
{
  // 未使用
  GFL_ASSERT(0);
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケパーティ検査して結果送信
 */
//--------------------------------------------------------------------------
bool NBRPokePartyCheckFrame::CheckAndSendPokePartyCore()
{
  NBRNetDefine::PokePartyRetCommand  sendPokePartyRetCommand[ NBRNetDefine::NET_NUM_MAX ];
  gfl2::std::MemClear( sendPokePartyRetCommand, sizeof(NBRNetDefine::PokePartyRetCommand)*NBRNetDefine::NET_NUM_MAX );

  // レギュレーションチェック
  NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = m_pNBRWork->m_pSelectRegulationDrawInfo;
  if( pRegulationDrawInfo == NULL )
  {
    GFL_ASSERT(0); // バグ以外あり得ない
    sendPokePartyRetCommand[0] = NBRNetDefine::POKEPARTY_RET_REG_NG;
    return m_pNet->SendPokePartyRet( sendPokePartyRetCommand );
  }

  Regulation& regulation = pRegulationDrawInfo->GetRegulation();

  // 検査用クラス作成
  PokeRegulation pokeRegulation( m_pHeap );
  pokeRegulation.SetRegulation( &regulation );

  // 受信した相手分チェック
  for( int netId=0; netId<m_pNet->GetConnectionNum(); netId++ )
  {
    // 自分はSkipする
    if( m_pNet->GetNetID() == netId )
    {
      sendPokePartyRetCommand[netId] = NBRNetDefine::POKEPARTY_RET_OK;
      continue;
    }
    // 受信していればNULL以外が格納されている
    pml::PokeParty* pPokeParty = m_pNBRWork->GetPokePartyFromNetId( netId );
    if( pPokeParty == NULL )
    {
      GFL_ASSERT_MSG(0,"%d",netId); // バグ以外あり得ない
      sendPokePartyRetCommand[netId] = NBRNetDefine::POKEPARTY_RET_REG_NG;
      m_isSendOtherPokePartyRetCommandNg = true;
      continue;
    }

    // バトルチーム用レギュレーションチェック
    PokeRegulation::ViolationDetails violationDetails; // エラー詳細データ
    if( pokeRegulation.CheckPokePartyForBtlTeam( pPokeParty, violationDetails ) != true )
    {
      GFL_PRINT("ポケパーティ検査でレギュレーション %d\n", netId );
      pPokeParty->Dump();
      sendPokePartyRetCommand[netId] = NBRNetDefine::POKEPARTY_RET_REG_NG;
      m_isSendOtherPokePartyRetCommandNg = true;
      continue;
    }

    // 不正チェックの証明書チェック結果（チェック自体は受信時にやっている）
    if( m_pNet->IsWifi() && m_pNet->IsValidationCheckOk( netId ) == false )
    {
      GFL_PRINT("ポケパーティ検査で不正チェックNG %d\n", netId );
      pPokeParty->Dump();
      sendPokePartyRetCommand[netId] = NBRNetDefine::POKEPARTY_RET_REG_NG;
      m_isSendOtherPokePartyRetCommandNg = true;
      continue;
    }

    // 検査OK
    sendPokePartyRetCommand[netId] = NBRNetDefine::POKEPARTY_RET_OK;
  }

  return m_pNet->SendPokePartyRet( sendPokePartyRetCommand );
}

//--------------------------------------------------------------------------
/**
 * @brief   全員のコマンド受信完了したか？
 * @return  trueで受信完了
 */
//--------------------------------------------------------------------------
bool NBRPokePartyCheckFrame::IsRecvCommandFinish() const
{
#ifdef  GF_PLATFORM_CTR
  return ( m_pNet->GetConnectionNum() == m_recvCount );
#else
  return true;
#endif // GF_PLATFORM_CTR
}

//--------------------------------------------------------------------------
/**
 * @brief   送受信コマンド見て次の処理確定
 */
//--------------------------------------------------------------------------
void NBRPokePartyCheckFrame::SetNextSeqByNetCommand()
{
  // 受信コマンド確認
  {
    // 自分のポケモンがレギュレーション違反だった
    if( m_isRecvMyPokePartyRetCommandNg == true )
    {
      // えらんだ　バトルチームにもんだいが　あります
      // @fix GFNMCat[5698]：キー待ちメッセージだと相手をはめれるため、自動で進むように修正
      m_pMesageMenuView->ShowMessage( msg_nbr_win_40, true );
      TimeOutStart(4000); // 4秒
      SetSubSeq( SEQ_MESSAGE_WAIT_TO_TEAM_SELECT );
      return;
    }
  }

  // 送信コマンド確認
  {
    // 相手のポケモンがレギュレーション違反だった
    if( m_isSendOtherPokePartyRetCommandNg == true )
    {
      // あいての　バトルチームに　もんだいがあるため
      // @fix GFNMCat[5698]：キー待ちメッセージだと相手をはめれるため、自動で進むように修正
      m_pMesageMenuView->ShowMessage( msg_nbr_win_43, true );
      TimeOutStart(4000); // 4秒
      SetSubSeq( SEQ_MESSAGE_WAIT_TO_TEAM_SELECT_WAIT );
      return;
    }
  }

  NetAppLib::System::FrameResult frameResult;

  // マルチの場合
  if( m_pNBRWork->IsMutli() )
  {
    frameResult = ( m_pNBRWork->IsOneselfRuleSelector() ) ? PokePartyCheckResult::TO_MULTI_PLAYER_SELECT : PokePartyCheckResult::TO_MULTI_PARENT_WAIT; 
  }
  else
  {
    frameResult = PokePartyCheckResult::TO_BATTLE_START;
  }

  ToTimingLoop( frameResult );
}

//--------------------------------------------------------------------------
/**
 * @brief   終了同期へ
 */
//--------------------------------------------------------------------------
void NBRPokePartyCheckFrame::ToTimingLoop( NetAppLib::System::FrameResult result )
{
  m_result = result;

  // メッセージを変えた時だけにしないと、タイムアイコンがパシる
  if( m_result == PokePartyCheckResult::TO_TEAM_SELECT ||
      m_result == PokePartyCheckResult::TO_TEAM_SELECT_WAIT )
  {
    // つうしんたいき中
    m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
  }

  TimeOutStart();

  SetSubSeq( SEQ_TIMING_LOOP );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

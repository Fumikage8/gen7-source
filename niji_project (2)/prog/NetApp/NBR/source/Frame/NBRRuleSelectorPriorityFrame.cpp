// ============================================================================
/*
 * @file NBRRuleSelectorPriorityFrame.cpp
 * @brief 通信対戦受付アプリのルール選択する人の優先順位決めプロセスです。
          フレームID：NBR_FRAME_ID_RULE_SELECTOR_PRIORITY
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRRuleSelectorPriorityFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "System/include/GameStrLen.h"
// system
#include "System/include/GflUse.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRRuleSelectorPriorityFrame::NBRRuleSelectorPriorityFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet * pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pUpperView( NULL )
{
}

// コンストラクタ
NBRRuleSelectorPriorityFrame::~NBRRuleSelectorPriorityFrame()
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
bool NBRRuleSelectorPriorityFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_SIMPLE );

  // 下画面View取得
  m_pUpperView = m_pParentView->GetUpperViewPlayerInfoSimple();

  // 表示
  m_pUpperView->SetAllVisible( true );

  // ===== 上画面 =========
  SetupUpperViewPlayerInfoSimple();

  // ===== 下画面 =========
  // つうしん　たいきちゅう　です
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
void NBRRuleSelectorPriorityFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_PRIORITY );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorPriorityFrame::removeListener()
{
  m_pNet->UnregistRecvListener();
  m_pNet->ClearRecvPacketEnableCommand();
  m_pMesageMenuView->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRRuleSelectorPriorityFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_NET_INITIALIZE:       //! NBRNetの初期化
    {
      m_pNet->Initialize();
      SetSubSeq( SEQ_NET_INITIALIZE_WAIT );
      GFL_RELEASE_PRINT(">NBR SEQ_NET_INITIALIZE_WAIT\n");
    }
    break;

  case SEQ_NET_INITIALIZE_WAIT:  //! NBRNetの初期化待ち
    {
      if( m_pNet->IsInitialize() )
      {
        SetSubSeq( SEQ_SEND_PRIORITY );
        GFL_RELEASE_PRINT(">NBR SEQ_SEND_PRIORITY\n");
      }
    }
    break;

  case SEQ_SEND_PRIORITY:        //! 優先順位送信（親）
    {
      if( m_pNet->IsMaster() )
      {
        if( m_pNet->SendPriority( m_pNBRWork->m_priority ) )
        {
          // デバッグ用出力
          for( u8 netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ ) { ICHI_PRINT("m_pNBRWork->m_priority[%d]=%d\n", netId, m_pNBRWork->m_priority[netId] ); }
          // 同期へ
          SetSubSeq( SEQ_TIMING_LOOP );
          GFL_RELEASE_PRINT(">NBR SEQ_TIMING_LOOP1\n");
        }
      }
      else
      {// 子
        SetSubSeq( SEQ_RECV_PRIORITY_WAIT );
        GFL_RELEASE_PRINT(">NBR SEQ_RECV_PRIORITY_WAIT\n");
      }
    }
    break;
  case SEQ_RECV_PRIORITY_WAIT:   //! 優先順位受信待ち（子）
    // リスナー待ち
    break;
  case SEQ_TIMING_LOOP:          //! 同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_PRIORITY ) )
      {
        // ルール選択する人決定へ
        exitFrame( RuleSelectorPriorityResult::TO_RULE_SELECTOR_DECIDE );
      }
    }
    break;
  default:
    GFL_ASSERT(0);
    exitFrame( RuleSelectorPriorityResult::APP_END );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRRuleSelectorPriorityFrame::cleanup()
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
void NBRRuleSelectorPriorityFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( RuleSelectorPriorityResult::APP_END );
}

//------------------------------------------------------------------
/**
 * @brief     ルール選ぶ人の候補者優先順位通知（子のみ）
 * @param[in] ルール選ぶ人の候補者優先順位（NetIdが要素で優先度が格納されている）
 */
//------------------------------------------------------------------
void NBRRuleSelectorPriorityFrame::OnNBRNetRecvPriority( const u8 priority[NBRNetDefine::NET_NUM_MAX] )
{
  ICHI_PRINT("OnNBRNetRecvPriority\n");
  // ワークに保存
  gfl2::std::MemCopy( priority, m_pNBRWork->m_priority, sizeof(u8)*NBRNetDefine::NET_NUM_MAX );

  // デバッグ用出力
  for( u8 netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ ) { ICHI_PRINT("m_pNBRWork->m_priority[%d]=%d\n", netId, m_pNBRWork->m_priority[netId] ); }

  // 優先度一番高い人をルール選択者として保存
  for( u8 netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  {
    if( m_pNBRWork->m_priority[netId] == 0 )
    {
      *m_pNBRWork->m_pRuleSelectorNetId = netId;
      ICHI_PRINT("ルール選択者のNetIdは %d\n", netId);
      break;
    }
  }

  // 同期へ
  SetSubSeq( SEQ_TIMING_LOOP );
  GFL_RELEASE_PRINT(">NBR SEQ_TIMING_LOOP2\n");
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

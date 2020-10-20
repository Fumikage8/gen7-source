// ============================================================================
/*
 * @file NBRMultiParentWaitFrame.cpp
 * @brief 通信対戦受付アプリのマルチ親待ちプロセスです。
 *        フレームID:NBR_FRAME_ID_MULTI_PARENT_WAIT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRMultiParentWaitFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRMultiParentWaitFrame::NBRMultiParentWaitFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet ),
  m_pLowerView( NULL )
{
  for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
  {
    m_decideNetId[i] = -1;
  }
}

// コンストラクタ
NBRMultiParentWaitFrame::~NBRMultiParentWaitFrame()
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
bool NBRMultiParentWaitFrame::startup()
{
  // 使用する画面
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_VS );

  // 下画面View取得
  m_pLowerView = m_pParentView->GetLowerViewReturnButton();

  // 表示
  m_pLowerView->SetAllVisible( true );
  m_pParentView->GetUpperViewVs()->SetAllVisible( true );

  // 上画面設定
  SetupUpperViewVs();

  // チームを　ふりわけています　おまちください
  m_pMesageMenuView->ShowMessage( msg_nbr_win_17, true );

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRMultiParentWaitFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_MULTI_DECIDE );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_MULTI_SELECT );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRMultiParentWaitFrame::removeListener()
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
void NBRMultiParentWaitFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case SEQ_RECV_WAIT:                //! 受信待ち
    // リスナー待ち
    break;
  case SEQ_TIMING_LOOP:              //! 同期待ち
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_MULTI_DECIDE ) )
      {
        exitFrame( MultiParentWaitResult::TO_BATTLE_START );
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
bool NBRMultiParentWaitFrame::cleanup()
{
  // 選択肢コマンドが出ている場合は消す
  if( m_pMesageMenuView->IsShowList() == true )
  {
    m_pMesageMenuView->HideMessage();
  }

  return true;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
// NetRecvListener
//------------------------------------------------------------------
/**
 * @brief  通信終了通知
 */
//------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnNBRNetRecvEnd()
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
  exitFrame( MultiParentWaitResult::APP_END );
}
//------------------------------------------------------------------
/**
 * @brief  マルチ振り分け決定通知
 * @param[in] NetIdが要素で戦う位置が格納されている
 */
//------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnNBRNetRecvMultiDecide( const u8 place[4] )
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnNBRNetRecvMultiDecide \n");
  ICHI_PRINT("[NetID=Place] → 0:%d 1:%d 2:%d 3:%d \n", place[0], place[1], place[2], place[3]);

  GFL_PRINT("----決まった立ち位置-----\n");

  // ↓ARMCC4.1によるコンパイラ不具合により、ループ展開しています。
  m_pNBRWork->m_place[0] = static_cast<BTL_CLIENT_ID>(place[0]);
  m_pNBRWork->m_place[1] = static_cast<BTL_CLIENT_ID>(place[1]);
  m_pNBRWork->m_place[2] = static_cast<BTL_CLIENT_ID>(place[2]);
  m_pNBRWork->m_place[3] = static_cast<BTL_CLIENT_ID>(place[3]);

  SetSubSeq( SEQ_TIMING_LOOP );
}

//------------------------------------------------------------------
/**
 * @brief  マルチ振り分け通知
 * @param[in] netId   決めた人のNetId
 * @param[in] place   立ち位置（BTL_CLIETN_ID）
 */
//------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnNBRNetRecvMultiSelect( const int netId, const u8 place )
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnNBRNetRecvMultiSelect netId%d place%d \n", netId, place);

  // 既にどこかの場所に入っているプレイヤー名を選んだ場合は、
  // 既に入っていた場所を空白にします。
  {
    for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
    {
      if( m_decideNetId[i] == netId )
      {// 既に入っていた
        m_decideNetId[i] = -1; // 初期化
        // 上画面更新
        m_pParentView->GetUpperViewVs()->SetPlayerData( i, NULL );
      }
    }
  }

  // 上画面更新
  m_pParentView->GetUpperViewVs()->SetPlayerData( place, m_pNBRWork->GetMyStatusFromNetId( netId ) );
  m_decideNetId[ place ] = netId;
}


// NBRLowerViewReturnButtonListener
//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnBackButtonAction()
{
  ICHI_PRINT("OnBackButtonAction\n");

  // やめますか[はい・いいえ]の選択肢出す
  m_pMesageMenuView->ShowAppEndList();
}

//--------------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
//--------------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnYesButtonAction()
{
  ICHI_PRINT("OnYesButtonAction\n");

  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  m_pLowerView->SetAllVisible( false );

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( MultiParentWaitResult::APP_END );
}

//--------------------------------------------------------------------------
/**
 * @brief   いいえボタン通知
 */
//--------------------------------------------------------------------------
void NBRMultiParentWaitFrame::OnNoButtonAction()
{
  ICHI_PRINT("OnNoButtonAction\n");
  // 画面戻す
  // チームを　ふりわけています　おまちください
  m_pMesageMenuView->ShowMessage( msg_nbr_win_17, true );
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

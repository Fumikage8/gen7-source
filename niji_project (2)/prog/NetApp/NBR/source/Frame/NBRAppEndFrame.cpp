// ============================================================================
/*
 * @file NBRAppEndFrame.cpp
 * @brief 通信対戦受付アプリのアプリ終了時フレーム
          フレームID：NBR_FRAME_ID_APP_END
 * @date 2015.07.25
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRAppEndFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "System/include/GameStrLen.h"

// P2P切断用
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRAppEndFrame::NBRAppEndFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet * pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
{
}

// コンストラクタ
NBRAppEndFrame::~NBRAppEndFrame()
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
bool NBRAppEndFrame::startup()
{
  // 使用する画面
  // 前の画面のものそのままで良い

  // ===== 下画面 =========
  if( m_pNBRWork->m_appEndFrameEndType == ApplicationSystem::NBRWork::END_TYPE_ONESELF )
  {
    // つうしんを　しゅうりょうしています
    m_pMesageMenuView->ShowAppEndMessage();
    TimeOutStart();
  }
  else
  {
    // つうしんから　ぬけたひとが　いるため　しゅうりょうします
    m_pMesageMenuView->StartMessage( msg_nbr_win_32, View::MESSAGE_FINISH_INPUT );
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRAppEndFrame::updateSequence()
{
  if( m_pNBRWork->m_appEndFrameEndType == ApplicationSystem::NBRWork::END_TYPE_ONESELF )
  { // 自らやめる場合の更新
    UpdateTypeOneself();
  }
  else
  { // 相手からの通知でやめる場合の更新
    UpdateTypeOpponent();
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRAppEndFrame::cleanup()
{
  if( IsTimeOut() == false )
  {
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   自らやめる場合の更新
 */
//--------------------------------------------------------------------------
void NBRAppEndFrame::UpdateTypeOneself()
{
  switch( GetSubSeq() ) {
  case 0:
    // 終了リクエストを送信
    if( m_pNet->SendEnd() )
    {
      // 切断開始
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart();

      AddSubSeq();
    }
    break;

  case 1:
    // 切断完了待ち
    if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsDisconnectSuccess() )
    {
      // Terminateは呼び出し元イベント (JoinFestaBeaconEvent JoinFestaQuickEvent )で行う
      
      // 終了
      exitFrame(0);
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   相手からの通知でやめる場合の更新
 */
//--------------------------------------------------------------------------
void NBRAppEndFrame::UpdateTypeOpponent()
{
  switch( GetSubSeq() ) {
  case 0:
    {
      // 裏で切断開始
      GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart();
      AddSubSeq();
    }
    // break through

  case 1:
    if( m_pMesageMenuView->IsEndMessage() )
    {
      // つうしんを　しゅうりょうしています
      m_pMesageMenuView->ShowAppEndMessage();
      TimeOutStart();
      AddSubSeq();
    }
    break;
  case 2:
    // 切断完了待ち
    if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsDisconnectSuccess() )
    {
      // Terminateは呼び出し元イベント (JoinFestaBeaconEvent JoinFestaQuickEvent )で行う
     
      // 終了
      exitFrame(0);
    }
    break;
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

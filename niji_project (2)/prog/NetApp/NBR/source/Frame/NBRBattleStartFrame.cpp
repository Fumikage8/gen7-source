// ============================================================================
/*
 * @file NBRBattleStartFrame.cpp
 * @brief 通信対戦受付アプリのバトルへプロセスです。
 *        フレームID:NBR_FRAME_ID_BATTLE_START
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRBattleStartFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewReturnButton.h"
#include "NetApp/NBR/source/View/SubView/NBRUpperViewPlayerInfoDefault.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRBattleStartFrame::NBRBattleStartFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
{
}

// コンストラクタ
NBRBattleStartFrame::~NBRBattleStartFrame()
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
bool NBRBattleStartFrame::startup()
{
  // ViewをはずしてBGのみ表示にする
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_RETURN_BUTTON );
  m_pParentView->GetLowerViewReturnButton()->SetAllVisible(false);
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_DEFAULT );
  m_pParentView->GetUpperViewPlayerInfoDefault()->SetAllVisible( false );

  // 通信中
  m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRBattleStartFrame::updateSequence()
{
  switch( GetSubSeq() ){
  case 0:
    // 同期待ち
    if( IsTimeOut() && m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_TO_BATTLE ) )
    {
      AddSubSeq();
    }
    break;

  case 1:
    // さんかするポケモンをえらんでください
    m_pMesageMenuView->StartMessage( msg_nbr_win_19, View::MESSAGE_FINISH_NONE );
    // 3秒後に自動で次に進む
    TimeOutStart( AUTO_END_WAIT );
    AddSubSeq();
    break;
  case 2:
    if( IsTimeOut() )
    {
      exitFrame( BattleStartResult::TIMING_FINISH );
    }
    break;
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

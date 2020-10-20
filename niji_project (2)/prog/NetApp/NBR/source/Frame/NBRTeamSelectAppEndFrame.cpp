// ============================================================================
/*
 * @file NBRTeamSelectAppEndFrame.cpp
 * @brief 通信対戦受付アプリのチーム選択アプリ終了プロセスです。
 *        フレームID:NBR_FRAME_ID_TEAM_SELECT_APP_END
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRTeamSelectAppEndFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRTeamSelectAppEndFrame::NBRTeamSelectAppEndFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
{
}

// コンストラクタ
NBRTeamSelectAppEndFrame::~NBRTeamSelectAppEndFrame()
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
bool NBRTeamSelectAppEndFrame::startup()
{
  //m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_TEAM_SELECT );
  //m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_PLAYER_INFO_DEFAULT );

  //m_pParentView->GetLowerViewTeamSelect()->SetVisible(true);
  //m_pParentView->GetLowerViewTeamSelect()->ShowMessage( msg_nbr_win_01, false );

  //m_pParentView->GetUpperViewPlayerInfoSimple()->SetVisible(true);

  // ===== 上画面 =========
  //SetupUpperViewPlayerInfoDefault();

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectAppEndFrame::setListener()
{
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectAppEndFrame::removeListener()
{
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRTeamSelectAppEndFrame::updateSequence()
{
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

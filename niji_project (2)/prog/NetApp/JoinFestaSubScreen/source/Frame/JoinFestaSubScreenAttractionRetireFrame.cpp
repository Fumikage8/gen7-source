// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionRetireFrame.cpp
 * @brief ジョインフェスタ下画面のアトラクションリタイア専用フレームです。
 * @date 2015.12.04
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenAttractionRetireFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h" // リクエストID

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_jf_menu.h>

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenAttractionRetireFrame::JoinFestaSubScreenAttractionRetireFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
    ,m_waitTimeout()
{
}


JoinFestaSubScreenAttractionRetireFrame::~JoinFestaSubScreenAttractionRetireFrame()
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
bool JoinFestaSubScreenAttractionRetireFrame::startup()
{
  m_pParentView->DisableAllView();

  // 選択肢ビューをお断りリストで設定（表示はしない）
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_ADD );
  m_pParentView->GetSelectLowerView()->SetVisible( true );
  m_pParentView->GetSelectLowerView()->SetVisibleSelectButton( false );

  // 「しゅうりょうしています　しばらくおまちください」
  m_pParentView->GetSelectLowerView()->SetStreamMessage( jf_menu_win_29, true );

  // 最低ウェイト
  m_waitTimeout.TimeOutStart( WAIT_TIME );

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenAttractionRetireFrame::updateSequence()
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  switch( GetSubSeq() ){
  case 0:
    {
      // 最低ウェイト ローカルは呼んだ瞬間に終わるのでここでチェック
      if( m_waitTimeout.IsTimeOut() )
      {
        // アトラクション終了リクエスト送信
        pAttractionManager->FinishRequest();
        AddSubSeq();
      }
    }
    break;
  case 1:
    {
      // リクエスト終了待ち
      if( pAttractionManager->IsEndRequest() )
      {
        // メッセージ非表示
        m_pParentView->GetSelectLowerView()->SetVisible( false );
        AddSubSeq();
      }
    }
    break;
  default:
    break;
  }
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

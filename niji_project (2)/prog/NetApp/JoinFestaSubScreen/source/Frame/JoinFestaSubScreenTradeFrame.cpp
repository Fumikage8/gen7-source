// ============================================================================
/*
 * @file JoinFestaSubScreenTradeFrame.cpp
 * @brief ジョインフェスタ下画面の何の交換をするか選ぶフレームです。
 * @date 2015.09.02
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/Frame/JoinFestaSubScreenTradeFrame.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/View/JoinFestaSubScreenParentView.h"

#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"
#include <niji_conv_header/message/msg_jf_menu.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenTradeFrame::JoinFestaSubScreenTradeFrame(
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
  NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  JoinFestaSubScreenFrameBase( pJoinFestaSubScreenWork, pJoinFestaSubScreenParentView )
{
}


JoinFestaSubScreenTradeFrame::~JoinFestaSubScreenTradeFrame()
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
bool JoinFestaSubScreenTradeFrame::startup()
{
  m_pParentView->DisableAllView();
  // ボタン構築
  m_pParentView->GetSelectLowerView()->SetupButton( View::JoinFestaSubScreenSelectLowerView::BUTTON_TEXT_MODE_TRADE );
  m_pParentView->GetSelectLowerView()->SetVisible( true );

  // @fix GFNMcat[3601]：サスペンド直後にここに来た時に入力許可させない
  if( !m_pJoinFestaSubScreenWork->IsSuspend() )
  {
    m_pParentView->GetSelectLowerView()->SetInputEnabled( true ); // ダブルタッチ修正
    m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( true );
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenTradeFrame::setListener()
{
  m_pParentView->GetSelectLowerView()->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenTradeFrame::removeListener()
{
  m_pParentView->GetSelectLowerView()->SetInputEnabled( false ); // ダブルタッチ修正
  m_pParentView->GetSelectLowerView()->GetButtonManager()->SetInputEnable( false ); // ダブルタッチ修正
  m_pParentView->GetSelectLowerView()->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenTradeFrame::updateSequence()
{
  // 受信パケットのイベント通知更新
  UpdatePacketEventCheck();
}

//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool JoinFestaSubScreenTradeFrame::cleanup()
{
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 * @param[out]  buttonId    選んだボタンのボタンID
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenTradeFrame::OnButtonAction( int buttonId )
{
  ICHI_PRINT("On2uttonAction buttonId %d\n", buttonId);

  bool bCheckPokemonCountForTrade = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForTrade( m_pJoinFestaSubScreenWork->GetDeviceHeap() );

  switch( buttonId )
  {
    case BUTTON_ID_TRADE:
    {
      // 2匹以上いる
      if( bCheckPokemonCountForTrade )
      {
        // 募集方法選択画面へ
        //exitFrame( TRADE_RESULT_BOSYUU );
        // 誰とでも募集がないから直接飛ばす。誰とでも募集の仕様はなくなった。
        m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE );
      }
      // 2匹以上いない
      else
      {
        // アラート
        m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_03, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TRADE );
        exitFrame( TRADE_RESULT_ALERT );
      }
    }
    break;
    
    case BUTTON_ID_GTS:
    {
      // 手持ちかボックスに空きがある
      if( NetAppLib::Util::NetAppEntryChecker::CheckFreeSpaceForGtsPlay( m_pJoinFestaSubScreenWork->GetDeviceHeap() ) )
      {
        // 2匹以上いる
        if( bCheckPokemonCountForTrade )
        {
          // GTSへいく
          m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_GTS );
        }
        // 2匹以上いない
        else
        {
          // GTSに預けている場合 かつ 手持ちかボックスに1匹いる
          if( NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForGtsPlay( m_pJoinFestaSubScreenWork->GetDeviceHeap() ) )
          {
            // GTSへいく
            m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_GTS );
          }
          else
          {
            // アラート
            m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_03, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TRADE );
            exitFrame( TRADE_RESULT_ALERT );
          }
        }
      }
      // 手持ちもボックスも空きがない
      else
      {
        // アラート
        m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_04, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TRADE );
        exitFrame( TRADE_RESULT_ALERT );
      }
    }
    break;

    case BUTTON_ID_MIRACLE:
    {
      // 2匹以上いる
      if( bCheckPokemonCountForTrade )
      {
        // ミラクル交換へいく
        m_pJoinFestaSubScreenWork->SetRequestID( Field::SubScreen::OUT_REQ_ID_MIRACLE );
      }
      // 2匹以上いない
      else
      {
        // アラート
        m_pJoinFestaSubScreenWork->SetAlertMessageIDAndBootFrameID( jf_menu_exp_03, NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_TRADE );
        exitFrame( TRADE_RESULT_ALERT );
      }
    }
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void JoinFestaSubScreenTradeFrame::OnBackButtonAction()
{
  // メインフレームへ
  exitFrame( TRADE_RESULT_RETURN );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

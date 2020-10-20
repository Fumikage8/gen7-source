// ============================================================================
/*
 * @file JoinFestaSubScreenBattleRuleFrame.h
 * @brief ジョインフェスタ下画面の対戦ルールを選ぶフレームです。
 * @date 2015.11.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_BTL_RULE_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_BTL_RULE_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerInputListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenBattleRuleFrame :
  public JoinFestaSubScreenFrameBase,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenSelectLowerInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenBattleRuleFrame );
public:
  JoinFestaSubScreenBattleRuleFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenBattleRuleFrame();

public:

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenMainLowerInputListener
  //-----------------------------------------------------------------------------
  //! ボタン通知
  virtual void OnButtonAction( int buttonId );
  //! 戻るボタン通知
  virtual void OnBackButtonAction();

private:

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  //! このフレームの起動処理を記述します。true で終了
  virtual bool startup();
  //! このフレームで実装するリスナーの登録処理を記述します。
  virtual void setListener();
  //! このフレームで実装するリスナーの解除処理を記述します。
  virtual void removeListener();
  //! このフレームのシーケンス制御処理を記述します。
  virtual void updateSequence();
  //! このフレームが終了してもよいかの判定を記述します。
  virtual bool cleanup();


private:

/**
   * @enum ButtonId
   * ボタンID定義
   */
  enum ButtonId
  {
    BUTTON_ID_SINGLE        = View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_4BUTTON_01, //! シングルバトルボタン
    BUTTON_ID_DOUBLE        = View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_4BUTTON_02, //! ダブルバトルボタン
    BUTTON_ID_MULTI         = View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_4BUTTON_03, //! マルチバトルボタン
    BUTTON_ID_ROYAL         = View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_4BUTTON_04, //! ロイヤルバトルボタン
  };
};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_BTL_RULE_FRAME_H_INCLUDE

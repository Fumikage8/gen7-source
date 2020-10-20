// ============================================================================
/*
 * @file JoinFestaSubScreenAskEndFrame.h
 * @brief ジョインフェスタ下画面の終了を問うフレームです。
 * @date 2015.09.02
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ASKEND_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_ASKEND_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerInputListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenAskEndFrame :
  public JoinFestaSubScreenFrameBase,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenSelectLowerInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenAskEndFrame );
public:
  JoinFestaSubScreenAskEndFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenAskEndFrame();

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
    BUTTON_ID_END = View::JoinFestaSubScreenSelectLowerView::BUTTON_ID_1BUTTON,    //! 終了するボタン
  };

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ASKEND_FRAME_H_INCLUDE

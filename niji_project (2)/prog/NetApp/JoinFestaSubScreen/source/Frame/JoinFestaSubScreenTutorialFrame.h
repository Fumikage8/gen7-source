// ============================================================================
/*
 * @file JoinFestaSubScreenTutorialFrame.h
 * @brief ジョインフェスタ下画面のチュートリアル中フレームです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_TUTORIAL_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_TUTORIAL_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenBgLowerView.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenTutorialFrame :
  public JoinFestaSubScreenFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenTutorialFrame );
public:
  JoinFestaSubScreenTutorialFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenTutorialFrame();

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


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_TUTORIAL_FRAME_H_INCLUDE

// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionFrame.h
 * @brief ジョインフェスタ下画面のアトラクション中専用フレームです。
 * @date 2015.11.17
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ATTRACTION_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_ATTRACTION_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenBgLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAttractionLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAttractionLowerInputListener.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerInputListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenAttractionFrame :
  public JoinFestaSubScreenFrameBase,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenAttractionLowerInputListener,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenSelectLowerInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenAttractionFrame );
public:
  JoinFestaSubScreenAttractionFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenAttractionFrame();

public:

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenAttractionLowerInputListener
  //-----------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  /**
   * @brief   中止ボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnStopButtonAction();

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenSelectLowerInputListener
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

  //------------------
  // ビュー操作
  //------------------
  //! アトラクション中画面に変更
  void ChangeAttractionView();
  //! 中止画面に変更
  void ChangeStopView();

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ATTRACTION_FRAME_H_INCLUDE

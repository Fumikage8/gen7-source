// ============================================================================
/*
 * @file JoinFestaSubScreenMainFrame.h
 * @brief ジョインフェスタ下画面のメインフレームです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_MAIN_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_MAIN_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenBgLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenMainLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenMainLowerInputListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenMainFrame :
  public JoinFestaSubScreenFrameBase,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenMainLowerInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenMainFrame );
public:
  JoinFestaSubScreenMainFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenMainFrame();

public:

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenMainLowerInputListener
  //-----------------------------------------------------------------------------
  //! 対戦ボタン通知
  virtual void OnBattleButtonAction();
  //! 交換ボタン通知
  virtual void OnTradeButtonAction();
  //! ストリーミングメッセージタッチ通知
  virtual void OnStreamMessageAction();
  //! インターネットボタン通知
  virtual void OnInternetButtonAction();
  //! お知らせボタン通知
  virtual void OnInfoButtonAction();
  //! プレイヤーリストボタン通知
  virtual void OnPlayerListButtonAction();
  //! Myプロフィールボタン通知
  virtual void OnMyProfileButtonAction();
  //! ポケモンリストボタン通知
  virtual void OnPokelistButtonAction();
  //! バッグボタン通知
  virtual void OnBagButtonAction();
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
  static const u32 UPDATE_ICON_ROTATE_TIME   = 6000; //!< リストボタン表示更新のローテション間隔（ms）

private:

  u64   m_updateIconRotateTime;   //! リストボタン表示更新のローテションタイム


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_MAIN_FRAME_H_INCLUDE

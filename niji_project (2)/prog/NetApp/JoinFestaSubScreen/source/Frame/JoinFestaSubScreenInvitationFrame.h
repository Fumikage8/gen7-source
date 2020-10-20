// ============================================================================
/*
 * @file JoinFestaSubScreenInvitationFrame.h
 * @brief ジョインフェスタ下画面のお誘い通知フレームです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_INVITATION_FRAME_H_INCLUDE )
#define JOINFESTASUBSCREEN_INVITATION_FRAME_H_INCLUDE
#pragma once

// FrameBase
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// 使用するView
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenInvitationLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenInvitationLowerInputListener.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerView.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerInputListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSubScreenInvitationFrame :
  public JoinFestaSubScreenFrameBase,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenInvitationLowerInputListener,
  public NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenSelectLowerInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenInvitationFrame );
public:
  JoinFestaSubScreenInvitationFrame(
    NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
    NetApp::JoinFestaSubScreen::View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView );
  virtual~JoinFestaSubScreenInvitationFrame();

public:

  //! @brief システム側で一時停止呼び出し時にコールされる
  virtual void OnSuspend();
  //! @brief システム側で再起動呼び出し時にコールされる
  virtual void OnResume();

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenInvitationLowerInputListener
  //-----------------------------------------------------------------------------
  //! 申し込みOKボタン通知
  virtual void OnOkButtonAction();
  //! お断りリストボタン通知
  virtual void OnBlackListButtonAction();
  //! 申し込みNGボタン（戻るボタン）通知
  virtual void OnNgButtonAction();

  //-----------------------------------------------------------------------------
  // JoinFestaSubScreenMainLowerInputListener
  //-----------------------------------------------------------------------------
  //! ボタン通知
  virtual void OnButtonAction( int buttonId );
  //! 戻るボタン通知
  virtual void OnBackButtonAction();
  //! 「おことわりリストを　こうしんしました」メッセージ終了通知
  virtual void OnBlackListUpdateMessageFinish();
  //!「つうしんが　ちゅうだんされました」メッセージ終了通知
  virtual void OnNetworkErrorMessageFinish();

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
  //! お誘い画面に変更
  void ChangeInvitationView();
  //! お断りリスト追加画面に変更
  void ChangeBlackListAddView();

  //------------------
  // 通信
  //------------------
  //! キャンセル送信
  void SendCancelRequest();


  // @fix GFNMcat[631] 違うお誘いでマッチングしてしまう
  //! パケットイベントの一致チェック
  bool CheckPacketEventAgree() const;


};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_INVITATION_FRAME_H_INCLUDE

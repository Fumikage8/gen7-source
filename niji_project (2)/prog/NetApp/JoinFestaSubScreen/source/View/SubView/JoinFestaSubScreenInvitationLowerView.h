// ============================================================================
/*
 * @file JoinFestaSubScreenInvitationLowerView.h
 * @brief ジョインフェスタ下画面のお誘い画面を表示するクラスです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_INVITATE_LOWER_VIEW_H_INCLUDE )
#define JOINFESTASUBSCREEN_INVITATE_LOWER_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"

// MyStatus
#include "Savedata/include/MyStatus.h"

#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenInvitationLowerInputListener.h"

// TimeLimit
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenInvitationLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenInvitationLowerView );
public:
  JoinFestaSubScreenInvitationLowerView( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenInvitationLowerView();

  void SetListener( JoinFestaSubScreenInvitationLowerInputListener* pJoinFestaSubScreenInvitationLowerInputListener ) { m_pListener = pJoinFestaSubScreenInvitationLowerInputListener; }
  void RemoveListener() { m_pListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

  //! 一時停止
  void Suspend();
  //! 再起動
  void Resume();

public:

  // お誘いメッセージ表示
  void SetInvitationMessage( Define::PacketEventType packetEventType, const Savedata::MyStatus& myStatus );

  // タイムアウト計測開始
  void TimeOutStart();
  // タイムアウトチェック
  bool IsTimeOut();
  // 計測一時停止
  void TimeOutSuspend();
  // 計測再開
  void TimeOutResume();

  // 残り時間の設定(ms)
  void SetTimeLimit( u32 value );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  /**
   * @enum ButtonId
   * ボタンID定義
   */
  enum ButtonID
  {
    BUTTON_ID_OK,         //! 申し込みOKボタン
    BUTTON_ID_BLACKLIST,  //! お断りリストボタン
    BUTTON_ID_BACK,       //! 戻るボタン
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();

private:

  //! 自動で閉じる時間（ms）
  static const u32 AUTO_CLOSE_TIME = 30000; // 30秒


  ApplicationSystem::JoinFestaSubScreenWork*  m_pJoinFestaSubScreenWork;
  JoinFestaSubScreenInvitationLowerInputListener*   m_pListener;

  // 制限時間
  NetAppLib::Util::NetAppTimeout  m_autoCloseTimeout; //! 自動で閉じる制限時間

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_INVITATE_LOWER_VIEW_H_INCLUDE

// ============================================================================
/*
 * @file JoinFestaSubScreenMainLowerView.h
 * @brief ジョインフェスタ下画面のメイン画面を表示するクラスです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_MAIN_LOWER_VIEW_H_INCLUDE )
#define JOINFESTASUBSCREEN_MAIN_LOWER_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenMainLowerInputListener.h"

// InfomatinMessageスクロール用
#include  <NetStatic/NetAppLib/include/Message/NetAppTelopTagProcessor.h>

// WiFi強制切断リスナー
#include "NetStatic/NetLib/include/Error/NijiNetworkForceDisconnectListener.h"
// システムイベントリスナ
#include <System/include/SystemEventListener.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)
class InformationMessageData;
GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenMainLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public NetLib::Error::NijiNetworkForceDisconnectListener,
  public System::SystemEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenMainLowerView );
public:
  JoinFestaSubScreenMainLowerView( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenMainLowerView();

  void SetListener( JoinFestaSubScreenMainLowerInputListener* pJoinFestaSubScreenMainLowerInputListener ) { m_pListener = pJoinFestaSubScreenMainLowerInputListener; }
  void RemoveListener() { m_pListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );


  // NijiNetworkForceDisconnectListener
  //------------------------------------------------------------------
  /**
  * @brief WiFi強制切断通知
  */
  //------------------------------------------------------------------
  virtual void OnForceDisconnect();

  // System::SystemEventListener
  //------------------------------------------------------------------
  /**
  * @brief スリープからの復帰時イベント（スリープ禁止の場合は呼ばれない）
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithSleep( void );

  //------------------------------------------------------------------
  /**
  * @brief ホームメニューからの復帰時イベント
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithHomeMenu( void );

  //! 一時停止
  void Suspend();
  //! 再起動
  void Resume();

  //! メッセージ再生更新
  void PlayMessageUpdate();
  //! メッセージ再生
  void PlayMessage( NetAppLib::JoinFesta::InformationMessageData& data );
  //! メッセージ再生中か判定
  bool IsPlayMessage();
  //! メッセージ強制非表示
  void ForceMessageHide();

  //! プレイヤーリストボタンの「！」アイコン表示状態更新
  void UpdatePlayerListModeIcon();

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
    BUTTON_ID_BATTLE,     //! 対戦ボタン
    BUTTON_ID_TRADE,      //! 交換ボタン
    BUTTON_ID_STREAM_MSG, //! ストリーミングメッセージ
    BUTTON_ID_PLAYER_LIST,//! プレイヤーリストボタン
    BUTTON_ID_MY_PROFILE, //! Myプロフィールボタン
    BUTTON_ID_POKELIST,   //! ポケモンリストボタン
    BUTTON_ID_BAG,        //! バッグボタン
    BUTTON_ID_INTERNET,   //! インターネットボタン
    BUTTON_ID_BACK,       //! 戻るボタン
    BUTTON_MAX,
  };

  enum PlayMessageState
  {
    PLAY_MESSAGE_STATE_NONE,
    PLAY_MESSAGE_STATE_START,
    PLAY_MESSAGE_STATE_BG_IN,
    PLAY_MESSAGE_STATE_PLAYING,
    PLAY_MESSAGE_STATE_BG_OUT,
  };

  //! ストリームメッセージが流れる設定
  // ○○Pxを○○フレームで流れる
  static const u32 STREAM_MESSAGE_PLAY_PX = 300.0f;
  static const u32 STREAM_MESSAGE_PLAY_FRAME = 100.0f;

  void setupTextMessage();
  void setupButton();

  void updateNetworkModeText();
  void updatePlayMessage();

private:

  ApplicationSystem::JoinFestaSubScreenWork*  m_pJoinFestaSubScreenWork;
  JoinFestaSubScreenMainLowerInputListener*   m_pListener;
  gfl2::lyt::LytTextBox*                      m_pStreamMessagePane;
  PlayMessageState                            m_ePlayMessageState;
  nw::math::VEC3                              m_MessagePosition;
  bool                                        m_isWLANSwitchEnable; //! 無線スイッチフラグk

  // ストリームメッセージ用
  NetAppLib::Message::TelopTagProcessor       m_TelopProcessor;
  gfl2::lyt::LytPane*                         m_pStremMessageBgPane;
  nw::math::VEC3                              m_MessageBgPosition;
  u8                                          m_bgInFrame;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_MAIN_LOWER_VIEW_H_INCLUDE

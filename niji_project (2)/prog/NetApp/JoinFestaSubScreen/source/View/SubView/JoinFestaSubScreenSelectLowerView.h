// ============================================================================
/*
 * @file JoinFestaSubScreenSelectLowerView.h
 * @brief ジョインフェスタ下画面の選択画面の入力イベントを受け取るリスナーです。
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_SELECT_LOWER_VIEW_H_INCLUDE )
#define JOINFESTASUBSCREEN_SELECT_LOWER_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenSelectLowerInputListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenSelectLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public NetAppLib::UI::NetAppCommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenSelectLowerView );
public:

  /**
   * @enum ButtonTextMode
   * ボタンテキストモード
   */
  enum ButtonTextMode{
    BUTTON_TEXT_MODE_ASK_END,     //! 「終了する」のボタン
    BUTTON_TEXT_MODE_ADD,         //! 「追加する」のボタン
    BUTTON_TEXT_MODE_TRADE,       //! 「通信交換/GTS/ミラクル交換」のボタン
    BUTTON_TEXT_MODE_BATTLE,      //! 「通信対戦/バトルスポット/ルールDL」のボタン
    BUTTON_TEXT_MODE_RULE,        //! 「シングル/ダブル/マルチ/ロイヤル」のボタン
    BUTTON_TEXT_MODE_BOSYUU,      //! 「相手を選んで申し込む/募集誰でも/募集フレンド」のボタン
    BUTTON_TEXT_MODE_ATTRACTION_END, //! アトラクション中専用「終了する」のボタン
    BUTTON_TEXT_MODE_RETURN,      //! 「戻る」のボタンのみ
  };

  /**
   * @enum ButtonId
   * ボタンID定義
   */
  enum ButtonId
  {
    // ボタン1つ
    BUTTON_ID_1BUTTON = 0,

    // ボタン3つ
    BUTTON_ID_3BUTTON_01,
    BUTTON_ID_3BUTTON_02,
    BUTTON_ID_3BUTTON_03,

    // ボタン4つ
    BUTTON_ID_4BUTTON_01,
    BUTTON_ID_4BUTTON_02,
    BUTTON_ID_4BUTTON_03,
    BUTTON_ID_4BUTTON_04,

    BUTTON_ID_RETURN,         //! 戻るボタン

    BUTTON_ID_MAX = 9, // 8ボタン+戻るボタン
  };

  enum tmpId 
  {
    //=====================================
    // BUTTON_TEXT_MODE_ADD
    BUTTON_ID_ADD = BUTTON_ID_1BUTTON,    //! 追加するボタン

    //=====================================
    // BUTTON_TEXT_MODE_BATTLE
    BUTTON_ID_NBR           = BUTTON_ID_3BUTTON_01, //! 通信対戦
    BUTTON_ID_BTLSPOT       = BUTTON_ID_3BUTTON_02, //! バトルスポットボタン
    BUTTON_ID_RULE_DL       = BUTTON_ID_3BUTTON_03, //! ルールDLボタン

    //=====================================
    // BUTTON_TEXT_MODE_RULE
    BUTTON_ID_SINGLE        = BUTTON_ID_4BUTTON_01, //! シングルバトルボタン
    BUTTON_ID_DOUBLE        = BUTTON_ID_4BUTTON_02, //! ダブルバトルボタン
    BUTTON_ID_MULTI         = BUTTON_ID_4BUTTON_03, //! マルチバトルボタン
    BUTTON_ID_ROYAL         = BUTTON_ID_4BUTTON_04, //! ロイヤルバトルボタン

    //=====================================
    // BUTTON_TEXT_MODE_BOSYUU
    BUTTON_ID_SELECT        = BUTTON_ID_3BUTTON_01, //! 相手を選んで申し込むボタン
    BUTTON_ID_ANYONE        = BUTTON_ID_3BUTTON_02, //! 誰でも募集ボタン
    BUTTON_ID_FRIEND        = BUTTON_ID_3BUTTON_03, //! フレンド募集ボタン
  };

public:
  JoinFestaSubScreenSelectLowerView( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenSelectLowerView();

  void SetListener( JoinFestaSubScreenSelectLowerInputListener* pJoinFestaSubScreenSelectLowerInputListener ) { m_pListener = pJoinFestaSubScreenSelectLowerInputListener; }
  void RemoveListener() { m_pListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  // NetAppCommonMessageWindowManipulator::IEventHandler
  virtual void CommonMessageWindowManipulator_OnEvent( const NetAppLib::UI::NetAppCommonMessageWindowManipulator::EventID eEventID );

  //! 一時停止
  void Suspend();
  //! 再起動
  void Resume();

  void SetVisibleSelectButton( bool bVisible );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );

public:

  /**
  * @breif  ボタン設定
  * @param  mode  ButtonTextMode
  */
  void SetupButton( const ButtonTextMode mode );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  void setButtonText( const ButtonTextMode mode );
  void setupMessageWindowManipulator();
  void showMessage( u32 messageID, bool isImmediateMessage, bool isShowTimerIcon );

private:

  ApplicationSystem::JoinFestaSubScreenWork*                   m_pJoinFestaSubScreenWork;
  JoinFestaSubScreenSelectLowerInputListener*                  m_pListener;
  u32                                                          m_MessageID;
  NetAppLib::UI::NetAppCommonMessageWindowManipulator          m_MessageWindowManipulator;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_SELECT_LOWER_VIEW_H_INCLUDE

// ============================================================================
/*
 * @file JoinFestaSubScreenAttractionLowerView.h
 * @brief ジョインフェスタ下画面のお誘い画面を表示するクラスです。
 * @date 2015.10.29
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ATTRACTION_LOWER_VIEW_H_INCLUDE )
#define JOINFESTASUBSCREEN_ATTRACTION_LOWER_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"

// MyStatus
#include "Savedata/include/MyStatus.h"

#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAttractionLowerInputListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenAttractionLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenAttractionLowerView );
public:
  JoinFestaSubScreenAttractionLowerView( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenAttractionLowerView();

  void SetListener( JoinFestaSubScreenAttractionLowerInputListener* pJoinFestaSubScreenAttractionLowerInputListener ) { m_pListener = pJoinFestaSubScreenAttractionLowerInputListener; }
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

  //! 残り時間の設定
  void SetTimeLimit( u32 minute, u32 second );
  //! トータルスコアの設定
  void SetTotalScore( u32 score );
  //! 個人スコアの設定
  void SetMyScore( u32 score );
  //! 参加人数の設定
  void SetJoinNum( u32 num );

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
    BUTTON_ID_STOP,       //! 中止ボタン
    BUTTON_MAX,
  };

  void setupTextMessage();
  void setupButton();

private:

  ApplicationSystem::JoinFestaSubScreenWork*  m_pJoinFestaSubScreenWork;
  JoinFestaSubScreenAttractionLowerInputListener*   m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ATTRACTION_LOWER_VIEW_H_INCLUDE

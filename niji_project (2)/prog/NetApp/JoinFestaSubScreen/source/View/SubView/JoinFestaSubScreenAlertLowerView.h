// ============================================================================
/*
 * @file JoinFestaSubScreenAlertLowerView.h
 * @brief ジョインフェスタ下画面のアラート画面です。
 * @date 2016.01.07
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_ALERT_LOWER_VIEW_H_INCLUDE )
#define JOINFESTASUBSCREEN_ALERT_LOWER_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/View/SubView/JoinFestaSubScreenAlertLowerInputListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSubScreenAlertLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenAlertLowerView );
public:


  /**
   * @enum ButtonId
   * ボタンID定義
   */
  enum ButtonId
  {
    BUTTON_ID_RETURN,         //! 戻るボタン
    BUTTON_ID_MAX
  };


public:
  JoinFestaSubScreenAlertLowerView( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork );
  virtual~JoinFestaSubScreenAlertLowerView();

  void SetListener( JoinFestaSubScreenAlertLowerInputListener* pJoinFestaSubScreenAlertLowerInputListener ) { m_pListener = pJoinFestaSubScreenAlertLowerInputListener; }
  void RemoveListener() { m_pListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  //! 一時停止
  void Suspend();
  //! 再起動
  void Resume();

  void SetMessage( u32 messageID );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTASUBSCREEN,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  void setupButton();

private:

  ApplicationSystem::JoinFestaSubScreenWork*    m_pJoinFestaSubScreenWork;
  JoinFestaSubScreenAlertLowerInputListener *   m_pListener;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREEN_ALERT_LOWER_VIEW_H_INCLUDE

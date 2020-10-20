// ============================================================================
/*
 * @file JoinFestaRecordDetailLowerView.h
 * @brief レコード詳細画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARECORDDETAILLOWERVIEW_H_INCLUDE )
#define JOINFESTARECORDDETAILLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordDetailLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRecordDetailLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRecordDetailLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_MAX
  };

public:

  JoinFestaRecordDetailLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaRecordDetailLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener* pJoinFestaRecordDetailLowerViewListener ) { m_pJoinFestaRecordDetailLowerViewListener = pJoinFestaRecordDetailLowerViewListener; }
  void RemoveListener() { m_pJoinFestaRecordDetailLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void SetRecordData( NetAppLib::JoinFestaUI::JoinFestaRecordData& data );

private:

  void setupButton();

  void setMessage( u32 paneID, u32 messageID, bool bEnable, bool bVisible );
  void setParam( u32 paneID, u32 param, bool bEnable, bool bVisible );

private:

  NetAppLib::JoinFestaUI::JoinFestaRecordDetailLowerViewListener*   m_pJoinFestaRecordDetailLowerViewListener;
  NetAppLib::System::ApplicationWorkBase*                           m_pWorkBase;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARECORDDETAILLOWERVIEW_H_INCLUDE

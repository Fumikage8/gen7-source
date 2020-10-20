// ============================================================================
/*
 * @file JoinFestaPlayerListLowerViewBase.h
 * @brief JFPL_MIN_LOW_000.flyt を扱う基底クラスです
 * @date 2015.10.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTLOWERVIEWBASE_H_INCLUDE )
#define JOINFESTAPLAYERLISTLOWERVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListLowerViewBase : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListLowerViewBase );

protected:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

public:

  JoinFestaPlayerListLowerViewBase(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaPlayerListLowerViewBase();

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;

  void SetVisibleUpdateButton( bool bVisible );
  void SetVisibleDecideButton( bool bVisible );
  void SetVisibleInfoButton( bool bVisible );
  void SetVisibleMenuCursor( bool bVisible );
  void SetVisibleBackButton( bool bVisible );
  void SetVisiblePhotoButton( bool bVisible );
  void SetVisibleSlideshowButton( bool bVisible );

protected:

  void setScreenTitleName( u32 messageID );

protected:

  NetAppLib::System::ApplicationWorkBase*   m_pWorkBase;
  NetAppLib::UI::NetAppMenuCursor           m_MenuCursor;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTLOWERVIEWBASE_H_INCLUDE

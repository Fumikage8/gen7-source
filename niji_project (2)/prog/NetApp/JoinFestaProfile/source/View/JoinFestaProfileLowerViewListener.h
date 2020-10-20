// ============================================================================
/*
 * @file JoinFestaProfileLowerViewListener.h
 * @brief プロフィールアプリの下画面の入力イベントを受け取るリスナーです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILELOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTAPROFILELOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppViewListenerBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileTypes.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaProfileLowerViewListener : public NetAppLib::System::NetAppViewListenerBase
{
public:

  JoinFestaProfileLowerViewListener() :
    m_pSelectEditMenuInfo( NULL )
  {
  }
  ~JoinFestaProfileLowerViewListener()
  {
  }

  enum InputEventID
  {
    INPUT_EVENT_ID_TOUCH_BACK_BUTTON,
    INPUT_EVENT_ID_SELECT_EDIT_MENU,
    INPUT_EVENT_ID_TOUCH_INFO_BUTTON
  };

  void TouchBackButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_BACK_BUTTON ); }
  void SetSelectEditMenu( NetApp::JoinFestaProfile::System::EditMenuInfo* pEditMenuInfo ){ SetInputEventID( INPUT_EVENT_ID_SELECT_EDIT_MENU ); m_pSelectEditMenuInfo = pEditMenuInfo; }
  void TouchInfoButton(){ SetInputEventID( INPUT_EVENT_ID_TOUCH_INFO_BUTTON ); }

  NetApp::JoinFestaProfile::System::EditMenuInfo* GetSelectEditMenu() { return m_pSelectEditMenuInfo; }

private:

  NetApp::JoinFestaProfile::System::EditMenuInfo*     m_pSelectEditMenuInfo;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILELOWERVIEWLISTENER_H_INCLUDE

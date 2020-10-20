//=============================================================================
/**
 * @file    JoinFestaBeaconLowerView.h
 * @brief   ビーコン下画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( JOINFESTABEACON_LOWERVIEW_H_INCLUDED )
#define JOINFESTABEACON_LOWERVIEW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

// 前方宣言
namespace NetApp
{
  namespace JoinFestaBeacon
  {
    namespace System
    {
      class JoinFestaBeaconWork;
    }
  }
}


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(View)

class JoinFestaBeaconUpperView;

//=============================================================================
/**
 * @class JoinFestaBeaconLowerView
 * @brief 
 * @date  2015.07.15
 */
//=============================================================================
class JoinFestaBeaconLowerView
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
private:

  enum
  {
    BUTTON_PANE_MAX = 2,//ボタンの数
    TIMER_WAIT_FRAME = 45,
    PLAYER_NET_ID_NULL = -1,
  };



  enum LytID
  {
    LYTID_MAIN = 0,

    LYTID_MAX
  };




  enum ButtonID
  {
    BUTTON_KEY_CANCEL = 0,

    BUTTON_MAX
  };

public:
  enum SELECT_RESULT
  {
    RESULT_NONE = 0,
    RESULT_YES,
    RESULT_NO,
    RESULT_MESSAGE_END,
    RESULT_EXIT,
    RESULT_EOM,
    RESULT_BUTTON
  };


public:
  JoinFestaBeaconLowerView( NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* m_pJoinFestaBeaconWork , NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu , 
    app::util::Heap * pHeap , NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* pUpperView );
  virtual ~JoinFestaBeaconLowerView();

  void SetupSelectMode( void );
  void SetupYesNoMode( void );
  void SetupMessageStreamMode( u32 messageID );
  void SetupMessageTimerMode( u32 messageID , bool isBGVisible = true );
  void SetRegisterPlayerName( u32 index );
  void SetPlayerNetIDforName( s32 playerNetIDForName );

  NetApp::JoinFestaBeacon::View::JoinFestaBeaconLowerView::SELECT_RESULT GetResult( void );

  void HideButton( void );
  void HideReturn( void );

private:

  void ShowMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void SetupFixedLayout( void );



public:

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );



  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

private:

  void CreateLayout(void);
  void DeleteLayout(void);


  void CreateButton(void);
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

  



private:
  app::util::Heap * m_heap;
  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット
  bool m_isDrawEnable;
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork* m_pJoinFestaBeaconWork;
  NetAppLib::UI::NetAppMessageMenu& m_netAppMessageMenu;
  SELECT_RESULT m_result;
  NetApp::JoinFestaBeacon::View::JoinFestaBeaconUpperView* m_pUpperView;

  bool m_isButtonHide;
  s32 m_playerNetIDForName;
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
#endif  // JOINFESTABEACON_LOWERVIEW_H_INCLUDED

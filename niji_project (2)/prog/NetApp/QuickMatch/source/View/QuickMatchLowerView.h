//=============================================================================
/**
 * @file    QuickMatchLowerView.h
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( QUICKMATCH_LOWERVIEW_H_INCLUDED )
#define QUICKMATCH_LOWERVIEW_H_INCLUDED

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
  namespace QuickMatch
  {
    namespace System
    {
      class QuickMatchWork;
    }
  }
}


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(View)

class QuickMatchUpperView;

//=============================================================================
/**
 * @class QuickMatchLowerView
 * @brief 
 * @date  2015.07.15
 */
//=============================================================================
class QuickMatchLowerView
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
private:

  enum
  {
    BUTTON_PANE_MAX = 5,//ボタンの数
    TIMER_WAIT_FRAME = 30,
    BUTTON_LIST_MAX = 4,
    PLAYER_NET_ID_NULL = -1,
  };



  enum LytID
  {
    LYTID_MAIN = 0,

    LYTID_MAX
  };




  enum ButtonID
  {
    BUTTON_1 = 0,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,

    BUTTON_KEY_CANCEL,

    BUTTON_MAX
  };

public:
  enum SELECT_RESULT
  {
    RESULT_NONE = 0,
    RESULT_YES,
    RESULT_NO,
    RESULT_MESSAGE_END,
    RESULT_SINGLE,
    RESULT_DOUBLE,
    RESULT_TRADE,
    RESULT_CHANGE,
    RESULT_EXIT,
    RESULT_EOM
  };


public:
  QuickMatchLowerView( NetApp::QuickMatch::System::QuickMatchWork* m_pQuickMatchWork , NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu , 
    app::util::Heap * pHeap , NetApp::QuickMatch::View::QuickMatchUpperView* pUpperView );
  virtual ~QuickMatchLowerView();

  void SetupSelectMode( void );
  void SetupYesNoMode( void );
  void SetupMessageStreamMode( u32 messageID );
  void SetupMessageTimerMode( u32 messageID , bool isBGVisible = true );
  void SetRegisterPlayerName( void );
  void SetPlayerNetIDforName( s32 playerNetIDForName );

  NetApp::QuickMatch::View::QuickMatchLowerView::SELECT_RESULT GetResult( void );

  void HideListLayout( void );
  void HideAllLayout( void );

private:

  void ShowMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void SetupFixedLayout( void );

  void MoveMenuCursor( s32 mv );

  bool CreateMenuCursor(void);
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2015.06.02
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2015.06.02
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

public:

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );



  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

private:

  void UpdateMain( void );

  void CreateLayout(void);
  void DeleteLayout(void);


  void CreateButton(void);
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

  



private:
  app::util::Heap * m_heap;
  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット
  bool m_isDrawEnable;
  NetApp::QuickMatch::System::QuickMatchWork* m_pQuickMatchWork;
  NetAppLib::UI::NetAppMessageMenu& m_netAppMessageMenu;
  SELECT_RESULT m_result;
  NetApp::QuickMatch::View::QuickMatchUpperView* m_pUpperView;
  s32 m_cursorPos;
  app::tool::MenuCursor * m_pMenuCursor;
  void * m_pMenuCursorBuffer;
  bool m_isListHide;
  app::util::KeyRepeatController* m_pKeyRepeatController;
  s32 m_playerNetIDForName;
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
#endif  // QUICKMATCH_LOWERVIEW_H_INCLUDED

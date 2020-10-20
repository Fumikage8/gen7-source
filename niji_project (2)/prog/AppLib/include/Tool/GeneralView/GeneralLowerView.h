
#if !defined(GENERALLOWERVIEW_H_INCLUDED)
#define GENERALLOWERVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GeneralLowerView.h
 * @date   2015/09/30 Wed. 12:07:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

/*    */
#include  <Layout/include/gfl2_LytSys.h>


#include  "AppLib/include/Ui/UIView.h"
#include  "AppLib/source/Tool/GeneralView/Module/CursorController.h"
#include  "AppLib/include/Tool/GeneralView/Module/IResourceIDHandler.h"
#include  "AppLib/include/Tool/GeneralView/Module/IResourceProvider.h"

namespace app   {
namespace tool  {
  class ResourceLoader;
  class CursorController;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/09/28 Mon. 16:06:46
  */
//==============================================================================
class GeneralLowerView
  : public    app::ui::UIView
  , public    app::ui::UIInputListener
  , protected CursorController::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(GeneralLowerView);
public:
  /**
    内部イベント取得ハンドラ
  */
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  button  */
      EVENT_OnButton0,      /**<  ボタン0が押された     */
      EVENT_OnButton1,      /**<  ボタン1が押された     */
      EVENT_OnButton2,      /**<  ボタン2が押された     */
      EVENT_OnButton3,      /**<  ボタン3が押された     */
      EVENT_OnButtonBack,   /**<  戻るボタンが押された  */
      
      /*  others  */
      EVENT_OnViewMode,     /**<  ViewModeが変更された  */

      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  GeneralLowerView_OnEvent(GeneralLowerView* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    *  @param    [in] displayType         描画対象LCD
    */
  //------------------------------------------------------------------
  GeneralLowerView(
    app::util::Heap*                              pHeap,
    app::util::AppRenderingManager*               pRenderingManager,
    const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType   = app::util::G2DUtil::SETUP_LOWER
  );

  /**
    dtor
  */
  virtual ~GeneralLowerView();


  /*
    setup
  */
  //------------------------------------------------------------------
  /**
    *  @brief   リソース読み込みやSetupなどの処理を進行させる
    */
  //------------------------------------------------------------------
  bool  ObserveSetup(void);


  /*
    表示モード
  */
  enum  LayoutMode
  {
    LAYOUTMODE_NoButton,                    /**<  ボタンなし                              */
    LAYOUTMODE_1Button,                     /**<  1ボタン                                 */
    LAYOUTMODE_2Buttons,                    /**<  2ボタン                                 */
    LAYOUTMODE_3Buttons,                    /**<  3ボタン                                 */
    LAYOUTMODE_4Buttons,                    /**<  4ボタン                                 */

    LAYOUTMODE_NoButton_WithMessage,        /**<  ボタンなし（メッセージウインドウあり）  */
    LAYOUTMODE_1Button_WithMessage,         /**<  4ボタン（メッセージウインドウあり）     */
    LAYOUTMODE_2Buttons_WithMessage,        /**<  4ボタン（メッセージウインドウあり）     */
    LAYOUTMODE_3Buttons_WithMessage,        /**<  4ボタン（メッセージウインドウあり）     */
    LAYOUTMODE_4Buttons_WithMessage,        /**<  4ボタン（メッセージウインドウあり）     */
    
    LAYOUTMODE_INVALID
  };
  //------------------------------------------------------------------
  /**
    *  @brief    表示するオブジェクトを変更する
    */
  //------------------------------------------------------------------
  void        SetLayoutMode(const LayoutMode layoutMode, const bool bEnableBackButton = true, const bool bShowBG = false);
  LayoutMode  GetLayoutMode(void) const {return m_LayoutMode;}


  /*
    cursor
  */
  void  CursorMoveTo(const u32 locationIndex) {m_CursorController.MoveTo(locationIndex);}


  /*
    BG
  */
  void  SetBGVisibility(const bool isVisible);



  /*
    TextBox
  */
  //------------------------------------------------------------------
  /**
    *  @brief    textboxに文字列を設定する
    *            必要に応じてExpandも行う
    *  @note     Register...系はGetG2DUtil()経由で行う
    */
  //------------------------------------------------------------------
  enum  TextBoxID
  {
    TEXTBOX_Button0,
    TEXTBOX_Button1,
    TEXTBOX_Button2,
    TEXTBOX_Button3,
    
    TEXTBOX_MAX
  };
  void  SetTextBoxString(const TextBoxID textBoxID, gfl2::str::StrBuf* pStr, const bool isExpand = false);


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetButtonSEtoDefault(void);


  /*  buttonID  */
  enum ButtonID
  {
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_Back,
    
    BUTTON_MAX
  };

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    ショートカット登録
    */
  //------------------------------------------------------------------
  void  RegisterShortcut_A(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_B = buttonID;};
  void  RegisterShortcut_B(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_B = buttonID;};
  void  RegisterShortcut_X(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_X = buttonID;};
  void  RegisterShortcut_Y(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_Y = buttonID;};
  void  RegisterShortcut_L(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_L = buttonID;};
  void  RegisterShortcut_R(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_R = buttonID;};



  /*
    resource
  */
  static u32  GetResourceNum(void);                             /**<  要求リソース数を取得                                        */
  static void GetResourceIDs(IResourceIDHandler& rHandler);     /**<  要求リソースのIDを通知させる(GetResourceNum分コールされる)  */
  //------------------------------------------------------------------
  /**
    *  @brief    リソース提供I/F
    *            設定しなければクラス内で読み込む
    *  @note     内部ローダを置き換える場合、ObserveSetup前にコールすること
    */
  //------------------------------------------------------------------
  void  SetResourceProvider(IResourceProvider* pProvider) {m_pResourceProvider  = pProvider;}



  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;

protected:
  static const u32  RESOURCE_LOADER_MAX = 2;

  class DefaultResourceProvider;

  /*  implement UIInputListener I/F  */
  virtual void                                      OnLayoutPaneTouchTriggerEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  /*  implement CursorController::IEventHandler  */
  virtual void  CursorController_OnEvent(CursorController* pSender, const CursorController::IEventHandler::EventCode eventCode);


  /*
    funcs
  */
  void    Event(const IEventHandler::EventCode eventCode);
  void    Setup(void);

  /*
    members
  */
  app::util::Heap*                            m_pHeap;
  app::util::AppRenderingManager*             m_pRenderingManager;
  app::util::G2DUtil::SETUP_DISPLAY_TYPE      m_DisplayType;
  u32                                         m_State;
  LayoutMode                                  m_LayoutMode;
  ResourceLoader*                             m_pResourceLoaders[RESOURCE_LOADER_MAX];

  /*  handler  */
  IEventHandler*                              m_pEventHandler;
  IResourceProvider*                          m_pResourceProvider;

  /*  cursor  */
  CursorController                            m_CursorController;

  /*  panes  */
  gfl2::lyt::LytTextBox*                      m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*                         m_pBGPane;

  /*  shortcut  */
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_A;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_B;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_X;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_Y;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_L;
  app::tool::ButtonManager::ButtonId  m_ShortcutButton_R;
};




} /*  namespace app   */
} /*  namespace tool  */
#endif  /*  #if !defined(GENERALLOWERVIEW_H_INCLUDED)  */

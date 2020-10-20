#if !defined(APP_TOOL_2BUTTONSVIEW_H_INCLUDED)
#define APP_TOOL_2BUTTONSVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   app_tool_2ButtonsView.h
 * @date   2015/09/08 Tue. 14:33:57
 * @author muto_yu
 * @brief  Menu2_MIN_LOW_000.flytによる2ボタンview
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <AppLib/include/Ui/UIView.h>

#include  "AppLib/source/Tool/GeneralView/Module/CursorController.h"


namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  Menu2_MIN_LOW_000.flytによる2ボタンview
  *          リソース読み込みを内部で行うか、外部提供とするかを選択可能
  *          カーソル制御をサポートするため、一部InputListenerを実装している
  *  @note   リソースは下画面用
  *  @code  
  *       
  *   //  リソース読み込みを内部に任せる場合
  *   //
  *   bool init()
  *   {
  *     bool isInitEnd = false;
  *
  *     switch(mSeq)
  *     {
  *       case  SEQ_1st:
  *         m_pTwoButtonView  = GFL_NEW(pHeapBase) TwoButtonsView(pHeap, pRenderingManager);    //  isAutoSetup == true
  *         m_pTwoButtonView->SetEventHandler(this);
  *         mSeq  = SEQ_Loading;
  *         break;
  *
  *       case  SEQ_Loading;
  *         if(m_pTwoButtonView->ObserveSetup())
  *         {
  *           isInitEnd = true;
  *         }
  *         break;
  *     }
  *   }
  *
  *   //  Update...
  *   //  Draw...
  *
  *
  *   //  リソースの提供を外部から受ける場合
  *   //
  *   bool init()
  *   {
  *     bool isInitEnd = false;
  *
  *     switch(mSeq)
  *     {
  *       case  SEQ_1st:
  *         m_pTwoButtonView  = GFL_NEW(pHeapBase) TwoButtonsView(pHeap, pRenderingManager, false);    //  isAutoSetup == false
  *         m_pTwoButtonView->SetEventHandler(this);
  *         mSeq  = SEQ_Loading;
  *         break;
  *
  *       case  SEQ_Loading;
  *         //  外部読み込みシステムなどからリソースデータを受け、手動でSetup
  *         if(pLoadSystem->IsEnd())
  *         {
  *           m_pTwoButtonView->Setup(pLoadSystem->GetLayoutResource(), pLoadSystem->GetCursorResource())
  *           isInitEnd = true;
  *         }
  *         break;
  *     }
  *   }
  *
  *   //  Update...
  *   //  Draw...
  *  @endcode
  *  @date   2015/09/08 Tue. 14:35:21
  */
//==============================================================================
class TwoButtonsView
  : public    app::ui::UIView
  , public    app::ui::UIInputListener
  , protected CursorController::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(TwoButtonsView);
public:
  /**
    内部イベント取得ハンドラ
  */
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_OnButton1Click,
      EVENT_OnButton2Click,
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  TwoButtonsView_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    *  @param    [in] isAutoSetup         リソース読み込みを内部で行い、Setupまでを自動で行うか（ObserveSetup()によって終了を監視する）
    *  @param    [in] displayType         描画対象LCD
    */
  //------------------------------------------------------------------
  TwoButtonsView(
    app::util::Heap*                              pHeap,
    app::util::AppRenderingManager*               pRenderingManager,
    const bool                                    isAutoSetup   = true,
    const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType   = app::util::G2DUtil::SETUP_LOWER
  );

  /**
    dtor
  */
  virtual ~TwoButtonsView();



  //------------------------------------------------------------------
  /**
    *  @brief   内部インスタンスの構築
    *  @note    ctorのisAutoLoadAndSetup==false時に手動でsetupする（ObserveSetup()から自動的に呼ばれる）
    *           true時は自動でコールされる
    */
  //------------------------------------------------------------------
  void  Setup(void* pLayoutResourceData, void* pCursorResourceData);

  //------------------------------------------------------------------
  /**
    *  @brief   ctorのisAutoLoadAndSetup==true 時、リソース読み込みやSetupなどの処理を進行させる
    *  @note    isAutoLoadAndSetup==false 時は不要
    */
  //------------------------------------------------------------------
  bool  ObserveSetup(void);


  //------------------------------------------------------------------
  /**
    *  @brief    textboxに文字列を設定する
    *            必要に応じてExpandも行う
    *  @note     Register...系はGetG2DUtil()経由で行う
    */
  //------------------------------------------------------------------
  enum  TextBoxID
  {
    TEXTBOX_Button1,
    TEXTBOX_Button2,
    
    TEXTBOX_MAX
  };
  void  SetTextBoxString(const TextBoxID textBoxID, gfl2::str::StrBuf* pStr, const bool isExpand = false);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    ショートカット登録
    */
  //------------------------------------------------------------------
  enum ButtonID
  {
    BUTTON_1of2,
    BUTTON_2of2,
    
    BUTTON_MAX
  };
  void  RegisterShortcut_A(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_B = buttonID;};
  void  RegisterShortcut_B(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_B = buttonID;};
  void  RegisterShortcut_X(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_X = buttonID;};
  void  RegisterShortcut_Y(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_Y = buttonID;};
  void  RegisterShortcut_L(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_L = buttonID;};
  void  RegisterShortcut_R(const app::tool::ButtonManager::ButtonId buttonID = app::tool::ButtonManager::BUTTON_ID_NULL)  {m_ShortcutButton_R = buttonID;};

  /*
    SE
  */
  //------------------------------------------------------------------
  /**
    *  @brief    SE割り当て
    */
  //------------------------------------------------------------------
  void  SetButtonSE(const ButtonID buttonID, const u32 seID)
  {
    GetButtonManager()->SetButtonSelectSE(buttonID, seID);
  }

  //------------------------------------------------------------------
  /**
    *  @brief    各ボタンのSE割り当てを変更する
    */
  //------------------------------------------------------------------
  enum  SEPreset
  {
    SEPRESET_None,              /**<  無音              */
    SEPRESET_Decide,            /**<  決定/決定         */
    SEPRESET_Decide_Cancel,     /**<  決定/キャンセル   */
    
    SEPRESET_DEFAULT  = SEPRESET_Decide,
    SEPRESET_
  };
  void  SetButtonSE_Preset(const SEPreset presetID = SEPRESET_DEFAULT);


  /*
    override UIView I/F
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);


  /**
    リソースローダー基底
  */
  class ResourceLoaderBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(ResourceLoaderBase);
  public:
    ResourceLoaderBase(app::util::Heap* pHeap)
      : m_pHeap(pHeap)
      , m_pData(NULL)
    {}
    virtual ~ResourceLoaderBase()
    {
      GflHeapFreeMemory(m_pData);
    }
    
    virtual bool  RequestLoad(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)  {return false;}
    virtual bool  ObserveLoad(void)    {return false;}
    void* GetData(void)                {return m_pData;}

  protected:
    app::util::Heap*  m_pHeap;
    void*             m_pData;
  };

  /**
    カーソル位置を設定する
   */
  void  CursorMoveTo(const u32 index);


protected:
  enum  LoaderID
  {
    LOADER_Layout,
    LOADER_Cursor,

    LOADER_MAX
  };

  class DefaultResourceLoader;


  /*
  */
  /*  implement UIInputListener I/F  */
  virtual void                                      OnLayoutPaneTouchTriggerEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*  end implement UIInputListener I/F  */

  /*  implement CursorController::IEventHandler  */
  virtual void  CursorController_OnEvent(CursorController* pSender, const CursorController::IEventHandler::EventCode eventCode);




  bool  IsCursorVisible(void) const;
  void  Event(const IEventHandler::EventCode eventCode);


  /*
    members
  */
  app::util::Heap*                        m_pHeap;
  app::util::AppRenderingManager*         m_pRenderingManager;
  app::util::G2DUtil::SETUP_DISPLAY_TYPE  m_DisplayType;
  ResourceLoaderBase*                     m_pResourceLoaders[LOADER_MAX];

  IEventHandler*                          m_pHandler;
  u32                                     m_State;

  /*  cursor  */
  CursorController                        m_CursorController;

  /*  panes  */
  gfl2::lyt::LytTextBox*                  m_pTextBoxes[TEXTBOX_MAX];

  /*  shortcut  */
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_A;
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_B;
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_X;
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_Y;
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_L;
  app::tool::ButtonManager::ButtonId      m_ShortcutButton_R;

};



} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(APP_TOOL_2BUTTONSVIEW_H_INCLUDED)  */

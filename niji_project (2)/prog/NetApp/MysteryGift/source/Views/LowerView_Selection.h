
#if !defined(LOWERVIEW_SELECTION_H_INCLUDED)
#define LOWERVIEW_SELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Selection.h
 * @date   2015/07/31 Fri. 16:05:29
 * @author muto_yu
 * @brief  ふしぎなおくりもの 下画面画面 項目選択
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/Message/NetAppTelopTagProcessor.h>


#include  "../GiftResource.h"
#include  "./Base/LowerView_SelectionBase.h"


/*
  forward declare
*/


namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  LowerView_Selection
  *  @brief  Draw: 項目選択 下画面
  *  @date   2015.05.20 Wed.
  */
//==============================================================================
class LowerView_Selection
  : public    LowerView_SelectionBase
  , protected GiftMessageMenu::IEventHandler
  , protected GiftMessageMenu::IValueProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Selection );

public:

  /**
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      /*  common  */
      EVENT_Exit,
      EVENT_MessageFinish,

      /*  mainmenu  */
      EVENT_MainMenu_SelectedReceive,
      EVENT_MainMenu_SelectedCardAlbum,
      EVENT_MainMenu_SelectedInstruction,

      /*  instruction  */
      EVENT_Instruction_SelectedGift,
      EVENT_Instruction_SelectedAlbum,
      EVENT_Instruction_SelectedAlbumFull,

      /*  receive  */
      EVENT_Receive_Selected_Ir,
      EVENT_Receive_Selected_UDS,
      EVENT_Receive_Selected_Internet,
      EVENT_Receive_Selected_Serial,
      EVENT_Receive_Selected_Password,

      EVENT_NONE
    };

    virtual void  LowerView_Selection_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  /** 表示モード  */
  enum  ViewMode
  {
    /*  mainmenu  */
    VIEWMODE_MainMenu_ChooseFunction,         /**<  機能選択      */

    /*  instruction  */
    VIEWMODE_Instructin_ChooseInstruction,    /**<  説明項目選択*/

    /*  受信方法  */
    VIEWMODE_Receive_ReceivingMethod,         /**<  受信方法選択  */


    VIEWMODE_NONE
  };
  //------------------------------------------------------------------
  /**
    *  @func     SetupViewMode
    *  @brief    モード切り替え
    */
  //------------------------------------------------------------------
  void  SetViewMode(const ViewMode viewMode);



  //------------------------------------------------------------------
  /**
    *  @func     Draw_SelectionLower
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LowerView_Selection(GiftWork* pWork);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    デストラクタ
  */
  //--------------------------------------------------------------------------------------------
  virtual ~LowerView_Selection();


  /*  telop */
  void  ShowDeliveryEventInformation(const bool isImmediate = false);
  void  SetDeliveryEventInformation( const u32 index, const wchar_t* pString);
  void  ClearDeliveryEventInformation(void);
  void  HideDeliveryEventInformation( const bool isImmediate = true);


  /*
    UIView
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  /*
    end UIView
  */


  /*
    UIInputListener
  */

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult
    OnLayoutPaneEvent( const u32 buttonID );
  /*
    end UIInputListener
  */


  void  ShowMessage(const GiftMessageMenu::Mode messageMode)  {m_pMessageMenu->Show(messageMode);}
  void  ShowMessage_SelectFunction(void)                      {m_pMessageMenu->Show(GiftMessageMenu::MODE_GiftNotThere);}
  void  ShowMessage_NoCard(void)                              {m_pMessageMenu->Show(GiftMessageMenu::MODE_GiftNotThere);}
  void  ShowMessage_AlbumFull(void)                           {m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumFull);}
  void  ShowMessage_AlbumEmpty(void)                          {m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumEmpty);}
  void  ShowMessage_AlbumLowVacancy(const u32 amountSpace)    {m_AmountSpace = amountSpace; m_pMessageMenu->Show(GiftMessageMenu::MODE_AmountSpace);}

  /*  error */
  void  ShowMessage_Error_WirelessOff(void)   {m_pMessageMenu->Show(GiftMessageMenu::MODE_WirelessSwitchOff);}

protected:
  /*
  */
  enum  LayoutMode
  {
    LAYOUT_Large,                     /**<  大ボタン              */
    LAYOUT_ReceivingMethod,           /**<  小ボタン（受信方法）  */

    LAYOUT_NONE,    /**<    */
    LAYOUT_
  };

  void  SetLayoutMode(const LayoutMode layoutModeImmediate, const LayoutMode layoutModeAfterMessageFinish = LAYOUT_NONE);
  

  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  gfl2::lyt::LytTextBox*  GetTextBox(const TextBoxID textBoxID);


  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  void  SetText(const TextBoxID textBoxID, const u32 messageID);


  void  ApplyButtonAndCursor(const u32 buttonIDs[], const u32 idNum);
  void  Event(const IEventHandler::EventCode event);
  void  ClearButtonEvent(void);
  void  SetNextTelopString(const bool isFirst);



  /*  GiftMessageMenu::IEventHandler  */
  virtual void  MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event);
  /*  GiftMessageMenu::IValueProvider  */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)     {return m_AmountSpace;}       /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender) {return 0;}                   /**<  受信Progress  */


  /*
    members
  */
  GiftWork*                               m_pWork;
  ViewMode                                m_ViewMode;
  IEventHandler*                          m_pHandler;
  IEventHandler::EventCode*               m_pButtonEventArray;
  LayoutMode                              m_ReservedLayoutModeOnMessageFinished;

  NetAppLib::Message::TelopTagProcessor   m_TelopProcessor;
  gfl2::str::StrBuf*                      m_pStrBufs[MAX_RECEIVE_ATONCE];
  u32                                     m_TelopStrIndex;
  bool                                    m_TelopVisible;

  /*  views  */
  GiftMessageMenu*                        m_pMessageMenu;
  u32                                     m_AmountSpace;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_SELECTION_H_INCLUDED)  */


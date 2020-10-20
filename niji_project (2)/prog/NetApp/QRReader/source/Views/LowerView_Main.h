#if !defined(LOWERVIEW_MAIN_H_INCLUDED)
#define LOWERVIEW_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Main.h
 * @date   2016/01/14 Thu. 18:03:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"

#include  "./QRReaderMessageMenu.h"
#include  "./LowerView_InfoWindow.h"

namespace NetApp    {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class LowerView_Main
  : public    NetAppLib::UI::NetAppEmptyView
  , protected app::ui::UIInputListener
  , protected MessageMenu::IEventHandler
  , protected LowerView_InfoWindow::IEventHandler
  , protected LowerView_InfoWindow::IInfoWindowOpenPositionProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Main );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  message */
      EVENT_EOM,
      EVENT_MessageFinish,
      EVENT_Message_Yes,
      EVENT_Message_No,

      /*    */
      EVENT_Back,
      EVENT_QRRead,
      EVENT_FieldScan,
      EVENT_ModeChange,
      EVENT_Information,
      EVENT_InformationEnd,
      
      EVENT_QRScanButtonHolding,
      EVENT_FieldScanButtonHolding,

      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_Main_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //==============================================================================
  /**
    *  @brief  
    *  @date   2016/02/04 Thu. 12:49:37
    */
  //==============================================================================
  class IFieldScanPowerProvider
  {
  public:
    virtual u32 IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive) = 0;
    virtual ~IFieldScanPowerProvider(void)  {}
  };
  void  SetFieldScanPowerProvider(IFieldScanPowerProvider* pObject) {m_pFieldScanPowerProvider = pObject;}



  enum  ViewMode
  {
    VIEWMODE_None,
    VIEWMODE_QRRead,
    VIEWMODE_FieldScan,
    
    VIEWMODE_
  };

  enum  ViewModeSubState
  {
    VIEWMODE_SUB_None,
    
    VIEWMODE_SUB_QRReadOnScan,
    VIEWMODE_SUB_QRReadLessPower,
    VIEWMODE_SUB_QRSimpleRead,
    VIEWMODE_SUB_QRSimpleReadOnScan,

    VIEWMODE_SUB_FieldScan_Running,
    VIEWMODE_SUB_FieldScan_Charge,

    VIEWMODE_SUB_
  };

  void  SetViewMode(const ViewMode viewMode, const ViewModeSubState subState = VIEWMODE_SUB_None);


  /*    */
  void  FieldScanAvailability(const bool isAvailable) {m_bFieldScanAvailability = isAvailable;}
  /*    */

  MessageMenu*  GetMessageMenu(void) {return m_pMessageMenu;}
  void          ShowInformation(void);
  void          HideInformation(void);


  void          RequestInputClean(void)     {m_bRequestedInputClean = true;}
  bool          IsShowingInformation(void) const;

  LowerView_Main(QRReaderWork* pWork);
  virtual ~LowerView_Main();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
  /*  endimplement UIView I/F  */


protected:
  /*  implement UIInputListener I/F  */
  virtual void                                      OnLayoutPaneTouchTriggerEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneLongPressEvent( const u32 buttonID );
  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual void                                      OnSetInputEnabled( bool isEnable );

  /*  implement MessageMenu::IEventHandler I/F  */
  virtual void  MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode event);

  /*  implement LowerView_InfoWindow::IEventHandler */
  virtual void  LowerView_InfoWindow_OnEvent(const LowerView_InfoWindow::IEventHandler::EventCode event);

  /*  implement LowerView_InfoWindow::IInfoWindowOpenPositionProvider */
  virtual void  IInfoWindowOpenPositionProvider_OnQueryInfoWindowPosition(gfl2::math::Vector3& rPos);


  void  HoldButtonUpdate(void);
  void  Event(const IEventHandler::EventCode eventCode);


  /*
    members
  */
  QRReaderWork*                 m_pWork;
  IEventHandler*                m_pHandler;
  IFieldScanPowerProvider*      m_pFieldScanPowerProvider;
  ViewMode                      m_ViewMode;
  MessageMenu*                  m_pMessageMenu;
  LowerView_InfoWindow*         m_pInfoWindow;
  bool                          m_bFieldScanAvailability;
  bool                          m_IsQRScanButtonHolding;
  bool                          m_IsFieldScanButtonHolding;
  bool                          m_bQRScanButtonForcedHold;
  bool                          m_bFieldScanButtonForcedHold;

  bool                          m_bRequestedInputClean;   /**<  入力系イベントがなくなることを要求  */
  bool                          m_IsInputDirty;           /**<  何らかの入力があった                */

  /*  pane  */
  gfl2::lyt::LytPane*           m_pInfoButtonPane;
  gfl2::lyt::LytPane*           m_pSonarPane;
  gfl2::lyt::LytTextBox*        m_pScanPower;

  struct ButtonAvailavilityInfo
  {
    bool  current;
    bool  request;
    
    ButtonAvailavilityInfo(void)
      : current(true)
      , request(true)
    {}
  }* m_pButtonAvailabilityInfos;

  /*  power gauge */
  class FieldScanPowerGaugeController;
  FieldScanPowerGaugeController*  m_pFieldScanPowerGaugeControllers;

};



} /*  namespace QRReader */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_MAIN_H_INCLUDED)  */

#if !defined(LOWERVIEW_QR_H_INCLUDED)
#define LOWERVIEW_QR_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_QR.h
 * @date   2015/10/09 Fri. 20:11:05
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
#include  "AppLib/include/Tool/GeneralView/GeneralLowerView.h"

#include  "./BattleSpotMessageMenu.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class LowerView_QR
  : public    NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_QR );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  general */
      EVENT_Back,
      
      EVENT_NONE    /**<  内部制御用  */
    };

    virtual ~IEventHandler(){}
    virtual void  LowerView_QR_OnEvent(LowerView_QR* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  LowerView_QR(BattleSpotWork* pWork);
  virtual ~LowerView_QR();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool IsDrawing(void) const;
   /*  endimplement UIView I/F  */


  enum  ViewMode
  {
    VIEWMODE_QR,

    VIEWMODE_MAX,
    VIEWMODE_INVALID
  };
  void  SetViewMode(const ViewMode viewMode);


  gfl2::lyt::LytPicture* GetQRPicture(void);


protected:
  /*  implement MessageMenu::IEventHandler  */
//  virtual void    MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode eventCode);


  static const u32  MESSAGEID_INVALID = UINT_MAX;
  void  Event(const IEventHandler::EventCode eventCode);


  /*
    members
  */
  BattleSpotWork*               m_pWork;
  IEventHandler*                m_pEventHandler;
  ViewMode                      m_ViewMode;

  gfl2::lyt::LytPicture*        m_pQRPicture;
};





} /*  namespace BattleSpot */
} /*  namespace NetApp    */
#endif  /*  #if !defined(LOWERVIEW_QR_H_INCLUDED)  */

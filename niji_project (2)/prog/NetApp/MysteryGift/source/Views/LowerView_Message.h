#if !defined(LOWERVIEW_MESSAGE_H_INCLUDED)
#define LOWERVIEW_MESSAGE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_Message.h
 * @date   2016/03/22 Tue. 19:11:09
 * @author muto_yu
 * @brief  
 * @note   上画面用Layoutを下画面でも使用するとのことで、UpperView_Messageを基底とする
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "./Base/UpperView_MessageBase.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/31 Fri. 20:46:29
  */
//==============================================================================
class LowerView_Message
  : public    UpperView_MessageBase
  , protected app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Message );
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_Exit,
      
      EVENT_
    };
  
  
    virtual void  LowerView_Message_OnEvent(LowerView_Message* pSender, const EventCode eventCode) = 0;
  
  
    virtual ~IEventHandler()  {}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}





  LowerView_Message(GiftWork* pWork);
  virtual ~LowerView_Message();

protected:
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void  Event(const IEventHandler::EventCode eventCode);

  IEventHandler*  m_pHandler;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_MESSAGE_H_INCLUDED)  */

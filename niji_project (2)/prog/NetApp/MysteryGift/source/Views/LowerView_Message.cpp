//==============================================================================
/**
 * @file   LowerView_Message.cpp
 * @date   2016/03/22 Tue. 19:15:32
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftResource.h"
#include  "../ApplicationSystem/GiftWork.h"

#include  "./LowerView_Message.h"

namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Message::LowerView_Message(GiftWork* pWork)
  : UpperView_MessageBase(pWork, true)
  , m_pHandler(NULL)
{
  SetLayoutMode(LAYOUT_ReceivedData); /**<  受信データ詳細            */
  SetInputListener(this);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Message::~LowerView_Message()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerView_Message::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult  result            = app::ui::UIInputListener::ENABLE_ACCESS;  /*  DISABLE_ACCESS  */
  app::tool::ButtonManager*                 pUIButtonManager  = GetButtonManager();

  if( IsButtonAnimation() )
  {
    return app::ui::UIInputListener::DISABLE_ACCESS;
  }

  /*
    shortcut
  */
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Event(IEventHandler::EVENT_Exit);
    result  = app::ui::UIInputListener::DISABLE_ACCESS;
  }

  return result;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Message::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Message_OnEvent(this, eventCode);
  }
}



} /*  namespace Gift    */
} /*  namespace NetApp  */

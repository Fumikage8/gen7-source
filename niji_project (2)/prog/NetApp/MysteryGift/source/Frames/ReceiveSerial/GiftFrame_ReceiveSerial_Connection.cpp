//==============================================================================
/**
 * @file   GiftFrame_ReceiveSerial_Connection.cpp
 * @date   2015/09/01 Tue. 19:58:03
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../../ApplicationSystem/GiftWork.h"
#include  "../../GiftResource.h"

#include  "./GiftFrame_ReceiveSerial_Connection.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_Main,
    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveSerial_Connection
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceiveSerial_Connection::Frame_ReceiveSerial_Connection(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pLowerView_Back(NULL)

{
  m_pLowerView_Back  = m_pViewContainer->m_pLowerView_Back;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveSerial_Connection
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceiveSerial_Connection::~Frame_ReceiveSerial_Connection()
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
bool  Frame_ReceiveSerial_Connection::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Receive_Serial);
  m_pLowerView_Back->SetViewMode(LowerView_Back::VIEWMODE_Searching);

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial_Connection::setListener()
{
  m_pLowerView_Back->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial_Connection::removeListener()
{
  m_pLowerView_Back->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial_Connection::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      /*  fallthrough  */

    case  STATE_Main:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;
  }



}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_ReceiveSerial_Connection::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Back::IEventHandler  */
void  Frame_ReceiveSerial_Connection::LowerView_Back_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {

    case  Frame_ReceiveSerial_Connection::IEventHandler::EVENT_Exit:
      ExitFrame();
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Back::IEventHandler  */



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceiveSerial_Connection::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



} /*  namespace Gift    */
} /*  namespace NetApp  */

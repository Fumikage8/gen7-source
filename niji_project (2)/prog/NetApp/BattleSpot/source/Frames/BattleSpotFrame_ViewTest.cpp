
#if PM_DEBUG

//==============================================================================
/**
 * @file   BattleSpotFrame_ViewTest.cpp
 * @date   2015/10/08 Thu. 12:01:34
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
#include  "../System/BattleSpotWork.h"

#include  "../BattleSpotResource.h"

#include  "./BattleSpotFrame_ViewTest.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Running,

    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ViewTest
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ViewTest::Frame_ViewTest(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
#if PM_DEBUG
  , m_Debug(pWork, this)
#endif
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ViewTest
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ViewTest::~Frame_ViewTest()
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
bool  Frame_ViewTest::startup()
{
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_Welcome);

  /*  views  */

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
void  Frame_ViewTest::setListener()
{
  m_pViewContainer->m_pLowerView_Selection->SetEventHandler(this);
  m_pViewContainer->m_pLowerView_Card->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ViewTest::removeListener()
{
  m_pViewContainer->m_pLowerView_Selection->SetEventHandler(NULL);
  m_pViewContainer->m_pLowerView_Card->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ViewTest::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State =  STATE_Running;
      break;

    case  STATE_Running:
      break;

    case  STATE_Exit:
      exitFrame(m_Result);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_ViewTest::cleanup()
{
  bool  bAllDone = false;


  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ViewTest::ExitSelf(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ViewTest::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  GFL_PRINT("LowerView_Selection_OnEvent:(%d)\n", static_cast<u32>(eventCode));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ViewTest::LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode)
{
  GFL_PRINT("LowerView_Card_OnEvent:(%d)\n", static_cast<u32>(eventCode));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
#if PM_DEBUG
void    Frame_ViewTest::ViewTest_OnEvent(Debug::ViewTest* pSender, const Debug::ViewTest::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  Debug::ViewTest::IEventHandler::EVENT_Exit:
      ExitSelf();
      break;
  }
}
#endif


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */

#endif
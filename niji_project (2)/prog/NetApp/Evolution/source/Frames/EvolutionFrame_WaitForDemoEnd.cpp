//==============================================================================
/**
 * @file   EvolutionFrame_WaitForDemoEnd.cpp
 * @date   2016/02/22 Mon. 13:37:43
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
#include  "../System/EvolutionWork.h"
#include  "../EvolutionResource.h"

#include  "./EvolutionFrame_WaitForDemoEnd.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_WaitForDemoEnd,
    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_CheckWaza
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_WaitForDemoEnd::Frame_WaitForDemoEnd(EvolutionWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pViewContainer(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_WaitForDemoEnd
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_WaitForDemoEnd::~Frame_WaitForDemoEnd()
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
void  Frame_WaitForDemoEnd::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  Frame_WaitForDemoEnd::startup()
{
  m_pViewContainer  = m_pWork->GetViewContainer();
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
void  Frame_WaitForDemoEnd::setListener()
{
  m_pViewContainer->m_pDemoUtil->SetEventHandler(NULL);
  m_pViewContainer->m_pDemoUtil->RemoveCallBack();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_WaitForDemoEnd::removeListener()
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
void  Frame_WaitForDemoEnd::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_pViewContainer->m_pDemoUtil->ExitRequest();
      m_State = STATE_WaitForDemoEnd;
      /*  fall through  */

    case  STATE_WaitForDemoEnd:
      m_pViewContainer->m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
      if(m_pViewContainer->m_pDemoUtil->IsStable())
      {
        ExitFrame();
      }
      break;

    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_WaitForDemoEnd::cleanup()
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
void  Frame_WaitForDemoEnd::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_WaitForDemoEnd::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  return App::AppDemo::AppDemoUtility::INVALID_DEMO_GARCID;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_WaitForDemoEnd::AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  /*  nop  */
}


} /*  namespace Live    */
} /*  namespace NetApp  */





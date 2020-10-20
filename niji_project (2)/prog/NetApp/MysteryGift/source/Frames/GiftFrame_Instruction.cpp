
//==============================================================================
/**
 * @file   GiftFrame_Instruction.cpp
 * @date   2015/08/03 Mon. 12:22:49
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
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_Instruction.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_Menu,
    STATE_Instruction,
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
  *  @func     Frame_Instruction
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Instruction::Frame_Instruction(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_SelectInstruction(NULL)
  , m_pLowerView_InfoWindow(NULL)

{
  m_pUpperView_Message            = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView_SelectInstruction  = m_pViewContainer->m_pLowerView_SelectInstruction;
  m_pLowerView_InfoWindow         = m_pViewContainer->m_pLowerView_InfoWindow;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Instruction
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Instruction::~Frame_Instruction()
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
bool  Frame_Instruction::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Instruction_Menu);
  m_pViewContainer->SetAllInputEnable(true);
  m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Instruction);

  m_pLowerView_SelectInstruction->Reset();
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
void  Frame_Instruction::setListener()
{
  m_pLowerView_SelectInstruction->SetEventHandler(this);
  m_pLowerView_InfoWindow->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Instruction::removeListener()
{
  m_pLowerView_SelectInstruction->SetEventHandler(NULL);
  m_pLowerView_InfoWindow->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Instruction::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_Menu;
      /*  fallthrough  */


    case  STATE_Menu:
      break;

    case  STATE_Instruction:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;

    default:
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
bool  Frame_Instruction::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Instruction::LowerView_SelectInstruction_OnEvent(const LowerView_SelectInstruction::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_SelectInstruction::IEventHandler::EVENT_AboutGift:
      m_pViewContainer->SetScene(GiftViewContainer::SCENE_Instruction_Instruction);
      m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Instruction_AboutGift);
      m_pLowerView_InfoWindow->SetViewMode(LowerView_InfoWindow::VIEWMODE_Instruction_Gift);
      m_State = STATE_Instruction;
      break;

    case  LowerView_SelectInstruction::IEventHandler::EVENT_AboutAlbum:
      m_pViewContainer->SetScene(GiftViewContainer::SCENE_Instruction_Instruction);
      m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Instruction_AboutAlbum);
      m_pLowerView_InfoWindow->SetViewMode(LowerView_InfoWindow::VIEWMODE_Instruction_Album);
      m_State = STATE_Instruction;
      break;

    case  LowerView_SelectInstruction::IEventHandler::EVENT_AboutAlbumFull:
      m_pViewContainer->SetScene(GiftViewContainer::SCENE_Instruction_Instruction);
      m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Instruction_AboutAlbumFull);
      m_pLowerView_InfoWindow->SetViewMode(LowerView_InfoWindow::VIEWMODE_Instruction_AlbumFull);
      m_State = STATE_Instruction;
      break;


    case  LowerView_SelectInstruction::IEventHandler::EVENT_Exit: /*  through  */
      ExitFrame();
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_SelectInstruction::IEventHandler  */


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement LowerView_InfoWindow::IEventHandler  */
void  Frame_Instruction::LowerView_InfoWindow_OnEvent(const LowerView_InfoWindow::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_InfoWindow::IEventHandler::EVENT_Exit:
      if(m_State == STATE_Instruction)
      {
        m_pViewContainer->SetScene(GiftViewContainer::SCENE_Instruction_Menu);
        m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_Instruction);
        m_State = STATE_Menu;
      }
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Instruction::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Gift    */
} /*  namespace NetApp  */

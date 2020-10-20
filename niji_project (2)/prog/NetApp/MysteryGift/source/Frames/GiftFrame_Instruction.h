
#if !defined(GIFTFRAME_INSTRUCTION_H_INCLUDED)
#define GIFTFRAME_INSTRUCTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_Instruction.h
 * @date   2015/08/03 Mon. 12:07:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../GiftFrameDef.h"

#include  "../Views/GiftViewContainer.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Instruction
  *  @brief  Gift大会Frame:説明
  */
//==============================================================================
class Frame_Instruction
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_SelectInstruction::IEventHandler
  , public LowerView_InfoWindow::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Instruction );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Instruction
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Instruction(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Instruction
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Instruction();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_INSTRUCTION_Exit);

  /*  implement LowerView_SelectInstruction::IEventHandler  */
  virtual void  LowerView_SelectInstruction_OnEvent(const LowerView_SelectInstruction::IEventHandler::EventCode eventCode);

  /*  implement LowerView_InfoWindow::IEventHandler  */
  virtual void  LowerView_InfoWindow_OnEvent(const LowerView_InfoWindow::IEventHandler::EventCode eventCode);



  /*
    members
  */
  GiftWork*                       m_pWork;
  GiftViewContainer*              m_pViewContainer;
  u32                             m_State;
  FrameResult                     m_Result;

  UpperView_Message*              m_pUpperView_Message;
  LowerView_SelectInstruction*    m_pLowerView_SelectInstruction;
  LowerView_InfoWindow*           m_pLowerView_InfoWindow;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_INSTRUCTION_H_INCLUDED)  */

#if !defined(LIVEFRAME_MAIN_H_INCLUDED)
#define LIVEFRAME_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrame_Main.h
 * @date   2015/07/09 Thu. 15:18:25
 * @author muto_yu
 * @brief  Live大会Frame：main
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
#include  "../LiveFrameDef.h"
#include  "../Views/Base/UpperView_Message.h"
#include  "../Views/Base/LowerView_Selection.h"

namespace NetApp  {
namespace Live {
  class LiveWork;
  class LiveViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Main
  *  @brief  Live大会Frame:main
  */
//==============================================================================
class Frame_Main
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Main );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Main
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Main(LiveWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Main
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Main();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  MainState(void);
  void  ExitFrame(const FrameResult = RESULT_MAIN_Exit);

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const EventCode eventCode);
  /*  endimplement LowerView_Selection::IEventHandler  */


  /*
    members
  */
  LiveWork*             m_pWork;
  LiveViewContainer*    m_pViewContainer;
  u32                   m_State;
  FrameResult           m_Result;

  UpperView_Message*    m_pUpperView_Message;
  LowerView_Selection*  m_pLowerView_Selection;
};  /*  class LiveFrame  */





} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVEFRAME_MAIN_H_INCLUDED)  */

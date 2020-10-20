#if !defined(EVOLUTIONFRAME_WAITFORDEMOEND_H_INCLUDED)
#define EVOLUTIONFRAME_WAITFORDEMOEND_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrame_WaitForDemoEnd.h
 * @date   2016/02/22 Mon. 13:32:18
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
#include  "../EvolutionFrameDef.h"


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
  class EvolutionViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  */
//==============================================================================
class Frame_WaitForDemoEnd
  : public  NetAppLib::System::NetAppFrameBase
  , public  App::AppDemo::AppDemoUtility::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(Frame_WaitForDemoEnd);
public:
  Frame_WaitForDemoEnd(EvolutionWork* pWork);
  virtual ~Frame_WaitForDemoEnd();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */


private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  App::AppDemo::AppDemoUtility::IEventHandler */
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void  AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  void  ExitFrame(const FrameResult = RESULT_WAITDEMO_Exit);


  /*
    members
  */
  EvolutionWork*           m_pWork;
  u32                      m_State;
  FrameResult              m_Result;
  EvolutionViewContainer*  m_pViewContainer;
};


} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAME_WAITFORDEMOEND_H_INCLUDED)  */

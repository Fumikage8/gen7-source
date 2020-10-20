#if !defined(LIVEFRAME_DEREGISTRATION_H_INCLUDED)
#define LIVEFRAME_DEREGISTRATION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrame_Deregistration.h
 * @date   2015/07/27 Mon. 19:40:54
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
#include  "NetStatic/NetAppLib/include/Util/NetAppSaveUtility.h"
#include  "../LiveFrameDef.h"
#include  "../Views/Base/UpperView_Message.h"
#include  "../Views/Base/LowerView_Selection.h"


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @brief  
  *  @date   2015/07/24 Fri. 16:28:34
  */
//==============================================================================
class Frame_Deregistration
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Deregistration );

public:
  Frame_Deregistration(LiveWork* pWork);
  virtual ~Frame_Deregistration();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();


  void  ExitFrame(const FrameResult = RESULT_DEREGISTRATION_Exit);

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const EventCode eventCode);
  /*  endimplement LowerView_Selection::IEventHandler  */


  /*
    members
  */
  LiveWork*                           m_pWork;
  LiveViewContainer*                  m_pViewContainer;
  u32                                 m_State;
  FrameResult                         m_Result;
  NetAppLib::Util::NetAppSaveUtility  m_SaveUtil;

  UpperView_Main*                     m_pUpperView_Main;
  LowerView_Selection*                m_pLowerView_Selection;
};

} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEFRAME_DEREGISTRATION_H_INCLUDED)  */


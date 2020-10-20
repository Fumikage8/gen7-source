#if !defined(LIVEFRAME_MATCHMENU_H_INCLUDED)
#define LIVEFRAME_MATCHMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrame_MatchMenu.h
 * @date   2015/07/27 Mon. 17:41:15
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
#include  "../LiveFrameDef.h"
#include  "../Views/Base/UpperView_Main.h"
#include  "../Views/Base/LowerView_Selection.h"


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2015/07/27 Mon. 17:42:25
  */
//==============================================================================
class Frame_MatchMenu
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_MatchMenu );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_MatchMenu
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_MatchMenu(LiveWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_MatchMenu
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_MatchMenu();

private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();



  void  ExitFrame(const FrameResult = RESULT_MATCHMENU_Exit);
  void  OnEntry(void);

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

  UpperView_Main*       m_pUpperView_Main;
  LowerView_Selection*  m_pLowerView_Selection;
};  /*  class LiveFrame  */





} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEFRAME_MATCHMENU_H_INCLUDED)  */

#if PM_DEBUG

#if !defined(BATTLESPOTFRAME_VIEWTEST_H_INCLUDED)
#define BATTLESPOTFRAME_VIEWTEST_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_ViewTest.h
 * @date   2015/10/08 Thu. 11:58:23
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
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"
#include  "../Debug/BattleSpotDebug_ViewTest.h"

namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/08 Thu. 11:59:06
  */
//==============================================================================
class Frame_ViewTest
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
  , protected LowerView_Card::IEventHandler
#if PM_DEBUG
  , protected Debug::ViewTest::IEventHandler
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ViewTest );

public:
  Frame_ViewTest(BattleSpotWork* pWork);
  virtual ~Frame_ViewTest();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */


protected:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /* implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);

  /* implement LowerView_Card::IEventHandler       */
  virtual void  LowerView_Card_OnEvent(LowerView_Card* pSender, const LowerView_Card::IEventHandler::EventCode eventCode);



  void  ExitSelf(const FrameResult frameResult = RESULT_Welcome_Back);


  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;

#if PM_DEBUG
  virtual void    ViewTest_OnEvent(Debug::ViewTest* pSender, const Debug::ViewTest::IEventHandler::EventCode eventCode);
  Debug::ViewTest           m_Debug;
#endif
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_VIEWTEST_H_INCLUDED)  */

#endif
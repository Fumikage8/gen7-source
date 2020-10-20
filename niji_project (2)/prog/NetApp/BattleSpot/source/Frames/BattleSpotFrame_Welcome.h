
#if !defined(BATTLESPOTFRAME_WELCOME_H_INCLUDED)
#define BATTLESPOTFRAME_WELCOME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_Welcome.h
 * @date   2015/10/02 Fri. 13:13:17
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


namespace NetApp    {
namespace BattleSpot {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief
  */
//==============================================================================
class Frame_Welcome
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Welcome );

public:
  Frame_Welcome(BattleSpotWork* pWork);
  virtual ~Frame_Welcome();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);


  void  ExitSelf(const FrameResult frameResult = RESULT_Welcome_Back);


  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
  UpperView_Information*    m_pUpperView;
  LowerView_Selection*      m_pLowerView;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_WELCOME_H_INCLUDED)  */

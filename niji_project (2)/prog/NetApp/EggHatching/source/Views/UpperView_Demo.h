
#if !defined(UPPERVIEW_DEMO_H_INCLUDED)
#define UPPERVIEW_DEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Demo.h
 * @date   2015/11/24 Tue. 13:20:35
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Tool/CommonMessageWindowManipulator.h"
#include  "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"


namespace NetApp    {
namespace EggHatching {
  class EggHatchingWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class UpperView_Demo
  : public NetAppLib::System::NetApplicationViewBase
  , public App::Tool::CommonMessageWindowManipulator::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Demo );
public:

  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_EOM,             /**<  message終了  */
      EVENT_MessageFinish,   /**<  message終了  */
      
      EVENT_
    };
    virtual void  UpperView_Demo_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler) {m_pHandler = pHandler;}





  UpperView_Demo(EggHatchingWork* pWork);
  virtual ~UpperView_Demo();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
   /*  endimplement UIView I/F  */


  void  SetViewMode_Hatched(          const pml::pokepara::CoreParam* pCoreParam);  /**<  タマゴがかえって              */

protected:
  void  Event(const IEventHandler::EventCode eventCode);

  virtual void  CommonMessageWindowManipulator_OnEvent(const App::Tool::CommonMessageWindowManipulator::EventID event);


  /*
    members
  */
  EggHatchingWork*                              m_pWork;
  App::Tool::CommonMessageWindowManipulator     m_MessageWindowManipulator;
  IEventHandler*                                m_pHandler;
  gfl2::lyt::LytPane*                           m_pMessageWindowPane;
};
} /*  namespace EggHatching */
} /*  namespace NetApp    */
#endif  /*  #if !defined(UPPERVIEW_DEMO_H_INCLUDED)  */

#if !defined(UPPERVIEW_DEMO_H_INCLUDED)
#define UPPERVIEW_DEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Demo.h
 * @date   2015/09/01 Tue. 12:03:29
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
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class UpperView_Demo
  : public    NetAppLib::System::NetApplicationViewBase
  , public    App::Tool::CommonMessageWindowManipulator::IEventHandler
  , protected print::MsgCallback
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





  UpperView_Demo(EvolutionWork* pWork);
  virtual ~UpperView_Demo();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
   /*  endimplement UIView I/F  */


  void  SetViewMode_Evolving(         const pml::pokepara::CoreParam* pCoreParam);                                                /**<  ～のようすが…                      */
  void  SetViewMode_Evolved(          const pml::pokepara::CoreParam* pCoreParam, const MonsNo after);                            /**<  ～は～にしんかした                  */
  void  SetViewMode_EvolutionCancel(  const pml::pokepara::CoreParam* pCoreParam);                                                /**<  ～のへんかがとまった                */
  void  SetViewMode_WazaConfirm(      const pml::pokepara::CoreParam* pCoreParam, const WazaNo wazaNo);                           /**<  ～は～をおぼえたい                  */
  void  SetViewMode_NewWaza(          const pml::pokepara::CoreParam* pCoreParam, const WazaNo wazaNo);                           /**<  ～は～をおぼえた                    */
  void  SetViewMode_NewWazaAndForget( const pml::pokepara::CoreParam* pCoreParam, const WazaNo oldWaza, const WazaNo newWaza);    /**<  ～は～をわすれたそして～をおぼえた  */
  void  SetViewMode_NewWazaCancel(    const pml::pokepara::CoreParam* pCoreParam, const WazaNo waza);                             /**<  ～は～をおぼえずに                  */


protected:
  void  Event(const IEventHandler::EventCode eventCode);

  virtual void  CommonMessageWindowManipulator_OnEvent(const App::Tool::CommonMessageWindowManipulator::EventID event);

  /** implememt print::MsgCallback  */
  virtual bool  UpdateMsgCallback(u16 arg);



  /*
    members
  */
  EvolutionWork*                                m_pWork;
  App::Tool::CommonMessageWindowManipulator     m_MessageWindowManipulator;
  IEventHandler*                                m_pHandler;
  gfl2::lyt::LytPane*                           m_pMessageWindowPane;


};





} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(UPPERVIEW_DEMO_H_INCLUDED)  */

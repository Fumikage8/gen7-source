
#if !defined(EVOLUTIONFRAME_CHECKWAZA_H_INCLUDED)
#define EVOLUTIONFRAME_CHECKWAZA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrame_CheckWaza.h
 * @date   2015/09/11 Fri. 14:32:18
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
  *  @brief  技関連
  */
//==============================================================================
class Frame_CheckWaza
  : public  NetAppLib::System::NetAppFrameBase
  , public  App::AppDemo::AppDemoUtility::IEventHandler
  , public  DemoLib::Sequence::SequenceCallBack
  , public  UpperView_Demo::IEventHandler
  , public  LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_CheckWaza );
public:
  Frame_CheckWaza(EvolutionWork* pWork);
  virtual ~Frame_CheckWaza();

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

  /*  DemoLib::Sequence::SequenceCallBack  */
  virtual void SequenceCallBackFunc(int command, int paramA, int paramB);


  /*  inplement UpperView_Demo::IEventHandler       */
  virtual void  UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode);

  /*  inplement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const LowerView_Selection::IEventHandler::EventCode eventCode);



  void  ExitFrame(const FrameResult = RESULT_CHECKWAZA_Exit);


  /*
    members
  */
  EvolutionWork*           m_pWork;
  EvolutionViewContainer*  m_pViewContainer;
  u32                      m_State;
  FrameResult              m_Result;
  APP_PARAM::WazaResult    m_NextWazaResult;
  bool                     m_bDemoRequest;


  /*  views  */
  UpperView_Demo*          m_pUpperView;
  LowerView_Selection*     m_pLowerView;
};


} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAME_CHECKWAZA_H_INCLUDED)  */

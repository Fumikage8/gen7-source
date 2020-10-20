
#if !defined(EVOLUTIONFRAME_EVOLUTIONDEMO_H_INCLUDED)
#define EVOLUTIONFRAME_EVOLUTIONDEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrame_EvolutionDemo.h
 * @date   2015/08/28 Fri. 18:41:36
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

#include  "../Debug/EvolutionDebug_Demo.h"

/*  views  */
//  #include  "../Views/Base/EvolutionMessageMenu.h"


namespace NetApp  {
namespace Evolution {
  class EvolutionWork;
  class EvolutionViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_EvolutionDemo
  *  @brief  EvolutionFrame
  */
//==============================================================================
class Frame_EvolutionDemo
  : public  NetAppLib::System::NetAppFrameBase
  , public  DemoLib::Sequence::SequenceCallBack
  , public  App::AppDemo::AppDemoUtility::IEventHandler
  , public  UpperView_Demo::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_EvolutionDemo );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_EvolutionDemo
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_EvolutionDemo(EvolutionWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_EvolutionDemo
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_EvolutionDemo();

  /*
      implement FrameCellSuper I/F
  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*
      end implement FrameCellSuper I/F
  */


private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  DoEvolution(void);


  /*  DemoLib::Sequence::SequenceCallBack */
  virtual void  SequenceCallBackFunc(int command, int paramA, int paramB);


  /*  App::AppDemo::AppDemoUtility::IEventHandler */
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void  AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem);


  /*  UpperView_Demo::IEventHandler  */
  virtual void  UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode);

  void  EvolutionCancelQuery(void);
  void  ExitFrame(const FrameResult = RESULT_EVOLUTIONDMO_Cancel);


  class DelayedBGMChange
  {
    GFL_FORBID_COPY_AND_ASSIGN(DelayedBGMChange);
  public:
    DelayedBGMChange(void)
      : m_SoundID(STRM_BGM_SILENCE)
      , m_FrameCount(0)
      , m_FadeOutFrame(Sound::BGM_FADE_VERY_FAST)
      , m_FadeInFrame(Sound::BGM_FADE_VERY_FAST)
    {}
    virtual ~DelayedBGMChange()
    {}
  
  
    void  Request(const u32 soundID, const u32 frameCount, const Sound::FadeFrame fadeoutFrame, const Sound::FadeFrame fadeinFrame)
          {
            m_SoundID       = soundID;
            m_FrameCount    = frameCount;
            m_FadeOutFrame  = fadeoutFrame;
            m_FadeInFrame   = fadeinFrame;
          }
  
    void  Update(void)
          {
            if(m_FrameCount)
            {
              --m_FrameCount;
              if(!m_FrameCount)
              {
                Sound::ChangeAppBGMReq(m_SoundID, m_FadeOutFrame, m_FadeInFrame);
              }
            }
          }

    bool  IsStable(void) const  {return (!m_FrameCount);}
  
  
  
  private:
    u32               m_SoundID;
    u32               m_FrameCount;
    Sound::FadeFrame  m_FadeOutFrame;
    Sound::FadeFrame  m_FadeInFrame;
  };

  void  BGMStartRequest(const u32 frameCount, const Sound::FadeFrame fadeinFrame);


  /*
    members
  */
  EvolutionWork*                    m_pWork;
  EvolutionViewContainer*           m_pViewContainer;
  u32                               m_State;
  FrameResult                       m_Result;
  bool                              m_IsEvolved;

  /*  demo  */
  App::AppDemo::AppDemoUtility*     m_pDemoUtil;
  DelayedBGMChange                  m_DelayedBGMChange;
  bool                              m_IsMessageFinished;


  /*  view  */
  UpperView_Demo*                   m_pUpperView;
  LowerView_Selection*              m_pLowerView;


  /*  debug  */
#if PM_DEBUG
  Debug::Demo                       m_DemoDebug;
#endif

//  UpperView_Message*    m_pUpperView_Message;
//  LowerView_Selection*  m_pLowerView_Selection;
};  /*  class EvolutionFrame  */





} /*  namespace Evolution */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EVOLUTIONFRAME_EVOLUTIONDEMO_H_INCLUDED)  */


#if !defined(EGGHATCHINGFRAME_DEMO_H_INCLUDED)
#define EGGHATCHINGFRAME_DEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingFrame_Demo.h
 * @date   2015/11/24 Tue. 12:15:02
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
#include  "sound/include/Sound.h"
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"

#include  "../EggHatchingFrameDef.h"
#include  "../Debug/EggHatchingDebug_Demo.h"


namespace NetApp      {
namespace EggHatching {
  class EggHatchingWork;
  class EgghatchingViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  playing seqDemo
  */
//==============================================================================
class Frame_Demo
  : public  NetAppLib::System::NetAppFrameBase
  , public  DemoLib::Sequence::SequenceCallBack
  , public  App::AppDemo::AppDemoUtility::IEventHandler
  , public  UpperView_Demo::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Demo );
public:

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Demo(EggHatchingWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Demo();

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

  /*  DemoLib::Sequence::SequenceCallBack */
  virtual void  SequenceCallBackFunc(int command, int paramA, int paramB);

  /*  App::AppDemo::AppDemoUtility::IEventHandler */
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void  AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem);


  /*  UpperView_Demo::IEventHandler  */
  virtual void  UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode);

  void  ExitFrame(const FrameResult = RESULT_DEMO_Hatched);






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
  EggHatchingWork*                  m_pWork;
  EggHatchingViewContainer*         m_pViewContainer;
  u32                               m_State;
  FrameResult                       m_Result;

  /*  demo  */
  App::AppDemo::AppDemoUtility*     m_pDemoUtil;
  DelayedBGMChange                  m_DelayedBGMChange;

  /*  view  */
  UpperView_Demo*                   m_pUpperView;
  LowerView_Selection*              m_pLowerView;


  /*  debug  */
#if PM_DEBUG
  Debug::Demo                       m_DemoDebug;
#endif
};  /*  class EvolutionFrame  */



} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGFRAME_DEMO_H_INCLUDED)  */

//==============================================================================
/**
 * @file   EggHatchingFrame_Demo.cpp
 * @date   2015/11/24 Tue. 12:19:44
 * @author muto_yu
 * @brief  卵の孵化
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/EggHatchingWork.h"
#include  "../EggHatchingResource.h"

#include  "./EggHatchingFrame_Demo.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  EVOLUTION_CANCEL_BUTTON = gfl2::ui::BUTTON_B;

  enum  {
    STATE_Entry = 0,
    STATE_PlayingDemo,
    STATE_ShowResult,
    STATE_ShowingResult,
    STATE_ResultHandling,

    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp      {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Demo::Frame_Demo(EggHatchingWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  /*  demo  */
  , m_pDemoUtil(NULL)
  , m_DelayedBGMChange()
  /*  view  */
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  /*  debug  */
#if PM_DEBUG
  , m_DemoDebug(pWork, NULL)
#endif
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Demo
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Demo::~Frame_Demo()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_Demo::startup()
{
  /*  demoutil  */
  m_pDemoUtil = m_pViewContainer->m_pDemoUtil;

  /*  view  */
  m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
  m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

  m_pViewContainer->SetScene(EggHatchingViewContainer::SCENE_Demo);
  m_State = STATE_PlayingDemo;

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::setListener()
{
  m_pDemoUtil->SetEventHandler(this);
  m_pDemoUtil->SetCallBack(this);


#if PM_DEBUG
    m_DemoDebug.SetDemoUtility(m_pDemoUtil);
#endif


  m_pUpperView->SetEventHandler(this);
//  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::removeListener()
{
#if PM_DEBUG
    m_DemoDebug.SetDemoUtility(NULL);
#endif
  m_pDemoUtil->SetEventHandler(NULL);
  m_pDemoUtil->RemoveCallBack();


  m_pUpperView->SetEventHandler(NULL);
//  m_pLowerView->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::updateSequence()
{
  m_DelayedBGMChange.Update();

  switch(m_State)
  {
    /*
      デモ
    */
    case  STATE_PlayingDemo:
      break;

    /*
      結果
    */
    case  STATE_ShowResult:
      break;

    /*
      結果表示中
    */
    case  STATE_ShowingResult:
      break;

    /*
      結果による分岐
    */
    case  STATE_ResultHandling:
      break;

    case  STATE_Exit:
      if(m_DelayedBGMChange.IsStable())
      {
        m_pWork->SetFrameResult( m_Result );
      }
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_Demo::cleanup()
{
  bool  bAllDone = false;


  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Demo::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_Demo::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  m_pWork->m_bDemoStarted = true;
  m_pViewContainer->SetupDemoParam(pViewSystem);

  return ((m_pWork->m_pAppParam->TargetMonsIsAlolaSpecies()) ? EGGHATCHING_DEMO_GRAC_ID_Hatching_Alola : EGGHATCHING_DEMO_GRAC_ID_Hatching);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Demo::AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
//  m_pDemoUtil->ExitRequest();
  m_pViewContainer->m_pDemoUtil->SetEventHandler(NULL);
  m_pViewContainer->m_pDemoUtil->RemoveCallBack();

  /*  とりあえず終了  */
  ExitFrame();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  UpperView_Demo::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_PlayingDemo)
      {
        m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
      }
      else if(m_State  == STATE_ShowingResult)
      {
        m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
        m_State = STATE_ResultHandling;
      }
      break;
  }
  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Demo::SequenceCallBackFunc(int command, int paramA, int paramB)
{
  GFL_PRINT("cmd[%d], prmA[%d], prmB[%d]\n", command, paramA, paramB);
  if(command == DemoLib::Sequence::SEQCALLBACK_COMMAND_SYSWINDOW)
  {
    switch(paramA)
    {
      case  0:
        /** タマゴがかえって  */
        m_pUpperView->SetViewMode_Hatched(m_pWork->m_pAppParam->m_pTargetEgg);
        break;

      /*  BGMの遅延変更*/
      case  10:   BGMStartRequest(paramB, Sound::BGM_FADE_VERY_FAST);   break;
      case  11:   BGMStartRequest(paramB, Sound::BGM_FADE_FAST);        break;
      case  12:   BGMStartRequest(paramB, Sound::BGM_FADE_MIDDLE);      break;
      case  13:   BGMStartRequest(paramB, Sound::BGM_FADE_SLOW);        break;
      case  14:   BGMStartRequest(paramB, Sound::BGM_FADE_VERY_SLOW);   break;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Demo::BGMStartRequest(const u32 frameCount, const Sound::FadeFrame fadeinFrame)
{
  const u32 soundID = m_pWork->m_pAppParam->TargetMonsIsAlolaSpecies() ?  STRM_BGM_SHINKA02 : STRM_BGM_SHINKA01;

  m_DelayedBGMChange.Request(soundID, frameCount, Sound::BGM_FADE_VERY_FAST, fadeinFrame);
}




} /*  namespace EggHatching */
} /*  namespace NetApp      */


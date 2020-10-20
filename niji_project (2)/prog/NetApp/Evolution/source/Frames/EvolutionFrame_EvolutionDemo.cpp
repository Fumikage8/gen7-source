
//==============================================================================
/**
 * @file   EvolutionFrame_EvolutionDemo.cpp
 * @date   2015/08/28 Fri. 18:48:18
 * @author muto_yu
 * @brief  対象ポケモンを進化させる/キャンセルする
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/EvolutionWork.h"
#include  "../EvolutionResource.h"

#include  "./EvolutionFrame_EvolutionDemo.h"

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


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_EvolutionDemo
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_EvolutionDemo::Frame_EvolutionDemo(EvolutionWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_IsEvolved(false)
  /*  demo  */
  , m_pDemoUtil(NULL)
  , m_DelayedBGMChange()
  , m_IsMessageFinished(true)
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
  *  @func     Frame_EvolutionDemo
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_EvolutionDemo::~Frame_EvolutionDemo()
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
void  Frame_EvolutionDemo::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  Frame_EvolutionDemo::startup()
{
//  gfl2::heap::HeapBase* pHeapBase           = m_pWork->GetDeviceHeap();;

  /*  demoutil  */
  m_pDemoUtil = m_pViewContainer->m_pDemoUtil;

  /*  view  */
  m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
  m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

  m_pViewContainer->SetScene(EvolutionViewContainer::SCENE_EvolutionDemo);
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
void  Frame_EvolutionDemo::setListener()
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
void  Frame_EvolutionDemo::removeListener()
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
void  Frame_EvolutionDemo::EvolutionCancelQuery(void)
{
  if(!m_pWork->m_pAppParam->IsEvolutionCancelable())  return;


  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(EVOLUTION_CANCEL_BUTTON))
  {
    GFL_PRINT("evolution cancel command [frame:%d]\n", m_pDemoUtil->GetCurrentFrame());
    m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SetSystemCancel);
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
void  Frame_EvolutionDemo::updateSequence()
{
  m_DelayedBGMChange.Update();


#if 0
if(m_pDemoUtil->GetCurrentFrame() == 519)
{
  GFL_PRINT("delayedBGM");
  BGMStartRequest(180, Sound::BGM_FADE_MIDDLE);
}
#endif




  switch(m_State)
  {
    /*
      デモ
    */
    case  STATE_PlayingDemo:
      EvolutionCancelQuery();
      /*
        NMcat[80]:進化開始時のテキストを素早く送ると、はまる
          ・Message表示タイミングコールバックが80F、Syncが90Fに設定されており、この間にメッセージを送ってしまうとシグナルを取りこぼしていた
            メッセージスピードが速いときのみ間に合ってしまう
          →メッセージを送った後、次のメッセージ表示まではメッセージを送り続けることにした
      */
      if(m_IsMessageFinished)
      {
        m_pDemoUtil->SendCommand(App::AppDemo::AppDemoUtility::COMMAND_SystemSync);
      }
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
      /*   */
      if(m_IsEvolved)
      {
        /*  入力データを実際に進化させる  */
        m_pWork->m_pAppParam->DoEvolve(m_pWork->GetDeviceHeap());
        ExitFrame(RESULT_EVOLUTIONDMO_Evolved);
      }
      else
      {
        m_pWork->m_pAppParam->m_Result  = APPRESULT_Cancel;
        ExitFrame(RESULT_EVOLUTIONDMO_Cancel);
      }
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
bool  Frame_EvolutionDemo::cleanup()
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
void  Frame_EvolutionDemo::ExitFrame(const FrameResult frameResult)
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
u32   Frame_EvolutionDemo::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  m_pWork->m_bDemoStarted = true;
  m_pViewContainer->SetupDemoParam(pViewSystem);

  return (m_pViewContainer->CheckEvolvedPokemonIsAlolaSpecies() ? EVOLUTION_DEMO_GRAC_ID_Evolution_Alola : EVOLUTION_DEMO_GRAC_ID_Evolution);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_EvolutionDemo::AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
//  m_pDemoUtil->ExitRequest();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_EvolutionDemo::UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  UpperView_Demo::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_PlayingDemo)
      {
        m_IsMessageFinished = true;
      }
      else if(m_State  == STATE_ShowingResult)
      {
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
void  Frame_EvolutionDemo::SequenceCallBackFunc(int command, int paramA, int paramB)
{
  GFL_PRINT("cmd[%d], prmA[%d], prmB[%d]\n", command, paramA, paramB);

  if(command == DemoLib::Sequence::SEQCALLBACK_COMMAND_SYSWINDOW)
  {
    switch(paramA)
    {
      case  0:
        /** おや？  */
        m_pUpperView->SetViewMode_Evolving(m_pWork->m_pAppParam->m_pTargetPokemon);
        m_IsMessageFinished = false;
        break;
      case  1:
        /**<  ～は～にしんかした                  */
        m_pUpperView->SetViewMode_Evolved(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_MonsNoEvolve);
        m_IsEvolved         = true;
        m_IsMessageFinished = false;
        m_State             = STATE_ShowingResult;
        break;
      case  2:
        /**  ～のへんかがとまった                */
        m_pUpperView->SetViewMode_EvolutionCancel(m_pWork->m_pAppParam->m_pTargetPokemon);
        m_IsEvolved         = false;
        m_IsMessageFinished = false;
        m_State             = STATE_ShowingResult;
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
void  Frame_EvolutionDemo::BGMStartRequest(const u32 frameCount, const Sound::FadeFrame fadeinFrame)
{
  const u32 soundID = m_pViewContainer->CheckEvolvedPokemonIsAlolaSpecies() ?  STRM_BGM_SHINKA02 : STRM_BGM_SHINKA01;

  m_DelayedBGMChange.Request(soundID, frameCount, Sound::BGM_FADE_VERY_FAST, fadeinFrame);
}


} /*  namespace Live    */
} /*  namespace NetApp  */

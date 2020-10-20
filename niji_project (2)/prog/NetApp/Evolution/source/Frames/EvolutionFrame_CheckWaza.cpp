//==============================================================================
/**
 * @file   EvolutionFrame_CheckWaza.cpp
 * @date   2015/09/11 Fri. 15:05:13
 * @author muto_yu
 * @brief  
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

//#include  "Savedata/include/ZukanRegisterChecker.h"

#include  "./EvolutionFrame_CheckWaza.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  EVOLUTION_CANCEL_BUTTON = gfl2::ui::BUTTON_B;

  enum  {
    STATE_Entry = 0,

    STATE_CheckWaza,                /**<  覚える技の候補をひとつ取り出す    */

    /*  demo  */
    STATE_RequestDemo,              /**<  Demoの再生開始リクエスト          */
    STATE_WaitDemoSignal,           /**<  Demoからの通知を待つ（Fade待ち）  */

    STATE_CheckWazaResultHandling,  /**<  CheckWazaの結果に応じた処理を行う */

    STATE_Learned,
    STATE_ForgetAndLearned,
    STATE_ForgetConfirm,
    STATE_DontForget,

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
  *  @func     Frame_CheckWaza
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_CheckWaza::Frame_CheckWaza(EvolutionWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_NextWazaResult(APP_PARAM::WAZARESULT_None)
  , m_bDemoRequest(false)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_CheckWaza
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_CheckWaza::~Frame_CheckWaza()
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
void  Frame_CheckWaza::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  Frame_CheckWaza::startup()
{
  m_pViewContainer->SetScene(EvolutionViewContainer::SCENE_CheckWaza);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;
  }

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
void  Frame_CheckWaza::setListener()
{
  m_pViewContainer->m_pDemoUtil->SetEventHandler(this);
  m_pViewContainer->m_pDemoUtil->SetCallBack(this);
  m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_CheckWaza::removeListener()
{
  m_pViewContainer->m_pDemoUtil->SetCallBack(NULL);
  m_pViewContainer->m_pDemoUtil->SetEventHandler(NULL);
  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
}



//------------------------------------------------------------------
/**
  *  @brief   
  *  @note    技を覚えるシーケンスは技忘れAppへ遷移する場合があり複雑化するので、単純化のため
  *           処理が一手進むたびに一旦本Frameを抜け、必要に応じて再度戻ってくる形となっている
  */
//------------------------------------------------------------------
void  Frame_CheckWaza::updateSequence()
{
  /*
    2016/04/11  Yu Muto
    [GF]調整要望リスト #151   進化し図鑑登録した後のＵＩ
      大森さん要望
      進化→図鑑登録の流れの後、覚える技がなければDemoを開始せずに終了するように流れを変える
  */
  switch(m_State)
  {
    case  STATE_Entry:
//      m_State =  m_pWork->m_bDemoStarted ? STATE_CheckWaza : STATE_WaitDemoSignal;
      m_State = STATE_CheckWaza;
      /*  fall through  */


    /*  技候補をひとつ取り出す  */
    case  STATE_CheckWaza:
    {
      if(m_pWork->m_pAppParam->IsWazaWasureSequence())
      {
        /*  技忘れシーケンス中  */
        m_State = STATE_RequestDemo;
      }
      else
      {
        /*  新規に技の候補を取得  */
        m_NextWazaResult  = m_pWork->m_pAppParam->NextWaza();   /*  appParamに新しく覚える技の情報が格納される  */
        
        switch(m_NextWazaResult)
        {
          case  APP_PARAM::WAZARESULT_None:      /**<  終了        */
            /*  次の候補がない場合はここで終了：※図鑑からの戻りの場合はDemoが復帰せず終了する  */
            ExitFrame();
            break;

          default:
            m_State = STATE_RequestDemo;
            break;
        }
      }
    }
      break;


    case  STATE_RequestDemo:
      /*  demo再生許可：既に再生されている場合は直接ResultHandlingへ、そうでなければ（Demoがここで新規に開始されるので）Demoからのシグナルを待つ（Fade待ち）  */
      m_bDemoRequest  = true;
      m_State = (m_pWork->m_bDemoStarted) ? STATE_CheckWazaResultHandling : STATE_WaitDemoSignal;
      /*  fall through  */

    case  STATE_WaitDemoSignal:
      /*  nop */
      break;


    /*  改めて結果に応じた実処理を行う  */
    case  STATE_CheckWazaResultHandling:
    {
      if(m_pWork->m_pAppParam->IsWazaWasureSequence())
      {
        /*  技忘れシーケンス中  */

        if(m_pWork->m_pAppParam->IsForgotWaza())
        {
          /*  忘れさせたあとにここにきている場合はそれを覚える  */
          m_pUpperView->SetViewMode_NewWazaAndForget(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_OldWaza, m_pWork->m_pAppParam->m_NewWaza);
          m_pWork->m_pAppParam->ForgetAndLearnWaza();   /*  結果確定：忘れる＆覚える                                            */
          m_pWork->m_pAppParam->ResetWaza();            /*  技覚えステータスをリセット：技忘れシーケンスもここでリセットされる  */
          m_State = STATE_ForgetAndLearned;
        }
        else if(m_pWork->m_pAppParam->IsAbandonedForget())    /*  IsForgetCancelの条件も同時に満たしているので注意  */
        {
          /*  あきらめる  */
          m_pUpperView->SetViewMode_NewWazaCancel(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_NewWaza);
                                                        /*  結果確定：あきらめる＝リセットのみ                                  */
          m_pWork->m_pAppParam->ResetWaza();            /*  技覚えステータスをリセット：技忘れシーケンスもここでリセットされる  */
          m_State = STATE_DontForget;
        }
        else if(m_pWork->m_pAppParam->IsForgetCancel())
        {
          /*  忘れにいったが、キャンセル      */
          /*  継続：もう一度2択で問い合わせる */
          /**<  空きがない  */
          m_pUpperView->SetViewMode_WazaConfirm(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_NewWaza);
          m_State = STATE_ForgetConfirm;
        }
        else
        {
          GFL_ASSERT(false);    /*  ここに来るばあいは実装にミスがある  */
        }
      }
      else
      {
        /*  新規候補  */
        switch(m_NextWazaResult)
        {
          case  APP_PARAM::WAZARESULT_Learned:   /**<  覚えた      */
            m_pUpperView->SetViewMode_NewWaza(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_NewWaza);
            m_pWork->m_pAppParam->ResetWaza();
            m_State = STATE_Learned;
            break;

          case  APP_PARAM::WAZARESULT_Full:      /**<  空きがない  */
            m_pUpperView->SetViewMode_WazaConfirm(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_NewWaza);
            m_State = STATE_ForgetConfirm;
            break;
            
          default:
            GFL_ASSERT(false);    /*  ここに来るばあいは実装にミスがある  */
            break;
        }
      }
    }
      break;

    case  STATE_Learned:
    case  STATE_ForgetAndLearned:
    case  STATE_ForgetConfirm:
    case  STATE_DontForget:
      break;

    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_CheckWaza::cleanup()
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
void  Frame_CheckWaza::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  inplement UpperView_Demo::IEventHandler       */
void  Frame_CheckWaza::UpperView_Demo_OnEvent(const UpperView_Demo::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case UpperView_Demo::IEventHandler::EVENT_EOM:
      if(m_State == STATE_ForgetConfirm)
      {
        m_pLowerView->ShowChoice();
      }
      break;

    case UpperView_Demo::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_Learned)
      {
        ExitFrame(RESULT_CHECKWAZA_Continue);
      }
      else if(m_State == STATE_DontForget)
      {
        ExitFrame(RESULT_CHECKWAZA_Continue);
      }
      else if(m_State == STATE_ForgetAndLearned)
      {
        ExitFrame(RESULT_CHECKWAZA_Continue);
      }
     break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*  inplement LowerView_Selection::IEventHandler  */
void  Frame_CheckWaza::LowerView_Selection_OnEvent(const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_ForgetWaza:    /**<  技を忘れる   */
      m_pWork->m_pAppParam->SetWazaWasureSequence(true);          /*  技忘れシーケンスであることをマークし、CheckWazaの挙動を変更  */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_CallWazaWasure;
      ExitFrame(RESULT_CHECKWAZA_Forget);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_KeepWaza:      /**<  技を忘れない */
      m_pUpperView->SetViewMode_NewWazaCancel(m_pWork->m_pAppParam->m_pTargetPokemon, m_pWork->m_pAppParam->m_NewWaza);
      m_pWork->m_pAppParam->ResetWaza();            /*  技覚えステータスをリセット：技忘れシーケンスもここでリセットされる  */
      m_State = STATE_DontForget;
      break;
  }
  m_pLowerView->HideChoice();
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_CheckWaza::AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
  if(m_bDemoRequest)
  {
    m_bDemoRequest  = false;
    
    m_pWork->m_bDemoStarted = true;
    m_pViewContainer->SetupDemoParam(pViewSystem);

    return (m_pViewContainer->CheckEvolvedPokemonIsAlolaSpecies() ? EVOLUTION_DEMO_GRAC_ID_Evolved_Alola : EVOLUTION_DEMO_GRAC_ID_Evolved);
  }

  return App::AppDemo::AppDemoUtility::INVALID_DEMO_GARCID;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_CheckWaza::AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*  DemoLib::Sequence::SequenceCallBack  */
void Frame_CheckWaza::SequenceCallBackFunc(int command, int paramA, int paramB)
{
  switch(command)
  {
    case  DemoLib::Sequence::SEQCALLBACK_COMMAND_SYSWINDOW:
      if(m_State == STATE_WaitDemoSignal)  {
        if(paramA == 3)
        {
          m_State = STATE_CheckWazaResultHandling;
        }
      }
      break;
  }
}



} /*  namespace Live    */
} /*  namespace NetApp  */



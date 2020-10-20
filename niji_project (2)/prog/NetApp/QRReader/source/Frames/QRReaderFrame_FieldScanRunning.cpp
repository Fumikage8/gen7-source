
//==============================================================================
/**
 * @file   QRReaderFrame_FieldScanRunning.cpp
 * @date   2016/01/18 Mon. 15:30:39
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
#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"

#include  "../Common/FieldFlag/QRReaderFieldFlagUtility.h"
#include  "./QRReaderFrame_FieldScanRunning.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    /*  発動中  */
    STATE_FieldScanRunning,
    STATE_FieldScanTimeout,   /**<  終了  */

    STATE_CannotChargeConfirm,

    STATE_Exit,

    STATE_     /*  terminator  */
  };


  static const u32  ScanFrameMax  = 90;

} /*  namespace  */



namespace NetApp      {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_FieldScanRunning::Frame_FieldScanRunning(QRReaderWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_ScanFrame(ScanFrameMax)
  , m_ScanPower(0)
  /*  fieldscan  */
  , m_RemainingMinutes(0)
  , m_RemainingSeconds(0)
  /*  view  */
  , m_pUpperView(NULL)
  , m_pUpperView_Pokemon(NULL)
  , m_pLowerView(NULL)
  , m_pMessageMenu(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_FieldScanRunning
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FieldScanRunning::~Frame_FieldScanRunning()
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
bool  Frame_FieldScanRunning::startup()
{
  /*  view  */
  m_pUpperView          = m_pViewContainer->m_pUpperView_Main;
  m_pUpperView_Pokemon  = m_pViewContainer->m_pUpperView_Pokemon;
  m_pLowerView          = m_pViewContainer->m_pLowerView_Main;
  m_pMessageMenu        = m_pLowerView->GetMessageMenu();

  m_pViewContainer->SetScene(QRReaderViewContainer::SCENE_FieldScan);
  m_pViewContainer->SetAllInputEnable(true);


  {
    u16                   islandID;
    u16                   zoneID;
    PokeTool::SimpleParam simpleParam;

    GetQRReaderSaveData()->GetFieldScanTargetInfo(&islandID, &zoneID, &(simpleParam.monsNo), &(simpleParam.formNo), &(simpleParam.sex));

    /*    */
    if(!m_pWork->m_IsTransitionFromFieldScanFrame)
    {
      m_pWork->m_pPokeModelUtil->RequestEx(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 1, simpleParam);

      {
        const s32 rectWH      = 160;
        const s32 rectWHHalf  = rectWH/2;
        const s32 posOfsY     = -22;
        
        m_pWork->m_pPokeModelUtil->SetFramePosition(200-rectWHHalf, 120-rectWHHalf + posOfsY, 200+rectWHHalf,120+rectWHHalf + posOfsY);
      }

      m_pUpperView->StartShilhouetteAnim();
      m_pUpperView_Pokemon->ShowFieldScanResult(islandID, zoneID, simpleParam.monsNo);
    }
    m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan, LowerView_Main::VIEWMODE_SUB_FieldScan_Running);
    m_pWork->m_IsTransitionFromFieldScanFrame = false;
  }


  FieldFlagUtility::QueryFieldScan(&m_RemainingMinutes, &m_RemainingSeconds);
  m_ScanPower = GetQRReaderSaveData()->GetScanPower();

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
void  Frame_FieldScanRunning::setListener()
{
  m_pUpperView_Pokemon->SetRemainingTimeProvider(this);

  m_pLowerView->SetEventHandler(this);
  m_pLowerView->SetFieldScanPowerProvider(this);

  m_pWork->m_pPokeModelUtil->SetEventHandler(1, this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::removeListener()
{
  m_pUpperView_Pokemon->SetRemainingTimeProvider(NULL);

  m_pLowerView->SetEventHandler(NULL);
  m_pLowerView->SetFieldScanPowerProvider(NULL);

  m_pWork->m_pPokeModelUtil->SetEventHandler(1, NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::updateSequence()
{
  FieldFlagUtility::QueryFieldScan(&m_RemainingMinutes, &m_RemainingSeconds);

  /*  scanpower  */
  {
    m_ScanPower = GetQRReaderSaveData()->GetScanPower();
  }


  switch(m_State)
  {
    case  STATE_Entry:
    {
      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan, LowerView_Main::VIEWMODE_SUB_FieldScan_Running);
      OnEntry();
    }
      /*  fallthrough  */

    /*  発動中  */
    case  STATE_FieldScanRunning:
      if(!GetQRReaderSaveData()->IsFieldScanActive())
      {
        /*
          NMcat[1683]   【QRスキャン】インフォメーション表示中にしまスキャンの残り時間が0になるとテキストが視認できない
          InfoWindow表示中は、各種演出は停止するがメッセージ表示と状態遷移はしないように
        */
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
        m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan);
        m_pWork->m_pPokeModelUtil->ReleaseRequest(1);
        if(!m_pLowerView->IsShowingInformation())
        {
          m_pMessageMenu->Show(MessageMenu::MODE_FieldScanPowerEmpty);
          m_State = STATE_FieldScanTimeout;
        }
      }
      break;

    case  STATE_FieldScanTimeout:
      break;

    case  STATE_CannotChargeConfirm:
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
bool  Frame_FieldScanRunning::cleanup()
{
  bool  bAllDone = false;


  m_pWork->m_pPokeModelUtil->ReleaseRequest(1);
  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_FieldScanRunning::IRemainingTimeProvider_OnQueryMinutes(void)
{
  return m_RemainingMinutes;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32   Frame_FieldScanRunning::IRemainingTimeProvider_OnQuerySeconds(void)
{
  return m_RemainingSeconds;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    /*
      message
    */
    case  LowerView_Main::IEventHandler::EVENT_EOM:
      if(m_State == STATE_FieldScanTimeout)
      {
        GetQRReaderSaveData()->InactivateFieldScan();
        ExitFrame(RESULT_FieldScanRunning_QRRead);
      }
      break;

    case  LowerView_Main::IEventHandler::EVENT_MessageFinish:
      break;

    case  LowerView_Main::IEventHandler::EVENT_Message_Yes:
    {
      if(m_State  == STATE_CannotChargeConfirm)
      {
        m_pWork->m_IsNeedCannotChargeConfirmIfFieldScanIsActive = false;
        ExitFrame(RESULT_FieldScanRunning_QRRead);
      }
    }
      break;

    case  LowerView_Main::IEventHandler::EVENT_Message_No:
      if(m_State  == STATE_CannotChargeConfirm)
      {
        m_State = STATE_Entry;
      }
      break;





    case  LowerView_Main::IEventHandler::EVENT_Back:
      ExitFrame();
      break;

    case  LowerView_Main::IEventHandler::EVENT_QRRead:
/*  RESULT_FieldScanRunning_QRRead  */
      break;

    case  LowerView_Main::IEventHandler::EVENT_FieldScan:
      break;

    case  LowerView_Main::IEventHandler::EVENT_ModeChange:
      /*
        2016/03/28  Yu Muto
        FieldScan発動中もScanPowerをためられるようにとの要望
      */
#if 0
      if(m_pWork->m_IsNeedCannotChargeConfirmIfFieldScanIsActive)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_CannotChargePowerConfirm);
        m_State = STATE_CannotChargeConfirm;
      }
      else
#endif
      {
        Sound::PlaySE(SEQ_SE_SYS_NJ_042);
        ExitFrame(RESULT_FieldScanRunning_QRRead);
      }
      break;

    case  LowerView_Main::IEventHandler::EVENT_Information:
      m_pLowerView->ShowInformation();
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::OnEntry(void)
{
  m_pMessageMenu->Show(MessageMenu::MDOE_FieldScan_LetsHunt);
  m_State = STATE_FieldScanRunning;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32   Frame_FieldScanRunning::IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive)
{
  rIsFull   = GetQRReaderSaveData()->IsScanPowerFull();
  rIsActive = GetQRReaderSaveData()->IsFieldScanActive();
  
  return m_ScanPower;
}




//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScanRunning::NetAppPokeModelUtility_OnEvent(NetAppLib::Util::NetAppPokeModelUtility* pSender, const NetAppLib::Util::NetAppPokeModelUtility::IEventHandler::EventID eventID, PokeTool::PokeModel* pPokeModel)
{
  switch(eventID)
  {
    case  NetAppLib::Util::NetAppPokeModelUtility::IEventHandler::EVENT_OnModelLoded:
    {
      m_pWork->m_pPokeModelUtil->SetShaderColor(1, gfl2::gfx::Color(.0f, .0f, .0f, 1.0f));
    }
      break;
  }
}


} /*  namespace QRReader */
} /*  namespace NetApp   */



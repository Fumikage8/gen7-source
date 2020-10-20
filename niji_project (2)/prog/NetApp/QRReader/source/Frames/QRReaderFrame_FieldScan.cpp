
//==============================================================================
/**
 * @file   QRReaderFrame_FieldScan.cpp
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
#include  "./QRReaderFrame_FieldScan.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    /*  発動前  */
    STATE_Idle_FieldScanAvailable,
    STATE_Idle_FieldScanNotAvailable,

    /*  発動開始  */
    STATE_ScanConfirm,
    STATE_ScanStart,
    STATE_Scanning,
    STATE_ScanResult_Success,
    STATE_ScanResult_Failure,

    STATE_Exit,

    STATE_     /*  terminator  */
  };


  static const u32  ScanFrameMax  = 60;

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
Frame_FieldScan::Frame_FieldScan(QRReaderWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_ScanFrame(ScanFrameMax)
  , m_ScanPower(0)
  , m_RemainingMinutes(0)
  , m_RemainingSeconds(0)
  , m_IsFieldScanning(false)
  , m_pPokeModel(NULL)
  /*  view  */
  , m_pUpperView(NULL)
  , m_pUpperView_Pokemon(NULL)
  , m_pLowerView(NULL)
  , m_pMessageMenu(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_FieldScan
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FieldScan::~Frame_FieldScan()
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
bool  Frame_FieldScan::startup()
{
  /*  view  */
  m_pUpperView          = m_pViewContainer->m_pUpperView_Main;
  m_pUpperView_Pokemon  = m_pViewContainer->m_pUpperView_Pokemon;
  m_pLowerView          = m_pViewContainer->m_pLowerView_Main;
  m_pMessageMenu        = m_pLowerView->GetMessageMenu();

  m_pUpperView_Pokemon->HideResult();

  m_pViewContainer->SetScene(QRReaderViewContainer::SCENE_FieldScan);
  m_pViewContainer->SetAllInputEnable(true);

  m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
  m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
  m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);

  if(FieldFlagUtility::QueryFieldScan())
  {
    ExitFrame(RESULT_FieldScan_FieldScanRunning);
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
void  Frame_FieldScan::setListener()
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
void  Frame_FieldScan::removeListener()
{
  m_pUpperView_Pokemon->SetRemainingTimeProvider(NULL);

  m_pLowerView->SetEventHandler(NULL);
  m_pLowerView->SetFieldScanPowerProvider(NULL);

  m_pWork->m_pPokeModelUtil->ClearHandlers();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_FieldScan::updateSequence()
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
      OnEntry();
    }
      /*  fallthrough  */

    /*  発動前  */
    case  STATE_Idle_FieldScanAvailable:
      /*
        GFNMcat[3552]   【QRスキャン】しまスキャンでポケモンが見つからなかった際のウィンドウの挙動が不自然
          入力抑制解除
      */
      m_pViewContainer->SetAllInputEnable(true);
      if(m_IsFieldScanning)
      {
        m_State = STATE_ScanStart;
      }
      break;


    case  STATE_Idle_FieldScanNotAvailable:
      break;

    /*  発動開始  */
    case  STATE_ScanConfirm:
      break;

    case  STATE_ScanStart:
      m_ScanFrame = ScanFrameMax;
      m_pMessageMenu->SetFieldScanIslandID(FieldFlagUtility::GetIslandID());
      m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Scanning);
      m_State     = STATE_Scanning;
      /*  through  */

    case  STATE_Scanning:
      if(m_IsFieldScanning)
      {
        if(m_ScanFrame)
        {
          --m_ScanFrame;
        }
        if(!m_ScanFrame)
        {
          DoFieldScan();
        }
      }
      else
      {
        m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Available);
        m_State = STATE_Idle_FieldScanAvailable;
      }
      break;

    case  STATE_ScanResult_Success:
    {
      /*
        GFNMcat[3895]   【ＱＲスキャン】特定のタイミングでしまスキャンを起動して放置すると、時間が０になっても上画面の表示が消えない
        このStateでメッセージを送らないでいるとActiveチェックがされない
      */
      if(!GetQRReaderSaveData()->IsFieldScanActive())
      {
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
        m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan);
        m_pWork->m_pPokeModelUtil->ReleaseRequest(1);
      }
    }
      break;



    case  STATE_ScanResult_Failure:
      break;

    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }

  m_IsFieldScanning = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_FieldScan::cleanup()
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
void  Frame_FieldScan::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScan::LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    /*
      message
    */
    case  LowerView_Main::IEventHandler::EVENT_EOM:
      if(m_State == STATE_ScanResult_Success)
      {
        ExitFrame(RESULT_FieldScan_FieldScanRunning);
      }
      else if(m_State == STATE_ScanResult_Failure)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Available);

        /*
          GFNMcat[3552]   【QRスキャン】しまスキャンでポケモンが見つからなかった際のウィンドウの挙動が不自然
            入力抑制
        */
        m_pLowerView->RequestInputClean();          /*  ボタン類のリリースを要求  */
        m_pViewContainer->SetAllInputEnable(false);

        m_State = STATE_Idle_FieldScanAvailable;
      }
      else if(m_State == STATE_Idle_FieldScanNotAvailable)
      {
        Sound::PlaySE(SEQ_SE_SYS_NJ_042);
        ExitFrame(RESULT_FieldScan_QRRead);
      }

      break;

    case  LowerView_Main::IEventHandler::EVENT_MessageFinish:
      break;

    case  LowerView_Main::IEventHandler::EVENT_Message_Yes:
      if(m_State == STATE_ScanConfirm)
      {
        m_State = STATE_ScanStart;
      }
      break;

    case  LowerView_Main::IEventHandler::EVENT_Message_No:
      if(m_State == STATE_ScanConfirm)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Available);
        m_State = STATE_Idle_FieldScanAvailable;
      }
      break;

    case  LowerView_Main::IEventHandler::EVENT_Back:
      ExitFrame();
      break;

    case  LowerView_Main::IEventHandler::EVENT_QRRead:
      break;

    case  LowerView_Main::IEventHandler::EVENT_FieldScan:
    {
/*
      if(m_State == STATE_Idle_FieldScanAvailable)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_StartConfirm);
        m_State = STATE_ScanConfirm;
      }
*/
    }
      break;

    case  LowerView_Main::IEventHandler::EVENT_ModeChange:
      Sound::PlaySE(SEQ_SE_SYS_NJ_042);
      ExitFrame(RESULT_FieldScan_QRRead);
      break;

    case  LowerView_Main::IEventHandler::EVENT_Information:
      m_pLowerView->ShowInformation();
      break;

    case  LowerView_Main::IEventHandler::EVENT_FieldScanButtonHolding:
      m_IsFieldScanning = true;
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScan::NetAppPokeModelUtility_OnEvent(NetAppLib::Util::NetAppPokeModelUtility* pSender, const NetAppLib::Util::NetAppPokeModelUtility::IEventHandler::EventID eventID, PokeTool::PokeModel* pPokeModel)
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


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FieldScan::OnEntry(void)
{
  if(GetQRReaderSaveData()->IsScanPowerFull())
  {
    m_pLowerView->FieldScanAvailability(true);
    m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Available);
    m_State = STATE_Idle_FieldScanAvailable;
  }
  else
  {
    m_pLowerView->FieldScanAvailability(false);
    m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_NotAvailable);
    m_State = STATE_Idle_FieldScanNotAvailable;
  }

  m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_FieldScan::DoFieldScan(void)
{
  u8                    islandID;
  Field::ZoneID         zoneID;
  PokeTool::SimpleParam simpleParam;


  if(FieldFlagUtility::DoFieldScan(m_pWork, &islandID, &zoneID, &(simpleParam.monsNo), &(simpleParam.formNo), &(simpleParam.sex)))
  {
    /*  success  */

    /*    */
    m_pWork->m_pPokeModelUtil->RequestEx(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 1, simpleParam);
    {
      const s32 rectWH      = 160;
      const s32 rectWHHalf  = rectWH/2;
      const s32 posOfsY     = -22;
      
      m_pWork->m_pPokeModelUtil->SetFramePosition(200-rectWHHalf, 120-rectWHHalf + posOfsY, 200+rectWHHalf,120+rectWHHalf + posOfsY);
    }
    m_pWork->m_IsTransitionFromFieldScanFrame = true;

    m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_Found);
    m_pUpperView->StartShilhouetteAnim();
    m_pUpperView_Pokemon->ShowFieldScanResult(islandID, zoneID, simpleParam.monsNo);
    m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan, LowerView_Main::VIEWMODE_SUB_FieldScan_Running);

    m_State = STATE_ScanResult_Success;

    GetQRReaderSaveData()->ResetScannedDataHistory();

    Sound::PlaySE(SEQ_SE_SYS_NJ_037);
  }
  else
  {
    /*  failure  */
    m_pMessageMenu->Show(MessageMenu::MODE_FieldScan_NotFound);
    m_State = STATE_ScanResult_Failure;

    Sound::PlaySE(SEQ_SE_SYS_NJ_026);
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
u32   Frame_FieldScan::IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive)
{
  rIsFull   = GetQRReaderSaveData()->IsScanPowerFull();
  rIsActive = GetQRReaderSaveData()->IsFieldScanActive();
  
  return m_ScanPower;
}



} /*  namespace QRReader */
} /*  namespace NetApp   */



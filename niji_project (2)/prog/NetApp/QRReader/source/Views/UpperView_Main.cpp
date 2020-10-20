//==============================================================================
/**
 * @file   UpperView_Main.cpp
 * @date   2016/01/18 Mon. 15:17:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"

#include  "../QRReaderResource.h"
#include  "../System/QRReaderWork.h"

#include  "./UpperView_Main.h"


namespace
{
  static const struct
  {
    u32   valueAnim;
    u32   maxAnim;
  }gaugeAnimTable[] = {
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_00_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_00_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_01_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_01_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_02_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_02_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_03_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_03_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_04_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_04_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_05_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_05_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_06_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_06_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_07_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_07_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_08_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_08_GAUGE_MAX},
    {LA_QRREADER_QRS_MIN_UPP_000__POWER_09_GAUGE_ANIME,   LA_QRREADER_QRS_MIN_UPP_000__POWER_09_GAUGE_MAX},
  };

  static const u32 gaugeAnimTableNum  = GFL_NELEMS(gaugeAnimTable);
  static const u32 gaugeCapacity      = Savedata::QRReaderSaveData::QRREAD_BATTERY_MAX / gaugeAnimTableNum; /**<  ゲージ一本あたりの許容量  */
}




namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/02/19 Fri. 15:59:16
  */
//==============================================================================
class UpperView_Main::BatteryGaugeController
{
  GFL_FORBID_COPY_AND_ASSIGN(BatteryGaugeController);
public:
  BatteryGaugeController(void)
    : m_pWork(NULL)
    , m_pOwner(NULL)
    , m_ValueAnim(0)
    , m_MaxAnim(0)
    , m_Value(0)
    , m_ValueMin(0)
    , m_ValueMax(Range)
  {}
  virtual ~BatteryGaugeController() {}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Setup(UpperView_Main* pOwner, QRReaderWork* pWork, const u32 valueAnim, const u32 maxAnim, const u32 valueMin, const u32 valueMax)
        {
          m_pOwner    = pOwner;
          m_pWork     = pWork;
          m_ValueAnim = valueAnim;
          m_MaxAnim   = maxAnim;
          m_ValueMin  = valueMin;
          m_ValueMax  = valueMax;

          m_pOwner->GetG2DUtil()->StartAnimeFrame(0, m_ValueAnim, 0.0f, false);
          SetValue(0);
        }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetValue(const u8 value)
        {
          u32 dispValue = value;
          
          /*  値域に正規化  */
          if(dispValue < m_ValueMin)      dispValue = m_ValueMin;
          else if(m_ValueMax < dispValue) dispValue = m_ValueMax;

          if(m_Value == dispValue)  return;

          /*  値変更前にAnimが必要か確認する  */
          bool  isStartMaxAnim  = ((m_Value < m_ValueMax) && (dispValue == m_ValueMax));
          
          /*  値を変更し、表示反映  */
          m_Value = dispValue;
          m_pOwner->GetG2DUtil()->SetAnimeFrame(0, m_ValueAnim, static_cast<f32>(Range * (m_Value - m_ValueMin) / (m_ValueMax - m_ValueMin)));
          if(isStartMaxAnim)
          {
            m_pOwner->GetG2DUtil()->StartAnime(0, m_MaxAnim);
          }
        }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsMax(void) const {return (m_Value == m_ValueMax);}


protected:
  static const u32   Range  = 100;

  QRReaderWork*     m_pWork;
  UpperView_Main*   m_pOwner;
  u32               m_ValueAnim;
  u32               m_MaxAnim;
  u32               m_Value;
  u32               m_ValueMin;
  u32               m_ValueMax;

};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/02/19 Fri. 16:23:00
  */
//==============================================================================
class UpperView_Main::BatteryValueInterp
{
  GFL_FORBID_COPY_AND_ASSIGN(BatteryValueInterp);
public:
  BatteryValueInterp(const u32 offsetABSPerFrame)
    : m_OffsetABSPerFrame(offsetABSPerFrame)
    , m_Current(0)
    , m_Target(0)
  {
  }

  void  Update(void)
        {
          if(m_Current < m_Target)
          {
            m_Current += m_OffsetABSPerFrame;
            if(m_Target < m_Current)
            {
              m_Current = m_Target;
            }
          }
          else if(m_Target < m_Current)
          {
            m_Current -= m_OffsetABSPerFrame;
            if(m_Current < m_Target)
            {
              m_Current = m_Target;
            }
          }
        }

  void  SetTarget(const s32 targetValue, const bool isImmediate)
        {
          m_Target  = targetValue;
          if(isImmediate) m_Current = m_Target;
        }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  s32  GetCurrent(void) const  {return m_Current;}

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  s32  GetTargetDir(void) const {return (m_Target - m_Current);}

protected:
  u32   m_OffsetABSPerFrame;
  s32   m_Current;
  s32   m_Target;
};






//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Main::UpperView_Main(QRReaderWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_ViewMode(VIEWMODE_INVALID)
  , m_pQRScanPowerProvider(NULL)
  , m_TargetMonsNo(MONSNO_NULL)
  , m_TargetFormNo(0)
  , m_pBatteryValueInterp(NULL)
  , m_IsBatteryGaugeChangeImmediately(false)
  , m_bNeedNotifyPokeFrameAnimEnd(false)
  , m_bGaugeEnable(true)
  , m_pPicturePane_CameraImage(NULL)
  , m_pPicturePane_QRImage(NULL)

{
//  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();


  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_QRREADER_QRS_MIN_UPP_000_BFLYT,
    LA_QRREADER_QRS_MIN_UPP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    m_pWork->GetMessageUtility(),
    QRREADER_MESSAGE_GRAC_ID
  );


  /*  panes  */
  {
    app::util::GetPaneHelper  helper( GetG2DUtil()->GetLayoutWork(0), GetG2DUtil()->GetLayoutResourceID() );

    m_pPicturePane_CameraImage          = helper.GetPicturePane(PANENAME_QRS_MIN_UPP_000_PANE_CAMERAIMAGE);
    m_pPicturePane_QRImage              = helper.GetPicturePane(PANENAME_QRS_MIN_UPP_000_PANE_PIC_QRIMAGE);


    m_pTextBoxes[TEXTBOX_Label_Scans]   = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_000_PANE_ANALYSIS_00);
    m_pTextBoxes[TEXTBOX_Scans]         = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_000_PANE_ANALYSIS_01);

    GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Label_Scans], qrscan_sys_01_06);

    m_pPanes[PANE_Win_PokeFrame]        = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_WIN_POKEFRAME);
    m_pPanes[PANE_Win_PokeFrame]->SetAlpha(0);

    m_pPanes[PANE_NULL_QRScanGuide]     = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_QR_BG);
    m_pPanes[PANE_NULL_QRScanGuide]->SetVisible(false);


    m_pPanes[PANE_NULL_CameraImage]     = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_CAMERA);
    m_pPanes[PANE_Pic_SilhouetteColor]  = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_PIC_SIL_COLOR);

    /*  se  */
    m_pPanes[PANE_NULL_SE_QRMove]       = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_QRMOVE);
    m_pPanes[PANE_NULL_SE_QREffect]     = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_QREFFECT);
    m_pPanes[PANE_NULL_SE_QRPulseBeat]  = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_QRPULSEBEAT);
    m_pPanes[PANE_NULL_SE_QRPulseBeat2] = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_QRPULSEBEAT2);
    m_pPanes[PANE_NULL_SE_QRPulseBeat3] = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_QRPULSEBEAT3);
    m_pPanes[PANE_NULL_SE_AppearMons]   = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_APPEARMONS);
    m_pPanes[PANE_NULL_SE_CryMons]      = helper.GetPane(PANENAME_QRS_MIN_UPP_000_PANE_NULL_SE_CRYMONS);
  }

  {
    m_pBatteryValueInterp = GFL_NEW(m_pWork->GetDeviceHeap()) BatteryValueInterp(1);
  }


  {
    m_pBatteryGaugeControllers  = GFL_NEW_ARRAY(m_pWork->GetDeviceHeap()) BatteryGaugeController[gaugeAnimTableNum];
    
    for(u32 index = 0; index < gaugeAnimTableNum; ++index)
    {
      m_pBatteryGaugeControllers[index].Setup(this, m_pWork, gaugeAnimTable[index].valueAnim, gaugeAnimTable[index].maxAnim, gaugeCapacity * index, gaugeCapacity * (index+1));
    }
  }

  /*  anims  */
  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000__BG_00_BG_LOOP_00);
  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000__TIMER_IN);
  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000__TIMER_KEEP);


  SetScanCount(0);
  SetViewMode(VIEWMODE_None);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
UpperView_Main::~UpperView_Main()
{
  GFL_SAFE_DELETE(m_pBatteryValueInterp);
  GFL_SAFE_DELETE_ARRAY(m_pBatteryGaugeControllers);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::Update(void)
{
  app::util::G2DUtil* pG2D = GetG2DUtil();


  NetAppLib::System::NetApplicationViewBase::Update();  /*  super  */




  if(m_pQRScanPowerProvider)
  {
    const u8  qrScanPower = m_pQRScanPowerProvider->QRScanPowerProvider_OnQueryPower();
    const u32 timerAnim   = LA_QRREADER_QRS_MIN_UPP_000__TIMER_KEEP;
    bool      isAllMax    = true;

    m_pBatteryValueInterp->SetTarget(qrScanPower, m_IsBatteryGaugeChangeImmediately);
    m_IsBatteryGaugeChangeImmediately = false;

    /*  gauge  */
    m_pBatteryValueInterp->Update();
    for(u32 index = 0; index < gaugeAnimTableNum; ++index)
    {
      m_pBatteryGaugeControllers[index].SetValue(m_pBatteryValueInterp->GetCurrent());
      isAllMax = (isAllMax & m_pBatteryGaugeControllers[index].IsMax());
    }
    
    /*  満タン時はTimerIconとめる  */
    if(isAllMax)
    {
      if(!pG2D->IsAnimePause(0, timerAnim))
      {
        pG2D->SetAnimeFrame(0, timerAnim, 0);
      }
    }
    else
    {
      if(pG2D->IsAnimePause(0, timerAnim))
      {
        pG2D->PauseAnime(0, timerAnim, true);   /*  bool notPause:  trueなら再開, falseなら停止  */
      }
    }
  }

  if(m_bNeedNotifyPokeFrameAnimEnd)
  {
    if(pG2D->IsAnimeEnd(0, LA_QRREADER_QRS_MIN_UPP_000_POKEFRAME))
    {
      Event(IEventHandler::EVENT_PokeFrameAnimEnd);
      m_bNeedNotifyPokeFrameAnimEnd = false;
    }
  }


  UpdatePaneSE();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Main::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->UpperView_Main_OnEvent(eventCode);
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
void  UpperView_Main::UpdatePaneSE(void) const
{
  const struct
  {
    PaneID  paneID;
    u32     seID;
  }paneSEDefTable[] =
  {
    {PANE_NULL_SE_QRMove,         SEQ_SE_SYS_NJ_039},
    {PANE_NULL_SE_QREffect,       SEQ_SE_SYS_NJ_040},
    {PANE_NULL_SE_QRPulseBeat,    SEQ_SE_SYS_NJ_041},
    {PANE_NULL_SE_QRPulseBeat2,   SEQ_SE_SYS_NJ_050},
    {PANE_NULL_SE_QRPulseBeat3,   SEQ_SE_SYS_NJ_052},
//    {PANE_NULL_SE_AppearMons, },
  };



  for(u32 index = 0; index < GFL_NELEMS(paneSEDefTable); ++index)
  {
    if(m_pPanes[paneSEDefTable[index].paneID]->IsVisible())
    {
      Sound::PlaySE(paneSEDefTable[index].seID);
    }
  }

  /*  鳴き声  */
  if((m_TargetMonsNo != MONSNO_NULL) && m_pPanes[PANE_NULL_SE_CryMons]->IsVisible()) Sound::PlayVoice(0, m_TargetMonsNo, m_TargetFormNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::SetBatteryGaugeVisibility(const bool isVisible)
{
  /*  anime  */
  if(m_bGaugeEnable)
  {
    GetG2DUtil()->StartAnime(0, isVisible ? LA_QRREADER_QRS_MIN_UPP_000_POWER_IN : LA_QRREADER_QRS_MIN_UPP_000_POWER_OUT);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::SetBatteryGaugeInvilibeImmediate(void)
{
  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POWER_OUT_IMMEDIATE);
  GetG2DUtil()->StopAnime(0,  LA_QRREADER_QRS_MIN_UPP_000_POWER_IN);
  GetG2DUtil()->StopAnime(0,  LA_QRREADER_QRS_MIN_UPP_000_POWER_OUT);
  GetG2DUtil()->Update2D();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Main::SetScanCount(const u32 scans)
{
  GetG2DUtil()->SetTextBoxPaneNumber(m_pTextBoxes[TEXTBOX_Scans], qrscan_sys_01_05, scans, 5, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Main::StartShilhouetteAnim(void)
{
  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_SILHOUETTECOLOR);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Main::GetSilhouetteColor(gfl2::gfx::Color& rColor) const
{
  rColor.x  = static_cast<f32>(m_pPanes[PANE_Pic_SilhouetteColor]->GetVtxColor(0).r) / 255.0f;
  rColor.y  = static_cast<f32>(m_pPanes[PANE_Pic_SilhouetteColor]->GetVtxColor(0).g) / 255.0f;
  rColor.z  = static_cast<f32>(m_pPanes[PANE_Pic_SilhouetteColor]->GetVtxColor(0).b) / 255.0f;
  rColor.w  = static_cast<f32>(m_pPanes[PANE_Pic_SilhouetteColor]->GetVtxColor(0).a) / 255.0f;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
bool  UpperView_Main::IsDrawing(void) const
{
  bool  isDrawing = false;


  isDrawing |= NetAppLib::System::NetApplicationViewBase::IsDrawing();


  return isDrawing;
}
#endif

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
void  UpperView_Main::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->UpperView_Main_OnEvent(eventCode);
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Main::SetViewMode(const ViewMode viewMode)
{
  if(m_ViewMode == viewMode) return;


  ViewMode oldMode  = m_ViewMode;

  m_ViewMode  = viewMode;

  switch(m_ViewMode)
  {
    case  VIEWMODE_None:
    
      m_pPanes[PANE_NULL_CameraImage]->SetVisible(false);
      m_pPanes[PANE_NULL_QRScanGuide]->SetVisible(false);
      if(m_bGaugeEnable)
      {
        if((oldMode != VIEWMODE_None) && (oldMode != VIEWMODE_PokeFrame))
        {
          GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POWER_OUT);
        }
      }
      break;

    case  VIEWMODE_Camera:
    case  VIEWMODE_OnScan:
      m_pPanes[PANE_NULL_CameraImage]->SetVisible(true);
      m_pPanes[PANE_NULL_QRScanGuide]->SetVisible(m_ViewMode == VIEWMODE_OnScan);
      GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POKEFRAME_RESET);
      if(m_bGaugeEnable)
      {
        if((oldMode != VIEWMODE_Camera) && (oldMode != VIEWMODE_OnScan))
        {
          GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POWER_IN);
        }
      }
      break;

    case  VIEWMODE_PokeFrame:
      m_pPanes[PANE_NULL_CameraImage]->SetVisible(true);
      m_pPanes[PANE_NULL_QRScanGuide]->SetVisible(false);
      GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POKEFRAME);
      m_bNeedNotifyPokeFrameAnimEnd = true;

      if(m_bGaugeEnable)
      {
        if((oldMode != VIEWMODE_None) && (oldMode != VIEWMODE_PokeFrame))
        {
          GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_000_POWER_OUT);
        }
      }
      break;


    default:
      break;
  }


  GetG2DUtil()->Update2D();
}


} /*  namespace QRReader */
} /*  namespace NetApp    */

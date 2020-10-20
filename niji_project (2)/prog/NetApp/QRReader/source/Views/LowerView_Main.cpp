
//==============================================================================
/**
 * @file   LowerView_Main.cpp
 * @date   2016/01/14 Thu. 18:02:45
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
#include  "AppLib/include/Tool/app_tool_ButtonManager.h"
#include  "Sound/include/Sound.h"

#include  "../QRReaderResource.h"
#include  "../System/QRReaderWork.h"

#include  "./LowerView_Main.h"
// =============================================================================
/**/
//==============================================================================
namespace {

  enum  ButtonID
  {
    BUTTON_Back,
    BUTTON_QRScan,
    BUTTON_FieldScan,
    BUTTON_ModeChangeToQRScan,
    BUTTON_ModeChangeToFieldScan,
    BUTTON_Information,

    BUTTON_MAX
  };

  static const struct
  {
    u32   full;
    u32   empty;
    u32   inRange;
  }gaugeAnimTable[] = {
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_00_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_00_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_00_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_01_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_01_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_01_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_02_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_02_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_02_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_03_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_03_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_03_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_04_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_04_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_04_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_05_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_05_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_05_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_06_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_06_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_06_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_07_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_07_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_07_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_08_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_08_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_08_LOOP},
    {LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_09_INANIME,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_09_OFF,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON__PARTS_09_LOOP},
  };
  
  static const u32  gaugeAnimTableNum = GFL_NELEMS(gaugeAnimTable);
  static const u32  gaugeUnitCapacity = 10;   /**<  ゲージ一目盛りあたりの容量  */




  void  SetButtonSE(app::tool::ButtonManager* pButtonManager)
  {
    pButtonManager->SetButtonSelectSE(BUTTON_Back,                   SEQ_SE_CANCEL1);
//    pButtonManager->SetButtonSelectSE(BUTTON_QRScan,                 );
//    pButtonManager->SetButtonSelectSE(BUTTON_FieldScan,              );
    pButtonManager->SetButtonSelectSE(BUTTON_ModeChangeToQRScan,     SEQ_SE_DECIDE1);
    pButtonManager->SetButtonSelectSE(BUTTON_ModeChangeToFieldScan,  SEQ_SE_DECIDE1);
    pButtonManager->SetButtonSelectSE(BUTTON_Information,            SEQ_SE_DECIDE1);
  }
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
  *  @date   2016/03/04 Fri. 11:56:51
  */
//==============================================================================
class LowerView_Main::FieldScanPowerGaugeController
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldScanPowerGaugeController );
public:
  FieldScanPowerGaugeController(void)
    : m_pOwner(NULL)
    , m_Config()
    , m_Value(0)
  {}
  
  virtual  ~FieldScanPowerGaugeController() {}


  static const u32  ANIMID_NOUSE  = UINT_MAX;
  struct  Config
  {
    u32   anim_Full;
    u32   anim_Empty;
    u32   anim_InRange;
    
    u32   valueMin;
    u32   valueMax;

    Config(void)
      : anim_Full(ANIMID_NOUSE)
      , anim_Empty(ANIMID_NOUSE)
      , anim_InRange(ANIMID_NOUSE)
      , valueMin(0)
      , valueMax(1)
    {}
  };

  void  Setup(LowerView_Main* pOwner, const Config& rConfig)
        {
          m_pOwner  = pOwner;
          m_Config  = rConfig;
          SetValue(0);
        }


  void  SetValue(const u8 value, const bool isSuppressFullAnim = false)
        {
          if(m_Value == value)  return;
          
          u32 dispValue = value;
          
          /*  値域に正規化  */
          if(dispValue < m_Config.valueMin)      dispValue = m_Config.valueMin;
          else if(m_Config.valueMax < dispValue) dispValue = m_Config.valueMax;
          
          /*  値変更前にAnimが必要か確認する  */
          bool  isTriggerFullAnim   = ((m_Value < m_Config.valueMax) && (dispValue == m_Config.valueMax));
          bool  isTriggerEmptyAnim  = ((m_Config.valueMin < m_Value) && (dispValue == m_Config.valueMin));
          
          /*  値を変更し、表示反映  */
          m_Value = dispValue;


          if(!isSuppressFullAnim && isTriggerFullAnim)
          {
            /*  full(trigger)  */
            if(m_Config.anim_Full != ANIMID_NOUSE)  {
              if(m_Config.anim_InRange != ANIMID_NOUSE)  m_pOwner->GetG2DUtil()->StopAnime(0, m_Config.anim_InRange);
              m_pOwner->GetG2DUtil()->StartAnime(0, m_Config.anim_Full);
            }
          }
          else if(!isSuppressFullAnim && (m_Value == m_Config.valueMax))
          {
            /*  full */
            /*  nop  */
          }
          else if(isTriggerEmptyAnim)
          {
            /*  empty(trigger)  */
            if(m_Config.anim_Empty != ANIMID_NOUSE)
            {
              if(m_Config.anim_InRange != ANIMID_NOUSE)  m_pOwner->GetG2DUtil()->StopAnime(0, m_Config.anim_InRange);
              m_pOwner->GetG2DUtil()->StartAnime(0, m_Config.anim_Empty);
            }
          }
          else if(m_Value == m_Config.valueMin)
          {
            /*  empty */
            /*  nop  */
          }
          else
          {
            /*  other  */
            if(m_Config.anim_InRange != ANIMID_NOUSE)  m_pOwner->GetG2DUtil()->StartAnime(0, m_Config.anim_InRange);
          }
        }

protected:
  LowerView_Main*   m_pOwner;
  Config            m_Config;

  /*  value */
  u32               m_Value;      /**<  範囲内に正規化した設定値  */
};




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Main::LowerView_Main(QRReaderWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pHandler(NULL)
  , m_pFieldScanPowerProvider(NULL)
  , m_ViewMode(VIEWMODE_None)
  , m_pMessageMenu(NULL)
  , m_pInfoWindow(NULL)
  , m_bFieldScanAvailability(false)
  , m_IsQRScanButtonHolding(false)
  , m_IsFieldScanButtonHolding(false)
  , m_bQRScanButtonForcedHold(false)
  , m_bFieldScanButtonForcedHold(false)
  , m_bRequestedInputClean(false)
  , m_IsInputDirty(false)
  , m_pInfoButtonPane(NULL)
  , m_pSonarPane(NULL)
  , m_pButtonAvailabilityInfos(NULL)
{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();


  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_QRREADER_QRS_MIN_LOW_000_BFLYT,
    LA_QRREADER_QRS_MIN_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    m_pWork->GetMessageUtility(),
    QRREADER_MESSAGE_GRAC_ID
  );

  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
  app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);

  /*  buttons  */
  {
    const app::ui::ButtonInfoEx   buttonInfoEx[BUTTON_MAX]  =
    {
      {
        BUTTON_Back,                                                              //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_BACK_BUTTON).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_TOUCH,
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_CANSEL,
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__BACK_BUTTON_PASSIVE,
      },
      {
        BUTTON_QRScan,                                                            //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_QRSCAN_BUTTON).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_QRS_BTN_000_PANE_BOUND_00),               //  gfl2::lyt::LytPane* bound_pane;
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_RELEASE,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_CANSEL,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_PASSIVE,
      },
      {
        BUTTON_FieldScan,                                                           //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_FIELDSCAN_BUTTON).Peek(), //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_QRS_BTN_001_PANE_BOUND_00),                 //  gfl2::lyt::LytPane* bound_pane;
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_RELEASE,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_CANSEL,
        UINT_MAX,   //        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_NEWPASSIVE,
      },
      {
        BUTTON_ModeChangeToQRScan,                                                        //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_CHANGE_BUTTON).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_TOUCH,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_CANSEL,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_PASSIVE,
      },
      {
        BUTTON_ModeChangeToFieldScan,                                                        //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_CHANGE_BUTTON_00).Peek(),  //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_009_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_TOUCH,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_CANSEL,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__CHANGE_BUTTON_00_PASSIVE,
      },

      {
        BUTTON_Information,                                                       //  app::tool::ButtonManager::ButtonId button_id;
        helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_INFO_BUTTON).Peek(),    //  gfl2::lyt::LytPane* picture_pane;
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00),        //  gfl2::lyt::LytPane* bound_pane;
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_TOUCH,
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_CANSEL,
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_TOUCH_RELEASE,
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_ACTIVE,
        LA_QRREADER_QRS_MIN_LOW_000__INFO_BUTTON_PASSIVE,
      },
    };

    CreateButtonManager( m_pWork->GetAppHeap(), pLayout, buttonInfoEx, GFL_NELEMS(buttonInfoEx) );
    m_pButtonAvailabilityInfos  = GFL_NEW_ARRAY(m_pWork->GetDeviceHeap()) ButtonAvailavilityInfo [BUTTON_MAX];

    m_pInfoButtonPane   = helper.Clear().GetPane(PANENAME_QRS_MIN_LOW_000_PANE_INFO_BUTTON);
    m_pSonarPane        = helper.Clear().GetPane(PANENAME_QRS_MIN_LOW_000_PANE_SEARCH_EFFECT);
    m_pScanPower        = helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_FIELDSCAN_BUTTON).GetTextBoxPane(PANENAME_QRS_BTN_001_PANE_TEXTBOX_00);

    m_pSonarPane->SetVisible(false);

    /*  SE  */
    SetButtonSE(GetButtonManager());


    /*  fixedstring  */
    pG2DUtil->SetTextBoxPaneString(helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_CHANGE_BUTTON   ).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00), qrscan_sel_02_02);    /*  change to qrread    */
    pG2DUtil->SetTextBoxPaneString(helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_CHANGE_BUTTON_00).GetTextBoxPane(PANENAME_COMMON_BTN_LOW_009_PANE_TEXTBOX_00), qrscan_sel_02_01);    /*  change to fieldscan */
  }

  /*  messagemenu  */
  {
    m_pMessageMenu  = GFL_NEW(pHeapBase) MessageMenu(m_pWork);


    m_pMessageMenu->Setup(RESOURCE_LayoutCommonWindow, RESOURCE_LayoutCommonMenuCursor, m_pWork->GetMessageUtility());
    m_pMessageMenu->SetEventHandler(this);
    AddSubView(m_pMessageMenu);
  }

  /*  infowin  */
  {
    m_pInfoWindow = GFL_NEW(pHeapBase) LowerView_InfoWindow(m_pWork);
    m_pInfoWindow->SetEventHandler(this);
    AddSubView(m_pInfoWindow);
    m_pInfoWindow->SetVisible(false);
  }


  /*  gauge */
  {
    FieldScanPowerGaugeController::Config conf;

    m_pFieldScanPowerGaugeControllers  = GFL_NEW_ARRAY(m_pWork->GetDeviceHeap()) FieldScanPowerGaugeController[gaugeAnimTableNum];
    
    for(u32 index = 0; index < gaugeAnimTableNum; ++index)
    {
      conf.anim_Full    = gaugeAnimTable[index].full;
      conf.anim_Empty   = gaugeAnimTable[index].empty;
      conf.anim_InRange = gaugeAnimTable[index].inRange;
          
      conf.valueMin     = gaugeUnitCapacity * (index);
      conf.valueMax     = gaugeUnitCapacity * (index+1);
      
      m_pFieldScanPowerGaugeControllers[index].Setup(this, conf);
    }
  }

  /*  RIcon表示制御  */
  {
    const bool  isLisA  = (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetConfig()->GetButtonMode() == Savedata::ConfigSave::BUTTON_MODE_L_EQUAL_A);
    
    gfl2::lyt::LytPane* const  pPanes[]  =
    {
      helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_QRSCAN_BUTTON).GetPane(PANENAME_QRS_BTN_000_PANE_NULL_RICON),
      helper.Clear().Push(PANENAME_QRS_MIN_LOW_000_PANE_FIELDSCAN_BUTTON).GetPane(PANENAME_QRS_BTN_001_PANE_NULL_RICON),
    };

    for(u32 index = 0; index < GFL_NELEMS(pPanes); ++index)
    {
      pPanes[index]->SetVisible(!isLisA);
    }
  }

  /*  anim  */
  {
    pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__BG_00_BG_LOOP_00);
  }

  SetInputListener(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
LowerView_Main::~LowerView_Main()
{
  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();
  if(m_pInfoWindow)   m_pInfoWindow->RemoveFromSuperView();


  GFL_SAFE_DELETE_ARRAY(m_pFieldScanPowerGaugeControllers);
  GFL_SAFE_DELETE_ARRAY(m_pButtonAvailabilityInfos);
  GFL_SAFE_DELETE(m_pInfoWindow);
  GFL_SAFE_DELETE(m_pMessageMenu);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::Update(void)
{
  app::util::G2DUtil*   pG2DUtil    = GetG2DUtil();

  m_bQRScanButtonForcedHold     = false;
  m_bFieldScanButtonForcedHold  = false;


  NetAppLib::System::NetApplicationViewBase::Update();  /*  super  */
  
  HoldButtonUpdate();


  if(m_bRequestedInputClean)
  {
    m_bRequestedInputClean  = m_IsInputDirty;
  }


  if((m_ViewMode == VIEWMODE_FieldScan) && m_pFieldScanPowerProvider)
  {
    const f32 animFrames            = 10.0f;
    const f32 scanPowerPerAnimFrame = 10;
    bool      isFull                = false;
    bool      isActive              = false;
    const u32 fieldScanPower        = m_pFieldScanPowerProvider->IFieldScanPowerProvider_OnQueryPower(isFull, isActive);
    const u32 digits                = 3;
    f32       animFrame;
    
    
    animFrame = static_cast<f32>(fieldScanPower) / scanPowerPerAnimFrame;
    animFrame = (animFrame < animFrames) ? animFrame : animFrames;        /*  max  */

    /*  %  */
    pG2DUtil->SetTextBoxPaneNumber(m_pScanPower, qrscan_sys_02_09, fieldScanPower, digits, 0, print::NUM_DISP_LEFT);
//    pG2DUtil->StartAnimeFrame(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_GAUGE_ANIME, animFrame, false);

    /*  gauge  */
    {
      for(u32 index = 0; index < gaugeAnimTableNum; ++index)
      {
        /*
          2016/03/28  Yu Muto
          FieldScan発動中も先行してScanPowerをためられるようにとの要望
        */
//        m_pFieldScanPowerGaugeControllers[index].SetValue(fieldScanPower, isActive);
        m_pFieldScanPowerGaugeControllers[index].SetValue(fieldScanPower, false);
      }
    }

    /*  antenna  */
    if(isActive)
    {
      if(pG2DUtil->IsAnimeEnd(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_ACTIVE))
      {
        pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_ACTIVE);
        pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_FIELDSCAN_WAITLOOP);
      }
    }
    else if(isFull)
    {
      if(pG2DUtil->IsAnimeEnd(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_FIELDSCAN_WAITLOOP))
      {
        pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_ACTIVE);
        pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_FIELDSCAN_WAITLOOP);
      }
    }
    else
    {
      pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_ACTIVE);
      pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_FIELDSCAN_WAITLOOP);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_INACTIVE);
    }

    /*  effect  */
    if(isActive)
    {
      m_pSonarPane->SetVisible(true);
      if(pG2DUtil->IsAnimeEnd(0, LA_QRREADER_QRS_MIN_LOW_000_SEARCH_EFFECT))
      {
        pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_SEARCH_EFFECT);
        /*  NMcat[1694]   【QRスキャン】しまスキャン発動時、SEの再生され始めるタイミングが遅い  */
        /*    発動の瞬間にも鳴らす必要がある                                                    */
        Sound::PlaySE(SEQ_SE_SYS_NJ_036);
      }
      else
      {
        if(pG2DUtil->GetAnimeFrame(0, LA_QRREADER_QRS_MIN_LOW_000_SEARCH_EFFECT) < FLT_EPSILON)
        {
          Sound::PlaySE(SEQ_SE_SYS_NJ_036);
        }
      }
    }
    else
    {
      m_pSonarPane->SetVisible(false);
      pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000_SEARCH_EFFECT);
    }
  }
  else
  {
    /*  scaneffect  */
    m_pSonarPane->SetVisible(false);
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000_SEARCH_EFFECT);
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ANTENNA_ACTIVE);   /*  同期のためアンテナも止める  */
  }

  m_IsInputDirty  = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  LowerView_Main::IsDrawing(void) const
{
  bool  isDrawing = false;


  isDrawing |= NetAppLib::System::NetApplicationViewBase::IsDrawing();
  isDrawing |= m_pMessageMenu->IsDrawing();

  return isDrawing;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Main_OnEvent(eventCode);
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
void  LowerView_Main::SetViewMode(const ViewMode viewMode, const ViewModeSubState subState)
{
  const ViewMode            oldViewMode     = m_ViewMode;
  app::util::G2DUtil*       pG2DUtil        = GetG2DUtil();
  app::tool::ButtonManager* pButtonManager  = GetButtonManager();
  bool                      isFullMode      = true;
  bool                      bNeedUpdate2D   = false;


  /*  disabling all buttons  */
  {
//    pButtonManager->SetButtonVisible();
    for(u32 buttonID = 0; buttonID < BUTTON_MAX; ++buttonID)
    {
      m_pButtonAvailabilityInfos[buttonID].request  = false;
    }
  }

  m_ViewMode  = viewMode;

  switch(m_ViewMode)
  {
    case  VIEWMODE_None:  /*  fallthrough */
    {
      if(oldViewMode == VIEWMODE_QRRead)          pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_QRSCAN_BUTTON_OUT);
      else if(oldViewMode == VIEWMODE_FieldScan)  pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_FIELDSCAN_BUTTON_OUT);
    }
      break;

    case  VIEWMODE_QRRead:
    {
      {
        const u32 buttonIDs[] =
        {
          BUTTON_Back,
          BUTTON_QRScan,
//         BUTTON_FieldScan,
//          BUTTON_ModeChangeToQRScan,
          BUTTON_ModeChangeToFieldScan,
          BUTTON_Information,
        };
        for(u32 index = 0; index < GFL_NELEMS(buttonIDs); ++index)
        {
          m_pButtonAvailabilityInfos[buttonIDs[index]].request  = true;
        }
      }

      /*
        sub
      */
      if(subState == VIEWMODE_SUB_QRReadOnScan)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_QRSearching);
      }
      else if(subState == VIEWMODE_SUB_QRReadLessPower)
      {
        m_pButtonAvailabilityInfos[BUTTON_QRScan].request  = false;
        m_pMessageMenu->Show(MessageMenu::MODE_LowBattery);

      }
      else if(subState == VIEWMODE_SUB_QRSimpleRead)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_QRReadGuide);

        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToQRScan].request     = false;
        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToFieldScan].request  = false;
        m_pButtonAvailabilityInfos[BUTTON_Information].request            = false;

        isFullMode  = false;
      }
      else if(subState == VIEWMODE_SUB_QRSimpleReadOnScan)
      {
        m_pMessageMenu->Show(MessageMenu::MODE_QRSearching);

        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToQRScan].request     = false;
        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToFieldScan].request  = false;
        m_pButtonAvailabilityInfos[BUTTON_Information].request            = false;

        isFullMode  = false;
      }
      else
      {
        m_pMessageMenu->Show(MessageMenu::MODE_QRReadGuide);
      }

      if(oldViewMode == VIEWMODE_None)            pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_QRSCAN_BUTTON_IN);
      else if(oldViewMode == VIEWMODE_FieldScan)  pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_CHANGE_01);
    }
      break;

    /*
    */
    case  VIEWMODE_FieldScan:
    {
      if(subState == VIEWMODE_SUB_FieldScan_Charge)
      {
        /*  チャージ演出  */
        m_pButtonAvailabilityInfos[BUTTON_FieldScan].request          = false;
        m_pButtonAvailabilityInfos[BUTTON_Back].request               = false;
        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToQRScan].request = false;
        m_pButtonAvailabilityInfos[BUTTON_Information].request        = false;
      }
      else if(subState == VIEWMODE_SUB_FieldScan_Running)
      {
        /*  発動中  */
        m_pButtonAvailabilityInfos[BUTTON_FieldScan].request          = false;
        m_pButtonAvailabilityInfos[BUTTON_Back].request               = true;
        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToQRScan].request = true;
        m_pButtonAvailabilityInfos[BUTTON_Information].request        = true;
      }
      else
      {
        /*  通常  */
        m_pButtonAvailabilityInfos[BUTTON_FieldScan].request          = m_bFieldScanAvailability;
        m_pButtonAvailabilityInfos[BUTTON_Back].request               = m_bFieldScanAvailability;
        m_pButtonAvailabilityInfos[BUTTON_ModeChangeToQRScan].request = m_bFieldScanAvailability;
        m_pButtonAvailabilityInfos[BUTTON_Information].request        = m_bFieldScanAvailability;
      }

      if(oldViewMode == VIEWMODE_None)            pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_FIELDSCAN_BUTTON_IN);
      else if(oldViewMode == VIEWMODE_QRRead)     pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000_CHANGE_00);
    }
      break;


    default:
      break;
  }


  bNeedUpdate2D = (oldViewMode != m_ViewMode);
  for(u32 buttonID = 0; buttonID < BUTTON_MAX; ++buttonID)
  {
    if(m_pButtonAvailabilityInfos[buttonID].request != m_pButtonAvailabilityInfos[buttonID].current)
    {
      SetButtonEnable(buttonID, m_pButtonAvailabilityInfos[buttonID].request);
      bNeedUpdate2D = true;
    }
    
    m_pButtonAvailabilityInfos[buttonID].current  = m_pButtonAvailabilityInfos[buttonID].request;
  }

  /*  full/simple  */
  pG2DUtil->StartAnime(0, isFullMode ? LA_QRREADER_QRS_MIN_LOW_000_TAG_FULLMODE : LA_QRREADER_QRS_MIN_LOW_000_TAG_SIMPLEMODE);

  if(bNeedUpdate2D)
  {
    pG2DUtil->Update2D();
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
void  LowerView_Main::ShowInformation(void)
{
  m_pInfoWindow->SetVisible(true);
  m_pInfoWindow->SetModal(true);
  m_pInfoWindow->SetViewMode(LowerView_InfoWindow::VIEWMODE_Instruction_All, this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Main::HideInformation(void)
{
  m_pInfoWindow->SetVisible(false);
  m_pInfoWindow->SetModal(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Main::OnLayoutPaneTouchTriggerEvent( const u32 buttonID )
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Main::OnLayoutPaneEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;  /*  DISABLE_ACCESS  */

  m_IsInputDirty  = true;
  if(m_bRequestedInputClean)  return  app::ui::UIInputListener::DISABLE_ACCESS;


  switch(buttonID)
  {
    case  BUTTON_Back:
      Event(LowerView_Main::IEventHandler::EVENT_Back);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_ModeChangeToQRScan:      /*  fall through  */
    case  BUTTON_ModeChangeToFieldScan:
      Event(LowerView_Main::IEventHandler::EVENT_ModeChange);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_QRScan:
      Event(LowerView_Main::IEventHandler::EVENT_QRRead);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_FieldScan:
      Event(LowerView_Main::IEventHandler::EVENT_FieldScan);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Information:
      Event(LowerView_Main::IEventHandler::EVENT_Information);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
  }


  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Main::OnLayoutPaneLongPressEvent( const u32 buttonID )
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;  /*  DISABLE_ACCESS  */

  m_IsInputDirty  = true;
  if(m_bRequestedInputClean)  return  app::ui::UIInputListener::DISABLE_ACCESS;

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Main::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult  result            = app::ui::UIInputListener::ENABLE_ACCESS;  /*  DISABLE_ACCESS  */
  app::tool::ButtonManager*                 pUIButtonManager  = GetButtonManager();

  m_IsInputDirty  = true;

  if( IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }

  if(m_bRequestedInputClean)  return  DISABLE_ACCESS;

  /*
    shortcut
  */
  {
    const struct  {
      u32                                 triggerButtonID;
      app::tool::ButtonManager::ButtonId  shotrcutUIButtonID;
    }shortcutTable[]  = {
//      {gfl2::ui::BUTTON_A, m_ShortcutButton_A},
      {gfl2::ui::BUTTON_B, BUTTON_Back},
      {gfl2::ui::BUTTON_X, BUTTON_ModeChangeToQRScan},
      {gfl2::ui::BUTTON_X, BUTTON_ModeChangeToFieldScan},
//      {gfl2::ui::BUTTON_Y, m_ShortcutButton_Y},
//      {gfl2::ui::BUTTON_L, m_ShortcutButton_L},
//      {gfl2::ui::BUTTON_R, m_ShortcutButton_R},
    };


    /*  特殊系  */
    {
      if(pButton->IsHold(gfl2::ui::BUTTON_R) || pButton->IsHold(gfl2::ui::BUTTON_A))
      {
        if(GetButtonManager()->IsButtonActive(BUTTON_QRScan))
        {
          m_bQRScanButtonForcedHold = true;
          return DISABLE_ACCESS;
        }

        if(GetButtonManager()->IsButtonActive(BUTTON_FieldScan))
        {
          m_bFieldScanButtonForcedHold  = true;
          return DISABLE_ACCESS;
        }
      }
    }


    /*  通常系  */
    for(int num = 0; num < GFL_NELEMS(shortcutTable); ++num)
    {
      if((shortcutTable[num].shotrcutUIButtonID != app::tool::ButtonManager::BUTTON_ID_NULL) && pButton->IsTrigger(shortcutTable[num].triggerButtonID))
      {
        /*  入力に対応したUIボタンを動作させる  */
        if(GetButtonManager()->IsButtonActive(shortcutTable[num].shotrcutUIButtonID))
        {
          pUIButtonManager->StartSelectedAct( shortcutTable[num].shotrcutUIButtonID );
          return DISABLE_ACCESS;
        }
      }
    }
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Main::OnSetInputEnabled( bool isEnable )
{
  m_pInfoWindow->SetInputEnabled(isEnable);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::MessageMenuView_OnEvent(const MessageMenu::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  MessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
      Event(LowerView_Main::IEventHandler::EVENT_EOM);
      break;

    case  MessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
      Event(LowerView_Main::IEventHandler::EVENT_MessageFinish);
      break;

    case  MessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
      break;

    case  MessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
      Event(LowerView_Main::IEventHandler::EVENT_Message_Yes);
      break;

    case  MessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
      Event(LowerView_Main::IEventHandler::EVENT_Message_No);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::LowerView_InfoWindow_OnEvent(const LowerView_InfoWindow::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  LowerView_InfoWindow::IEventHandler::EVENT_Exit:
      HideInformation();
      Event(LowerView_Main::IEventHandler::EVENT_InformationEnd);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::IInfoWindowOpenPositionProvider_OnQueryInfoWindowPosition(gfl2::math::Vector3& rPos)
{
  rPos.x  = m_pInfoButtonPane->GetGlobalMtx()._03;
  rPos.y  = m_pInfoButtonPane->GetGlobalMtx()._13;
//  rPos.z  = m_pInfoButtonPane->GetGlobalMtx()._23;
  rPos.z  = 0.0f;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Main::HoldButtonUpdate(void)
{
  app::util::G2DUtil*   pG2DUtil    = GetG2DUtil();



  /*  se  */

  /*  qrscan  */
  if((GetButtonManager()->GetHoldingButtonId()==BUTTON_QRScan) || m_bQRScanButtonForcedHold)
  {
    Event(LowerView_Main::IEventHandler::EVENT_QRScanButtonHolding);

    if(!m_IsQRScanButtonHolding)
    {
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH);
//      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH_RELEASE);
      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_RELEASE);
      if(!Sound::IsSEPlaying(SEQ_SE_SYS_NJ_024))  Sound::PlaySE(SEQ_SE_SYS_NJ_024);
      m_IsQRScanButtonHolding = true;
    }
  }
  else
  {
    if(m_IsQRScanButtonHolding)
    {
      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH);
//      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_TOUCH_RELEASE);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__QRSCAN_BUTTON_RELEASE);
      if(Sound::IsSEPlaying(SEQ_SE_SYS_NJ_024))  Sound::StopSE(SEQ_SE_SYS_NJ_024);
      m_IsQRScanButtonHolding = false;
    }
  }


  /*  fieldscan  */
  if((GetButtonManager()->GetHoldingButtonId()==BUTTON_FieldScan) || m_bFieldScanButtonForcedHold)
  {
    Event(LowerView_Main::IEventHandler::EVENT_FieldScanButtonHolding);

    if(!m_IsFieldScanButtonHolding)
    {
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH);
//      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH_RELEASE);
      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_RELEASE);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ROTATE);

      /*  SE  */
      Sound::PlaySE(SEQ_SE_SYS_NJ_035);

      m_IsFieldScanButtonHolding  = true;
    }
  }
  else
  {
    if(m_IsFieldScanButtonHolding)
    {
      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH);
//      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_TOUCH_RELEASE);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_RELEASE);
      pG2DUtil->StopAnime(0,  LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ROTATE);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_LOW_000__FIELDSCAN_BUTTON_ROTATE_RESET);
      /*  SE  */
      Sound::StopSE(SEQ_SE_SYS_NJ_035);
      Sound::PlaySE(SEQ_SE_SYS_NJ_038);

      m_IsFieldScanButtonHolding  = false;
    }
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
bool  LowerView_Main::IsShowingInformation(void) const
{
  return (m_pInfoWindow->IsVisible());
}



} /*  namespace QRReader */
} /*  namespace NetApp    */

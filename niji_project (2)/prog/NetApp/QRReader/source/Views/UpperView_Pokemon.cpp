
//==============================================================================
/**
 * @file   UpperView_Pokemon.cpp
 * @date   2016/01/18 Mon. 15:38:01
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

/*
  R:\home\niji\git_program\niji_project\prog\App\Zukan\source\ZukanViewInfoFunc.cpp

  void ZukanViewInfoFunc::SetContents(

  PokeTool::ExtendData*
  u32 tableIndex = m_param.tool->GetPokeExtendData()->GetTableIndex(sp.monsNo, sp.formNo);

  // ねずみポケモン
  // たかさ値
  // おもさ値
*/



// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  <niji_conv_header/field/zone/zone_id.h>

#include  "GameSys/include/GameData.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include  "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"

#include  "../QRReaderResource.h"
#include  "../System/QRReaderWork.h"

#include  "./UpperView_Pokemon.h"

// =============================================================================
/**/
//==============================================================================
namespace {
}





namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Pokemon::UpperView_Pokemon(QRReaderWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
//  , m_pHandler(NULL)
  , m_pRemainingTimeProvider(NULL)
  , m_ViewMode(VIEWMODE_None)
{
//  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();


  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_QRREADER_QRS_MIN_UPP_001_BFLYT,
    LA_QRREADER_QRS_MIN_UPP_001___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    m_pWork->GetMessageUtility(),
    QRREADER_MESSAGE_GRAC_ID
  );

  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
  app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);


  /*  pane  */
  {
/*
    enum  TextboxID
    {
      TEXTBOX_Label_Place,
      TEXTBOX_Label_RemainTime,

      TEXTBOX_Island,
      TEXTBOX_Area,
      TEXTBOX_RemainTime,

      TEXTBOX_NUM
    };
    gfl2::lyt::LytTextBox*        m_pTextBoxes[TEXTBOX_NUM];

  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_00,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_01,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_02,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_04,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_03,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_05,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_06,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_07,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_08,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_09,
  PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_10,
*/

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWork->GetMessageUtility();

    /*  fieldscan  */
    m_pTextBoxes[TEXTBOX_Label_Place]         = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_10);
//    m_pTextBoxes[TEXTBOX_Label_RemainTime]    = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_06);

    m_pTextBoxes[TEXTBOX_Island]              = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_01);
    m_pTextBoxes[TEXTBOX_Area]                = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_09);
    m_pTextBoxes[TEXTBOX_RemainTime]          = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_07);

    /*  qrscan  */
//    m_pTextBoxes[TEXTBOX_QRScan_Label_Height] = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_02);
//    m_pTextBoxes[TEXTBOX_QRScan_Label_Weight] = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_03);

    m_pTextBoxes[TEXTBOX_QRScan_MonsName]     = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_00);
//    m_pTextBoxes[TEXTBOX_QRScan_MonsType]     = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_01);
//    m_pTextBoxes[TEXTBOX_QRScan_Height]       = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_04);
//    m_pTextBoxes[TEXTBOX_QRScan_Weight]       = helper.GetTextBoxPane(PANENAME_QRS_MIN_UPP_001_PANE_TEXTBOX_05);

    /*  fixedstrings  */
    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Label_Place],         &pMessageUtility->GetString(QRREADER_MESSAGE_GRAC_ID, qrscan_sys_02_01));
//    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Label_RemainTime],    &pMessageUtility->GetString(QRREADER_MESSAGE_GRAC_ID, qrscan_sys_02_07));
//    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_QRScan_Label_Height], &pMessageUtility->GetString(QRREADER_MESSAGE_GRAC_ID, qrscan_sys_02_01));
//    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_QRScan_Label_Weight], &pMessageUtility->GetString(QRREADER_MESSAGE_GRAC_ID, qrscan_sys_02_07));

    /*  picture  */
    m_pPictures[PICTURE_TypeIcon_0]   = helper.Clear().Push(PANENAME_QRS_MIN_UPP_001_PANE_PARTSTYPE_00).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);
    m_pPictures[PICTURE_TypeIcon_1]   = helper.Clear().Push(PANENAME_QRS_MIN_UPP_001_PANE_PARTSTYPE_01).GetPicturePane(PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE);
    helper.Clear();

    /*  panes  */
    m_pPanes[PANE_Null_Discover]      = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_DISCOVER);
    m_pPanes[PANE_Null_Capture]       = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_CAPTURE);
    m_pPanes[PANE_NULL_ScanResultBG]  = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_BG_Y);

    m_pPanes[PANE_GenderIcon_0]       = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_PARTS_GENDER_00);
    m_pPanes[PANE_GenderIcon_1]       = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_PARTS_GENDER_01);
    m_pPanes[PANE_StarIcon]           = helper.GetPane(PANENAME_QRS_MIN_UPP_001_PANE_STAR_ICON);

  }


  /*  anim  */
  {
    pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__TIME_IN);
    pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__TIME_KEEP);
    pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR);
    pG2DUtil->SetAnimeFrame(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR, 0.0f);
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
UpperView_Pokemon::~UpperView_Pokemon()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Pokemon::Update(void)
{
  NetAppLib::System::NetApplicationViewBase::Update();  /*  super  */

  if((m_ViewMode == VIEWMODE_Discover) && m_pRemainingTimeProvider)
  {
    const u32                   digits            = 2;
    const print::NumberDispType dispType          = print::NUM_DISP_ZERO;
    const print::NumberCodeType codeType          = print::NUM_CODE_HANKAKU;
    const u32                   remainingMinutes  = m_pRemainingTimeProvider->IRemainingTimeProvider_OnQueryMinutes();
    const u32                   remainingSeconds  = m_pRemainingTimeProvider->IRemainingTimeProvider_OnQuerySeconds();
    app::util::G2DUtil*         pG2DUtil          = GetG2DUtil();

    pG2DUtil->SetRegisterNumber(0, remainingMinutes, digits, dispType, codeType);
    pG2DUtil->SetRegisterNumber(1, remainingSeconds, digits, dispType, codeType);
    pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_RemainTime], qrscan_sys_02_08);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Pokemon::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0, gfl2::lyt::ORDER_NORMAL, 1);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
bool  UpperView_Pokemon::IsDrawing(void) const
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
void  UpperView_Pokemon::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->UpperView_Pokemon_OnEvent(eventCode);
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
void  UpperView_Pokemon::SetViewMode(const ViewMode viewMode)
{
  const ViewMode  oldMode = m_ViewMode;

  m_ViewMode  = viewMode;

  switch(m_ViewMode)
  {
    /*  fieldscan  */
    case  VIEWMODE_Discover:
      m_pPanes[PANE_Null_Discover]->SetVisible(true);
      m_pPanes[PANE_Null_Capture]->SetVisible(false);
      m_pPanes[PANE_NULL_ScanResultBG]->SetVisible(true);

      if(oldMode == VIEWMODE_None)    GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_FIELD_IN);
      if(oldMode == VIEWMODE_Capture) GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_QR_OUT);
      break;

    /*  qrscan  */
    case  VIEWMODE_Capture:
      m_pPanes[PANE_Null_Discover]->SetVisible(false);
      m_pPanes[PANE_Null_Capture]->SetVisible(true);
      m_pPanes[PANE_NULL_ScanResultBG]->SetVisible(true);

      if(oldMode != VIEWMODE_Capture)   GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_QR_IN);
      if(oldMode == VIEWMODE_Discover)  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_FIELD_OUT);
      break;

    case  VIEWMODE_None:
      m_pPanes[PANE_Null_Discover]->SetVisible(false);
      m_pPanes[PANE_Null_Capture]->SetVisible(false);
      m_pPanes[PANE_NULL_ScanResultBG]->SetVisible(false);

      if(oldMode == VIEWMODE_Capture)   GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_QR_OUT);
      if(oldMode == VIEWMODE_Discover)  GetG2DUtil()->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001_FIELD_OUT);
      break;


    default:
      break;
  }


  GetG2DUtil()->Update2D();
}




//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Pokemon::ShowFieldScanResult(const u8 islandID, const Field::ZoneID zoneID, const MonsNo targetMonsNo)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();


  pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Island], FieldFlagUtility::IslandIDtoIslandNameMessageID(islandID));

  /*  zone名  */
  {
    GameSys::GameData*      pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    void*                   pZoneData = pGameData->GetFieldZoneDataLoader()->GetZoneData( zoneID );
    Field::ZoneDataAccessor accessor(pZoneData);

    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Area],   &(m_pWork->GetMessageUtility()->GetString(QRREADER_MESSAGE_PLACENAME_ARCDAT, accessor.GetPlaceLabelID())));
  }


  SetViewMode(VIEWMODE_Discover);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Pokemon::HideResult(void)
{
  SetViewMode(VIEWMODE_None);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Pokemon::ShowQRReadResult(const pml::pokepara::CoreParam* pCoreParam, bool isSameFormBothGender)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  print::WordSet*       pWordSet  = m_pWork->GetMessageUtility()->GetWordSet();


  SetViewMode(VIEWMODE_Capture);


  /*  種族名  */
  pG2DUtil->SetRegisterMonsName(0, pCoreParam);
  pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_QRScan_MonsName], qrscan_sys_01_01);

  /*  タイプ  */
//  qrscan_sys_01_02
//  m_pTextBoxes[TEXTBOX_QRScan_MonsType]

  /*  性別  */
  {
//    const pml::Sex  gender  = pCoreParam->GetSex();
    const pml::Sex  gender  = PokeTool::GetDrawSex(pCoreParam);   /*  「ニドラン♂」のようなケースに対応するため、CoreParamから直接取得しないようにする  */
   

    /*
      NMCat[831]:   性別による違いがないポケモンの図鑑QRについての質問
      trueの場合、雌雄で同じ見た目である→pokeparaに設定される性別値には意味がないため、両方の記号を表示する
      それ以外は、雌雄で見た目が違ったり性別自体がないものであるため、設定値に従い分岐
    */
    /*  unbind  */
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_00_ICON_MALE);
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_00_ICON_FEMALE);
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_01_ICON_MALE);
    pG2DUtil->StopAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_01_ICON_FEMALE);
    if(isSameFormBothGender)
    {
      m_pPanes[PANE_GenderIcon_0]->SetVisible(true);
      m_pPanes[PANE_GenderIcon_1]->SetVisible(true);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_00_ICON_MALE);
      pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_01_ICON_FEMALE);
      pG2DUtil->SetAnimeFrame(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR, 2.0f);
    }
    else
    {
      switch(gender)
      {
        case pml::SEX_MALE:
          m_pPanes[PANE_GenderIcon_0]->SetVisible(true);
          m_pPanes[PANE_GenderIcon_1]->SetVisible(false);
          pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_00_ICON_MALE);
          pG2DUtil->SetAnimeFrame(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR, 1.0f);
          break;

        case pml::SEX_FEMALE:
          m_pPanes[PANE_GenderIcon_0]->SetVisible(true);
          m_pPanes[PANE_GenderIcon_1]->SetVisible(false);
          pG2DUtil->StartAnime(0, LA_QRREADER_QRS_MIN_UPP_001__PARTS_GENDER_00_ICON_FEMALE);
          pG2DUtil->SetAnimeFrame(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR, 1.0f);
          break;

        case pml::SEX_UNKNOWN:  /*  fall through  */
        default:
          m_pPanes[PANE_GenderIcon_0]->SetVisible(false);
          m_pPanes[PANE_GenderIcon_1]->SetVisible(false);
          pG2DUtil->SetAnimeFrame(0, LA_QRREADER_QRS_MIN_UPP_001_RARESTAR, 0.0f);
          break;
      }
    }
  }

  /*  レアリティ  */
  {
    m_pPanes[PANE_StarIcon]->SetVisible(pCoreParam->IsRare());
  }

  /*  属性  */
  {
    const MonsNo  monsNo  = pCoreParam->GetMonsNo();
    const FormNo  formNo  = pCoreParam->GetFormNo();

    pml::personal::LoadPersonalData(monsNo, formNo);
    {
      PokeType_tag  poketype[2] = {
                      static_cast<PokeType_tag>(pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type1)),
                      static_cast<PokeType_tag>(pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type2))
                    };

      /*
        NMCat[2794]   ノーマルタイプ以外のシルヴァディQRコードを読み込んだ際、タイプが「ノーマル」と表示される
        図鑑側の
          GFNMCat[3114] シルヴァディのタイプアイコンが「ノーマル」のまま
        と同じ対処を施す
      */
      if((monsNo == MONSNO_ARUSEUSU) || (monsNo == MONSNO_GURIPUSU2))
      {
        poketype[0] =
        poketype[1] = static_cast<PokeType_tag>(formNo);
      }
      
      /*  同値補正  */
      if(poketype[0] == poketype[1])  poketype[1] = POKETYPE_NULL;


      m_pPictures[PICTURE_TypeIcon_0]->SetVisible(true);
      m_pPictures[PICTURE_TypeIcon_1]->SetVisible(true);
      if(m_pWork->m_pCommonIconData->ReplacePaneTextureByWazaTypeIcon(poketype[0], m_pPictures[PICTURE_TypeIcon_0]))
      {
        /*  一つ目成功 */

        if(m_pWork->m_pCommonIconData->ReplacePaneTextureByWazaTypeIcon(poketype[1], m_pPictures[PICTURE_TypeIcon_1]))
        {
          /*  両方成功  */
          /*  両方表示  */
        }
        else
        {
          /*  二つ目失敗    */
          /*  二つ目非表示  */
          m_pPictures[PICTURE_TypeIcon_1]->SetVisible(false);
        }
      }
      else
      {
        /*  一つ目失敗  */
        if(m_pWork->m_pCommonIconData->ReplacePaneTextureByWazaTypeIcon(poketype[1], m_pPictures[PICTURE_TypeIcon_0]))   /*  前詰めのため#0に対して行う  */
        {
          /*  二つ目成功        */
          /*  前詰めで一つだけ  */
          m_pPictures[PICTURE_TypeIcon_1]->SetVisible(false);
        }
        else
        {
          /*  両方失敗  */
          m_pPictures[PICTURE_TypeIcon_0]->SetVisible(false);
          m_pPictures[PICTURE_TypeIcon_1]->SetVisible(false);
        }
      }
    }
  }
}

} /*  namespace QRReader */
} /*  namespace NetApp    */

//==============================================================================
/**
 * @file   SurfUI_Lower_Tutorial.cpp
 * @date   2017/01/19 Thu. 11:27:51
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <math/include/gfl2_Math.h>
#include  <System/include/GflUse.h>
#include  <AppLib/include/Util/app_util_GetPaneHelper.h>
#include  <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>

#include  "../SurfUIResource.h"
#include  "./SurfUI_Lower_Tutorial.h"


namespace App {
namespace MantainSurf {
namespace UI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_SurfUI_Lower_Tutorial
namespace FILELOCAL {

  enum
  {
    STATE_Initial,
    STATE_Idle,

    /*  表示中  */
    STATE_Showing,

    STATE_WaitForExit,
    STATE_Exit,

    STATE_
  };

  /*  contents  */
#if 0
    TUTORIAL_Info01,   /**<  マンタインサーフは　もくてきのしまに…  */
    TUTORIAL_Info02,   /**<  マンタインサーフは　オートで進みます…  */
      /*  （速度アップ操作練習）  */
    TUTORIAL_Info03,   /**<  レッスン1　終了！…                     */
    TUTORIAL_Info04,   /**<  レッスン2　障害物…                     */
      /*  （障害物操作練習）  */
    TUTORIAL_Info05,   /**<  レッスン2　クリア！…                   */
    TUTORIAL_Info06,   /**<  レッスン3　ジャンプ…                   */
      /*  （ジャンプ操作練習）  */
    TUTORIAL_Info07,   /**<  レッスン3　クリア！…                   */
    TUTORIAL_Info08,   /**<  レッスン4　エアリアル技…               */
      /*  （エアリアル操作練習）  */
    TUTORIAL_Info09,   /**<  レッスン4　クリア！…                   */
    TUTORIAL_Info10,   /**<  レッスン5　着水…                       */
      /*  （着水操作練習）  */
    TUTORIAL_Info11,   /**<  レッスン5　クリア!…                    */
    TUTORIAL_Info12,   /**<  全てのレッスン　クリア!…               */

  LYTRES_SURFUI_INFO_SURF_00_BFLIM,
  LYTRES_SURFUI_INFO_SURF_01_BFLIM,
  LYTRES_SURFUI_INFO_SURF_02_BFLIM,
  LYTRES_SURFUI_INFO_SURF_03_BFLIM,
  LYTRES_SURFUI_INFO_SURF_04_BFLIM,
  LYTRES_SURFUI_INFO_SURF_05_BFLIM,
  LYTRES_SURFUI_INFO_SURF_06_BFLIM,
  LYTRES_SURFUI_INFO_SURF_07_BFLIM,
  LYTRES_SURFUI_INFO_SURF_08_BFLIM,
  LYTRES_SURFUI_INFO_SURF_09_BFLIM,
  LYTRES_SURFUI_INFO_SURF_10_BFLIM,
  LYTRES_SURFUI_INFO_SURF_11_BFLIM,
  LYTRES_SURFUI_INFO_SURF_12_BFLIM,
  LYTRES_SURFUI_INFO_SURF_13_BFLIM,
  LYTRES_SURFUI_INFO_SURF_14_BFLIM,
#endif

  static const u32  tutorialPageCapacity      = 3;
  static const u32  tutorialTextureMaxPerPage = 3;
  static const u32  NOUSE = UINT_MAX;
  static const struct TutorialMessageTable
  {
    struct PageInfo
    {
      u32   messageID;
      u32   textureIDs[tutorialTextureMaxPerPage];
    }pageInfos[tutorialPageCapacity];
  }
  tutorialMessageTable[MantainSurfAppData::TUTORIAL_MAX]  =
  {
    //    TUTORIAL_Info01,   /**<  マンタインサーフは　もくてきのしまに…  */
    {{
      {mantainsurf_01,  {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE,           {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE,           {NOUSE,   NOUSE,    NOUSE}},
    }},
    //    TUTORIAL_Info02,   /**<  マンタインサーフは　オートで進みます…  */
    {{
      {mantainsurf_02,  {LYTRES_SURFUI_INFO_SURF_00_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_03,  {LYTRES_SURFUI_INFO_SURF_01_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_04,  {NOUSE,   NOUSE,    NOUSE}},
    }},
    //      /*  （速度アップ操作練習）  */

    //    TUTORIAL_Info03,   /**<  レッスン1　終了！…                     */
    {{
      {mantainsurf_05,  {LYTRES_SURFUI_INFO_SURF_02_BFLIM,   NOUSE,    NOUSE}},
      {NOUSE,           {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE,           {NOUSE,   NOUSE,    NOUSE}},
    }},
    //    TUTORIAL_Info04,   /**<  レッスン2　障害物…                     */
    {{
      {mantainsurf_06,  {LYTRES_SURFUI_INFO_SURF_03_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_07,  {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
    }},

    //      /*  （障害物操作練習）  */

    //    TUTORIAL_Info05,   /**<  レッスン2　クリア！…                   */
    {{
      {mantainsurf_08,  {LYTRES_SURFUI_INFO_SURF_04_BFLIM,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
    }},

    //    TUTORIAL_Info06,   /**<  レッスン3　ジャンプ…                   */
    {{
      {mantainsurf_09,  {LYTRES_SURFUI_INFO_SURF_05_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_10,  {LYTRES_SURFUI_INFO_SURF_06_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_11,  {NOUSE,   NOUSE,    NOUSE}},
    }},

    //      /*  （ジャンプ操作練習）  */

    //    TUTORIAL_Info07,   /**<  レッスン3　クリア！…                   */
    {{
      {mantainsurf_12,  {LYTRES_SURFUI_INFO_SURF_07_BFLIM,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
    }},

    //    TUTORIAL_Info08,   /**<  レッスン4　エアリアル技…               */
    {{
      {mantainsurf_13,  {LYTRES_SURFUI_INFO_SURF_08_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_14,  {LYTRES_SURFUI_INFO_SURF_09_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_15,  {NOUSE,   NOUSE,    NOUSE}},
    }},

    //      /*  （エアリアル操作練習）  */

    //    TUTORIAL_Info09,   /**<  レッスン4　クリア！…                   */
    {{
      {mantainsurf_16,  {LYTRES_SURFUI_INFO_SURF_10_BFLIM,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
    }},

    //    TUTORIAL_Info10,   /**<  レッスン5　着水…                       */
    {{
      {mantainsurf_17,  {LYTRES_SURFUI_INFO_SURF_11_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_18,  {LYTRES_SURFUI_INFO_SURF_12_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_19,  {NOUSE,   NOUSE,    NOUSE}},
    }},

    //      /*  （着水操作練習）  */

    //    TUTORIAL_Info11,   /**<  レッスン5　クリア!…                    */
    {{
      {mantainsurf_20,  {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
      {NOUSE, {NOUSE,   NOUSE,    NOUSE}},
    }},

    //    TUTORIAL_Info12,   /**<  全てのレッスン　クリア!…               */
    {{
      {mantainsurf_21,  {LYTRES_SURFUI_INFO_SURF_13_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_22,  {LYTRES_SURFUI_INFO_SURF_14_BFLIM,   NOUSE,    NOUSE}},
      {mantainsurf_23,  {NOUSE,   NOUSE,    NOUSE}},
    }},
  };
  static const u32  tutorialPages = GFL_NELEMS(tutorialMessageTable);

}

// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Lower_Tutorial::SurfUI_Lower_Tutorial(
  app::util::Heap*                pHeap,
  app::util::AppRenderingManager* pRenderingManager
)
  : app::ui::UIView(pHeap)
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_State(FILELOCAL::STATE_Initial)
  , m_pResourceProvider(NULL)
  /*  handler  */
  , m_pEventHandler(NULL)
  /*  infowindow  */
  , m_pInfoWindow(NULL)
  , m_TutorialID(MantainSurfAppData::TUTORIAL_Info01)
  , m_MessagePages(0)
{
  /*  infowindow  */
  {
    gfl2::heap::HeapBase* pHeapBase = m_pHeap->GetDeviceHeap();

    m_pInfoWindow = GFL_NEW(pHeapBase) app::tool::InfoWindowBookType(pHeap);
    m_pInfoWindow->SyncCreate(pHeap, pRenderingManager, FILELOCAL::tutorialPageCapacity, NULL);

    AddSubView(m_pInfoWindow);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Lower_Tutorial::~SurfUI_Lower_Tutorial()
{
  if(m_pInfoWindow) m_pInfoWindow->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pInfoWindow);

  /*  this  */
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Lower_Tutorial::Setup(SurfUIResourceProvider* pResourceProvider)
{
  GFL_ASSERT(pResourceProvider);
  GFL_ASSERT(m_State == FILELOCAL::STATE_Initial);

  m_pResourceProvider = pResourceProvider;

  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType = app::util::G2DUtil::SETUP_LOWER;
    const app::util::G2DUtil::LytResData          lytResDataTable[] =
    {
      { pResourceProvider->IResourceProvider_OnResourceRequest(ARCID_SURF_UI, GARC_SurfUI_SurfUI_applyt_COMP), 0, app::util::G2DUtil::ATTACH_NONE },
    };
    const u32 lytResources = GFL_NELEMS(lytResDataTable);

    const app::util::G2DUtil::LytwkData   layoutWorkTable[] =
    {
      /*  waza  */
      {
        0,                                            //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_SURFUI_SURF_TUTORIAL_LOW_00_BFLYT,     //!< レイアウトデータID ( *_BFLYT ) : 
        LA_SURFUI_SURF_TUTORIAL_LOW_00___NUM,         //!< アニメ数                       : 
        lytResDataTable[0].buf,                       //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                         //!< 非表示ペインに行列計算を行うか
        displayType,                                  //!< ディスプレイ
        true                                          //!< 表示設定
      },
    };
    const u32 layoutWorks = GFL_NELEMS(layoutWorkTable);


    /*
      UIView
    */
    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( m_pHeap, false );

    Create2D(
      m_pHeap,                          /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      layoutWorks,                      /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      lytResources,                     /*  u32                               リソーステーブル要素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      layoutWorks,                      /*  u32                               ワークテーブル要素数    */
      app::ui::UIView::UNUSE_MESSAGE_DATA
    );
  }

  m_State = FILELOCAL::STATE_Idle;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Lower_Tutorial::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
/*
  if(m_State  != FILELOCAL::STATE_Initial)
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderingManager, displayNo, 0);
  }
*/
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Lower_Tutorial::Start(const MantainSurfAppData::TutorialID tutorialID)
{
  const bool  isValid = (tutorialID < MantainSurfAppData::TUTORIAL_MAX);

  GFL_ASSERT(isValid);
  if(!isValid) return;

  m_MessagePages  = 0;
  m_TutorialID    = tutorialID;
  /*  pages  */
  for(u32 index = 0; index < FILELOCAL::tutorialPageCapacity; ++index)
  {
    if(FILELOCAL::tutorialMessageTable[tutorialID].pageInfos[index].messageID == FILELOCAL::NOUSE)
    {
      break;
    }
    ++m_MessagePages;
  }

  m_pInfoWindow->ResetContents(m_MessagePages);
  m_pInfoWindow->StartOpen(this, gfl2::math::Vector3(0,0,0), app::tool::InfoWindowBookType::OPEN_TYPE_NO_ANIM);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI_Lower_Tutorial::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->SurfUI_Lower_Tutorial_OnEvent(this, eventCode);
  }
}


/*
  implement app::tool::InfoWindowBookTypeListener I/F
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void SurfUI_Lower_Tutorial::OnClose(void)
{
  Event(IEventHandler::EVENT_End);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void SurfUI_Lower_Tutorial::OnChangePage(u32 page)
{
  const bool  isValid           = (page < m_MessagePages);
  GFL_ASSERT(isValid);
  if(!isValid)  return;

  const u32   messageID         = FILELOCAL::tutorialMessageTable[m_TutorialID].pageInfos[page].messageID;
  const bool  isValidMessageID  = (messageID != FILELOCAL::NOUSE);
  GFL_ASSERT(isValidMessageID);
  if(!isValidMessageID) return;


  const gfl2::str::StrBuf*  pStr = &(m_pResourceProvider->GetMessageUtil()->GetString(SURFUI_MESSAGE_ARCDAT, messageID));
  const u32 textureIDs[FILELOCAL::tutorialTextureMaxPerPage]  =
  {
    FILELOCAL::tutorialMessageTable[m_TutorialID].pageInfos[page].textureIDs[0],
    FILELOCAL::tutorialMessageTable[m_TutorialID].pageInfos[page].textureIDs[1],
    FILELOCAL::tutorialMessageTable[m_TutorialID].pageInfos[page].textureIDs[2],
  };

  if((textureIDs[0] != FILELOCAL::NOUSE) && (textureIDs[1] != FILELOCAL::NOUSE) && (textureIDs[2] != FILELOCAL::NOUSE))
  {
    /*  画像3枚  */
  }
  else if(textureIDs[0] != FILELOCAL::NOUSE)
  {
    /*  画像1枚  */
    m_pInfoWindow->SetTextureAndTextContent(
      page,
      GetLayoutSystem()->GetResource( &GetG2DUtil()->GetLayoutResourceID( 0 ) )->GetTexture( textureIDs[0], 0 ),
      pStr,
      NULL
    );
  }
  else
  {
    /*  通常  */
    m_pInfoWindow->SetTextContent(page, pStr, NULL);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void SurfUI_Lower_Tutorial::OnCommand(u32 page)
{
}



} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

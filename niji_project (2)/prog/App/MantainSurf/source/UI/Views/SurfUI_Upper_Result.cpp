
//==============================================================================
/**
 * @file   SurfUI_Upper_Result.cpp
 * @date   2016/11/14 Mon. 16:41:06
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
#include  <AppLib/include/Util/app_util_GetPaneHelper.h>
#include  <System/include/GflUse.h>

#include  "../SurfUIResource.h"
#include  "./SurfUI_Upper_Result.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
#define FILELOCAL namespace_SurfUI_Upper_Result

namespace FILELOCAL {
  enum
  {
    STATE_Initial,
    STATE_Idle,

    /*  登場  */
    STATE_AppearResult,

    /*  自動表示  */
    STATE_NextItem,
    STATE_ScrollList,
    STATE_AppearItem,

    /*  即時表示  */
    STATE_ShowImmediately,

    /*  表示中  */
    STATE_Showing,

    STATE_WaitForExit,
    STATE_Exit,

    STATE_
  };

  static const u32  Digits_ListItem     = 5;
  static const u32  Digits_TotalResult  = 6;
  static const u32  Digits_BestScore    = 6;
  static const u32  ListItemsOnScreen   = 5;    /*  画面上に同時に表示される項目数  */
}


// =============================================================================
/**/
//==============================================================================
SurfUI_Upper_Result::SurfUI_Upper_Result(
  app::util::Heap*                pHeap,
  app::util::AppRenderingManager* pRenderingManager
)
  : app::ui::UIView(pHeap)
  , SEPlayer()
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_State(FILELOCAL::STATE_Initial)
  /*  handler  */
  , m_pEventHandler(NULL)
  , m_pPaneListView(NULL)
  , m_pResultListInfo(NULL)
  , m_IsPaneListScrolling(false)
  , m_pListDataProvider(NULL)
  , m_ListItemNum(0)
  , m_LisScrollMax(0)
  , m_NumberController_ResultTotal(FILELOCAL::Digits_TotalResult)
  , m_NumberController_BestScore(FILELOCAL::Digits_BestScore)
  , m_IsNewRecord(false)
{
  memset(m_PaneListPaneData, 0, sizeof(m_PaneListPaneData));

  for(u32 index = 0; index < GFL_NELEMS(m_pPanes); ++index)
  {
    m_pPanes[index] = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Upper_Result::~SurfUI_Upper_Result()
{
  CleanupList();
  if(m_pPaneListView) m_pPaneListView->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pPaneListView);

  /*  this  */
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::Setup(SurfUIResourceProvider* pResourceProvider)
{
  GFL_ASSERT(pResourceProvider);
  GFL_ASSERT(m_State == FILELOCAL::STATE_Initial);

  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType = app::util::G2DUtil::SETUP_UPPER;
    const app::util::G2DUtil::LytResData          lytResDataTable[] =
    {
      { pResourceProvider->IResourceProvider_OnResourceRequest(ARCID_SURF_UI, GARC_SurfUI_SurfUI_applyt_COMP), 0, app::util::G2DUtil::ATTACH_NONE },
    };
    const u32 lytResources = GFL_NELEMS(lytResDataTable);

    const app::util::G2DUtil::LytwkData   layoutWorkTable[] =
    {
      /*  result  */
      {
        0,                                            //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_SURFUI_SURF_RESULT_UPP_00_BFLYT,       //!< レイアウトデータID ( *_BFLYT ) : 
        LA_SURFUI_SURF_RESULT_UPP_00___NUM,           //!< アニメ数                       : 
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

    /*  panelistview  */
    m_pPaneListView = GFL_NEW(m_pHeap->GetDeviceHeap())  app::tool::PaneListView(m_pHeap, true);
    m_pPaneListView->SetListener(this);
    m_pPaneListView->SetInputEnabled(false);
    AddSubView(m_pPaneListView);
  }


  {
    const u8              pokeLanguageID  = System::GetLang();
    app::util::G2DUtil*   pG2DUtil        = GetG2DUtil();


    /*  #0: result  */
    {
      gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
      gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
      app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);

      /*  total  */
      m_NumberController_ResultTotal.SetG2DUtil(pG2DUtil, 0);
      m_NumberController_ResultTotal.SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_00), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_00_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_01), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_01_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_02), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_02_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_03), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_03_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_04), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_04_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupDigit(5, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_TOTAL_NUM_05), LA_SURFUI_SURF_RESULT_UPP_00__TOTAL_NUM_05_NUMBER_CHANGE);
      m_NumberController_ResultTotal.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NULL_SEPARATOR));
      m_NumberController_ResultTotal.SetTargetValue(0);
      m_NumberController_ResultTotal.EnablePaddingZero(false);
      /*  best  */
      m_NumberController_BestScore.SetG2DUtil(pG2DUtil, 0);
      m_NumberController_BestScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_00_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_01_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_02_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_03_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_04_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupDigit(5, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NUMBER_05), LA_SURFUI_SURF_RESULT_UPP_00__NUMBER_05_NUMBER_CHANGE);
      m_NumberController_BestScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_NULL_SEPARATOR_01));
      m_NumberController_BestScore.SetTargetValue(0);
      m_NumberController_BestScore.EnablePaddingZero(false);
      
      /*  digits  */
      {
        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_00);
        m_NumberController_PaneList[0].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[0].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[0].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[0].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[0].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[0].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[0].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[0].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_IN);
        m_NumberController_PaneList[0].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[0].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));

        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_01);
        m_NumberController_PaneList[1].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[1].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[1].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[1].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[1].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[1].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[1].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[1].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01_IN);
        m_NumberController_PaneList[1].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[1].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));

        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_02);
        m_NumberController_PaneList[2].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[2].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[2].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[2].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[2].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[2].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[2].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[2].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02_IN);
        m_NumberController_PaneList[2].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[2].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));

        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_03);
        m_NumberController_PaneList[3].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[3].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[3].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[3].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[3].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[3].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[3].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[3].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03_IN);
        m_NumberController_PaneList[3].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[3].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));

        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_04);
        m_NumberController_PaneList[4].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[4].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[4].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[4].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[4].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[4].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[4].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[4].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04_IN);
        m_NumberController_PaneList[4].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[4].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));

        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_05);
        m_NumberController_PaneList[5].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[5].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[5].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[5].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[5].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[5].SetupDigit(3, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_03), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_03_NUMBER_CHANGE);
        m_NumberController_PaneList[5].SetupDigit(4, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_04), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_04_NUMBER_CHANGE);
        m_NumberController_PaneList[5].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05_IN);
        m_NumberController_PaneList[5].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
        m_NumberController_PaneList[5].SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_COMMA, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_SEPARATOR_00));
/*
        helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_06);
        m_NumberController_PaneList[6].SetG2DUtil(pG2DUtil, 0);
        m_NumberController_PaneList[6].SetDigits(FILELOCAL::Digits_ListItem);
        m_NumberController_PaneList[6].SetupDigit(0, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_00), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_00_NUMBER_CHANGE);
        m_NumberController_PaneList[6].SetupDigit(1, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_01), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_01_NUMBER_CHANGE);
        m_NumberController_PaneList[6].SetupDigit(2, helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NUMBER_02), LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_02_NUMBER_CHANGE);
        m_NumberController_PaneList[6].SetAppearAnimID(LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06_IN);
        m_NumberController_PaneList[6].SetNumberGroupPane(helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_NULL_NUMBER));
*/
      }
      
      
      /*  panes  */
      m_pPanes[PANE_ScrollBar]      = helper.Clear().GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_SCROLL_BAR);
      m_pPanes[PANE_ListItemParent] = helper.Clear().GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_LIST);
      m_pPanes[PANE_Arrow_Up]       = helper.Clear().GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_ARROW_01);
      m_pPanes[PANE_Arrow_Down]     = helper.Clear().GetPane(PANENAME_SURF_RESULT_UPP_00_PANE_ARROW_00);
      
      m_pPanes[PANE_ScrollBar]->SetVisible(false);
      m_pPanes[PANE_Arrow_Up]->SetVisible(false);
      m_pPanes[PANE_Arrow_Down]->SetVisible(false);
    }


    /*  init  */
    {
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_INITIAL);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_IN_LOOP);

      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00__NUMBER_02_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01__NUMBER_02_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02__NUMBER_02_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03__NUMBER_02_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04__NUMBER_02_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05__NUMBER_02_DRUM_ROLL_01);
  /*
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_00_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_01_DRUM_ROLL_01);
      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06__NUMBER_02_DRUM_ROLL_01);
  */

      pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00__COURSE_COURSE, false);
    }
    SetInputListener(this);
    SetInputEnabled(false);
    UpdateG2DUtil();
  }


  m_State = FILELOCAL::STATE_Idle;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::SetupList(const u32 itemNum, const bool isShowBottom)
{
  CleanupList();
  if(!itemNum)
  {
    m_pPanes[PANE_ListItemParent]->SetVisible(false);
    return;
  }
  m_pPanes[PANE_ListItemParent]->SetVisible(true);

  /*  panelist  */
  app::util::Heap*          pHeap             = m_pHeap;
  gfl2::heap::HeapBase*     pHeapBase         = pHeap->GetDeviceHeap();
  app::util::G2DUtil*       pG2DUtil          = GetG2DUtil();
  gfl2::lyt::LytWk*         pLayout           = pG2DUtil->GetLayoutWork( 0 );
  app::util::GetPaneHelper  helper(pLayout, &(pG2DUtil->GetLayoutResourceID( 0 )));
  const u32                 InitialListPos    = isShowBottom ? (gfl2::math::Min(itemNum, FILELOCAL::ListItemsOnScreen-1)) : 0;            /**<  リスト表示部分でのカーソル初期位置  */
  const u32                 listScrollMax     = (FILELOCAL::ListItemsOnScreen < itemNum) ? (itemNum - FILELOCAL::ListItemsOnScreen) : 0;  /**<  スクロール値最大                    */
  const u32                 InitialListScroll = isShowBottom ? listScrollMax : 0;                                                         /**<  初期スクロール位置                  */

  app::tool::PaneList::SETUP_PARAM  paneListSetupParam    = 
  {
    pHeapBase,                      //  gfl2::heap::HeapBase * heap;  //!< ヒープ
                                    //  
    pLayout,                        //  gfl2::lyt::LytWk * pLytWk;    //!< レイアウトワーク
    m_PaneListPaneData,             //  const PANE_DATA * paneList;   //!< ペインデータ
    PaneDataNum,                    //  u32	paneListMax;              //!< ペインデータ数
                                    //  
    itemNum,                        //  u32	valMax;					//!< 実際に使用するリストの項目数（ペインの数ではない）
    FILELOCAL::ListItemsOnScreen,   //  u32	listPosMax;			//!< カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
    InitialListPos,                 //  u32	listPos;				//!< 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
    InitialListScroll,              //  f32	listScroll;			//!< 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                                    //                      //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                                    //                      //   valMax-1になるようにする設定する。
                                    //  
     54.0f,                         //  f32	defPY;					//!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                                    //                      //   この座標で指定した場所にあるペインが描画開始位置のペインになる
    648.0f,                         //  f32	defSX;					//!< ペインのXサイズ(レイアウトデディターで厳密に確認）
     26.0f,                         //  f32	defSY;					//!< ペインのYサイズ(レイアウトデディターで厳密に確認）
                                    //  
    0,                              //  s32	writeOffsetY;		//!< 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)
                                    //  
    NULL,                           //  gfl2::lyt::LytPane * listStartPane;   //!< 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )
                                    //  
    this,                           //  CallBack * cbFunc;	//!< コールバック関数
    NULL,                           //  void * cbWork;			//!< コールバック関数に渡すワーク
                                    //  
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(), //  gfl2::ui::DeviceManager * uiDevMan;		//!< UIデバイスマネージャ
    NULL                            //  app::tool::MenuCursor * menuCursor;		//!< カーソル
  };


  app::util::ScrollBar::SETUP_PARAM scrollBarSetupParam =
  {
    pHeapBase,                                                    //    gfl2::heap::HeapBase * heap;                //!< ヒープ
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,              //    app::tool::ScrollBar::ScrollDirection dir;  //!< スクロールの向き
    paneListSetupParam.valMax - paneListSetupParam.listPosMax,    //    u32 max;                                    //!< スクロール最大値
                                                                  //
                                                                  //    // @note  以下、[set1]か[set2]を設定してください
                                                                  //    // [set1]
    pLayout,                                                      //    gfl2::lyt::LytWk * lytwk;                   //!< レイアウトワーク ( NULLの場合、[set2]を参照します )
    pG2DUtil->GetLayoutResourceID( 0 ),                           //    gfl2::lyt::LytMultiResID res_id;            //!< リソースID
    PANENAME_SURF_RESULT_UPP_00_PANE_SCROLL_BAR,                  //    gfl2::lyt::LytPaneIndex parts_index;        //!< 本体の部品ペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,                 //    gfl2::lyt::LytPaneIndex mark_index;         //!< ツマミ部分のペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,                    //    gfl2::lyt::LytPaneIndex top_index;          //!< スクロール値最小位置のペインのインデックス
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,                 //    gfl2::lyt::LytPaneIndex bottom_index;       //!< スクロール値最大位置のペインのインデックス
                                                                  //    // [set2]
    NULL,                                                         //    gfl2::lyt::LytPane * parts_pane;            //!< 本体の部品ペイン
    NULL,                                                         //    gfl2::lyt::LytPane * mark_pane;             //!< ツマミ部分のペイン
    NULL,                                                         //    gfl2::lyt::LytPane * top_pane;              //!< スクロール値最小位置のペイン
    NULL                                                          //    gfl2::lyt::LytPane * bottom_pane;           //!< スクロール値最大位置のペイン
  };

  m_ListItemNum     = itemNum;
  m_LisScrollMax    = listScrollMax;

  /*  panelist  */
  {
    app::tool::PaneList::PANE_DATA*  pPaneData  = &m_PaneListPaneData[0];
    
    /* 0 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_00).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 1 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_01).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 2 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_02).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 3 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_03).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 4 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_04).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 5 */
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_05).Peek()->SetVisible(false);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;

    /* 6 */
/*
    helper.Clear().Push(PANENAME_SURF_RESULT_UPP_00_PANE_SKILL_NAME_06);
    pPaneData->base          = helper.GetPane(PANENAME_SURF_RESULT_WAZA_00_PANE_ROOT);             //    gfl2::lyt::LytPane * base;        //!< 親ペイン
    pPaneData->text          = NULL;                                                               //    gfl2::lyt::LytTextBox * text;     //!< テキストボックスペイン
    pPaneData->bound         = NULL;                                                               //    gfl2::lyt::LytBounding * bound;   //!< 境界ペイン
    pPaneData->cursor_pos    = pPaneData->base;                                                    //    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    pPaneData->holdAnime     = UINT_MAX;                                                           //    u32 holdAnime;				//!< タッチ時のアニメ
    pPaneData->releaseAnime  = UINT_MAX;                                                           //    u32 releaseAnime;			//!< リリース時のアニメ
    pPaneData->cancelAnime   = UINT_MAX;                                                           //    u32 cancelAnime;			//!< キャンセル時のアニメ
    pPaneData->selectAnime   = UINT_MAX;                                                           //    u32 selectAnime;			//!< キー選択アニメ
    ++pPaneData;
*/
  }

  m_pPaneListView->CreatePaneList(&paneListSetupParam);
  m_pPaneListView->SetInputEnabled(false);
  m_pPaneListView->GetPaneList()->SetInputEnabled(false);
  if(FILELOCAL::ListItemsOnScreen < m_ListItemNum)
  {
    m_pPanes[PANE_ScrollBar]->SetVisible(true);
    m_pPaneListView->CreateScrollBar(&scrollBarSetupParam);
  }
  else
  {
    m_pPanes[PANE_ScrollBar]->SetVisible(false);
  }
  
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, false, false );

#if 0
    { /*  タッチ範囲補正  */
      const gfl2::math::VEC3  pos     = m_PaneData[0].bound->GetTranslate();
            u32               pos_max = paneListSetupParam.listPosMax;
      if( paneListSetupParam.valMax < pos_max )
      {
        pos_max = paneListSetupParam.valMax;
      }
      
      const f32 listPX  = 0.0f;
      const f32 listPY  = paneListSetupParam.defPY;
      const f32 listSX  = paneListSetupParam.defSX;
      const f32 listSY  = paneListSetupParam.defSY;
      
      const f32 lx = listPX - listSX / 2.0f;
      const f32 rx = lx + listSX;
      const f32 uy = listPY + pos.y + listSY / 2.0f;
      const f32 dy = uy - listSY * pos_max;

      pPaneList->MakeTouchArea( lx, rx, uy, dy );
    }
#endif
  }

  UpdateArrows();

  /*  info  */
  m_pResultListInfo = GFL_NEW(pHeapBase)  ResultListInfo[m_ListItemNum];
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::CleanupList(void)
{
  GFL_SAFE_DELETE_ARRAY(m_pResultListInfo);
  if(m_pPaneListView)
  {
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
  }
  m_IsPaneListScrolling = false;
  m_ListItemNum   = 0;
  m_LisScrollMax  = 0;

  m_pPanes[PANE_ScrollBar]->SetVisible(false);
  m_pPanes[PANE_Arrow_Up]->SetVisible(false);
  m_pPanes[PANE_Arrow_Down]->SetVisible(false);
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::Update(void)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  app::tool::PaneList*  pPaneList = m_pPaneListView ? m_pPaneListView->GetPaneList() : NULL;
  
  /*  numberController  */
  {
    m_NumberController_ResultTotal.Update();
    m_NumberController_BestScore.Update();
    for(u32 index = 0; index < GFL_NELEMS(m_NumberController_PaneList); ++index)
    {
      m_NumberController_PaneList[index].Update();
    }
    /*  new record  */
    if(!m_IsNewRecord)
    {
      if(m_NumberController_BestScore.GetTargetValue() < m_NumberController_ResultTotal.GetValue())
      {
        pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_BEST_IN);
        m_IsNewRecord = true;
      }
    }
  }

  /*  paneList: 自動スクロール  */
  {
    if(m_IsPaneListScrolling)
    {
      /**
        @fix  MMCat[293]   【マンタインサーフ】リザルト画面を終了させようとした時にアサートがでる事があった
                PaneList内でf32の一致によりPaneを検索している箇所があり、Listスクロールの終了時に親座標がAnimationしていると
                float計算誤差により値が一致するPaneを見つけられず、Assertが発生することがあった
                親座標のAnimationが再生中でなく、またStateが終了待ち以前である場合のみPaneListのスクロール処理を許可するよう修正
      */
      const bool  bPaneListUpdateEnable = (pG2DUtil->IsAnimeEnd(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_OUT) && (m_State < FILELOCAL::STATE_WaitForExit));
      
      if(bPaneListUpdateEnable)
      {
        if(pPaneList) m_IsPaneListScrolling = pPaneList->MainExScroll();
      }
    }

    if(!m_IsPaneListScrolling)  ProcResultItemAppearRequest();    /*  AppearRequestの処理(ViewUpdate前に行う)  */
  }

  /*  super  */
  app::ui::UIView::Update();
  /*  ▲▼  */
  UpdateArrows();

  SetInputEnabled(false);

  switch(m_State)
  {
    case  FILELOCAL::STATE_Idle:
      break;

    case  FILELOCAL::STATE_AppearResult:
    {
      if(pG2DUtil->IsAnimeEnd(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_IN))
      {
        PlaySE(SurfSound::SurfSoundManager::SE_UI_RESULT_COUNTUP_LOOP);
        m_State = FILELOCAL::STATE_NextItem;
      }
    }
      break;

    case  FILELOCAL::STATE_NextItem:
    {
      SetInputEnabled(true);
      if(!m_IsPaneListScrolling)
      {
        if(ShowNextItem())
        {
          UpdateG2DUtil();
          m_State = FILELOCAL::STATE_ScrollList;
        }
        else
        {
          m_State = FILELOCAL::STATE_ShowImmediately;
        }
      }
    }
      break;

    /*  自動スクロール  */
    case  FILELOCAL::STATE_ScrollList:
    {
      SetInputEnabled(true);
      if(!m_IsPaneListScrolling)
      {
        m_State = FILELOCAL::STATE_AppearItem;
      }
    }
      break;


    /*    */
    case  FILELOCAL::STATE_AppearItem:
    {
      SetInputEnabled(true);
      if(IsListItemAnimEnd())
      {
        m_State = FILELOCAL::STATE_NextItem;
      }
    }
      break;

    case  FILELOCAL::STATE_ShowImmediately:
    {
      u32 totalScore  = 0;
      
      SetupList(m_ListItemNum, true);
      for(u32 index = 0; index < m_ListItemNum; ++index)  {
        App::MantainSurf::MantainSurfAppData::SkillId trickID;
        u32                                           score;
        m_pResultListInfo[index].mode = MODE_ShowImmediately;
        m_pListDataProvider->OnRequireListData(index, trickID, score);
        totalScore  += score; 
      }
      m_NumberController_ResultTotal.SetTargetValue(totalScore, true);

      if(m_pPaneListView)
      {
        if(pPaneList)
        {
          pPaneList->InitListPut();
          pPaneList->Update();
        }
      }
      UpdateG2DUtil();

      /*  se  */
      StopSE(SurfSound::SurfSoundManager::SE_UI_RESULT_COUNTUP_LOOP);
      PlaySE(SurfSound::SurfSoundManager::SE_UI_RESULT_COUNTUP_FINISH);
      m_State = FILELOCAL::STATE_Showing;
    }
      break;

    case  FILELOCAL::STATE_Showing:
    {
      SetInputEnabled(true);
    }
      break;


    case  FILELOCAL::STATE_WaitForExit:
      if(pG2DUtil->IsAnimeEnd(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_OUT))
      {
        Event(IEventHandler::EVENT_End);
        m_State = FILELOCAL::STATE_Exit;
      }
      break;

    case  FILELOCAL::STATE_Exit:
      /*  nop */
      break;


    case  FILELOCAL::STATE_Initial:
    default:
      GFL_ASSERT_MSG(false, "invalid state[%u]\n", m_State);
      break;
  }




}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
  if(m_State  != FILELOCAL::STATE_Initial)
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderingManager, displayNo, 0);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
bool  SurfUI_Upper_Result::IsDrawing(void) const
{
  return this->app::ui::UIView::IsDrawing();
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::ShowResult(void)
{
  const bool  isValidState = m_State == FILELOCAL::STATE_Idle;
  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();

  GFL_ASSERT(isValidState);
  if(!isValidState) return;
  
  pG2DUtil->StopAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_OUT);
  pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_IN);
  
  PlaySE(SurfSound::SurfSoundManager::SE_UI_RESULT_INOUT);
  
  m_State = FILELOCAL::STATE_AppearResult;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::HideResult(void)
{
  app::util::G2DUtil*         pG2DUtil = GetG2DUtil();

  pG2DUtil->StopAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_IN);
  pG2DUtil->StartAnime(0, LA_SURFUI_SURF_RESULT_UPP_00_FADE_OUT);

  PlaySE(SurfSound::SurfSoundManager::SE_UI_RESULT_INOUT);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->SurfUI_Upper_Result_OnEvent(this, eventCode);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param   work        外部から指定されたワーク
  *  @param   pane        ペインデータ
  *  @param   pane_index  項目位置のペインデータインデックス
  *  @param   pos         項目位置
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil*   pG2DUtil          = GetG2DUtil();
  u32                   animID_TrickName  = UINT_MAX;
  u32                   animID_In         = UINT_MAX;
  App::MantainSurf::MantainSurfAppData::SkillId trickID = App::MantainSurf::MantainSurfAppData::SKILL_ID_EX_01;
  u32                   score             = 0;


  /*
    準備
  */
  /*  validation  */
  {
    const bool  isValidIndex      = pos < m_ListItemNum;
    bool        isValidData       = false;

    /*  listInfo  */
    if(!m_pResultListInfo)  return;

    /*  index  */
    GFL_ASSERT(isValidIndex);
    if(!isValidIndex) return;

    /*  data  */
    if(m_pListDataProvider)   isValidData = m_pListDataProvider->OnRequireListData(pos, trickID, score);  /*  trickIDとscoreの取得  */
    if(!isValidData)          return;
  }

  /*  技名  */
  switch(pane_index)
  {
    case  0:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_IN; break;
    case  1:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01_IN; break;
    case  2:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02_IN; break;
    case  3:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03_IN; break;
    case  4:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04_IN; break;
    case  5:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05_IN; break;
//    case  6:  animID_TrickName = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06_WAZA_CHANGE; animID_In = LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06_IN; break;
    default:  GFL_ASSERT(false);  break;    /*  illegal index  */
  }


  /*
    表示処理
  */
  m_PaneListPaneData[pane_index].base->GetParent()->SetVisible(true);
  switch(m_pResultListInfo[pos].mode)
  {
    /*    */
    case  MODE_Invisible:
      m_PaneListPaneData[pane_index].base->GetParent()->SetVisible(false);
      break;

    case  MODE_AppearRequest:
    {
      ResultListInfo& rInfo = m_pResultListInfo[pos];

      m_PaneListPaneData[pane_index].base->GetParent()->SetVisible(false);

      /*  request発行  */
      rInfo.paneDataIndex     = pane_index;
      rInfo.animID_TrickName  = animID_TrickName;
      rInfo.animID_In         = animID_In;
      rInfo.pNumberController = &m_NumberController_PaneList[pane_index];
      rInfo.trickID           = trickID;
      rInfo.score             = score;
    }
      break;

    /*    */
    case  MODE_Appear:
    {
      /*  nop  */
    }
      break;

    /*    */
    case  MODE_ShowImmediately:
    {
      if(animID_TrickName != UINT_MAX)
      {
        pG2DUtil->StartAnimeFrame(0, animID_TrickName, static_cast<f32>(trickID), false);     /**<  技名表示  */
        pG2DUtil->StartAnime(     0, animID_In);                                              /**<  登場演出  */
        pG2DUtil->SetAnimeFrame(  0, animID_In, pG2DUtil->GetAnimeMaxFrame(0, animID_In));
        /**  スコア    */
        m_NumberController_PaneList[pane_index].ClearAfterAnimRequest();
        m_NumberController_PaneList[pane_index].SetTargetValue(score, true);
        m_NumberController_PaneList[pane_index].Update();
      }
    }
      break;
    
    /*    */
    default:
      GFL_ASSERT_MSG(false, "invalid listinfo mode: %d\n", m_pResultListInfo[pos].mode);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    ResultList項目の状態を更新
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::ProcResultItemAppearRequest(void)
{
  app::util::G2DUtil*   pG2DUtil          = GetG2DUtil();

  for(u32 index = 0; index < m_ListItemNum; ++index)
  {
    ResultListInfo& rInfo = m_pResultListInfo[index];

    if(rInfo.mode == MODE_AppearRequest)
    {
      const bool  isValidRequest = rInfo.IsValidRequest();
      
      GFL_ASSERT(isValidRequest);
      if(isValidRequest)
      {
        m_PaneListPaneData[rInfo.paneDataIndex].base->GetParent()->SetVisible(true);
        pG2DUtil->StartAnimeFrame(0, rInfo.animID_TrickName, static_cast<f32>(rInfo.trickID), false); /**<  技名表示  */
        pG2DUtil->StartAnime(     0, rInfo.animID_In);                                                /**<  登場演出  */
        PlaySE(SurfSound::SurfSoundManager::SE_UI_RESULTITEM_IN);
        rInfo.pNumberController->SetTargetValueAfterAnim(rInfo.score);                                      /**<  スコア    */
        /*  total反映  */
        {
          m_NumberController_ResultTotal.SetStartValueToCurrentValue();
          m_NumberController_ResultTotal.SetTargetValue(m_NumberController_ResultTotal.GetTargetValue() + rInfo.score);
        }
        rInfo.mode = MODE_ShowImmediately;
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::ScrollUp(void)
{
  if(IsListEmpty()) return;

  if(!m_IsPaneListScrolling)
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if(pPaneList) m_IsPaneListScrolling = m_pPaneListView->GetPaneList()->InitExScroll(-1);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::ScrollDown(void)
{
  if(IsListEmpty()) return;

  if(!m_IsPaneListScrolling)
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if(pPaneList) m_IsPaneListScrolling = m_pPaneListView->GetPaneList()->InitExScroll(1);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    全てのPaneList項目のAnimationが終了しているか
  */
//------------------------------------------------------------------
bool  SurfUI_Upper_Result::IsListItemAnimEnd(void) const
{
  const u32 targetAnimIDs[] =
  {
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_00_IN,
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_01_IN,
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_02_IN,
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_03_IN,
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_04_IN,
    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_05_IN,
//    LA_SURFUI_SURF_RESULT_UPP_00__SKILL_NAME_06_IN,
  };
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  bool                  isAllEnd  = true;

  for(u32 index = 0; index < GFL_NELEMS(targetAnimIDs); ++index)
  {
    if(
      (!pG2DUtil->IsAnimeEnd(0, targetAnimIDs[index]))
      || ((m_NumberController_PaneList[index].GetValue() != m_NumberController_PaneList[index].GetTargetValue()) || m_NumberController_PaneList[index].IsAfterAnimRequested()))
    {
      isAllEnd  = false;
      break;
    }
  }

  return isAllEnd;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  SurfUI_Upper_Result::ShowNextItem(void)
{
  bool  isExistNextItem = false;

  for(u32 index = 0; index < m_ListItemNum; ++index)
  {
    ResultListInfo& rInfo = m_pResultListInfo[index];

    if(rInfo.mode  == MODE_Invisible)
    {
      rInfo.mode = MODE_AppearRequest;
      if(index < (FILELOCAL::ListItemsOnScreen+1))
      {
        /*  スクロール未満は直接WriteCBをコールする  */
        const u32 paneDataIndex = index;
        CallBack_ListWrite(NULL, &m_PaneListPaneData[paneDataIndex], paneDataIndex, index);
      }

      if(FILELOCAL::ListItemsOnScreen <= index)
      {
        ScrollDown();
      }

      isExistNextItem  = true;
      break;
    }
  }

  return isExistNextItem;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::SetBestScore(const u32 score)
{
  m_NumberController_BestScore.SetTargetValue(score, true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  SurfUI_Upper_Result::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;


  if( IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }

  switch(m_State)
  {
    case  FILELOCAL::STATE_NextItem:
    case  FILELOCAL::STATE_AppearItem:
    {
      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_State = FILELOCAL::STATE_ShowImmediately;
        return app::ui::UIInputListener::DISABLE_ACCESS;
      }
    }
      break;


    case  FILELOCAL::STATE_Showing:
    {
      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();

        HideResult();
        if(pPaneList)  pPaneList->SetInputEnabled(false);
        m_State = FILELOCAL::STATE_WaitForExit;
        return app::ui::UIInputListener::DISABLE_ACCESS;
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_UP))
      {
        ScrollUp();
      }
      else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN))
      {
        ScrollDown();
      }
    }
      break;

    case  FILELOCAL::STATE_Initial:
    case  FILELOCAL::STATE_Idle:
    case  FILELOCAL::STATE_AppearResult:
    case  FILELOCAL::STATE_ShowImmediately:
    case  FILELOCAL::STATE_WaitForExit:
    default:
      break;
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::UpdateArrows(void)
{
  if(m_pPaneListView)
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();

    if(pPaneList)
    {
      u32 pos       = 0;
      f32 scrollPos = .0f;;
      
      pPaneList->GetCursorData( &pos, &scrollPos );
      m_pPanes[PANE_Arrow_Up]->SetVisible(0.0f < scrollPos);
      m_pPanes[PANE_Arrow_Down]->SetVisible(scrollPos < m_LisScrollMax);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Result::SetCourseID(const App::MantainSurf::MantainSurfAppData::CourseID courseID)
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  f32 animFrame = -1.0f;

  if(pG2DUtil)
  {
    switch(courseID)
    {
      case  MantainSurfAppData::COURSE_NORMAL:          animFrame = 0.0f; break;
      case  MantainSurfAppData::COURSE_SUPER:           animFrame = 1.0f; break;
      case  MantainSurfAppData::COURSE_HYPER:           animFrame = 2.0f; break;
      case  MantainSurfAppData::COURSE_MASTER:          animFrame = 3.0f; break;
    }

    if(.0f <= animFrame)
    {
      pG2DUtil->SetAnimeFrame(0, LA_SURFUI_SURF_RESULT_UPP_00__COURSE_COURSE, animFrame);
      pG2DUtil->Update2D();
    }
  }
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

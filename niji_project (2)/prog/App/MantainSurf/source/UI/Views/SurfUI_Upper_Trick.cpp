//==============================================================================
/**
 * @file   SurfUI_Upper_Trick.cpp
 * @date   2016/11/15 Tue. 11:39:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================

#include  <AppLib/include/Util/app_util_GetPaneHelper.h>


#include  "../SurfUIResource.h"
#include  "./SurfUI_Upper_Trick.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
#define FILELOCAL namespace_SurfUI_Upper_Trick

namespace FILELOCAL {
  enum
  {
    STATE_Initial,
    STATE_Running,

    STATE_
  };

  const u32   ScoreDigits   = 5;
}


// =============================================================================
/**/
//==============================================================================
SurfUI_Upper_Trick::SurfUI_Upper_Trick(
  app::util::Heap*                pHeap,
  app::util::AppRenderingManager* pRenderingManager
)
  : app::ui::UIView(pHeap)
  , SurfUI_Upper_TotalScore::Notifier()
  , SEPlayer()
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_State(FILELOCAL::STATE_Initial)
  , m_TotalScoreView(pHeap, pRenderingManager)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pPanes); ++index)
  {
    m_pPanes[index] = NULL;
  }

  SetNotifyTarget(&m_TotalScoreView);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Upper_Trick::~SurfUI_Upper_Trick()
{
  m_TotalScoreView.RemoveFromSuperView();
  /*  this  */
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Trick::Setup(SurfUIResourceProvider* pResourceProvider)
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
      /*  waza  */
      {
        0,                                            //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_SURFUI_SURF_WAZA_UPP_00_BFLYT,         //!< レイアウトデータID ( *_BFLYT ) : 
        LA_SURFUI_SURF_WAZA_UPP_00___NUM,             //!< アニメ数                       : 
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


  {
    app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();

    /*  #0: waza  */
    {
      gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
      gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
      app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);

      m_pPanes[NULL_TrickNew]             = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICKNEW);
      m_pPanes[NULL_Trick00]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_00);
      m_pPanes[NULL_Trick01]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_01);
      m_pPanes[NULL_Trick02]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_02);
      m_pPanes[NULL_Trick03]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_03);
      m_pPanes[NULL_Trick04]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_04);
      m_pPanes[NULL_Trick05]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_05);
      m_pPanes[NULL_Trick06]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_06);
      m_pPanes[NULL_Trick07]              = helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICK_07);
      m_pPanes[NULL_TrickName00_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_00).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName01_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_01).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName02_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_02).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName03_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_03).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName04_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_04).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName05_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_05).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName06_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_06).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName07_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_07).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickName08_SETiming] = helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_08).GetPane(PANENAME_SURF_WAZA_00_PANE_NULL_SETIMING);
      m_pPanes[NULL_TrickFree]        = helper.Clear().GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_NULL_TRICKFREE);

      helper.Clear();
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_00)->SetUserData("0");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_01)->SetUserData("1");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_02)->SetUserData("2");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_03)->SetUserData("3");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_04)->SetUserData("4");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_05)->SetUserData("5");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_06)->SetUserData("6");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_07)->SetUserData("7");
      helper.GetPane(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_08)->SetUserData("8");

      /*  scoreController  */
      {
        const u8  pokeLanguageID  = System::GetLang();

        for(u32 index = 0; index < GFL_NELEMS(m_ScoreControllers); ++index)
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[index];

          rScore.SetG2DUtil(pG2DUtil, 0);
          rScore.SetDigits(FILELOCAL::ScoreDigits);
        }

        /*  #0  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[0];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_00);;
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00_PLUS_ANIME);
        }
        /*  #1  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[1];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_01);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01_PLUS_ANIME);
        }
        /*  #2  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[2];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_02);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02_PLUS_ANIME);
        }
        /*  #3  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[3];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_03);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03_PLUS_ANIME);
        }
        /*  #4  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[4];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_04);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04_PLUS_ANIME);
        }
        /*  #5  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[5];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_05);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05_PLUS_ANIME);
        }
        /*  #6  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[6];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_06);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06_PLUS_ANIME);
        }
        /*  #7  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[7];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_07);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07_PLUS_ANIME);
        }
        /*  #8  */
        {
          NumberController_TrickScore&  rScore  = m_ScoreControllers[8];

          helper.Clear().Push(PANENAME_SURF_WAZA_UPP_00_PANE_WAZA_FONT_08);
          rScore.SetupDigit(0, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_00), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08__PARTS_00_NUMBER_CHANGE);
          rScore.SetupDigit(1, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_01), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08__PARTS_01_NUMBER_CHANGE);
          rScore.SetupDigit(2, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_02), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08__PARTS_02_NUMBER_CHANGE);
          rScore.SetupDigit(3, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_03), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08__PARTS_03_NUMBER_CHANGE);
          rScore.SetupDigit(4, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_PARTS_04), LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08__PARTS_04_NUMBER_CHANGE);
          rScore.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08_COMMA, helper.GetPane(PANENAME_SURF_WAZA_00_PANE_COMMA_00));
          rScore.SetupAnimID_PlusSign(LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08_PLUS_ANIME);
        }
      }
    }


  }


  /*  totalscore  */
  {
    m_TotalScoreView.Setup(pResourceProvider);
    AddSubView(&m_TotalScoreView);
  }


  m_State = FILELOCAL::STATE_Running;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Trick::Update(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_ScoreControllers); ++index)
  {
    m_ScoreControllers[index].Update();
  }


  /*  super  */
  app::ui::UIView::Update();

  /*  SE  */
  UpdateSE();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Trick::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
  if(m_State  == FILELOCAL::STATE_Running)
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
bool  SurfUI_Upper_Trick::IsDrawing(void) const
{
  return this->app::ui::UIView::IsDrawing();
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
void  SurfUI_Upper_Trick::ShowTrick(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 score)
{
  const u32 trickPaneIDs[] =
  {
    NULL_Trick07,
    NULL_Trick06,
    NULL_Trick05,
    NULL_Trick04,
    NULL_Trick03,
    NULL_Trick02,
    NULL_Trick01,
    NULL_Trick00,
  };
  const u32           trickPaneIDNum        = GFL_NELEMS(trickPaneIDs);
  u32                 trickPaneIDListIndex  = 0;
  gfl2::lyt::LytPane* pNewTrickPane = NULL;

  /*  find free pane  */
  {
    /*  freelistから探す  */
    if(!m_pPanes[NULL_TrickFree]->GetChildList().empty())
    {
      pNewTrickPane = &(m_pPanes[NULL_TrickFree]->GetChildList().front());
      m_pPanes[NULL_TrickFree]->RemoveChild(pNewTrickPane);
    }

    /*  使用中リストから探す*/
    if(!pNewTrickPane)
    {
      for(trickPaneIDListIndex = 0; trickPaneIDListIndex < trickPaneIDNum; ++trickPaneIDListIndex)
      {
        const u32 paneID = trickPaneIDs[trickPaneIDListIndex];
        if(!m_pPanes[paneID]->GetChildList().empty())
        {
          pNewTrickPane = &(m_pPanes[paneID]->GetChildList().front());
          m_pPanes[paneID]->RemoveChild(pNewTrickPane);
          break;
        }
      }
    }
  }

  GFL_ASSERT(pNewTrickPane);    /*  開発用Assert */
  if(!pNewTrickPane)  return;   /*  見つからなかった場合は実装エラー  */

  /*  リスト追加 */
  {
    /*  前詰め */
    for(; trickPaneIDListIndex < trickPaneIDNum; ++trickPaneIDListIndex)
    {
      if(!trickPaneIDListIndex) continue;

      ChildPaneTreeMove(static_cast<PaneListID>(trickPaneIDs[trickPaneIDListIndex]), static_cast<PaneListID>(trickPaneIDs[trickPaneIDListIndex - 1]));
    }
    ChildPaneTreeMove(NULL_TrickNew, NULL_Trick00);

    /*  新Paneを追加  */
    m_pPanes[NULL_TrickNew]->AppendChild(pNewTrickPane);
  }

  /*  animation */
  {
    app::util::G2DUtil* pG2DUtil = GetG2DUtil();
    u32                 animID_TrickIn    = UINT_MAX;
    u32                 animID_TrickName  = UINT_MAX;
    const char* const   userData_IsHigh   = MantainSurfAppData::IsHighScoreSkill(trickID) ? "1" : "0";

    pG2DUtil->StopAnime(0, LA_SURFUI_SURF_WAZA_UPP_00_TAG_ROLLUP);
    pG2DUtil->StartAnime(0, LA_SURFUI_SURF_WAZA_UPP_00_TAG_ROLLUP);

    switch(pNewTrickPane->GetUserData()[0])
    {
      case  '0':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_00_WAZA_CHANGE;  m_ScoreControllers[0].SetTargetValue(score, true);  m_pPanes[NULL_TrickName00_SETiming]->SetUserData(userData_IsHigh); break;
      case  '1':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_01_WAZA_CHANGE;  m_ScoreControllers[1].SetTargetValue(score, true);  m_pPanes[NULL_TrickName01_SETiming]->SetUserData(userData_IsHigh); break;
      case  '2':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_02_WAZA_CHANGE;  m_ScoreControllers[2].SetTargetValue(score, true);  m_pPanes[NULL_TrickName02_SETiming]->SetUserData(userData_IsHigh); break;
      case  '3':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_03_WAZA_CHANGE;  m_ScoreControllers[3].SetTargetValue(score, true);  m_pPanes[NULL_TrickName03_SETiming]->SetUserData(userData_IsHigh); break;
      case  '4':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_04_WAZA_CHANGE;  m_ScoreControllers[4].SetTargetValue(score, true);  m_pPanes[NULL_TrickName04_SETiming]->SetUserData(userData_IsHigh); break;
      case  '5':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_05_WAZA_CHANGE;  m_ScoreControllers[5].SetTargetValue(score, true);  m_pPanes[NULL_TrickName05_SETiming]->SetUserData(userData_IsHigh); break;
      case  '6':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_06_WAZA_CHANGE;  m_ScoreControllers[6].SetTargetValue(score, true);  m_pPanes[NULL_TrickName06_SETiming]->SetUserData(userData_IsHigh); break;
      case  '7':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_07_WAZA_CHANGE;  m_ScoreControllers[7].SetTargetValue(score, true);  m_pPanes[NULL_TrickName07_SETiming]->SetUserData(userData_IsHigh); break;
      case  '8':  animID_TrickIn = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08_TAG_INOUT;  animID_TrickName = LA_SURFUI_SURF_WAZA_UPP_00__WAZA_FONT_08_WAZA_CHANGE;  m_ScoreControllers[8].SetTargetValue(score, true);  m_pPanes[NULL_TrickName08_SETiming]->SetUserData(userData_IsHigh); break;
    }

    if(animID_TrickIn != UINT_MAX)    pG2DUtil->StartAnime(     0, animID_TrickIn);
    if(animID_TrickName != UINT_MAX)  pG2DUtil->StartAnimeFrame(0, animID_TrickName, static_cast<f32>(trickID), false);
  }
  
  /*  score  */
  m_TotalScoreView.AddScore(score);

  UpdateG2DUtil();
  UpdateSE();
}


//------------------------------------------------------------------
/**
*  @brief   子paneのツリー間移動
*  @return  移動したPane
*/
//------------------------------------------------------------------
gfl2::lyt::LytPane*  SurfUI_Upper_Trick::ChildPaneTreeMove(const PaneListID from, const PaneListID to)
{
  gfl2::lyt::LytPane* pTargetPane = NULL;
  
  if(!m_pPanes[from]->GetChildList().empty())
  {
    pTargetPane = &(m_pPanes[from]->GetChildList().front());

    m_pPanes[from]->RemoveChild(pTargetPane);
    m_pPanes[to]->AppendChild(pTargetPane);
  }

  return pTargetPane;
}


//------------------------------------------------------------------
/**
  *  @brief    タイミング系SE処理
  *  @note     animationが進行するたびにコールする
  */
//------------------------------------------------------------------
void  SurfUI_Upper_Trick::UpdateSE(void)
{
  const u32 paneIDTable[] =
  {
    NULL_TrickName00_SETiming,
    NULL_TrickName01_SETiming,
    NULL_TrickName02_SETiming,
    NULL_TrickName03_SETiming,
    NULL_TrickName04_SETiming,
    NULL_TrickName05_SETiming,
    NULL_TrickName06_SETiming,
    NULL_TrickName07_SETiming,
    NULL_TrickName08_SETiming,
  };

  for(u32 index = 0; index < GFL_NELEMS(paneIDTable); ++index)
  {
    if(m_pPanes[paneIDTable[index]]->IsVisible())
    {
      /*  SE  */
      const bool                                  isHigh  = (m_pPanes[paneIDTable[index]]->GetUserData()[0] == '1');
      const SurfSound::SurfSoundManager::SEIndex  seIndex = isHigh ? SurfSound::SurfSoundManager::SE_UI_SCORE_HIGH : SurfSound::SurfSoundManager::SE_UI_SCORE_LOW;

      PlaySE(seIndex);
      break;  /*  一度にひとつのみ有効とする  */
    }
  }
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

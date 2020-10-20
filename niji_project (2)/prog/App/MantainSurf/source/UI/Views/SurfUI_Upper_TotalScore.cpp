//==============================================================================
/**
 * @file   SurfUI_Upper_TotalScore.cpp
 * @date   2017/01/23 Mon. 15:13:07
 * @author muto_yu
 * @brief  今回のジャンプの合計スコア
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
#include  "./SurfUI_Upper_TotalScore.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_SurfUI_Upper_TotalScore
namespace FILELOCAL {
  enum
  {
    STATE_Initial,
    STATE_Idle,

    STATE_Showing,
    STATE_Disappear,

    STATE_Exit,

    STATE_
  };

  static const u32  Digits              = 6;

  /*  anim  */
  static const u32  Anim_In   = LA_SURFUI_SURF_SCORE_UPP_00_IN;
  static const u32  Anim_Out  = LA_SURFUI_SURF_SCORE_UPP_00_OUT;
}


// =============================================================================
/**/
//==============================================================================
SurfUI_Upper_TotalScore::SurfUI_Upper_TotalScore(
  app::util::Heap*                pHeap,
  app::util::AppRenderingManager* pRenderingManager
)
  : app::ui::UIView(pHeap)
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_State(FILELOCAL::STATE_Initial)
  , m_TotalScore(0)
  , m_NumberController(FILELOCAL::Digits)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Upper_TotalScore::~SurfUI_Upper_TotalScore()
{
  /*  this  */
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_TotalScore::Setup(SurfUIResourceProvider* pResourceProvider)
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
        LYTRES_SURFUI_SURF_SCORE_UPP_00_BFLYT,        //!< レイアウトデータID ( *_BFLYT ) : 
        LA_SURFUI_SURF_SCORE_UPP_00___NUM,            //!< アニメ数                       : 
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
    const u8              pokeLanguageID  = System::GetLang();
    app::util::G2DUtil*   pG2DUtil        = GetG2DUtil();


    {
      gfl2::lyt::LytWk*           pLayout         = pG2DUtil->GetLayoutWork( 0 );
      gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID( 0 );
      app::util::GetPaneHelper    helper(pLayout, &rLytMultiResID);

      /*  total  */
      m_NumberController.SetG2DUtil(pG2DUtil, 0);
      m_NumberController.SetupDigit(0, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_00), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_00_NUMBER_CHANGE);
      m_NumberController.SetupDigit(1, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_01), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_01_NUMBER_CHANGE);
      m_NumberController.SetupDigit(2, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_02), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_02_NUMBER_CHANGE);
      m_NumberController.SetupDigit(3, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_03), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_03_NUMBER_CHANGE);
      m_NumberController.SetupDigit(4, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_04), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_04_NUMBER_CHANGE);
      m_NumberController.SetupDigit(5, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_TOTAL_NUM_05), LA_SURFUI_SURF_SCORE_UPP_00__TOTAL_NUM_05_NUMBER_CHANGE);
      m_NumberController.SetupSeparator(pokeLanguageID, LA_SURFUI_SURF_SCORE_UPP_00_COMMA, helper.GetPane(PANENAME_SURF_SCORE_UPP_00_PANE_COMMA_00));
      m_NumberController.SetTargetValue(0);
      m_NumberController.EnablePaddingZero(false);
    }


    UpdateG2DUtil();
  }


  m_State = FILELOCAL::STATE_Idle;
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_TotalScore::Update(void)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  
  /*  numberController  */
  {
    m_NumberController.Update();
  }

  /*  super  */
  app::ui::UIView::Update();

  switch(m_State)
  {
    case  FILELOCAL::STATE_Idle:
    case  FILELOCAL::STATE_Showing:
      /*  nop */
      break;

    case  FILELOCAL::STATE_Disappear:
      if(pG2DUtil->IsAnimeEnd(0, FILELOCAL::Anim_In))
      {
        pG2DUtil->StartAnime(0, FILELOCAL::Anim_Out);
        m_State = FILELOCAL::STATE_Idle;
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
void  SurfUI_Upper_TotalScore::Draw(gfl2::gfx::CtrDisplayNo displayNo)
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
void  SurfUI_Upper_TotalScore::AddScore(const u32 score)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();

  switch(m_State)
  {
    case  FILELOCAL::STATE_Idle:
      /*  初回add  */
      m_TotalScore  = 0;
      m_NumberController.SetTargetValue(m_TotalScore, true);
      
      pG2DUtil->StopAnime(0, FILELOCAL::Anim_Out);
      pG2DUtil->StartAnime(0, FILELOCAL::Anim_In);
      m_State = FILELOCAL::STATE_Showing;
      break;

    case  FILELOCAL::STATE_Showing:
      break;
  }

  m_TotalScore  += score;
  m_NumberController.SetStartValueToCurrentValue();
  m_NumberController.SetTargetValue(m_TotalScore);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Upper_TotalScore::Notify(const NotifyEvent notifyEvent)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();

  switch(notifyEvent)
  {
    case  NOTIFY_JumpStart:   /**<  ジャンプ開始  */
      break;

    case  NOTIFY_JumpEnd:     /**<  着水          */
      if(m_State == FILELOCAL::STATE_Showing)
      {
        m_State = FILELOCAL::STATE_Disappear;
      }
      break;

    default:
      GFL_ASSERT_MSG(false, "illegal notify:[%d]\n", notifyEvent);
      break;
  }
}






} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

//==============================================================================
/**
 * @file   SurfUI_Lower_TrickList.cpp
 * @date   2017/01/23 Mon. 17:08:37
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
#include  "./SurfUI_Lower_TrickList.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_SurfUI_Lower_TrickList
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================
SurfUI_Lower_TrickList::SurfUI_Lower_TrickList(
  app::util::Heap*                pHeap,
  app::util::AppRenderingManager* pRenderingManager
)
  : app::ui::UIView(pHeap)
  , m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI_Lower_TrickList::~SurfUI_Lower_TrickList()
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
void  SurfUI_Lower_TrickList::Setup(SurfUIResourceProvider* pResourceProvider)
{
  GFL_ASSERT(pResourceProvider);

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
        LYTRES_SURFUI_SURF_BG_LOW_01_BFLYT,           //!< レイアウトデータID ( *_BFLYT ) : 
        LA_SURFUI_SURF_BG_LOW_01___NUM,               //!< アニメ数                       : 
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
  
  /*  anim  */
  {
    GetG2DUtil()->StartAnime(0, LA_SURFUI_SURF_BG_LOW_01_WZA_LIST, false);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
void  SurfUI_Lower_TrickList::Update(void)
{
  /*  super  */
  app::ui::UIView::Update();
}
#endif

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Lower_TrickList::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  /*    */
  if(GetG2DUtil())
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
bool  SurfUI_Lower_TrickList::IsDrawing(void) const
{
  return this->app::ui::UIView::IsDrawing();
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI_Lower_TrickList::SetMode(const App::MantainSurf::MantainSurfAppData::SkillListMode mode)
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();
  f32 animFrame = -1.0f;

  if(pG2DUtil)
  {
    switch(mode)
    {
      case  MantainSurfAppData::SKILLLIST_MODE_NOTHING: animFrame = 4.0f; break;  /**<  初期技も含めウインドウ以外全て非表示  */
      case  MantainSurfAppData::SKILLLIST_MODE_0:       animFrame = 0.0f; break;  /**<  技未解放  */
      case  MantainSurfAppData::SKILLLIST_MODE_1:       animFrame = 1.0f; break;  /**<  技1解放   */
      case  MantainSurfAppData::SKILLLIST_MODE_2:       animFrame = 2.0f; break;  /**<  技2解放   */
      case  MantainSurfAppData::SKILLLIST_MODE_3:       animFrame = 3.0f; break;  /**<  技3解放   */
    }

    if(.0f <= animFrame)
    {
      pG2DUtil->SetAnimeFrame(0, LA_SURFUI_SURF_BG_LOW_01_WZA_LIST, animFrame);
      pG2DUtil->Update2D();
    }
  }
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */

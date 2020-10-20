//==============================================================================
/**
 * @file   DiveUI_Lower_BG.cpp
 * @date   2016/11/29 Tue. 18:32:28
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

#include  "./DiveUI_Lower_BG.h"
#include  "../DiveUIResource.h"


#if 0
namespace App  {
namespace LunaSolDive  {
namespace UI  {
#else
namespace Test  {
namespace MutoTest  {
#endif
// =============================================================================
/**/
//==============================================================================
#define FILELOCAL namespace_DiveUI_Lower_BG

namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================
DiveUI_Lower_BG::DiveUI_Lower_BG(
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
DiveUI_Lower_BG::~DiveUI_Lower_BG()
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
void  DiveUI_Lower_BG::Setup(app::tool::IResourceProvider* pResourceProvider)
{
  GFL_ASSERT(pResourceProvider);

  /*  setupLayoutSystem  */
  {
    const app::util::G2DUtil::SETUP_DISPLAY_TYPE  displayType = app::util::G2DUtil::SETUP_LOWER;
    const app::util::G2DUtil::LytResData          lytResDataTable[] =
    {
      { pResourceProvider->IResourceProvider_OnResourceRequest(ARCID_DIVE_UI, GARC_DiveUI_DiveUI_applyt_COMP), 0, app::util::G2DUtil::ATTACH_NONE },
    };
    const u32 lytResources = GFL_NELEMS(lytResDataTable);

    const app::util::G2DUtil::LytwkData   layoutWorkTable[] =
    {
      /*  waza  */
      {
        0,                                            //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_DIVEUI_DIVE_BG_LOW_00_BFLYT,           //!< レイアウトデータID ( *_BFLYT ) : 
        LA_DIVEUI_DIVE_BG_LOW_00___NUM,               //!< アニメ数                       : 
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

    pG2DUtil->StartAnime(0, LA_DIVEUI_DIVE_BG_LOW_00_BG00);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if 0
void  DiveUI_Lower_BG::Update(void)
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
void  DiveUI_Lower_BG::Draw(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  DiveUI_Lower_BG::IsDrawing(void) const
{
  return this->app::ui::UIView::IsDrawing();
}
#endif


#if 0
} /*  namespace UI  */
} /*  namespace LunaSolDive  */
} /*  namespace App  */
#else
} /*  namespace Test  */
} /*  namespace MutoTest  */
#endif

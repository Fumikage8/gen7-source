//==============================================================================
/**
 * @file   DecoView.cpp
 * @date   2017/02/15 Wed. 13:04:36
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <arc_def_index/arc_def.h>
#include  <arc_index/FinderStudioDecoView.gaix>
#include  <niji_conv_header/app/studio_decoview/FinderStudioDecoView.h>
#include  <niji_conv_header/app/studio_decoview/FinderStudioDecoView_anim_list.h>
#include  <niji_conv_header/app/studio_decoview/FinderStudioDecoView_pane.h>

#include  <AppLib/include/Util/app_util_GetPaneHelper.h>

#include  "./DecoView.h"


namespace App {
namespace FinderStudio {
namespace Viewer {
namespace Deco {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_DecoView
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoView::DecoView(
  app::util::Heap*                pUtilHeap,
  app::util::AppRenderingManager* pRenderingManager,
  void*                           pResource
)
  : app::ui::UIView(pUtilHeap)
  , m_pUtilHeap(pUtilHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_pDecoParentPane_Frame(NULL)
  , m_pDecoParentPane_Stamps(NULL)
{
  const bool isValidParam = ((!!pUtilHeap) && (!!pRenderingManager));
  GFL_ASSERT(isValidParam); /*  invalid param  */
  if(!isValidParam) return;
  

  gfl2::heap::HeapBase* pHeapBase = m_pUtilHeap->GetDeviceHeap();

  
  {
    app::util::G2DUtil::LytResData    layoutResourceData;
    app::util::G2DUtil::LytwkData     layoutWorkData;


    layoutResourceData.buf              = pResource;
    layoutResourceData.addTexNum        = 0;
    layoutResourceData.addRegisterFlag  = app::util::G2DUtil::ATTACH_NONE;
    
    layoutWorkData.res_index            = 0;
    layoutWorkData.lytres_id            = LYTRES_FINDERSTUDIODECOVIEW_DECOVIEW_UPPER_BFLYT;
    layoutWorkData.anm_max              = LA_FINDERSTUDIODECOVIEW_DECOVIEW_UPPER___NUM;
    layoutWorkData.buf                  = pResource;
    layoutWorkData.calculateMtxFlag     = true;
    layoutWorkData.setupDisp            = app::util::G2DUtil::SETUP_UPPER;
    layoutWorkData.enable               = true;

    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pUtilHeap, false );
    Create2D(
      m_pUtilHeap,
      NULL,
      1,
      &layoutResourceData, 1,
      &layoutWorkData, 1,
      UNUSE_MESSAGE_DATA
    );
  }

  /*  panes */
  {
    app::util::GetPaneHelper  helper(GetG2DUtil());
    gfl2::lyt::LytPane*       pParentPane = helper.GetPane(PANENAME_DECOVIEW_UPPER_PANE_ALL);
    
    /*  描画順制御のためのPane  */
    {
      const gfl2::math::VEC3  panePos(0, 0, 0);
      m_pDecoParentPane_Frame   = GFL_NEW(pHeapBase) gfl2::lyt::LytPane();
      m_pDecoParentPane_Stamps  = GFL_NEW(pHeapBase) gfl2::lyt::LytPane[stamps];
      
      m_pDecoParentPane_Frame->SetUserAllocated();
      m_pDecoParentPane_Frame->SetTranslate(panePos);
      pParentPane->AppendChild( m_pDecoParentPane_Frame );   /*  Frameが一番下  */
      for( u32 index = 0; index < stamps; ++index )
      {
        m_pDecoParentPane_Stamps[index].SetUserAllocated();
        m_pDecoParentPane_Stamps[index].SetTranslate(panePos);
        pParentPane->AppendChild( &( m_pDecoParentPane_Stamps[index] ) );
      }
    }

    /*  decoItems  */
    m_DecoItem_Frame.Setup(m_pUtilHeap, m_pDecoParentPane_Frame);
    for(u32 index = 0; index < stamps; ++index)
    {
      m_DecoItem_Stamps[index].Setup(m_pUtilHeap, &(m_pDecoParentPane_Stamps[index]));
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoView::~DecoView()
{
  GFL_ASSERT(IsStable());

  Delete2D();
  DeleteGraphicsSystem();

  GFL_SAFE_DELETE(m_pDecoParentPane_Frame);
  GFL_SAFE_DELETE_ARRAY(m_pDecoParentPane_Stamps);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoView::Update(void)
{
  UpdateSystems();
  UIView::Update(); /*  super  */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoView::UpdateSystems(void)
{
  m_DecoItem_Frame.Update();
  for(u32 index = 0; index < stamps; ++index)
  {
    m_DecoItem_Stamps[index].Update();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(IsItemsLoaded())
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pRenderingManager, displayNo, 0);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  DecoView::IsDrawing(void) const
{
  return  (
    UIView::IsDrawing()
    ||  (!IsStable())
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  DecoView::RequestFrame(const App::FinderStudio::Static::StampData* pStampData)
{
  m_DecoItem_Frame.ReleaseRequest(false);
  m_DecoItem_Frame.Request(*pStampData, true);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoView::RequestStamp(const App::FinderStudio::Static::StampData* pStampData, const u32 dataNum)
{
  for(u32 index = 0; (index < dataNum) && (index < stamps); ++index)
  {
    m_DecoItem_Stamps[index].ReleaseRequest(false);
    m_DecoItem_Stamps[index].Request(pStampData[index], false);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoView::ReleaseRequest(const bool isFinalize)
{
  m_DecoItem_Frame.ReleaseRequest(isFinalize);
  for(u32 index = 0; index < stamps; ++index)
  {
    m_DecoItem_Stamps[index].ReleaseRequest(isFinalize);
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
bool  DecoView::IsStable(void) const
{
  bool isStable = m_DecoItem_Frame.IsStable();

  if(isStable)
  {
    for(u32 index = 0; index < stamps; ++index)
    {
      if(!m_DecoItem_Stamps[index].IsStable())
      {
        isStable  = false;
        break;
      }
    }
  }

  return isStable;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  DecoView::GetArcID(void)
{
  return  ARCID_DECOVIEW;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  DecoView::GetArcDatID(void)
{
  return  GARC_FinderStudioDecoView_FinderStudioDecoView_applyt_COMP;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool DecoView::IsResourceCompressed(void)
{
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
bool  DecoView::IsItemsLoaded(void) const
{
  bool  isLoaded  = m_DecoItem_Frame.IsEmptyOrLoaded();

  if(isLoaded)
  {
    for(u32 index = 0; index < stamps; ++index)
    {
      if(!m_DecoItem_Stamps[index].IsEmptyOrLoaded())
      {
        isLoaded  = false;
        break;
      }
    }
  }

  return isLoaded;
}



}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */

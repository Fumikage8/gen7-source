
//==============================================================================
/**
 * @file   PhotoViewViewContainer.cpp
 * @date   2017/02/07 Tue. 16:05:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/PhotoViewWork.h"

#include  "./PhotoViewViewContainer.h"


namespace NetApp  {
namespace PhotoView    {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewViewContainer
namespace FILELOCAL {
}

// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
PhotoViewViewContainer::PhotoViewViewContainer(PhotoViewWork* pWork)
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  m_pUpperView_Photo  = GFL_NEW(pHeapBase)  UpperView_Photo(pWork);
  m_pLowerView_Info   = GFL_NEW(pHeapBase)  LowerView_Info(pWork);
  m_pLowerView_BG     = GFL_NEW(pHeapBase)  LowerView_BG(pWork);

  SetScene(SCENE_Void);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
PhotoViewViewContainer::~PhotoViewViewContainer()
{
  GFL_SAFE_DELETE(m_pUpperView_Photo);
  GFL_SAFE_DELETE(m_pLowerView_Info);
  GFL_SAFE_DELETE(m_pLowerView_BG);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewViewContainer::SetAllInputEnable(const bool isEnable)
{
  m_pLowerView_Info->SetInputEnabled(isEnable);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  PhotoViewViewContainer::IsDrawingAny(void) const
{
  if(
    m_pUpperView_Photo->IsDrawing()
    || (!m_pUpperView_Photo->Finalize())
    || m_pLowerView_Info->IsDrawing()
    || m_pLowerView_BG->IsDrawing()
  )
  {
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();

  switch(sceneID)
  {
    case  SCENE_View:
      m_pViewArray[0] = m_pUpperView_Photo;

      m_pViewArray[1] = m_pLowerView_BG;
      m_pViewArray[2] = m_pLowerView_Info;
      break;

    case  SCENE_Void: /*  through  */
    default:
      /*  nop  */
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewViewContainer::Update(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->DrawTree(dispNo);
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
void  PhotoViewViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
  }
}



} /*  namespace PhotoView    */
} /*  namespace NetApp  */

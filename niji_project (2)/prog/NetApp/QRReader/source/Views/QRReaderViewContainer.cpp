
//==============================================================================
/**
 * @file   QRReaderViewContainer.cpp
 * @date   2016/01/14 Thu. 17:49:26
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/QRReaderWork.h"


#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"
#include  "../Views/QRReaderViewContainer.h"


namespace NetApp     {
namespace QRReader   {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
QRReaderViewContainer::QRReaderViewContainer(QRReaderWork* pWork)
  : m_pWork(pWork)
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  m_pUpperView_Main     = GFL_NEW(pHeapBase)  UpperView_Main(pWork);
  m_pUpperView_Pokemon  = GFL_NEW(pHeapBase)  UpperView_Pokemon(pWork);
  m_pLowerView_Main     = GFL_NEW(pHeapBase)  LowerView_Main(pWork);

  SetScene(SCENE_Void);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
QRReaderViewContainer::~QRReaderViewContainer()
{
  GFL_SAFE_DELETE(m_pLowerView_Main);
  GFL_SAFE_DELETE(m_pUpperView_Pokemon);
  GFL_SAFE_DELETE(m_pUpperView_Main);
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  QRReaderViewContainer::SetAllInputEnable(const bool isEnable)
{
  m_pLowerView_Main->SetInputEnabled(isEnable);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  QRReaderViewContainer::IsDrawingAny(void) const
{
  if(
    false
    ||  m_pUpperView_Main->IsDrawing()
    ||  m_pUpperView_Pokemon->IsDrawing()
    ||  m_pLowerView_Main->IsDrawing()
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
void  QRReaderViewContainer::SetScene(const SceneID sceneID)
{
  ClearViewArray();

  switch(sceneID)
  {
    case  SCENE_Tutorial:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pLowerView_Main;
      break;

    case  SCENE_QRRead:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pUpperView_Pokemon;
      m_pViewArray[2] = m_pLowerView_Main;
      break;


    case  SCENE_FieldScan:
      m_pViewArray[0] = m_pUpperView_Main;
      m_pViewArray[1] = m_pUpperView_Pokemon;
      m_pViewArray[2] = m_pLowerView_Main;
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
void  QRReaderViewContainer::Update(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }

  /*  silhouette color  */
  if(m_pUpperView_Main)
  {
    /*  layoutでsilhouetteColorを設定する  */
//    gfl2::gfx::Color  color;
//    
//    m_pUpperView_Main->GetSilhouetteColor(color);
//    m_pWork->m_pPokeModelUtil->SetShaderColor(1, color);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  QRReaderViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
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
void  QRReaderViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
  }
}

} /*  namespace QRReader  */
} /*  namespace NetApp    */

//==============================================================================
/**
 * @file   DecoComponent.cpp
 * @date   2017/02/15 Wed. 15:49:58
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./DecoComponent.h"

namespace App {
namespace FinderStudio {
namespace Viewer {
namespace Deco {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_DecoComponent
namespace FILELOCAL {
  static const u32  heapBytes_System  = 0x00080000;
  static const u32  heapBytes_Device  = 0x00180000;

  enum {
    STATE_Initial,
    STATE_Setup,
    STATE_Loading,
    STATE_Idle,
    STATE_Release,
    
    STATE_
  };

}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoComponent::DecoComponent(void)
  : m_UtilHeap()
  , m_pRenderingManager(NULL)
  , m_Loader()
  , m_pDecoView(NULL)
  , m_State(FILELOCAL::STATE_Initial)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoComponent::~DecoComponent()
{
  GFL_ASSERT(m_State == FILELOCAL::STATE_Initial);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoComponent::Setup(gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderingManager)
{
  const bool  isValidState  = (m_State == FILELOCAL::STATE_Initial);

  GFL_ASSERT(isValidState);
  if(!isValidState) return;

  m_pRenderingManager = pRenderingManager;
  m_UtilHeap.LocalHeapCreate(pHeapBase, pHeapBase, FILELOCAL::heapBytes_System, FILELOCAL::heapBytes_Device);
  m_State = FILELOCAL::STATE_Setup;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  DecoComponent::InitFunc(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
      /*  nop  */
      break;

    case FILELOCAL::STATE_Setup:
      m_Loader.RequestLoad(&m_UtilHeap, DecoView::GetArcID(), DecoView::GetArcDatID(), DecoView::IsResourceCompressed());
      m_State = FILELOCAL::STATE_Loading;
      break;

    case FILELOCAL::STATE_Loading:
    {
      if(m_Loader.ObserveLoad())
      {
        m_pDecoView = GFL_NEW(m_UtilHeap.GetDeviceHeap()) DecoView(&m_UtilHeap, m_pRenderingManager, m_Loader.GetData());
        m_State     = FILELOCAL::STATE_Idle;
      }
    }
      break;

    case FILELOCAL::STATE_Idle:
    case FILELOCAL::STATE_Release:
      /*  nop  */
      break;

    default:
      GFL_ASSERT(false);
      break;
  }

  return  (m_State == FILELOCAL::STATE_Idle);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  DecoComponent::EndFunc(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
      /*  nop  */
      break;

    case FILELOCAL::STATE_Setup:
    case FILELOCAL::STATE_Loading:
      GFL_ASSERT(false);
      break;

    case FILELOCAL::STATE_Idle:
      m_pDecoView->ReleaseRequest(true);
      m_State = FILELOCAL::STATE_Release;
      break;

    case FILELOCAL::STATE_Release:
      m_pDecoView->UpdateSystems();
      if(!(m_pDecoView->IsDrawing()))
      {
        GFL_SAFE_DELETE(m_pDecoView)
        m_State = FILELOCAL::STATE_Initial;
      }
      break;

    default:
      GFL_ASSERT(false);
      break;
  }

  return (m_State == FILELOCAL::STATE_Initial);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoComponent::Update(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Setup:
    case FILELOCAL::STATE_Loading:
      /*  nop  */
      break;

    case FILELOCAL::STATE_Idle:
      m_pDecoView->UpdateTree();
      break;

    case FILELOCAL::STATE_Release:
    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoComponent::Draw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Setup:
    case FILELOCAL::STATE_Loading:
      /*  nop  */
      break;

    case FILELOCAL::STATE_Idle:
      m_pDecoView->Draw(displayNo);
      break;

    case FILELOCAL::STATE_Release:
    default:
      break;
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
void  DecoComponent::Request(const App::FinderStudio::Static::StampData* pStampData, const u32 dataNum)
{
  if(!m_pDecoView)  return;
  if(!pStampData)   return;

  m_pDecoView->RequestFrame(&pStampData[0]);
  m_pDecoView->RequestStamp(pStampData, dataNum);
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*  @param
*  @return
*/
//------------------------------------------------------------------
bool  DecoComponent::IsLoaded(void) const
{
  return (m_pDecoView ? m_pDecoView->IsItemsLoaded() : false);
}



}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */

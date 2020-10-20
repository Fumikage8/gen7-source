//==============================================================================
/**
 * @file   DiveUI.cpp
 * @date   2016/11/29 Tue. 18:18:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <thread/include/gfl2_ThreadStatic.h>

#include  "./DiveUI.h"
#include  "./DiveUIResource.h"


//namespace App  {
namespace LunaSolDive  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  ※momijiで unnamed namespace が禁止となったためnamedにしている  */
#define FILELOCAL namespace_DiveUI
namespace FILELOCAL {
  enum  {
    STATE_Initial,
    STATE_Loading,
    STATE_Setup,
    STATE_Running,

    STATE_
  };

  /**   */
  static const app::tool::ResourceProvider::ArcTable  arcTable[]  =
  {
    {ARCID_DIVE_UI, GARC_DiveUI_DiveUI_applyt_COMP,     true},
//    {ARCID_DIVE_UI, GARC_DiveUI_lsd_bg_eff_bptcl_COMP,  true},
  };
  static const u32  arcTableElems = GFL_NELEMS(arcTable);
  
  
  static const struct
  {
    u32 SystemHeapBytes;
    u32 DeviceHeapBytes;
  }HeapInfo = {
    0x00080000,
    0x00200000,
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
u32  DiveUI::GetRequiredHeapBytes(void)
{
  return (FILELOCAL::HeapInfo.SystemHeapBytes + FILELOCAL::HeapInfo.DeviceHeapBytes);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DiveUI::DiveUI(
  gfl2::heap::HeapBase*           pHeapBase,
  app::util::AppRenderingManager* pRenderingManager
)
  : m_UtilHeap()
  , m_pRenderingManager(pRenderingManager)
  , m_pResourceProvider(NULL)
  , m_DefaultResourceProvider()
//  , m_EffectBG()
  , m_pLower_BG(NULL)
  , m_State(FILELOCAL::STATE_Initial)
{
  m_UtilHeap.LocalHeapCreate( pHeapBase, pHeapBase, FILELOCAL::HeapInfo.SystemHeapBytes, FILELOCAL::HeapInfo.DeviceHeapBytes);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DiveUI::~DiveUI()
{
//  m_EffectBG.Cleanup();

  GFL_SAFE_DELETE(m_pLower_BG);
  m_DefaultResourceProvider.ReleaseData();
  m_UtilHeap.Delete();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  DiveUI::ObserveSetup(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
    {
      if(!m_pResourceProvider)
      {
        m_DefaultResourceProvider.SetHeap(&m_UtilHeap);
        m_pResourceProvider = &m_DefaultResourceProvider;
        m_pResourceProvider->LoadRequest(FILELOCAL::arcTable, FILELOCAL::arcTableElems);
        m_State = FILELOCAL::STATE_Loading;
      }
      else
      {
        m_State = FILELOCAL::STATE_Setup;
      }
    }
      /*  fall through  */

    case  FILELOCAL::STATE_Loading:
    {
      if(m_pResourceProvider->ObserveLoad())
      {
        m_State = FILELOCAL::STATE_Setup;
      }
    }
      break;


    case FILELOCAL::STATE_Setup:
    {
      gfl2::heap::HeapBase* pHeapBase = m_UtilHeap.GetDeviceHeap();
      
      /*    */
/*
      m_EffectBG.SetResourceProvider(this);
      m_EffectBG.Setup(1);
      m_EffectBG.SetVisibility(false, true);
      m_EffectBG.SetLowerCamera(0,0,-1, 0,0,0);
*/
      /*    */
      m_pLower_BG     = GFL_NEW(pHeapBase) DiveUI_Lower_BG(&m_UtilHeap, m_pRenderingManager);
      m_pLower_BG->Setup(m_pResourceProvider);

      m_State = FILELOCAL::STATE_Running;
    }
      break;

    case FILELOCAL::STATE_Running:
    default:
      break;
  }

  return (m_State == FILELOCAL::STATE_Running);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DiveUI::Update(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Running:
    {
//      m_EffectBG.Update();
      if(m_pLower_BG)       m_pLower_BG->UpdateTree();
    }
      break;

    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Loading:
    case FILELOCAL::STATE_Setup:
    default:
      break;
  }


}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DiveUI::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Running:
    {
      if(m_pLower_BG)     m_pLower_BG->DrawTree(displayNo);
    }
      break;

    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Loading:
    case FILELOCAL::STATE_Setup:
    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  DiveUI::IsDrawing(void) const
{
  bool  isDrawing = false;

  isDrawing |= (m_pLower_BG ? m_pLower_BG->IsDrawing() : false);


  return isDrawing;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*
app::util::AppRenderingManager* DiveUI::GetRenderingManager(void)
{
  return  m_pRenderingManager;
}
*/

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*
app::util::Heap*  DiveUI::GetUtilHeap(void)
{
  return &m_UtilHeap;
}
*/

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
/*
void* DiveUI::GetResource(const u32 resourceIndex)
{
  return m_pResourceProvider->IResourceProvider_OnResourceRequest(ARCID_DIVE_UI, GARC_DiveUI_lsd_bg_eff_bptcl_COMP);
}
*/

} /*  namespace UI  */
} /*  namespace LunaSolDive  */
//} /*  namespace App  */

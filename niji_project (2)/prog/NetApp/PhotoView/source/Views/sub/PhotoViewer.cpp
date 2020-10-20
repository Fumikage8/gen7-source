//==============================================================================
/**
 * @file   PhotoViewer.cpp
 * @date   2017/02/07 Tue. 16:51:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./PhotoViewer.h"

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewer
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
PhotoViewer::PhotoViewer(gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderingManager)
  : m_pHeapBase(pHeapBase)
  , m_pRenderingManager(pRenderingManager)
  , m_State(STATE_Initial)
  , m_pViewer(NULL)
  , m_NextData()
  , m_IsRequested(false)
  , m_IsReleaseRequested(false)

{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
PhotoViewer::~PhotoViewer()
{
  GFL_ASSERT(m_State == STATE_Initial);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
PhotoViewer::State  PhotoViewer::Update(void)
{
  /*
    initial
      ↓
    initViewer
      ↓
    viewing         ←
      ↓             ↑
      ↓→reloading→↑
      ↓
    EndViewer
      ↓
    (initial)
  */
  switch(m_State)
  {
    case  STATE_Initial:
      GFL_ASSERT(!m_pViewer);
      if(m_IsRequested)
      {
        m_pViewer     = GFL_NEW(m_pHeapBase) App::FinderStudio::Viewer::FinderStudioViewer(m_pHeapBase, m_pRenderingManager, m_NextData.photo_param, m_NextData.deco_param);
        m_IsRequested = false;
        m_State       = STATE_InitViewer;
      }
      break;
    
    case  STATE_InitViewer:
      if(m_pViewer->InitFunc())
      {
        m_State = STATE_Viewing;
      }
      break;

    case  STATE_Reloading:
      m_pViewer->Update();
      if(m_pViewer->IsFinishPhotoReproduct())
      {
        m_State = STATE_Viewing;
      }
      break;

    case  STATE_Viewing:
      if(m_IsReleaseRequested)
      {
        m_IsReleaseRequested  = false;
        m_State               = STATE_EndViewer;
      }
      else if(m_IsRequested && m_pViewer->IsCanStartPhotoProduct())
      {
        m_pViewer->StartPhotoReproduct(m_NextData.photo_param, &(m_NextData.deco_param));
        m_IsRequested = false;
        m_State       = STATE_Reloading;
      }
      else
      {
        m_pViewer->Update();
      }
      break;

    case  STATE_EndViewer:
      if(m_pViewer->EndFunc())
      {
        GFL_SAFE_DELETE(m_pViewer);
        m_State = STATE_Initial;
      }
      break;

    default:
      GFL_ASSERT(false);
      break;
  }

  return m_State;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  PhotoViewer::Draw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_State == STATE_Viewing)
  {
    if(m_pViewer)
    {
      m_pViewer->Draw(displayNo);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewer::Request(const App::FinderStudio::Static::PhotoData& rData)
{
  m_NextData    = rData;
  m_IsRequested = true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  PhotoViewer::Release(void)
{
  m_IsRequested         = false;
  m_IsReleaseRequested  = true;
}




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */

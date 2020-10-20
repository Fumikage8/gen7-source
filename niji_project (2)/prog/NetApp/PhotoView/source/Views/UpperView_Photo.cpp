//==============================================================================
/**
 * @file   UpperView_Photo.cpp
 * @date   2017/02/07 Tue. 16:32:55
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
#include  "../PhotoViewResource.h"
#include  "./UpperView_Photo.h"


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_UpperView_Photo
namespace FILELOCAL {

  enum 
  {
    STATE_Initial,
    STATE_Idle,
    /*  viewer  */
    STATE_InitViewer,
    STATE_Viewing,
    STATE_EndViewer,
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
UpperView_Photo::UpperView_Photo(PhotoViewWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_Viewer(pWork->GetDeviceHeap(), pWork->GetAppRenderingManager())
  , m_State()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Photo::~UpperView_Photo()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Photo::Update(void)
{
  m_Viewer.Update();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Photo::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  m_Viewer.Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Photo::Request(const App::FinderStudio::Static::PhotoData& rData)
{
  m_Viewer.Request(rData);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  UpperView_Photo::Finalize(void)
{
  bool  isEnd = false;

  m_Viewer.Release();
  isEnd = PhotoViewer::IsStableState(m_Viewer.Update());

  return isEnd;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  UpperView_Photo::IsLoaded(void) const
{
  return m_Viewer.IsViewing();
}


}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */

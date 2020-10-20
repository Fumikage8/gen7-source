#if !defined(PHOTOVIEWER_H_INCLUDED)
#define PHOTOVIEWER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewer.h
 * @date   2017/02/07 Tue. 16:45:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================

#include  <App/FinderStudioViewer/include/FinderStudioViewer.h>

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/02/07 Tue. 16:46:06
  */
//==============================================================================
class PhotoViewer
{
  GFL_FORBID_COPY_AND_ASSIGN(PhotoViewer);
public:
  PhotoViewer(gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderingManager);
  virtual ~PhotoViewer();

  enum State
  {
    STATE_Initial,
    STATE_InitViewer,
    STATE_Reloading,
    STATE_Viewing,
    STATE_EndViewer,

    STATE_
  };

  State Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo displayNo);

  void  Request(const App::FinderStudio::Static::PhotoData& rData);
  void  Release(void);

  bool  IsViewing(void) const  {return (m_State == STATE_Viewing);}
  

  static bool IsStableState(const State state) {return (state == STATE_Initial);}

protected:
  gfl2::heap::HeapBase*                           m_pHeapBase;
  app::util::AppRenderingManager*                 m_pRenderingManager;
  State                                           m_State;
  App::FinderStudio::Viewer::FinderStudioViewer*  m_pViewer;
  App::FinderStudio::Static::PhotoData            m_NextData;
  bool                                            m_IsRequested;
  bool                                            m_IsReleaseRequested;
};



}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWER_H_INCLUDED)  */

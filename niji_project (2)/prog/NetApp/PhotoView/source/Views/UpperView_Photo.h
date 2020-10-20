#if !defined(UPPERVIEW_PHOTO_H_INCLUDED)
#define UPPERVIEW_PHOTO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Photo.h
 * @date   2017/02/07 Tue. 16:27:36
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/UI/NetAppEmptyView.h>

#include  <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include  "./sub/PhotoViewer.h"

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
class PhotoViewWork;

//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/07 Tue. 15:08:49
  */
//==============================================================================
class UpperView_Photo
  : public    NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperView_Photo);
public:
  UpperView_Photo(PhotoViewWork* pWork);
  virtual ~UpperView_Photo();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */

  void  Request(const App::FinderStudio::Static::PhotoData& rData);
  bool  IsLoaded(void) const;
  bool  Finalize(void);

protected:
  PhotoViewWork*  m_pWork;
  PhotoViewer     m_Viewer;

  u32   m_State;
};




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_PHOTO_H_INCLUDED)  */

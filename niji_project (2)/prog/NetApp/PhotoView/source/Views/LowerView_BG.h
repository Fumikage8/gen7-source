#if !defined(LOWERVIEW_BG_H_INCLUDED)
#define LOWERVIEW_BG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_BG.h
 * @date   2017/02/13 Mon. 11:20:23
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

namespace NetApp {
namespace PhotoView {
  class PhotoViewWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/02/13 Mon. 11:20:27
  */
//==============================================================================
class LowerView_BG
  : public  NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerView_BG);
public:
  LowerView_BG(PhotoViewWork* pWork);
  virtual  ~LowerView_BG();


  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
   /*  endimplement UIView I/F  */
protected:
  PhotoViewWork*  m_pWork;
};


}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_BG_H_INCLUDED)  */

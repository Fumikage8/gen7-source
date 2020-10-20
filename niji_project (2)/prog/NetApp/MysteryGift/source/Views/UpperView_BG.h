#if !defined(UPPERVIEW_BG_H_INCLUDED)
#define UPPERVIEW_BG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_BG.h
 * @date   2016/03/09 Wed. 18:59:48
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"




namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
class UpperView_BG
  : public  NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_BG );
public:
  UpperView_BG(GiftWork* pWork);
  virtual ~UpperView_BG();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
  /*  endimplement UIView I/F  */


protected:
  /*
    members
  */
  GiftWork* m_pWork;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_BG_H_INCLUDED)  */
